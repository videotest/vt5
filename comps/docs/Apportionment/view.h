#ifndef __aview_h__
#define __aview_h__

#include "\vt5\ifaces\aview_int.h"
#include "win_view.h"
#include "misc_list.h"
#include "misc_utils.h"
#include "docview5.h"
#include "impl_mouse.h"
#include "core5.h"
#include "PropBagImpl.h"
#include "impl_help.h"
#include "\vt5\awin\win_base.h"


typedef std::map<long, bool>	tmapGUID2BOOL;


//thumbnail with object - cell
//cell with text at bottom - unit
//units of one class - classcell

struct ClassCell
{
	ClassCell()
	{
		ZeroMemory(&sizeClassCell, sizeof(SIZE));
		bstrName = "";
		m_rcLastDrawRect=NORECT;
	}
	~ClassCell()
	{
		long lPos = listObjects.head();
		while(lPos)
		{
			IUnknown* punk = listObjects.next(lPos);
			if(punk) punk->Release();
		}

		lPos = listTopLeftCell.head();
		while(lPos)
			delete listTopLeftCell.next(lPos);
		lPos = listRowHeights.head();
		while(lPos)
			delete listRowHeights.next(lPos);
	}

	_bstr_t bstrName;
	SIZE	sizeClassCell;
	_rect m_rcLastDrawRect; //прямоугольник, который она занимала при последней отрисовке
	_list_t2<POINT*>		listTopLeftCell;
	_list_t2<long*>		listRowHeights; // for print
	_list_t2<IUnknown*>	listObjects;
};

class CRenameEditBox : public win_impl
{
public:
	CRenameEditBox();
protected:
	virtual long on_char( long nVirtKey );
public:
	char m_szText[256];
	bool m_bReady;
};

class CAView : public CWinViewBase, 				
	public IPersist,
	public IMultiSelection,
	public IPrintView,
	public CNamedObjectImpl,
	public CMouseImpl,
	public IMenuInitializer,
	public IViewDispatch,
	public IAView,
	public CNamedPropBagImpl,
	public CHelpInfoImpl,
	public _dyncreate_t<CAView>
{
public:
	CAView();
	virtual ~CAView();

public: 
	virtual IUnknown *DoGetInterface( const IID &iid );
	
	route_unknown();

	/////////////////////////////////////////////////
	//interface IAviImageDisp
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
	virtual LRESULT OnTimer(UINT_PTR nTimer);						//WM_TIMER
	virtual LRESULT OnMouseWheel(UINT nFlags, short zDelta, _point pt);

	//virtual LRESULT	OnLButtonClick( int x, int y );		//WM_LBUTTONDOWN
	virtual bool DoLButtonDown( _point  point );
	virtual bool DoLButtonDblClick( _point  point );
	virtual bool DoRButtonDown( _point  point );
	virtual bool DoChar( int nChar, bool bKeyDown );

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	//tracking
	virtual bool DoStartTracking( _point point );
	virtual bool DoFinishTracking( _point point );
	virtual bool DoTrack( _point point );

	virtual void EnsureVisible( _point point );

	virtual void HilightObjectImage(BITMAPINFOHEADER *pbmih, byte *pdibBits, IImage3Ptr ptrImage, COLORREF clr_highlight);

	void _InitBinaryColorsTable();

	//Persist
	com_call GetClassID( CLSID *pClassID ); 

	com_call GetFirstVisibleObjectPosition( long *plpos );
	com_call GetNextVisibleObject( IUnknown ** ppunkObject, long *plPos );

	com_call GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch );

	com_call GetMultiFrame(IUnknown **ppunkObject, BOOL bDragFrame);

	//IMultiSelection
	com_call SelectObject( IUnknown* punkObj, BOOL bAddToExist );
	com_call UnselectObject( IUnknown* punkObj );
	com_call GetObjectsCount(DWORD* pnCount);
	com_call GetObjectByIdx(DWORD nIdx, IUnknown** ppunkObj);
	com_call GetFlags(DWORD* pnFlags);
	com_call SetFlags(DWORD nFlags);
	com_call GetFrameOffset(POINT* pptOffset);
	com_call SetFrameOffset(POINT ptOffset);
	com_call GetObjectOffsetInFrame(IUnknown* punkObject, POINT* pptOffset);
	com_call RedrawFrame();
	com_call EmptyFrame();
	com_call SyncObjectsRect();

	//IPrintView
	com_call GetPrintWidth( int nMaxWidth, int *pnReturnWidth, BOOL *pbContinue, int nUserPosX, int *pnNewUserPosX );
	com_call GetPrintHeight( int nMaxHeight, int *pnReturnHeight, BOOL *pbContinue, int nUserPosY, int *pnNewUserPosY );
	com_call Print( HDC hdc, RECT rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, BOOL bUseScrollZoom, DWORD dwFlags );
	
	//IMenuInitializer
	com_call OnInitPopup( BSTR bstrMenuName, HMENU hMenu, HMENU *phOutMenu );

	void BuildView(bool bNewObjectsAttached);
	void CalcCellSizeByList();
	ClassCell* CalcClassCellLayout( long keyClass, long nWidthOfClassCell, double fZoom, bool bActive, bool bFillUnknown);
	void _KillClassCellsList();
	void SetClientSize(SIZE size);
	void GetClientSize(SIZE& size);
	void DrawClassCell(HDC hdc, HBITMAP hDIBSection, _rect rectPaint, _point ptScroll, double fZoom, ClassCell* pClassCell, long& nCurHeightPos, bool bActiveClass);
	void _ReadParams();
	long _GetFontHeight(LOGFONT* pFont);
	_rect _CalcActiveClassCell(int x, int y);
	void _SetActiveClassCell(IUnknown *punkObject);
	void _DrawTablText(HDC hdc, _rect rcText, _bstr_t bstrText, bool bFirstRow, bool b3D);
	void BuildPrint(long nMaxWidth);
	void _DrawUnit(HDC hdcMem, IUnknown* punkObject, bool bActive, _point ptOrig, bool bPrint, double fZoom = 1.);
	bool _DrawUnitTransparent(HDC hdcMem, 
		BITMAPINFOHEADER bmih, byte *pdibBits,
		IUnknown* punkObject, bool bActive, _point pt, bool bPrint, double fZoom, _rect rc);
	void _DrawResultBar(HDC hdcMem, long& nCurHeightPos, double fZoom, bool bPrint, long nStartClass = 0, long nCount = -1);
	void DrawDragObj(HDC hdc, HBITMAP hDIBSection,
		BITMAPINFOHEADER bmih, byte *pdibBits,
		_rect rectPaint, _point ptScroll, double fZoom);

	//AView disp
	com_call ScrollToClass( long nClass);

	//IViewDispatch
	com_call GetDispIID( GUID* pguid );

