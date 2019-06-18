// ViewAXCtl.cpp : Implementation of the CViewAXCtrl ActiveX Control class.

#include "stdafx.h"
#include "ViewAX.h"
#include "ViewAXCtl.h"
//#include "ViewAXPpg.h"
#include "ViewAXObjectPropPage.h"
#include "ViewAXSizePropPage.h"
#include "ViewAXCGHPropPage.h"
#include "ViewAXStatUIPropPage.h"

#include "obj_types.h"
#include "cgh_int.h"

#include "..\\common2\\misc_calibr.h"
#include "\vt5\awin\profiler.h"

//Utils.cpp
std_dll CSize ThumbnailGetRatioSize( CSize sizeThumbnail, CSize sizeImage );

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LPSTREAM AFXAPI _AfxCreateMemoryStream()
{
	LPSTREAM lpStream = NULL;

	// Create a stream object on a memory block.
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, 0);
	if (hGlobal != NULL)
	{
		if (FAILED(CreateStreamOnHGlobal(hGlobal, TRUE, &lpStream)))
		{
			TRACE0("CreateStreamOnHGlobal failed.\n");
			GlobalFree(hGlobal);
			return NULL;
		}

		ASSERT_POINTER(lpStream, IStream);
	}
	else
	{
		TRACE0("Failed to allocate memory for stream.\n");
		return NULL;
	}

	return lpStream;
}

////////////////////////////////////////////////////////////////////////////
//
//
//	class CObjectDefinition
//
//
////////////////////////////////////////////////////////////////////////////
CObjectDefinition::CObjectDefinition()
{
	m_strObjectName.Empty();
	m_strObjectType.Empty();
	m_objectName = onAuto;
	m_bBaseObject = false;

}

////////////////////////////////////////////////////////////////////////////
void CObjectDefinition::CopyFrom( CObjectDefinition* pSource )
{
	if( !pSource )
		return;

	m_strObjectName	= pSource->m_strObjectName;
	m_strObjectType	= pSource->m_strObjectType;
	m_objectName	= pSource->m_objectName;


}

////////////////////////////////////////////////////////////////////////////
void CObjectDefinition::Serialize(CArchive& ar)
{
	long nVersion = 1;
	
	if( ar.IsStoring() )
	{		
		ar<<nVersion;
		ar<<m_strObjectName;
		ar<<m_strObjectType;
		ar<<(short)m_objectName;		
	}
	else
	{
		ar>>nVersion;
		ar>>m_strObjectName;
		ar>>m_strObjectType;
		short n;
		ar>>n;
		m_objectName = (ObjectName)n;
	}
}



IMPLEMENT_DYNCREATE(CViewAXCtrl, COleControl)

BEGIN_INTERFACE_MAP(CViewAXCtrl, COleControl)
	INTERFACE_PART(CViewAXCtrl, IID_IVtActiveXCtrl, ActiveXCtrl2)
	INTERFACE_PART(CViewAXCtrl, IID_IVtActiveXCtrl2, ActiveXCtrl2)
	INTERFACE_PART(CViewAXCtrl, IID_IScrollZoomSite, ScrollSite)
	INTERFACE_PART(CViewAXCtrl, IID_IScrollZoomSite2, ScrollSite)
	INTERFACE_PART(CViewAXCtrl, IID_IViewCtrl, ViewCtrl)
	INTERFACE_PART(CViewAXCtrl, IID_IViewAXPropBag, ViewAXBag )	
	INTERFACE_AGGREGATE(CViewAXCtrl, m_punkContext)
	INTERFACE_AGGREGATE(CViewAXCtrl, m_punkView)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CViewAXCtrl, COleControl)
	//{{AFX_MSG_MAP(CViewAXCtrl)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CViewAXCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CViewAXCtrl)
	DISP_PROPERTY_EX(CViewAXCtrl, "Zoom", GetZoom, SetZoom, VT_R8)
	DISP_PROPERTY_EX(CViewAXCtrl, "DPI", GetDPI, SetDPI, VT_R8)
	DISP_PROPERTY_EX(CViewAXCtrl, "AutoBuild", GetAutoBuild, SetAutoBuild, VT_BOOL)
	DISP_PROPERTY_EX(CViewAXCtrl, "ObjectTransformation", GetObjectTransformation, SetObjectTransformation, VT_I2)
	DISP_PROPERTY_EX(CViewAXCtrl, "UseActiveView", GetUseActiveView, SetUseActiveView, VT_BOOL)
	DISP_PROPERTY_EX(CViewAXCtrl, "ViewAutoAssigned", GetViewAutoAssigned, SetViewAutoAssigned, VT_BOOL)
	DISP_PROPERTY_EX(CViewAXCtrl, "ViewName", GetViewName, SetViewName, VT_BSTR)	
	DISP_PROPERTY_EX(CViewAXCtrl, "UseObjectDPI", GetUseObjectDPI, SetUseObjectDPI, VT_BOOL)
	DISP_PROPERTY_EX(CViewAXCtrl, "ObjectDPI", GetObjectDPI, SetObjectDPI, VT_R8)	
	DISP_FUNCTION(CViewAXCtrl, "GetFirstObjectPos", GetFirstObjectPos, VT_I4, VTS_NONE)
	DISP_FUNCTION(CViewAXCtrl, "GetNextObject", GetNextObject, VT_I4, VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT)
	DISP_FUNCTION(CViewAXCtrl, "AddObject", AddObject, VT_BOOL, VTS_BSTR VTS_BOOL VTS_BSTR)
	DISP_FUNCTION(CViewAXCtrl, "DeleteObject", DeleteObject, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CViewAXCtrl, "GetFirstPropertyPos", GetFirstPropertyPos, VT_I4, VTS_NONE)
	DISP_FUNCTION(CViewAXCtrl, "GetNextProperty", GetNextProperty, VT_I4, VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT)
	DISP_FUNCTION(CViewAXCtrl, "SetProperty", SetProperty, VT_BOOL, VTS_BSTR VTS_VARIANT)
	DISP_FUNCTION(CViewAXCtrl, "DeletePropery", DeletePropery, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CViewAXCtrl, "Rebuild", Rebuild, VT_BOOL, VTS_NONE)
	DISP_PROPERTY_EX(CViewAXCtrl, "ViewProgID", GetViewProgID, SetViewProgID, VT_BSTR)	
	DISP_FUNCTION(CViewAXCtrl, "GetPropertyAXStatus", GetPropertyAXStatus, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CViewAXCtrl, "SetPropertyAXStatus", SetPropertyAXStatus, VT_EMPTY, VTS_BOOL)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CViewAXCtrl, COleControl)
	//{{AFX_EVENT_MAP(CViewAXCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
/*BEGIN_PROPPAGEIDS(CViewAXCtrl, 2)
	//PROPPAGEID(CViewAXPropPage::guid)
	PROPPAGEID(CViewAXObjectPropPage::guid)
	PROPPAGEID(CViewAXSizePropPage::guid)
END_PROPPAGEIDS(CViewAXCtrl)
*/

static CLSID pages[3];

LPCLSID CViewAXCtrl::GetPropPageIDs( ULONG& cPropPages )
{
	cPropPages = 0;

	if( ::GetValueInt( ::GetAppUnknown(), "\\ViewAX", "UseObjectPane", 1 ) != 0 )
		pages[cPropPages++] = CViewAXObjectPropPage::guid;

	pages[cPropPages++] = CViewAXSizePropPage::guid;

	if( ::GetValueInt( ::GetAppUnknown(), "\\ViewAX", "UseCGHPane", 0 ) != 0 )
		pages[cPropPages++] = CViewAXCGHPropPage::guid;
	if( ::GetValueInt( ::GetAppUnknown(), "\\ViewAX", "UseStatUIPane", 0 ) != 0 )
		pages[cPropPages++] = CViewAXStatUIPropPage::guid;

	//added by akm 25_10_k5
	m_PropertyAXStatus=true;

	return pages;
}


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid
// {14C00616-4768-4091-9231-4D18C101FFBB}
GUARD_IMPLEMENT_OLECREATE_CTRL(CViewAXCtrl, "VIEWAX.ViewAXCtrl.1",
0x14c00616, 0x4768, 0x4091, 0x92, 0x31, 0x4d, 0x18, 0xc1, 0x1, 0xff, 0xbb);



/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CViewAXCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DViewAX =
		{ 0xa108b56b, 0xb544, 0x4eb3, { 0xac, 0x1d, 0x10, 0, 0x64, 0x35, 0xd2, 0x3 } };
