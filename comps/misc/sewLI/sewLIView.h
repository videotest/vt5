#ifndef __sewLIView_h__
#define __sewLIView_h__

#include "\vt5\ifaces\sewLI_int.h"
#include "\vt5\ifaces\iSewLI.h"
#include "NotifySpec.h"
#include "win_view.h"
#include "misc_list.h"
#include "misc_utils.h"
#include "docview5.h"
#include "impl_mouse.h"
#include "core5.h"
#include "PropBagImpl.h"
#include "impl_help.h"
#include "\vt5\awin\win_base.h"
#include "atltypes.h"
#include "input.h"

#include "sewLI_object.h"

enum DrawPreviewMode
{
	OpacquePreview,
	TransparentPreview,
	SubtractPreview
};

enum DrawFragmentMode
{
	FragmentOpaque = 0,
	FragmentTransparent = 1,
	FragmentSmoothTransition = 2,
	FragmentNegative = 3,
};

enum DrawFrameMode
{
	DrawFrameActiveFrame = 1,
	DrawFrameOverlay = 2,
};

class CLVFrameBuffer : public IDIBProvider
{
	CRITICAL_SECTION m_cs;
	LPBITMAPINFOHEADER m_lpbiLVCur;
	DWORD m_dwPrvDataSize;
	bool m_bCalculating;
	CPoint m_ptShift;

	dummy_unknown()
	void *__stdcall AllocMem(DWORD dwSize); //MUST be inside critical section!
public:

	CLVFrameBuffer();
	~CLVFrameBuffer();
	void Free();



	enum EFrameBufferState
	{
		NotInit = 0,
		OneFrame = 1,
	};
	EFrameBufferState m_State;

	LPBITMAPINFOHEADER GetData() {return m_State==OneFrame?m_lpbiLVCur:NULL;}
	void CopyData(IDriver3 *pDriver, int nDevice);
	void CopyDataDir(IDriver3 *pDriver, int nDevice);
	void CopyDataDIB(IDriver3 *pDriver, int nDevice);
	CPoint CalculateShiftLastFrag(ISewImageList *pFragList, ISewFragment *pFragPrev,
		CPoint ptOrig, SIZE szLV, int nMode, double dZoom);
	int CalcDifference(ISewImageList *pFragList, ISewFragment *pFragPrev,
		CPoint ptOrig, SIZE szLV);
	void PaintToDIB(LPBITMAPINFOHEADER lpbiDest, LPBYTE lpDest,
		_ptr_t2<WORD> &GlobalDistMap, double xSrc1, double ySrc1, double xSrc2,
		double ySrc2, RECT rcDst, double fZoom,
		DrawPreviewMode dpm);
	bool CanDraw() {return m_State == OneFrame;}
};