protected:
	INamedDataObject2Ptr	m_sptrObjectList;
//	INamedDataObject2Ptr	m_sptrClassList;
	long					m_nCellSize;
	_list_t2<ClassCell*>		m_listClassCells;
	double					m_fPrevZoom;
	_rect					m_rcPrevSelected;
	_rect					m_rcPrevSelectedText;
	long					m_nActiveClass;
	long					m_nActiveThumb; //in class
	tmapGUID2BOOL			m_mapClassifiedObjects;
	GuidKey					m_keyExcludeIt;
	bool					m_bPrintInited;
	long					m_nObjectsInRow;
	long					m_nPrintWidth;

	long					m_nMinBorder;

	//drag'n'drop
	IUnknownPtr m_sptrDragObj;
	_point		m_ptDragPos;
	_rect		m_rcPrevDrag;
	_point		m_ptDragOffset;

	bool		m_bTimerSet;
	_point		m_ptLastDelta;
	_point		m_ptLastMousePos;

	// for rename:
	CRenameEditBox m_RenameEditBox;
	
	//params
	LOGFONT					m_fontClassCellsHeader;
	COLORREF				m_clrClassCellsHeader;
	long					m_nHeaderHeight;
	LOGFONT					m_fontText; //text is description for thumbs
	long					m_nTextHeight;
	long					m_nInterCellDist;
	long					m_nThumbToTextDist;
	bool					m_bDrawThumb;
	bool					m_bDrawText;
	bool					m_bHeaderAtTop;
	COLORREF				m_clrActive;
	COLORREF				m_clrInactive;
	COLORREF				m_clrPrint;
	COLORREF				m_clrActiveText;
	COLORREF				m_clrInactiveText;
	bool					m_bDrawEdge;
	bool					m_bDrawEdgeOnPrint;
	bool					m_b3D;
	bool					m_b3DOnPrint;
	long					m_nMinThumbSize;
	long					m_nMaxPrintResolution;
	UINT					m_wheelHorzSwitch;
	bool					m_bUseMouseWheel;
	long					m_nWheelStep;
	bool					m_bShowUnknown;
	int						m_nHighlightPercent;
	int						m_nHighlightPhases;
};


#endif //__aview_h__