const IID BASED_CODE IID_DViewAXEvents =
		{ 0x5a5575ea, 0xbb7d, 0x45a3, { 0x97, 0x1b, 0x8f, 0x21, 0x81, 0xd7, 0xe6, 0x83 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwViewAXOleMisc =
	//OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CViewAXCtrl, IDS_VIEWAX, _dwViewAXOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CViewAXCtrl::CViewAXCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CViewAXCtrl

BOOL CViewAXCtrl::CViewAXCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegInsertable | afxRegApartmentThreading to afxRegInsertable.
	return UpdateRegistryCtrl(bRegister, AfxGetInstanceHandle(), IDS_VIEWAX, IDB_VIEWAX,
							  afxRegInsertable | afxRegApartmentThreading, _dwViewAXOleMisc,
							  _tlid, _wVerMajor, _wVerMinor);
}


////////////////////////////////////////////////////////////////////////////
int CViewAXCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//TryBuild();	
	m_PropertyAXStatus=false;
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CViewAXCtrl IVtActiveXCtrl2 implement
IMPLEMENT_UNKNOWN(CViewAXCtrl, ActiveXCtrl2);

HRESULT CViewAXCtrl::XActiveXCtrl2::SetSite( IUnknown *punkVtApp, IUnknown *punkSite )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ActiveXCtrl2)

	pThis->m_sptrApp = punkVtApp;
	
	/*
	//init doc ptr
	if(pThis->m_sptrApp != 0 && pThis->m_sptrDoc == 0)
	{
		IUnknown *punkDoc = 0;
		pThis->m_sptrApp->GetActiveDocument(&punkDoc);
		pThis->m_sptrDoc = punkDoc;
		if(punkDoc)
			punkDoc->Release();
	}
	*/

	pThis->TryBuild();

	return S_OK;
}
  

HRESULT CViewAXCtrl::XActiveXCtrl2::GetName( BSTR *pbstrName )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ActiveXCtrl2)	
	*pbstrName = NULL;
	return S_OK;
}

HRESULT CViewAXCtrl::XActiveXCtrl2::SetDoc( IUnknown *punkDoc )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ActiveXCtrl2)	

	pThis->m_sptrDoc = punkDoc;
	//pThis->TryBuild();

	return S_OK;
}

HRESULT CViewAXCtrl::XActiveXCtrl2::SetApp( IUnknown *punkVtApp )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ActiveXCtrl2)	

	pThis->m_sptrApp = punkVtApp;
	pThis->TryBuild();

	return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// CViewAXCtrl IViewCtrl implement
IMPLEMENT_UNKNOWN(CViewAXCtrl, ViewCtrl);

////////////////////////////////////////////////////////////////////////////
double CViewAXCtrl::GetZoom() 
{
	return m_fVZoom;
}

////////////////////////////////////////////////////////////////////////////
void CViewAXCtrl::SetZoom(double newValue) 
{
	m_fVZoom = newValue;
	//m_fVZoom  = m_fVZoom;
	SetModifiedFlag();
	//TryBuild();
	//ResizeControl();
}

////////////////////////////////////////////////////////////////////////////
double CViewAXCtrl::GetDPI() 
{		
	return m_fDPI;
}

////////////////////////////////////////////////////////////////////////////
void CViewAXCtrl::SetDPI(double fNewValue) 
{
	m_fDPI = fNewValue;
	SetModifiedFlag();
	//TryBuild();
	//ResizeControl();
}

////////////////////////////////////////////////////////////////////////////
BOOL CViewAXCtrl::GetAutoBuild() 
{	
	return m_bAutoBuild;
}

////////////////////////////////////////////////////////////////////////////
void CViewAXCtrl::SetAutoBuild(BOOL bNewValue) 
{	
	m_bAutoBuild = (bNewValue == TRUE ? true : false );
	SetModifiedFlag();
	//TryBuild();
	
}

////////////////////////////////////////////////////////////////////////////
short CViewAXCtrl::GetObjectTransformation() 
{
	return m_nObjectTransformation;
}

////////////////////////////////////////////////////////////////////////////
void CViewAXCtrl::SetObjectTransformation(short nNewValue) 
{
	m_nObjectTransformation = (ObjectTransformation)nNewValue;
	SetModifiedFlag();
	//TryBuild();
	//ResizeControl();
}

////////////////////////////////////////////////////////////////////////////
BOOL CViewAXCtrl::GetUseActiveView() 
{
	return m_bUseActiveView;
}

////////////////////////////////////////////////////////////////////////////
void CViewAXCtrl::SetUseActiveView(BOOL bNewValue) 
{	
	m_bUseActiveView = (bNewValue == TRUE ? true : false );
	SetModifiedFlag();
	//TryBuild();
}

////////////////////////////////////////////////////////////////////////////
BOOL CViewAXCtrl::GetViewAutoAssigned() 
{
	return m_bViewAutoAssigned;
}

////////////////////////////////////////////////////////////////////////////
BOOL CViewAXCtrl::GetUseObjectDPI()
{
	return ( m_bUseObjectDPI == TRUE );
}

////////////////////////////////////////////////////////////////////////////
void CViewAXCtrl::SetUseObjectDPI(BOOL bNewValue)
{
	m_bUseObjectDPI = ( TRUE == bNewValue);
}

////////////////////////////////////////////////////////////////////////////
void CViewAXCtrl::SetObjectDPI(double fNewValue)
{
	AfxMessageBox( "ViewAX: [ObjectDPI] Read only property. " );
}

////////////////////////////////////////////////////////////////////////////
double CViewAXCtrl::GetObjectDPI()
{
	return m_fObjectDPI;
}


////////////////////////////////////////////////////////////////////////////
void CViewAXCtrl::SetViewAutoAssigned(BOOL bNewValue) 
{
	m_bViewAutoAssigned = (bNewValue == TRUE ? true : false );
	SetModifiedFlag();
	//TryBuild();
}

////////////////////////////////////////////////////////////////////////////
BSTR CViewAXCtrl::GetViewName() 
{
	CString strResult = GetViewNameByProgID( GetViewProgID(), true );	
	return strResult.AllocSysString();
}

////////////////////////////////////////////////////////////////////////////
void CViewAXCtrl::SetViewName(LPCTSTR lpszNewValue) 
{	
	SetViewProgID( GetViewProgIDByName(lpszNewValue ) );	
	SetModifiedFlag();
	
}

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::GetApp(IUnknown** ppunkApp)
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)
	*ppunkApp = NULL;
	if( pThis->m_sptrApp != NULL )
	{
		pThis->m_sptrApp->AddRef();
		*ppunkApp = pThis->m_sptrApp;
	}
	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::GetDoc(IUnknown** ppunkDoc)
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)
	*ppunkDoc = NULL;
	if( pThis->m_sptrDoc != NULL )
	{
		pThis->m_sptrDoc->AddRef();
		*ppunkDoc = pThis->m_sptrDoc;
	}
	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::GetAutoBuild( BOOL* pbAutoBuild )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	*pbAutoBuild = pThis->GetAutoBuild();
	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::SetAutoBuild( BOOL bAutoBuild )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	pThis->SetAutoBuild( bAutoBuild );	
	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::Build( BOOL* pbSucceded )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	HRESULT hr = pThis->Build();
	if( pbSucceded )		
		*pbSucceded = (hr ? TRUE : FALSE );	
	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::GetDPI( double* pfDPI )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	*pfDPI = pThis->GetDPI();

	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::SetDPI( double fDPI )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	pThis->SetDPI( fDPI );
	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::GetZoom( double* pfZoom )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	*pfZoom = pThis->GetZoom();
	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::SetZoom( double fZoom )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	pThis->SetZoom( fZoom );
	return S_OK;
}		


