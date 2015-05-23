// AuditTrailObject.cpp : implementation file
//

#include "stdafx.h"
#include "AuditTrail.h"
#include "AuditTrailObject.h"
#include "thumbnail.h"


BYTE* DrawThumbnail( IUnknown* punkObject, CDC* pDC, CRect rcTarget, COLORREF clrBack)
{
	if( punkObject == NULL )
		return 0;

	sptrIThumbnailManager spThumbMan( GetAppUnknown() );
	if( spThumbMan == NULL )
		return 0;


	IUnknown* punkRenderObject = NULL;

	spThumbMan->GetRenderObject( punkObject, &punkRenderObject );
	if( punkRenderObject == NULL )
		return 0;
			   

	sptrIRenderObject spRenderObject( punkRenderObject );
	punkRenderObject->Release();

	if( spRenderObject == NULL ) 
		return 0;


	BYTE* pbi = NULL;
	
	short nSupported;
	spRenderObject->GenerateThumbnail( punkObject, 24, tfVarStretch, 0, 
				CSize(rcTarget.Width(), rcTarget.Height() ), 
				&nSupported, &pbi, 0 );

	if( pbi )
	{		
		pDC->FillRect( &rcTarget, &CBrush(clrBack) );
		::ThumbnailDraw( pbi, pDC, 
				CRect(	rcTarget.left, rcTarget.top,
						rcTarget.right, rcTarget.bottom
						), 0
					);
		//::ThumbnailFree( pbi );
		//pbi = NULL;
	}
	return pbi;
}


const char *CAuditTrailObject::c_szType = szTypeAuditTrailObject;
DrawDib	g_DrawDibInstance;

IMPLEMENT_DYNCREATE(CAuditTrailObject, CCmdTargetEx)

// {0BD3A2DA-5680-4fe0-AA21-416368F848EC}
static const GUID clsidATDataObject = 
{ 0xbd3a2da, 0x5680, 0x4fe0, { 0xaa, 0x21, 0x41, 0x63, 0x68, 0xf8, 0x48, 0xec } };
// {1892816C-14E8-467b-8621-5D4C0E31BBD2}
GUARD_IMPLEMENT_OLECREATE(CAuditTrailObject, "AuditTrail.AuditTrailObject", 
0x1892816c, 0x14e8, 0x467b, 0x86, 0x21, 0x5d, 0x4c, 0xe, 0x31, 0xbb, 0xd2);

DATA_OBJECT_INFO(IDS_AUDIT_TYPE, CAuditTrailObject, CAuditTrailObject::c_szType, clsidATDataObject, IDI_AT_OBJECTS );

BEGIN_INTERFACE_MAP(CAuditTrailObject, CDataObjectBase)
	INTERFACE_PART(CAuditTrailObject, IID_IAuditTrailObject, ATobj)	 
	INTERFACE_PART(CAuditTrailObject, IID_IPrintView, PrintView)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CAuditTrailObject, ATobj)

HRESULT CAuditTrailObject::XATobj::CreateThumbnail(IUnknown* punkDoc, IUnknown* punkImage, IUnknown* punkAction, BSTR bstrParams, BOOL bRender)
{
	METHOD_PROLOGUE_EX(CAuditTrailObject, ATobj);
	if(punkDoc)
		pThis->CreateThumbnail(punkDoc, punkImage, punkAction, bstrParams, bRender == TRUE);
	return S_OK;
}

HRESULT CAuditTrailObject::XATobj::GetDocumentSize(SIZE* psizeDoc)
{
	METHOD_PROLOGUE_EX(CAuditTrailObject, ATobj);
	if(psizeDoc)
		*psizeDoc = pThis->GetDocumentSize();
	return S_OK;
}

HRESULT CAuditTrailObject::XATobj::Draw(BITMAPINFOHEADER *pbi, BYTE *pdibBits, RECT rectDest)
{
	METHOD_PROLOGUE_EX(CAuditTrailObject, ATobj);

	pThis->Draw(pbi, pdibBits, rectDest);
	return S_OK;
}

HRESULT CAuditTrailObject::XATobj::GetThumbnailFromPoint(POINT ptCoord, long* pnIndex)
{
	METHOD_PROLOGUE_EX(CAuditTrailObject, ATobj);

	if(pnIndex)
		*pnIndex = pThis->GetThumbnailFromPoint(ptCoord);
	return S_OK;
}

HRESULT CAuditTrailObject::XATobj::SetActiveThumbnail(long nIndex)
{
	METHOD_PROLOGUE_EX(CAuditTrailObject, ATobj);

	pThis->SetActiveThumbnail(nIndex);
	return S_OK;
}

HRESULT CAuditTrailObject::XATobj::GetActiveThumbnail(long* pnIndex)
{
	METHOD_PROLOGUE_EX(CAuditTrailObject, ATobj);

	if(pnIndex)
		*pnIndex = pThis->m_nActiveThumbnail;
	return S_OK;
}

HRESULT CAuditTrailObject::XATobj::GetThumbnailRect(long nIndex, RECT* prcBound)
{
	METHOD_PROLOGUE_EX(CAuditTrailObject, ATobj);
	if(prcBound)
	{
		*prcBound = pThis->GetThumbnailRect(nIndex);
	}
	return S_OK;
}


HRESULT CAuditTrailObject::XATobj::UndoAction(GuidKey* pKeyAction, GuidKey* pKeyImage)
{
	METHOD_PROLOGUE_EX(CAuditTrailObject, ATobj);

	if(pThis->UndoAction(*pKeyAction, pKeyImage))
		return S_OK;
	else
		return S_FALSE;
}

HRESULT CAuditTrailObject::XATobj::SetFlags(DWORD dwFlags)
{
	METHOD_PROLOGUE_EX(CAuditTrailObject, ATobj);

	pThis->m_dwFlags = dwFlags;

	return S_OK;
}

HRESULT CAuditTrailObject::XATobj::GetThumbnailsCount(long* pnCount)
{
	METHOD_PROLOGUE_EX(CAuditTrailObject, ATobj);

	if(pnCount)
		*pnCount = pThis->m_arrThumbnailsInfo.GetSize();
	else
		return E_INVALIDARG;

	return S_OK;
}

HRESULT CAuditTrailObject::XATobj::GetThumbnailInfo(long nIdx, BSTR* pbstrActionName, BSTR* pbstrActionParams, BSTR* pbstrActionTime)
{
	METHOD_PROLOGUE_EX(CAuditTrailObject, ATobj);

	int nCount = pThis->m_arrThumbnailsInfo.GetSize();

	if(nIdx < 0 || nIdx >= nCount || !pbstrActionName || !pbstrActionParams || !pbstrActionTime)
		return E_INVALIDARG;

	*pbstrActionName = pThis->m_arrThumbnailsInfo[nIdx]->strActionName.AllocSysString();
	*pbstrActionParams = pThis->m_arrThumbnailsInfo[nIdx]->strParams.AllocSysString();
	*pbstrActionTime = pThis->m_arrThumbnailsInfo[nIdx]->strActionTime.AllocSysString();
	
	return S_OK;
}


HRESULT CAuditTrailObject::XATobj::SetGridData( BYTE* pBuf, long lSize )
{
	METHOD_PROLOGUE_EX(CAuditTrailObject, ATobj);

	pThis->_deinit_grid_data();
	if( lSize < 0 )
		return S_FALSE;

	pThis->m_pGridData		= new BYTE[lSize];
	pThis->m_lGridDataSize	= lSize;

	memcpy( pThis->m_pGridData, pBuf, lSize );

	return S_OK;
}

