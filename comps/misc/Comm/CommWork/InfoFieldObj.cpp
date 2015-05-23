#include "StdAfx.h"
#include "InfoFieldObj.h"
#include "MessageException.h"
#include "misc_calibr.h"

#define xMarkOffs 3
#define yUnitOffs 4
#define yTipPoint 1
#define hotPoint  8

class CStreamException : public CMessageException
{
public:
	CStreamException() : CMessageException("")
	{
	}
};

static void do_read(IStream *pStream, void *p, size_t n)
{
	ULONG ulRead;
	HRESULT hr = pStream->Read(p, n, &ulRead);
	if (FAILED(hr) || ulRead != n)
		throw new CStreamException;
}

static void do_write(IStream *pStream, void *p, size_t n)
{
	ULONG ulWritten;
	HRESULT hr = pStream->Write(p, n, &ulWritten);
	if (FAILED(hr) || ulWritten != n)
		throw new CStreamException;
}

static void do_read(IStream *pStream, _bstr_t &bstr)
{
	unsigned short nLen;
	do_read(pStream, &nLen, sizeof(nLen));
	_ptr_t2<char> buff(nLen+1);
	do_read(pStream, buff, nLen);
	buff[nLen] = 0;
	bstr = (char *)buff;
}

static void do_write(IStream *pStream, _bstr_t bstr)
{
	unsigned short nLen = bstr.length();
	do_write(pStream, &nLen, sizeof(nLen));
	do_write(pStream, (void *)(const char *)bstr, nLen);
}

CInfoFieldObject::CInfoFieldObject()
{
	m_dwFlags = nofNormal|nofStoreByParent;
	m_punkParent = NULL;
	m_bEditMode = false;
}

CInfoFieldObject::~CInfoFieldObject()
{
}


IUnknown *CInfoFieldObject::DoGetInterface( const IID &iid )
{
	if (iid == IID_IDrawObject2 || iid == IID_IDrawObject)
		return (IDrawObject2 *)this;
	else if (iid == IID_ICommentObj)
		return (ICommentObj*)this;
	else if (iid == IID_IFontPropPage)
		return (IFontPropPage *)this;
	else if (iid == IID_IRectObject)
		return (IRectObject *)this;
	else if (iid == IID_IInfoField)
		return (IInfoField *)this;
	else
		return CObjectBase::DoGetInterface(iid);
}

HRESULT CInfoFieldObject::IsBaseObject(BOOL * pbFlag)
{
	*pbFlag = false;
	return S_OK;
}

HRESULT CInfoFieldObject::GetParent( IUnknown **ppunkParent )
{
	*ppunkParent = m_punkParent;
	if (*ppunkParent) (*ppunkParent)->AddRef();
	return S_OK;
}

HRESULT CInfoFieldObject::SetParent( IUnknown *punkParent, DWORD dwFlags /*AttachParentFlag*/ )
{
	if (m_punkParent != 0)
	{
		INamedDataObject2Ptr sptrNDOParent(m_punkParent);
		sptrNDOParent->RemoveChild(Unknown());
	}
	m_punkParent = punkParent;
	if (m_punkParent != 0)
	{
		INamedDataObject2Ptr sptrNDOParent(m_punkParent);
		sptrNDOParent->AddChild(Unknown());
	}
	return S_OK;
}



HRESULT CInfoFieldObject::Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache )
{
	return PaintEx(hDC, rectDraw, punkTarget, pbiCache, pdibCache, NULL);;
}

HRESULT CInfoFieldObject::GetRect( RECT *prect )
{
	memset(prect,0,sizeof(*prect));
	return S_OK;
}

HRESULT CInfoFieldObject::SetTargetWindow(IUnknown *punkTarget)
{
	return S_OK;
}

