// ViewLoaderCtrl.cpp : Implementation of the CViewLoaderCtrl ActiveX Control class.
//$(SolutionDir)\comps\misc\Print\Debug\print.lib
#include "stdafx.h"
#include <afxocc.h>
#include "PrintFieldAX.h"
#include "ViewLoaderCtrl.h"
#include "ViewLoaderPropPage.h"
//#include "..\\..\\comps\\misc\\print\\ReportConstructor.h"
//#include "..\\..\\comps\\misc\\print\\printview.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//extern CReportConstructor g_ReportConstructor;
//BOOL(*CViewLoaderCtrl::procAutoRep)(int) = 0;
IMPLEMENT_DYNCREATE(CViewLoaderCtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CViewLoaderCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CViewLoaderCtrl, COleControl)
	INTERFACE_PART(CViewLoaderCtrl, IID_IVTPrintCtrl, PrintCtrl)
	INTERFACE_PART(CViewLoaderCtrl, IID_IVtActiveXCtrl, ActiveXCtrl)
END_INTERFACE_MAP()


IMPLEMENT_UNKNOWN(CViewLoaderCtrl, PrintCtrl)
IMPLEMENT_AX_DATA_SITE(CViewLoaderCtrl)
// Dispatch map

BEGIN_DISPATCH_MAP(CViewLoaderCtrl, COleControl)
	DISP_FUNCTION(CViewLoaderCtrl, "GetPropertyAXStatus", GetPropertyAXStatus, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CViewLoaderCtrl, "SetPropertyAXStatus", SetPropertyAXStatus, VT_EMPTY, VTS_BOOL)
	DISP_PROPERTY_EX_ID(CViewLoaderCtrl, "TemplateAlgorithm", dispidTemplateAlgorithm, GetTemplateAlgorithm, SetTemplateAlgorithm, VT_I2)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CViewLoaderCtrl, COleControl)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CViewLoaderCtrl, 1)
	PROPPAGEID(CViewLoaderPropPage::guid)
END_PROPPAGEIDS(CViewLoaderCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CViewLoaderCtrl, "VIEWLOADER.ViewLoaderCtrl.1",
	0x67df5557, 0x7009, 0x432c, 0x8c, 0x3c, 0x1e, 0x56, 0x4c, 0x7, 0x48, 0x11)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CViewLoaderCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID BASED_CODE IID_DViewLoader =
		{ 0x8305E68, 0xA9B4, 0x4069, { 0x96, 0xD4, 0xEC, 0xBA, 0xF5, 0x93, 0xCE, 0x5C } };
const IID BASED_CODE IID_DViewLoaderEvents =
		{ 0x8360C193, 0x90B2, 0x4972, { 0xA2, 0xA0, 0xC5, 0x1C, 0x20, 0xDD, 0xCB, 0xE6 } };



// Control type information

static const DWORD BASED_CODE _dwViewLoaderOleMisc =
	OLEMISC_INVISIBLEATRUNTIME |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE /*|
	OLEMISC_RECOMPOSEONRESIZE*/ ;

IMPLEMENT_OLECTLTYPE(CViewLoaderCtrl, IDS_VIEWLOADER, _dwViewLoaderOleMisc)


// CViewLoaderCtrl::CViewLoaderCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CViewLoaderCtrl

BOOL CViewLoaderCtrl::CViewLoaderCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegInsertable | afxRegApartmentThreading to afxRegInsertable.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_VIEWLOADER,
			IDB_VIEWLOADER,
			afxRegInsertable | afxRegApartmentThreading,
			_dwViewLoaderOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CViewLoaderCtrl::CViewLoaderCtrl - Constructor

CViewLoaderCtrl::CViewLoaderCtrl()
{
	InitializeIIDs(&IID_DViewLoader, &IID_DViewLoaderEvents);
	/*if(m_bViewLoaded) this->m_pCtrlSite->DestroyControl();*/
	m_PropertyAXStatus = FALSE;
	
	
	//oldPrompt = ::GetValue(GetAppUnknown(),"General","PromtCloseModified",_variant_t((short)0));
	::SetValue(GetAppUnknown(),"ViewLoader","SaveProhibits",_variant_t((short)1));
	/*long d = ::GetValue(GetAppUnknown(),"General","PromtCloseModified",_variant_t((short)0));*/
	procAutoRep = 0;
	hLib = LoadLibrary("comps\\print.dll");
	if(hLib) procAutoRep = (BOOL(*)(int))GetProcAddress(hLib, "GenerateAutoReport");

}