////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::GetObjectTransformation( short* pnObjectTransformation )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	*pnObjectTransformation = pThis->GetObjectTransformation();
	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::SetObjectTransformation( short nObjectTransformation )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	pThis->SetObjectTransformation( nObjectTransformation );
	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::GetUseActiveView( BOOL* pbUseActiveView )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	*pbUseActiveView = pThis->GetUseActiveView();
	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::SetUseActiveView( BOOL bUseActiveView )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	pThis->SetUseActiveView( bUseActiveView );
	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::GetViewAutoAssigned( BOOL* pbViewAutoAssigned )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	*pbViewAutoAssigned = pThis->GetViewAutoAssigned();
	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::SetViewAutoAssigned( BOOL bViewAutoAssigned )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	pThis->SetViewAutoAssigned( bViewAutoAssigned );
	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::GetViewProgID( BSTR* pbstrProgID )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)		
	*pbstrProgID = pThis->GetViewProgID();
	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::SetViewProgID( BSTR bstrProgID )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	pThis->SetViewProgID( _bstr_t( bstrProgID ) );
	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::GetFirstObjectPosition( long* plPos )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	pThis->_GetFirstObjectPosition( plPos );
	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::GetNextObject( BSTR* pbstrObjectName, BOOL* pbActiveObject, 
								BSTR* bstrObjectType, long *plPos )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	CString strObjectName, strObjectType;
	bool bActiveObject = false;
	pThis->_GetNextObject( strObjectName, bActiveObject, 
							strObjectType, plPos );
	
	if( pbstrObjectName )
		*pbstrObjectName	= strObjectName.AllocSysString();

	if( pbActiveObject )
		*pbActiveObject		= ( bActiveObject == true ? TRUE : FALSE );

	if( pbActiveObject )
		*bstrObjectType		= strObjectType.AllocSysString();

	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::InsertAfter( long lPos,  
								BSTR bstrObjectName, BOOL bActiveObject, 
								BSTR bstrObjectType )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)
	pThis->_InsertAfter( lPos,  
								bstrObjectName, ( bActiveObject == TRUE ? true : false ), 
								bstrObjectType );
	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::EditAt( long lPos,  
								BSTR bstrObjectName, BOOL bActiveObject, 
								BSTR bstrObjectType )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	pThis->_EditAt( lPos,  
								bstrObjectName, ( bActiveObject == TRUE ? true : false ), 
								bstrObjectType );
	return S_OK;
}		

////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::RemoveAt( long lPos )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	pThis->_RemoveAt(lPos);
	return S_OK;
}		


////////////////////////////////////////////////////////////////////////////
HRESULT CViewAXCtrl::XViewCtrl::DestroyView()
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewCtrl)	
	pThis->DestroyAggregates();
	if( pThis->GetSafeHwnd() )
	{
		pThis->Invalidate();
		pThis->UpdateWindow();
	}
	return S_OK;
}


////////////////////////////////////////////////////////////////////////////
// CViewAXCtrl::CViewAXCtrl - Constructor

CViewAXCtrl::CViewAXCtrl()
{
	m_bInitOK = false;

	EnableAggregation();

	InitializeIIDs(&IID_DViewAX, &IID_DViewAXEvents);

	// TODO: Initialize your control's instance data here.

	m_sptrApp		= 0;
	m_sptrDoc		= 0;
	
	m_punkContext	= 0;
	m_punkView		= 0;


	m_bAutoBuild	= false;

	m_bUseObjectDPI = true;
	m_fObjectDPI	= 0.0;


	m_fVZoom = 1.0;

	{
		CClientDC	dc(0);
		m_fDPI = (double)dc.GetDeviceCaps(LOGPIXELSX);
	}

	m_nObjectTransformation = otGrowTwoSide;

	m_bUseActiveView = true;
	
	m_bViewAutoAssigned = true;

	m_strViewProgID.Empty();
}


/////////////////////////////////////////////////////////////////////////////
// CViewAXCtrl::~CViewAXCtrl - Destructor

CViewAXCtrl::~CViewAXCtrl()
{
	DestroyObjectList( &m_ObjectList );
	DestroyAggregates();

}

