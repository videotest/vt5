#include "stdafx.h"
#include "view.h"
#include "apportionment.h"
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

#include "vfw.h"
#pragma comment(lib, "vfw32.lib")

//#define ID_CLASSMENU_BASE	2000

#define TIMER_ID 1

#define MOUSE_SECTION				"IntelliMouse"

//mouse wheel
#define MOUSE_WHEEL_ENABLE			"Wheel enable"
#define MOUSE_WHEEL_HORZ_KEY		"Wheel horz scroll key"
#define MOUSE_WHEEL_SCROLL_STEP		"Wheel step"
#define MOUSE_WHEEL_INVERT_HORZ		"Invert wheel horz direction"
#define MOUSE_WHEEL_INVERT_VERT		"Invert wheel vert direction"

#define VK_PAGE_DOWN	34
#define VK_PAGE_UP		33

#define RESTABLE_COL_WIDTH 150
//char* szNoObject = "No objects of this class";

DWORD     g_pdwBinaryColors[256];
DWORD     g_clSyncColor;

class DrawDib
{
public:
	DrawDib()
	{		m_hDrawDib = ::DrawDibOpen();	}
	~DrawDib()
	{		::DrawDibClose( m_hDrawDib );	}
	operator HDRAWDIB()
	{	return m_hDrawDib;}
protected:
	HDRAWDIB	m_hDrawDib;
};
DrawDib	g_DrawDibInstance;

_size ThumbnailGetRatioSize(_size sizeTarget, _size sizeThumbnail)
{	
	double fTargetWidth		= (double)sizeTarget.cx;
	double fTargetHeight	= (double)sizeTarget.cy;
	double fTargetZoom		= fTargetWidth / fTargetHeight;

	double fThumbnailWidth	= (double)sizeThumbnail.cx;
	double fThumbnailHeight	= (double)sizeThumbnail.cy;	
	double fThumbnailZoom	= fThumbnailWidth / fThumbnailHeight;

	int nWidth, nHeight;
	
	nWidth = (int)fTargetWidth;
	nHeight = (int)fTargetHeight;
	
	if( (double)nWidth / fThumbnailZoom > fTargetHeight )
	{		
		nWidth = (int)(fTargetHeight * fThumbnailZoom);
		nHeight = (int)(nWidth / fThumbnailZoom);
	}

	if( (double)nHeight * fThumbnailZoom > fTargetWidth )
	{
		nHeight = (int)(fTargetWidth / fThumbnailZoom);
		nWidth = (int)(nHeight * fThumbnailZoom);
	}

	return _size(nWidth, nHeight);
}	


bool ThumbnailDraw(BYTE* pbi, HDC hdc, _rect rcTarget, IUnknown* punkDataObject)
{
	 if( pbi == NULL )
		return false;

	BITMAPINFOHEADER* pBitmapInfoHeader = (BITMAPINFOHEADER*)pbi;

	BYTE* pDibBits = pbi + ((BYTE)sizeof(BITMAPINFOHEADER)) + ((BYTE)sizeof(TumbnailInfo));

	if( hdc == NULL )
		return false;

	TumbnailInfo* pti = (TumbnailInfo*) ( (BYTE*)pbi + (BYTE)sizeof(BITMAPINFOHEADER) );


	_rect rcDibOutput;

	_size sizeOutput = ::ThumbnailGetRatioSize(
							_size( rcTarget.width(), rcTarget.height() ),
							_size( pBitmapInfoHeader->biWidth, pBitmapInfoHeader->biHeight)
							);

	if( pti->dwFlags & tfVarStretch )
	{
		//Do nothing
	}
	else
	if( pti->dwFlags & tfCanStretch2_1 ) // only 2:1, || 1:1
	{
		if( rcTarget.width() > pBitmapInfoHeader->biWidth * 2 )
		{			
			sizeOutput.cx = pBitmapInfoHeader->biWidth * 2;
			sizeOutput.cy = pBitmapInfoHeader->biHeight * 2;

		}
		else
		if( rcTarget.width() > pBitmapInfoHeader->biWidth )
		{
			sizeOutput.cx = pBitmapInfoHeader->biWidth;
			sizeOutput.cy = pBitmapInfoHeader->biHeight;
		}
		
	}


	rcDibOutput.left	= rcTarget.left + (rcTarget.width() - sizeOutput.cx+1) / 2;
	rcDibOutput.top		= rcTarget.top  + (rcTarget.height() - sizeOutput.cy+1) / 2;

	rcDibOutput.right	= rcDibOutput.left + sizeOutput.cx;
	rcDibOutput.bottom	= rcDibOutput.top  + sizeOutput.cy;

	
	POINT p1[2];
	p1[0].x=rcDibOutput.left;
	p1[0].y=rcDibOutput.top;
	p1[1].x=rcDibOutput.right;
	p1[1].y=rcDibOutput.bottom;
	
	//int mm=::SetMapMode(hdc, MM_TEXT);
	int mm=::GetMapMode(hdc);
	//if(mm==MM_ISOTROPIC) LPtoDP(hdc,p1,2);
	_rect rcDibOutput1(p1[0].x, p1[0].y, p1[1].x, p1[1].y);
	if(mm==MM_ISOTROPIC)
	{ // workaround for windoze MM_ISOTROPIC scaling bug
		if(rcDibOutput1.width()==pBitmapInfoHeader->biWidth
			&& rcDibOutput1.height()==pBitmapInfoHeader->biHeight)
		{
			rcDibOutput1.right++;
			rcDibOutput1.bottom++;
		}
	}

	BOOL bRes = ::DrawDibDraw(
				g_DrawDibInstance, hdc, 
				rcDibOutput1.left, rcDibOutput1.top, rcDibOutput1.width(), rcDibOutput1.height(), // !!! ВНИМАНИЕ! Здесь винда может глючить - см. print (не preview)
				(BITMAPINFOHEADER*)pBitmapInfoHeader, pDibBits, 
				0, 0, pBitmapInfoHeader->biWidth, pBitmapInfoHeader->biHeight, 0 );
	//::SetMapMode(hdc, mm);


	if( punkDataObject )
	{
		
		IThumbnailSurfacePtr ptrTS( punkDataObject );
		if( ptrTS )
		{
			ptrTS->Draw( pbi, hdc, 
				rcTarget, _size( pBitmapInfoHeader->biWidth, pBitmapInfoHeader->biHeight),
				punkDataObject
				);
		}
		

		INamedDataObject2Ptr ptrDO( punkDataObject );
		if( ptrDO )
		{
			IUnknown* punkDoc = 0;			
			ptrDO->GetData( &punkDoc );
			if( punkDoc )
			{
				IThumbnailSurfacePtr ptrTS( punkDoc );
				if( ptrTS )
				{
					ptrTS->Draw( pbi, hdc, 
						rcTarget, _size( pBitmapInfoHeader->biWidth, pBitmapInfoHeader->biHeight),
						punkDataObject
						);
				}

				punkDoc->Release();	punkDoc = 0;
			}
		}
	}
	

	return true;
}


CAView::CAView()
{
	m_fPrevZoom = 1.;
	m_nCellSize = 0;
	m_rcPrevSelected = NORECT;
	m_rcPrevSelectedText = NORECT;
	m_nActiveClass = -1;
	m_nActiveThumb = -1;

	m_bPrintInited = false;
	m_nObjectsInRow = 0;

	m_sptrDragObj=0;
	m_ptDragPos=_point(0,0);
	m_rcPrevDrag = NORECT;
	m_ptDragOffset=_point(0,0);

	m_bTimerSet=0;
	m_ptLastDelta = NOPOINT;
	m_ptLastMousePos = NOPOINT;

	m_bstrName		= "Apportionment";

	char sz_buf[256];	sz_buf[0] = 0;	
	::LoadString( App::handle(), IDS_APPORTIONMENT_VIEW_NAME, sz_buf, sizeof(sz_buf) );	

	m_bstrUserName	= sz_buf;

	if( !m_bstrUserName.length() )
		m_bstrUserName = m_bstrName;

	m_wheelHorzSwitch=0;
	m_bUseMouseWheel = false;
	m_nWheelStep = 1;
	m_pi_type_lib	= 0;
	m_pi_type_info	= 0;
	m_bShowUnknown = true;

	m_nMinBorder = 0;

	m_nHighlightPhases = 0;
	m_nHighlightPercent = 30;

	_ReadParams();
}

CAView::~CAView()
{
	_KillClassCellsList();
}

static void set_object_class_with_undo(long lclass, bool bDeselect=false)
{
	char sz_cl[60];
	_itoa(lclass, sz_cl, 10);
	if(bDeselect) strcat(sz_cl,",\"\", 1"); //если  надо - задеселектить
	::ExecuteAction("SetClass",sz_cl,0);
}

IUnknown* CAView::DoGetInterface( const IID &iid )
{
	if( iid == IID_IPersist ) return (IPersist*)this;
	else if( iid == IID_INamedObject2 ) return (INamedObject2*)this;	
	else if( iid == IID_IMultiSelection ) return (IMultiSelection*)this;	
	else if( iid == IID_IPrintView ) return (IPrintView*)this;	
	else if( iid == IID_IMenuInitializer ) return (IMenuInitializer*)this;
	else if( iid == IID_IViewDispatch ) return (IViewDispatch*)this;
	else if( iid == IID_IDispatch ) return (IDispatch*)this;
	else if( iid == IID_IAView ) return (IAView*)this;
	else if (iid == IID_INamedPropBag || iid == IID_INamedPropBagSer) return (INamedPropBagSer*)this;
	else if (iid == IID_IHelpInfo) return (IHelpInfo*)this;
	else return CWinViewBase::DoGetInterface( iid );
}

HRESULT CAView::GetDispIID( GUID* pguid )
{
	if( !pguid ) return E_INVALIDARG;

	*pguid = IID_IAView;
	return S_OK;
};

bool CAView::DoStartTracking( _point point )
{
	BOOL bDragAndDropEnabled = ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "DragAndDropEnabled", 1) != 0;
	if(!bDragAndDropEnabled)
	{
		m_sptrDragObj=0; //нечего таскать
		return true;
	}
	IUnknown *punkObject=0;
	HRESULT r=GetObjectByIdx(0, &punkObject);
	m_sptrDragObj=IUnknownPtr(punkObject);
	if(punkObject) punkObject->Release();

	double fZoom = 1.;
	_point ptScroll(0, 0);
	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
	{
		sptrSZ->GetZoom(&fZoom);
		sptrSZ->GetScrollPos(&ptScroll);
	}
	int x = int(point.x*fZoom)-ptScroll.x, y = int(point.y*fZoom)-ptScroll.y;
	_point point1(x,y); //в координатах окна

	//_point point1 = m_pointTestTracking-ptScroll;
	m_ptDragOffset=_point(point1.x+ptScroll.x-m_rcPrevSelected.left,point1.y+ptScroll.y-m_rcPrevSelected.top);
	m_ptDragPos=point1.delta(m_ptDragOffset);
	m_ptDragPos+=ptScroll;

	_rect rcUpdate;
	if(!(m_rcPrevDrag==NORECT))
	{
		rcUpdate = m_rcPrevDrag;
		rcUpdate.inflate(2);
		rcUpdate.delta(-ptScroll.x, -ptScroll.y);
		InvalidateRect(m_hwnd,&rcUpdate,0);
	}

	m_rcPrevDrag=m_rcPrevSelected;

	if(m_hwnd && !m_bTimerSet)
	{
		m_bTimerSet = SetTimer(m_hwnd, TIMER_ID, 50, 0)!=0;
	}

	return true;
}

bool CAView::DoFinishTracking( _point point )
{
	if(m_bTimerSet)
	{
		KillTimer(m_hwnd, TIMER_ID);
		m_bTimerSet=false;
	}
	double fZoom = 1.;
	_point ptScroll(0, 0);
	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
	{
		sptrSZ->GetZoom(&fZoom);
		sptrSZ->GetScrollPos(&ptScroll);
	}
	int x = int(point.x*fZoom)-ptScroll.x, y = int(point.y*fZoom)-ptScroll.y;
	_point point1 = _window(point,m_ptrSite); //в координатах окна

	if(m_sptrDragObj!=0)
	{
		long nClassCounter = 0;
		long lPos = m_listClassCells.head();
		while(lPos)
		{
			long lPos1=lPos;
			ClassCell* pClassCell = m_listClassCells.next(lPos);
			if(pClassCell!=0)
			{
				_rect rc=pClassCell->m_rcLastDrawRect;
				if(point1.y+ptScroll.y>=rc.top && point1.y+ptScroll.y<rc.bottom)
				{
					ICalcObjectPtr sptrCO(m_sptrDragObj);
					if(sptrCO != 0)
					{
						//set_object_class( sptrCO, nClassCounter );
						int nOldClass = get_object_class( sptrCO );
						if( nClassCounter != nOldClass )
						{
							set_object_class_with_undo(nClassCounter, 0);
						}
						INamedDataObject2Ptr	ptrNamed( m_sptrDragObj );
						IUnknown		*punkParent;
						ptrNamed->GetParent( &punkParent );
						IDataObjectListPtr	ptrList( punkParent );
						if( punkParent )punkParent->Release();
						if( ptrList!=0 )
						{
							ptrList->UpdateObject( ptrNamed );
						}
					}
					break; //иначе падаем на продолжении обхода
				}
			}
			nClassCounter++;
		}
		m_sptrDragObj=0; //закончим таскание
	}

	InvalidateRect(m_hwnd,0,0);
	return true;
}