// CViewLoaderCtrl::~CViewLoaderCtrl - Destructor

CViewLoaderCtrl::~CViewLoaderCtrl()
{
	if(hLib) FreeLibrary(hLib);
	hLib =0;
	::SetValue(GetAppUnknown(),"ViewLoader","SaveProhibits",_variant_t((short)0));

}



// CViewLoaderCtrl::OnDraw - Drawing function

void CViewLoaderCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{

 }



// CViewLoaderCtrl::DoPropExchange - Persistence support

void CViewLoaderCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}



// CViewLoaderCtrl::GetControlFlags -
// Flags to customize MFC's implementation of ActiveX controls.
//
// For information on using these flags, please see MFC technical note
// #nnn, "Optimizing an ActiveX Control".
DWORD CViewLoaderCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();

	return dwFlags;
}



// CViewLoaderCtrl::OnResetState - Reset control to default state

void CViewLoaderCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

}



// CViewLoaderCtrl message handlers



/////////IVtActiveXCtrl
IMPLEMENT_UNKNOWN(CViewLoaderCtrl, ActiveXCtrl);

HRESULT CViewLoaderCtrl::XActiveXCtrl::SetSite( IUnknown *punkVtApp, IUnknown *punkSite )
{
	METHOD_PROLOGUE_EX(CViewLoaderCtrl, ActiveXCtrl)
	pThis->m_ptrApp = punkVtApp;
	pThis->m_ptrSite = punkSite;
	return S_OK;
}
HRESULT CViewLoaderCtrl::XActiveXCtrl::GetName( BSTR *pbstrName)
{
	/*METHOD_PROLOGUE_EX(CViewLoaderCtrl, ActiveXCtrl)*/
	*pbstrName = CString( "View Loader" ).AllocSysString();
	return S_OK;
}
BOOL CViewLoaderCtrl::GetPropertyAXStatus() 
{
	return m_PropertyAXStatus;
}

void CViewLoaderCtrl::SetPropertyAXStatus(BOOL val) 
{		
	m_PropertyAXStatus=val;
}



SHORT CViewLoaderCtrl::GetTemplateAlgorithm(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return m_TemplateAlgorithm;
}

void CViewLoaderCtrl::SetTemplateAlgorithm(SHORT newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_TemplateAlgorithm = newVal;
	SetModifiedFlag();
}

void CViewLoaderCtrl::Serialize(CArchive &ar)
{

	COleControl::Serialize(ar);
	
	long nVersion = 1;
	if(ar.IsStoring())
	{
		ar<<nVersion;
		ar<<m_TemplateAlgorithm;
	}
	else
	{
		
		ar>>nVersion;
		ar>>m_TemplateAlgorithm;
		OnSerialize();

	}

}
HRESULT CViewLoaderCtrl::XPrintCtrl::Draw( HDC hDC, SIZE sizeVTPixel )
{
	return S_OK;
}

HRESULT CViewLoaderCtrl::XPrintCtrl::FlipHorizontal(  ){return S_OK;}
HRESULT CViewLoaderCtrl::XPrintCtrl::FlipVertical(  ){return S_OK;}
HRESULT CViewLoaderCtrl::XPrintCtrl::SetPageInfo( int nCurPage, int nPageCount ){return S_OK;}
HRESULT CViewLoaderCtrl::XPrintCtrl::NeedUpdate( BOOL* pbUpdate ){*pbUpdate=FALSE; return S_OK;}

void CViewLoaderCtrl::OnSerialize()
{
	if(m_TemplateAlgorithm<0 || m_TemplateAlgorithm>5) return;
	if(procAutoRep)	procAutoRep(m_TemplateAlgorithm);
	else AfxMessageBox("print.dll not found", MB_ICONERROR | MB_OK, 0);
}


