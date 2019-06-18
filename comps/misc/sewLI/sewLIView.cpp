#include "stdafx.h"
#include "sewLIView.h"
#include "sewLI.h"
#include "nameconsts.h"
#include "core5.h"
#include "measure5.h"
#include "thumbnail.h"
#include "class_utils.h"
#include "resource.h"
#include "MenuRegistrator.h"
#include "object5.h"
#include "zmouse.h"
#include "alloc.h"
#include "class_utils.h"
#include "action5.h"
#include "math.h"
#include "FindPos.h"
#include "ScriptNotifyInt.h"
#include "PropPage.h"
#include "\vt5\awin\trace.h"

CIntIniValue g_CheckGetImage("\\Sew Large Image", "CheckGetImage", 1);


#include "vfw.h"
#pragma comment(lib, "vfw32.lib")

CLVFrameBuffer::CLVFrameBuffer()
{
	m_State = NotInit;
	InitializeCriticalSection(&m_cs);
	m_lpbiLVCur = NULL;
	m_dwPrvDataSize = 0;
	m_ptShift = CPoint(0,0);
	m_bCalculating = false;
}

CLVFrameBuffer::~CLVFrameBuffer()
{
	DeleteCriticalSection(&m_cs);
	delete m_lpbiLVCur;
}

void CLVFrameBuffer::Free()
{
	delete m_lpbiLVCur;
	m_lpbiLVCur = NULL;
	m_dwPrvDataSize = 0;
}

void *CLVFrameBuffer::AllocMem(DWORD dwSize)
{
	if (m_lpbiLVCur == NULL || m_dwPrvDataSize != dwSize)
	{
		delete m_lpbiLVCur;
		m_lpbiLVCur = (LPBITMAPINFOHEADER)new char[dwSize];
		m_dwPrvDataSize = dwSize;
	}
	return m_lpbiLVCur;
}


static bool ValidBitmap(LPBITMAPINFOHEADER lpbi, DWORD dwSize)
{
	if (dwSize > sizeof(BITMAPINFOHEADER))
	{
		if (lpbi->biPlanes == 1 && lpbi->biSize == sizeof(BITMAPINFOHEADER))
		{
			int nColors = lpbi->biClrUsed==0?lpbi->biBitCount==8?256:0:lpbi->biClrUsed;
			DWORD dwRow = ((lpbi->biBitCount/8)*lpbi->biWidth+3)/4*4;
			DWORD dwCalcSize = sizeof(BITMAPINFOHEADER)+nColors*sizeof(RGBQUAD)+dwRow*lpbi->biHeight;
			if (dwSize == dwCalcSize)
				return true;
		}
	}
	return false;
}

void CLVFrameBuffer::CopyDataDir(IDriver3 *pDriver, int nDevice)
{
	DWORD dwDataSize = 0;
	LPVOID lpData = NULL;
	pDriver->GetImage(nDevice, &lpData, &dwDataSize);
	if (m_bCalculating) return;
	EnterCriticalSection(&m_cs);
	if (m_State == NotInit)
	{
		if (ValidBitmap((LPBITMAPINFOHEADER)lpData, dwDataSize))
		{
			m_lpbiLVCur = (LPBITMAPINFOHEADER)new char[dwDataSize];
			memcpy(m_lpbiLVCur, lpData, dwDataSize);
			m_dwPrvDataSize = dwDataSize;
			m_State = OneFrame;
		}
	}
	else if (m_State == OneFrame)
	{
		if (m_dwPrvDataSize == dwDataSize)
		{
			memcpy(m_lpbiLVCur, lpData, dwDataSize);
		}
	}
	LeaveCriticalSection(&m_cs);
}

void CLVFrameBuffer::CopyDataDIB(IDriver3 *pDriver, int nDevice)
{
	DWORD dwDataSize = 0;
	LPVOID lpData = NULL;
	if (m_bCalculating) return;
	EnterCriticalSection(&m_cs);
	pDriver->InputDIB(this, NULL, nDevice, false);
	if (m_dwPrvDataSize != 0)
		m_State = OneFrame;
	LeaveCriticalSection(&m_cs);
}

CPoint CLVFrameBuffer::CalculateShiftLastFrag(ISewImageList *pFragList,
	ISewFragment *pFragPrev, CPoint ptOrig, SIZE szLV, int nMode, double dZoom)
{
	CPoint ptShift(0,0);
	CPoint ptPrevPos;
	pFragPrev->GetOffset(&ptPrevPos);
	EnterCriticalSection(&m_cs);
	m_bCalculating = true;
	if (m_State == OneFrame)
	{
		CPoint ptPos(ptOrig);
		/*if (nMode == 1)
		{
			if (FindOptimalPositionByPointLive(pFragList,pFragPrev,m_lpbiLVCur,szLV,&ptPos))
				ptShift = ptPos-ptPrevPos;
		}
		else */if (nMode == 2)
		{
			if (FindOptimalPositionCorrectLive(pFragList,pFragPrev,m_lpbiLVCur,szLV,dZoom,&ptPos))
				ptShift = ptPos-ptPrevPos;
		}
		else //if (nMode == 0)
		{
			if (FindOptimalPositionLive(pFragList,pFragPrev,m_lpbiLVCur,szLV,&ptPos))
				ptShift = ptPos-ptPrevPos;
		}
	}
	m_bCalculating = false;
	LeaveCriticalSection(&m_cs);
	return ptShift;
}

int CLVFrameBuffer::CalcDifference(ISewImageList *pFragList, ISewFragment *pFragPrev,
	CPoint ptOffs, SIZE szLV)
{
	int nDiff = -1;
	EnterCriticalSection(&m_cs);
	m_bCalculating = true;
	if (m_State == OneFrame)
		nDiff = CalcDifferenceLive(pFragList,pFragPrev,m_lpbiLVCur,szLV,ptOffs);
	m_bCalculating = false;
	LeaveCriticalSection(&m_cs);
	return nDiff;
}

typedef void (*COPYPIXEL)(LPBYTE lpDst, LPBYTE lpSrc, WORD nDistCount);

static void CopyDIB2DIB(LPBITMAPINFOHEADER lpbiDest, LPBYTE lpDest,
	LPBITMAPINFOHEADER lpbiSrc, LPBYTE lpSrc, _ptr_t2<WORD> &GlobalDistMap,
	CRect rcSrc, CRect rcDst, COPYPIXEL cpf)
{
	double fZoomX = double(rcDst.Width())/double(rcSrc.Width());
	double fZoomY = double(rcDst.Height())/double(rcSrc.Height());
	DWORD dwRowSrc = (lpbiSrc->biWidth*lpbiSrc->biBitCount/8+3)/4*4;
	DWORD dwRowDst = (lpbiDest->biWidth*lpbiDest->biBitCount/8+3)/4*4;
	int nDstHeight = rcDst.Height(), nDstWidth = rcDst.Width();
	int nSrcHeight = rcSrc.Height(), nSrcWidth = rcSrc.Width();
	int nSrcIncr = lpbiSrc->biBitCount/8;
	_ptr_t2<int> xOffs(nDstWidth);
	for (int x = 0; x < nDstWidth; x++)
	{
		int x1 = (int)(double(x)/fZoomX);
		xOffs[x] = min(x1,nSrcWidth-1);
	}
	for (int y = 0; y < nDstHeight; y++)
	{
		int yDst = rcDst.top+y;
		int y1 = (int)(double(y)/fZoomY);
		int ySrc = rcSrc.top+min(y1,nSrcHeight-1);
		LPBYTE lpDstRow = lpDest+(lpbiDest->biHeight-1-yDst)*dwRowDst+3*rcDst.left;
		LPBYTE lpSrcRow = lpSrc+(lpbiSrc->biHeight-1-ySrc)*dwRowSrc+nSrcIncr*rcSrc.left;
		WORD *pwDMap = GlobalDistMap.ptr+(lpbiDest->biHeight-1-yDst)*lpbiDest->biWidth;
		for (int x = 0; x < nDstWidth; x++)
		{
			cpf(lpDstRow+3*x,lpSrcRow+nSrcIncr*xOffs[x],pwDMap[rcDst.left+x]);
		}

	}
}

static void CopyPixel24Transp(LPBYTE lpDst, LPBYTE lpSrc, WORD nDistCount)
{
	if (nDistCount > 0)
	{
		lpDst[0] = lpDst[0]/2+lpSrc[0]/2;
		lpDst[1] = lpDst[1]/2+lpSrc[1]/2;
		lpDst[2] = lpDst[2]/2+lpSrc[2]/2;
	}
	else
	{
		lpDst[0] = lpSrc[0];
		lpDst[1] = lpSrc[1];
		lpDst[2] = lpSrc[2];
	}
}

static void CopyPixel24Neg(LPBYTE lpDst, LPBYTE lpSrc, WORD nDistCount)
{
	if (nDistCount > 0)
	{
		lpDst[0] = lpDst[0]>lpSrc[0]?lpDst[0]-lpSrc[0]:lpSrc[0]-lpDst[0];
		lpDst[1] = lpDst[1]>lpSrc[1]?lpDst[1]-lpSrc[1]:lpSrc[1]-lpDst[1];
		lpDst[2] = lpDst[2]>lpSrc[2]?lpDst[2]-lpSrc[2]:lpSrc[2]-lpDst[2];
	}
	else
	{
		lpDst[0] = lpSrc[0];
		lpDst[1] = lpSrc[1];
		lpDst[2] = lpSrc[2];
	}
}

static void CopyPixel8Transp(LPBYTE lpDst, LPBYTE lpSrc, WORD nDistCount)
{
	if (nDistCount > 0)
	{
		lpDst[0] = lpDst[0]/2+lpSrc[0]/2;
		lpDst[1] = lpDst[1]/2+lpSrc[0]/2;
		lpDst[2] = lpDst[2]/2+lpSrc[0]/2;
	}
	else
	{
		lpDst[0] = lpSrc[0];
		lpDst[1] = lpSrc[0];
		lpDst[2] = lpSrc[0];
	}
}