void CViewAXCtrl::DestroyAggregates()
{
	m_bInitOK = false;

	if (m_punkView != 0)
	{
		{
		sptrIWindow sptrWin(m_punkView);
		ASSERT(sptrWin != 0);
		sptrWin->DestroyWindow();
		}
		m_punkView->Release();
		m_punkView = 0;
	}

	
	if (m_punkContext != 0)
	{
		{
		sptrIDataContext	sptrContext(m_punkContext);
		ASSERT(sptrContext != 0);
		sptrContext->AttachData(0);
		}
		m_punkContext->Release();
		m_punkContext = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CViewAXCtrl::DestroyObjectList( CPtrList* pOL )
{
	if( !pOL )
		return;

	POSITION pos = pOL->GetHeadPosition();
	
	while( pos )
	{
		CObjectDefinition* pOD = (CObjectDefinition*)pOL->GetNext( pos );
		delete pOD;
	}

	pOL->RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
void CViewAXCtrl::SerializeBool( CArchive& ar, bool& b )
{
	BOOL Bool;
	if( ar.IsStoring() )
	{
		Bool = ( b == true ? TRUE : FALSE );
		ar<<Bool;
	}
	else
	{
		ar>>Bool;
		b = ( Bool == TRUE ? true : false );
	}
};


/////////////////////////////////////////////////////////////////////////////
void CViewAXCtrl::Serialize(CArchive& ar)
{
	long nVersion = 4;

	COleControl::Serialize( ar );

	IStream *punkStream = _AfxCreateMemoryStream();

	if( ar.IsStoring() )
	{		
		ar<<nVersion;
		SerializeBool( ar, m_bAutoBuild );		
		ar<<m_fVZoom;
		ar<<m_fDPI;
		ar<<(short)m_nObjectTransformation;
		SerializeBool( ar, m_bUseActiveView );
		SerializeBool( ar, m_bViewAutoAssigned );
		ar<<m_strViewProgID;

		DWORD dwObjectCount = m_ObjectList.GetCount();
		ar<<dwObjectCount;

		POSITION pos = m_ObjectList.GetHeadPosition();		
		while( pos )
		{
			CObjectDefinition* pOD = (CObjectDefinition*)m_ObjectList.GetNext( pos );
			pOD->Serialize( ar );			
		}

		SerializeBool( ar, m_bUseObjectDPI );
		//ar << m_fObjectDPI;

		if( nVersion >= 4 && punkStream )
		{
			((INamedPropBagSer*)this)->Store( punkStream );

			COleStreamFile sfile( punkStream );
			DWORD dwLen = sfile.GetLength();
			if( dwLen )
			{
				BYTE *lpBuf = new BYTE[dwLen];

				sfile.Seek( 0, CFile::begin );
				sfile.Read( (void*)lpBuf, dwLen );
							
				ar << dwLen;
				ar.Write( lpBuf, dwLen );

				delete []lpBuf;
			}
		}

		
	}
	else
	{
		ar>>nVersion;
		SerializeBool( ar, m_bAutoBuild );		
		ar>>m_fVZoom;
		
		if( nVersion >= 3 )
			ar>>m_fDPI;
		else
		{
			short nDpi;
			ar>>nDpi;
			if( nDpi <= 0 )
				nDpi = 96;

			m_fDPI = nDpi;
		}

		short n;
		ar>>n;
		m_nObjectTransformation = (ObjectTransformation)n;
		SerializeBool( ar, m_bUseActiveView );
		SerializeBool( ar, m_bViewAutoAssigned );
		ar>>m_strViewProgID;

		DestroyObjectList( &m_ObjectList );

		DWORD dwObjectCount = 0;
		ar>>dwObjectCount;
		for( int i=0;i<(int)dwObjectCount;i++ )
		{
			CObjectDefinition* pOD = new CObjectDefinition;
			pOD->Serialize( ar );
			m_ObjectList.AddTail( pOD );
		}


		if( nVersion >= 2 )
		{
			SerializeBool( ar, m_bUseObjectDPI );
			//ar << m_fObjectDPI;
		}

		if( nVersion >= 4 && punkStream )
		{
			DWORD dwLen = 0;

			ar >> dwLen;
			if( dwLen )
			{
				BYTE *lpBuf = new BYTE[dwLen];

				ar.Read( (LPVOID)lpBuf, dwLen );

				COleStreamFile sfile( punkStream );
				sfile.Write( (void*)lpBuf, dwLen );
				sfile.Seek( 0, CFile::begin );

				((INamedPropBagSer*)this)->Load( punkStream );
				delete []lpBuf;
			}
		}


		TryBuild();

	}

	if( punkStream )
		punkStream->Release();
}

/////////////////////////////////////////////////////////////////////////////
// CViewAXCtrl::OnDraw - Drawing function

#define NEW_PRINT
void CViewAXCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if( m_bInitOK )
		return;

	CRect rc_wnd = rcBounds;

//paul
#ifndef NEW_PRINT
	if( GetSafeHwnd() )
		GetWindowRect( &rc_wnd );
#endif//NEW_PRINT

	COLORREF clr = ::GetSysColor( COLOR_3DFACE );
	pdc->FillRect( rcInvalid, &CBrush( clr ) );
	

	CStringArray arTypes;
	CStringArray arUserNames;
	::GetAllObjectTypes( arTypes, arUserNames );


	CBrush	brush;
	brush.CreateHatchBrush( HS_FDIAGONAL, RGB( GetRValue(clr) - 30, GetGValue(clr) - 30, GetBValue(clr) - 30 ) );
	brush.UnrealizeObject();
#ifndef _DEBUG
	//pdc->SetBrushOrg( rcBounds.left%8, rcBounds.top%8 );
#endif //_DEBUG
	pdc->SetBkColor( clr );
	pdc->SetBkMode( TRANSPARENT );
	pdc->SelectObject( &brush );
	pdc->Rectangle(rcBounds);

	pdc->SelectStockObject( NULL_BRUSH );	

	CString strOut;

	CString strViewType;
	strViewType.LoadString( IDS_VIEWTYPE );

	CString strObject;
	strObject.LoadString( IDS_OBJECT );	
	
	
	CString strViewAutoAssigned;
	CString strUseActiveView;
	
	strViewAutoAssigned.LoadString( IDS_VIEW_AUTO_ASSIGNED );
	strUseActiveView.LoadString( IDS_USE_ACTIVE_VIEW );

	CString strActiveObject;
	strActiveObject.LoadString( IDS_ACTIVE_OBJECT );
	

	strOut = strViewType;
	if( m_bUseActiveView || m_bViewAutoAssigned )
		strOut += strViewAutoAssigned;
	else
		strOut += CString( GetViewName() );

	strOut += "\n";
	strOut += strObject;
	

	if( m_bUseActiveView )
	{
		strOut += strUseActiveView;
	}
	else
	{
		long lPos;
		_GetFirstObjectPosition( &lPos );
		while( lPos )
		{
			CString strObjectName, strObjectType;
			bool bActiveObject = false;
			_GetNextObject(strObjectName, bActiveObject, 
							strObjectType, &lPos);

			for( int i=0;i<arTypes.GetSize();i++ )
			{
				if( strObjectType == arTypes[i] )
				{
					strObjectType = arUserNames[i];
					break;
				}
			}

			CString _strObject;
			_strObject.Format( "%s, %s ", ( bActiveObject ? strActiveObject : strObjectName ), strObjectType );			
			strOut += _strObject;
			strOut += "\n";
		}
	}

	/*
	CRect rcText = CRect( 0, 0, 0, 0 );	
	rcText.DeflateRect( 2, 2, 2, 2 );

	
	strOut = "ssss";
	int nRes = pdc->DrawText( strOut, &rcText, DT_CALCRECT );
	DWORD dw = GetLastError();
	
	rcText.DeflateRect( 2, 2, -2, -2 );
	VERIFY( TRUE == pdc->DrawText( strOut, &rcText, DT_LEFT ) );
	
	
	*/

	//strOut = "1 st string";

	CFont font;
	CFont* pFont = &font;//= CFont::FromHandle( (HFONT)GetStockObject(ANSI_VAR_FONT) );
	
	{
		CFont* pTempFont = CFont::FromHandle( (HFONT)GetStockObject(ANSI_VAR_FONT) );
		LOGFONT lf;
		pTempFont->GetLogFont( &lf );
		strcpy( lf.lfFaceName, "Tahoma" );		
		lf.lfWidth = 0;
		lf.lfCharSet = DEFAULT_CHARSET;
		font.CreateFontIndirect( &lf );		
	}

	CFont* pOldFont = pdc->SelectObject( pFont );

	COLORREF clrOldText = pdc->SetBkColor( RGB( 0, 0, 0 ) );
	CRect rcClient;
	GetClientRect( &rcClient );
	
	TEXTMETRIC tm;
	ZeroMemory( &tm, sizeof(TEXTMETRIC) );
	pdc->GetTextMetrics( &tm );


	CStringArray arStr;

	int nIdx = strOut.Find( "\n" );
	int nIdxPrev = -1;
	if( nIdx == -1 )
		arStr.Add( strOut );

	while( nIdx != -1 )
	{
		nIdx = strOut.Find( "\n", nIdxPrev + 1 );
		if( nIdx != -1 )
		{
			CString strAdd = strOut.Mid( nIdxPrev + 1, nIdx - nIdxPrev - 1 );
			arStr.Add( strAdd );
		}
		else
		{
			CString strAdd = strOut.Mid( nIdxPrev + 1, strOut.GetLength() - nIdxPrev - 1 );
			arStr.Add( strAdd );
		}

		nIdxPrev = nIdx;
	}

#ifdef NEW_PRINT
	int ny = 2 + rcBounds.top;
	int nx = 2 + rcBounds.left;
#else
	int ny = 2;
	int nx = 2;
#endif

	for( int i=0;i<arStr.GetSize();i++ )
	{
		CRect rc_text;
		rc_text.left	= nx;
		rc_text.top		= ny;
		rc_text.right	= rc_text.left + rcBounds.Width() - 2;
		rc_text.bottom	= rc_text.top + tm.tmHeight;
		rc_text.bottom = min( rc_text.bottom, rcBounds.bottom );


		pdc->DrawText( arStr[i], &rc_text, DT_LEFT|DT_END_ELLIPSIS );
		ny += tm.tmHeight;
	}


	pdc->SelectObject( pOldFont );
	pdc->SetBkColor( clrOldText );

}


/////////////////////////////////////////////////////////////////////////////
// CViewAXCtrl::DoPropExchange - Persistence support

void CViewAXCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	//COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
	/*CString s;	
	float f;
	short nn;
	PX_String(pPX, "ViewProgID", s);
	PX_String(pPX, "ViewObject", s);
	PX_Float(pPX, "Zoom", f);
	PX_Short(pPX, "DPI", nn);
	PX_Short(pPX, "Mode", nn);
	PX_Short(pPX, "ObjectDefinition", nn);
	*/
}




/////////////////////////////////////////////////////////////////////////////
// CViewAXCtrl::OnResetState - Reset control to default state

void CViewAXCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}



////////////////////////////////////////////////////////////////////////////
bool CViewAXCtrl::TryBuild()
{
	if( !m_bAutoBuild )
		return false;

	Build();

	return true;
}

