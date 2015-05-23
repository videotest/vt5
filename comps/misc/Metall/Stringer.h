#pragma once

#include "action_filter.h"
#include "data5.h"
#include "data_main.h"
#include "nameconsts.h"
#include "resource.h"
#include "defs.h"
#include "Object5.h"
#include "classes5.h"
#include "array.h"

interface IStringerAxis : public IUnknown
{
	virtual void __stdcall SetCoords(POINT pt1, POINT pt2) = 0;
	virtual BOOL __stdcall GetCoords(POINT *ppt1, POINT *ppt2) = 0;
	virtual void __stdcall SetParameter(long lParam, double dValue) = 0;
	virtual void __stdcall GetParameter(long lParam, double *pdValue) = 0;
};

declare_interface(IStringerAxis, "4A7EFCEA-6EE3-4d81-895F-540070AB0DC2")

#define szTypeStringerAxis  "StringerAxis"
#define szStringerAxisProgId  "StringerAxis"
#define szStringerAxisInfoProgId  "StringerAxisInfo"

// {8BAF38EE-E555-4d7f-9C8A-CC6A69025225}
static const GUID clsidStringerAxis = 
{ 0x8baf38ee, 0xe555, 0x4d7f, { 0x9c, 0x8a, 0xcc, 0x6a, 0x69, 0x2, 0x52, 0x25 } };

// {AE09C8B7-4D81-4047-8F22-9F365EA0A01B}
static const GUID clsidStringerAxisInfo = 
{ 0xae09c8b7, 0x4d81, 0x4047, { 0x8f, 0x22, 0x9f, 0x36, 0x5e, 0xa0, 0xa0, 0x1b } };


class CStringerAxis : public CObjectBase, public IStringerAxis, public IDrawObject2,
	public _dyncreate_t<CStringerAxis>
{
	POINT m_pt1,m_pt2;
	double m_dLength,m_dWidth,m_dSumLenght,m_dWidthAstm45;
	IUnknown *punkParent;
	route_unknown();
public:
	CStringerAxis();
	virtual ~CStringerAxis();
	virtual IUnknown *DoGetInterface(const IID &iid);
	//IPersist helper
	virtual GuidKey GetInternalClassID();
public:
	//IStringerObj
	virtual void __stdcall SetCoords(POINT pt1, POINT pt2);
	virtual BOOL __stdcall GetCoords(POINT *ppt1, POINT *ppt2);
	virtual void __stdcall SetParameter(long lParam, double dValue);
	virtual void __stdcall GetParameter(long lParam, double *pdValue);
	//ISerializableObject
	com_call Load( IStream *pStream, SerializeParams *pparams );
	com_call Store( IStream *pStream, SerializeParams *pparams );
	//INamedDataObject2
	com_call IsBaseObject(BOOL * pbFlag);
	com_call GetParent( IUnknown **ppunkParent );
	com_call SetParent( IUnknown *punkParent, DWORD dwFlags /*AttachParentFlag*/ );
	// IDrawObject2
	com_call Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache );
	com_call GetRect( RECT *prect );
	com_call SetTargetWindow( IUnknown *punkTarget );
	com_call PaintEx(HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache, DRAWOPTIONS *pOptions);
};

class CStringerAxisInfo : public CDataInfoBase,
	public _dyncreate_t<CStringerAxisInfo>
{
public:
	CStringerAxisInfo() : CDataInfoBase(clsidStringerAxis, szTypeStringerAxis, 0, 0)
	{	}
};

class CStringer : public CFilter, public _dyncreate_t<CStringer>
{
public:
	route_unknown();
public:
	
	CStringer();
	virtual bool InvokeFilter();
//	virtual bool CanDeleteArgument(CFilterArgument *pa) {return false;}
};

class CStringerInfo : public _ainfo_t<ID_ACTION_STRINGER, _dyncreate_t<CStringer>::CreateObject, 0>,
	public _dyncreate_t<CStringerInfo>
{
	route_unknown();
public:
	CStringerInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};