static void CopyPixel8Neg(LPBYTE lpDst, LPBYTE lpSrc, WORD nDistCount)
{
	if (nDistCount > 0)
	{
		lpDst[0] = lpDst[0]>lpSrc[0]?lpDst[0]-lpSrc[0]:lpSrc[0]-lpDst[0];
		lpDst[1] = lpDst[1]>lpSrc[0]?lpDst[1]-lpSrc[0]:lpSrc[0]-lpDst[1];
		lpDst[2] = lpDst[2]>lpSrc[0]?lpDst[2]-lpSrc[0]:lpSrc[0]-lpDst[2];
	}
	else
	{
		lpDst[0] = lpSrc[0];
		lpDst[1] = lpSrc[0];
		lpDst[2] = lpSrc[0];
	}
}

void CLVFrameBuffer::PaintToDIB(LPBITMAPINFOHEADER lpbiDest, LPBYTE lpDest,
	_ptr_t2<WORD> &GlobalDistMap, double xSrc1, double ySrc1, double xSrc2,
		double ySrc2, RECT rcDst, double fZoom,	DrawPreviewMode dpm)
{
	EnterCriticalSection(&m_cs);
	m_bCalculating = true;
	if (m_State == OneFrame)
	{
		COPYPIXEL cpf = NULL;
		if (m_lpbiLVCur->biBitCount == 24)
		{
			if (dpm == SubtractPreview)
				cpf = CopyPixel24Neg;
			else
				cpf = CopyPixel24Transp;
		}
		else if (m_lpbiLVCur->biBitCount == 8)
		{
			if (dpm == SubtractPreview)
				cpf = CopyPixel8Neg;
			else
				cpf = CopyPixel8Transp;
		}
		int nSrcColor = m_lpbiLVCur->biClrUsed?m_lpbiLVCur->biBitCount==8?
			256:0:m_lpbiLVCur->biClrUsed;
		LPBYTE lpSrcData = (LPBYTE)(((RGBQUAD*)(m_lpbiLVCur+1))+nSrcColor);

		CRect rcSrc((int)(xSrc1*m_lpbiLVCur->biWidth),(int)(ySrc1*m_lpbiLVCur->biHeight),
			(int)(xSrc2*m_lpbiLVCur->biWidth),(int)(ySrc2*m_lpbiLVCur->biHeight));
		if (cpf != NULL)
			CopyDIB2DIB(lpbiDest,lpDest,m_lpbiLVCur,lpSrcData,GlobalDistMap,
				rcSrc,rcDst,cpf);
	}
	m_bCalculating = false;
	LeaveCriticalSection(&m_cs);
}


CSewLIView::CSewLIView()
{
	m_bstrName		= "SewLIView";

	char sz_buf[256];	sz_buf[0] = 0;	
	::LoadString( App::handle(), IDS_SEWLI_VIEW_NAME, sz_buf, sizeof(sz_buf) );	

	m_bstrUserName	= sz_buf;
	m_rcBoundsPrev = _rect(0,0,0,0);
	m_rcImagePrev = _rect(0,0,0,0);

	if( !m_bstrUserName.length() )
		m_bstrUserName = m_bstrName;

	m_pi_type_lib	= 0;
	m_pi_type_info	= 0;
	m_nTimerId = 0;	
	m_szCoordSpaceExt.cx = m_szCoordSpaceExt.cy = 0;
	m_rcPreviewRect = CRect(0,0,0,0);
	m_nInputDevice = 0;
	m_nTimerPrv = 0;
	m_bAlwaysRedrawPrv = false;
	m_bInvalidatePrv = false;
	m_bLiveVideoMoving = FALSE;
	m_dwLastAutoMove = 0;
	m_dwAutoMoveTime = 0;
	m_ptLVOffs = CPoint(0,0);
	m_AutoMoveLVMode = AutoMoveLV;
	m_bCalcOverlayCoef = true;
	m_nPrvOvrPerc = 0;
	m_bOvrPercNtyCalled = false;
	m_bAlternateFrame = false;
	m_DrawPreviewMode = OpacquePreview;
	m_nPrevDiffState = -1;
	m_dwDrvFlag = 0;
}

CSewLIView::~CSewLIView()
{
}

IUnknown* CSewLIView::DoGetInterface( const IID &iid )
{
	if( iid == IID_IPersist ) 
		return (IPersist*)this;
	else if( iid == IID_INamedObject2 ) 
		return (INamedObject2*)this;	
//	else if( iid == IID_IMultiSelection ) 
//		return (IMultiSelection*)this;	
//	else if( iid == IID_IPrintView ) 
//		return (IPrintView*)this;	
//	else if( iid == IID_IMenuInitializer ) 
//		return (IMenuInitializer*)this;
	else if( iid == IID_IViewDispatch ) 
		return (IViewDispatch*)this;
	else if( iid == IID_IDispatch ) 
		return (IDispatch*)this;
	else if( iid == IID_ISewLIView ) 
		return (ISewLIView*)this;
	else if (iid == IID_INamedPropBag || iid == IID_INamedPropBagSer) 
		return (INamedPropBagSer*)this;
	else if (iid == IID_IHelpInfo) 
		return (IHelpInfo*)this;
	else if (iid == IID_INotifyPlace)
		return (INotifyPlace *)this;
	else if (iid == IID_IDriverSite)
		return (IDriverSite *)this;
	else return CWinViewBase::DoGetInterface( iid );
}

HRESULT CSewLIView::GetDispIID( GUID* pguid )
{
	if( !pguid ) return E_INVALIDARG;

	*pguid = IID_ISewLIView;
	return S_OK;
};


LRESULT	CSewLIView::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT	lResult = 0;
	switch( nMsg )
	{
		case WM_PAINT:			lResult = OnPaint();	
			break;
		case WM_ERASEBKGND:		lResult = OnEraseBackground( (HDC)wParam );
			break; 
		case WM_SIZE:			lResult = OnSize( (int)LOWORD(lParam), (int)HIWORD(lParam) );
			break;
		case WM_CONTEXTMENU:	lResult = OnContextMenu( HWND(wParam), (int)GET_X_LPARAM(lParam), (int)GET_Y_LPARAM(lParam) );
			break;
		case WM_CREATE:			lResult = OnCreate();
			break;
		case WM_DESTROY:		lResult = OnDestroy();
			break;
		case WM_CHAR:			lResult = OnChar(wParam, lParam);
			break;
		//case WM_LBUTTONDOWN:	lResult = OnLButtonClick((int)LOWORD(lParam), (int)HIWORD(lParam));		
			//break;
		case WM_MOUSEWHEEL:			lResult = OnMouseWheel(
									LOWORD(wParam), HIWORD(wParam),
									_point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))
									);
			break;
		case WM_SETCURSOR:
			{
				MSG msg={0};
				msg.message=nMsg;
				msg.wParam=wParam;
				msg.lParam=lParam;
				BOOL bReturn=FALSE;
				PreTranslateMsg( &msg, &bReturn );
				if(bReturn) lResult = TRUE;
			}
			break;
		case WM_TIMER: lResult = OnTimer(wParam);
			break;
	}
	if( lResult )return lResult;

	/*if(m_ptrSite!=0)
	{
		lResult = DoMouseMessage( nMsg, wParam, lParam );
		if( lResult )return lResult;
	}*/

	/*switch( nMsg )
	{
	case WM_CREATE:			DoCreate( (CREATESTRUCT*)lParam );
	case WM_PAINT:			DoPaint();return 1;
	case WM_COMMAND:		DoCommand( wParam );return 1;
	case WM_NOTIFY:			DoNotify( wParam, (NMHDR*)lParam );
	case WM_GETINTERFACE:	return (LRESULT)this;
	}

	LONG lRes = 0;
	if( m_pDefHandler )lRes = CallWindowProc( m_pDefHandler, m_hwnd, nMsg, wParam, lParam );
	if( lRes != 0 )return lRes;
	if( nMsg == WM_NCCREATE )return  1;
	if( nMsg == WM_NCDESTROY )
	{
		m_hwnd = 0;
		m_pDefHandler = 0;
	}

	return 0;*/

	return CWinViewBase::DoMessage( nMsg, wParam, lParam );
}

void CSewLIView::DoAttachObjects()
{
	IUnknown* punkObjectList = ::GetActiveObjectFromContext(Unknown(), szTypeSewLI);

	if( punkObjectList == 0 )
	{
		SIZE size;
		size.cx = size.cy = 0;
		m_szCoordSpaceExt = CSize(0,0);
		m_ptCoordSpaceOrg = CPoint(0,0);
		SetClientSize(size); 
		m_sptrObjectList = 0;
		m_LoadFragList.deinit();
		InvalidateRect(m_hwnd,NULL,TRUE);
	}

	if( ::GetKey(m_sptrObjectList) != ::GetKey(punkObjectList) )
	{
		m_sptrObjectList = punkObjectList;
		ISewImageListPtr sptrSZ(punkObjectList);
		if(sptrSZ)
		{
			sptrSZ->OnActivateObject();
//			sptrSZ->GetOrigin(&m_ptCoordSpaceOrg);
//			sptrSZ->GetSize(&m_szCoordSpaceExt);
			_UpdateScrollSizes();
			MakeLoadFragList(true);
		}
		InvalidateRect(m_hwnd,NULL,TRUE);
	}	
	if(punkObjectList) 
		punkObjectList->Release();
//	DrawImages();

}

static bool IsRealDoc(INamedDataObject2 *pSLI)
{
	IUnknownPtr punk;
	pSLI->GetData(&punk);
	return punk != 0 &&	CheckInterface(punk, IID_IDocumentSite2);
}

LRESULT CSewLIView::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hwnd, &ps);
	if (m_szCoordSpaceExt.cx == 0 || m_szCoordSpaceExt.cy == 0)
	{
		COLORREF	cr = ::GetValueColor( GetAppUnknown(), "\\Colors", "Background", RGB( 128, 128, 255 ) );
		HBRUSH hbrBkgnd = CreateSolidBrush(cr); 
		::FillRect(hdc, &ps.rcPaint, hbrBkgnd );
		::DeleteObject(hbrBkgnd);
	}
	else
	{
//		MakeLoadFragList(false);
		DrawImages(hdc, ps.rcPaint);
	}
	EndPaint(m_hwnd, &ps);
	return true;
}