////////////////////////////////////////////////////////////////////////////
bool CViewAXCtrl::Build()
{	
	//TIME_TEST( "CViewAxCtrl::Build" );

	if( !GetSafeHwnd() )
		return false;

	CWnd* pWndParent = GetParent();
	if( pWndParent )
	{
		CWnd* pWndParent2 = pWndParent->GetParent();
		if( pWndParent2 )
		{		
			IUnknown* punk = pWndParent2->GetControllingUnknown();
			
			if( CheckInterface( punk, IID_IViewCtrl ) )
				return false;			
		}		
	}


	DestroyAggregates();

	m_bAutoScrollMode = true;
	

	if( m_sptrApp == NULL )
	{
		m_sptrApp = ::GetAppUnknown();
	}

	if( m_sptrApp == NULL )
		return false;

	if( m_sptrDoc == NULL )
	{
		IUnknown* punkDoc = NULL;
		m_sptrApp->GetActiveDocument( &punkDoc );
		if( punkDoc == NULL )
			return false;
		
		m_sptrDoc = punkDoc;
		punkDoc->Release();
	}

	if( m_sptrDoc == NULL )
		return false;


	///////////////////////////
	//1.Determine object list if need
	///////////////////////////

	CPtrList objList; /*!m_bAutoBuild || ( m_bAutoBuild && !m_bUseActiveView )*/ 
	if( !m_bUseActiveView )
	{
		
		POSITION pos = m_ObjectList.GetHeadPosition();
		
		while( pos )
		{
			CObjectDefinition* pOD = (CObjectDefinition*)m_ObjectList.GetNext( pos );
			
			CObjectDefinition* pODNew = new CObjectDefinition;
			pODNew->CopyFrom( pOD );
			objList.AddTail( pODNew );		
		}
		

		{//Sunc
			sptrIDocumentSite sptrDS( m_sptrDoc );
			//IUnknown* punkActiveView = NULL;
			//sptrDS->GetActiveView( &punkActiveView );
			IUnknown* punkActiveView = GetActiveView();
			if( punkActiveView )
			{
				sptrIDataContext2 sptrDC2( m_sptrDoc );
				if( sptrDC2 )
					sptrDC2->SynchronizeWithContext( punkActiveView );
				punkActiveView->Release();
			}

				
			
		}
		sptrIDataContext sptrDC( m_sptrDoc );
		if( /*m_bAutoBuild && */( sptrDC != NULL ) )
		{

			POSITION pos = objList.GetHeadPosition();
			
			while( pos )
			{
				CObjectDefinition* pOD = (CObjectDefinition*)objList.GetNext( pos );
				if( pOD->m_objectName == onAuto )
				{
					pOD->m_strObjectName.Empty();

					IUnknown* punkObj = NULL;
					sptrDC->GetActiveObject( _bstr_t( (LPCSTR)pOD->m_strObjectType ), &punkObj );
					if( punkObj == NULL )
					{
						bool buse_last = ::GetValueInt( ::GetAppUnknown(), "\\ViewAX", "UseLastActiveObject", 1 ) == 1L;

						if( buse_last )
						{
							sptrIDataContext2 sptrDC2( m_sptrDoc );	
							if( sptrDC2 )
							{
								long lPos = 0;
								sptrDC2->GetFirstObjectPos( _bstr_t( (LPCSTR)pOD->m_strObjectType), &lPos );
								if( lPos )
									sptrDC2->GetNextObject( _bstr_t( (LPCSTR)pOD->m_strObjectType), &lPos, &punkObj );
							}
						}
					}
						
					pOD->m_strObjectName = ::GetObjectName( punkObj );

					if( punkObj )
						punkObj->Release();	punkObj = 0;
				}
			}			
		}
	}
	else //Use active View
	{		
		
		sptrIDataContext sptrDC( m_sptrDoc );
		if( sptrDC == NULL )
			return false;

		{//Sunc
			sptrIDocumentSite sptrDS( m_sptrDoc );
			//IUnknown* punkActiveView = NULL;
			//sptrDS->GetActiveView( &punkActiveView );
			IUnknown* punkActiveView = GetActiveView();
			if( punkActiveView )
			{
				sptrIDataContext2 sptrDC2( m_sptrDoc );
				if( sptrDC2 )
					sptrDC2->SynchronizeWithContext( punkActiveView );

				punkActiveView->Release();
			}

				
			
		}


		CStringArray arTypes;
		CStringArray arUserNames;
		GetAllObjectTypes( arTypes, arUserNames );

		for( int i=0;i<arTypes.GetSize();i++ )
		{
			IUnknown* punkObj = NULL;
			sptrDC->GetActiveObject( _bstr_t( (LPCSTR)arTypes[i] ), &punkObj );
			if( punkObj == NULL )
				continue;

			CString strObjectName = ::GetObjectName( punkObj );
			punkObj->Release();
			
			CObjectDefinition* pODNew = new CObjectDefinition;
			pODNew->m_strObjectName = strObjectName;
			pODNew->m_strObjectType = arTypes[i];
			pODNew->m_objectName = onManual;

			objList.AddTail( pODNew );		
		}

	}




	CString strViewProgID;	
	
	///////////////////////////
	//2.Determine view progid
	///////////////////////////

	if( m_bUseActiveView )
	{
		IUnknown* punkView = GetActiveView();
		if( punkView )
		{
			IPersistPtr ptrPersist( punkView );			
			punkView->Release();	punkView = 0;

			if( ptrPersist )
			{
				CLSID clsid;
				::ZeroMemory( &clsid, sizeof(CLSID) );
				ptrPersist->GetClassID( &clsid );

				BSTR bstrProgID = 0;
				if( S_OK == ::ProgIDFromCLSID( clsid, &bstrProgID ) )
				{
					strViewProgID = bstrProgID;
					CoTaskMemFree( bstrProgID );
					bstrProgID = 0;
				}				
			}
		}

		
		
	}
	else
	{
		if( m_bViewAutoAssigned )
		{
			//***
			CArray<CString,CString> arTypes;
			POSITION pos = objList.GetHeadPosition();
			
			while( pos )
			{
				CObjectDefinition* pOD = (CObjectDefinition*)objList.GetNext( pos );			
				arTypes.Add( pOD->m_strObjectType );
			}
			
			//***
			CStringArray arAllViewProgID;
			GetAvailableViewProgID( arAllViewProgID );

			DWORD dwNeedMatch = mvPartial;			

			CString strFullSupportView;

			if( arAllViewProgID.GetSize() > 0 && arTypes.GetSize() > 0 )
			{

				for( int i=0;i<arAllViewProgID.GetSize();i++ )
				{
					CString strViewProgIDCur = arAllViewProgID[i];

					bool bAllTypeSupport	= true;
					DWORD dwMaxMatch		= mvNone;

					for( int j=0;j<arTypes.GetSize();j++ )
					{
						CString strObjectType = arTypes[j];
						DWORD dwMatchType = GetViewMatchType( strViewProgIDCur, strObjectType );
						dwMaxMatch = max( dwMatchType, dwMaxMatch );
						if( !( dwMatchType >= dwNeedMatch ) )					
						{
							bAllTypeSupport = false;					
							break;
						}

						if( dwMatchType & mvFull && strFullSupportView.IsEmpty() )
							strFullSupportView = strViewProgIDCur;
					}

					if( !( dwMaxMatch & mvFull ) )
						bAllTypeSupport = false;
						 
					if( bAllTypeSupport )
					{						
						strViewProgID = strViewProgIDCur;
						break;
					}
				}
			}

			if( strViewProgID.IsEmpty() )
				strViewProgID = strFullSupportView;


			/*
			if( arAllViewProgID.GetSize() > 0 && arTypes.GetSize() > 0 )
			{
				bool bFound = false;
				for( int j=0;j<arTypes.GetSize();j++ )
				{
					for( int i=0;i<arAllViewProgID.GetSize();i++ )
					{
						if( bFound )
							continue;

						DWORD dwMatchType = GetViewMatchType( arAllViewProgID[i], arTypes[j] );
						if( dwMatchType >= dwNeedMatch )
						{
							strViewProgID = arAllViewProgID[i];						
						}
						if( dwMatchType >= mvFull )
							bFound = true;
						
					}			
				}
			}
			*/

			arTypes.RemoveAll();
			arAllViewProgID.RemoveAll();


		}
		else
			strViewProgID = m_strViewProgID;
	}

	
	_bstr_t bstrContextProgID( szContextProgID );
	_bstr_t bstrViewProgID( strViewProgID );


	m_strViewProgID = strViewProgID;
	

	CLSID clsid;
	HRESULT hr;


	//Create context
	if( ::CLSIDFromProgID( bstrContextProgID, &clsid ) != S_OK)
	{
		DestroyObjectList( &objList );
		ASSERT( false );
		return false;
	}

	hr = CoCreateInstance(	clsid, GetControllingUnknown(), 
							CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&m_punkContext );	
	if(hr != S_OK)
	{
		m_punkContext = NULL;
		DestroyObjectList( &objList );
		ASSERT( false );
		return false;
	}


	//create view
	if( ::CLSIDFromProgID( bstrViewProgID, &clsid ) != S_OK)
	{
		DestroyObjectList( &objList );
		ASSERT( false );
		return false;
	}

	hr = CoCreateInstance(	clsid, GetControllingUnknown(), 
							CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&m_punkView );	


	VERIFY( CheckInterface( GetControllingUnknown(), IID_IPrintView) );

	if(hr != S_OK)
	{
		m_punkView = 0;		
		DestroyObjectList( &objList );
		ASSERT( false );
		return false;
	}

	{
		IViewPtr sptrV(m_punkView);
		if( sptrV )
			sptrV->Init(m_sptrDoc, 0);
	}


	//Move window
	CRect	rectWnd = NORECT;
	GetClientRect( &rectWnd );
	rectWnd.InflateRect(-1, -1, 0, 0);
	{
		IWindowPtr sptrWin(m_punkView);
		if (sptrWin != 0)
		{				
			sptrWin->SetWindowFlags(wfPlaceInDialog);	//for prevent registrate drop targer

			if (sptrWin->CreateWnd(GetSafeHwnd(), rectWnd, WS_CHILD|WS_VISIBLE, WM_USER+20) != S_OK)
			{
				DestroyObjectList( &objList );
				ASSERT( false );
				return false;
			}

		}
	}



	//Set base object flag
	{
		INamedDataPtr ptrD( m_sptrDoc );
		if( ptrD == NULL )
		{
			ASSERT( false );
			return false;
		}

		long lPos = -1;
		ptrD->GetBaseGroupFirstPos( &lPos );
		while( lPos )
		{
			IUnknown* punkObject = NULL;
			GuidKey guid = INVALID_KEY;
			ptrD->GetNextBaseGroup( &guid, &lPos );
			ptrD->GetBaseGroupBaseObject( &guid, &punkObject );
			if( !punkObject )
				continue;


			POSITION pos = objList.GetHeadPosition();
			while( pos )
			{
				CObjectDefinition* pOD = (CObjectDefinition*)objList.GetNext( pos );
				IUnknown* punkTestObj = NULL;
				punkTestObj = ::GetObjectByName( ptrD, (LPCSTR)pOD->m_strObjectName, (LPCSTR)pOD->m_strObjectType );
				if( punkTestObj )
				{
					if( ::GetObjectKey( punkTestObj ) == ::GetObjectKey( punkObject ) )
					{
						pOD->m_bBaseObject = true;
					}
					/*
					else
					{
						INamedDataObject2Ptr ptrNDO2( punkTestObj );
						if( ptrNDO2 )
						{
							IUnknown* punkParent = NULL;
							ptrNDO2->GetParent( &punkParent );
							if( punkParent )
							{
								pOD->m_bBaseObject = true;
								punkParent->Release();
							}
						}
					}
					*/

					punkTestObj->Release();
				}
			}

			punkObject->Release();
		}
		
		
		


	
		sptrIDataContext sptrContext(m_punkContext);
		if( sptrContext == NULL )
		{
			DestroyObjectList( &objList );
			ASSERT( false );
			return false;
		}
		
		if (sptrContext->AttachData(m_sptrDoc) != S_OK)		
		{
			DestroyObjectList( &objList );
			ASSERT( false );
			return false;
		}


		//Deselect all
		{
			IDataContext2Ptr ptrDC2( sptrContext );
			if( ptrDC2 )
			{
				long lCount = 0;
				VERIFY( S_OK == ptrDC2->GetObjectTypeCount( &lCount ) );
				for( long l=0;l<lCount;l++ )
				{
					BSTR bstrTypeName = 0;
					VERIFY( S_OK == ptrDC2->GetObjectTypeName( l, &bstrTypeName ) );
					VERIFY( S_OK == ptrDC2->UnselectAll( bstrTypeName ) );
					::SysFreeString( bstrTypeName );
				}
				
			}
		}



		CStringList lst_str_types;

		
		//at first activate base object
		POSITION pos = objList.GetHeadPosition();		
		while( pos )
		{
			CObjectDefinition* pOD = (CObjectDefinition*)objList.GetNext( pos );			
			if( !pOD->m_bBaseObject )
				continue;

			IUnknown* punkObject = ::FindObjectByName( m_sptrDoc, pOD->m_strObjectName );			
			if( punkObject )
			{				
				if( !CheckInterface( punkObject, IID_IActiveXForm) )
				{
					if( !lst_str_types.Find( (LPCSTR)pOD->m_strObjectType ) )
					{
						sptrContext->SetActiveObject( _bstr_t( (LPCSTR)pOD->m_strObjectType ), punkObject, 0 );
						lst_str_types.AddTail( (LPCSTR)pOD->m_strObjectType );
					}
					else
					{
						IDataContext3Ptr sptrContext3 = sptrContext;
						sptrContext3->SetObjectSelect( punkObject, 1 );
					}
				}
				punkObject->Release();
			}			
		}

		lst_str_types.RemoveAll();

		//then activate not'base object
		pos = objList.GetHeadPosition();		
		while( pos )
		{
			CObjectDefinition* pOD = (CObjectDefinition*)objList.GetNext( pos );			
			if( pOD->m_bBaseObject )
				continue;

			IUnknown* punkObject = ::FindObjectByName( m_sptrDoc, pOD->m_strObjectName );			
			if( punkObject )
			{	
				if( !CheckInterface( punkObject, IID_IActiveXForm) )
				{
					if( !lst_str_types.Find( (LPCSTR)pOD->m_strObjectType ) )
					{
						sptrContext->SetActiveObject( _bstr_t( (LPCSTR)pOD->m_strObjectType ), punkObject, 0 );
						lst_str_types.AddTail( (LPCSTR)pOD->m_strObjectType );
					}
					else
					{
						IDataContext3Ptr sptrContext3 = sptrContext;
						sptrContext3->SetObjectSelect( punkObject, 0 );
					}
				}

				punkObject->Release();
			}			
		}		

	}

/*	
	//Move window
	CRect	rectWnd = NORECT;
	GetClientRect( &rectWnd );
	rectWnd.InflateRect(-1, -1, 0, 0);
	{
		IWindowPtr sptrWin(m_punkView);
		if (sptrWin != 0)
		{				
			sptrWin->SetWindowFlags(wfPlaceInDialog);	//for prevent registrate drop targer

			if (sptrWin->CreateWnd(GetSafeHwnd(), rectWnd, WS_CHILD|WS_VISIBLE, WM_USER+20) != S_OK)
			{
				DestroyObjectList( &objList );
				ASSERT( false );
				return false;
			}

		}
	}
	*/

	m_fObjectDPI = 0.0;

	if( ::IsCalibrationForPrintEnable() )
	{
		//try to calc m_fObjectDPI
		POSITION pos = objList.GetHeadPosition();		
		while( pos )
		{
			CObjectDefinition* pOD = (CObjectDefinition*)objList.GetNext( pos );			
			//if( pOD->m_bBaseObject )
			{
				IUnknown* punkObject = ::FindObjectByName( m_sptrDoc, pOD->m_strObjectName );			
				if( punkObject )
				{
					CImageWrp image( punkObject );	
					punkObject->Release();
					if( image )
					{
						CRect rcImage = image.GetRect();
						//[AY]
						double /*fMeterPerUnit = 0, */fCalibr = 0;
						//::GetCalibrationUnit( &fMeterPerUnit, 0 );
						::GetCalibration( punkObject, &fCalibr, 0 );
						if( fCalibr <= 0.0 )
							continue;

						//Image size in unit
						double fW = ( (double)rcImage.Width() ) * fCalibr;
						//double fH = ( (double)rcImage.Height() ) * calibr.fCalibrY;

						if( fW <= 0.0 )
							continue;

						//Image size in meter
						//fW *= fMeterPerUnit;
						//fH /= fUnitPerMeter;

						//Image size in mm
						fW *= 1000.0;
						//fH *= 1000.0;

						//calc DPI
						m_fObjectDPI = ( (double)rcImage.Width() ) / fW * MeasureUnitTable::mmPerInch;
					}
				}			
			}
		}	
	}


	DestroyObjectList( &objList );

	if( m_punkView )
	{
		/*
		_variant_t varPane;
        ((INamedPropBagSer*)this)->GetProperty( _bstr_t( szPaneNum ), &varPane );
		if( varPane.vt == VT_EMPTY || varPane.vt == VT_I4 && varPane.lVal == -1 )
		{
			INamedPropBagPtr sptrBag2;

			IUnknown* punk_view = GetActiveView();			
			if( punk_view )
			{
				sptrBag2 = punk_view;
				punk_view->Release();	punk_view = 0;
			}

			if( sptrBag2 != 0 )
			{
				long lPos = 0;

				sptrBag2->GetFirstPropertyPos( &lPos );

				INamedPropBagPtr sptrBag = m_punkView;

				while( lPos != 0 )
				{
					_bstr_t varName;
					_variant_t varValue;
					sptrBag2->GetNextProperty( varName.GetAddress(), &varValue, &lPos );
					((INamedPropBagSer*)this)->SetProperty( varName, varValue );
					sptrBag->SetProperty( varName, varValue );
				}
			}
		}
		else
		{
			INamedPropBagPtr sptrBag2 = m_punkView;
			if( sptrBag2 != 0 )
			{
				_variant_t varPos( (long)GetFirstPropertyPos() );
				while( varPos.lVal )
				{
					_variant_t varName, varValue;
					varPos.lVal = GetNextProperty( &varName, &varValue, &varPos );
					sptrBag2->SetProperty( _bstr_t( varName ), varValue );
				}
			}
		}
		*/

		INamedPropBagPtr sptrBag;

		IUnknown* punk_view = GetActiveView();			
		if( punk_view )
		{
			sptrBag = punk_view;
			punk_view->Release();	punk_view = 0;
		}

		INamedPropBagPtr sptrBag2 = m_punkView;
		if( sptrBag2 != 0 )
		{
			_variant_t varPos( (long)GetFirstPropertyPos() );
			if( varPos.lVal )
			{
				while( varPos.lVal )
				{
					_variant_t varName, varValue;
					varPos.lVal = GetNextProperty( &varName, &varValue, &varPos );
					sptrBag2->SetProperty( _bstr_t( varName ), varValue );
					//sptrBag->SetProperty( _bstr_t( varName ), varValue );
				}
			}
			else if( sptrBag )
			{
				long lPos = 0;

				sptrBag->GetFirstPropertyPos( &lPos );

				while( lPos != 0 )
				{
					_bstr_t varName;
					_variant_t varValue;
					sptrBag->GetNextProperty( varName.GetAddress(), &varValue, &lPos );

					((INamedPropBagSer*)this)->SetProperty( varName, varValue );
					sptrBag2->SetProperty( varName, varValue );
				}
			}
		}

	}

	ResizeView();

	m_bInitOK = true;
	
	return true;	
}