bool CAView::DoTrack( _point point )
{
	double fZoom = 1.;
	_point ptScroll(0, 0);
	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
	{
		sptrSZ->GetZoom(&fZoom);
		sptrSZ->GetScrollPos(&ptScroll);
	}
	int x = int(point.x*fZoom)-ptScroll.x, y = int(point.y*fZoom)-ptScroll.y;
	_point point1(x,y);

	if(m_ptDragOffset.x>0) m_ptDragOffset.x=0;
	if(m_ptDragOffset.y>0) m_ptDragOffset.y=0;
	m_ptDragPos=point1.delta(m_ptDragOffset);
	m_ptDragPos+=ptScroll;
	//m_ptDragPos.x = int(m_ptDragPos.x*fZoom);
	//m_ptDragPos.y = int(m_ptDragPos.y*fZoom);
	_rect rcUpdate;

	if(!(m_rcPrevDrag==NORECT))
	{
		rcUpdate = m_rcPrevDrag;
		rcUpdate.inflate(2);
		rcUpdate.delta(-ptScroll.x, -ptScroll.y);
		InvalidateRect(m_hwnd,&rcUpdate,0);
	}

	if(m_sptrDragObj!=0)
	{
		_rect rc(m_ptDragPos,m_rcPrevSelected.size());
		m_rcPrevDrag=rc;
		rcUpdate = rc;
		rcUpdate.inflate(2);
		rcUpdate.delta(-ptScroll.x, -ptScroll.y);
		InvalidateRect(m_hwnd,&rcUpdate,0);
	}
	else m_rcPrevDrag=NORECT; //если объект не задан - тем проще...

	return true;
}

	
LRESULT	CAView::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
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
		//case WM_LBUTTONDOWN:	lResult = OnLButtonClick((int)LOWORD(lParam), (int)HIWORD(lParam));		
			//break;
		case WM_TIMER:			lResult = OnTimer(nMsg);
			break;
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
	}
	if( lResult )return lResult;

	if(m_ptrSite!=0)
	{
		lResult = DoMouseMessage( nMsg, wParam, lParam );
		if( lResult )return lResult;
	}

	return CWinViewBase::DoMessage( nMsg, wParam, lParam );
}

void CAView::DoAttachObjects()
{
	IUnknown* punkObjectList = ::GetActiveObjectFromContext(Unknown(), szTypeObjectList);

	if( punkObjectList == 0 )
	{
		SIZE size;
		size.cx = size.cy = 0;
		SetClientSize(size); 

		m_sptrObjectList = 0;
	}

	if( ::GetKey(m_sptrObjectList) != ::GetKey(punkObjectList) )
	{
		m_sptrObjectList = punkObjectList;
	}
	if(punkObjectList) punkObjectList->Release();

	BuildView(true);
}

LRESULT CAView::OnPaint()
{
	PAINTSTRUCT	ps;	

	_rect rectPaint(0, 0, 0, 0);
	::GetUpdateRect(m_hwnd, &rectPaint, FALSE);

	/* debug - output to file 
	FILE *f=fopen("e:\\mylog.log","a");
	fprintf(f,"paint (%i,%i)-(%i,%i)\n",
		rectPaint.left,rectPaint.top,
		rectPaint.right,rectPaint.bottom);
	fclose(f);
	/**/

	HDC hdcPaint = ::BeginPaint( m_hwnd, &ps );	

	BITMAPINFOHEADER	bmih;
	ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
	bmih.biBitCount = 24;
	bmih.biHeight	= rectPaint.height();
	bmih.biWidth	= rectPaint.width();
	bmih.biSize		= sizeof(BITMAPINFOHEADER);
	bmih.biPlanes	= 1;
	
	if(m_listClassCells.Count() == 0 || bmih.biHeight == 0 ||bmih.biWidth == 0)
	{
		_rect rectImage = NORECT;
		::GetClientRect(m_hwnd, &rectImage);
		::FillRect(hdcPaint, &rectImage, CreateSolidBrush(m_clrInactive));
		::EndPaint( m_hwnd, &ps );
		return 1;
	}

	double fZoom = 1.;
	_point ptScroll(0, 0);
	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
	{
		sptrSZ->GetZoom(&fZoom);
		sptrSZ->GetScrollPos(&ptScroll);
	}

	if(m_fPrevZoom != fZoom)
	{
		m_fPrevZoom = fZoom;
		BuildView(false);
	}
	
	//create a DIB section
	byte	*pdibBits = 0;
	HBITMAP	hDIBSection = ::CreateDIBSection(hdcPaint, (BITMAPINFO*)&bmih, DIB_RGB_COLORS, (void**)&pdibBits, 0, 0);
	
	//set background
	for(int y = 0; y < bmih.biHeight; y++)
	{
		int nRowOffset = (bmih.biWidth*3+3)/4*4*y;
		for(int x = 0; x < bmih.biWidth; x++)
		{
			pdibBits[nRowOffset + 3*x + 2] = GetRValue(m_clrInactive);
			pdibBits[nRowOffset + 3*x + 1] = GetGValue(m_clrInactive);
			pdibBits[nRowOffset + 3*x + 0] = GetBValue(m_clrInactive);
		}
	}
	
	//drawing class cells
	long nCurHeightPos = (long)(m_nInterCellDist*fZoom + .5);

	ClassCell* pClassCell = 0;
	long lPos = m_listClassCells.head();
	long nClassCounter = 0;
	bool bDrawTable = true;
	while(lPos)
	{
		DrawClassCell(hdcPaint, hDIBSection, rectPaint, ptScroll, fZoom, m_listClassCells.next(lPos), nCurHeightPos, m_nActiveClass == nClassCounter);
		nClassCounter++;
	}

		
	//draw result bar
	{
		_rect rcP = rectPaint;
		rcP.delta(ptScroll.x, ptScroll.y);
		HDC hdcMem = ::CreateCompatibleDC(hdcPaint);
		HBITMAP hBitmapOld = (HBITMAP)::SelectObject(hdcMem, hDIBSection);
		::SetMapMode(hdcMem, MM_TEXT);
		::SetViewportOrgEx(hdcMem, -rcP.left, -rcP.top, NULL);
		
		_DrawResultBar(hdcMem, nCurHeightPos, fZoom, false);

		::SelectObject(hdcMem, hBitmapOld);
		::DeleteDC(hdcMem);

	}

	DrawDragObj(hdcPaint, hDIBSection, bmih, pdibBits, rectPaint, ptScroll, fZoom);

	//paint DIB to DC
	::DrawDibDraw(g_DrawDibInstance, hdcPaint, rectPaint.left, rectPaint.top, rectPaint.width(), rectPaint.height(), &bmih, pdibBits, 0, 0, rectPaint.width(), rectPaint.height(), 0);

	::DeleteObject(hDIBSection);
	::EndPaint( m_hwnd, &ps );

	return 1;
}

LRESULT CAView::OnEraseBackground( HDC hDC )
{
	if( !::IsWindow( m_hwnd ) )
		return 0;
	return 1;
}

LRESULT CAView::OnSize( int cx, int cy )
{
	if( !::IsWindow( m_hwnd ) )
		return 0;

	BuildView(false);

	return 0;
}

//////////////////////////////////////////////////////////////////////
LRESULT CAView::OnContextMenu( HWND hWnd, int x, int y )
{
	BOOL bContextMenuEnabled = ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "ContextMenuEnabled", 1) != 0;
	if(!bContextMenuEnabled) return 0;
	POINT pt;
	pt.x = x;
	pt.y = y;

	//ScreenToClient( m_hwnd, &pt );

	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );

	if( !punk )
		return 0;

	ICommandManager2Ptr ptrCM = punk;
	punk->Release();	punk = 0;

	if( ptrCM == NULL )
		return 0;
	
	CMenuRegistrator rcm;	

	UINT uCMD=0;
	_bstr_t	bstrMenuName = rcm.GetMenu( szApportionmentViewNameMenu, 0 );

	//ptrCM->ExecuteContextMenu2( bstrMenuName, pt, 0 );

	// register context menu initializer
	ptrCM->RegisterMenuCreator((IMenuInitializer*)this);

	// execute context menu (return value is action's ID which selected by user)
	ptrCM->ExecuteContextMenu3(bstrMenuName, pt, TPM_RETURNCMD, &uCMD);

	ptrCM->UnRegisterMenuCreator((IMenuInitializer*)this);

	long nClassCount = class_count();
	if(uCMD>=ID_CLASSMENU_BASE && uCMD<ID_CLASSMENU_BASE+UINT(nClassCount)+1)
	{
		IUnknown *punkObject=0;
		HRESULT r=GetObjectByIdx(0, &punkObject);
		ICalcObjectPtr sptrCO(punkObject);
		if(punkObject) punkObject->Release();
		if(sptrCO != 0)
		{
			//set_object_class( sptrCO, uCMD - ID_CLASSMENU_BASE - 1 );
			int m_nOldActiveClass = m_nActiveClass;
			int m_nOldActiveThumb = m_nActiveThumb;
			int nClass = uCMD - ID_CLASSMENU_BASE - 1;
			int nOldClass = get_object_class( sptrCO );
			if( nClass != nOldClass )
			{
				set_object_class_with_undo(nClass, 1);
			}
			{
				m_nActiveClass = m_nOldActiveClass;
				m_nActiveThumb = m_nOldActiveThumb;
				IUnknown *punk=0;
				GetObjectByIdx(0,&punk);
				if(punk==0)
				{
					m_nActiveThumb--;
					GetObjectByIdx(0,&punk);
				}
				if(punk!=0)
				{
					ICalcObjectPtr sptrCO1(punk);
					long lPos=0;
					m_sptrObjectList->GetFirstChildPosition(&lPos);
					while(lPos)
					{
						IUnknown* punkObject = 0;
						long lPos1=lPos;
						m_sptrObjectList->GetNextChild(&lPos, &punkObject);
						ICalcObjectPtr sptrCO2(punkObject);
						if(punkObject) punkObject->Release();
						if(sptrCO1==sptrCO2)
						{
							m_sptrObjectList->SetActiveChild( lPos1 );
						}
					}
					_SetActiveClassCell(punk);
					punk->Release();
				}
			}
			

			INamedDataObject2Ptr	ptrNamed( sptrCO );
			IUnknown		*punkParent;
			ptrNamed->GetParent( &punkParent );
			IDataObjectListPtr	ptrList( punkParent );
			if( punkParent )punkParent->Release();
			if( ptrList!=0 )
			{
				ptrList->UpdateObject( ptrNamed );
			}
		}
	}
	else
	{
		// set selected action arguments
		bool bExec = true;

		BSTR	bstrActionName;
		_bstr_t strArgs = "";
		DWORD	dwFlag = 0;

		if (!SUCCEEDED(ptrCM->GetActionName(&bstrActionName, uCMD)))
			return 0;
		
		_bstr_t strActionName = bstrActionName;
		::SysFreeString(bstrActionName);


		//AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
		::ExecuteAction(strActionName, strArgs, dwFlag);
	}

	return 0;
}

LRESULT CAView::OnCreate()
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
			strcpy( psz_last, "comps\\aview.tlb" );
			HRESULT hr = ::LoadTypeLib( _bstr_t( sz_path ), &m_pi_type_lib );
			dbg_assert( S_OK == hr );

			if( m_pi_type_lib )
			{
				hr = m_pi_type_lib->GetTypeInfoOfGuid( IID_IAView, &m_pi_type_info );
				dbg_assert( S_OK == hr );
			}
		}		
	}

	return 0;
}

LRESULT CAView::OnMouseWheel(UINT nFlags, short zDelta, _point pt)
{
	if(!m_bUseMouseWheel) return 0;
	if(nFlags && m_wheelHorzSwitch)
	{
		_point ptScroll(0,0);
		IScrollZoomSitePtr sptrSZ(Unknown());
		if(sptrSZ != 0)
		{
			sptrSZ->GetScrollPos(&ptScroll);
		}

		int nClass=0;
		long lPos = m_listClassCells.head();
		while(lPos)
		{
			ClassCell* pClassCell = m_listClassCells.next(lPos);
			if(pClassCell->m_rcLastDrawRect.bottom > ptScroll.y) break;
			nClass++;
		}

		long nClassCount = class_count();
		if(nClass==-1) nClass=0;
		if(zDelta>0) nClass--;
		if(zDelta<0) nClass++;
		nClass = max(0,min(nClassCount,nClass));

		ScrollToClass(nClass);
	}
	else
	{ // просто скролл - на высоту 1 ряда images
        int nRowsScrolled = zDelta / 120;
		IScrollZoomSitePtr sptrSZ(Unknown());
		if(sptrSZ != 0)
		{
			_point ptScroll(0,0);
			sptrSZ->GetScrollPos(&ptScroll);
			ptScroll.y -= nRowsScrolled*m_nWheelStep;
			sptrSZ->SetScrollPos(ptScroll);
		}
		return 1;
	}
	return 0;
}