LRESULT CSewLIView::OnEraseBackground( HDC hDC )
{
	if( !::IsWindow( m_hwnd ) )
		return 0;
	return 1;
}

LRESULT CSewLIView::OnTimer( UINT nTimer)
{
	if (nTimer == m_nTimerId)
	{
		if (m_sptrIP == 0)
		{
			HDC hdcClient = ::GetDC(m_hwnd);
			int nROP = SetROP2(hdcClient, R2_XORPEN);
			DrawActiveFragment(hdcClient, Xor);
			SetROP2(hdcClient, nROP);
			::ReleaseDC(m_hwnd, hdcClient);
		}
		else
			m_bAlternateFrame = !m_bAlternateFrame;
		if (MakeLoadFragList(false))
			InvalidateRect(m_hwnd, NULL, TRUE);
	}
	else if (nTimer == m_nTimerPrv)
	{
		if (m_sptrIP != 0)
		{
			if (m_bAlwaysRedrawPrv)
			{
				if (g_CheckGetImage == 0 || (m_dwDrvFlag&FG_GETIMAGE_DIB))
					m_2FramesBuffer.CopyDataDir(m_sptrDrv3,m_nInputDevice);
				else
					m_2FramesBuffer.CopyDataDIB(m_sptrDrv3,m_nInputDevice);
			}
			if (m_bAlwaysRedrawPrv || m_bInvalidatePrv)
			{
				bool bRedraw = true;
				if (!m_bLiveVideoMoving && m_AutoMoveLVMode == AutoMoveLV)
					bRedraw = !AutoMovePreview(false);
				if (bRedraw)
				{
					if (m_DrawPreviewMode == OpacquePreview)
					{
						HDC hdcClient = ::GetDC(m_hwnd);
						DrawPreview(hdcClient, FALSE);
						DrawActiveFragment(hdcClient, m_bAlternateFrame?Alternate:FirstDraw);
						DrawIntersection(hdcClient);
						::ReleaseDC(m_hwnd, hdcClient);
					}
					else
					{
						CRect rcInv(m_rcPreviewRect);
						rcInv.InflateRect(1,1,1,1);
						CRect rc = ConvertToClient(rcInv);
						InvalidateRect(m_hwnd, &rc, TRUE);
					}
				}
				m_bInvalidatePrv = false;
			}
		}
	}
	return 0;
}


LRESULT CSewLIView::OnSize( int cx, int cy )
{
	if( !::IsWindow( m_hwnd ) )
		return 0;

//	DrawImages();

	return 0;
}

//////////////////////////////////////////////////////////////////////
LRESULT CSewLIView::OnContextMenu( HWND hWnd, int x, int y )
{
	return 0;
}

LRESULT CSewLIView::OnCreate()
{
	InitMouse(IUnknownPtr(this));

	{
		DWORD dwFlags = 0;
		GetWindowFlags( &dwFlags );
		SetWindowFlags( dwFlags | wfZoomSupport );
	}

	//load type lib && type info
	m_pi_type_lib	= 0;
	m_pi_type_info	= 0;
	{
		char sz_path[MAX_PATH];	sz_path[0] = 0;
		::GetModuleFileName( 0, sz_path, sizeof(sz_path) );
		char* psz_last = strrchr( sz_path, '\\' );
		if( psz_last )
		{
			psz_last++;
			strcpy( psz_last, "comps\\sewLI.tlb" );
			HRESULT hr = ::LoadTypeLib( _bstr_t( sz_path ), &m_pi_type_lib );
			dbg_assert( S_OK == hr );

			if( m_pi_type_lib )
			{
				//временно закомментируем
				hr = m_pi_type_lib->GetTypeInfoOfGuid( IID_ISewLIView, &m_pi_type_info );
				dbg_assert( S_OK == hr );
			}
		}		
	}
	m_nTimerId = SetTimer(m_hwnd, 1, 500, NULL);
//	InitPreview();

	return 0;
}

LRESULT CSewLIView::OnMouseWheel(UINT nFlags, short zDelta, _point pt)
{
	return 0;
}


LRESULT CSewLIView::OnDestroy()
{
	DeinitPreview();
	if (m_nTimerId > 0) KillTimer(m_hwnd, m_nTimerId);
	DeInitMouse();
	return 0;
}

#if defined(_DEBUG)
extern int g_nDebugWrite;
#endif


LRESULT CSewLIView::OnChar( UINT nChar, UINT wParam )
{
#if defined(_DEBUG)
	if (nChar == 'D' || nChar == 'd')
	{
		g_nDebugWrite = 1;
	}
#endif
	return 0;
}


bool CSewLIView::DoLButtonDown( _point  point )
{

//	CPoint ptScroll(0, 0);
//	IScrollZoomSitePtr sptrSZ(Unknown());
//	if(sptrSZ != 0) 
//	{
//		sptrSZ->GetScrollPos(&ptScroll);
//	}
//	if(m_pointScroll!=ptScroll)
//		DrawImages();

	return 0;
}

bool CSewLIView::DoLButtonDblClick( _point  point )
{
	return __super::DoLButtonDblClick(point);
}

bool CSewLIView::DoRButtonDown( _point  point )
{
	return __super::DoRButtonDown(point);
}

bool CSewLIView::DoChar( int nChar, bool bKeyDown )
{
	/*if(nChar==VK_PAGE_DOWN || nChar==VK_PAGE_UP)
	{
		IScrollZoomSitePtr sptrSZ(Unknown());
		if(sptrSZ != 0)
		{
			_point ptScroll(0,0);
			sptrSZ->GetScrollPos(&ptScroll);
			_rect rc(0,0,0,0);
			sptrSZ->GetVisibleRect(&rc);
			if(nChar==VK_PAGE_DOWN) ptScroll.y += rc.height();
			else if(nChar==VK_PAGE_UP) ptScroll.y -= rc.height();
			sptrSZ->SetScrollPos(ptScroll);
			return true;
		}
	}*/
	return __super::DoChar(nChar, bKeyDown );
}


void CSewLIView::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp( pszEvent, szEventChangeObject ) )
	{
		ISewImageListPtr sptrSLI(m_sptrObjectList);
		if (sptrSLI != 0 && pdata !=  NULL)
		{
			if ((*(int*)pdata == silevAddImage ||
				*(int*)pdata == silevDeleteImage) && m_sptrIP != 0)
			{
				_UpdatePreviewSize();
				_UpdateBaseFragAndOffs();
				m_bOvrPercNtyCalled = false;
			}
			if (*(int*)pdata == silevAddImage ||
				*(int*)pdata == silevMoveImage || *(int*)pdata == silevDeleteImage)
			{
				_UpdateScrollSizes();
				MakeLoadFragList(true);
			}
			if (*(int*)pdata != silevAddImage)
				InvalidateRect(m_hwnd, NULL, TRUE);
		}
	}
}

//////////////////////////////////////////////////////////////////////
LRESULT CSewLIView::OnActivateView( BOOL bActivate, IUnknown *punkOtherView )
{
	_ShowPropPage( bActivate == TRUE );
	return S_OK;	
}


////////////////////////////////////////////////////////////////////////
//
HRESULT CSewLIView::GetClassID( CLSID *pClassID )
{
	//memcpy(pClassID, &clsidAView, sizeof(CLSID));
	return S_OK;
}

HRESULT CSewLIView::GetFirstVisibleObjectPosition( long *plpos )
{
	if(plpos)
	{
		*plpos = 1;
		return S_OK;
	}
	else
		return E_INVALIDARG;
}

HRESULT CSewLIView::GetNextVisibleObject( IUnknown ** ppunkObject, long *plPos )
{
	if(plPos && ppunkObject)
	{
		if(*plPos > 0)
			*ppunkObject = m_sptrObjectList;
		
		(*plPos)--;
		
		if(*ppunkObject) (*ppunkObject)->AddRef();

		return S_OK;
	}
	else
		return E_INVALIDARG;
}

HRESULT CSewLIView::GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch )
{
	CWinViewBase::GetObjectFlags( bstrObjectType, pdwMatch );
	_bstr_t	bstr( bstrObjectType );
	if( !strcmp(bstr, szTypeSewLI))
		*pdwMatch = mvFull;

	return S_OK;
}

void CSewLIView::SetClientSize(SIZE size)
{
	if(!::IsWindow(m_hwnd))
		return;

	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0 && size.cx >= 0 && size.cy >= 0) 
	{
		SIZE sizeOld;
		sptrSZ->GetClientSize(&sizeOld);
		if(sizeOld.cx != size.cx || sizeOld.cy != size.cy)
			sptrSZ->SetClientSize(size);
	}

}

void CSewLIView::GetClientSize(SIZE& size)
{
	if(!::IsWindow(m_hwnd))
		return;

	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
		sptrSZ->GetClientSize(&size);
}

void CSewLIView::_ShowPropPage( bool bShow )
{
	IPropertySheetPtr ptrSheet;
	IOptionsPagePtr ptrPage;
	int nPage = -1;
	ptrSheet = IPropertySheetPtr(::FindPropertySheet(), false);
	if (ptrSheet == 0)
		return;
	CLSID clsidNavigator;
	::CLSIDFromProgID(_bstr_t(szZoomNavigatorPropPageProgID), &clsidNavigator);
	ptrPage = IOptionsPagePtr(::FindPage(ptrSheet, clsidNavigator, &nPage), false);
	if (nPage == -1)
		return;
	if (bShow)
		ptrSheet->IncludePage(nPage);
	else
		ptrSheet->ExcludePage(nPage);
}


////////////////////////////////////////////////////////////////////////
// IDriverSite
////////////////////////////////////////////////////////////////////////

//static DWORD s_dwInvalidate = 0;

HRESULT CSewLIView::Invalidate()
{
/*	DWORD dwStart = GetTickCount();
	if (s_dwInvalidate == 0)
		s_dwInvalidate = dwStart;*/
//	if (m_LoadFragList.head())
	if (g_CheckGetImage == 0 || (m_dwDrvFlag&FG_GETIMAGE_DIB))
		m_2FramesBuffer.CopyDataDir(m_sptrDrv3,m_nInputDevice);
	else
		m_2FramesBuffer.CopyDataDIB(m_sptrDrv3,m_nInputDevice);
	m_bInvalidatePrv = true;
/*	DWORD dwEnd = GetTickCount();;
	message<mc_DebugText>("CSewLIView::Invalidate starts at %d, occur %d\n", dwStart-s_dwInvalidate,
		dwEnd-dwStart);*/
	return S_OK;
}

