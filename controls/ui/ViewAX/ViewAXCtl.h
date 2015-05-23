#if !defined(AFX_VIEWAXCTL_H__73395963_19B7_47B4_A9FB_42FD51B54D85__INCLUDED_)
#define AFX_VIEWAXCTL_H__73395963_19B7_47B4_A9FB_42FD51B54D85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ViewAXCtl.h : Declaration of the CViewAXCtrl ActiveX Control class.


#include "PropBagImpl.h"
#include "iface.h"

////////////////////////////////////////////////////////////////////////////
//
//
//	enum ObjectName
//
//
////////////////////////////////////////////////////////////////////////////
enum ObjectName
{
	onManual = 0,
	onAuto = 1,
};

////////////////////////////////////////////////////////////////////////////
//
//
//	class CObjectDefinition
//
//
////////////////////////////////////////////////////////////////////////////
class CObjectDefinition
{
public:
	CObjectDefinition();	
	void CopyFrom( CObjectDefinition* pSource );


	CString m_strObjectName;
	CString m_strObjectType;
	ObjectName m_objectName;

	void Serialize(CArchive& ar);
	bool m_bBaseObject;

};

/////////////////////////////////////////////////////////////////////////////
// CViewAXCtrl : See ViewAXCtl.cpp for implementation.

////////////////////////////////////////////////////////////////////////////
//
//
//	class CViewAXCtrl
//
//
////////////////////////////////////////////////////////////////////////////
class CViewAXCtrl : public COleControl,
					public CScrollZoomSiteImpl,
					public CNamedPropBagImpl
{
	DECLARE_DYNCREATE(CViewAXCtrl)
	ENABLE_MULTYINTERFACE()

// Constructor
public:
	CViewAXCtrl();

	virtual LPUNKNOWN GetInterfaceHook(const void* p)
	{
		if (*(IID*)p == IID_INamedPropBag || *(IID*)p == IID_INamedPropBagSer)
			return (INamedPropBagSer*)this;
		else return __super::GetInterfaceHook(p);
	}
	
	virtual CWnd *GetWindow(){return (CWnd*)this;};
	virtual CScrollBar *GetScrollBarCtrl( int sbCode ){return 0;};
	virtual DWORD GetControlFlags();

	virtual void OnChange() { if( m_hWnd ) Invalidate(); };

	/*

	void ResizeControl();
	void RebuildView();
	virtual void OnFinalRelease();
	*/

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewAXCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();	
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CViewAXCtrl();
	virtual void Serialize(CArchive& ar);
	
		
	
	GUARD_DECLARE_OLECREATE_CTRL(CViewAXCtrl)    // Class factory and guid
	

	DECLARE_OLETYPELIB(CViewAXCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CViewAXCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CViewAXCtrl)		// Type name and misc status

	BEGIN_INTERFACE_PART(ActiveXCtrl2, IVtActiveXCtrl2)
		com_call SetSite( IUnknown *punkVtApp, IUnknown *punkSite );
		com_call GetName( BSTR *pbstrName );	//such as Editor1
		com_call SetDoc( IUnknown *punkDoc );
		com_call SetApp( IUnknown *punkVtApp );
	END_INTERFACE_PART(ActiveXCtrl2)

	BEGIN_INTERFACE_PART(ViewAXBag, IViewAXPropBag)
		com_call SetProperty( BSTR bstrName, VARIANT var );
		com_call GetProperty( BSTR bstrName, VARIANT* pvar );
	END_INTERFACE_PART(ViewAXBag)

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
		com_call GetFirstObjectPosition(LONG_PTR* plPos);
		com_call GetNextObject( BSTR* pbstrObjectName, BOOL* pbActiveObject, 
			BSTR* bstrObjectType, LONG_PTR *plPos);
		com_call InsertAfter(LONG_PTR lPos,
								BSTR bstrObjectName, BOOL bActiveObject, 
								BSTR bstrObjectType );
		com_call EditAt(LONG_PTR lPos,
								BSTR bstrObjectName, BOOL bActiveObject, 
								BSTR bstrObjectType );
		com_call RemoveAt(LONG_PTR lPos);
		
		com_call DestroyView();

	END_INTERFACE_PART(ViewCtrl)

	DECLARE_INTERFACE_MAP()

// Message maps
	//{{AFX_MSG(CViewAXCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CViewAXCtrl)
	afx_msg double GetZoom();
	afx_msg void SetZoom(double newValue);
	afx_msg double GetDPI();
	afx_msg void SetDPI(double newValue);
	afx_msg BOOL GetAutoBuild();
	afx_msg void SetAutoBuild(BOOL bNewValue);
	afx_msg short GetObjectTransformation();
	afx_msg void SetObjectTransformation(short nNewValue);
	afx_msg BOOL GetUseActiveView();
	afx_msg void SetUseActiveView(BOOL bNewValue);
	afx_msg BOOL GetViewAutoAssigned();
	afx_msg void SetViewAutoAssigned(BOOL bNewValue);
	afx_msg BSTR GetViewName();
	afx_msg void SetViewName(LPCTSTR lpszNewValue);
	afx_msg BOOL GetUseObjectDPI();
	afx_msg void SetUseObjectDPI(BOOL bNewValue);
	afx_msg double GetObjectDPI();
	afx_msg void SetObjectDPI(double newValue);
	afx_msg LONG_PTR GetFirstObjectPos();
	afx_msg LONG_PTR GetNextObject(VARIANT FAR* varObjectName, VARIANT FAR* varActiveObject, VARIANT FAR* varObjectType, VARIANT FAR* varPos);
	afx_msg BOOL AddObject(LPCTSTR strObjectName, BOOL bActiveObject, LPCTSTR strObjectType);
	afx_msg BOOL DeleteObject(long lPos);
	afx_msg long GetFirstPropertyPos();
	afx_msg long GetNextProperty(VARIANT FAR* varName, VARIANT FAR* varValue, VARIANT FAR* varPos);
	afx_msg BOOL SetProperty(LPCTSTR strName, const VARIANT FAR& varValue);
	afx_msg BOOL DeletePropery(long lPos);
	afx_msg BOOL Rebuild();
	afx_msg BSTR GetViewProgID();
	afx_msg void SetViewProgID(LPCTSTR lpszNewValue);
	afx_msg BOOL GetPropertyAXStatus();
	afx_msg void SetPropertyAXStatus(BOOL bNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CViewAXCtrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CViewAXCtrl)
	dispidZoom = 1L,
	dispidDPI = 2L,
	dispidAutoBuild = 3L,
	dispidObjectTransformation = 4L,
	dispidUseActiveView = 5L,
	dispidViewAutoAssigned = 6L,
	dispidViewName = 7L,
	dispidUseObjectDPI = 8L,
	dispidObjectDPI = 9L,
	dispidGetFirstObjectPos = 10L,
	dispidGetNextObject = 11L,
	dispidAddObject = 12L,
	dispidDeleteObject = 13L,
	dispidGetFirstPropertyPos = 14L,
	dispidGetNextProperty = 15L,
	dispidAddProperty = 16L,
	dispidDeletePropery = 17L,
	dispidRebuild = 18L,
	dispidViewProgID = 19L,
	dispidGetPropertyAXStatus = 20L,
	dispidSetPropertyAXStatus = 21L,
	//}}AFX_DISP_ID
	};


