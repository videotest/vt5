#if !defined(__InfoFieldObj_H__)
#define __InfoFieldObj_H__

#include "data_main.h"
#include "object5.h"
#include "EditorInt.h"
#include "nameconsts.h"
#include "misc_classes.h"
#include "InfoFieldInt.h"

// {4F271FC0-64A3-4837-8FFD-A2CB6F446D5F}
static const GUID clsidInfoFieldObject = 
{ 0x4f271fc0, 0x64a3, 0x4837, { 0x8f, 0xfd, 0xa2, 0xcb, 0x6f, 0x44, 0x6d, 0x5f } };

// {51581FBE-268A-48C0-88AB-EC15210E819D}
static const GUID clsidInfoFieldObjectInfo = 
{ 0x51581fbe, 0x268a, 0x48c0, { 0x88, 0xab, 0xec, 0x15, 0x21, 0xe, 0x81, 0x9d } };


class CInfoFieldObject : public CObjectBase, public IDrawObject2, public ICommentObj, public IFontPropPage,
	public IRectObject, public IInfoField, public _dyncreate_t<CInfoFieldObject>
{
	IUnknown *m_punkParent; // Do not AddRef/Release !!!!!
	// Stored data
	_bstr_t m_bstrMicroName;
	SYSTEMTIME m_localTime;
	unsigned long m_nVoltageKV;
	unsigned long m_nZoom;
	double m_dMarkerNm;
	_rect m_rc;
	LOGFONT m_font;
	int m_nAlign;
	OLE_COLOR m_clr,m_clrBack;
	BOOL m_bBackMode;
	unsigned m_nUnitMarkerSize;
	_bstr_t m_bstrUnitName;
	unsigned m_nUnitSpace;
	// Modes
	bool m_bEditMode;
	route_unknown();
public:
	CInfoFieldObject();
	virtual ~CInfoFieldObject();
	virtual IUnknown *DoGetInterface(const IID &iid);
	//IPersist helper
	virtual GuidKey GetInternalClassID(){ return clsidInfoFieldObject; }
public:
	//ISerializableObject
	com_call Load( IStream *pStream, SerializeParams *pparams );
	com_call Store( IStream *pStream, SerializeParams *pparams );
	//INamedDataObject2
	com_call IsBaseObject(BOOL * pbFlag);
	com_call GetParent( IUnknown **ppunkParent );
	com_call SetParent( IUnknown *punkParent, DWORD dwFlags);
	//IDrawObject2
	com_call Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache );
	com_call GetRect( RECT *prect );
	com_call SetTargetWindow( IUnknown *punkTarget );
	com_call PaintEx(HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache, DRAWOPTIONS *pOptions);
	//ICommentObj
	com_call GetText(BSTR *pbstrText);
	com_call SetText(BSTR bstrText);
	com_call GetCoordinates(RECT* prcText, POINT* pPtArrow, BOOL bFull = 0);
	com_call SetCoordinates(RECT rcText, POINT ptArrow);
	com_call SetEditMode(BOOL bEdit);
	com_call LockDraw(BOOL bLock);
	com_call UpdateObject();
	//IFontPropPage
	com_call GetFont(LOGFONT*	pLogFont, int* pnAlign);
	com_call GetColorsAndStuff(OLE_COLOR* pcolorText, OLE_COLOR* pcolorBack, BOOL* pbBack, BOOL* pbArrow);
	com_call SetFont(LOGFONT*	pLogFont, int nAlign);
	com_call SetColorsAndStuff(OLE_COLOR colorText, OLE_COLOR colorBack, BOOL bBack, BOOL bArrow);
	//IRectObject
	com_call SetRect(RECT rc);
//	com_call GetRect(RECT* prc);
	com_call HitTest( POINT	point, long *plHitTest );
	com_call Move( POINT point );
	//IInfoField
	com_call SetData(InfoFieldData ifd);
	com_call GetData(InfoFieldData *p);
	com_call CalcSize(SIZE *pSize);
};



class CInfoFieldObjectInfo : public CDataInfoBase,
	public _dyncreate_t<CInfoFieldObjectInfo>
{
public:
	CInfoFieldObjectInfo() : CDataInfoBase(clsidInfoFieldObject, szTypeInfoFieldObject, szDrawingObjectList, 0)
	{	}
};










#endif
