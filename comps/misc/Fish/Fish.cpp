// Fish.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Fish.h"
#include "MenuRegistrator.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFishApp

BEGIN_MESSAGE_MAP(CFishApp, CWinApp)
	//{{AFX_MSG_MAP(CFishApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFishApp construction

CFishApp::CFishApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFishApp object

CFishApp theApp;

#pragma comment(linker, "/EXPORT:DllCanUnloadNow=_DllCanUnloadNow@0,PRIVATE")
#pragma comment(linker, "/EXPORT:DllGetClassObject=_DllGetClassObject@12,PRIVATE")
#pragma comment(linker, "/EXPORT:DllRegisterServer=_DllRegisterServer@0,PRIVATE")
#pragma comment(linker, "/EXPORT:DllUnregisterServer=_DllUnregisterServer@0,PRIVATE")

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NeedComponentInfo(rclsid, riid))
		return GetComponentInfo(ppv);
	return AfxDllGetClassObject(rclsid, riid, ppv);
}

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllCanUnloadNow();
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleObjectFactory::UpdateRegistryAll();

	CActionRegistrator	ra;
	ra.RegisterActions();

	CCompRegistrator	rc( szPluginColorCnv ),
						rdw( szPluginDockableWindows );

	rc.RegisterComponent( "FISH.ColorConvertorFISH" );
	rdw.RegisterComponent( "FISH.FishContextViewer" );

	CMenuRegistrator rcm;
	rcm.RegisterMenu(szFishContextMenu, 0, szFishContextMenuUN);


	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CActionRegistrator	ra;
	ra.RegisterActions( false );
			  
	CCompRegistrator	rc( szPluginColorCnv ),
						rdw( szPluginDockableWindows );

	rc.UnRegisterComponent( "FISH.ColorConvertorFISH" );
	rdw.UnRegisterComponent( "FISH.FishContextViewer" );

	CMenuRegistrator rcm;
	rcm.UnRegisterMenu(szFishContextMenu, 0 );

	COleObjectFactory::UpdateRegistryAll( FALSE );
	return S_OK;
}


BOOL CFishApp::InitInstance() 
{
	VERIFY(LoadVTClass(this) == true);

	COleObjectFactory::RegisterAll();
	
	::AfxEnableControlContainer();
	return CWinApp::InitInstance();
}

int CFishColor::m_nCustomize = -1;
CFishColor::CFishColor(LPCTSTR lpstrName, int nSysIndex) : CIniValue(FISH_ROOT, lpstrName)
{
	m_nSysIndex = nSysIndex;
}

CFishColor::operator COLORREF()
{
	if (m_nCustomize == -1)
		m_nCustomize = GetValueInt(GetAppUnknown(), FISH_ROOT, "CustomizedColors", 1);
	if (m_nCustomize == 0)
		return ::GetSysColor(m_nSysIndex);
	if (!m_bInit)
	{
		m_clr = ::GetValueColor(::GetAppUnknown(), m_lpSection, m_lpEntry, ::GetSysColor(m_nSysIndex));
		m_bInit = true;
	}
	return m_clr;
}

void CFishColor::OnReset()
{
	m_nCustomize = -1;
	CIniValue::OnReset();
}

CFishColor g_BackgroundColor(_T("BackgroundColor"), COLOR_3DFACE);
CFishColor g_ActiveColor(_T("ActiveColor"), COLOR_3DLIGHT);
CFishColor g_3DLightColor(_T("3DLightColor"), COLOR_3DLIGHT);
CFishColor g_3DHilightColor(_T("3DHilightColor"), COLOR_3DHILIGHT);
CFishColor g_3DDKShadowColor(_T("3DDKShadowColor"), COLOR_3DDKSHADOW);
CFishColor g_PaneText(_T("PaneText"), COLOR_BTNTEXT);

/*CIniValue *CIniValue::s_pFirst = NULL;

CIniValue::CIniValue(LPCTSTR lpSection, LPCTSTR lpEntry)
{
	m_lpSection = lpSection;
	m_lpEntry = lpEntry;
	m_bInit = false;
	m_pNext = s_pFirst;
	s_pFirst = this;
}

void CIniValue::OnReset()
{
	m_bInit = false;
}

void CIniValue::Reset()
{
	for (CIniValue *p = s_pFirst; p; p = p->m_pNext)
		p->OnReset();
}


CIntIniValue::CIntIniValue(LPCTSTR lpSection, LPCTSTR lpEntry, int nDefault) : CIniValue(lpSection,lpEntry)
{
	m_nValue = nDefault;
}

CIntIniValue::operator int()
{
	if (!m_bInit)
	{
		m_nValue = ::GetValueInt(::GetAppUnknown(), m_lpSection, m_lpEntry, m_nValue);
		m_bInit = true;
	}
	return m_nValue;
}

CColorIniValue::CColorIniValue(LPCTSTR lpSection, LPCTSTR lpEntry, COLORREF clrDefault) : CIniValue(lpSection,lpEntry)
{
	m_clr = clrDefault;
}

CColorIniValue::operator COLORREF()
{
	if (!m_bInit)
	{
		m_clr = ::GetValueColor(::GetAppUnknown(), m_lpSection, m_lpEntry, m_clr);
		m_bInit = true;
	}
	return m_clr;
}*/