HRESULT CAuditTrailObject::XATobj::GetGridData( BYTE** ppBuf, long* plSize )
{
	METHOD_PROLOGUE_EX(CAuditTrailObject, ATobj);
	if( !ppBuf || !plSize )
		return E_INVALIDARG;

	*ppBuf	= pThis->m_pGridData;
	*plSize = pThis->m_lGridDataSize;


	return S_OK;
}

///////////////////////////////////////////////////////////////////////////// 
HRESULT CAuditTrailObject::XATobj::ReloadSettings( )
{
	METHOD_PROLOGUE_EX(CAuditTrailObject, ATobj);

	pThis->m_sizeThumbnail = CSize(::GetValueInt(GetAppUnknown(), "\\AuditTrail", "ThumbnailWidth", 128), ::GetValueInt(GetAppUnknown(), "AuditTrail", "ThumbnailHight", 128));
	pThis->m_nBorder = ::GetValueInt(GetAppUnknown(), "\\AuditTrail", "ThumbnailBorder", 10);
	pThis->m_bShowActionParams = ::GetValueInt(GetAppUnknown(), "\\AuditTrail", "ShowActionParams", 1) == 1;

	pThis->m_bOnlyText = ::GetValueInt(GetAppUnknown(), "\\AuditTrail", "OnlyText", 0) == 1;

	pThis->m_clrActive = ::GetValueColor(GetAppUnknown(), "\\AuditTrail", "ActiveThumbnailColor", ::GetSysColor(COLOR_HIGHLIGHT));
	pThis->m_clrInactive = ::GetValueColor(GetAppUnknown(), "\\AuditTrail", "InactiveThumbnailColor", GetSysColor(COLOR_WINDOW));
	pThis->m_clrActiveText = ::GetValueColor(GetAppUnknown(), "\\AuditTrail", "ActiveTextColor", ::GetSysColor(COLOR_HIGHLIGHTTEXT));
	pThis->m_clrInactiveText = ::GetValueColor(GetAppUnknown(), "\\AuditTrail", "InactiveTextColor", ::GetSysColor(COLOR_WINDOWTEXT));

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
// CAuditTrailObject
CAuditTrailObject::CAuditTrailObject()
{
	m_sizeThumbnail = CSize(::GetValueInt(GetAppUnknown(), "\\AuditTrail", "ThumbnailWidth", 128), ::GetValueInt(GetAppUnknown(), "AuditTrail", "ThumbnailHight", 128));
	m_nBorder = ::GetValueInt(GetAppUnknown(), "\\AuditTrail", "ThumbnailBorder", 10);
	m_bShowActionParams = ::GetValueInt(GetAppUnknown(), "\\AuditTrail", "ShowActionParams", 1) == 1;
	_OleLockApp( this );

	m_bOnlyText = ::GetValueInt(GetAppUnknown(), "\\AuditTrail", "OnlyText", 0) == 1;

	m_pCurrentDIB = 0;
	m_sizeCurrentDIB = CSize(0, 0);

	m_nFromPureBlackCount = 0;
	m_nActiveThumbnail = 0;
	
	m_clrActive = ::GetValueColor(GetAppUnknown(), "\\AuditTrail", "ActiveThumbnailColor", ::GetSysColor(COLOR_HIGHLIGHT));
	m_clrInactive = ::GetValueColor(GetAppUnknown(), "\\AuditTrail", "InactiveThumbnailColor", GetSysColor(COLOR_WINDOW));
	m_clrActiveText = ::GetValueColor(GetAppUnknown(), "\\AuditTrail", "ActiveTextColor", ::GetSysColor(COLOR_HIGHLIGHTTEXT));
	m_clrInactiveText = ::GetValueColor(GetAppUnknown(), "\\AuditTrail", "InactiveTextColor", ::GetSysColor(COLOR_WINDOWTEXT));

	LOGFONT font;
	::ZeroMemory(&font, sizeof(LOGFONT));
	CFont* pFont = CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT));	
	if( pFont )
	{
		pFont->GetLogFont(&font);
		m_fontText.CreatePointFontIndirect(&font);
	}

	m_arrDifferences.RemoveAll();
	m_arrSizes.RemoveAll();

	m_pGridData		= 0;
	m_lGridDataSize	= 0;

}

CAuditTrailObject::~CAuditTrailObject()
{
	CleanUpDifferences();

	CleanUpInfos();

	if(m_pCurrentDIB)
	{
		delete m_pCurrentDIB;
		m_pCurrentDIB = 0;
	}

	_deinit_grid_data();

	_OleUnlockApp( this );
}

void CAuditTrailObject::_deinit_grid_data()
{
	if( m_pGridData )
		delete m_pGridData;

	m_pGridData		= 0;
	m_lGridDataSize	= 0;
}