LRESULT CAView::OnTimer(UINT_PTR nTimer)
{
	if(nTimer==TIMER_ID)
	{
		if(m_sptrDragObj!=0)
		{
			if(m_ptLastDelta.x || m_ptLastDelta.y)
			{
				IScrollZoomSitePtr sptrSZ(Unknown());
				if(sptrSZ != 0)
				{
					_point ptScroll(0,0);
					sptrSZ->GetScrollPos(&ptScroll);
					m_ptLastMousePos -= m_ptLastDelta;
					DoTrack(m_ptLastMousePos);
					EnsureVisible(m_ptLastMousePos);
					_point ptScroll2(m_ptLastMousePos);
					sptrSZ->GetScrollPos(&ptScroll2);
					m_ptLastDelta = ptScroll-ptScroll2; // для непрерывного скролла
				}
			}
		}
	}
	return 0;
}

LRESULT CAView::OnDestroy()
{
	if(m_bTimerSet)
	{
		KillTimer(m_hwnd, TIMER_ID);
		m_bTimerSet=false;
	}
	DeInitMouse();
	return 0;
}

bool CAView::DoLButtonDown( _point  point )
//LRESULT	CAView::OnLButtonClick( int x, int y )
{
	double fZoom = 1.;
	_point ptScroll(0, 0);
	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
	{
		sptrSZ->GetZoom(&fZoom);
		sptrSZ->GetScrollPos(&ptScroll);
	}
	int x = int(point.x*fZoom)-ptScroll.x, y = int(point.y*fZoom)-ptScroll.y;

	_rect rc = _CalcActiveClassCell(x, y);
	if(!(rc == NORECT))
	{
		_rect rcUpdate = m_rcPrevSelected;
		rcUpdate.delta(-ptScroll.x, -ptScroll.y);
		::InvalidateRect(m_hwnd, &rcUpdate, 0);
		rcUpdate = rc;
		rcUpdate.delta(-ptScroll.x, -ptScroll.y);
		::InvalidateRect(m_hwnd, &rcUpdate, 0);
		m_rcPrevSelected = rc;

		if(sptrSZ != 0) 
		{
			_rect rcClient = NORECT;
			GetClientRect(hwnd(),  &rcClient);
			rcClient.delta(ptScroll.x, ptScroll.y);
			
			if(rcClient.top > rc.top || rcClient.bottom <= rc.bottom)
			{				
				if( rcClient.top > rc.top )ptScroll.y-=rcClient.top - rc.top;
				if( rcClient.bottom < rc.bottom )ptScroll.y-=rcClient.bottom - rc.bottom-1;
				sptrSZ->SetScrollPos(ptScroll);
			}	

			//sptrSZ->EnsureVisible(_client(rc, sptrSZ));
		}
	}
	return 0;
}

bool CAView::DoLButtonDblClick( _point  point )
{
	double fZoom = 1.;
	_point ptScroll(0, 0);
	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
	{
		sptrSZ->GetZoom(&fZoom);
		sptrSZ->GetScrollPos(&ptScroll);
	}

	if( ::IsWindow( m_hwnd ) )
	{
		if(m_RenameEditBox.handle()) // если уже был эдитбокс - грохнем...
			::DestroyWindow( m_RenameEditBox.handle() );

		IUnknownPtr ptrObject=0;
		HRESULT r=GetObjectByIdx(0, &ptrObject);
		INamedObject2Ptr sptrNamedObject(ptrObject);
		if(sptrNamedObject!=0)
		{
			//_bstr_t bstr;
			//sptrNamedObject->GetUserName(bstr.GetAddress());
			_bstr_t	bstr = ::GetName(sptrNamedObject);

			::UpdateWindow( m_hwnd );
			_rect rc(m_rcPrevSelectedText);
			rc.delta(-ptScroll.x,-ptScroll.y);
			m_RenameEditBox.create(WS_VISIBLE|WS_CHILD, rc, bstr, m_hwnd, 0, _T("EDIT"));
			m_RenameEditBox.m_szText[0]=0;
			m_RenameEditBox.m_bReady = false;
			m_RenameEditBox.send_message(EM_SETSEL, 0, -1);
			SetFocus(m_RenameEditBox.handle());
			IApplicationPtr sptrApp( GetAppUnknown() );
			if( sptrApp != 0 ) 
			{
				while( m_RenameEditBox.handle() )
				{
					sptrApp->ProcessMessage();
					if(m_RenameEditBox.m_bReady)
					{ // был нажат Enter - прописали текст
						if(m_RenameEditBox.handle()) // если был эдитбокс - грохнем...
							::DestroyWindow( m_RenameEditBox.handle() );
						::SetName(sptrNamedObject, _bstr_t(m_RenameEditBox.m_szText));
					}
				}
			}
		}
	}

	return __super::DoLButtonDblClick(point);
}

bool CAView::DoRButtonDown( _point  point )
{
	DoLButtonDown( point );
	return __super::DoRButtonDown(point);
}

bool CAView::DoChar( int nChar, bool bKeyDown )
{
	if(nChar==VK_PAGE_DOWN || nChar==VK_PAGE_UP)
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
	}
	return __super::DoChar(nChar, bKeyDown );
}

void CAView::EnsureVisible( _point point )
{
	m_ptLastMousePos=point;
	int x=point.x, y=point.y;
	double fZoom = 1.;
	_point ptScroll(0, 0);
	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
	{
		sptrSZ->GetZoom(&fZoom);
		sptrSZ->GetScrollPos(&ptScroll);
	}
	int x1 = int(point.x*fZoom)-ptScroll.x, y1 = int(point.y*fZoom)-ptScroll.y;
	_point point1(x1,y1); //в координатах окна

	if(sptrSZ != 0) 
	{
		_rect rcClient = NORECT;
		GetClientRect(hwnd(),  &rcClient);
		//rcClient.delta(ptScroll.x, ptScroll.y);

		_point ptDelta(0,0);
		if(rcClient.top > y1 || rcClient.bottom <= y1)
		{				
			if( rcClient.top > y1 ) ptDelta.y = rcClient.top - y1;
			if( rcClient.bottom < y1 ) ptDelta.y = rcClient.bottom - y1-1;
		}

		if(ptDelta.y!=0 || ptDelta.x!=0)
		{
			sptrSZ->SetScrollPos(ptScroll-ptDelta);
		}
		_point ptScroll2(ptScroll);
		sptrSZ->GetScrollPos(&ptScroll2);
		m_ptLastDelta = ptScroll-ptScroll2; // для непрерывного скролла

		//sptrSZ->EnsureVisible(_client(rc, sptrSZ));
	}
}