HRESULT CSewLIView::OnChangeSize()
{
	return S_OK;
}


////////////////////////////////////////////////////////////////////////
// INotifyPlace
////////////////////////////////////////////////////////////////////////

HRESULT CSewLIView::NotifyPutToData( IUnknown* punkObj )
{
	return E_FAIL;
}

HRESULT CSewLIView::NotifyPutToDataEx( IUnknown* punkObj, IUnknown** punkUndoObj )
{
	ISewImageListPtr sptrSLI(m_sptrObjectList);
	if (sptrSLI == 0)
		return E_FAIL;
	*punkUndoObj = NULL;
	HRESULT hr = sptrSLI->AddImage(punkObj, CPoint(0,0), addImageFindPos);
	if (SUCCEEDED(hr))
	{
		long lpos;
		sptrSLI->GetLastFragmentPosition(&lpos);
		if (lpos != 0)
		{
			ISewFragmentPtr sptr;
			sptrSLI->GetNextFragment(&lpos, &sptr);
			if (sptr != 0)
			{
				*punkUndoObj = sptr;
				(*punkUndoObj)->AddRef();
			}
		}
	}
	return S_OK;
}

HRESULT CSewLIView::Undo( IUnknown* punkUndoObj )
{
	ISewImageListPtr sptrSLI(m_sptrObjectList);
	if (sptrSLI == 0)
		return E_FAIL;
	ISewFragmentPtr ptrFragToDel(punkUndoObj);
	if (ptrFragToDel != 0)
	{
		long lpos,lToDel=0;
		sptrSLI->GetFirstFragmentPos(&lpos);
		while (lpos)
		{
			long lPrev = lpos;
			ISewFragmentPtr ptrFragCur;
			sptrSLI->GetNextFragment(&lpos, &ptrFragCur);
			if (ptrFragCur == ptrFragToDel)
				lToDel = lPrev;
		}
		if (lToDel != 0)
			sptrSLI->DeleteFragment(lToDel);
	}
	return S_OK;
}

HRESULT CSewLIView::Redo( IUnknown* punkUndoObj )
{
	ISewImageListPtr sptrSLI(m_sptrObjectList);
	if (sptrSLI == 0)
		return E_FAIL;
	ISewFragmentPtr ptrFragRestore(punkUndoObj);
	if (ptrFragRestore != 0)
	{
//		ptrFragRestore->RestoreImages();
		sptrSLI->AddFragment(ptrFragRestore);
	}
	return S_OK;
}


