#include "stdafx.h"
#include "comments.h" 
#include "math.h"
#include "actions.h"
#include "resource.h"

#include "resource.h"


const char *CCommentsObjectList::c_szType = szDrawingObjectList;
const char *CCommentsObject::c_szType = szDrawingObject;

static bool s_bCanSelectGDI = true;

IMPLEMENT_DYNCREATE(CCommentsObjectList, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CCommentsObject, CCmdTargetEx);

BEGIN_INTERFACE_MAP(CCommentsObject, CDataObjectBase)
	INTERFACE_PART(CCommentsObject, IID_IDrawObject, Draw)
	INTERFACE_PART(CCommentsObject, IID_ICommentObj, CommObj)
	INTERFACE_PART(CCommentsObject, IID_IFontPropPage, FontPage)
	INTERFACE_PART(CCommentsObject, IID_IRectObject, Rect)
END_INTERFACE_MAP()

BEGIN_INTERFACE_MAP(CCommentsObjectList, CDataObjectListBase)
	INTERFACE_PART(CCommentsObjectList, IID_IDrawObject, Draw)
END_INTERFACE_MAP()



// {4099DEE5-92D5-11d3-A52C-0000B493A187}
static const GUID clsidDataObjectList = 
{ 0x4099dee5, 0x92d5, 0x11d3, { 0xa5, 0x2c, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {4099DEE6-92D5-11d3-A52C-0000B493A187}
static const GUID clsidDataObject = 
{ 0x4099dee6, 0x92d5, 0x11d3, { 0xa5, 0x2c, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {4099DEE7-92D5-11d3-A52C-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CCommentsObjectList, "Editor.CommentsObjectList", 
0x4099dee7, 0x92d5, 0x11d3, 0xa5, 0x2c, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {4099DEE8-92D5-11d3-A52C-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CCommentsObject, "Editor.CommentsObject", 
0x4099dee8, 0x92d5, 0x11d3, 0xa5, 0x2c, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);



DATA_OBJECT_INFO(IDS_COMMENTLIST_TYPE, CCommentsObjectList, CCommentsObjectList::c_szType, clsidDataObjectList, IDI_DRAWOBJECTS );
DATA_OBJECT_INFO_FULL(IDS_COMMENTOBJ_TYPE, CCommentsObject, CCommentsObject::c_szType, CCommentsObjectList::c_szType, clsidDataObject, IDI_DRAWOBJECTS );

IMPLEMENT_UNKNOWN(CCommentsObject, CommObj);
IMPLEMENT_UNKNOWN(CCommentsObject, Rect);

/////////////////////////////////////////////////////////////////////////////////////////
//native interface implementation
HRESULT CCommentsObject::XCommObj::GetText(BSTR *pbstrText)
{
	_try_nested(CCommentsObject, CommObj, GetText)
	{
		if(pbstrText)
			*pbstrText = pThis->m_strText.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CCommentsObject::XCommObj::SetText(BSTR bstrText)
{
	_try_nested(CCommentsObject, CommObj, SetText)
	{
		pThis->m_strText = bstrText;
		pThis->UpdateObject();
		pThis->_SetRectByText();
		pThis->UpdateObject();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CCommentsObject::XCommObj::GetCoordinates(RECT* prcText, POINT* pPtArrow, BOOL bFull )
{
	_try_nested(CCommentsObject, CommObj, GetCoordinates)
	{
		if(prcText)
		{
			CRect rc = pThis->m_rcText;
			*prcText = bFull && pThis->m_rcLastDraw.Width() > rc.Width() ? pThis->m_rcLastDraw : rc;
		}
		if(pPtArrow)
			*pPtArrow = pThis->m_ptArrow;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CCommentsObject::XCommObj::SetCoordinates(RECT rcText, POINT ptArrow)
{
	_try_nested(CCommentsObject, CommObj, SetCoordinates)
	{
		pThis->UpdateObject();
		pThis->m_rcText = CRect(rcText.left, rcText.top, rcText.left+pThis->m_rcText.Width(), rcText.top+pThis->m_rcText.Height());
		pThis->m_ptArrow = ptArrow;
		pThis->UpdateObject();

		//CRect rcInvalid = pThis->DoGetRect();
		//pThis->m_rcInvalid = NORECT;
		//rcInvalid.UnionRect(&rcInvalid , &pThis->DoGetRect());
		//rcInvalid.InflateRect(5,5,5,5);
		//pThis->m_rcInvalid = rcInvalid;
		//pThis->UpdateObject(rcInvalid);
		//pThis->UpdateObject();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CCommentsObject::XCommObj::SetEditMode(BOOL bEdit)
{
	_try_nested(CCommentsObject, CommObj, SetEditMode)
	{
		pThis->m_bEditMode = bEdit;
		pThis->UpdateObject();
		return S_OK;
	}
	_catch_nested;
}


HRESULT CCommentsObject::XCommObj::LockDraw(BOOL bLock)
{
	_try_nested(CCommentsObject, CommObj, LockDraw)
	{
		pThis->m_bLockDraw = bLock;
		return S_OK;
	}
	_catch_nested;
}


HRESULT CCommentsObject::XCommObj::UpdateObject()
{
	_try_nested(CCommentsObject, CommObj, UpdateObject)
	{
		pThis->UpdateObject();
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////////////////
//IFontPropPage interface implementation
IMPLEMENT_UNKNOWN(CCommentsObject, FontPage);
HRESULT CCommentsObject::XFontPage::SetFont(LOGFONT*	pLogFont, int nAlign)
{
	_try_nested(CCommentsObject, FontPage, SetFont)
	{
		memcpy(&pThis->m_logFont, pLogFont, sizeof(LOGFONT));
		switch(nAlign)
		{
		case 1:
			pThis->m_nAlign = DT_CENTER;
			break;
		case 2:
			pThis->m_nAlign = DT_RIGHT;
			break;
		default:
			pThis->m_nAlign = DT_LEFT;
			break;
		}

		pThis->UpdateObject();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CCommentsObject::XFontPage::SetColorsAndStuff(OLE_COLOR colorText, OLE_COLOR colorBack, BOOL bBack, BOOL bArrow)
{
	_try_nested(CCommentsObject, FontPage, SetColorsAndStuff)
	{
		pThis->m_color = (COLORREF)colorText;
		pThis->m_backColor = (COLORREF)colorBack;
		pThis->m_bTransparent = !bBack;
		pThis->m_bDrawArrow = (bArrow==TRUE);
		pThis->UpdateObject();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CCommentsObject::XFontPage::GetColorsAndStuff(OLE_COLOR* pcolorText, OLE_COLOR* pcolorBack, BOOL* pbBack, BOOL* pbArrow)
{
	_try_nested(CCommentsObject, FontPage, GetColorsAndStuff)
	{
		if(pcolorBack)
			*pcolorBack = (OLE_COLOR)pThis->m_backColor;
		if(pcolorText)
			*pcolorText  = (OLE_COLOR)pThis->m_color;
		if(pbBack)
			*pbBack = !pThis->m_bTransparent;
		if(pbArrow)
			*pbArrow = pThis->m_bDrawArrow;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CCommentsObject::XFontPage::GetFont(LOGFONT*	pLogFont, int* pnAlign)
{
	_try_nested(CCommentsObject, FontPage, GetFont)
	{
		if(pLogFont)
			memcpy(pLogFont, &pThis->m_logFont, sizeof(LOGFONT));
		if(pnAlign)
		{
			switch(pThis->m_nAlign)
			{
			case DT_CENTER:
				*pnAlign = 1;
				break;
			case DT_RIGHT:
				*pnAlign = 2;
				break;
			default:
				*pnAlign = 0;
				break;
			}
		}
		return S_OK;
	}
	_catch_nested;
}



/////////////////////////////////////////////////////////////////////////////////////////
void CCommentsObjectList::DoDraw( CDC &dc )
{
	sptrINamedDataObject2 sptrList(GetControllingUnknown());
	sptrIDrawObject sptrDrawObj = 0;
	IUnknown* punk = 0;
	if(sptrList == 0)return;
	long POS = 0;
	sptrList->GetFirstChildPosition( &POS );
	while(POS)
	{
		sptrList->GetNextChild( &POS, &punk );
		sptrDrawObj = punk;
		if(punk)
		{
			punk->Release();
			punk = 0;
		}
		if(sptrDrawObj == 0)continue;
		
		sptrDrawObj->Paint(dc.GetSafeHdc(), m_rcInvalid, m_punkDrawTarget, m_pbi, m_pdib );
		
	}
	
}

/////////////////////////////////////////////////////////////////////////////////////////
CCommentsObject::CCommentsObject() : 
	m_ptArrow(-1, -1)
{
	m_strText = "Uninitialized text object";
	m_rcText = NORECT;
	m_color = RGB( 0, 0, 0 );
	m_backColor = RGB( 255, 255, 255 );
	m_bTransparent = true;
	m_bDrawArrow = false;
	m_nAlign = DT_LEFT;
	m_rcLastDraw = NORECT;
	m_bLockDraw = FALSE;

	ZeroMemory(&m_logFont, sizeof(LOGFONT));
}

CCommentsObject::~CCommentsObject()
{	
}


/////////////////////////////////////////////////////////////////////////////////////////
//overrided virtuals
DWORD CCommentsObject::GetNamedObjectFlags()
{
	return nofNormal|nofStoreByParent;
}


bool CCommentsObject::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	if( ar.IsStoring() )
	{
		long	lVersion = 2;

		ar<<lVersion;	//
		ar<<m_strText<<m_rcText.left<<m_rcText.top<<m_rcText.right<<m_rcText.bottom<<m_ptArrow.x<<m_ptArrow.y;
		ar<<m_nAlign<<(long)m_bDrawArrow<<(long)m_color<<(long)m_backColor<<(long)m_bTransparent;
		ar.Write(&m_logFont, sizeof(LOGFONT));
	}
	else
	{
		long	lVersion = 1;
		ar>>lVersion;

		if( lVersion < 1 )
			return false;	//invalid version
		m_rcText = NORECT;
		m_ptArrow = CPoint(0,0);
		ar>>m_strText>>m_rcText.left>>m_rcText.top>>m_rcText.right>>m_rcText.bottom>>m_ptArrow.x>>m_ptArrow.y;

		if( lVersion < 2 )
			return true;	//invalid version


		long	lArrow, lTransparent;
		ar>>m_nAlign>>lArrow>>m_color>>m_backColor>>lTransparent;
		ar.Read(&m_logFont, sizeof(LOGFONT));
	
		m_bDrawArrow  = lArrow != 0;
		
		m_bTransparent = lTransparent != 0;
	}
	return true;
}

void CCommentsObject::DoDraw( CDC &dc )
{
	double fdisplay_dpi = 96.;
	{
 		CClientDC dc_screen( 0 );
		fdisplay_dpi = ::GetDeviceCaps( dc_screen.m_hDC, LOGPIXELSY );
	}

	int nDX = m_logFont.lfHeight/5;
	_SetRectByText();

	if(m_bLockDraw)return;
	_try(CCommentsObject, DoDraw)
	{
		double Angle = ArrAngle*PI/180;

		CPen pen(PS_SOLID, 1, m_color);
		CBrush brush(m_color);


		CPen* pOldPen = 0;
		CBrush* pOldBrush = 0;

		if(s_bCanSelectGDI)
		{
			pOldPen = dc.SelectObject(&pen);
			pOldBrush = dc.SelectObject(&brush);
		}
					

		dc.SetROP2(R2_COPYPEN);

		CPoint ptArrow = m_ptArrow;
		CRect rc = m_rcText;
		rc = ConvertToWindow( m_punkDrawTarget, rc );
//		rc.OffsetRect( 0, rc.Height() / 2 );

		CFont fontText;
		LOGFONT logfont;
		memcpy(&logfont, &m_logFont, sizeof(LOGFONT));
		logfont.lfHeight *= GetZoom(m_punkDrawTarget);
		fontText.CreateFontIndirect(&logfont);
		CFont* pOldFont = dc.SelectObject(&fontText);
		
		if(ptArrow != CPoint(-1, -1))
		{
			ptArrow = ConvertToWindow( m_punkDrawTarget, ptArrow );
			long textX = rc.left;
			long textY = rc.top;
			if(ptArrow.x > textX+5 && ptArrow.y > textY+5)
			{
				//long nTextHeight = rcRecalc.Height();
				//long nTextWidth = rcRecalc.Width();
				long nTextHeight = rc.Height();
				long nTextWidth = rc.Width();
				long dY = ptArrow.y - textY;
				long dX = ptArrow.x - textX;
				if(nTextHeight*1.0/nTextWidth < dY*1.0/dX)
				{
					textX += (nTextHeight*dX)/dY;
					textY += nTextHeight;
				}
				else
				{
					textX += nTextWidth;
					textY += (nTextWidth*dY)/dX; 
				}
			}
			
			if(!rc.PtInRect(ptArrow))
			{
				dc.MoveTo(ptArrow);
				dc.LineTo(textX, textY);
				long X = ptArrow.x;
				long Y = ptArrow.y;
				long Xo = rc.left;
				long Yo = rc.top;
				double length = sqrt(double(pow(double(X-Xo),2)+pow(double(Y-Yo), 2)));
				if(m_bDrawArrow && (ArrHeight <= length))
				{	
				
					double length = sqrt(double(pow(double(X-Xo),2)+pow(double(Y-Yo), 2)));
					if(X != Xo || Y != Yo)
					{
						//double ATAN;
						//ATAN = Y==Yo?pi/2:atan((X-Xo)/(Y-Yo));

						//double fVal = MulDiv(1, GetDeviceCaps(dc, LOGPIXELSY), 83) * ::GetZoom( m_punkDrawTarget );

						double ATAN = atan2(double(X-Xo), double(Y-Yo));
						POINT points[3];
						points[0].x = X;
						points[0].y = Y;

						/*
						points[1].x = X - (X-Xo)*ArrHeight/length + ArrHeight*tan(Angle/2)*cos(ATAN);
						points[1].y = Y - (Y-Yo)*ArrHeight/length - ArrHeight*tan(Angle/2)*sin(ATAN);
						points[2].x = X - (X-Xo)*ArrHeight/length - ArrHeight*tan(Angle/2)*cos(ATAN);
						points[2].y = Y - (Y-Yo)*ArrHeight/length + ArrHeight*tan(Angle/2)*sin(ATAN);
						*/						

						
						double fVal = 1.;//MulDiv(1, GetDeviceCaps(dc, LOGPIXELSY), 83) * ::GetZoom( m_punkDrawTarget );
						double fdevice_dpi = ::GetDeviceCaps( dc.m_hDC, LOGPIXELSY );
						if( fdevice_dpi > 0 && fdisplay_dpi > 0 )
						{
							fVal = 1;//fdevice_dpi / fdisplay_dpi;
							//if( fdevice_dpi == fdisplay_dpi )
								 fVal *= GetZoom( m_punkDrawTarget );
						}
						double dArrHeight = min(ArrHeight*fVal,length);

						points[1].x = X - (X-Xo)*dArrHeight/length + dArrHeight*tan(Angle/2)*cos(ATAN);
						points[1].y = Y - (Y-Yo)*dArrHeight/length - dArrHeight*tan(Angle/2)*sin(ATAN);
						points[2].x = X - (X-Xo)*dArrHeight/length - dArrHeight*tan(Angle/2)*cos(ATAN);
						points[2].y = Y - (Y-Yo)*dArrHeight/length + dArrHeight*tan(Angle/2)*sin(ATAN);
						
						dc.Polygon(points, 3);
					}
				}
			}
		}

		if(s_bCanSelectGDI)
			dc.SetTextColor(m_color);		
		
		if(pOldPen)		dc.SelectObject(pOldPen);
		if(pOldBrush)	dc.SelectObject(pOldBrush);
		
		/*
		rc.right = rc.left + rcRecalc.Width();
		rc.bottom = rc.top + rcRecalc.Height();


		m_rcText = ConvertToClient( m_punkDrawTarget, rc );


		ASSERT(m_rcText != NORECT);
		*/
		
//		rc.OffsetRect( 0, -rc.Height() / 2 );
		if(!m_strText.IsEmpty())
		{
			if( !s_bCanSelectGDI )
			{
				dc.FillRect( &rc, &CBrush( RGB( 255, 255, 255 ) ) );
			}
			else
			{
				if(m_bTransparent)
				{
					dc.SetBkMode( TRANSPARENT);
				}
				else
				{
					dc.SetBkMode(OPAQUE);
					CBrush br(m_backColor);
					dc.SetBkColor(m_backColor);
					dc.FillRect(&rc, &br);
				}

				{
					double fVal = MulDiv(1, GetDeviceCaps(dc, LOGPIXELSY), 83) * ::GetZoom( m_punkDrawTarget );
					fVal = 1;
					rc.right -= nDX*fVal;
				}

				dc.DrawText(m_strText, &rc, m_nAlign | DT_NOCLIP);
			}
		}

		//if(!m_bTransparent)
		//	dc.SelectObject(pOldBrush);


		
		if(m_bEditMode == TRUE)
		{
			CRect rcHot = NORECT;
			//pOldBrush = (CBrush*)dc.SelectStockObject(WHITE_BRUSH);
			//pOldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
			dc.SelectStockObject(WHITE_BRUSH);
			dc.SelectStockObject(BLACK_PEN);
			dc.SetROP2(R2_COPYPEN);
			if(m_ptArrow != CPoint(-1,-1))
			{
				rcHot = CRect(m_ptArrow, m_ptArrow);
				rcHot = ConvertToWindow( m_punkDrawTarget, rcHot );
//				rcHot.left -= hotPoint/2;
//				rcHot.top -= hotPoint/2;
				rcHot.right = rcHot.left + hotPoint;
				rcHot.bottom = rcHot.top + hotPoint;
				dc.Rectangle(&rcHot);
			}
			rcHot = CRect(m_rcText);
			rcHot = ConvertToWindow( m_punkDrawTarget, rcHot );
//			rcHot.left -= hotPoint/2;
//			rcHot.top -= hotPoint/2;
			rcHot.right = rcHot.left + hotPoint;
			rcHot.bottom = rcHot.top + hotPoint;

			dc.Rectangle(&rcHot);

			//dc.SelectObject(pOldBrush);
			//dc.SelectObject(pOldPen);
		}

  
		dc.SelectObject(pOldFont);
		
	}
	_catch;

}

CRect CCommentsObject::DoGetRect()
{
	return NORECT;

	CRect	rect = NORECT;
	/*rect.left = max(0, min(m_rcText.left, m_ptArrow.x-hotPoint));
	rect.top = max(0, min(m_rcText.top, m_ptArrow.y-hotPoint));
	rect.right = max(m_rcText.right+hotPoint, m_ptArrow.x);
	rect.bottom = max(m_rcText.bottom+hotPoint, m_ptArrow.y);*/
	rect.left = max(0, min(m_rcText.left-hotPoint, m_ptArrow.x-hotPoint));
	rect.top = max(0, min(m_rcText.top-hotPoint, m_ptArrow.y-hotPoint));

	if(m_punkDrawTarget)
	{
	CWnd* pwnd = ::GetWindowFromUnknown(m_punkDrawTarget);
	CWindowDC dc(pwnd);
	CFont fontText;
	//fontText.CreateFont((int)m_nHeight*GetZoom(m_punkDrawTarget), 0, 0, 0, m_bBold?FW_BOLD:FW_NORMAL, m_bItalic, m_bUnderline, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, m_nFamily, m_strName);
	LOGFONT logfont;
	memcpy(&logfont, &m_logFont, sizeof(LOGFONT));
	logfont.lfHeight *= GetZoom(m_punkDrawTarget);
	fontText.CreateFontIndirect(&logfont);

	CFont* pOldFont = dc.SelectObject(&fontText);
	CRect rcRecalc = NORECT;
	dc.DrawText(m_strText, &rcRecalc, DT_CALCRECT);
	dc.SelectObject(pOldFont);
	rcRecalc.InflateRect(1,1,3,1);
	//m_sizeTextWindow = CSize(rcRecalc.Width(), rcRecalc.Height());
	CRect	rc1(0,0,rcRecalc.Width(),rcRecalc.Height());
	rc1 = ConvertToClient( m_punkDrawTarget, rc1 );
	CRect rc2 = m_rcText;
	rc2.right = rc2.left + rc1.Width();
	rc2.bottom = rc2.top + rc1.Height();
	//if(m_rcText.right < rc2.right)
	//	m_rcText.right = rc2.right;
	//if(m_rcText.bottom < rc2.bottom)
	//	m_rcText.bottom = rc2.bottom;
	}

	rect.right = max(m_rcText.right, m_ptArrow.x);
	rect.bottom = max(m_rcText.bottom, m_ptArrow.y);
	rect.NormalizeRect();

	if(m_rcInvalid == NORECT)
		return rect;
	else
		return m_rcInvalid;
}

void CCommentsObject::UpdateObject()
{
	_SetRectByText();
//	if(m_rcLastDraw.Width() > m_rcText.Width() || m_rcLastDraw.Height() > m_rcText.Height())
//		m_rcText = m_rcLastDraw;
	sptrIDataObjectList sptrList(m_pParent);
	if(sptrList !=0 )
		sptrList->UpdateObject(GetControllingUnknown());
}

void CCommentsObject::_SetRectByText()
{
	m_rcInvalid = m_rcLastDraw = m_rcText;
	if(m_punkDrawTarget)
	{	
		CWnd* pwnd = ::GetWindowFromUnknown(m_punkDrawTarget);
		CWindowDC dc(pwnd);
		CFont fontText;
		LOGFONT logfont;
		memcpy(&logfont, &m_logFont, sizeof(LOGFONT));
		//[ay] так делать нельзя - иначе текст будет хреново отображаться на разных
		//view с разным zoom
		//logfont.lfHeight *= GetZoom(m_punkDrawTarget);
		fontText.CreateFontIndirect(&logfont);
		CFont* pOldFont = dc.SelectObject(&fontText);
		CRect rcRecalc = NORECT;
		CString strText = m_strText;
		if(strText.IsEmpty())
			strText = ".";
		dc.DrawText(strText, &rcRecalc, DT_CALCRECT);

		int nDX = m_logFont.lfHeight/5;
		double fVal = MulDiv(1, GetDeviceCaps(dc, LOGPIXELSY), 83) * ::GetZoom( m_punkDrawTarget );

		dc.SelectObject(pOldFont);
		rcRecalc.InflateRect(1,1,3,1);

		//[ay] продолжение темы
		//rcRecalc = ConvertToClient( m_punkDrawTarget, rcRecalc );

		fVal = 1;
		long nHeight = rcRecalc.Height();
		long nWidth = rcRecalc.Width() + nDX*fVal;
		if(m_strText.IsEmpty())
		{
			nHeight = 1;
			nWidth  = 1;
		}
		m_rcText.right = m_rcText.left + nWidth;
		m_rcText.bottom = m_rcText.top + nHeight;
	}
}


HRESULT CCommentsObject::XRect::SetRect(RECT rc)
{
	METHOD_PROLOGUE_EX(CCommentsObject, Rect);

	CPoint	ptDelta( pThis->m_ptArrow.x - pThis->m_rcText.left, 
		pThis->m_ptArrow.y - pThis->m_rcText.top );
	pThis->m_rcText = rc;
	pThis->m_ptArrow = rc.left+ptDelta.x, rc.top + ptDelta.y;

	return S_OK;
}

HRESULT CCommentsObject::XRect::GetRect(RECT* prc)
{
	METHOD_PROLOGUE_EX(CCommentsObject, Rect);
	/*CRect	rc = pThis->m_rcText;
	rc.left = min( rc.left, pThis->m_ptArrow.x-5 );
	rc.right = max( rc.right, pThis->m_ptArrow.x+5 );
	rc.top = min( rc.top, pThis->m_ptArrow.y-5 );
	rc.bottom = max( rc.bottom, pThis->m_ptArrow.y+5 );
	*prc = rc;
	*/
	*prc = pThis->DoGetRect();

	return S_OK;
}

HRESULT CCommentsObject::XRect::HitTest( POINT	point, long *plHitTest )
{
	METHOD_PROLOGUE_EX(CCommentsObject, Rect);

	CRect	rc = pThis->m_rcText;
	rc.left = min( rc.left, pThis->m_ptArrow.x-5 );
	rc.right = max( rc.right, pThis->m_ptArrow.x+5 );
	rc.top = min( rc.top, pThis->m_ptArrow.y-5 );
	rc.bottom = max( rc.bottom, pThis->m_ptArrow.y+5 );

	rc.InflateRect( 5, 5 );

	if( !rc.PtInRect(point) )
	{
		*plHitTest = false;
		return S_OK;
	}

	int nWidth	= rc.Width();
	int nHeight	= rc.Height();

	if( nWidth < 1 || nHeight < 1 )
	{
		*plHitTest = false;
		return S_OK;
	}

	CDC* pDC = 0;
	CDC dc;
	CClientDC dcScreen( 0 );

	if( !dc.CreateCompatibleDC( &dcScreen ) )
	{
		*plHitTest = false;
		return S_OK;
	}

	pDC = &dc;//&dcScreen - for test purpose
	

	CBitmap bitmap;
	if( !bitmap.CreateCompatibleBitmap( &dcScreen, nWidth, nHeight ) )
	{
		*plHitTest = false;
		return S_OK;
	}

	CBitmap* pOldBitmap = (CBitmap*)pDC->SelectObject( &bitmap );
	pDC->FillRect( &CRect( 0, 0, nWidth, nHeight ), &CBrush( RGB( 0, 0, 0 ) ) );	

	pDC->SetWindowOrg( rc.left, rc.top );	

	COLORREF clrTest = RGB( 255, 255, 255 );

	pDC->SetTextColor( clrTest );

	CPen pen( PS_SOLID, 10, clrTest );
	CPen* pOldPen = (CPen*)pDC->SelectObject( &pen );
	CBrush brush(clrTest);
	CBrush* pOldBrush = (CBrush*)pDC->SelectObject( &brush );

	IUnknown* punkSaveTarget = pThis->m_punkDrawTarget;
	pThis->m_punkDrawTarget = 0;


	s_bCanSelectGDI = false;
	pThis->DoDraw( *pDC );
	s_bCanSelectGDI = true;

	pThis->m_punkDrawTarget = punkSaveTarget;

	COLORREF clrRes = pDC->GetPixel( point );
	
	if(pOldPen) pDC->SelectObject( pOldPen );
	if(pOldBrush) pDC->SelectObject( pOldBrush );
	pDC->SelectObject( pOldBitmap );

	*plHitTest = clrRes!=0;


	/**plHitTest = 0;	

	CRect	rect;
	if( pThis->m_rcText.PtInRect( point ) )
		*plHitTest = 1;
	if( ::abs( pThis->m_ptArrow.x - point.x ) <= 5 &&
		::abs( pThis->m_ptArrow.y - point.y ) <= 5 )
		*plHitTest = 2;
		*/

	return S_OK;
}

HRESULT CCommentsObject::XRect::Move( POINT point )
{
	METHOD_PROLOGUE_EX(CCommentsObject, Rect);

	CRect rect;
	GetRect( &rect );
	point.x -= rect.left;
	point.y -= rect.top;

	pThis->m_rcText.left += point.x;
	pThis->m_rcText.right += point.x;
	pThis->m_rcText.top += point.y;
	pThis->m_rcText.bottom += point.y;

	pThis->m_ptArrow.x += point.x;
	pThis->m_ptArrow.y += point.y;

	return S_OK;
}