void CAView::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if(!strcmp(pszEvent, szEventNewSettings))
    {
		_ReadParams();
		BuildView(false);
    }
	else if(!strcmp( pszEvent, szEventChangeObjectList))
	{
		if(GetKey(punkFrom) == GetKey(m_sptrObjectList) )
		{
			if(m_RenameEditBox.handle()) // если был эдитбокс - грохнем...
				::DestroyWindow( m_RenameEditBox.handle() );
			m_bPrintInited=false;
			long	lHint= *(long*)pdata;
			if(lHint == cncRemove)
			{
				m_keyExcludeIt = GetKey(punkHit);
				m_nActiveClass=-1;
				m_nActiveThumb=-1;
			}
			if(lHint == cncReset || lHint == cncAdd)
			{
				BuildView(true);
			}
			else
			{
				BuildView(false);
			}
			m_keyExcludeIt = INVALID_KEY;
			if(lHint == cncRemove)
			{
				m_nActiveClass=-1;
				m_nActiveThumb=-1;
			}
			else
			{
				long lPos;
				m_sptrObjectList->GetActiveChild( &lPos );
				if( lPos )
				{
					IUnknown* punkObject = 0;
					m_sptrObjectList->GetNextChild(&lPos, &punkObject);
					IMeasureObjectPtr sptrMO(punkObject);
					if(punkObject) punkObject->Release();
					_SetActiveClassCell(sptrMO);
				}
				else
				{
					m_nActiveClass = -1;
					m_nActiveThumb = -1;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////
//
HRESULT CAView::GetClassID( CLSID *pClassID )
{
	memcpy(pClassID, &clsidAView, sizeof(CLSID));
	return S_OK;
}

HRESULT CAView::GetFirstVisibleObjectPosition( long *plpos )
{
	if(plpos)
	{
		*plpos = 1;
		return S_OK;
	}
	else
		return E_INVALIDARG;
}

HRESULT CAView::GetNextVisibleObject( IUnknown ** ppunkObject, long *plPos )
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

HRESULT CAView::GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch )
{
	CWinViewBase::GetObjectFlags( bstrObjectType, pdwMatch );
	_bstr_t	bstr( bstrObjectType );
	if( !strcmp(bstr, szTypeObjectList))
		*pdwMatch = mvFull;

	return S_OK;
}


HRESULT CAView::GetMultiFrame(IUnknown **ppunkObject, BOOL bDragFrame)
{
	*ppunkObject = Unknown();
	if(*ppunkObject) (*ppunkObject)->AddRef();
	return S_OK;
}

///////////////////////////
//multiframe
HRESULT CAView::SelectObject( IUnknown* punkObj, BOOL bAddToExist )
{
	return S_FALSE;
}

HRESULT CAView::UnselectObject( IUnknown* punkObj )
{
	return S_FALSE;
}

HRESULT CAView::GetObjectsCount(DWORD* pnCount)
{
	if(m_nActiveClass >= 0 && m_nActiveThumb >= 0)
		*pnCount = 1;
	else
		*pnCount = 0;
	return S_OK;
}

HRESULT CAView::GetObjectByIdx(DWORD nIdx, IUnknown** ppunkObj)
//плюем на nIdx, возвращаем активный объект... :-(
{
	if(m_nActiveClass >= 0 && m_nActiveThumb >= 0)
	{
		*ppunkObj=0;
		ClassCell* pClassCell = 0;
		long lPos = m_listClassCells.head();
		long nClassCounter = 0;
		while(lPos)
		{
			pClassCell = m_listClassCells.next(lPos);
			if(m_nActiveClass == nClassCounter)
			{
				long nObjectCounter = 0;
				long lPosObj =  pClassCell->listObjects.head();
				while(lPosObj)
				{
					IUnknown* punk = pClassCell->listObjects.next(lPosObj);
					if(m_nActiveThumb == nObjectCounter)
					{
						*ppunkObj = punk;
						break;
					}
					nObjectCounter++;
				}
				break;
			}
			nClassCounter++;
		}

		if(*ppunkObj)(*ppunkObj)->AddRef();
	}
	return S_OK;
}

HRESULT CAView::GetFlags(DWORD* pnFlags)
{
	*pnFlags = 0;
	return S_OK;
}

HRESULT CAView::SetFlags(DWORD nFlags)
{
	return S_OK;
}

HRESULT CAView::GetFrameOffset(POINT* pptOffset)
{
	pptOffset->x = 0;
	pptOffset->y = 0;
	return S_OK;
}

HRESULT CAView::SetFrameOffset(POINT ptOffset)
{
	return S_OK;
}

HRESULT CAView::GetObjectOffsetInFrame(IUnknown* punkObject, POINT* pptOffset)
{
	pptOffset->x = 0;
	pptOffset->y = 0;
	return S_OK;
}

HRESULT CAView::RedrawFrame()
{
	return S_OK;
}

HRESULT CAView::EmptyFrame()
{
	return S_OK;
}

HRESULT CAView::SyncObjectsRect()
{
	return S_OK;
}


void CAView::BuildView(bool bNewObjectsAttached)
{
	_KillClassCellsList();

	m_mapClassifiedObjects.clear();

	if(m_sptrObjectList == 0 )
	{
		if( ::IsWindow( m_hwnd ) )
			InvalidateRect(m_hwnd, 0, 0);
		return;
	}

	if(bNewObjectsAttached)
		CalcCellSizeByList();

	double fZoom = 1.;
	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
		sptrSZ->GetZoom(&fZoom);

	long nCurCellSize		= (long)(fZoom*m_nCellSize		+ .5)+m_nMinBorder;
	long nCurInterCellDist	= (long)(fZoom*m_nInterCellDist + .5);
	
	long nWidthOfClassCell = 0;
	_rect rcClient = NORECT;
	GetWindowRect(m_hwnd, &rcClient);
	nWidthOfClassCell = max(rcClient.width(), nCurCellSize + 2*nCurInterCellDist);
	nWidthOfClassCell = max(nWidthOfClassCell, RESTABLE_COL_WIDTH*3);

	long nIntegralHeight = nCurInterCellDist;


	long	lClassCount = class_count();
	long nClassCounter;
	for( nClassCounter = 0; nClassCounter < lClassCount; nClassCounter++ )
	{
		ClassCell* pClassCell = 
			CalcClassCellLayout( nClassCounter, nWidthOfClassCell, fZoom, m_nActiveClass == nClassCounter, false);

		if(m_nActiveClass == nClassCounter)
			m_rcPrevSelected.delta(0, nIntegralHeight);
		
		if(pClassCell) 
		{
			nIntegralHeight += pClassCell->sizeClassCell.cy;
			pClassCell->bstrName = get_class_name( nClassCounter );
			m_listClassCells.insert(pClassCell);

			//AAM - временно (для blood) вывод в shell.data информации для бланка
			SetValue(GetAppUnknown(),"\\AView\\ClassCount", pClassCell->bstrName, pClassCell->listObjects.Count());

		}
	}

	if(m_bShowUnknown)
	{
		ClassCell* pClassCell = CalcClassCellLayout( -1, nWidthOfClassCell, fZoom, m_nActiveClass == nClassCounter, true);
		if(m_nActiveClass == nClassCounter)
			m_rcPrevSelected.delta(0, nIntegralHeight);
		if(pClassCell) 
		{
			nIntegralHeight += pClassCell->sizeClassCell.cy;
			pClassCell->bstrName = get_class_name( -1 );
				//::GetValueString(GetAppUnknown(), "\\Classes", "UnkClassName", "Unknown");
			m_listClassCells.insert(pClassCell);
		}
		nClassCounter++;
	}

	BOOL bShowResultBar = ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "ShowResultBar", 1) != 0;
	if(bShowResultBar)
	{
		nIntegralHeight += (m_nTextHeight+(m_b3D ? 4 : 2))*(nClassCounter + 1) + nCurInterCellDist; //for table
	}

	// [vanek] : устанавливаем ширину, равную ширине окна вьюхи - 15.11.2004 
	SetClientSize(_size((int)(nWidthOfClassCell/fZoom), (int)(nIntegralHeight/fZoom)));
    
	if( ::IsWindow( m_hwnd ) )
		InvalidateRect(m_hwnd, 0, 0);
}

void CAView::CalcCellSizeByList()
{
	m_nCellSize = m_nMinThumbSize;

	if(m_sptrObjectList == 0)
		return;

	long lPos = 0;
	m_sptrObjectList->GetFirstChildPosition(&lPos);
	while(lPos)
	{
		IUnknown* punkObject = 0;
		m_sptrObjectList->GetNextChild(&lPos, &punkObject);
		IMeasureObjectPtr sptrMO(punkObject);
		if(punkObject) punkObject->Release();
		if(sptrMO != 0)
		{
			IUnknown* punkImage = 0;
			sptrMO->GetImage(&punkImage);
			IImagePtr sptrImage(punkImage);
			if(punkImage) punkImage->Release();
			if(sptrImage != 0)
			{
				int nCX = 0, nCY = 0;
				sptrImage->GetSize(&nCX, &nCY);
				m_nCellSize = max(m_nCellSize, max(nCX, nCY));
			}
		}
	}
}

ClassCell* CAView::CalcClassCellLayout( long keyClass, long nWidthOfClassCell, double fZoom, bool bActive, bool bFillUnknown)
{
	long nCurCellSize		 = (long)(fZoom*m_nCellSize			+ .5)+m_nMinBorder;
	long nCurInterCellDist	 = (long)(fZoom*m_nInterCellDist	+ .5);
	long nCurThumbToTextDist = (long)(fZoom*m_nThumbToTextDist	+ .5);

	m_nWheelStep = nCurInterCellDist + (m_bDrawThumb ? nCurCellSize : 0) + (m_bDrawText ? nCurThumbToTextDist + m_nTextHeight : 0);	
	ClassCell* pClassCell = new ClassCell;

	if(!m_bDrawThumb && !m_bDrawText)
		return pClassCell;

	long nCurXPos = nCurInterCellDist;
	long nCurYPos = nCurInterCellDist;

	if(m_bHeaderAtTop)
		nCurYPos =+ m_nHeaderHeight;
	
	long nObjectsInClassCount = 0;
	bool bAddUnitHeight = false;
	long lPos = 0;
	m_sptrObjectList->GetFirstChildPosition(&lPos);
	while(lPos)
	{
		IUnknown* punkObject = 0;
		m_sptrObjectList->GetNextChild(&lPos, &punkObject);
		ICalcObjectPtr sptrCO(punkObject);
		if(punkObject) punkObject->Release();
		if(sptrCO != 0 && m_keyExcludeIt != GetKey(sptrCO))
		{
			long keyObjectClass = get_object_class( sptrCO );

			if(bFillUnknown || keyObjectClass == keyClass)
			{
				if(!bFillUnknown)
					m_mapClassifiedObjects.insert(tmapGUID2BOOL::value_type(keyObjectClass, true));
				else
				{
					tmapGUID2BOOL::iterator it;
					if((it = m_mapClassifiedObjects.find(keyObjectClass)) != m_mapClassifiedObjects.end())
						continue;

				}

				sptrCO->AddRef();
				pClassCell->listObjects.insert(sptrCO);
				POINT* pptTopLeft = new POINT;

				pptTopLeft->x = nCurXPos;
				pptTopLeft->y = nCurYPos;

				pClassCell->listTopLeftCell.insert(pptTopLeft);

				nCurXPos += nCurInterCellDist + nCurCellSize;

				bAddUnitHeight = true;

				if((nCurXPos + nCurCellSize + nCurInterCellDist) >= nWidthOfClassCell)
				{
					nCurXPos = nCurInterCellDist;
					nCurYPos += nCurInterCellDist + (m_bDrawThumb ? nCurCellSize : 0) + (m_bDrawText ? nCurThumbToTextDist + m_nTextHeight : 0);
					bAddUnitHeight = false;
				}

				if(bActive && nObjectsInClassCount == m_nActiveThumb)
				{
					m_rcPrevSelected.left   = pptTopLeft->x;
					m_rcPrevSelected.top    = pptTopLeft->y;
					m_rcPrevSelected.right  = m_rcPrevSelected.left + nCurCellSize;
					m_rcPrevSelected.bottom = m_rcPrevSelected.top + nCurCellSize + nCurThumbToTextDist + m_nTextHeight;
				}

				nObjectsInClassCount++;

			}
		}

	}

	pClassCell->sizeClassCell.cy = nCurYPos + nCurInterCellDist;
	if(bAddUnitHeight)
	{
		pClassCell->sizeClassCell.cy +=  (m_bDrawThumb ? nCurCellSize : 0) + (m_bDrawText ? nCurThumbToTextDist + m_nTextHeight : 0);
	}

	if(!nObjectsInClassCount)
	{
		//for "no objects" inscription
		pClassCell->sizeClassCell.cy +=  nCurThumbToTextDist + m_nTextHeight;
	}

	if(!m_bHeaderAtTop)
		pClassCell->sizeClassCell.cy += m_nHeaderHeight;
	
	return pClassCell;
}

void CAView::_KillClassCellsList()
{
	long lPos = m_listClassCells.head();
	while(lPos)
		delete  m_listClassCells.next(lPos);
	
	m_listClassCells.deinit();
}

void CAView::SetClientSize(SIZE size)
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

void CAView::GetClientSize(SIZE& size)
{
	if(!::IsWindow(m_hwnd))
		return;

	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
		sptrSZ->GetClientSize(&size);
}

void CAView::DrawClassCell(HDC hdc, HBITMAP hDIBSection, _rect rectPaint, _point ptScroll, double fZoom, ClassCell* pClassCell, long& nCurHeightPos, bool bActiveClass)
{
	if(!pClassCell) return;

	pClassCell->m_rcLastDrawRect.top=nCurHeightPos;
	pClassCell->m_rcLastDrawRect.left=0;
	
	long nCurCellSize		 = (long)(fZoom*m_nCellSize			+ .5)+m_nMinBorder;
	long nCurInterCellDist	 = (long)(fZoom*m_nInterCellDist	+ .5);
	long nCurThumbToTextDist = (long)(fZoom*m_nThumbToTextDist	+ .5);
	
	_size sizeDoc(0, 0); 
	GetClientSize(sizeDoc);
	
	rectPaint.delta(ptScroll.x, ptScroll.y);

	HDC hdcMem = ::CreateCompatibleDC(hdc);
	HBITMAP hBitmapOld = (HBITMAP)::SelectObject(hdcMem, hDIBSection);
	::SetMapMode(hdcMem, MM_TEXT);
	::SetViewportOrgEx(hdcMem, -rectPaint.left, -rectPaint.top, NULL);

	HFONT hFontHeader = ::CreateFontIndirect(&m_fontClassCellsHeader);
	HFONT hFontText	  = ::CreateFontIndirect(&m_fontText);

	HFONT hfontOld = (HFONT)::SelectObject(hdcMem, hFontHeader);

	
	if(m_bHeaderAtTop)
	{
		//draw header
		::SetTextColor(hdcMem, m_clrClassCellsHeader);
		::SetBkColor  (hdcMem, m_clrInactive);
		::SetBkMode(hdc, TRANSPARENT);
		::DrawText(hdcMem, (char*)pClassCell->bstrName, lstrlen((char*)pClassCell->bstrName), &_rect(0, nCurHeightPos, sizeDoc.cx*fZoom, nCurHeightPos+m_nHeaderHeight), DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);
		::SetBkMode(hdc, OPAQUE);
	}

	long nPosTLC = pClassCell->listTopLeftCell.head();
	long nPosObj = pClassCell->listObjects.head();

	long nThumbsCounter = 0;

	::SelectObject(hdcMem, hFontText);
	
	bool bHeaderDrawed = false;
	while(nPosTLC)
	{
		dbg_assert(nPosObj != 0);
		_point pt(*(pClassCell->listTopLeftCell.next(nPosTLC)));
		pt.y += nCurHeightPos;
		IUnknown* punkObject = pClassCell->listObjects.next(nPosObj);

		bool bActive = (nThumbsCounter == m_nActiveThumb)&&bActiveClass;

		if(punkObject != m_sptrDragObj)
		{
			_DrawUnit(hdcMem, punkObject, bActive, pt, false, fZoom);
		}
		
		nThumbsCounter++;
	}

	nCurHeightPos += pClassCell->sizeClassCell.cy;
	pClassCell->m_rcLastDrawRect.bottom=nCurHeightPos;
	pClassCell->m_rcLastDrawRect.right=10000;

	if(!nThumbsCounter)
	{
		_rect rcNoObjectText(10, 0, sizeDoc.cx, nCurHeightPos);
		if(!m_bHeaderAtTop)
			rcNoObjectText.bottom = nCurHeightPos - m_nHeaderHeight;
		else
			rcNoObjectText.bottom = nCurHeightPos - nCurInterCellDist;

		rcNoObjectText.top = rcNoObjectText.bottom - m_nTextHeight;
		
		::SetTextColor(hdcMem, m_clrInactiveText);
		::SetBkColor  (hdcMem, m_clrInactive);
		::SelectObject(hdcMem, hFontText);

		char szNoObject[256];	szNoObject[0] = 0;	
		::LoadString( App::handle(), IDS_NOOBJECTS, szNoObject, sizeof(szNoObject) );
		::DrawText(hdcMem, szNoObject, lstrlen(szNoObject), &rcNoObjectText, DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);
	}

	if(!m_bHeaderAtTop)
	{
		//draw header
		::SetTextColor(hdcMem, m_clrClassCellsHeader);
		::SetBkColor  (hdcMem, m_clrInactive);
		::SelectObject(hdcMem, hFontHeader);
		::DrawText(hdcMem, (char*)pClassCell->bstrName, lstrlen((char*)pClassCell->bstrName), &_rect(10, nCurHeightPos-m_nHeaderHeight, sizeDoc.cx*fZoom, nCurHeightPos), DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);
	}

	/*
	if(m_sptrDragObj!=0)
	{
		_DrawUnit(hdcMem, m_sptrDragObj, true, m_ptDragPos, false, fZoom);
	}
	*/

	::SelectObject(hdcMem, hfontOld);
	::SelectObject(hdcMem, hBitmapOld);
	::DeleteDC(hdcMem);

	::DeleteObject(hFontHeader);
	::DeleteObject(hFontText);
	
}

void CAView::DrawDragObj(HDC hdc, HBITMAP hDIBSection,
						 BITMAPINFOHEADER bmih, byte *pdibBits,
						 _rect rectPaint, _point ptScroll, double fZoom)
{
	long nCurCellSize		 = (long)(fZoom*m_nCellSize			+ .5)+m_nMinBorder;
	long nCurInterCellDist	 = (long)(fZoom*m_nInterCellDist	+ .5);
	long nCurThumbToTextDist = (long)(fZoom*m_nThumbToTextDist	+ .5);
	
	_size sizeDoc(0, 0); 
	GetClientSize(sizeDoc);
	
	rectPaint.delta(ptScroll.x, ptScroll.y);

	HDC hdcMem = ::CreateCompatibleDC(hdc);
	HBITMAP hBitmapOld = (HBITMAP)::SelectObject(hdcMem, hDIBSection);
	::SetMapMode(hdcMem, MM_TEXT);
	::SetViewportOrgEx(hdcMem, -rectPaint.left, -rectPaint.top, NULL);

	HFONT hFontHeader = ::CreateFontIndirect(&m_fontClassCellsHeader);
	HFONT hFontText	  = ::CreateFontIndirect(&m_fontText);

	HFONT hfontOld = (HFONT)::SelectObject(hdcMem, hFontHeader);

	::SelectObject(hdcMem, hFontText);

	if(m_sptrDragObj!=0)
	{
		_rect rc(m_ptDragPos,m_rcPrevSelected.size());
		rc.top=max(rc.top,rectPaint.top);
		rc.left=max(rc.left,rectPaint.left);
		rc.bottom=min(rc.bottom,rectPaint.bottom);
		rc.right=min(rc.right,rectPaint.right);

		_DrawUnitTransparent(hdcMem,
			bmih, pdibBits,
			m_sptrDragObj, true, m_ptDragPos, false, fZoom, rc);
	}

	::SelectObject(hdcMem, hfontOld);
	::SelectObject(hdcMem, hBitmapOld);
	::DeleteDC(hdcMem);

	::DeleteObject(hFontHeader);
	::DeleteObject(hFontText);
}

void CAView::_ReadParams()
{
	strcpy((char*)m_fontClassCellsHeader.lfFaceName, (char*)::GetValueString(GetAppUnknown(), szApportionmentSectName, "HeaderFont", "Arial"));
	m_fontClassCellsHeader.lfHeight = ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "HeaderFont_Height", 40);	
	m_fontClassCellsHeader.lfWidth = 0;
	m_fontClassCellsHeader.lfEscapement = 0;
	m_fontClassCellsHeader.lfOrientation = 0;
	m_fontClassCellsHeader.lfWeight = ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "HeaderFont_Weight", FW_BOLD);
	m_fontClassCellsHeader.lfItalic = (byte)::GetValueInt(GetAppUnknown(), szApportionmentSectName, "HeaderFont_Italic", 1);
	m_fontClassCellsHeader.lfUnderline = (byte)::GetValueInt(GetAppUnknown(), szApportionmentSectName, "HeaderFont_Underline", 0);
	m_fontClassCellsHeader.lfStrikeOut = FALSE;
	m_fontClassCellsHeader.lfCharSet = DEFAULT_CHARSET;
	byte family = FF_DONTCARE;
	family = family << 4;
	family |= DEFAULT_PITCH;
	m_fontClassCellsHeader.lfPitchAndFamily = family;
	m_fontClassCellsHeader.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_fontClassCellsHeader.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_fontClassCellsHeader.lfQuality = DEFAULT_QUALITY;

	m_nHeaderHeight = _GetFontHeight(&m_fontClassCellsHeader);
	
	m_clrClassCellsHeader = ::GetValueColor(GetAppUnknown(), szApportionmentSectName, "HeaderFont_Color", RGB(255, 0, 0));

	m_nInterCellDist	= ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "InterCellDist", 20);

	m_nThumbToTextDist	= ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "ThumbToTextDist", 5);

	m_bDrawThumb = ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "DrawThumb", 1) == 1;

	m_bDrawText = ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "DrawText", 1 ) == 1;

	m_bHeaderAtTop = ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "HeaderAtTop", 1 ) == 1;

	strcpy((char*)m_fontText.lfFaceName, (char*)::GetValueString(GetAppUnknown(), szApportionmentSectName, "TextFont", "Arial"));
	m_fontText.lfHeight = ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "TextFont_Height", 16);
	m_fontText.lfWidth = 0;
	m_fontText.lfEscapement = 0;
	m_fontText.lfOrientation = 0;
	m_fontText.lfWeight = ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "TextFont_Weight", FW_NORMAL);
	m_fontText.lfItalic = (byte)::GetValueInt(GetAppUnknown(), szApportionmentSectName, "TextFont_Italic", 0);
	m_fontText.lfUnderline = (byte)::GetValueInt(GetAppUnknown(), szApportionmentSectName, "TextFont_Underline", 0);
	m_fontText.lfStrikeOut = FALSE;
	m_fontText.lfCharSet = DEFAULT_CHARSET;
	m_fontText.lfPitchAndFamily = family;
	m_fontText.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_fontText.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_fontText.lfQuality = DEFAULT_QUALITY;

	m_nTextHeight = _GetFontHeight(&m_fontText);


	m_clrActive			= ::GetValueColor(GetAppUnknown(), szApportionmentSectName, "ActiveThumbnailColor",		::GetSysColor(COLOR_HIGHLIGHT));
	m_clrInactive		= ::GetValueColor(GetAppUnknown(), szApportionmentSectName, "InactiveThumbnailColor",	::GetSysColor(COLOR_BTNFACE));
	m_clrPrint			= ::GetValueColor(GetAppUnknown(), szApportionmentSectName, "PrintThumbnailColor",	::GetSysColor(COLOR_BTNFACE));
	m_clrActiveText		= ::GetValueColor(GetAppUnknown(), szApportionmentSectName, "ActiveTextColor",			::GetSysColor(COLOR_HIGHLIGHTTEXT));
	m_clrInactiveText	= ::GetValueColor(GetAppUnknown(), szApportionmentSectName, "InactiveTextColor",		::GetSysColor(COLOR_WINDOWTEXT));

	m_bDrawEdge			= ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "DrawEdge", 1) == 1;
	m_bDrawEdgeOnPrint	= ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "DrawEdgeOnPrint", 1) == 1;
	m_b3D				= ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "3D", 1) == 1;
	m_b3DOnPrint		= ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "3DOnPrint", 1) == 1;

	m_nMinThumbSize		= ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "MinThumbSize", 100);

	m_nMinBorder		= ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "MinBorder", 4);

	m_nMaxPrintResolution = ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "MaxPrintResolution", 100);

	m_bShowUnknown = ::GetValueInt( ::GetAppUnknown(), szApportionmentSectName, "ShowUnknown", m_bShowUnknown?1:0 ) != 0;
	m_nHighlightPhases = :: GetValueInt( ::GetAppUnknown(), szApportionmentSectName, "HighlightPhases", m_nHighlightPhases );
	m_nHighlightPercent = :: GetValueInt( ::GetAppUnknown(), szApportionmentSectName, "HighlightPercent", m_nHighlightPercent );

	//mouse wheel
	m_bUseMouseWheel = ::GetValueInt( ::GetAppUnknown(), MOUSE_SECTION, MOUSE_WHEEL_ENABLE, 1 ) != 0;

	m_wheelHorzSwitch = 0;
	_bstr_t str = ::GetValueString( ::GetAppUnknown(), MOUSE_SECTION, MOUSE_WHEEL_HORZ_KEY, "Ctrl" );
	if( stricmp(str, "Ctrl") == 0 ) 
		m_wheelHorzSwitch = MK_CONTROL;
	else if( stricmp(str, "Shift") == 0 )
		m_wheelHorzSwitch = MK_SHIFT;
	
	INamedPropBagPtr bag(Unknown());
	if(bag!=0)
	{
		long nShowBackground = ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "ShowBackground", 0);
		bag->SetProperty(_bstr_t("Object.ShowBackground"),_variant_t(nShowBackground));
		long nHighlightMask = ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "HighlightMask", 0);
		bag->SetProperty(_bstr_t("Object.HighlightMask"),_variant_t(nShowBackground));
	}

	_InitBinaryColorsTable();
}