void CAuditTrailObject::CreateThumbnail(IUnknown* punkDoc, IUnknown* punkImage, IUnknown* punkAction, CString strParams, bool bRender)
{
  if((m_sizeThumbnail.cx <= 0 || m_sizeThumbnail.cy <= 0) && !m_bOnlyText)
		return;

	IDocumentSitePtr sptrDoc(punkDoc);
	if(sptrDoc == 0)
		return;


	//temporatory
	{
	/*IUnknown* punkView = 0;
	sptrDoc->GetActiveView(&punkView);
	if(punkView)
	{
		IDataContextPtr	sptrContext(punkView);
		if(sptrContext != 0)
		{
			sptrContext->SetActiveObject(_bstr_t(szTypeImage), punkImage);
		}
		punkView->Release();
	}*/

	//::ExecuteAction("ObjectSelect", CString("\"") + GetObjectName(punkImage) + CString("\""));

	}
	//temporatory

	
	IActionPtr sptrAction(punkAction);
	IActionInfoPtr	sptrActionInfo;
	if(sptrAction == 0)
	{
		sptrActionInfo = punkAction;
		if(sptrActionInfo == 0)
			return;
	}
	else
	{
		IUnknown* punkAI = 0;
		sptrAction->GetActionInfo(&punkAI);
		sptrActionInfo = punkAI;
		if(punkAI)
			punkAI->Release();
		if(sptrActionInfo == 0)
			return;
	}

	CString strActionName;
	GuidKey keyAction;

	BSTR	bstrActionName = 0;
	sptrActionInfo->GetCommandInfo( 0, &bstrActionName, 0, 0); 
	strActionName = bstrActionName;
	::SysFreeString(bstrActionName);

	if(strActionName.IsEmpty())
		return;

	if(sptrAction != 0)
		keyAction = ::GetObjectKey(sptrAction);



	IUnknown* punkView = 0;
	sptrDoc->GetActiveView(&punkView);
	IPrintViewPtr sptrPrint(punkView);
	IScrollZoomSitePtr	sptrSZ(punkView);		
	if(punkView)
		punkView->Release();
	if(sptrPrint == 0 || sptrSZ == 0)
		return;

	CSize sizeV(0, 0);
	sptrSZ->GetClientSize(&sizeV);		
	//sizeV = ::ConvertToClient(sptrPrint, sizeV);

	if(sizeV.cx <= 0 || sizeV.cy <= 0)
		return;

	DifferenceUnit* pHead = 0;

	if(!m_bOnlyText)
	{
		CDC dcImage;
		byte	*pdibBits = 0;
		HBITMAP	hDIBSectionSrc = 0;
		CBitmap* pOldbitmap;

		CClientDC dcScreen(0);		
		VERIFY( dcImage.CreateCompatibleDC( &dcScreen ) );
		CBitmap  Bitmap;

		VERIFY( Bitmap.CreateCompatibleBitmap( &dcScreen, sizeV.cx, sizeV.cy ) );
		pOldbitmap = dcImage.SelectObject( &Bitmap );

		if(bRender)
			DrawThumbnail(punkImage, &dcImage, CRect( 0, 0, sizeV.cx, sizeV.cy ), m_clrInactive);
		else
			sptrPrint->Print(dcImage.GetSafeHdc(), CRect(0, 0, sizeV.cx, sizeV.cy), 0, 0, sizeV.cx, sizeV.cy, FALSE, 0 );		

		//make dib from dc
		BITMAPINFOHEADER BitmapInfoSrc;
		FILL_BITMAPBITMAPINFOHEADER(BitmapInfoSrc, sizeV);
		
		hDIBSectionSrc = ::CreateDIBSection( dcImage.m_hDC, (BITMAPINFO*)&BitmapInfoSrc, DIB_RGB_COLORS, (void**)&pdibBits, 0, 0 );
		ASSERT( hDIBSectionSrc );
		GetDIBits(dcImage, Bitmap, 0, sizeV.cy, pdibBits, (BITMAPINFO*)&BitmapInfoSrc, DIB_RGB_COLORS);
		
		bool bUsePureBlackDIB = (m_nFromPureBlackCount%MAX_FROM_PURE_BLACK) == 0;

		long nDibSize = ((sizeV.cx*3+3)/4*4)*sizeV.cy;

		if((!m_pCurrentDIB) || (m_pCurrentDIB && (m_sizeCurrentDIB != sizeV)) || bUsePureBlackDIB)
		{
			if(m_pCurrentDIB)
			{
				delete m_pCurrentDIB;
				m_pCurrentDIB = 0;
			}
			m_sizeCurrentDIB = sizeV;

			//make pure black DIB
			m_pCurrentDIB = new byte[nDibSize];
			ZeroMemory(m_pCurrentDIB, nDibSize); 

			bUsePureBlackDIB = true;
		}
		
		//process thumbnail dib and create difference structures
		int	cx4Bmp = (m_sizeCurrentDIB.cx*3+3)/4*4;
		DifferenceUnit* pLast = 0;

			
		for(int y = 0; y < m_sizeCurrentDIB.cy; y++)
		{
			long nRowOffset = y*cx4Bmp;
			bool bGraySeq = false;
			bool bColorSeq = false;
			byte* pGraySeq = 0;
			byte* pColorSeq = 0;
			DifferenceUnit* pCurDiff = 0;

			for(int x = 0; x < m_sizeCurrentDIB.cx; x++)
			{
				byte R = pdibBits[nRowOffset+3*x];
				byte G = pdibBits[nRowOffset+3*x + 1];
				byte B = pdibBits[nRowOffset+3*x + 2];

				byte curR = m_pCurrentDIB[nRowOffset+3*x];
				byte curG = m_pCurrentDIB[nRowOffset+3*x + 1];
				byte curB = m_pCurrentDIB[nRowOffset+3*x + 2];

				if(pCurDiff && (pCurDiff->bPixelCount & 0x7f) >= 0x7f)
					CLOSE_SEQ

				if(R != curR || G != curG || B != curB) //compare with current black DIB
				{
					if(R == G && G == B) //grayscale
					{
						if(bGraySeq)		
						{
							SET_GRAY_PIX
						}
						else
						{
							CLOSE_SEQ
							
							bGraySeq = true;
							pCurDiff = new DifferenceUnit;
							pCurDiff->pNext = 0;
							pCurDiff->bPixelCount = 0;
							pCurDiff->nX = x;
							pCurDiff->nY = y;
							pGraySeq = new byte[m_sizeCurrentDIB.cx];

							SET_GRAY_PIX
						}
					}
					else
					{
						if(bColorSeq)		
						{
							SET_COLOR_PIX
						}
						else
						{
							CLOSE_SEQ

							bColorSeq = true;
							pCurDiff = new DifferenceUnit;
							pCurDiff->pNext = 0;
							pCurDiff->bPixelCount = 1<<7;
							pCurDiff->nX = x;
							pCurDiff->nY = y;
							pColorSeq = new byte[m_sizeCurrentDIB.cx*3];

							SET_COLOR_PIX
						}
					}
				}
				else
				{
					CLOSE_SEQ
				}

			}

			CLOSE_SEQ
		}

		memcpy(m_pCurrentDIB, pdibBits, nDibSize);

		TRACE("@@@@@@@Thumbnail generates of %d bytes. Appropriate DIB bits = %d\n", GetDifferenceSize(pHead), nDibSize);

		m_arrDifferences.Add(pHead);
		m_arrSizes.Add(bUsePureBlackDIB ? m_sizeCurrentDIB : CSize(0, 0));

		dcImage.SelectObject( pOldbitmap );
		::DeleteObject(hDIBSectionSrc);
	}


	SYSTEMTIME st;
	GetLocalTime(&st);
	CTime time(st);
	CString strTime;
	strTime = time.Format("%c");

	ThumbnailInfo* pInfo = new ThumbnailInfo;
	pInfo->strActionName = strActionName;
	pInfo->strParams = strParams;
	pInfo->strActionTime = strTime;
	pInfo->keyAction = keyAction;

	//pInfo->keyImage = GetObjectKey(punkImage);
	INamedDataObject2Ptr sptrNDO(punkImage);
	if(sptrNDO != 0)
		sptrNDO->GetBaseKey(&pInfo->keyImage);
	
	m_arrThumbnailsInfo.Add(pInfo);
	
	m_nActiveThumbnail = m_nFromPureBlackCount;
	m_nFromPureBlackCount++;

	::UpdateDataObject(m_punkNamedData, GetControllingUnknown());
}

int CAuditTrailObject::GetThumbnailsCount()
{
	return m_arrThumbnailsInfo.GetSize();
}
		
CSize CAuditTrailObject::GetDocumentSize()
{
  //update settings
  /*m_sizeThumbnail = CSize(::GetValueInt(GetAppUnknown(), "AuditTrail", "ThumbnailWidth", 128), ::GetValueInt(GetAppUnknown(), "AuditTrail", "ThumbnailHight", 128));
  m_nBorder = ::GetValueInt(GetAppUnknown(), "AuditTrail", "ThumbnailBorder", 10);
  m_bShowActionParams = ::GetValueInt(GetAppUnknown(), "AuditTrail", "ShowActionParams", 1) == 1;
  m_clrActive = ::GetValueColor(GetAppUnknown(), "AuditTrail", "ActiveThumbnailColor", ::GetSysColor(COLOR_HIGHLIGHT));
	m_clrInactive = ::GetValueColor(GetAppUnknown(), "AuditTrail", "InactiveThumbnailColor", GetSysColor(COLOR_WINDOW));
	m_clrActiveText = ::GetValueColor(GetAppUnknown(), "AuditTrail", "ActiveTextColor", ::GetSysColor(COLOR_HIGHLIGHTTEXT));
	m_clrInactiveText = ::GetValueColor(GetAppUnknown(), "AuditTrail", "InactiveTextColor", ::GetSysColor(COLOR_WINDOWTEXT));
  //m_bOnlyText = ::GetValueInt(GetAppUnknown(), "AuditTrail", "OnlyText", 0) == 1;
	*/


	CSize size(0, 0);
	if((m_dwFlags & atdfHorz) == atdfHorz)
	{
		size.cx = m_sizeThumbnail.cx*GetThumbnailsCount() + m_nBorder*(GetThumbnailsCount()+1);
		if(m_bOnlyText)
			size.cy = m_nBorder + GetTextLineHeight()*2 + TEXT_INTERLINE_HEIGHT;
		else
			size.cy = m_sizeThumbnail.cy + m_nBorder*3 + GetTextLineHeight()*2 + TEXT_INTERLINE_HEIGHT;
	}
	else if((m_dwFlags & atdfVert) == atdfVert)
	{
		size.cx = m_sizeThumbnail.cx + m_nBorder*2;
		if(m_bOnlyText)
			size.cy = (m_nBorder + GetTextLineHeight()*2 + TEXT_INTERLINE_HEIGHT)*GetThumbnailsCount() + m_nBorder;
		else
			size.cy = (m_nBorder + m_sizeThumbnail.cy + m_nBorder + GetTextLineHeight()*2 + TEXT_INTERLINE_HEIGHT)*GetThumbnailsCount() + m_nBorder;
	}
	else
	{
		for(int i = m_nActiveThumbnail; i >= 0; i--)
		{
			CSize size(0,0);
			if(m_bOnlyText)
				size = CSize(m_sizeThumbnail.cx*3, 0);
			else
				size = m_arrSizes[i];
			if(size != CSize(0, 0))
			{
				size += CSize(m_nBorder*2, m_nBorder*3 + GetTextLineHeight()*2 + TEXT_INTERLINE_HEIGHT);
				return size;
			}
		}
	}
			
	return size;
}

