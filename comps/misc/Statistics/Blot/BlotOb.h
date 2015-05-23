// BlotOb.h : Declaration of the CBlotOb

#pragma once
#include "resource.h"       // main symbols

#include "Blot.h"


inline float sqr(float x){return x*x;}

struct sxy{
	sxy():n(0),s(0.f),x(0.f),y(0.f),nf(0.f),sf(0.f),xf(0.f),yf(0.f){}
	int n;
	float s,x,y;
	float nf;
	float sf,xf,yf;
	float roCentreMass;
	float CentreMassRo(const Gdiplus::PointF& pt) const{
		return (float)sqrt(sqr(pt.X-xf)+sqr(pt.Y-yf));
	}
};

struct hashCompare:hash_compare<CPoint>
{
	size_t operator()(const CPoint& _Keyval) const
	{	
		union{
			long Lng;
			struct{
				short Lo,Hi;
			};
		} Val;
		Val.Lng =  _Keyval.y;
		short temp=Val.Lo;
		Val.Lo=Val.Hi;
		Val.Hi=temp;

		size_t sizet=(size_t)(_Keyval.x ^ Val.Lng);

		ldiv_t _Qrem = ldiv((long)sizet, 127773);
		_Qrem.rem = 16807 * _Qrem.rem - 2836 * _Qrem.quot;
		if (_Qrem.rem < 0)
			_Qrem.rem += 2147483647;
		return ((size_t)_Qrem.rem); 
	}

	bool operator()(const CPoint& a, const CPoint& b) const
	{
		return a.y<b.y || a.y==b.y && a.x<b.x; 
	}
};

typedef std::pair <CPoint, sxy> PtSxy;

// CBlotOb

class ATL_NO_VTABLE CBlotOb : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CBlotOb, &CLSID_BlotOb>,
	public IDispatchImpl<IBlotOb, &IID_IBlotOb, &LIBID_BlotLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CBlotOb():_NCadr(40)
	, _fMaxOverlapArea(std::numeric_limits<float>::quiet_NaN())
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_BLOTOB)


BEGIN_COM_MAP(CBlotOb)
	COM_INTERFACE_ENTRY(IBlotOb)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		static long Nfout=0;
		++Nfout;
//		_fout.open(((stringstream&)(stringstream()<<"_fout"<<Nfout<<".txt")).str().c_str());
		return S_OK;
	}
	
	void FinalRelease() 
	{
//		_fout.close();
	}
	float _fXSize, _fYSize;
	float _fXFrameSize, _fYFrameSize;
	float _fXMin, _fXMax, _fYMin, _fYMax;
	float _fStepX, _fStepY, _fXPrecision, _fYPrecision;
private:

	// array
	typedef vector<PtSxy> VecPtSxy;
	VecPtSxy _vit;
	set<SHORT> _setClasses;
	float _fMaxOverlapArea;
	INamedDataPtr _pTableND;
	IStatTablePtr _sptrTable;
	set<CComBSTR> _excludedPictures;
	long _NCadr, _Count;
	
//	ofstream _fout;


	map<CComBSTR,Gdiplus::PointF> _pictureOffsets;
public:
	STDMETHOD(get_Classes)(VARIANT* pVal);
	STDMETHOD(put_Classes)(VARIANT newVal);
	STDMETHOD(get_Table)(IUnknown** pVal);
	STDMETHOD(put_Table)(IUnknown* newVal);
	STDMETHOD(get_XPrecision)(FLOAT* pVal);
	STDMETHOD(put_XPrecision)(FLOAT newVal);
	STDMETHOD(get_YPrecision)(FLOAT* pVal);
	STDMETHOD(put_YPrecision)(FLOAT newVal);
	STDMETHOD(get_Count)(SHORT* pVal);
	STDMETHOD(get_Area)(SHORT Index, FLOAT* pVal);
	STDMETHOD(get_MassX)(SHORT Index, FLOAT* pVal);
	STDMETHOD(get_MassY)(SHORT Index, FLOAT* pVal);
	STDMETHOD(Find)(void);
	STDMETHOD(AddClass)(SHORT iClass);
	STDMETHOD(SetPictureSize)(FLOAT x, FLOAT y);
	STDMETHOD(SetOffs)(BSTR Picture, FLOAT x, FLOAT y);
	STDMETHOD(Exclude)(BSTR PictureName);
	STDMETHOD(get_MaxOverlapArea)(FLOAT* pVal);
	STDMETHOD(put_MaxOverlapArea)(FLOAT newVal);
	STDMETHOD(get_XShift)(SHORT Index, FLOAT* pVal);
	STDMETHOD(get_YShift)(SHORT Index, FLOAT* pVal);

	bool  overlap(const PtSxy& a, const PtSxy& bt);

	STDMETHOD(get_NCadr)(LONG* pVal);
	STDMETHOD(put_NCadr)(LONG newVal);
	STDMETHOD(SetFrameSize)(FLOAT x, FLOAT y);
};

OBJECT_ENTRY_AUTO(__uuidof(BlotOb), CBlotOb)