long CAView::_GetFontHeight(LOGFONT* pFont)
{
	HDC hdc = CreateCompatibleDC(0);
	HFONT hFont = CreateFontIndirect(pFont);
	HFONT hOldFont = (HFONT)::SelectObject(hdc, hFont);
	_rect rcRecalc = NORECT;
	LPCTSTR szStr = "GetFontHeight";
	DrawText(hdc, szStr, lstrlen(szStr), &rcRecalc, DT_CALCRECT);
	::SelectObject(hdc, hOldFont);
	DeleteObject(hFont);
	DeleteDC(hdc);
	return rcRecalc.height() + 1;
}

void CAView::_SetActiveClassCell(IUnknown *punkObject)
{
	_rect rcCell = NORECT;
	if(m_sptrObjectList==0) return;

	double fZoom = 1.;
	_point ptScroll(0, 0);
	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
	{
		sptrSZ->GetZoom(&fZoom);
		sptrSZ->GetScrollPos(&ptScroll);
	}

	long nCurCellSize		 = (long)(fZoom*m_nCellSize			+ .5)+m_nMinBorder;
	long nCurThumbToTextDist = (long)(fZoom*m_nThumbToTextDist	+ .5);
	long nCurHeightPos = (long)(m_nInterCellDist*fZoom + .5);


	ICalcObjectPtr sptrCO2(punkObject);
	//scanning class cells
	bool bFound=false;

	long nClassCounter=0;
	long lPosClass = m_listClassCells.head();

	while(lPosClass)
	{
		ClassCell* pClassCell = pClassCell = m_listClassCells.next(lPosClass);
		{
			long nThumbCounter = 0;
			long lPosObj =  pClassCell->listObjects.head();
			while(lPosObj)
			{
				IUnknown* punk = pClassCell->listObjects.next(lPosObj);
				ICalcObjectPtr sptrCO1(punk);
				if(sptrCO1==sptrCO2)
				{
					m_nActiveThumb = nThumbCounter;
					bFound=true;

					// Ensure visible
					long lPosTLC = pClassCell->listTopLeftCell.head();
					_rect rcCell = NORECT;
					long nThumbCounter2 = 0;
					while(lPosTLC)
					{
						_point pt(*(pClassCell->listTopLeftCell.next(lPosTLC)));
						if(nThumbCounter2==nThumbCounter)
						{
							pt.y += nCurHeightPos;
							_rect rcThumb=NORECT, rcText=NORECT;
							if(m_bDrawThumb)
								rcThumb = _rect(pt.x, pt.y, pt.x + nCurCellSize, pt.y + nCurCellSize);
							if(m_bDrawText)
								rcText  = _rect(pt.x, pt.y + (m_bDrawThumb ? nCurCellSize + nCurThumbToTextDist : 0), pt.x + nCurCellSize, pt.y + (m_bDrawThumb ? nCurCellSize + nCurThumbToTextDist : 0) + m_nTextHeight);
							rcCell=_rect(pt.x, pt.y, pt.x, pt.y);
							if(m_bDrawText) rcCell.merge(rcText);
							if(m_bDrawThumb) rcCell.merge(rcThumb);
						}
						nThumbCounter2++;
					}
					if(sptrSZ != 0) 
					{
						_rect rcClient = NORECT;
						GetClientRect(hwnd(),  &rcClient);
						rcClient.delta(ptScroll.x, ptScroll.y);
						
						if(rcClient.top > rcCell.top || rcClient.bottom <= rcCell.bottom)
						{				
							if( rcClient.top > rcCell.top )ptScroll.y-=rcClient.top - rcCell.top;
							if( rcClient.bottom < rcCell.bottom )ptScroll.y-=rcClient.bottom - rcCell.bottom-1;
							sptrSZ->SetScrollPos(ptScroll);
						}	

						//sptrSZ->EnsureVisible(_client(rc, sptrSZ));
					}

					break;
				}
				nThumbCounter++;
			}
			if(bFound)
			{
				m_nActiveClass = nClassCounter;
				break;
			}
		}
		nCurHeightPos += pClassCell->sizeClassCell.cy;
		nClassCounter++;
	}

	return;
}