class CSewLIView : public CWinViewBase, 				
	public IPersist,
	public CNamedObjectImpl,
	public CMouseImpl,
	public IViewDispatch,
	public ISewLIView,
	public CNamedPropBagImpl,
	public CHelpInfoImpl,
	//public ISewImageList,
	public INotifyPlace,
	public IDriverSite,
	public _dyncreate_t<CSewLIView>
{
public:
	CSewLIView();
	virtual ~CSewLIView();

public: 
	virtual IUnknown *DoGetInterface( const IID &iid );
	
	route_unknown();

	/////////////////////////////////////////////////
    com_call GetTypeInfoCount( 
            /* [out] */ UINT *pctinfo);
	com_call GetTypeInfo( 
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
	com_call GetIDsOfNames( 
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
	com_call Invoke( 
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);

protected:
	ITypeLib*	m_pi_type_lib;
	ITypeInfo*	m_pi_type_info;


	virtual LRESULT		DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );
	virtual void		DoAttachObjects();

	virtual LRESULT OnCreate();							//WM_CREATE
	virtual LRESULT OnPaint();							//WM_PAINT	
	virtual LRESULT OnEraseBackground( HDC hDC );		//WM_ERASEBKGND
	virtual LRESULT OnSize( int cx, int cy );			//WM_SIZE
	virtual LRESULT OnContextMenu( HWND hWnd, int x, int y );	//WM_CONTEXTMENU
	virtual LRESULT OnDestroy( );						//WM_DESTROY
	virtual LRESULT OnMouseWheel(UINT nFlags, short zDelta, _point pt);
	virtual LRESULT OnTimer( UINT nTimer);
	virtual LRESULT OnChar( UINT nChar, UINT lParam );

	//virtual LRESULT	OnLButtonClick( int x, int y );		//WM_LBUTTONDOWN
	virtual bool DoLButtonDown( _point  point );
	virtual bool DoLButtonDblClick( _point  point );
	virtual bool DoRButtonDown( _point  point );
	virtual bool DoChar( int nChar, bool bKeyDown );

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );


	//Persist
	com_call GetClassID( CLSID *pClassID ); 

	com_call GetFirstVisibleObjectPosition(TPOS *plpos);
	com_call GetNextVisibleObject(IUnknown ** ppunkObject, TPOS *plPos);

	com_call GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch );

	// IViewDispatch
    virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
        /* [retval][out] */ BSTR *pVal);
    virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
        /* [in] */ BSTR newVal);
    virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OverlapProcent( 
        /* [retval][out] */ int *pVal);
    virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OverlayCoefficient( 
        /* [retval][out] */ int *pVal);
    virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ShowLiveVideo( 
        /* [retval][out] */ int *pVal);
    virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ShowLiveVideo( 
        /* [in] */ int newVal);
    virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLiveVideoRect( 
        /* [out] */ int *pLeft,
        /* [out] */ int *pTop,
        /* [out] */ int *pRight,
        /* [out] */ int *pBottom);
    virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDocumentOrg( 
        /* [out] */ int *px,
        /* [out] */ int *py);
    virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MoveLiveVideo( 
        /* [in] */ int x,
        /* [in] */ int y,
		/* [in] */ int nFlags);
    virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LiveVideoMoving( 
        /* [retval][out] */ int *pVal);
    virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LiveVideoMoving( 
        /* [in] */ int newVal);
    virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MoveLiveVideoTo (
        /*[in]*/ int x,
        /*[in]*/ int y,
        /*[in]*/ int nFlags );
    virtual HRESULT STDMETHODCALLTYPE get_AutoMoveLiveVideoMode (
		/*[out,retval]*/ int * pVal );
    virtual HRESULT STDMETHODCALLTYPE put_AutoMoveLiveVideoMode (
		/*[in]*/ int pVal );
    virtual HRESULT __stdcall get_ShowLiveVideoMode (
		/*[out,retval]*/ int * pVal );
    virtual HRESULT __stdcall put_ShowLiveVideoMode (
		/*[in]*/ int pVal );
    virtual HRESULT __stdcall get_BaseFragment (
        /*[out,retval]*/ IUnknown * * ppunkVal );
    virtual HRESULT __stdcall get_LastFragmentMode (
        /*[out,retval]*/ int * pVal );
    virtual HRESULT __stdcall put_LastFragmentMode (
        /*[in]*/ int pVal );
    virtual HRESULT __stdcall get_ShowFrame (
        /*[out,retval]*/ int * pVal );
    virtual HRESULT __stdcall put_ShowFrame (
        /*[in]*/ int pVal );
    virtual HRESULT __stdcall get_NextFragmentDirection (
        /*[out,retval]*/ double * pVal );

	// INotifyPlace
	com_call NotifyPutToData( IUnknown* punkObj );
	com_call NotifyPutToDataEx( IUnknown* punkObj, IUnknown** punkUndoObj );
	com_call Undo( IUnknown* punkUndoObj );
	com_call Redo( IUnknown* punkUndoObj );

	// IDriverSite
	com_call Invalidate();
	com_call OnChangeSize();


	void SetClientSize(SIZE size);
	void GetClientSize(SIZE& size);

	com_call OnActivateView( BOOL bActivate, IUnknown *punkOtherView );


	//IViewDispatch
	com_call GetDispIID( GUID* pguid );