HRESULT CInfoFieldObject::PaintEx(HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache, DRAWOPTIONS *pOptions)
{
	// Prepare text to draw
	BSTR bstr1;
	GetText(&bstr1);
	_bstr_t bstr(bstr1);
	::SysFreeString(bstr1);
	// Prepare coordinates
	IScrollZoomSitePtr sptrSZS(punkTarget);
	INIT_ZOOM_SCROLL(sptrSZS);
	_rect rc(m_rc.left,m_rc.top,m_rc.right-m_nUnitSpace-2*xMarkOffs,m_rc.bottom);
	rc = _window(rc, sptrSZS);
	_rect rcMarker(m_rc.right-m_nUnitMarkerSize-xMarkOffs,m_rc.bottom-yUnitOffs,m_rc.right-xMarkOffs,m_rc.bottom-yUnitOffs);
	rcMarker = _window(rcMarker, sptrSZS);
	_rect rcUnit(m_rc.right-m_nUnitSpace-xMarkOffs, m_rc.top, m_rc.right, m_rc.bottom-yUnitOffs-2*yTipPoint);
	rcUnit = _window(rcUnit, sptrSZS);
	int yTipPoint1 = int(yTipPoint*fZoom);
	// Prepare font
	LOGFONT font = m_font;
	font.lfHeight = int(fZoom*font.lfHeight);
//	font.lfWidth = int(fZoom*font.lfWidth);
	_gdi_t<HFONT> ft(CreateFontIndirect(&font));
	// Prepare DC
	HFONT hfold = (HFONT)SelectObject(hDC, ft);
	_pen pen(CreatePen(PS_SOLID,0,m_clr));
	HPEN hpold = (HPEN)SelectObject(hDC, pen);
	COLORREF clr = SetTextColor(hDC, m_clr);
	COLORREF clrBack = SetBkColor(hDC, m_clrBack);
	int nMode = SetBkMode(hDC, m_bBackMode?OPAQUE:TRANSPARENT);
	// Draw text
	DrawText(hDC, (const char *)bstr, bstr.length(), &rc, DT_SINGLELINE|DT_LEFT|DT_BOTTOM);
	// Draw unit name
	DrawText(hDC, (const char *)m_bstrUnitName, m_bstrUnitName.length(), &rcUnit, DT_SINGLELINE|DT_LEFT|DT_TOP);
	// Draw unit marker
	MoveToEx(hDC, rcMarker.left, rcMarker.top, NULL);
	LineTo(hDC, rcMarker.right, rcMarker.top);
	MoveToEx(hDC, rcMarker.left, rcMarker.top-yTipPoint1, NULL);
	LineTo(hDC, rcMarker.left, rcMarker.top+yTipPoint1);
	MoveToEx(hDC, rcMarker.right, rcMarker.top-yTipPoint1, NULL);
	LineTo(hDC, rcMarker.right, rcMarker.top+yTipPoint1);
	// Draw handle
	if (m_bEditMode)
	{
		_rect rcHot(rc.left-hotPoint/2,rc.top-hotPoint/2,rc.left+hotPoint/2,rc.top+hotPoint/2);
		SelectObject(hDC, GetStockObject(WHITE_BRUSH));
		SelectObject(hDC, GetStockObject(BLACK_PEN));
		Rectangle(hDC, rcHot.left, rcHot.top, rcHot.right, rcHot.bottom);
	}
	// Restore DC
	SetBkMode(hDC, nMode);
	SetBkColor(hDC, clrBack);
	SetTextColor(hDC, clr);
	SelectObject(hDC, hpold);
	SelectObject(hDC, hfold);
	return S_OK;
}


HRESULT CInfoFieldObject::GetText(BSTR *pbstrText)
{
	_bstr_t bstr(m_bstrMicroName);
	if (m_bstrMicroName.length() > 0)
	{
		bstr += ";";
		char szBuff[50];
		GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &m_localTime, NULL, szBuff, 50);
		bstr += szBuff;
		bstr += ";";
		bstr += _itoa(m_nVoltageKV, szBuff, 10);
		bstr += "kV;";
		bstr += _itoa(m_nZoom, szBuff, 10);
		bstr += ";";
	}
	*pbstrText = bstr.copy();
	return S_OK;
}

HRESULT CInfoFieldObject::SetText(BSTR bstrText)
{
	return S_OK;
}

HRESULT CInfoFieldObject::GetCoordinates(RECT* prcText, POINT* pPtArrow, BOOL bFull/* = 0*/)
{
	*prcText = m_rc;
	pPtArrow->x = pPtArrow->y = -1;
	return S_OK;
}

HRESULT CInfoFieldObject::SetCoordinates(RECT rcText, POINT ptArrow)
{
	UpdateObject();
	memcpy(&m_rc,&rcText,sizeof(RECT));
	UpdateObject();
	return S_OK;
}

HRESULT CInfoFieldObject::SetEditMode(BOOL bEdit)
{
	UpdateObject();
	m_bEditMode = bEdit?true:false;
	UpdateObject();
	return S_OK;
}

HRESULT CInfoFieldObject::LockDraw(BOOL bLock)
{
	return S_OK;
}