_rect CAView::_CalcActiveClassCell(int x, int y)
{
	_rect rcCell = NORECT;

	if(m_sptrObjectList==0) return rcCell;

	double fZoom = 1.;
	_point ptScroll(0, 0);
	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
	{
		sptrSZ->GetZoom(&fZoom);
		sptrSZ->GetScrollPos(&ptScroll);
	}

	x += ptScroll.x;
	y += ptScroll.y;

	long nCurCellSize		 = (long)(fZoom*m_nCellSize			+ .5)+m_nMinBorder;
	long nCurThumbToTextDist = (long)(fZoom*m_nThumbToTextDist	+ .5);
	
	//drawing class cells
	long nCurHeightPos = (long)(m_nInterCellDist*fZoom + .5);
	ClassCell* pClassCell = 0;
	long lPos = m_listClassCells.head();
	m_nActiveClass = 0;
	m_nActiveThumb = -1;
	while(lPos)
	{
		if(nCurHeightPos > y + ptScroll.y)
			break;
		
		pClassCell = m_listClassCells.next(lPos);

		if(y > nCurHeightPos && y < nCurHeightPos + pClassCell->sizeClassCell.cy)
		{
			long lPosTLC = pClassCell->listTopLeftCell.head();
			long nThumbCounter = 0;
			while(lPosTLC)
			{
				_point pt(*(pClassCell->listTopLeftCell.next(lPosTLC)));
				pt.y += nCurHeightPos;
				_rect rcThumb = NORECT;
				_rect rcText  = NORECT;
				if(m_bDrawThumb)
					rcThumb = _rect(pt.x, pt.y, pt.x + nCurCellSize, pt.y + nCurCellSize);
				if(m_bDrawText)
					rcText  = _rect(pt.x, pt.y + (m_bDrawThumb ? nCurCellSize + nCurThumbToTextDist : 0), pt.x + nCurCellSize, pt.y + (m_bDrawThumb ? nCurCellSize + nCurThumbToTextDist : 0) + m_nTextHeight);
					

				if((x > rcThumb.left && x <= rcThumb.right && y > rcThumb.top && y <= rcThumb.bottom) ||
				   (x > rcText. left && x <= rcText. right && y > rcText. top && y <= rcText. bottom))
				{
					//AAM: bug fixed - если запрещен text, увеличивали rect вверх до 0
					rcCell=_rect(pt.x, pt.y, pt.x, pt.y);
					if(m_bDrawText) rcCell.merge(rcText);
					if(m_bDrawThumb) rcCell.merge(rcThumb);
					
					m_nActiveThumb = nThumbCounter;
					break;
				}

				nThumbCounter++;
			}
			break;
		}
		m_nActiveClass++;

		nCurHeightPos += pClassCell->sizeClassCell.cy;
	}

	{
		IUnknown *punkObject=0;
		HRESULT r=GetObjectByIdx(0, &punkObject);
		ICalcObjectPtr sptrCO1(punkObject);
		if(punkObject) punkObject->Release();

		if(sptrCO1)
		{
			m_sptrObjectList->GetFirstChildPosition(&lPos);
			while(lPos)
			{
				IUnknown* punkObject = 0;
				long lPos1=lPos;
				m_sptrObjectList->GetNextChild(&lPos, &punkObject);
				ICalcObjectPtr sptrCO2(punkObject);
				if(punkObject) punkObject->Release();
				if(sptrCO1==sptrCO2)
				{
					m_sptrObjectList->SetActiveChild( lPos1 );
				}
			}
		}
		else
		{
			m_sptrObjectList->SetActiveChild( 0 );
		}
	}

	return rcCell;
}

void CAView::_DrawTablText(HDC hdc, _rect rcText, _bstr_t bstrText, bool bFirstRow, bool b3D)
{
	HBRUSH hbr	= ::CreateSolidBrush(m_clrInactive);

	if(b3D)
		::DrawEdge(hdc, &rcText, bFirstRow ? BDR_RAISEDINNER : BDR_SUNKENOUTER, BF_RECT);
	else
		::Rectangle(hdc, rcText.left, rcText.top, rcText.right, rcText.bottom);
	rcText.inflate(m_b3D ? -2 : -1);

	::FillRect(hdc, &rcText, hbr);

	::DrawText(hdc, (char*)bstrText, lstrlen((char*)bstrText), &rcText, DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);

	::DeleteObject(hbr);
}

HRESULT CAView::GetPrintWidth( int nMaxWidth, int *pnReturnWidth, BOOL *pbContinue, int nUserPosX, int *pnNewUserPosX )
{

	long lPos = m_listClassCells.head();
	long nMaxCount = 0;
	while(lPos)
	{
		ClassCell *pClassCell = m_listClassCells.next(lPos);
		nMaxCount = max(nMaxCount,pClassCell->listObjects.Count());
	}

	double fZoom = 1.;	
	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
		sptrSZ->GetZoom( &fZoom );

	long nCurCellSize		 = (long)(fZoom*m_nCellSize		   + .5);
	long nCurInterCellDist	 = (long)(fZoom*m_nInterCellDist   + .5);
	long nCurThumbToTextDist = (long)(fZoom*m_nThumbToTextDist + .5);

	int nMaxWidth2 = nCurInterCellDist + (nCurCellSize+nCurInterCellDist)*nMaxCount;
	nMaxWidth2 = max(nMaxWidth2,600); //на всякий случай - чтобы надписи влезали
	nMaxWidth2 = min(nMaxWidth, nMaxWidth2);

	*pnNewUserPosX = *pnReturnWidth = nMaxWidth2;
	*pbContinue = FALSE;
	m_nPrintWidth = nMaxWidth2;
	BuildPrint(nMaxWidth);
	return S_OK;
}

HRESULT CAView::GetPrintHeight( int nMaxHeight, int *pnReturnHeight, BOOL *pbContinue, int nUserPosY, int *pnNewUserPosY )
{	

	*pbContinue = FALSE;

	if(!m_bPrintInited)
		return S_FALSE;

	ClassCell* pClassCell = 0;
	long lPos = m_listClassCells.head();
	long nCurIntegralHeight = 0;
	long nProcessedRows = 0;
	bool bPureTablePaper = true;
	while(lPos)
	{
		pClassCell = m_listClassCells.next(lPos);
		
		long lPosRH = pClassCell->listRowHeights.head();
		while(lPosRH)
		{
			long nRowHeight = *(pClassCell->listRowHeights.next(lPosRH));
			if(nRowHeight>nMaxHeight) nRowHeight=nMaxHeight; // затычка от слишком большого масштаба
			if(nProcessedRows < nUserPosY)
			{
				nProcessedRows++;
				continue;
			}

			bPureTablePaper = false;

			nCurIntegralHeight += nRowHeight;

			nProcessedRows++;

			if(nCurIntegralHeight > nMaxHeight)
			{
				*pnReturnHeight = nCurIntegralHeight - nRowHeight;
				*pnNewUserPosY = nProcessedRows - 1;
				*pbContinue = TRUE;
				return S_OK;
			}	
		}
	}

	long nTableHeight = (m_listClassCells.Count()+2)*(m_nTextHeight + (m_b3DOnPrint?4:2));

	BOOL bShowResultBar = ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "ShowResultBar", 1) != 0;
	if(!bShowResultBar) nTableHeight=0;

	if(!bPureTablePaper)
	{
		if(nCurIntegralHeight + nTableHeight <= nMaxHeight)
		{
			*pnReturnHeight = nCurIntegralHeight + nTableHeight;
			*pnNewUserPosY = nProcessedRows + m_listClassCells.Count() + 2;
			*pbContinue = FALSE;
		}
		else
		{
			*pnReturnHeight = nCurIntegralHeight;
			*pnNewUserPosY = nProcessedRows;
			*pbContinue = TRUE;
		}
	}
	else
	{
		long nTableRows = nMaxHeight/(m_nTextHeight + (m_b3DOnPrint?4:2));
		long nCurTableRow = nUserPosY - nProcessedRows;

		if(nCurTableRow +  nTableRows - 1 < m_listClassCells.Count() + 2)
		{
			*pnReturnHeight = nTableRows*(m_nTextHeight + (m_b3DOnPrint?4:2));
			*pnNewUserPosY = nUserPosY +  nTableRows;
			*pbContinue = TRUE;
		}
		else
		{
			*pnReturnHeight = (m_listClassCells.Count() + 2 - nCurTableRow)*(m_nTextHeight + (m_b3DOnPrint?4:2));
			*pnNewUserPosY = nUserPosY + m_listClassCells.Count() + 2 - nCurTableRow;
			*pbContinue = FALSE;
		}
	}

	return S_OK;
}

HRESULT CAView::Print( HDC hdc, RECT rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, BOOL bUseScrollZoom, DWORD dwFlags  )
{
	if(!m_bPrintInited)
	{
		return S_FALSE;
	}

	BuildPrint(m_nPrintWidth);

	double fZoom = 1.;	
	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
		sptrSZ->GetZoom( &fZoom );

	long nCurCellSize		 = (long)(fZoom*m_nCellSize		   + .5);
	long nCurInterCellDist	 = (long)(fZoom*m_nInterCellDist   + .5);
	long nCurThumbToTextDist = (long)(fZoom*m_nThumbToTextDist + .5);

	long nHeight = 0;

	ClassCell* pClassCell = 0;
	long lPos = m_listClassCells.head();
	long nCurIntegralHeight = 0;
	long nProcessedRows = 0;
	long nRowCounter = 0;
	long nTotalCellRows = 0;
	while(lPos)
	{
		pClassCell = m_listClassCells.next(lPos);

		nTotalCellRows += pClassCell->listRowHeights.Count();

		long nRowPos = pClassCell->listRowHeights.head();
		while(nRowPos)
		{
			long nCurHeight = *(pClassCell->listRowHeights.next(nRowPos));

			if(nRowCounter >= nUserPosY)
			{
				if(nRowCounter < nUserPosY + nUserPosDY)
				{
					nHeight += nCurHeight;
				}
				else
					break;
			}
			nRowCounter++;
		}
	}

	long nTableRows = nUserPosY + nUserPosDY - nTotalCellRows;
	if(nTableRows > 0)
		nHeight += nTableRows*(m_nTextHeight + (m_b3DOnPrint?4:2));


	_rect rectPaint(nUserPosX, 0, nUserPosX+nUserPosDX, nHeight);
	if(rectPaint == NORECT)
		return S_OK;

	BITMAPINFOHEADER	bmih;
	ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
	bmih.biBitCount = 24;
	bmih.biHeight	= rectPaint.height();
	bmih.biWidth	= rectPaint.width();
	bmih.biSize		= sizeof(BITMAPINFOHEADER);
	bmih.biPlanes	= 1;
	
	//create a DIB section
	byte	*pdibBits = 0;
	HBITMAP	hDIBSection = ::CreateDIBSection(hdc, (BITMAPINFO*)&bmih, DIB_RGB_COLORS, (void**)&pdibBits, 0, 0);
		
	//set background
	for(int y = 0; y < bmih.biHeight; y++)
	{
		int nRowOffset = (bmih.biWidth*3+3)/4*4*y;
		for(int x = 0; x < bmih.biWidth; x++)
		{
			pdibBits[nRowOffset + 3*x + 2] = GetRValue(m_clrPrint);
			pdibBits[nRowOffset + 3*x + 1] = GetGValue(m_clrPrint);
			pdibBits[nRowOffset + 3*x + 0] = GetBValue(m_clrPrint);
		}
	}

	HDC hdcMem = hdc; //!!
	//!!HDC hdcMem = ::CreateCompatibleDC(hdc);
	//!!HBITMAP hBitmapOld = (HBITMAP)::SelectObject(hdcMem, hDIBSection);
	HFONT hFontHeader = ::CreateFontIndirect(&m_fontClassCellsHeader);
	HFONT hFontText	  = ::CreateFontIndirect(&m_fontText);
	HFONT hfontOld = (HFONT)::SelectObject(hdcMem, hFontHeader);

	//::SetMapMode(hdcMem, MM_TEXT);
	::SetMapMode(hdcMem, MM_ISOTROPIC);
	SIZE sizePrt,sizePrt1;
	
	::SetWindowExtEx(hdcMem, 
		nUserPosDX,nUserPosDX,
		//rectTarget.right-rectTarget.left,rectTarget.bottom-rectTarget.top,
		//96,96,
		&sizePrt1); 

	::SetViewportExtEx(hdcMem,
		rectTarget.right-rectTarget.left,rectTarget.right-rectTarget.left,
		//nUserPosDX,nUserPosDY,
		//GetDeviceCaps(hdcMem, LOGPIXELSX), GetDeviceCaps(hdcMem, LOGPIXELSY),
		&sizePrt);
	
	::SetViewportOrgEx(hdcMem, rectTarget.left, rectTarget.top, NULL);

	::SetTextColor(hdcMem, m_clrInactiveText);
	::SetBkColor  (hdcMem, m_clrPrint);

	//print stuff
	pClassCell = 0;
	lPos = m_listClassCells.head();
	nRowCounter = 0;
	long nY = 0;
	while(lPos)
	{
		pClassCell = m_listClassCells.next(lPos);
		long lPosRH = pClassCell->listRowHeights.head();

		if(!pClassCell->listObjects.Count())
		{
			if(nRowCounter >= nUserPosY && nRowCounter < nUserPosY+nUserPosDY)
			{
				long nY0=nY;
				nY += nCurInterCellDist;
				::SetTextColor(hdcMem, m_clrClassCellsHeader);
				::SetBkColor  (hdcMem, m_clrPrint);
				::SetBkMode(hdc, TRANSPARENT);
				::SelectObject(hdcMem, hFontHeader);
				::DrawText(hdcMem, (char*)pClassCell->bstrName, lstrlen((char*)pClassCell->bstrName), &_rect(0, nY, nUserPosDX, nY+m_nHeaderHeight), DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);
				nY += m_nHeaderHeight + nCurThumbToTextDist;
				::SetTextColor(hdcMem, m_clrInactiveText);
				::SelectObject(hdcMem, hFontText);
				char szNoObject[256];	szNoObject[0] = 0;	
				::LoadString( App::handle(), IDS_NOOBJECTS, szNoObject, sizeof(szNoObject) );	
				::DrawText(hdcMem, szNoObject, lstrlen(szNoObject), &_rect(0, nY, nUserPosDX, nY+m_nTextHeight), DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);
				::SetBkMode(hdc, OPAQUE);
				nY += m_nTextHeight;
				if(lPosRH) 
					nY = nY0 + *(pClassCell->listRowHeights.next(lPosRH));
			}
		}
		else
		{
			long nY0=nY;
			long nPosObj = pClassCell->listObjects.head();
			long nObjCounter = 0;
			long nX = 0;
			long nPrevRow = -1;
			bool bAddRowHeight = false;
			while(nPosObj)
			{
				IUnknown* punkObject = pClassCell->listObjects.next(nPosObj);
				long nRow = nObjCounter/m_nObjectsInRow;
				nObjCounter++;
				if(nRow != nPrevRow)
				{
					if(nX != 0)
					{
						nY += nCurCellSize + nCurThumbToTextDist + m_nTextHeight;
						if(lPosRH) 
							nY = nY0 = nY0 + *(pClassCell->listRowHeights.next(lPosRH));
					}
					else
					{
						if(nRow!=0)
							pClassCell->listRowHeights.next(lPosRH);
					}
					nPrevRow = nRow;
					nX = 0;
					bAddRowHeight = false;
				}
				if(nRowCounter+nRow >= nUserPosY && nRowCounter+nRow < nUserPosY+nUserPosDY)
				{
					bAddRowHeight = true;
					if(nX == 0)
					{
						if(nRow == 0)
						{
							nY += nCurInterCellDist;
							::SetTextColor(hdcMem, m_clrClassCellsHeader);
							::SetBkColor  (hdcMem, m_clrPrint);
							::SetBkMode(hdc, TRANSPARENT);
							::SelectObject(hdcMem, hFontHeader);
							::DrawText(hdcMem, (char*)pClassCell->bstrName, lstrlen((char*)pClassCell->bstrName), &_rect(0, nY, nUserPosDX, nY+m_nHeaderHeight), DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);
							::SetBkMode(hdc, OPAQUE);
							nY += m_nHeaderHeight;
						}
						else
							nY += nCurInterCellDist;
					}

					::SelectObject(hdcMem, hFontText);
					nX += nCurInterCellDist;
					_DrawUnit(hdcMem, punkObject, false, _point(nX, nY), true, fZoom );
					nX += nCurCellSize;
				}
				else
				{
				}
			}
			if(bAddRowHeight)
			{
				nY += nCurCellSize + nCurThumbToTextDist + m_nTextHeight;
				if(lPosRH) 
					nY = nY0 = nY0 + *(pClassCell->listRowHeights.next(lPosRH));
			}
		}

		nRowCounter += pClassCell->listRowHeights.Count();
	}


	//draw table
	if(nTableRows > 0)
	{
		long nFromRow = max(0, nUserPosY - nTotalCellRows);
		_DrawResultBar(hdcMem, nY, 1., true, nFromRow, nTableRows);
	}
	

	//paint DIB to DC
	//!!::DrawDibDraw(g_DrawDibInstance, hdc, rectTarget.left, rectTarget.top, rectTarget.right-rectTarget.left, rectTarget.bottom-rectTarget.top, &bmih, pdibBits, 0, 0, rectPaint.width(), rectPaint.height(), 0);

	::DeleteObject(hDIBSection);
	::DeleteObject(hFontHeader);
	::DeleteObject(hFontText);
	::SelectObject(hdcMem, hfontOld);
	//!!::SelectObject(hdcMem, hBitmapOld);
	//!!::DeleteDC(hdcMem);
	return S_OK;
}