protected:
	void _UpdateScrollSizes();
	void _ShowPropPage(bool bShow);

	CLoadFragList m_LoadFragList;
	bool MakeLoadFragList(bool bLIChanged);
	void GetFragmentOffset(ISewFragment *pFrag, POINT *pptOffs);

	INamedDataObject2Ptr	m_sptrObjectList;

	CPoint	m_ptCoordSpaceOrg;
	CSize	m_szCoordSpaceExt;
	CRect	m_rcBoundsPrev;
	CRect	m_rcImagePrev;
	int		m_nTimerId;
//	_bstr_t m_bstrName;

	int		m_nTimerPrv;
	bool	m_bAlwaysRedrawPrv;
	bool	m_bInvalidatePrv;
	CRect m_rcPreviewRect;
	IInputPreview2Ptr m_sptrIP;
	IDriver3Ptr m_sptrDrv3;
	CLVFrameBuffer m_2FramesBuffer;
	BOOL m_bLiveVideoMoving;
	int m_nInputDevice;
	void _UpdatePreviewSize();
	CPoint GetInitPreviewPos();
	CSize GetDrvImgSize();
	void InitPreview();
	void DeinitPreview();

//	BOOL m_bAutoMoveLV;
//	BOOL m_bCorrectLVPos;
	enum AutoMoveLVMode
	{
		AutoMoveLV = 0,
		CorrectAfterMove = 1,
		OnlyCalcDiff = 2,
	};

	AutoMoveLVMode  m_AutoMoveLVMode; 
	DWORD m_dwLastAutoMove,m_dwAutoMoveTime;
	int m_nDiffThresh;
	CLoadFragment m_LFragBase;
	CPoint m_ptLVOffs;
	void _UpdateBaseFragAndOffs(ISewFragment *pFrag = NULL, CPoint ptOffs = CPoint(0,0));
	bool DoMoveLV(int x, int y, int nMode);
	bool DoMoveLVTo(int x, int y, int nMode);
	bool AutoMovePreview(bool bForce);
	BOOL m_bCalcOverlayCoef;

	int m_nPrvOvrPerc;
	bool m_bOvrPercNtyCalled;
	void CheckOverlayPercent();
	int m_nPrevDiffState;
	void CheckDiff();
	CRect ConvertToClient(CRect rcImage);
	
	DrawFragmentMode m_LastFragDrawMode;
	BOOL DrawImages(HDC hdcScreen, CRect rectPaint);
	void DrawFragment(BITMAPINFOHEADER &bmih, LPBYTE pdibBits,
		CPoint pointScroll, CRect rectPaint, COLORREF cr, DrawFragmentMode dfm,
		ISewFragment *pFrag, double fZoom, _ptr_t2<WORD> &GlobalDistMap,
		bool *pbCC2 = NULL);
	int m_nDrawFrameMode;
	bool m_bAlternateFrame;
	enum DrawFrameMode
	{
		FirstDraw,
		Alternate,
		Xor
	};
	BOOL DrawActiveFragment(HDC hdcMem, DrawFrameMode Mode);
	BOOL DrawIntersection(HDC hdcMem);
	DrawPreviewMode m_DrawPreviewMode;
	BOOL DrawPreview(HDC hdc, BOOL b1st);
	BOOL DrawPreview2(LPBITMAPINFOHEADER lpbi, LPBYTE lpData, _ptr_t2<WORD> &GlobalDistMap,
		CRect rcPaintImg, double fZoom, CPoint pointScroll);
	DWORD m_dwDrvFlag;
};


#endif //__sewLIView_h__