void CAuditTrailObject::Draw(BITMAPINFOHEADER *pbi, BYTE *pdibBits, CRect rectDest, bool bDrawSelection)
{

	CRect rcDoc(CPoint(0, 0), GetDocumentSize());
	rectDest.IntersectRect(&rectDest, &rcDoc);
	if(rectDest.top > rcDoc.bottom || rectDest.left > rcDoc.right)
		return;

	BITMAPINFOHEADER BitmapInfoCurrentDIB;			
	int	cx4Bmp = 0;
	byte* pdibCurrent = 0;
	int nDIBrow = (pbi->biWidth*3+3)/4*4;

	if((m_dwFlags & atdfHorz) == atdfHorz || (m_dwFlags & atdfVert) == atdfVert)
	{
		
		//thumbnail DIB
		CSize sizeThumbnailUnit(m_sizeThumbnail.cx, (m_bOnlyText ? 0 : m_sizeThumbnail.cy + m_nBorder) + GetTextLineHeight()*2 + TEXT_INTERLINE_HEIGHT); //unit with text
		BITMAPINFOHEADER BitmapInfo;
		FILL_BITMAPBITMAPINFOHEADER(BitmapInfo, sizeThumbnailUnit);
		byte	*pdibThumbnail = 0;
		HBITMAP	hDIBSection = ::CreateDIBSection(0, (BITMAPINFO*)&BitmapInfo, DIB_RGB_COLORS, (void**)&pdibThumbnail, 0, 0);
		ASSERT(hDIBSection);
	
		CSize sizeDIB(0, 0);
		CSize sizeThumbnail(0, 0);
		CRect rcWorkingArea(4, 4, m_sizeThumbnail.cx-4, m_sizeThumbnail.cy-4);
		
		for(int i = 0; i < GetThumbnailsCount(); i++)
		{
			if(!m_bOnlyText)
			{
				//generate cur DIB
				DifferenceUnit* pHead = m_arrDifferences[i];			
				if(m_arrSizes[i] != CSize(0, 0))
				{
					sizeDIB = m_arrSizes[i];
					cx4Bmp = (sizeDIB.cx*3+3)/4*4;
					if(pdibCurrent)
						delete pdibCurrent;
					pdibCurrent = new byte[cx4Bmp*sizeDIB.cy];
					ZeroMemory(pdibCurrent, cx4Bmp*sizeDIB.cy);

					FILL_BITMAPBITMAPINFOHEADER(BitmapInfoCurrentDIB, sizeDIB);
				}
				ASSERT(sizeDIB != CSize(0, 0));

				sizeThumbnail = CSize(rcWorkingArea.Width(), rcWorkingArea.Height());

				if(sizeDIB.cx > sizeDIB.cy)
				{
					sizeThumbnail.cy = rcWorkingArea.Height()*sizeDIB.cy/sizeDIB.cx;
				}
				else if(sizeDIB.cx < sizeDIB.cy)
				{
					sizeThumbnail.cx = rcWorkingArea.Width()*sizeDIB.cx/sizeDIB.cy;
				}

						
				
				while(pHead)
				{
					bool bColor = pHead->bPixelCount >> 7 == 1;
					int nCount = pHead->bPixelCount & 0x7f;
					
					if(bColor)
					{
						memcpy((pdibCurrent+(cx4Bmp*pHead->nY + pHead->nX*3)), pHead->pbData, nCount*3);
					}
					else
					{
						for(int x = 0; x < nCount; x++)
						{
							byte nColor = pHead->pbData[x];
							*(pdibCurrent + cx4Bmp*pHead->nY + (pHead->nX + x)*3 + 0) = nColor;
							*(pdibCurrent + cx4Bmp*pHead->nY + (pHead->nX + x)*3 + 1) = nColor;
							*(pdibCurrent + cx4Bmp*pHead->nY + (pHead->nX + x)*3 + 2) = nColor;
						}
					}
					pHead = pHead->pNext;
				}
			}


			//make thumbnail DIB
			{
			CClientDC dc(0);
			CDC dcMem;
			dcMem.CreateCompatibleDC(&dc);
			CBitmap* pBmp = CBitmap::FromHandle(hDIBSection);
			CBitmap* pOldBmp = dcMem.SelectObject(pBmp);
			if(!m_bOnlyText)
			{
				dcMem.FillRect(&CRect(CPoint(0, 0), m_sizeThumbnail), &CBrush((m_nActiveThumbnail == i && bDrawSelection) ? m_clrActive : m_clrInactive));
				dcMem.DrawEdge(&CRect(CPoint(0, 0), m_sizeThumbnail), EDGE_RAISED, BF_RECT );
				::DrawDibDraw(g_DrawDibInstance, dcMem, (m_sizeThumbnail.cx-sizeThumbnail.cx)/2, (m_sizeThumbnail.cy-sizeThumbnail.cy)/2, sizeThumbnail.cx, sizeThumbnail.cy, &BitmapInfoCurrentDIB, pdibCurrent, 0, 0, sizeDIB.cx, sizeDIB.cy, 0);
			}

			dcMem.FillRect(&CRect(CPoint(0, m_bOnlyText ? 0 : m_sizeThumbnail.cy), sizeThumbnailUnit), &CBrush(m_clrInactive));

			//draw text
			CRect rcBorderAroundText(0, (m_bOnlyText ? 0 : m_sizeThumbnail.cy+m_nBorder), m_sizeThumbnail.cx, sizeThumbnailUnit.cy);
			CRect rcActionName = rcBorderAroundText;
			CRect rcTime = rcBorderAroundText;

			rcActionName.DeflateRect(5, 2);
			rcTime.DeflateRect(2, 2);

			rcTime.top = rcActionName.bottom = rcActionName.top + GetTextLineHeight();
			
			COLORREF colorText, colorBack;
			if(m_nActiveThumbnail == i  && bDrawSelection)
			{
				colorBack = m_clrActive;
				colorText = m_clrActiveText;
			}
			else	  
			{
				colorBack = m_clrInactive;
				colorText = m_clrInactiveText;
			}

			dcMem.FillRect(&rcBorderAroundText, &CBrush(colorBack));

			dcMem.SetTextColor( colorText );
			dcMem.SetBkColor( colorBack );

			CFont* pOldFont = dcMem.SelectObject(&m_fontText);

			CString strActionInfo = m_arrThumbnailsInfo[i]->strActionName;
			if(m_bShowActionParams)
				strActionInfo += " " + m_arrThumbnailsInfo[i]->strParams;

			dcMem.DrawText(strActionInfo, &rcActionName, DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS );
			dcMem.DrawText(m_arrThumbnailsInfo[i]->strActionTime, &rcTime, DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS );

			dcMem.SelectObject( pOldFont );

			dcMem.DrawEdge(&rcBorderAroundText, EDGE_SUNKEN, BF_RECT);	

			dcMem.SelectObject(pOldBmp);
			}

			int nThumbnailDibRow = (sizeThumbnailUnit.cx*3+3)/4*4;

			if((m_dwFlags & atdfHorz) == atdfHorz)
			{
				//horizontal
				int nCurXStartDIB = 0;
				int nLeftPosX = (i+1)*m_nBorder + i*m_sizeThumbnail.cx;
				int nRightPosX = (i+1)*m_nBorder + (i+1)*m_sizeThumbnail.cx;
				if((rectDest.left < nRightPosX && rectDest.right > nLeftPosX) && rectDest.top < m_nBorder + sizeThumbnailUnit.cy && rectDest.bottom > m_nBorder)
				{
					//need to paint part or entire of i thumbnail
					if(!nCurXStartDIB)
						nCurXStartDIB = max(0, (i+1)*m_nBorder + i*m_sizeThumbnail.cx - rectDest.left);	
					int nCurYStartDIB = max(0, m_nBorder - rectDest.top);	
					
					int nStartXCurThumbnail = max(rectDest.left - (i+1)*m_nBorder - i*m_sizeThumbnail.cx, 0);
					int nEndXCurThumbnail = min(m_sizeThumbnail.cx, rectDest.right - (i+1)*m_nBorder - i*m_sizeThumbnail.cx);
					int nStartYCurThumbnail = max(rectDest.top - m_nBorder, 0);
					int nEndYCurThumbnail = min(sizeThumbnailUnit.cy, rectDest.bottom - m_nBorder);

					for(int y = nStartYCurThumbnail; y < nEndYCurThumbnail; y++)
					{
						for(int x = nStartXCurThumbnail; x < nEndXCurThumbnail; x++)
						{
							pdibBits[nDIBrow*(pbi->biHeight - (nCurYStartDIB+y-nStartYCurThumbnail) - 1) + (x - nStartXCurThumbnail + nCurXStartDIB)*3 + 0] = pdibThumbnail[(sizeThumbnailUnit.cy-y-1)*nThumbnailDibRow+x*3 + 0];
							pdibBits[nDIBrow*(pbi->biHeight - (nCurYStartDIB+y-nStartYCurThumbnail) - 1) + (x - nStartXCurThumbnail + nCurXStartDIB)*3 + 1] = pdibThumbnail[(sizeThumbnailUnit.cy-y-1)*nThumbnailDibRow+x*3 + 1];
							pdibBits[nDIBrow*(pbi->biHeight - (nCurYStartDIB+y-nStartYCurThumbnail) - 1) + (x - nStartXCurThumbnail + nCurXStartDIB)*3 + 2] = pdibThumbnail[(sizeThumbnailUnit.cy-y-1)*nThumbnailDibRow+x*3 + 2];
						}
						
					}
					nCurXStartDIB += m_nBorder + nEndXCurThumbnail - nStartXCurThumbnail;
				}
			}
			else
			{
				//vertical
				int nCurYStartDIB = 0;
				long nVertUnit = (m_bOnlyText ? 0 : m_sizeThumbnail.cy + m_nBorder) + GetTextLineHeight()*2 + TEXT_INTERLINE_HEIGHT;

				/*
				CRect rcItem;
				rcItem.left		= m_nBorder;
				rcItem.right	= rcItem.left + m_sizeThumbnail.cx;
				rcItem.top		= (nVertUnit+m_nBorder)*i + m_nBorder;
				rcItem.bottom	= rcItem.top + nVertUnit;

				CRect _rc;
				if( _rc.IntersectRect( &rcItem, &rectDest ) )
				*/

				if( rectDest.left	< m_sizeThumbnail.cx+m_nBorder && 
					rectDest.right	> m_nBorder && 
					rectDest.top	< (nVertUnit+m_nBorder)*(i+1) && 
					rectDest.bottom	> (nVertUnit+m_nBorder)*i + m_nBorder )
				{
					if(!nCurYStartDIB)
						nCurYStartDIB = max(0, i*(m_nBorder+nVertUnit) + m_nBorder - rectDest.top);	
					int nCurXStartDIB = max(0, m_nBorder - rectDest.left);	

					int nStartXCurThumbnail = max(rectDest.left - m_nBorder, 0);
					int nEndXCurThumbnail = min(m_sizeThumbnail.cx, rectDest.right - m_nBorder);
					int nStartYCurThumbnail = max(0, rectDest.top - (i*(m_nBorder+nVertUnit) + m_nBorder));
					int nEndYCurThumbnail = min(sizeThumbnailUnit.cy, rectDest.bottom - (i*(m_nBorder+nVertUnit) + m_nBorder));

					for(int y = nStartYCurThumbnail; y < nEndYCurThumbnail; y++)
					{
						for(int x = nStartXCurThumbnail; x < nEndXCurThumbnail; x++)
						{
							pdibBits[nDIBrow*(pbi->biHeight - (nCurYStartDIB+y-nStartYCurThumbnail) - 1) + (x - nStartXCurThumbnail + nCurXStartDIB)*3 + 0] = pdibThumbnail[(sizeThumbnailUnit.cy-y-1)*nThumbnailDibRow+x*3 + 0];
							pdibBits[nDIBrow*(pbi->biHeight - (nCurYStartDIB+y-nStartYCurThumbnail) - 1) + (x - nStartXCurThumbnail + nCurXStartDIB)*3 + 1] = pdibThumbnail[(sizeThumbnailUnit.cy-y-1)*nThumbnailDibRow+x*3 + 1];
							pdibBits[nDIBrow*(pbi->biHeight - (nCurYStartDIB+y-nStartYCurThumbnail) - 1) + (x - nStartXCurThumbnail + nCurXStartDIB)*3 + 2] = pdibThumbnail[(sizeThumbnailUnit.cy-y-1)*nThumbnailDibRow+x*3 + 2];
						}
						
					}

					nCurYStartDIB += m_nBorder + nEndYCurThumbnail - nStartYCurThumbnail;

				}
			}

		}
		if(pdibCurrent)
			delete pdibCurrent;
		::DeleteObject(hDIBSection);
	}
	else
	{
		//single, draw active
		long nStartIdx = 0;
		CSize sizeDIB(0, 0);

		if(!m_bOnlyText)
		{
			for(int i = m_nActiveThumbnail; i >= 0; i--)
			{
				CSize size = m_arrSizes[i];
				if(size != CSize(0, 0))
				{
					sizeDIB = size;
					nStartIdx = i;
					break;
				}
			}

			if(sizeDIB == CSize(0, 0))
				return;
		}
		else
		{
			sizeDIB.cx = m_sizeThumbnail.cx*3;
		}
		CSize sizeUnit = sizeDIB;
		sizeUnit.cy += (m_bOnlyText?0:m_nBorder)+ GetTextLineHeight()*2 + TEXT_INTERLINE_HEIGHT;

		long nDiff = sizeUnit.cy - sizeDIB.cy;
		
		FILL_BITMAPBITMAPINFOHEADER(BitmapInfoCurrentDIB, sizeUnit);
		HBITMAP	hDIBSection = ::CreateDIBSection(0, (BITMAPINFO*)&BitmapInfoCurrentDIB, DIB_RGB_COLORS, (void**)&pdibCurrent, 0, 0);
		ASSERT(hDIBSection);

		cx4Bmp = (sizeUnit.cx*3+3)/4*4;
				
		if(!(rectDest.left < m_nBorder+sizeUnit.cx && rectDest.right > m_nBorder && rectDest.top < m_nBorder+sizeUnit.cy && rectDest.bottom > m_nBorder))
				return;

		long nLeftSrcDIB = max(0, rectDest.left - m_nBorder);
		long nTopSrcDIB = max(0, rectDest.top - m_nBorder);
		long nBottomSrcDIB = min(rectDest.bottom - m_nBorder, sizeUnit.cy);
		long nRightSrcDIB = min(rectDest.right - m_nBorder, sizeUnit.cx);

		if(nLeftSrcDIB >= nRightSrcDIB || nTopSrcDIB >= nBottomSrcDIB)
			return;


		if(!m_bOnlyText)
		{
			for(long i = nStartIdx; i <= m_nActiveThumbnail; i++)
			{
				if(!m_bOnlyText)
				{
					//generate cur DIB
					DifferenceUnit* pHead = m_arrDifferences[i];			
					
					while(pHead)
					{
						bool bColor = pHead->bPixelCount >> 7 == 1;
						int nCount = pHead->bPixelCount & 0x7f;
						
						if(bColor)
						{
							memcpy((pdibCurrent+(cx4Bmp*(pHead->nY+nDiff) + pHead->nX*3)), pHead->pbData, nCount*3);
						}
						else
						{
							for(int x = 0; x < nCount; x++)
							{
								byte nColor = pHead->pbData[x];
								*(pdibCurrent + cx4Bmp*(pHead->nY+nDiff) + (pHead->nX + x)*3 + 0) = nColor;
								*(pdibCurrent + cx4Bmp*(pHead->nY+nDiff) + (pHead->nX + x)*3 + 1) = nColor;
								*(pdibCurrent + cx4Bmp*(pHead->nY+nDiff) + (pHead->nX + x)*3 + 2) = nColor;
							}
						}
						pHead = pHead->pNext;
					}
				}
			}
		}

		{
			CClientDC dc(0);
			CDC dcMem;
			dcMem.CreateCompatibleDC(&dc);
			CBitmap* pBmp = CBitmap::FromHandle(hDIBSection);
			CBitmap* pOldBmp = dcMem.SelectObject(pBmp);
			
			dcMem.FillRect(&CRect(CPoint(0, m_bOnlyText ? 0 : sizeDIB.cy), sizeUnit), &CBrush(m_clrInactive));

			//draw text
			CRect rcBorderAroundText(0, (m_bOnlyText ? 0 : sizeDIB.cy+m_nBorder), sizeDIB.cx, sizeUnit.cy);
			CRect rcActionName = rcBorderAroundText;
			CRect rcTime = rcBorderAroundText;

			rcActionName.DeflateRect(2, 2);
			rcTime.DeflateRect(2, 2);

			rcTime.top = rcActionName.bottom = rcActionName.top + GetTextLineHeight();
			
			COLORREF colorText, colorBack;

			colorBack = m_clrInactive;
			colorText = m_clrInactiveText;
			
			dcMem.FillRect(&rcBorderAroundText, &CBrush(colorBack));

			dcMem.SetTextColor( colorText );
			dcMem.SetBkColor( colorBack );

			CFont* pOldFont = dcMem.SelectObject(&m_fontText);

			CString strActionInfo = m_arrThumbnailsInfo[m_nActiveThumbnail]->strActionName;
			if(m_bShowActionParams)
				strActionInfo += " " + m_arrThumbnailsInfo[m_nActiveThumbnail]->strParams;

			dcMem.DrawText(strActionInfo, &rcActionName, DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
			dcMem.DrawText(m_arrThumbnailsInfo[m_nActiveThumbnail]->strActionTime, &rcTime, DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			dcMem.SelectObject( pOldFont );

			dcMem.DrawEdge(&rcBorderAroundText, EDGE_SUNKEN, BF_RECT);	

			dcMem.SelectObject(pOldBmp);
		}


		long nLeftDstDIB = max(0, m_nBorder - rectDest.left);
		long nTopDstDIB = max(0, m_nBorder - rectDest.top);

		for(long y = nTopSrcDIB; y < nBottomSrcDIB; y++)
		{
			for(long x = nLeftSrcDIB; x < nRightSrcDIB; x++)
			{
				pdibBits[nDIBrow*(pbi->biHeight - (nTopDstDIB+y-nTopSrcDIB) - 1) + (x - nLeftSrcDIB + nLeftDstDIB)*3 + 0] = pdibCurrent[(sizeUnit.cy-y-1)*cx4Bmp+x*3 + 0];
				pdibBits[nDIBrow*(pbi->biHeight - (nTopDstDIB+y-nTopSrcDIB) - 1) + (x - nLeftSrcDIB + nLeftDstDIB)*3 + 1] = pdibCurrent[(sizeUnit.cy-y-1)*cx4Bmp+x*3 + 1];
				pdibBits[nDIBrow*(pbi->biHeight - (nTopDstDIB+y-nTopSrcDIB) - 1) + (x - nLeftSrcDIB + nLeftDstDIB)*3 + 2] = pdibCurrent[(sizeUnit.cy-y-1)*cx4Bmp+x*3 + 2];
			}
		}

		DeleteObject(hDIBSection);

	}
}

long CAuditTrailObject::GetThumbnailFromPoint(CPoint ptCoord)
{
	bool bHorz = (m_dwFlags & atdfHorz) == atdfHorz;
	long nIdx = -1;
	if(bHorz)
	{
		long nBottomRangeThumb = m_nBorder;
		long nTopRangeThumb = nBottomRangeThumb + m_sizeThumbnail.cy;
		long nBottomRangeText = (m_bOnlyText ? 0 : m_nBorder + m_sizeThumbnail.cy) + m_nBorder;
		long nTopRangeText = nBottomRangeText + GetTextLineHeight()*2 + TEXT_INTERLINE_HEIGHT;
		
		if((ptCoord.y > nBottomRangeThumb && ptCoord.y < nTopRangeThumb && !m_bOnlyText) || (ptCoord.y > nBottomRangeText && ptCoord.y < nTopRangeText))
		{
			int n = ptCoord.x/(m_nBorder + m_sizeThumbnail.cx);
			if((ptCoord.x > (n+1)*m_nBorder + n*m_sizeThumbnail.cx) && (ptCoord.x < (n+1)*m_nBorder + (n+1)*m_sizeThumbnail.cx))
				nIdx = n;
		}
	}
	else
	{
		if(ptCoord.x > m_nBorder && ptCoord.x < m_nBorder + m_sizeThumbnail.cx)
		{
			long nVertUnit = (m_bOnlyText ? 0 : m_nBorder + m_sizeThumbnail.cy) + m_nBorder + GetTextLineHeight()*2 + TEXT_INTERLINE_HEIGHT;
			int n = ptCoord.y/nVertUnit;

			long nBottomRangeThumb = n*nVertUnit + m_nBorder;
			long nTopRangeThumb = (n+1)*nVertUnit;
			long nTopRangeText = (n+1)*nVertUnit/* + m_nBorder*/;
			long nBottomRangeText = nTopRangeText - GetTextLineHeight()*2 - TEXT_INTERLINE_HEIGHT;

			if((ptCoord.y > nBottomRangeThumb && ptCoord.y < nTopRangeThumb && !m_bOnlyText) || (ptCoord.y > nBottomRangeText && ptCoord.y < nTopRangeText))
				nIdx = n;
		}
	}

	nIdx = min(nIdx, GetThumbnailsCount()-1);

	return nIdx;
}

CRect CAuditTrailObject::GetThumbnailRect(long nIndex)
{
	bool bHorz = (m_dwFlags & atdfHorz) == atdfHorz;

	CRect rc = NORECT;
	long nThumbHeight = m_bOnlyText ? 0 : m_sizeThumbnail.cy + m_nBorder;

	if(bHorz)
		rc = CRect(nIndex*m_sizeThumbnail.cx + (nIndex + 1)*m_nBorder, m_nBorder, 0, 0);
	else
		rc = CRect(m_nBorder, (m_nBorder + nThumbHeight + GetTextLineHeight()*2 + TEXT_INTERLINE_HEIGHT)*nIndex + m_nBorder, 0, 0);

	rc.right = rc.left + m_sizeThumbnail.cx;
	rc.bottom = rc.top + nThumbHeight + GetTextLineHeight()*2 + TEXT_INTERLINE_HEIGHT;

	return rc;
}

void CAuditTrailObject::SetActiveThumbnail(long nIndex)
{
	if(nIndex < 0 || nIndex == m_nActiveThumbnail || nIndex >= GetThumbnailsCount())
		return;

	long nElements = (((short)m_nActiveThumbnail)<<16)|((short)nIndex);

	m_nActiveThumbnail = nIndex;
	
	::FireEvent(m_punkNamedData, szEventChangeObject, GetControllingUnknown(), 0, &nElements, sizeof(nElements) );	
}
	
long CAuditTrailObject::GetTextLineHeight()
{
	CClientDC dc(0);
	CFont* pOldFont = dc.SelectObject(&m_fontText);

	TEXTMETRIC tm;
	dc.GetTextMetrics( &tm );					
	dc.SelectObject( pOldFont );

	return tm.tmHeight;
}

bool CAuditTrailObject::UndoAction(GuidKey keyAction, GuidKey* pKeyImage)
{
	int nLastIdx = m_arrThumbnailsInfo.GetSize()-1;

	if(m_arrThumbnailsInfo[nLastIdx]->keyAction != keyAction)
		return false;

	if(pKeyImage)
	{
		ASSERT(nLastIdx-1 >= 0);
		*pKeyImage = m_arrThumbnailsInfo[nLastIdx-1]->keyImage;
	}

	CleanUpDifferences(m_arrDifferences.GetSize()-1);

	CleanUpInfos(nLastIdx);
	
	RegenerateCurrentDIB();		
	
	m_nFromPureBlackCount--;
	if(m_nActiveThumbnail == nLastIdx)
		m_nActiveThumbnail = nLastIdx - 1;
	
	::UpdateDataObject(m_punkNamedData, GetControllingUnknown());

	return true;
}

void CAuditTrailObject::CleanUpDifferences(long nIndex)
{
	int nSize = m_arrDifferences.GetSize();
	if(nIndex >= nSize)
		return;
	int nStart = 0, nEnd = nSize - 1;
	if(nIndex >= 0 && nIndex < nSize)
		nStart = nEnd = nIndex;
		
	for(int i = nEnd; i >= nStart; i--)
	{
		DifferenceUnit* pHead = m_arrDifferences[i];
		while(pHead)
		{
			DifferenceUnit* pNext = pHead->pNext;
			delete pHead->pbData;
			delete pHead;
			pHead = pNext;
		}
		m_arrDifferences.RemoveAt(i);
		m_arrSizes.RemoveAt(i);
	}
}

void CAuditTrailObject::CleanUpInfos(long nIndex)
{
	if(nIndex >= 0)
	{
		delete m_arrThumbnailsInfo[nIndex];
		m_arrThumbnailsInfo.RemoveAt(nIndex);
	}
	else
	{
		long nSize = m_arrThumbnailsInfo.GetSize();
		for(long i = 0; i < nSize; i++)
			delete m_arrThumbnailsInfo[i];

		m_arrThumbnailsInfo.RemoveAll();
	}
	
}

long CAuditTrailObject::GetDifferenceSize(DifferenceUnit* pHead)
{
	long nTotalBytes = 0;
	while(pHead)
	{
		long nPixelsByte = (pHead->bPixelCount>>7) ? (pHead->bPixelCount & 0x7f)*3 : pHead->bPixelCount;

		nTotalBytes += nPixelsByte + sizeof(DifferenceUnit);

		pHead = pHead->pNext;
	}

	return nTotalBytes;
}

bool CAuditTrailObject::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	if(!CDataObjectBase::SerializeObject(ar, pparams))
		return false;

	if(ar.IsStoring())
	{
		long nSize = 0;
		long i = 0;
		//store differences
		nSize = m_arrDifferences.GetSize();
		ar<<nSize;
		for(i = 0; i < nSize; i++)
		{
			DifferenceUnit* pHead = m_arrDifferences[i];
			long nCount = 0;
			while(pHead)
			{
				pHead = pHead->pNext;
				nCount++;
			}
			ar<<nCount;
			pHead = m_arrDifferences[i];
			while(pHead)
			{
				ar<<pHead->bPixelCount;
				long nPixelsByte = (pHead->bPixelCount>>7) ? (pHead->bPixelCount & 0x7f)*3 : pHead->bPixelCount;
				if(nPixelsByte > 0)
					ar.Write(pHead->pbData, nPixelsByte);
				ar<<pHead->nX;
				ar<<pHead->nY;
				pHead = pHead->pNext;
			}
		}
		
		//store sizes
		nSize = m_arrSizes.GetSize();
		ar<<nSize;
		long nCount = 0;
		for(i = 0; i < nSize; i++)
		{
			if(m_arrSizes[i] != CSize(0, 0))
				nCount++;
		}
		ar<<nCount;
		for(i = 0; i < nSize; i++)
		{
			if(m_arrSizes[i] != CSize(0, 0))
			{
				ar<<i;
				ar<<m_arrSizes[i];
			}
		}
		

		//store ThumbnailsInfo
		nSize = m_arrThumbnailsInfo.GetSize();
		ar<<nSize;
		for(i = 0; i < nSize; i++)
		{
			ar<<m_arrThumbnailsInfo[i]->strActionName;
			ar<<m_arrThumbnailsInfo[i]->strActionTime;
		}
		
		//store stuff
		WORD wVersion = 1;

		BOOL booool = MAKELONG( m_bOnlyText, wVersion );
		ar<<(BOOL)booool;		

		//ar<<m_clrActive;
		//ar<<m_clrInactive;
		//ar<<m_clrActiveText;
		//ar<<m_clrInactiveText;
		ar<<m_sizeThumbnail;
		ar<<m_nBorder;

		ar<<m_lGridDataSize;
		if( m_lGridDataSize > 0 )
			ar.Write( m_pGridData, m_lGridDataSize );
	}
	else
	{
		long nSize = 0, i = 0, j = 0;
		//restore differences
		CleanUpDifferences();
		ar>>nSize;
		m_arrDifferences.SetSize(nSize);
		for(i = 0; i < nSize; i++)
		{
			DifferenceUnit* pLast = 0;
			DifferenceUnit* pHead = 0;
			long nCount = 0;
			ar>>nCount;
			for(j = 0; j < nCount; j++)
			{
				DifferenceUnit* pCur = new DifferenceUnit;
				if(!pHead)
					pHead = pCur;
				if(pLast)
					pLast->pNext = pCur;
				pLast = pCur;
				pCur->pNext = 0;
				ar>>pCur->bPixelCount;
				long nPixelsByte = (pCur->bPixelCount>>7) ? (pCur->bPixelCount & 0x7f)*3 : pCur->bPixelCount;
				if(nPixelsByte > 0)
				{
					pCur->pbData = new byte[nPixelsByte];
					ar.Read(pCur->pbData, nPixelsByte);
				}
				ar>>pCur->nX;
				ar>>pCur->nY;
			}
			m_arrDifferences.SetAt(i, pHead);
		}

		//restore sizes
		m_arrSizes.RemoveAll();
		ar>>nSize;
		m_arrSizes.SetSize(nSize);
		long nCount = 0;
		ar>>nCount;
		long nCurIdx = 0;
		for(i = 0; i < nCount; i++)
		{
			ASSERT(nCurIdx < nSize);
			long nIdx;
			ar>>nIdx;
			for(j = nCurIdx; j < nIdx; j++)
				m_arrSizes.SetAt(j, CSize(0, 0));
			ar>>m_arrSizes[nIdx];

			nCurIdx = nIdx+1;
		}

		//restore ThumbnailsInfo
		CleanUpInfos();		
		ar>>nSize;
		m_arrThumbnailsInfo.SetSize(nSize);
		for(i = 0; i < nSize; i++)
		{
			ThumbnailInfo* pInfo = new ThumbnailInfo;
			ar>>pInfo->strActionName;
			ar>>pInfo->strActionTime;
			
			m_arrThumbnailsInfo.SetAt(i, pInfo);
		}

		//restore stuff
		BOOL boooool;
		

		ar>>boooool;

		WORD l = LOWORD(boooool);
		WORD h = HIWORD(boooool);		

		m_bOnlyText = l == 1;
		WORD wVersion = h;
		//ar>>m_clrActive;
		//ar>>m_clrInactive;
		//ar>>m_clrActiveText;
		//ar>>m_clrInactiveText;
		ar>>m_sizeThumbnail;
		ar>>m_nBorder;

		if( wVersion >= 1 )
		{
			_deinit_grid_data();
			ar>>m_lGridDataSize;
			if( m_lGridDataSize > 0 )
			{
				m_pGridData = new BYTE[m_lGridDataSize];
				ar.Read( m_pGridData, m_lGridDataSize );
			}
		}

		::UpdateDataObject(m_punkNamedData, GetControllingUnknown());
	}

	return true;
}

void CAuditTrailObject::RegenerateCurrentDIB()
{
	if(m_pCurrentDIB)
	{
		delete m_pCurrentDIB;
		m_pCurrentDIB = 0;
	}

	int nLast = m_arrDifferences.GetSize()-1;

	for(int i = nLast; i >= 0; i--)
	{
		CSize size = m_arrSizes[i];
		if(size != CSize(0, 0))
		{
			long nDibSize = ((size.cx*3+3)/4*4)*size.cy;
			m_sizeCurrentDIB = size;
			//make pure black DIB
			m_pCurrentDIB = new byte[nDibSize];
			ZeroMemory(m_pCurrentDIB, nDibSize); 

			long cx4Bmp = (size.cx*3+3)/4*4;

			//fill from differences
			for(int j = i+1; j <= nLast; j++)
			{
				DifferenceUnit* pHead = m_arrDifferences[j];
				while(pHead)
				{
					bool bColor = pHead->bPixelCount >> 7 == 1;
					int nCount = pHead->bPixelCount & 0x7f;
					
					if(bColor)
					{
						memcpy((m_pCurrentDIB+(cx4Bmp*pHead->nY + pHead->nX*3)), pHead->pbData, nCount*3);
					}
					else
					{
						for(int x = 0; x < nCount; x++)
						{
							byte nColor = pHead->pbData[x];
							*(m_pCurrentDIB + cx4Bmp*pHead->nY + (pHead->nX + x)*3 + 0) = nColor;
							*(m_pCurrentDIB + cx4Bmp*pHead->nY + (pHead->nX + x)*3 + 1) = nColor;
							*(m_pCurrentDIB + cx4Bmp*pHead->nY + (pHead->nX + x)*3 + 2) = nColor;
						}
					}
					pHead = pHead->pNext;
				}
			}

			break;
		}
	}

}

bool CAuditTrailObject::GetPrintWidth(int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX)
{
	bool bContinue = false;

	long nUnitX = m_sizeThumbnail.cx+m_nBorder;
	long nCountInWidth = nMaxWidth/nUnitX;
	long nCurCount = nUserPosX/nUnitX;

	if((m_dwFlags & atdfHorz) == atdfHorz && nCountInWidth)
	{
		long nCount = GetThumbnailsCount();
		if(nCount > nCountInWidth + nCurCount)
		{
			nReturnWidth = nCountInWidth*nUnitX;
			bContinue = true;
		}
		else
		{
			nReturnWidth = (nCount - nCurCount)*nUnitX;
		}
		
	}
	else
	{
		CSize sizeDoc = GetDocumentSize();
		if(sizeDoc.cx > nUserPosX + nMaxWidth)
		{
			nReturnWidth = nMaxWidth;
			bContinue = true;
		}
		else
			nReturnWidth = sizeDoc.cx;

	}

	nNewUserPosX =  nUserPosX + nReturnWidth;

	return bContinue;
}

bool CAuditTrailObject::GetPrintHeight(int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY)
{
	bool bContinue = false;

	long nUnitY = m_sizeThumbnail.cy + 2 * m_nBorder + GetTextLineHeight()*2 + TEXT_INTERLINE_HEIGHT;
	
	long nCountInHeight = nMaxHeight/nUnitY;
	long nCurCount = nUserPosY/nUnitY;

	if((m_dwFlags & atdfVert) == atdfVert && nCountInHeight)
	{
		long nCount = GetThumbnailsCount();
		if(nCount > nCountInHeight + nCurCount)
		{
			nReturnHeight = nCountInHeight*nUnitY;
			bContinue = true;
		}
		else
		{
			nReturnHeight = (nCount - nCurCount)*nUnitY;
		}
		
	}
	else
	{
		CSize sizeDoc = GetDocumentSize();
		if(sizeDoc.cy > nUserPosY + nMaxHeight)
		{
			nReturnHeight = nMaxHeight;
			bContinue = true;
		}
		else
			nReturnHeight = sizeDoc.cy;

	}

	nNewUserPosY =  nUserPosY + nReturnHeight;
	return bContinue;
}

void CAuditTrailObject::Print(HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags)
{
	CRect rcSource = CRect(nUserPosX, nUserPosY, nUserPosX+nUserPosDX, nUserPosY+nUserPosDY);

	BITMAPINFOHEADER BitmapHdr;
	FILL_BITMAPBITMAPINFOHEADER(BitmapHdr, CSize(rcSource.Width(), rcSource.Height()));
	
	byte	*pdibBits = 0;
	HBITMAP	hDIBSection = ::CreateDIBSection(hdc, (BITMAPINFO*)&BitmapHdr, DIB_RGB_COLORS, (void**)&pdibBits, 0, 0 );
	ASSERT(hDIBSection);

	for(int y = 0; y < BitmapHdr.biHeight; y++)
	{
		int nRowOffset = (BitmapHdr.biWidth*3+3)/4*4*y;
		for(int x = 0; x < BitmapHdr.biWidth; x++)
		{
			pdibBits[nRowOffset + 3*x + 2] = GetRValue(m_clrInactive);
			pdibBits[nRowOffset + 3*x + 1] = GetGValue(m_clrInactive);
			pdibBits[nRowOffset + 3*x + 0] = GetBValue(m_clrInactive);
		}
	}

	Draw(&BitmapHdr, pdibBits, rcSource, false);

	::DrawDibDraw(g_DrawDibInstance, hdc, rectTarget.left, rectTarget.top, rectTarget.Width(), rectTarget.Height(), &BitmapHdr, pdibBits, 0, 0, rcSource.Width(), rcSource.Height(), 0);

	DeleteObject(hDIBSection);		
}