////////////////////////////////////////////////////////////////////////////
IUnknown* CViewAXCtrl::GetActiveView()
{

	if( m_sptrDoc == 0 ) 
		return 0;


	sptrIViewSite	sptrVS;
	
	long lPos = 0;
	m_sptrDoc->GetFirstViewPosition( &lPos );
	while( lPos )
	{
		IUnknown* punk = 0;
		m_sptrDoc->GetNextView( &punk, &lPos );
		if( !punk )
			continue;

		sptrVS = punk;
		punk->Release();	punk = 0;

		if( sptrVS != 0 )//may be it ptintpreview aggregate
			break;		
	}

	if( sptrVS == 0 )
		return 0;

	
	IUnknown*	punkFrame = NULL;
	sptrVS->GetFrameWindow( &punkFrame );
	sptrIFrameWindow	sptrF( punkFrame ) ;
	if ( punkFrame )
		punkFrame->Release();

	if( sptrF == NULL )
		return 0;

	IUnknown* punkSplitter = NULL;		
	sptrF->GetSplitter( &punkSplitter );

	ISplitterWindowPtr ptrSplitter( punkSplitter );

	if( punkSplitter )
		punkSplitter->Release();

	if( ptrSplitter == 0 )
		return 0;

	
	int nRow, nCol;
	nRow = nCol = -1;
	ptrSplitter->GetActivePane( &nRow, &nCol );

	IUnknown* punkActiveView = 0;
	ptrSplitter->GetViewRowCol( nRow, nCol, &punkActiveView );


	return punkActiveView;
}