private:
	IApplicationPtr		m_sptrApp;
	IDocumentSitePtr	m_sptrDoc;
	
	

	IUnknown*			m_punkContext;
	IUnknown*			m_punkView;

protected:
	//Properties:
	bool					m_bInitOK;

	//Auto build mode
	bool					m_bAutoBuild;
	
	//Size:
	double					m_fVZoom;
	double					m_fDPI;
	ObjectTransformation	m_nObjectTransformation;

	bool					m_bUseActiveView;
	bool					m_bViewAutoAssigned;

	CString					m_strViewProgID;

	//added by akm 25_10_k5
	BOOL					m_PropertyAXStatus;

	//Calibration
	bool					m_bUseObjectDPI;
	double					m_fObjectDPI;

	CPtrList				m_ObjectList;

	bool TryBuild();
	bool Build();


	bool _GetFirstObjectPosition(LONG_PTR* plPos);
	bool _GetNextObject( CString& strObjectName, bool& bActiveObject, 
		CString& strObjectType, LONG_PTR *plPos);
	bool _InsertAfter(LONG_PTR lPos,
							CString strObjectName, bool bActiveObject, 
							CString strObjectType );
	bool _EditAt(LONG_PTR lPos,
							CString strObjectName, bool bActiveObject, 
							CString strObjectType );
	bool _RemoveAt(LONG_PTR lPos);

	void DestroyObjectList( CPtrList* pOL );

	void SerializeBool( CArchive& ar, bool& b );

	void DestroyAggregates();

	void ResizeView();

	IUnknown* GetActiveView();
	IUnknown* _get_unknown();
public:
	virtual void OnClose(DWORD dwSaveOption);
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWAXCTL_H__73395963_19B7_47B4_A9FB_42FD51B54D85__INCLUDED)