/////////////////////////////////////////////////
//interface ISewLIView
//////////////////////////////////////////////////////////////////////
HRESULT CSewLIView::GetTypeInfoCount( 
            /* [out] */ UINT *pctinfo)
{
	if( !pctinfo )
		return E_POINTER;

	*pctinfo = 1;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CSewLIView::GetTypeInfo( 
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo)
{
	if( !ppTInfo )
		return E_POINTER;

	if( !iTInfo )
		return E_INVALIDARG;

	if( !m_pi_type_info )
		return E_FAIL;

	m_pi_type_info->AddRef();

	(*ppTInfo) = m_pi_type_info;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CSewLIView::GetIDsOfNames( 
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId)
{
	if( !m_pi_type_info )
		return E_FAIL;

	return m_pi_type_info->GetIDsOfNames( rgszNames, cNames, rgDispId );
}


//////////////////////////////////////////////////////////////////////
HRESULT CSewLIView::Invoke( 
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr)
{
	if( !m_pi_type_info )
		return E_FAIL;

	return m_pi_type_info->Invoke(	(IUnknown *)(ISewLIView *)this, dispIdMember, wFlags, 
		pDispParams, pVarResult, pExcepInfo, puArgErr );
}

HRESULT STDMETHODCALLTYPE CSewLIView::get_Name(/* [retval][out] */ BSTR *pVal)
{
	*pVal = m_bstrName.copy();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CSewLIView::put_Name(/* [in] */ BSTR newVal)
{
	m_bstrName = newVal;
	return S_OK;
}

HRESULT CSewLIView::get_OverlapProcent(/* [retval][out] */ int *pVal)
{
	*pVal = 0;
	if (m_sptrIP != 0)
		*pVal = m_nPrvOvrPerc;
	else
	{
		ISewImageListPtr sptrSLI(m_sptrObjectList);
		if (sptrSLI != 0)
			sptrSLI->GetOverlapPercent(pVal);
	}
	return S_OK;
}

HRESULT CSewLIView::get_OverlayCoefficient(/* [retval][out] */ int *pVal)
{
	*pVal = GetOptimalDiff();
	return S_OK;
}

HRESULT CSewLIView::get_ShowLiveVideo(/* [retval][out] */ int *pVal)
{
	*pVal = m_sptrIP!=0?1:0;
	return S_OK;
}

HRESULT CSewLIView::put_ShowLiveVideo(/* [in] */ int newVal)
{
	if (newVal == 0)
	{
		if (m_sptrIP!=0)
		{
			DeinitPreview();
			_UpdateScrollSizes();
			InvalidateRect(m_hwnd, NULL, TRUE);
		}
	}
	else
	{
		if (m_sptrIP==0)
		{
			InitPreview();
			_UpdateScrollSizes();
			InvalidateRect(m_hwnd, NULL, TRUE);
		}
	}
	return S_OK;
}

HRESULT CSewLIView::GetLiveVideoRect(/* [out] */ int *pLeft, /* [out] */ int *pTop,
        /* [out] */ int *pRight, /* [out] */ int *pBottom)
{
	*pLeft = m_rcPreviewRect.left;
	*pRight = m_rcPreviewRect.right;
	*pTop = m_rcPreviewRect.top;
	*pBottom = m_rcPreviewRect.bottom;
	return S_OK;
}

HRESULT CSewLIView::GetDocumentOrg(/* [out] */ int *px, /* [out] */ int *py)
{
	*px = m_ptCoordSpaceOrg.x;
	*py = m_ptCoordSpaceOrg.y;
	return S_OK;
}

HRESULT CSewLIView::MoveLiveVideo(/* [in] */ int x, /* [in] */ int y, /* [in] */ int nFlags)
{
	if (nFlags&moveFragCorrect)
	{
		DoMoveLV(x,y,0);
		return S_OK;
	}
	m_rcPreviewRect = CRect(CPoint(x,y), m_rcPreviewRect.Size());
	_UpdateScrollSizes();
	InvalidateRect(m_hwnd, NULL, TRUE);
	return S_OK;
}

HRESULT CSewLIView::get_LiveVideoMoving(/* [retval][out] */ int *pVal)
{
	*pVal = m_bLiveVideoMoving;
	return S_OK;
}

HRESULT CSewLIView::put_LiveVideoMoving(/* [in] */ int newVal)
{
	m_bLiveVideoMoving = newVal;
	return S_OK;
}

HRESULT CSewLIView::MoveLiveVideoTo(/*[in]*/ int x, /*[in]*/ int y, /*[in]*/ int nFlags )
{
	if (m_AutoMoveLVMode != AutoMoveLV)
		return S_FALSE;
	return DoMoveLVTo(x,y,nFlags)?S_OK:S_FALSE;
}

HRESULT CSewLIView::get_AutoMoveLiveVideoMode(/*[out,retval]*/ int * pVal )
{
	*pVal = m_AutoMoveLVMode;
	return S_OK;
}

HRESULT CSewLIView::put_AutoMoveLiveVideoMode(/*[in]*/ int pVal )
{
	if (pVal >= AutoMoveLV && pVal <= OnlyCalcDiff)
		m_AutoMoveLVMode = (AutoMoveLVMode)pVal;
	return S_OK;
}

HRESULT CSewLIView::get_ShowLiveVideoMode(/*[out,retval]*/ int * pVal )
{
	*pVal = m_DrawPreviewMode;
	return S_OK;
}

HRESULT CSewLIView::put_ShowLiveVideoMode(/*[in]*/ int pVal )
{
	if (pVal >= OpacquePreview && pVal <= SubtractPreview)
	{
		m_DrawPreviewMode = (DrawPreviewMode)pVal;
		InvalidateRect(m_hwnd, NULL, TRUE);
	}
	return S_OK;
}


bool CSewLIView::DoMoveLVTo(int x, int y, int nFlags)
{
	if (m_sptrIP == 0)
		return false;
	int nMode = nFlags&0xF;
	ISewImageListPtr sptrSLI(m_sptrObjectList);
	ISewFragmentPtr sptrFragFound;
	long lPos,lPosFound;
	if (nMode == 0)
	{
		sptrSLI->GetLastFragmentPosition(&lPos);
		while (lPos)
		{
			ISewFragmentPtr sptrFrag;
			long lPosPrev = lPos;
			sptrSLI->GetPrevFragment(&lPos, &sptrFrag);
			POINT ptFrag;
			sptrFrag->GetOffset(&ptFrag);
			SIZE szFrag;
			sptrFrag->GetSize(&szFrag);
			if (x >= ptFrag.x && x < ptFrag.x+szFrag.cx &&
				y >= ptFrag.y && y < ptFrag.y+szFrag.cy)
			{
				sptrFragFound = sptrFrag;
				lPosFound = lPosPrev;
				break;
			}
		}
	}
	else
	{
		sptrSLI->GetLastFragmentPosition(&lPos);
		if (lPos != 0)
		{
			sptrSLI->GetPrevFragment(&lPos, &sptrFragFound);
			CRect rc = GetFragmentRect(sptrFragFound);
			int hm = (nMode-1)%3;
			int vm = (nMode-1)/3;
			if (hm == 0)
				x = rc.left;
			else if (hm == 1)
				x = (rc.left+rc.right)/2;
			else //if(hm == 2)
				x = rc.right;
			if (vm == 0)
				y = rc.top;
			else if (vm == 1)
				y = (rc.top+rc.bottom)/2;
			else //if (vm == 2)
				y = rc.bottom;
		}
	}
	if (sptrFragFound != 0)
	{
		if (m_AutoMoveLVMode == AutoMoveLV)
		{
			// AutoMovePreview works with active fragment
			sptrSLI->SetActiveFragmentPosition(lPosFound);
			CRect rc = GetFragmentRect(sptrFragFound);
			// Calculate direction by click point
			x = x-(m_rcPreviewRect.Width()-m_rcPreviewRect.Width()*(x-rc.left)/rc.Width());
			y = y-(m_rcPreviewRect.Height()-m_rcPreviewRect.Height()*(y-rc.top)/rc.Height());
			_UpdateBaseFragAndOffs(sptrFragFound, CPoint(x,y));
			AutoMovePreview(true);
		}
	}
	return true;
}

bool CSewLIView::DoMoveLV(int x, int y, int nMode)
{
	if (m_sptrIP == 0)
		return false;
	CRect rcPrv(x,y,x+m_rcPreviewRect.Width(),y+m_rcPreviewRect.Height());
	ISewImageListPtr sptrSLI(m_sptrObjectList);
	ISewFragmentPtr sptrFragFound;
	int nOvrArea = 0;
	long lPos,lPosFound;
	sptrSLI->GetLastFragmentPosition(&lPos);
	while (lPos)
	{
		ISewFragmentPtr sptrFrag;
		long lPosPrev = lPos;
		sptrSLI->GetPrevFragment(&lPos, &sptrFrag);
		POINT ptFrag;
		sptrFrag->GetOffset(&ptFrag);
		SIZE szFrag;
		sptrFrag->GetSize(&szFrag);
		CRect rcFrag(ptFrag,szFrag);
		CRect rcInter;
		if (rcInter.IntersectRect(rcFrag,rcPrv))
		{
			int nArea = rcInter.Width()*rcInter.Height();
			if (nArea > nOvrArea)
			{
				sptrFragFound = sptrFrag;
				lPosFound = lPosPrev;
				nOvrArea = nArea;
				break;
			}
		}
	}
	if (sptrFragFound != 0)
	{
		// AutoMovePreview works with active fragment
		sptrSLI->SetActiveFragmentPosition(lPosFound);
		if (m_AutoMoveLVMode == AutoMoveLV)
		{
			// Find direction. Search will be preceed in that direction
			// from previous fragment origin to its bounds.
			CRect rc = GetFragmentRect(sptrFragFound);
			int nMinOvrPerc = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\FindOptimalPosition", "MinOverlapPercent", 10);
			CSize sz(rc.Width()*(100-nMinOvrPerc)/100,rc.Height()*(100-nMinOvrPerc)/100);
			if (abs(rc.left-x) < sz.cx && abs(rc.top-y) < sz.cy)
			{
				double d = min(double(sz.cx)/double(abs(rc.left-x)),double(sz.cy)/double(abs(rc.top-y)));
				x = rc.left+(int)((x-rc.left)*d);
				y = rc.top+(int)((y-rc.top)*d);
			}
		}
		_UpdateBaseFragAndOffs(sptrFragFound, CPoint(x,y));
		if (m_AutoMoveLVMode == AutoMoveLV || m_AutoMoveLVMode == CorrectAfterMove)
			AutoMovePreview(true);
		else if (m_AutoMoveLVMode == OnlyCalcDiff && m_bCalcOverlayCoef)
		{
			m_2FramesBuffer.CalcDifference(sptrSLI, sptrFragFound,
				CPoint(x,y), m_rcPreviewRect.Size());
			CheckOverlayPercent();
			CheckDiff();
		}
	}
	return true;
}


void CSewLIView::_UpdateScrollSizes()
{
	CRect rcImage(0,0,0,0);
	ISewImageListPtr sptrSLI(m_sptrObjectList);
	if (m_sptrObjectList != 0)
	{
		CPoint ptCoordSpaceOrg;
		sptrSLI->GetOrigin(&ptCoordSpaceOrg);
		CSize szCoordSpaceExt;
		sptrSLI->GetSize(&szCoordSpaceExt);
		rcImage = CRect(ptCoordSpaceOrg, szCoordSpaceExt);
	}
	if (m_sptrIP != 0)
	{
		CRect rcPrv(m_rcPreviewRect);
		rcImage.left = min(rcImage.left, rcPrv.left);
		rcImage.right = max(rcImage.right, rcPrv.right);
		rcImage.top = min(rcImage.top, rcPrv.top);
		rcImage.bottom = max(rcImage.bottom, rcPrv.bottom);
	}
	m_ptCoordSpaceOrg = rcImage.TopLeft();
	m_szCoordSpaceExt = rcImage.Size();

	IScrollZoomSitePtr	site( Unknown() );
	IScrollZoomSite2Ptr	site2( Unknown() );
	//update scroll size
	CSize	size;
	site->GetClientSize( &size );
	CSize	sizeNew = m_szCoordSpaceExt;

	if( size != sizeNew )
		site->SetClientSize( sizeNew );
}

void CSewLIView::GetFragmentOffset(ISewFragment *pFrag, POINT *pptOffs)
{
	pFrag->GetOffset(pptOffs);
	pptOffs->x -= m_ptCoordSpaceOrg.x;
	pptOffs->y -= m_ptCoordSpaceOrg.y;
}

CRect CSewLIView::ConvertToClient(CRect rcImage)
{
	rcImage.OffsetRect(-m_ptCoordSpaceOrg);
	double fZoom = 1.;
	_point pointScroll(0, 0);
	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
	{
		sptrSZ->GetZoom(&fZoom);
		sptrSZ->GetScrollPos(&pointScroll);
	}
	CRect	rc;
	rc.left = int(ceil( rcImage.left*fZoom-pointScroll.x));
	rc.right = int(ceil(rcImage.right*fZoom-pointScroll.x));
	rc.top = int(ceil( rcImage.top*fZoom-pointScroll.y));
	rc.bottom = int(ceil(rcImage.bottom*fZoom-pointScroll.y));
	return rc;
}

void CSewLIView::_UpdatePreviewSize()
{
	ISewImageListPtr sptrSIL(m_sptrObjectList);
	if (sptrSIL != 0)
	{
		long lLast = 0;
		sptrSIL->GetLastFragmentPosition(&lLast);
		if (lLast != 0)
		{
			ISewFragmentPtr sptrFrag;
			sptrSIL->GetNextFragment(&lLast, &sptrFrag);
			SIZE sz;
			sptrFrag->GetSize(&sz);
			m_rcPreviewRect = CRect(m_rcPreviewRect.TopLeft(),sz);
		}
		else
		{
			m_rcPreviewRect = CRect(CPoint(0,0),m_rcPreviewRect.Size());
		}
	}
}


bool CSewLIView::MakeLoadFragList(bool bLIChanged)
{
	double fZoom = 1.;
	_point pointScroll(0, 0);
	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
	{
		sptrSZ->GetZoom(&fZoom);
		sptrSZ->GetScrollPos(&pointScroll);
	}
	int nZoom = fZoom>=1.?1:(int)floor(1/fZoom);
	_rect rcClient;
	GetClientRect(m_hwnd, &rcClient);
	_rect rcImage(int(pointScroll.x/fZoom), int(pointScroll.y/fZoom),
		int((pointScroll.x+rcClient.width())/fZoom),
		int((pointScroll.y+rcClient.height())/fZoom));

	if (!bLIChanged)
	{
		if (rcImage == m_rcImagePrev)
			return false;
		/*if (rcImage.left >= m_rcBoundsPrev.left && rcImage.top >= m_rcBoundsPrev.top &&
			rcImage.right <= m_rcBoundsPrev.right && rcImage.bottom <= m_rcBoundsPrev.bottom)
			return; // Visible fragments are same*/
	}
	ISewImageListPtr sptrSIL(m_sptrObjectList);
	CLoadFragList NewList;
	if (sptrSIL == 0)
		return false;
	int nRangeCoef = 2;
	sptrSIL->GetRangeCoef(&nRangeCoef);
	int nViewRange = GetViewRangeByZoom(fZoom, nRangeCoef);
	CRect rcBounds(m_ptCoordSpaceOrg,m_szCoordSpaceExt);
	long lPos = 0;
	sptrSIL->GetFirstFragmentPos(&lPos);
	while (lPos)
	{
		ISewFragmentPtr sptrFrag;
		sptrSIL->GetNextFragment(&lPos, &sptrFrag);
		POINT pt;
		GetFragmentOffset(sptrFrag, &pt);
		SIZE sz;
		sptrFrag->GetSize(&sz);
		if (pt.x >= rcImage.right)
		{
			rcBounds.right = min(rcBounds.right,pt.x);
			continue;
		}
		if (pt.y >= rcImage.bottom)
		{
			rcBounds.bottom = min(rcBounds.bottom,pt.y);
			continue;
		}
		if (pt.x+sz.cx < rcImage.left)
		{
			rcBounds.left = max(rcBounds.left,pt.x+sz.cx);
			continue;
		}
		if (pt.y+sz.cy < rcImage.top)
		{
			rcBounds.top = max(rcBounds.top,pt.y+sz.cy);
			continue;
		}
		NewList.insert(new CSewLoadedFragment(sptrFrag,nZoom));
	}

	m_LoadFragList.deinit();
	long lpos = NewList.head();
	while (lpos)
	{
		CSewLoadedFragment *p = NewList.next(lpos);
		m_LoadFragList.insert(new CSewLoadedFragment(p->m_pFrag,p->m_nZoom));
	}
	m_rcBoundsPrev = rcBounds;
	m_rcImagePrev = rcImage;
	return true;
}

BOOL CSewLIView::DrawActiveFragment(HDC hdcMem, DrawFrameMode Mode)
{
	CRect	rectActive(0,0,0,0);
	if (m_sptrIP != 0)
		rectActive = ConvertToClient(m_rcPreviewRect);
	else
	{
		double fZoom = 1.;
		_point pointScroll(0, 0);
		IScrollZoomSitePtr sptrSZ(Unknown());
		if(sptrSZ != 0) 
		{
			sptrSZ->GetZoom(&fZoom);
			sptrSZ->GetScrollPos(&pointScroll);
		}
		ISewImageListPtr sptrSIL(m_sptrObjectList);
		if (sptrSIL == 0)
			return false;
		long lPosActive;
		sptrSIL->GetActiveFragmentPosition(&lPosActive);
		if (lPosActive != 0)
		{
			ISewFragmentPtr sptrActive;
			sptrSIL->GetNextFragment(&lPosActive, &sptrActive);
			POINT ptOffsActive;
			GetFragmentOffset(sptrActive,&ptOffsActive);
			SIZE szActive;
			sptrActive->GetSize(&szActive);
			rectActive.left = int(ceil( ptOffsActive.x*fZoom-pointScroll.x));
			rectActive.right = int(floor((ptOffsActive.x+szActive.cx)*fZoom-pointScroll.x + .5));
			rectActive.top = int(ceil( ptOffsActive.y*fZoom-pointScroll.y));
			rectActive.bottom = int(floor((ptOffsActive.y+szActive.cy)*fZoom-pointScroll.y + .5));
		}
	}
	if (rectActive.Width())
	{
		_pen pen(CreatePen(Mode!=Xor?PS_DOT:PS_SOLID,0,
			Mode==FirstDraw?RGB(0,0,0):RGB(255,255,255)));
		HPEN hpenOld = (HPEN)SelectObject(hdcMem,(HPEN)pen);
		COLORREF clrPrev = GetBkColor(hdcMem);
		if (Mode == Alternate)
			SetBkColor(hdcMem, RGB(0,0,0));
		::MoveToEx(hdcMem, rectActive.left, rectActive.top, NULL);
		::LineTo(hdcMem, rectActive.right-1, rectActive.top);
		::LineTo(hdcMem, rectActive.right-1, rectActive.bottom-1);
		::LineTo(hdcMem, rectActive.left, rectActive.bottom-1);
		::LineTo(hdcMem, rectActive.left, rectActive.top);
		if (Mode == Alternate)
			SetBkColor(hdcMem, clrPrev);
		SelectObject(hdcMem, hpenOld);
	}
	return true;
}

BOOL CSewLIView::DrawIntersection(HDC hdcMem)
{
	CRect rcInter(0,0,0,0);
	if (m_sptrIP != 0)
	{
		ISewFragment *pBaseFrag = m_LFragBase.GetFrag();
		if (pBaseFrag != 0)
		{
			CRect rcFrag = GetFragmentRect(pBaseFrag);
			rcInter.IntersectRect(rcFrag,m_rcPreviewRect);
		}
	}
	else
	{
		ISewImageListPtr sptrSIL(m_sptrObjectList);
		long lPosLast,lPosActive;
		sptrSIL->GetLastFragmentPosition(&lPosLast);
		sptrSIL->GetActiveFragmentPosition(&lPosActive);
		if (lPosLast != 0 && lPosLast == lPosActive)
		{
			long lPos = lPosLast;
			ISewFragmentPtr sptrFragLast,sptrFragPrev;
			sptrSIL->GetPrevFragment(&lPos,&sptrFragLast);
			if (lPos != 0)
			{
				sptrSIL->GetPrevFragment(&lPos,&sptrFragPrev);
				CRect rcLast = GetFragmentRect(sptrFragLast);
				CRect rcPrev = GetFragmentRect(sptrFragPrev);
				rcInter.IntersectRect(rcLast,rcPrev);
			}
		}
	}
	if (!rcInter.IsRectEmpty())
	{
		rcInter = ConvertToClient(rcInter);
		if (rcInter.Width() >= 7 && rcInter.Height() >= 7)
		{
			rcInter.InflateRect(-1,-1,-1,-1);
			_pen pen1(CreatePen(PS_SOLID, 0, (COLORREF)g_IntersectionColor1));
			_pen pen2(CreatePen(PS_SOLID, 0, (COLORREF)g_IntersectionColor2));
			HPEN hpenOld = (HPEN)SelectObject(hdcMem,(HPEN)pen1);
			::MoveToEx(hdcMem, rcInter.left, rcInter.top, NULL);
			::LineTo(hdcMem, rcInter.right-1, rcInter.top);
			::LineTo(hdcMem, rcInter.right-1, rcInter.bottom-1);
			::LineTo(hdcMem, rcInter.left, rcInter.bottom-1);
			::LineTo(hdcMem, rcInter.left, rcInter.top);
			(HPEN)SelectObject(hdcMem,(HPEN)pen2);
			::MoveToEx(hdcMem, rcInter.left+1, rcInter.top+1, NULL);
			::LineTo(hdcMem, rcInter.right-2, rcInter.top+1);
			::LineTo(hdcMem, rcInter.right-2, rcInter.bottom-2);
			::LineTo(hdcMem, rcInter.left+1, rcInter.bottom-2);
			::LineTo(hdcMem, rcInter.left+1, rcInter.top+1);
			SelectObject(hdcMem, hpenOld);
		}
	}

	return true;
}

//выводит на экран за один проход все фрагменты из списка pFragmentList
BOOL CSewLIView::DrawImages(HDC hdcScreen, CRect rectPaint1)
{
	HDC hdcMem; 
	HBITMAP hBitmapOld;
	BITMAPINFOHEADER	bmih;
	byte	*pdibBits = 0;
	HBITMAP	hDIBSection;
	HBRUSH hbrBkgnd;
	
	CRect rectPaint(rectPaint1);
//	BOOL bSmoothTrans = GetValueInt(GetAppUnknown(), "\\Sew Large Image", "SmoothTransition", 1);

	double fZoom = 1.;
	_point pointScroll(0, 0);
	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
	{
		sptrSZ->GetZoom(&fZoom);
		sptrSZ->GetScrollPos(&pointScroll);
	}
	//get the image rectangle
	CSize	size_cur = m_szCoordSpaceExt;
	// Image area in window coordinates.
	CRect	rectImage;
	rectImage.left = int(ceil( 0*fZoom-pointScroll.x));
	rectImage.right = int(floor((size_cur.cx)*fZoom-pointScroll.x + .5));
	rectImage.top = int(ceil( 0*fZoom-pointScroll.y));
	rectImage.bottom = int(floor((size_cur.cy)*fZoom-pointScroll.y + .5));
	// Paint area in window coordinates.
	rectPaint.left = min( max( rectPaint.left, rectImage.left ), rectImage.right );
	rectPaint.right = min( max( rectPaint.right, rectImage.left ), rectImage.right );
	rectPaint.top = min( max( rectPaint.top, rectImage.top ), rectImage.bottom );
	rectPaint.bottom = min( max( rectPaint.bottom, rectImage.top ), rectImage.bottom );
	// Paint area in image coordinates.
	CRect rectPaintImg;
	rectPaintImg.left = (int)floor((rectPaint.left+pointScroll.x)/fZoom);
	rectPaintImg.right = (int)ceil((rectPaint.right+pointScroll.x)/fZoom);
	rectPaintImg.top = (int)floor((rectPaint.top+pointScroll.y)/fZoom);
	rectPaintImg.bottom = (int)ceil((rectPaint.bottom+pointScroll.y)/fZoom);
	//init the DIB hearer
	ZeroMemory( &bmih, sizeof( bmih ) );
	bmih.biBitCount = 24;
	bmih.biHeight = rectPaint.Height();
	bmih.biWidth = rectPaint.Width();
	bmih.biSize = sizeof( bmih );
	bmih.biPlanes = 1;
	bmih.biSizeImage=((int)(bmih.biWidth*3+3)/4)*4*bmih.biHeight;
	if( bmih.biHeight == 0 ||bmih.biWidth == 0 )
		return false;
	// Create a normal DC and a memory DC for the entire screen.
	hdcMem = CreateCompatibleDC(hdcScreen); 
	//create a DIB section
	hDIBSection = ::CreateDIBSection( hdcScreen, (BITMAPINFO*)&bmih, DIB_PAL_COLORS, (void**)&pdibBits, 0, 0 );
	//propare the memory DC to drawing
	hBitmapOld = (HBITMAP)::SelectObject(hdcMem, hDIBSection);
	::SetMapMode(hdcMem, MM_TEXT);
	::SetViewportOrgEx(hdcMem, -rectPaint.left, -rectPaint.top, NULL);
	COLORREF	cr = ::GetValueColor( GetAppUnknown(), "\\Colors", "Background", RGB( 128, 128, 255 ) );
	hbrBkgnd = CreateSolidBrush(cr); 
	::FillRect( hdcMem,rectPaint, hbrBkgnd );
	if (IsRealDoc(m_sptrObjectList))
	{
	// Allocate memory for distance map.
	_ptr_t2<WORD> GlobalDistMap;
	GlobalDistMap.alloc(bmih.biWidth*bmih.biHeight);
	GlobalDistMap.zero();
	// Process fragments drawing itself.
	long lPos = m_LoadFragList.head();;
	while (lPos)
	{
		CSewLoadedFragment *pSewLoadedFragment = m_LoadFragList.next(lPos);
		ISewFragment *pFrag = pSewLoadedFragment->m_pFrag;
		IUnknownPtr punkImage;
		int nZoom = fZoom>=1.?1:(int)floor(1/fZoom);
		pFrag->GetImage(&nZoom, &punkImage);
		POINT ptOffset;
		GetFragmentOffset(pFrag,&ptOffset);
		SIZE szFrag;
		pFrag->GetSize(&szFrag);
		double dFragZoom = 1./double(nZoom);
		CPoint ptZOffset(ptOffset.x/nZoom,ptOffset.y/nZoom);

		sptrINamedDataObject2	sptrN(punkImage);
		IImage3Ptr image(punkImage);

		//cache color convertor information
		IUnknown *punkColorCnv = 0;
		if( image != 0 )
		{
			image->GetColorConvertor( &punkColorCnv );
		}
		else 
			continue;
		IColorConvertorExPtr		ptrCC(punkColorCnv);
		IColorConvertorEx2Ptr		ptrCC2(punkColorCnv);
		IColorConvertorLookupPtr	ptrCCLookup(punkColorCnv);
		if( punkColorCnv )punkColorCnv->Release();
		// Make transparency map
		IDistanceMapPtr sptrDistMap;
		pFrag->GetDistanceMap(&nZoom, &sptrDistMap);
		//set lookup table
		byte* pbyteLookup = 0;
		long nSize = 0;
		pbyteLookup = ::GetValuePtr(image, szLookup, szLookupTable, &nSize);
		if( ptrCCLookup != 0 )
		{
			int cx = 256;
			int cy = nSize/cx; 
			ptrCCLookup->SetLookupTable( pbyteLookup, cx, cy );
			if( pbyteLookup )delete pbyteLookup;
		}
		IUnknown *punkColorCnvTemp = 0;
		image->GetColorConvertor( &punkColorCnvTemp );
		CPoint	point = CPoint( 0, 0 );
		if( punkColorCnvTemp )
		{
			IColorConvertorExPtr	ptrCCTemp( punkColorCnvTemp );
			IColorConvertor2Ptr ptrCC2Temp( punkColorCnvTemp );

			punkColorCnvTemp->Release();
	
			image->GetOffset( &point );
			CRect	rectDraft;
			rectDraft.left = point.x;
			rectDraft.top = point.y;
			rectDraft.right = rectDraft.left+m_szCoordSpaceExt.cx;
			rectDraft.bottom = rectDraft.top+m_szCoordSpaceExt.cy;
	
			rectDraft.left = int( rectDraft.left*fZoom-pointScroll.x-2 );
			rectDraft.right = int( rectDraft.right*fZoom-pointScroll.x+2 );
			rectDraft.top = int( rectDraft.top*fZoom-pointScroll.y-2 );
			rectDraft.bottom = int( rectDraft.bottom*fZoom-pointScroll.y+2 );

			bool bScrollZoom=true;
			if( pdibBits)
			{
				rectDraft.IntersectRect( rectPaint, rectDraft );
				if( (rectDraft.right <= rectDraft.left||	rectDraft.bottom <= rectDraft.top) && bScrollZoom )
					continue;
				if (ptrCC2 != 0 && (int)g_SmoothTransition && sptrDistMap != 0)
					ptrCC2->ConvertImageToDIBRect2( &bmih, pdibBits,
							(WORD *)GlobalDistMap, rectPaint.TopLeft(), 
							(IImage2*)image, rectPaint,  ptZOffset, 
							bScrollZoom ? fZoom/dFragZoom : 1, bScrollZoom ? pointScroll : CPoint(0, 0),
							cr, /*dwDrawFlags*/0, sptrDistMap, Unknown());
				else
					ptrCC->ConvertImageToDIBRect( &bmih, pdibBits, rectPaint.TopLeft(), 
							(IImage2*)image, rectPaint,  ptZOffset, 
							bScrollZoom ? fZoom/dFragZoom : 1, bScrollZoom ? pointScroll : CPoint(0, 0),
							cr, /*dwDrawFlags*/0, Unknown());
			}
		}
		if( ptrCCLookup != 0 )
			ptrCCLookup->KillLookup();
	}
	// Draw preview with frame or active fragment's frame.
	if (m_sptrIP != 0)
	{
		if (m_DrawPreviewMode == OpacquePreview || !m_2FramesBuffer.CanDraw())
			DrawPreview(hdcMem, TRUE);
		else if (m_DrawPreviewMode == TransparentPreview ||
			m_DrawPreviewMode == SubtractPreview)
			DrawPreview2(&bmih, pdibBits, GlobalDistMap, rectPaintImg,
				fZoom, pointScroll);
	}
	DrawActiveFragment(hdcMem, m_bAlternateFrame?Alternate:FirstDraw);
	DrawIntersection(hdcMem);
	}
	else
	{
		ISewImageListPtr sptrSLI(m_sptrObjectList);
		IUnknownPtr punkTI;
		sptrSLI->GetTotalImage(&punkTI);
		if (punkTI != 0)
		{
			IImage3Ptr image(punkTI);
			int cxImg,cyImg;
			image->GetSize(&cxImg,&cyImg);
			double dTotZoom = double(cxImg)/double(m_szCoordSpaceExt.cx);
			IUnknownPtr punkColorCnv;
			image->GetColorConvertor(&punkColorCnv);
			IColorConvertorExPtr		ptrCC(punkColorCnv);
			IColorConvertorEx2Ptr		ptrCC2(punkColorCnv);
			IColorConvertorLookupPtr	ptrCCLookup(punkColorCnv);
			ptrCC->ConvertImageToDIBRect(&bmih, pdibBits, rectPaint.TopLeft(), 
					(IImage2*)image, rectPaint,  CPoint(0,0), 
					fZoom/dTotZoom, pointScroll,
					cr, /*dwDrawFlags*/0, Unknown());
		}
	}
	
	::SetViewportOrgEx(hdcMem, 0, 0, NULL);
	::SetDIBitsToDevice( hdcScreen, rectPaint.left, rectPaint.top, rectPaint.Width(), rectPaint.Height(),
			0, 0, 0, rectPaint.Height(), pdibBits, (BITMAPINFO*)&bmih, DIB_RGB_COLORS );
	//prepare to delete objects
	::SelectObject(hdcMem, hBitmapOld );

	// Fill outside sew rectangle
	if (rectImage.right < rectPaint1.right)
	{
		CRect rc(rectImage.right,rectPaint1.top,rectPaint1.right,rectPaint1.bottom);
		::FillRect(hdcScreen,rc,hbrBkgnd);
	}
	if (rectImage.bottom < rectPaint1.bottom)
	{
		CRect rc(rectPaint1.left,rectImage.bottom,rectPaint1.right,rectPaint1.bottom);
		::FillRect(hdcScreen,rc,hbrBkgnd);
	}

	DeleteObject(hbrBkgnd); 
	if( hDIBSection )
		DeleteObject(hDIBSection);
	//::SetROP2( hdcScreen, R2_COPYPEN );
	::DeleteDC(hdcMem);
	return true;
}

void CSewLIView::InitPreview()
{
	DeinitPreview();
	if (!GetValueInt(GetAppUnknown(), "\\Sew Large Image", "ShowPreview", TRUE))
		return;
	IUnknownPtr punkDM(_GetOtherComponent(GetAppUnknown(), IID_IDriverManager), false);
	if (punkDM != 0)
	{
		sptrIDriverManager sptrDM(punkDM);
		IUnknownPtr punkD;
		sptrDM->GetCurrentDeviceAndDriver(NULL, &punkD, &m_nInputDevice);
		m_sptrIP = punkD;
		m_sptrDrv3 = punkD;
		if (m_sptrIP != 0)
		{
			m_sptrDrv3->GetFlags(&m_dwDrvFlag);
			if (!(g_CheckGetImage == 0 || (m_dwDrvFlag&FG_GETIMAGE_DIB)))
			{
				m_sptrIP = 0;
				m_sptrDrv3 = 0;
				return;
			}
			if (FAILED(m_sptrIP->BeginPreview(m_nInputDevice, this)))
			{
				m_sptrIP = 0;
				m_sptrDrv3 = 0;
				return;
			}
			DWORD dwFreq = 0;
			m_sptrIP->GetPreviewFreq(m_nInputDevice, &dwFreq);
			m_bAlwaysRedrawPrv = dwFreq > 0;
			m_nTimerPrv = SetTimer(m_hwnd, 2, dwFreq>0?dwFreq:60, NULL);
			CPoint ptPrv = GetInitPreviewPos();
			CSize szPrv = GetDrvImgSize();
			m_rcPreviewRect = CRect(ptPrv, szPrv);
			_UpdateBaseFragAndOffs();
			m_AutoMoveLVMode = (AutoMoveLVMode)Range(GetValueInt(GetAppUnknown(), "\\Sew Large Image", "AutoMoveLiveVideoMode", m_AutoMoveLVMode),
				FirstDraw,Xor);
			m_nDiffThresh = GetValueInt(GetAppUnknown(), "\\Sew Large Image", "DifferenceThreshold", 20);
//			m_nDiffThresh2 = GetValueInt(GetAppUnknown(), "\\Sew Large Image", "DifferenceThreshold2", 60);
			m_bCalcOverlayCoef = GetValueInt(GetAppUnknown(), "\\Sew Large Image", "CalcOverlayCoefficient", TRUE);
			m_DrawPreviewMode = (DrawPreviewMode)Range(GetValueInt(GetAppUnknown(), "\\Sew Large Image", "DrawPreviewMode", m_DrawPreviewMode),
				OpacquePreview,SubtractPreview);
		}
	}
}

void CSewLIView::DeinitPreview()
{
	if (m_sptrIP != 0)
	{
		m_sptrIP->EndPreview(m_nInputDevice, this);
		m_sptrIP = 0;
		m_sptrDrv3 = 0;
	}
	if (m_nTimerPrv != 0)
	{
		KillTimer(m_hwnd, m_nTimerPrv);
		m_nTimerPrv = 0;
	}
	m_LFragBase.Unload();
	m_ptLVOffs = CPoint(0,0);
}

BOOL CSewLIView::DrawPreview(HDC hdc, BOOL b1st)
{
	CRect rcDst = ConvertToClient(m_rcPreviewRect);
	short cxPrv,cyPrv;
	m_sptrIP->GetSize(m_nInputDevice, &cxPrv, &cyPrv);
	CRect rcSrc(0, 0, cxPrv, cyPrv);
	m_sptrIP->DrawRect(m_nInputDevice, hdc, rcSrc, rcDst);
	return TRUE;
}

BOOL CSewLIView::DrawPreview2(LPBITMAPINFOHEADER lpbi, LPBYTE lpData,
	_ptr_t2<WORD> &GlobalDistMap, CRect rcPaintImg, double fZoom,
	CPoint pointScroll)
{
	CRect rcPreviewRect(m_rcPreviewRect);
	rcPreviewRect.OffsetRect(-m_ptCoordSpaceOrg.x,-m_ptCoordSpaceOrg.y);
	CRect rcLVPaintImg;
	if (!rcLVPaintImg.IntersectRect(rcPaintImg, rcPreviewRect))
		return FALSE;
	// Target rectangle
	CRect rcLVPaintBmp((rcLVPaintImg.left-rcPaintImg.left)*fZoom,
		(rcLVPaintImg.top-rcPaintImg.top)*fZoom,
		(rcLVPaintImg.right-rcPaintImg.left)*fZoom,
		(rcLVPaintImg.bottom-rcPaintImg.top)*fZoom);
	// Problems with integral division/multiplication.
	rcLVPaintBmp.left = Range(rcLVPaintBmp.left,0,lpbi->biWidth);
	rcLVPaintBmp.right = Range(rcLVPaintBmp.right,0,lpbi->biWidth);
	rcLVPaintBmp.top = Range(rcLVPaintBmp.top,0,lpbi->biHeight);
	rcLVPaintBmp.bottom = Range(rcLVPaintBmp.bottom,0,lpbi->biHeight);
	// Source rectangle
	double xSrc1 = ((double)(rcLVPaintImg.left-rcPreviewRect.left))/((double)rcPreviewRect.Width());
	double xSrc2 = ((double)(rcLVPaintImg.right-rcPreviewRect.left))/((double)rcPreviewRect.Width());
	double ySrc1 = ((double)(rcLVPaintImg.top-rcPreviewRect.top))/((double)rcPreviewRect.Height());
	double ySrc2 = ((double)(rcLVPaintImg.bottom-rcPreviewRect.top))/((double)rcPreviewRect.Height());
//	CRect rcLVPaintLVBmp(rcLVPaintImg.left-rcPreviewRect.left,
//		rcLVPaintImg.top-rcPreviewRect.top,rcLVPaintImg.right-rcPreviewRect.left,
//		rcLVPaintImg.bottom-rcPreviewRect.top);
	m_2FramesBuffer.PaintToDIB(lpbi, lpData, GlobalDistMap, xSrc1, ySrc1, xSrc2, ySrc2,
		rcLVPaintBmp, fZoom, m_DrawPreviewMode);
	return TRUE;
}


CPoint CSewLIView::GetInitPreviewPos()
{
	ISewImageListPtr sptrSIL(m_sptrObjectList);
	if (sptrSIL != 0)
	{
		long lLastPos;
		sptrSIL->GetLastFragmentPosition(&lLastPos);
		if (lLastPos != 0)
		{
			ISewFragmentPtr sptrFrag;
			sptrSIL->GetNextFragment(&lLastPos, &sptrFrag);
			if (sptrFrag != 0)
			{
				CPoint ptPos;
				sptrFrag->GetOffset(&ptPos);
				return ptPos;
			}
		}
	}
	return CPoint(0,0);
}

CSize CSewLIView::GetDrvImgSize()
{
	int cx=0,cy=0;
	IDriverPtr sptrDrv(m_sptrIP);
	if (sptrDrv != 0)
	{
		_variant_t varCX, varCY;
		sptrDrv->GetValue(m_nInputDevice, _bstr_t("CaptureSizeCX"), &varCX);
		sptrDrv->GetValue(m_nInputDevice, _bstr_t("CaptureSizeCY"), &varCY);
		cx = varCX.vt!=VT_I4?0:varCX.lVal;
		cy = varCY.vt!=VT_I4?0:varCY.lVal;
		if (cx == 0 || cy == 0)
		{
			ISewImageListPtr sptrSIL(m_sptrObjectList);
			if (sptrSIL != 0)
			{
				long lLastPos;
				sptrSIL->GetLastFragmentPosition(&lLastPos);
				if (lLastPos != 0)
				{
					ISewFragmentPtr sptrFrag;
					sptrSIL->GetNextFragment(&lLastPos, &sptrFrag);
					if (sptrFrag != 0)
					{
						SIZE sz;
						sptrFrag->GetSize(&sz);
						cx = sz.cx;
						cy = sz.cy;
					}
				}
			}
		}
	}
	if (cx == 0 || cy == 0)
	{
		cx = 1024;
		cy = 768;
	}
	return CSize(cx,cy);
}

void CSewLIView::_UpdateBaseFragAndOffs(ISewFragment *pFrag, CPoint ptOffs)
{
	ISewFragmentPtr sptrFrag;
	ISewImageListPtr sptrSLI(m_sptrObjectList);
	LPBITMAPINFOHEADER lpbi = m_2FramesBuffer.GetData();
	if (lpbi == NULL) return;
	if (pFrag)
	{
		sptrFrag = pFrag;
		m_ptLVOffs = ptOffs;
	}
	else
		sptrSLI->CalcOrigFragOffset(&sptrFrag, &m_ptLVOffs);
	// Find needed zoom.
	CSize szLV = m_rcPreviewRect.Size();
	double dZoomReq = max(szLV.cx/lpbi->biWidth,szLV.cy/lpbi->biHeight);
	int nRangeCoef;
	sptrSLI->GetRangeCoef(&nRangeCoef);
	int iZoom = 1;
	for (int i = 0; i < g_nFragmentImages; i++)
	{
		if (iZoom >= dZoomReq)
			break;
	}
	if (m_LFragBase.GetFrag() != (ISewFragment *)sptrFrag || 
		m_LFragBase.GetZoom() != iZoom)
		m_LFragBase.Load(sptrFrag, iZoom);
}


bool CSewLIView::AutoMovePreview(bool bForce)
{
	DWORD dwStart = GetTickCount();
	if (dwStart < m_dwLastAutoMove+m_dwAutoMoveTime && !bForce)
		return false;
	ISewImageListPtr sptrSLI(m_sptrObjectList);
	if (sptrSLI == 0)
		return false;
	BOOL bSaving,bLoading; // Whether some image is writing to disk or reading?
	sptrSLI->IsSavingOrLoading(&bSaving,&bLoading);
	if (bSaving || bLoading)
		return false;
	// Use previous fragment and its direction or fragment and direction which was set by right button.
	CPoint ptOffs = m_ptLVOffs;
	ISewFragment *pFrag = m_LFragBase.GetFrag();
	if (pFrag == NULL)
		return false;
	// Zoom will be used for correction
	double fZoom = 1.;
	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
		sptrSZ->GetZoom(&fZoom);
	ptOffs = m_2FramesBuffer.CalculateShiftLastFrag(sptrSLI, pFrag,
		ptOffs, m_rcPreviewRect.Size(), m_AutoMoveLVMode==AutoMoveLV?0:2, fZoom);
	CheckDiff();
	// Calculate new position and update area
	CRect rcPrvPrev = m_rcPreviewRect;
	CPoint ptFragPos;
	pFrag->GetOffset(&ptFragPos);
	CPoint ptNewPrvPos = ptFragPos+ptOffs;
	if (ptNewPrvPos == m_rcPreviewRect.TopLeft())
		return false;
	m_rcPreviewRect = CRect(ptNewPrvPos,m_rcPreviewRect.Size());
	// Update area
	CRect rcSA(min(rcPrvPrev.left,m_rcPreviewRect.left),min(rcPrvPrev.top,m_rcPreviewRect.top),
		max(rcPrvPrev.right,m_rcPreviewRect.right),max(rcPrvPrev.bottom,m_rcPreviewRect.bottom));
	if (m_sptrObjectList != 0)
	{
		// Whether begin or end of coordinates are changed?
		CPoint ptCoordSpaceOrg;
		sptrSLI->GetOrigin(&ptCoordSpaceOrg);
		CSize szCoordSpaceExt;
		sptrSLI->GetSize(&szCoordSpaceExt);
		CRect rcImage = CRect(ptCoordSpaceOrg, szCoordSpaceExt);
		if (rcSA.left < rcImage.left || rcSA.right > rcImage.right ||
			rcSA.top < rcImage.top || rcSA.bottom > rcImage.bottom)
		{
			_UpdateScrollSizes();
			InvalidateRect(m_hwnd, NULL, FALSE);
		}
		else
		{
			CRect rcSA1 = ConvertToClient(rcSA);
			rcSA1.InflateRect(1,1,1,1);
			InvalidateRect(m_hwnd, &rcSA1, FALSE);
		}
		// Whether need to scroll?
		IScrollZoomSitePtr sptrSZ(Unknown());
		if(sptrSZ != 0) 
			sptrSZ->EnsureVisible(m_rcPreviewRect-m_ptCoordSpaceOrg);
	}
	DWORD dwEnd = GetTickCount();
	m_dwAutoMoveTime = dwEnd-dwStart;
	if (m_dwAutoMoveTime > 1000) m_dwAutoMoveTime = 1000;
	m_dwLastAutoMove = dwEnd;
	return true;
}

void CSewLIView::CheckOverlayPercent()
{
	m_nPrvOvrPerc = 0;
	ISewFragment *pFrag = m_LFragBase.GetFrag();
	if (pFrag)
	{
		CPoint ptFrag;
		pFrag->GetOffset(&ptFrag);
		CSize szFrag;
		pFrag->GetSize(&szFrag);
		CRect rcFrag(ptFrag,szFrag);
		CRect rcInter;
		if (rcInter.IntersectRect(rcFrag,m_rcPreviewRect))
			m_nPrvOvrPerc = rcInter.Width()*rcInter.Height()*100/rcFrag.Width()/rcFrag.Height();
	}
	if (!m_bOvrPercNtyCalled && m_nPrvOvrPerc >= 0 && m_nPrvOvrPerc < 
		::GetValueInt(::GetAppUnknown(),"\\Sew Large Image", "MinOverlapPercentForControl", 5))
	{
		m_bOvrPercNtyCalled = true;
		IScriptSitePtr	ptr_ss = ::GetAppUnknown();
		if (ptr_ss != 0)
			ptr_ss->Invoke(_bstr_t("SewLI_OverlapPercentTooSmall"),
				0, NULL, 0, fwAppScript|fwFormScript);
	}
}

void CSewLIView::CheckDiff()
{
	if (m_nDiffThresh > 0)
	{
		int nDiff = GetOptimalDiff();
		int nDiffState = nDiff > m_nDiffThresh ? 1 : 0;
		int nPrevDiffState = m_nPrevDiffState;
		m_nPrevDiffState = nDiffState;
		if (nPrevDiffState != -1 && nPrevDiffState != nDiffState)
		{
			IScriptSitePtr	ptr_ss = ::GetAppUnknown();
			if (ptr_ss != 0)
			{
				_variant_t var((long)nDiffState);
				ptr_ss->Invoke(_bstr_t("SewLI_PositionCorrectnessChanged"),
					&var, 1, 0, fwAppScript|fwFormScript);
			}
		}
	}
}