void CViewAXCtrl::OnSize(UINT nType, int cx, int cy) 
{
	COleControl::OnSize(nType, cx, cy);	
	ResizeView();
}

////////////////////////////////////////////////////////////////////////////
void CViewAXCtrl::ResizeView()
{
	if( !GetSafeHwnd() )
		return;

	if( !m_punkView )
		return;

	
	int nDPIX;


	{
		DisplayCalibration dc;
		double fPixelPerMM = dc.GetPixelPerMM();
		nDPIX = fPixelPerMM * MeasureUnitTable::mmPerInch;
	}


	//int nDPIY = dc.GetDeviceCaps(LOGPIXELSY);

	CRect rc = NORECT;
	HWND	hwnd = 0;
	{
		IWindowPtr sptrWin(m_punkView);
		if(sptrWin != 0)
			sptrWin->GetHandle((HANDLE*)&hwnd);
	}

	if(hwnd)
	{
		::GetClientRect(hwnd, &rc);
		CSize sizeV(rc.Width(), rc.Height());
		IScrollZoomSitePtr	sptrSZ(GetControllingUnknown());
		if(sptrSZ != 0)
		{
			sptrSZ->GetClientSize(&sizeV);
		}
		CRect	rcCtrl = NORECT;
		//::GetWindowRect(hwnd, &rcCtrl);
		GetWindowRect(&rcCtrl);
		//GetClientRect(&rcCtrl);

		CSize sizeCtrl = CSize( rcCtrl.Width(), rcCtrl.Height() );

		{
			HANDLE h = OpenMutex( MUTEX_ALL_ACCESS, false, "ResizeView.lock.1C6E8B14-678E-4553-A16E-9705EA42F170" );
			if(h)
			{
				CloseHandle(h);
				return;
			}
		}

		if( sizeV.cx <= 0 || sizeV.cy <= 0 || sizeCtrl.cx <= 0 || sizeCtrl.cy <= 0 )
			return;


		//CSize size = ::ThumbnailGetRatioSize( CSize( rcCtrl.Width(), rcCtrl.Height() ), sizeV );

		double fZoom = 1.0;
		
		double fViewZoom = (double)sizeV.cx / (double)sizeV.cy;
		double fCtrlZoom = (double)sizeCtrl.cx / (double)sizeCtrl.cy;

		switch(m_nObjectTransformation)
		{
		case otGrowVert:
			int nWidth;
			
			nWidth	= sizeCtrl.cx;			
			
			/*
			if( (int)( (double)nWidth / (double)fViewZoom ) > sizeCtrl.cy )
			{				
				nWidth = sizeCtrl.cy * fViewZoom;
			}
			*/
			
			fZoom = (double)nWidth/(double)sizeV.cx;
			break;
		case otGrowHorz:
			int nHeight;
			
			nHeight	= sizeCtrl.cy;			
			
			/*
			if( (int)( (double)nHeight * (double)fViewZoom ) > sizeCtrl.cx )
			{				
				nHeight = (int)( (double)sizeCtrl.cx / fViewZoom);
			}
			*/
			
			fZoom = (double)nHeight/(double)sizeV.cy;			
			break;
		case otGrowTwoSide:
			//CSize size = ::ThumbnailGetRatioSize( sizeCtrl, sizeV );			
			fZoom = min((double)sizeCtrl.cx/(double)sizeV.cx, (double)sizeCtrl.cy/(double)sizeV.cy );
			break;
		default: // otStretch:
			{
				double fCtrlDPI = m_fDPI;
				if( m_bUseObjectDPI && m_fObjectDPI > 0 )
				{
					fCtrlDPI = m_fObjectDPI;
				}

				if( fCtrlDPI <= 0 )
					return;

				fZoom = m_fVZoom*nDPIX/fCtrlDPI;
			}
			break;
		}
		
		if(sptrSZ != 0)
		{
			sptrSZ->SetZoom(fZoom);
			//paul 14.09.2001 - may be view not support zoom
			sptrSZ->GetZoom( &fZoom );
			//EOC 14.09.2001
			sptrSZ->GetClientSize(&sizeV);
		}

		/*
		if( m_nObjectTransformation == otStretch )
		{
			GetParent()->ScreenToClient(&rcCtrl);
			rcCtrl.right = rcCtrl.left + rc.Width();
			rcCtrl.bottom = rcCtrl.top + rc.Height();
			MoveWindow(&rcCtrl);
		}
		else//Center View in client
		*/
		{
			CRect rcClient;
			GetClientRect( &rcClient );
			CSize sizeView( sizeV.cx*fZoom, sizeV.cy*fZoom ) ;

			CRect rcView = NORECT;

			rcView.left = rcClient.left + (rcClient.Width()  - sizeView.cx) / 2;
			rcView.top  = rcClient.top  + (rcClient.Height() - sizeView.cy) / 2;

			rcView.right  = rcView.left + sizeView.cx;
			rcView.bottom = rcView.top  + sizeView.cy;

			// [vanek] BT2000:4214 - 22.12.2004
			::MoveWindow(hwnd, rcView.left, rcView.top, rcView.Width(), rcView.Height(), TRUE );
		}		
		
		
	}


	/*
	if( !GetSafeHwnd() )
		return;

	if( !m_punkView )
		return;

	CClientDC	dc(0);
	int nDPIX = dc.GetDeviceCaps(LOGPIXELSX);
	//int nDPIY = dc.GetDeviceCaps(LOGPIXELSY);

	CRect rc = NORECT;
	HWND	hwnd = 0;
	{
		IWindowPtr sptrWin(m_punkView);
		if(sptrWin != 0)
			sptrWin->GetHandle((HANDLE*)&hwnd);
	}

	if(hwnd)
	{
		::GetClientRect(hwnd, &rc);
		CSize sizeV(rc.Width(), rc.Height());
		IScrollZoomSitePtr	sptrSZ( GetControllingUnknown() );
		if(sptrSZ != 0)
		{
			sptrSZ->GetClientSize(&sizeV);
		}
		CRect	rcCtrl = NORECT;
		//::GetWindowRect(hwnd, &rcCtrl);
		GetWindowRect(&rcCtrl);

		CSize size = ::ThumbnailGetRatioSize( sizeV, CSize( rcCtrl.Width(), rcCtrl.Height() ) );

		double fZoom;
		switch(m_nObjectTransformation)
		{
		case otGrowHorz:
			fZoom = (double)sizeV.cy/(double)size.cy;
			break;
		case otGrowVert:
			fZoom = (double)sizeV.cx/(double)size.cx;
			break;
		case otStretch:
			fZoom = min(rcCtrl.Height()/(double)sizeV.cy, rcCtrl.Width()/(double)sizeV.cx);
			break;
		default: // otGrowTwoSide
			fZoom = m_fVZoom*nDPIX/m_nDPI;
			break;
		}
		
		if(sptrSZ != 0)
		{
			sptrSZ->SetZoom(fZoom);
			sptrSZ->GetClientSize(&sizeV);
		}

		rc.right = rc.left + sizeV.cx*fZoom;//m_fVZoom*m_nDPI/nDPIX;
		rc.bottom = rc.top + sizeV.cy*fZoom;//m_fVZoom*m_nDPI/nDPIY;
		::MoveWindow(hwnd, rc.left, rc.top, rc.Width(), rc.Height(), TRUE);
		
		
		GetParent()->ScreenToClient(&rcCtrl);
		rcCtrl.right = rcCtrl.left + rc.Width();
		rcCtrl.bottom = rcCtrl.top + rc.Height();

		if(sptrSZ != 0)
		{
			//sptrSZ->SetClientSize( CSize(100, 500) );
		}

		//MoveWindow(&rcCtrl);
	}
	*/


}