HRESULT CInfoFieldObject::UpdateObject()
{
	sptrIDataObjectList sptrList(m_punkParent);
	if (sptrList !=0)
		sptrList->UpdateObject(Unknown());
	return S_OK;
}

HRESULT CInfoFieldObject::GetFont(LOGFONT*	pLogFont, int* pnAlign)
{
	*pLogFont = m_font;
	*pnAlign = m_nAlign;
	return S_OK;
}

HRESULT CInfoFieldObject::GetColorsAndStuff(OLE_COLOR* pcolorText, OLE_COLOR* pcolorBack, BOOL* pbBack, BOOL* pbArrow)
{
	*pcolorText = m_clr;
	*pcolorBack = m_clrBack;
	*pbBack = m_bBackMode;
	*pbArrow = FALSE;
	return S_OK;
}

HRESULT CInfoFieldObject::SetFont(LOGFONT*	pLogFont, int nAlign)
{
	UpdateObject();
	m_font = *pLogFont;
	m_nAlign = nAlign;
	SIZE sz;
	CalcSize(&sz);
	m_rc.left = m_rc.right-sz.cx;
	if (m_rc.left < 0)
	{
		m_rc.left = 0;
		m_rc.right = m_rc.left+sz.cx;
	}
	m_rc.top = m_rc.bottom-sz.cy;
	if (m_rc.top < 0)
	{
		m_rc.top = 0;
		m_rc.bottom = m_rc.top+sz.cx;
	}
	return S_OK;
}

HRESULT CInfoFieldObject::SetColorsAndStuff(OLE_COLOR colorText, OLE_COLOR colorBack, BOOL bBack, BOOL bArrow)
{
	m_clr = colorText;
	m_clrBack = colorBack;
	m_bBackMode = bBack;
	return S_OK;
}


HRESULT CInfoFieldObject::SetRect(RECT rc)
{
	memcpy(&m_rc,&rc,sizeof(RECT));
	return S_OK;
}

/*HRESULT CInfoFieldObject::GetRect(RECT* prc)
{
	*prc = m_rc;
	return S_OK;
}*/

HRESULT CInfoFieldObject::HitTest( POINT	point, long *plHitTest )
{
	*plHitTest = 0;
	return S_OK;
}

HRESULT CInfoFieldObject::Move( POINT point )
{
	_rect rc(m_rc);
	m_rc.left = point.x;
	m_rc.top = point.y;
	m_rc.right = m_rc.left+rc.width();
	m_rc.bottom = m_rc.top+rc.height();
	return S_OK;
}

HRESULT CInfoFieldObject::Load(IStream *pStream, SerializeParams *pparams)
{
	try
	{
		BYTE bVersion;
		do_read(pStream, &bVersion, sizeof(bVersion));
		do_read(pStream, m_bstrMicroName);
		do_read(pStream, &m_localTime, sizeof(m_localTime));
		do_read(pStream, &m_nVoltageKV, sizeof(m_nVoltageKV));
		do_read(pStream, &m_nZoom, sizeof(m_nZoom));
		do_read(pStream, &m_rc, sizeof(m_rc));
		do_read(pStream, &m_font, sizeof(m_font));
		do_read(pStream, &m_nAlign, sizeof(m_nAlign));
		do_read(pStream, &m_clr, sizeof(m_clr));
		do_read(pStream, &m_clrBack, sizeof(m_clrBack));
		do_read(pStream, &m_bBackMode, sizeof(m_bBackMode));
		do_read(pStream, &m_nUnitMarkerSize, sizeof(m_nUnitMarkerSize));
		do_read(pStream, m_bstrUnitName);
		do_read(pStream, &m_nUnitSpace, sizeof(m_nUnitSpace));
		if (bVersion >= 2)
			do_read(pStream, &m_dMarkerNm, sizeof(m_dMarkerNm));
	}
	catch(CStreamException *e)
	{
		delete e;
		return E_UNEXPECTED;
	}
	return S_OK;
}

