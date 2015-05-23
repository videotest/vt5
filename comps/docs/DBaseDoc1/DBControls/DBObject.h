#if !defined(AFX_DBOBJECT_H__A746DB6F_DB01_4DCF_B20B_45F4D3C344B0__INCLUDED_)
#define AFX_DBOBJECT_H__A746DB6F_DB01_4DCF_B20B_45F4D3C344B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DBObject.h : header file
//




#include "DBControl.h"

//#include "dtpicker.h"
#include "PickerCtrl.h"
#include "\vt5\common\ax_ctrl_misc.h"


/////////////////////////////////////////////////////////////////////////////
// CDBObject : See DBObject.cpp for implementation.

class CDBObject :	public COleControl, 
					public CDBControlImpl, 
					public CEventListenerImpl
{
	DECLARE_AX_DATA_SITE(CDBObject)

	DECLARE_DYNCREATE(CDBObject)
	ENABLE_MULTYINTERFACE()		
	DECLARE_INTERFACE_MAP()	

	enum DrawControlMode
	{
		dcmAlreadyZoom = 0,
		dcmTopLeft = 1,
		dcmPicture = 2,
		dcmMaximal = 2, // for use in if's
	};

	struct CDrawControlMode
	{
		DrawControlMode dcm;
		int nLimitProc;
		CString sPicturePath;
		HBITMAP bmp;
		BOOL bZoomPicture;
		int nPictureZoomMode;

		CDrawControlMode()
		{
			dcm = dcmAlreadyZoom;
			nLimitProc = 0;
			bmp = NULL;
		};
		~CDrawControlMode()
		{
			if (bmp != NULL)
				DeleteObject(bmp);
		}
	};

	CDrawControlMode m_DrawControlMode;
	void InitDrawControlMode();
	
// Constructor
public:
	CDBObject();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBObject)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();	
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CDBObject();	

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	DECLARE_OLECREATE_EX(CDBObject)    // Class factory and guid
	DECLARE_OLETYPELIB(CDBObject)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CDBObject)     // Property page IDs
	DECLARE_OLECTLTYPE(CDBObject)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CDBObject)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CDBObject)
	afx_msg BOOL GetAutoLabel();
	afx_msg void SetAutoLabel(BOOL bNewValue);
	afx_msg BOOL GetEnableEnumeration();
	afx_msg void SetEnableEnumeration(BOOL bNewValue);
	afx_msg BSTR GetTableName();
	afx_msg void SetTableName(LPCTSTR lpszNewValue);
	afx_msg BSTR GetFieldName();
	afx_msg void SetFieldName(LPCTSTR lpszNewValue);
	afx_msg short GetLabelPosition();
	afx_msg void SetLabelPosition(short nNewValue);
	afx_msg BSTR GetViewType();
	afx_msg void SetViewType(LPCTSTR lpszNewValue);
	afx_msg BOOL GetAutoViewType();
	afx_msg void SetAutoViewType(BOOL bNewValue);
	//afx_msg BOOL GetReadOnly();
	//afx_msg void SetReadOnly(BOOL bNewValue);
	afx_msg BOOL GetPropertyAXStatus();
	afx_msg void SetPropertyAXStatus(BOOL bNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CDBObject)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CDBObject)
	dispidAutoLabel = 1L,
	dispidEnableEnumeration = 2L,
	dispidTableName = 3L,
	dispidFieldName = 4L,
	dispidLabelPosition = 5L,
	dispidViewType = 6L,
	dispidAutoViewType = 7L,
	//dispidReadOnly = 8L,
	dispidGetPropertyAXStatus = 8L,
	dispidSetPropertyAXStatus = 9L,
	//}}AFX_DISP_ID
	};
protected:	

	//IUnknown* m_pUnkViewAxCtrl;

	CString m_strViewProgID;
	BOOL	m_bAutoViewType;

	CWnd m_ctrlAXView;
	//CPickerCtrl m_ctrlPicker;
	void Resize();

	void SetViewName();



	void OnPropertyChange();
	virtual void Serialize(CArchive& ar);

	void GetValue(sptrIDBaseDocument spDBaseDoc);

	//follow members in CDBControlImpl
	//
	//	CString m_strTableName;
	//	CString m_strFieldName;
	//	bool m_bEnableEnumeration;
	//	bool m_bAutoLabel;
	//
	//	CDBControlImpl implementation
	
	virtual void _RepaintCtrl(){ if( GetSafeHwnd() ) { Invalidate(); } };

	//Notification
	virtual void IDBControl_OnSetTableName(  );
	virtual void IDBControl_OnSetFieldName(  );
	virtual void IDBControl_OnSetEnableEnumeration( );
	virtual void IDBControl_OnSetAutoLabel( );
	virtual void IDBControl2_OnSetReadOnly( );

	virtual void IDBControl_GetValue( tagVARIANT *pvar );
	virtual void IDBControl_SetValue( const tagVARIANT var );

	virtual void IDBControl_BuildAppearanceAfterLoad( );
	virtual void IDBControl_OnSetLabelPosition( );

	virtual void IDBControl_ChangeValue( const tagVARIANT var );

	virtual void OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var );	

	virtual void SetEnableWindow( bool bEnable );
	virtual void EnterViewMode( BlankViewMode newBlankMode );


	BEGIN_INTERFACE_PART(DBObjectControl, IDBObjectControl)
		com_call SetViewType( BSTR bstrViewName );
		com_call GetViewType( BSTR* pbstrViewName );

		com_call SetAutoViewType( BOOL bAutoViewType );
		com_call GetAutoViewType( BOOL* pbAutoViewType );

		com_call Build();
	END_INTERFACE_PART(DBObjectControl)
	
	BEGIN_INTERFACE_PART(PrintCtrl, IVTPrintCtrl)
		com_call Draw( HDC hDC, SIZE sizeVTPixel );
		com_call FlipHorizontal(  ){ return S_OK;}
		com_call FlipVertical(  ){ return S_OK;}
		com_call SetPageInfo( int nCurPage, int nPageCount ){ return S_OK;}
		com_call NeedUpdate( BOOL* pbUpdate ){ return S_OK;}
	END_INTERFACE_PART(PrintCtrl)