////////////////////////////////////////////////////////////////////////////
bool CViewAXCtrl::_GetFirstObjectPosition( long* plPos )
{
	*plPos = (long)m_ObjectList.GetHeadPosition( );	
	return true;
}

////////////////////////////////////////////////////////////////////////////
bool CViewAXCtrl::_GetNextObject( CString& strObjectName, bool& bActiveObject, 
						CString& strObjectType, long *plPos )
{
	POSITION pos = (POSITION)*plPos;
	CObjectDefinition* pOD = (CObjectDefinition*)m_ObjectList.GetNext( pos );
	if( pOD )
	{
		strObjectName = pOD->m_strObjectName;
		bActiveObject = ( pOD->m_objectName == onAuto ? true : false );
		strObjectType = pOD->m_strObjectType;

	}
	(*plPos) = (long)pos;
	return true;
}

////////////////////////////////////////////////////////////////////////////
bool CViewAXCtrl::_InsertAfter( long lPos,  
						CString strObjectName, bool bActiveObject, 
						CString strObjectType )
{
	POSITION pos = (POSITION)lPos;
	CObjectDefinition* pOD = new CObjectDefinition;
	pOD->m_strObjectName	= strObjectName;
	pOD->m_strObjectType	= strObjectType;
	pOD->m_objectName		= ( bActiveObject == true ? onAuto : onManual );

	if( lPos < 0 )
		m_ObjectList.AddTail( pOD );
	else
	{
		m_ObjectList.InsertAfter( pos, pOD );
	}

	return true;

}

////////////////////////////////////////////////////////////////////////////
bool CViewAXCtrl::_EditAt( long lPos,  
						CString strObjectName, bool bActiveObject, 
						CString strObjectType )
{
	POSITION pos = (POSITION)lPos;
	CObjectDefinition* pOD = (CObjectDefinition*)m_ObjectList.GetNext( pos );
	if( pOD == NULL )
		return false;

	pOD->m_strObjectName	= strObjectName;
	pOD->m_strObjectType	= strObjectType;
	pOD->m_objectName		= ( bActiveObject == true ? onAuto : onManual );

	return true;
}

////////////////////////////////////////////////////////////////////////////
bool CViewAXCtrl::_RemoveAt( long lPos )
{
	POSITION pos = (POSITION)lPos;
	POSITION posSave = pos;
	CObjectDefinition* pOD = (CObjectDefinition*)m_ObjectList.GetNext( pos );
	if( pOD == NULL )
		return false;

	m_ObjectList.RemoveAt( posSave );
	delete pOD;

	return true;
}




//Trnasparency support
////////////////////////////////////////////////////////////////////////////
BOOL CViewAXCtrl::OnEraseBkgnd(CDC* pDC) 
{	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
DWORD CViewAXCtrl::GetControlFlags()
{
	return COleControl::GetControlFlags();// | windowlessActivate;
}

////////////////////////////////////////////////////////////////////////////
BOOL CViewAXCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_TRANSPARENT;	
	return COleControl::PreCreateWindow(cs);
}

////////////////////////////////////////////////////////////////////////////
void CViewAXCtrl::OnDestroy() 
{
	DestroyAggregates();
	COleControl::OnDestroy();
}

////////////////////////////////////////////////////////////////////////////
long CViewAXCtrl::GetFirstObjectPos() 
{
	long lpos = 0;
	_GetFirstObjectPosition( &lpos );	
	return lpos;
}

////////////////////////////////////////////////////////////////////////////
long CViewAXCtrl::GetNextObject(VARIANT FAR* varObjectName, VARIANT FAR* varActiveObject, VARIANT FAR* varObjectType, VARIANT FAR* varPos) 
{	
	if( varPos->vt != VT_I4 )
		return 0;

	long lpos = varPos->lVal;
	CString strObjectName, strObjectType;
	bool bActiveObject = false;
	_GetNextObject( strObjectName, bActiveObject, strObjectType, &lpos );

	*varObjectName		= _variant_t( (LPCSTR)strObjectName ).Detach();
	*varActiveObject	= _variant_t( bActiveObject ).Detach();
	*varObjectType		= _variant_t( (LPCSTR)strObjectType).Detach();
	*varPos				= _variant_t( (long)lpos );

	return lpos;
}

////////////////////////////////////////////////////////////////////////////
BOOL CViewAXCtrl::AddObject(LPCTSTR strObjectName, BOOL bActiveObject, LPCTSTR strObjectType) 
{				  
	return ( true == _InsertAfter( 0, strObjectName, bActiveObject == TRUE, strObjectType ) );
}

////////////////////////////////////////////////////////////////////////////
BOOL CViewAXCtrl::DeleteObject(long lPos) 
{
	return _RemoveAt( lPos );
}

////////////////////////////////////////////////////////////////////////////
long CViewAXCtrl::GetFirstPropertyPos() 
{	
	long lpos = 0;
	((INamedPropBagSer*)this)->GetFirstPropertyPos( &lpos );
	return lpos;
}

////////////////////////////////////////////////////////////////////////////
long CViewAXCtrl::GetNextProperty(VARIANT FAR* varName, VARIANT FAR* varValue, VARIANT FAR* varPos) 
{
	if( varPos->vt != VT_I4 )
		return 0;
	
	long lpos = varPos->lVal;

	BSTR bstr_name = 0;
	VARIANT var_value;
	::VariantInit( &var_value );
	
	((INamedPropBagSer*)this)->GetNextProperty( &bstr_name, &var_value, &lpos );
	

	if( bstr_name )
	*varPos		= _variant_t( (long)lpos );	

	VariantCopy( varName, &_variant_t( _bstr_t(bstr_name) ) );	
	VariantCopy( varValue, &var_value );	
	
	if( bstr_name )
		::SysFreeString( bstr_name );	bstr_name = 0;

	VariantClear( &var_value );

	return lpos;
}

////////////////////////////////////////////////////////////////////////////
BOOL CViewAXCtrl::SetProperty(LPCTSTR strName, const VARIANT FAR& varValue) 
{
	return ( S_OK == ((INamedPropBagSer*)this)->SetProperty( _bstr_t(strName), varValue ) );	
}

////////////////////////////////////////////////////////////////////////////
BOOL CViewAXCtrl::DeletePropery(long lPos) 
{
	return ( S_OK == ((INamedPropBagSer*)this)->DeleteProperty( lPos ) );
}

////////////////////////////////////////////////////////////////////////////
BOOL CViewAXCtrl::Rebuild()
{
	return Build();
}

////////////////////////////////////////////////////////////////////////////
//added by akm 25_10_k5
BOOL CViewAXCtrl::GetPropertyAXStatus() 
{
	return m_PropertyAXStatus;
}

void CViewAXCtrl::SetPropertyAXStatus(BOOL val) 
{		
	m_PropertyAXStatus=val;
}

////////////////////////////////////////////////////////////////////////////
BSTR CViewAXCtrl::GetViewProgID()
{
	return m_strViewProgID.AllocSysString();
}
////////////////////////////////////////////////////////////////////////////
void CViewAXCtrl::SetViewProgID( LPCTSTR lpszNewValue )
{
	m_strViewProgID = lpszNewValue;
}

void CViewAXCtrl::OnClose(DWORD dwSaveOption)
{
	m_xViewCtrl.DestroyView();
	__super::OnClose(dwSaveOption);
}


IMPLEMENT_UNKNOWN(CViewAXCtrl, ViewAXBag);

HRESULT CViewAXCtrl::XViewAXBag::SetProperty( BSTR bstrName, VARIANT var )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewAXBag)
	return ( S_OK == ((INamedPropBagSer*)pThis)->SetProperty( bstrName, var ) );	
}

HRESULT CViewAXCtrl::XViewAXBag::GetProperty( BSTR bstrName, VARIANT* pvar )
{
	METHOD_PROLOGUE_EX(CViewAXCtrl, ViewAXBag)
	return ( S_OK == ((INamedPropBagSer*)pThis)->GetProperty( bstrName, pvar ) );
}