HRESULT CInfoFieldObject::Store(IStream *pStream, SerializeParams *pparams)
{
	try
	{
		BYTE bVersion = 2;
		do_write(pStream, &bVersion, sizeof(bVersion));
		do_write(pStream, m_bstrMicroName);
		do_write(pStream, &m_localTime, sizeof(m_localTime));
		do_write(pStream, &m_nVoltageKV, sizeof(m_nVoltageKV));
		do_write(pStream, &m_nZoom, sizeof(m_nZoom));
		do_write(pStream, &m_rc, sizeof(m_rc));
		do_write(pStream, &m_font, sizeof(m_font));
		do_write(pStream, &m_nAlign, sizeof(m_nAlign));
		do_write(pStream, &m_clr, sizeof(m_clr));
		do_write(pStream, &m_clrBack, sizeof(m_clrBack));
		do_write(pStream, &m_bBackMode, sizeof(m_bBackMode));
		do_write(pStream, &m_nUnitMarkerSize, sizeof(m_nUnitMarkerSize));
		do_write(pStream, m_bstrUnitName);
		do_write(pStream, &m_nUnitSpace, sizeof(m_nUnitSpace));
		do_write(pStream, &m_dMarkerNm, sizeof(m_dMarkerNm));
	}
	catch(CStreamException *e)
	{
		delete e;
		return E_UNEXPECTED;
	}
	return S_OK;
}

HRESULT CInfoFieldObject::SetData(InfoFieldData ifd)
{
	m_bstrMicroName = ifd.szMicroName;
	m_localTime = ifd.localTime;
	m_nVoltageKV = ifd.nVoltageKV;
	m_nZoom = ifd.nZoom;
	m_dMarkerNm = ifd.dMarkerNm;
	return S_OK;
}

HRESULT CInfoFieldObject::GetData(InfoFieldData *p)
{
	strncpy(p->szMicroName,(const char *)m_bstrMicroName,MAX_MICRO_NAME);
	p->localTime = m_localTime;
	p->nVoltageKV = m_nVoltageKV;
	p->nZoom = m_nZoom;
	return S_OK;
}

_size CalcSizeOfText(LOGFONT lf, const char *pszText)
{
	HDC hdc = ::GetDC(NULL);
	HFONT hf = ::CreateFontIndirect(&lf);
	HFONT hfold = (HFONT)::SelectObject(hdc, hf);
	_rect rc(0,0,0,0);
	rc.bottom = ::DrawText(hdc, pszText, -1, &rc, DT_CALCRECT|DT_SINGLELINE);
	::SelectObject(hdc, hfold);
	::ReleaseDC(NULL, hdc);
	_size sz(rc.right, rc.bottom);
	DeleteObject(hf);
	return sz;
}

HRESULT CInfoFieldObject::CalcSize(SIZE *pSize)
{
	BSTR bstr1;
	GetText(&bstr1);
	_bstr_t bstr(bstr1);
	::SysFreeString(bstr1);
	_size sz = CalcSizeOfText(m_font, (const char *)bstr);
#if 1
	char szText[MAX_CALIBR_NAME+5];
	m_nUnitMarkerSize = 30;
	if (int(m_dMarkerNm) == m_dMarkerNm)
		sprintf(szText, "%.f nm", m_dMarkerNm);
	else
		sprintf(szText, "%.1f nm", m_dMarkerNm);
	_size sz1 = CalcSizeOfText(m_font, szText);
	m_bstrUnitName = szText;
	m_nUnitSpace = max(m_nUnitMarkerSize,sz1.cx);
	sz.cx += max(m_nUnitMarkerSize,sz1.cx)+2*xMarkOffs;
	sz.cy = max(sz.cy,sz1.cy+yUnitOffs+2*yTipPoint);
#else
	double d = GetValueDouble(GetAppUnknown(), "CommWork", "UnitValue", 1.);
	d = min(max(0.1,d),5.);
	double dCalibr,dCalibrPerUnit;
	char szUnitName[MAX_CALIBR_NAME],szText[MAX_CALIBR_NAME+5];
	if (GetDefaultCalibration(&dCalibr,NULL) && GetCalibrationUnit(&dCalibrPerUnit, szUnitName))
	{
		double d1 = d/dCalibr/dCalibrPerUnit;
		m_nUnitMarkerSize = int(d1);
		if (int(d) == d)
			sprintf(szText, "%.f %s", d, szUnitName);
		else
			sprintf(szText, "%.1f %s", d, szUnitName);
		_size sz1 = CalcSizeOfText(m_font, szText);
		m_bstrUnitName = szText;
		m_nUnitSpace = max(m_nUnitMarkerSize,sz1.cx);
		sz.cx += max(m_nUnitMarkerSize,sz1.cx)+2*xMarkOffs;
		sz.cy = max(sz.cy,sz1.cy+yUnitOffs+2*yTipPoint);
	}
#endif
	*pSize = sz;
	return S_OK;
}