protected:

	FONTDESC	m_fontDesc;
	CFontHolder m_fontHolder;

	CSize GetLabelSize( CDC* pdc );	
	
	BEGIN_INTERFACE_PART(ViewCtrl, IViewCtrl)

		com_call GetApp(IUnknown** ppunkApp);
		com_call GetDoc(IUnknown** ppunkDoc);

		//Auto build mode
		com_call GetAutoBuild( BOOL* pbAutoBuild );
		com_call SetAutoBuild( BOOL bAutoBuild );

		com_call Build( BOOL* pbSucceded );

		//size
		com_call GetDPI( double* pfDPI );
		com_call SetDPI( double fDPI );

		com_call GetZoom( double* pfZoom );
		com_call SetZoom( double fZoom );

		com_call GetObjectTransformation( short* pnObjectTransformation );
		com_call SetObjectTransformation( short nObjectTransformation );

		//active view
		com_call GetUseActiveView( BOOL* pbUseActiveView );
		com_call SetUseActiveView( BOOL bUseActiveView );

		com_call GetViewAutoAssigned( BOOL* pbViewAutoAssigned );
		com_call SetViewAutoAssigned( BOOL bViewAutoAssigned );

		com_call GetViewProgID( BSTR* pbstrProgID );
		com_call SetViewProgID( BSTR bstrProgID );

		//object list
		com_call GetFirstObjectPosition( long* plPos );
		com_call GetNextObject( BSTR* pbstrObjectName, BOOL* pbActiveObject, 
								BSTR* bstrObjectType, long *plPos );
		com_call InsertAfter( long lPos,  
								BSTR bstrObjectName, BOOL bActiveObject, 
								BSTR bstrObjectType );
		com_call EditAt( long lPos,  
								BSTR bstrObjectName, BOOL bActiveObject, 
								BSTR bstrObjectType );
		com_call RemoveAt( long lPos );
		com_call DestroyView();
	END_INTERFACE_PART(ViewCtrl)
		

	BEGIN_INTERFACE_PART(PrintView, IPrintView)
		com_call GetPrintWidth( int nMaxWidth, int *pnReturnWidth, BOOL *pbContinue, int nUserPosX, int *pnNewUserPosX );
		com_call GetPrintHeight( int nMaxHeight, int *pnReturnHeight, BOOL *pbContinue, int nUserPosY, int *pnNewUserPosY );
		com_call Print( HDC hdc, RECT rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, BOOL bUseScrollZoom, DWORD dwFlags );
	END_INTERFACE_PART(PrintView)


	BEGIN_INTERFACE_PART(VtActiveXCtrl, IVtActiveXCtrl)
		com_call SetSite( IUnknown *punkVtApp, IUnknown *punkSite );
		com_call GetName( BSTR *pbstrName );
	END_INTERFACE_PART(VtActiveXCtrl)

	
	BEGIN_INTERFACE_PART(VtActiveXCtrl2, IVtActiveXCtrl2)
		com_call SetSite( IUnknown *punkVtApp, IUnknown *punkSite );
		com_call GetName( BSTR *pbstrName );
		com_call SetDoc( IUnknown *punkDoc );
		com_call SetApp( IUnknown *punkVtApp );
	END_INTERFACE_PART(VtActiveXCtrl2)


		/*
	BEGIN_INTERFACE_PART(PrintCtrl, IVTPrintCtrl)				
		com_call Draw( HDC hDC, SIZE sizeVTPixel );
		com_call FlipHorizontal(  );
		com_call FlipVertical(  );
		com_call SetPageInfo( int nCurPage, int nPageCount );
		com_call NeedUpdate( BOOL* pbUpdate );
	END_INTERFACE_PART(PrintCtrl)
	*/



	IUnknown* GetViewAXControlUnknown();
	

//	void DrawBorder( CDC* pdc, CRect rcBorder, bool bFocused );

	//added by akm 13_12_k5
	BOOL		m_PropertyAXStatus;

public:
	virtual BOOL OnSetObjectRects(LPCRECT lpRectPos, LPCRECT lpRectClip);

	virtual void RefreshAppearance(){}

	bool ChangeViewType();
	afx_msg void OnDestroy();
	CString	get_object_name();
	CStringArray	m_ar_objects;
private:
	unsigned long m_ViewSubType;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBOBJECT_H__A746DB6F_DB01_4DCF_B20B_45F4D3C344B0__INCLUDED_)