void CAView::BuildPrint(long nMaxWidth)
{
	//m_nCellSize *= ::GetValueDouble( GetAppUnknown(), "\\AView", "PrintZoom", 1 );
	double fZoom = 1.;	
	IScrollZoomSitePtr sptrSZ(Unknown());
	if(sptrSZ != 0) 
		sptrSZ->GetZoom( &fZoom );

	long nCurCellSize		 = (long)(fZoom*m_nCellSize		   + .5);
	long nCurInterCellDist	 = (long)(fZoom*m_nInterCellDist   + .5);
	long nCurThumbToTextDist = (long)(fZoom*m_nThumbToTextDist + .5);

	
	//m_nCellSize *= fZoom;

	m_bPrintInited = true;
	ClassCell* pClassCell = 0;
	long lPos = m_listClassCells.head();
	while(lPos)
	{
		pClassCell = m_listClassCells.next(lPos);

		long lPosRH = pClassCell->listRowHeights.head();
		while(lPosRH)
			delete pClassCell->listRowHeights.next(lPosRH);
		pClassCell->listRowHeights.deinit();

		long nObjectsInClass = pClassCell->listObjects.Count();
		if(!nObjectsInClass)
		{
			pClassCell->listRowHeights.insert(new long(m_nHeaderHeight + nCurThumbToTextDist + m_nTextHeight + 2*nCurInterCellDist));
		}
		else
		{
			m_nObjectsInRow = (nMaxWidth-nCurInterCellDist)/(nCurCellSize+nCurInterCellDist);
			if( m_nObjectsInRow < 1 )m_nObjectsInRow = 1;

			long nRows = nObjectsInClass/m_nObjectsInRow + (nObjectsInClass%m_nObjectsInRow ? 1 : 0);
			for(long i = 0; i < nRows; i++)
				pClassCell->listRowHeights.insert(new long(
				(i==0 ? m_nHeaderHeight + nCurInterCellDist: 0) +
				2*nCurInterCellDist + nCurCellSize +
				(m_bDrawText ? nCurThumbToTextDist + m_nTextHeight : 0)
				));
		}
		
	}
}

bool CAView::_DrawUnitTransparent(HDC hdcMem, 
								  BITMAPINFOHEADER bmih, byte *pdibBits,
								  IUnknown* punkObject, bool bActive, _point pt, bool bPrint, double fZoom, _rect rc)
{
	if(bmih.biWidth<4 || bmih.biHeight<4)
	{ // прозрачность - только для квадратов не менее 4x4
		_DrawUnit(hdcMem, punkObject, bActive, pt, bPrint, fZoom);
		return false;
	}
	int bi_w34=(bmih.biWidth*3+3)/4*4;

	int nSize=bi_w34*bmih.biHeight;
	smart_alloc(old,byte,nSize);
	if(old==0)
	{
		_DrawUnit(hdcMem, punkObject, bActive, pt, bPrint, fZoom);
		return false;
	}
	memcpy(old,pdibBits,nSize);

	_DrawUnit(hdcMem, punkObject, bActive, pt, bPrint, fZoom);

	for(int i=0; i<nSize; i++)
	{
		pdibBits[i] = (pdibBits[i]*3+old[i])/4;
	}

	return true;
}


void CAView::_DrawUnit(HDC hdcMem, IUnknown* punkObject, bool bActive, _point pt, bool bPrint, double fZoom)
{
	::SetTextColor(hdcMem, bActive ? m_clrActiveText : m_clrInactiveText);
	::SetBkColor  (hdcMem, bActive ? m_clrActive : m_clrInactive);

	sptrIThumbnailManager spThumbMan(GetAppUnknown());
	if(spThumbMan == NULL)
		return;

	long nCurCellSize		 = (long)(fZoom*m_nCellSize		   + .5) +
		(bPrint ? 0 : m_nMinBorder);
	long nCurInterCellDist	 = (long)(fZoom*m_nInterCellDist   + .5);
	long nCurThumbToTextDist = (long)(fZoom*m_nThumbToTextDist + .5);

	bool b3D = bPrint ? m_b3DOnPrint : m_b3D;
	bool bDrawEdge = bPrint ? m_bDrawEdgeOnPrint : m_bDrawEdge;
	

	HBRUSH hbrActive	= ::CreateSolidBrush(m_clrActive);
	HBRUSH hbrInactive	= ::CreateSolidBrush(bPrint?m_clrPrint:m_clrInactive);

	//if(m_bDrawThumb || bPrint)
	if(m_bDrawThumb)
	{
		_rect rcThumb(pt.x, pt.y, pt.x + nCurCellSize, pt.y + nCurCellSize);

		IMeasureObjectPtr sptrMO(punkObject);
		IUnknown* punkImage = 0;
		_size sizeImg(nCurCellSize, nCurCellSize);
		if(sptrMO != 0)
		{
			sptrMO->GetImage(&punkImage);
			IImagePtr sptrImg(punkImage);
			if(sptrImg != 0) 
			{
				sptrImg->GetSize((int*)&sizeImg.cx, (int*)&sizeImg.cy);
				sizeImg.cx = int(sizeImg.cx*fZoom + .5);
				sizeImg.cy = int(sizeImg.cy*fZoom + .5);
			}
		}

		IUnknown* punkRenderObject = NULL;
		spThumbMan->GetRenderObject((punkImage ? punkImage : punkObject), &punkRenderObject);

		sptrIRenderObject spRenderObject(punkRenderObject);
		if(punkRenderObject) punkRenderObject->Release();

		if(spRenderObject == NULL) 
		{
			::DeleteObject(hbrActive);
			::DeleteObject(hbrInactive);
			return;
		}

		BYTE* pbi = NULL;
		short nSupported;
		COLORREF bkColor=bActive ? m_clrActive : m_clrInactive;
		if(bPrint) bkColor=m_clrPrint;
		spRenderObject->GenerateThumbnail((punkImage ? punkImage : punkObject), 24, RF_BACKGROUND, bkColor, 
					sizeImg, 
					&nSupported, &pbi, IViewPtr(this));

		BYTE* pDibBits = pbi + (sizeof(BITMAPINFOHEADER)) + 
							(sizeof(TumbnailInfo));
		COLORREF	cr = get_class_color( get_object_class( ICalcObjectPtr(punkObject) ) );		
		HilightObjectImage(LPBITMAPINFOHEADER(pbi), pDibBits, IImage3Ptr(punkImage), cr);

		if( pbi )
		{	
			rcThumb.inflate(b3D ? 2 : 1);
			if(bDrawEdge)
			{
				if(b3D)
					::DrawEdge(hdcMem, &rcThumb, EDGE_RAISED, BF_RECT);
				else
					::Rectangle(hdcMem, rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
			}
			rcThumb.inflate(b3D ? - 2 : -1);

			
			::FillRect(hdcMem, &rcThumb, (bActive ? hbrActive : hbrInactive));

			rcThumb.left   = rcThumb.left + nCurCellSize/2 - sizeImg.cx/2;
			rcThumb.top    = rcThumb.top  + nCurCellSize/2 - sizeImg.cy/2;
			rcThumb.right  = rcThumb.left + sizeImg.cx;
			rcThumb.bottom = rcThumb.top  + sizeImg.cy;

			POINT p1[2];
			p1[0].x=rcThumb.left;
			p1[0].y=rcThumb.top;
			p1[1].x=rcThumb.right;
			p1[1].y=rcThumb.bottom;

			//int mm=::SetMapMode(hdcMem, MM_TEXT);
			//if(mm==MM_ISOTROPIC) LPtoDP(hdcMem,p1,2);
			_rect rcThumb1(p1[0].x, p1[0].y, p1[1].x, p1[1].y);
			//rcThumb1.right+=rcThumb1.width();
			ThumbnailDraw(pbi, hdcMem, rcThumb1, punkImage);
			//::SetMapMode(hdcMem, mm);
			

			if(punkImage) punkImage->Release();
			
			delete[] pbi;
		}
	}

	//if(m_bDrawText || bPrint)
	if(m_bDrawText)
	{
		_rect rcText(pt.x, pt.y + (m_bDrawThumb ? nCurCellSize + nCurThumbToTextDist : 0), pt.x + nCurCellSize, pt.y + (m_bDrawThumb ? nCurCellSize + nCurThumbToTextDist : 0) + m_nTextHeight);
		rcText.inflate(b3D ? 2 : 1);
		if(bDrawEdge)
		{
			if(b3D)
				::DrawEdge(hdcMem, &rcText, EDGE_SUNKEN, BF_RECT);
			else
				::Rectangle(hdcMem, rcText.left, rcText.top, rcText.right, rcText.bottom);
		}
		rcText.inflate(b3D ? - 2 : -1);
		::FillRect(hdcMem, &rcText, (bActive ? hbrActive : hbrInactive));


		_bstr_t	bstrName = ::GetName(punkObject);
		::SetBkMode(hdcMem, TRANSPARENT);
		::DrawText(hdcMem, (char*)bstrName, lstrlen((char*)bstrName), &rcText, DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);
		::SetBkMode(hdcMem, OPAQUE);

		if(bActive) m_rcPrevSelectedText = rcText; // запомним позицию текста активного - для EditBox
	}

	::DeleteObject(hbrActive);
	::DeleteObject(hbrInactive);
}


void CAView::_DrawResultBar(HDC hdcMem, long& nCurHeightPos, double fZoom, bool bPrint, long nStartClass, long nCount)
{
	BOOL bShowResultBar = ::GetValueInt(GetAppUnknown(), szApportionmentSectName, "ShowResultBar", 1) != 0;
	if(!bShowResultBar) return;

	long nTotalObjectsCount = 0;
	if( m_sptrObjectList != 0 )
		m_sptrObjectList->GetChildsCount(&nTotalObjectsCount);
	
	HFONT hFontText	  = ::CreateFontIndirect(&m_fontText);
	HFONT hfontOld = (HFONT)::SelectObject(hdcMem, hFontText);

	::SetTextColor(hdcMem, m_clrInactiveText);
	::SetBkColor  (hdcMem, m_clrInactive);

	bool b3D = bPrint ? m_b3DOnPrint : m_b3D;

	nCurHeightPos += (long)(m_nInterCellDist*fZoom + .5);

	//table header
	if(nStartClass == 0)
	{
	_rect rcText(0, nCurHeightPos-(b3D?2:1), RESTABLE_COL_WIDTH, nCurHeightPos + m_nTextHeight + (b3D?2:1));


	char sz_buf[256];

	sz_buf[0] = 0;
	::LoadString( App::handle(), IDS_APPORTIONMENT_CLASS_NAME, sz_buf, sizeof(sz_buf) );
	_DrawTablText(hdcMem, rcText, _bstr_t(sz_buf), true, b3D); // "Class name"

	rcText.delta(RESTABLE_COL_WIDTH, 0);
	sz_buf[0] = 0;
	::LoadString( App::handle(), IDS_APPORTIONMENT_NUMBER_OF_OBJECTS, sz_buf, sizeof(sz_buf) );
	_DrawTablText(hdcMem, rcText, _bstr_t(sz_buf), true, b3D); // "Number of objects"

	rcText.delta(RESTABLE_COL_WIDTH, 0);
	sz_buf[0] = 0;
	::LoadString( App::handle(), IDS_APPORTIONMENT_PERCENT_OF_TOTAL, sz_buf, sizeof(sz_buf) );
	_DrawTablText(hdcMem, rcText, _bstr_t(sz_buf), true, b3D); // "Percent of total"
	
	nCurHeightPos += m_nTextHeight + (b3D?4:2);
	}


	if(nCount < 0)
		nCount = m_listClassCells.Count();
	long lPos = m_listClassCells.head();
	long nClassCounter = 0;
	ClassCell* pClassCell = 0;
	while(lPos)
	{
		pClassCell = m_listClassCells.next(lPos);
		if(nStartClass <= nClassCounter && nCount > 0)
		{
			nCount--;

			_rect rcText(0, nCurHeightPos-(b3D?2:1), RESTABLE_COL_WIDTH, nCurHeightPos + m_nTextHeight + (b3D?2:1));

			_DrawTablText(hdcMem, rcText, pClassCell->bstrName, false, b3D);

			char zsBuff[255];
			sprintf(zsBuff, "%d", pClassCell->listObjects.Count());
			rcText.delta(RESTABLE_COL_WIDTH, 0);
			_DrawTablText(hdcMem, rcText, _bstr_t(zsBuff), false, b3D);

			sprintf(zsBuff, "%.2f", pClassCell->listObjects.Count()*100./max(1,nTotalObjectsCount));
			rcText.delta(RESTABLE_COL_WIDTH, 0);
			_DrawTablText(hdcMem, rcText, _bstr_t(zsBuff), false, b3D);
			
			nCurHeightPos += m_nTextHeight + (b3D?4:1);
		}

		nClassCounter++;
	}

	::SelectObject(hdcMem, hfontOld);
	::DeleteObject(hFontText);	
}

// copy source menu to dst menu
static HMENU CopyMenu(HMENU hSrc)
{
	UINT nDefItem = -1;
	HMENU hMenu = NULL, hSubSrc = NULL;

	try
	{
		int nCount = GetMenuItemCount(hSrc);
		if (nCount == -1) return NULL;
		
		hMenu=CreatePopupMenu();

		for (int i = 0; i < nCount; i ++)
		{
			HMENU hSubMenu = NULL;

			// offer no buffer first
			int nStringLen = ::GetMenuString(hSrc, i, NULL, 0, MF_BYPOSITION);

			_ptr_t2<TCHAR> str(1);
			str[0]=NULL;
			if(nStringLen>0)
			{// use exact buffer length
				str.alloc(nStringLen+1);
				if(str==0) return NULL;
				::GetMenuString(hSrc, i, str, nStringLen+1, MF_BYPOSITION);
			}

			UINT uiCmd = GetMenuItemID(hSrc,i);
			UINT uiState = GetMenuState(hSrc, i, MF_BYPOSITION);

			switch (uiCmd)
			{
			case 0:
				if (!InsertMenu(hMenu, i, MF_BYPOSITION | MF_SEPARATOR, 0, 0))
					throw "uiCmd == 0";
				break;

			case -1:
				hSubSrc = GetSubMenu(hSrc,i);
				if(hSubSrc==NULL) throw "No submenu";
				hSubMenu = CopyMenu(hSubSrc);
				if (!InsertMenu(hMenu, i, MF_BYPOSITION | MF_STRING | MF_POPUP | uiState, (UINT)hSubMenu, str))
					throw "Can't insert submenu";

				if (hSubMenu) ::DestroyMenu(hSubMenu);
				hSubMenu = NULL;
				
				break;

			default:
				if (!InsertMenu(hMenu, i, MF_BYPOSITION | MF_STRING | uiState, uiCmd, str))
					throw "Can't insert menu item";
				break;
			}// switch
		}

	
		nDefItem = GetMenuDefaultItem(hSrc,GMDI_USEDISABLED, FALSE);
		if (nDefItem != -1)
			SetMenuDefaultItem(hMenu, nDefItem, FALSE);
		
	}
	catch (...)
	{
		DestroyMenu(hMenu);
		return NULL;
	}
	
	return hMenu;
}


HRESULT CAView::OnInitPopup(BSTR bstrMenuName, HMENU hMenu, HMENU *phOutMenu)
{
	*phOutMenu = ::CopyMenu(hMenu);
	if (!*phOutMenu) return S_FALSE;

	IUnknown *punkObject=0;
	HRESULT r=GetObjectByIdx(0, &punkObject);
	ICalcObjectPtr sptrCO(punkObject);
	if(punkObject) punkObject->Release();
	long nClass = get_object_class( sptrCO );
	
	IUnknownPtr punkAM(::_GetOtherComponent(::GetAppUnknown(), IID_IActionManager), false);
	sptrIActionManager sptrAM(punkAM);
	DWORD	dwFlags = 0;
	sptrAM->GetActionFlags( _bstr_t("SetClass"), &dwFlags );

	if( sptrCO!=0 && (dwFlags&afEnabled) )
	{
		UINT uState = (sptrCO != 0) ? MF_ENABLED : (MF_DISABLED | MF_GRAYED);

		InsertMenu(*phOutMenu, -1, MF_BYPOSITION | MF_SEPARATOR, 0, 0);

		long nClassCount = class_count();
		for(int i=0; i<nClassCount; i++)
		{
			_bstr_t bstr = get_class_name( i );
			UINT uState2 = (i==nClass) ? MF_GRAYED | MF_CHECKED : MF_ENABLED;
			InsertMenu(*phOutMenu, -1, MF_BYPOSITION | MF_STRING | uState | uState2,
				ID_CLASSMENU_BASE+1+i, bstr);
		}
		if(m_bShowUnknown)
		{
			_bstr_t bstr = get_class_name( -1 );
			UINT uState2 = (-1==nClass) ? MF_GRAYED | MF_CHECKED : MF_ENABLED;
			InsertMenu(*phOutMenu, -1, MF_BYPOSITION | MF_STRING | uState | uState2,
				ID_CLASSMENU_BASE, bstr);
		}
	}

	return S_OK;
}

/////////////////////////////////////////////////
//interface IAView
//////////////////////////////////////////////////////////////////////
HRESULT CAView::GetTypeInfoCount( 
            /* [out] */ UINT *pctinfo)
{
	if( !pctinfo )
		return E_POINTER;

	*pctinfo = 1;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CAView::GetTypeInfo( 
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
HRESULT CAView::GetIDsOfNames( 
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
HRESULT CAView::Invoke( 
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

	return m_pi_type_info->Invoke(	(IUnknown*)(IAView*)this, dispIdMember, wFlags, 
		pDispParams, pVarResult, pExcepInfo, puArgErr );
}

HRESULT CAView::ScrollToClass( long nClass)
{
	ClassCell* pClassCell = 0;
	long lPos = m_listClassCells.head();
	long nClassCounter = 0;
	m_nActiveClass=nClass;
	m_nActiveThumb=-1;
	while(lPos)
	{
		pClassCell=m_listClassCells.next(lPos);
		if(nClassCounter==nClass)
		{
			_point ptScroll = pClassCell->m_rcLastDrawRect.top_left();
			IScrollZoomSitePtr sptrSZ(Unknown());
			if(sptrSZ != 0) sptrSZ->SetScrollPos(ptScroll);
			break;
		}
		nClassCounter++;
	}
	return S_OK;
}

void CAView::HilightObjectImage(BITMAPINFOHEADER *pbmih, byte *pdibBits, IImage3Ptr ptrImage, COLORREF clr_highlight)
{
	if( pdibBits==0 || pbmih==0) return;
	if(pbmih->biBitCount!=24) return;
	if( ptrImage == 0 ) return;

	POINT ptOffset={0};
	int img_cx = 0;
	int img_cy = 0;
	ptrImage->GetSize( &img_cx, &img_cy );
	ptrImage->GetOffset( &ptOffset );
	_rect	rect2(ptOffset,_size(img_cx,img_cy));
	_rect	rect(0,0, pbmih->biWidth, pbmih->biHeight);

	int rMask = GetRValue( clr_highlight );
	int gMask = GetGValue( clr_highlight );
	int bMask = GetBValue( clr_highlight );

	int cx	= pbmih->biWidth;
	int cy	= pbmih->biHeight;
	int	cx4	= (cx*3+3)/4*4;

	int alpha = int(m_nHighlightPercent/100.0*256+0.5);

	double fZoom = (double)cx/img_cx;

	for( int _y=0;_y<cy;_y++ )
	{
		int y = cy - _y - 1;
		long nRowOffset = y*cx4;

		int y_img = _y / fZoom;

		if( y_img < 0 || y_img >= img_cy )
			continue;

		byte *pmask = 0;
		ptrImage->GetRowMask( y_img, &pmask );

		byte *pb = pdibBits + y*cx4;

		if(m_nHighlightPhases)
		{	// подсветка по фазам
			for( int x=0; x<cx; x++,pb+=3 )
			{
				int x_img = x / fZoom;

				if( x_img < 0 || x_img >= img_cx )
					continue;

				int m=pmask[x_img];
				if( !(m & 0x80) ) continue;

				COLORREF clPhase=g_pdwBinaryColors[255-m];
                // 255->0, 254->1 и так далее - для Binary цветов
				if(m_nHighlightPhases==2) clPhase ^= clr_highlight;

				pb[0] = pb[0] - (pb[0] - GetBValue(clPhase))*alpha/256;
				pb[1] = pb[1] - (pb[1] - GetGValue(clPhase))*alpha/256;
				pb[2] = pb[2] - (pb[2] - GetRValue(clPhase))*alpha/256;				
			}
		}
	}			
}

void CAView::_InitBinaryColorsTable()
{
  DWORD dwDefColor = RGB(255,255,0);
  long nCounter = GetValueInt(GetAppUnknown(), "\\Binary", "IndexedColorsCount", 1);
  nCounter = max(0, min(256, nCounter));
  char szStr[128];
  for(long i = 0; i < 256; i++)
  {
    if(i < nCounter)
    {
	  sprintf(szStr, "Fore_%d", i);
      g_pdwBinaryColors[i] = GetValueColor(GetAppUnknown(), "\\Binary", szStr, dwDefColor);
    }
    else
      g_pdwBinaryColors[i] = dwDefColor;
  }
  g_clSyncColor = GetValueColor(GetAppUnknown(), "\\Binary", "SyncColor", RGB( 255,255,0 ) );
}

CRenameEditBox::CRenameEditBox()
{
	m_szText[0]=0;
	m_bReady = false;
}

long CRenameEditBox::on_char( long nVirtKey )
{
	if( VK_RETURN == nVirtKey )
	{
		send_message( WM_GETTEXT, (LPARAM)255, (LPARAM)m_szText );
		m_bReady = true;
	}
	else if( VK_ESCAPE == nVirtKey )
	{
		::DestroyWindow( handle() );
	}
	return __super::on_char(nVirtKey);
}
