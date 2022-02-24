// AppDisp.cpp : implementation file
//
#include "stdafx.h"
#include "shell.h"
#include "docs.h"
#include "ActiveScript.h"
#include "\vt5\common2\misc_calibr.h"
#include "\vt5\com_base\perf_counters.h"
#include "AppDisp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

inline CString _change_chars( CString str, CString strA, CString strB )
{
	CString strRet;

	long lPrev = 0;
	long lPos = str.Find( strA, lPrev );

	while( lPos != -1 )
	{
		strRet += str.Mid( lPrev, lPos - lPrev ); 

		if( !strB.IsEmpty() )
			strRet +=  strB;
		else
			strRet +=  '\0';

		lPos += strA.GetLength();
		lPrev = lPos;

		lPos = str.Find( strA, lPrev );
	}

	if( lPos == -1 )
		strRet += str.Right( str.GetLength() - lPrev );

	return strRet;
}

/////////////////////////////////////////////////////////////////////////////
// CAppDisp
#ifdef _AFXDLL
#else
#error NO _AFXDLL
#endif

IMPLEMENT_DYNCREATE(CAppDisp, CCmdTarget)
CAppDisp::CAppDisp()
{
	EnableAutomation();

	SetName( _T("Application") );	
	_OleLockApp( this );
}

CAppDisp::~CAppDisp()
{
	_OleUnlockApp( this );
}


void CAppDisp::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CAppDisp, CCmdTarget)
	//{{AFX_MSG_MAP(CAppDisp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAppDisp, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CAppDisp)
	DISP_FUNCTION(CAppDisp, "ProcessMessage", ProcessMessage, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CAppDisp, "DoOpenFileDialog", DoOpenFileDialog, VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CAppDisp, "DoSaveFileDialog", DoSaveFileDialog, VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CAppDisp, "GetGlobalScript", GetGlobalScript, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION(CAppDisp, "debug_break", debug_break, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CAppDisp, "WritePrivateProfileString", DoWritePrivateProfileString, VT_BOOL, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CAppDisp, "GetPrivateProfileString", DoGetPrivateProfileString, VT_BSTR, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CAppDisp, "VtDbl", VtDbl, VT_R8, VTS_VARIANT)
	DISP_FUNCTION(CAppDisp, "VtStr", VtStr, VT_BSTR, VTS_VARIANT VTS_BSTR )	
	DISP_FUNCTION(CAppDisp, "DoSelectFolderDialog", DoSelectFolderDialog, VT_BSTR, VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CAppDisp, "GetDisplayPixelPerM", GetDisplayPixelPerM, VT_R8, VTS_NONE)
	DISP_FUNCTION(CAppDisp, "OpenHelp", OpenHelp, VT_EMPTY, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CAppDisp, "GetVMSize", GetVMSize, VT_I4, VTS_NONE)
	DISP_FUNCTION(CAppDisp, "GetGDICount", GetGDICount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CAppDisp, "ShellExecute", ShellExecute, VT_EMPTY, VTS_BSTR )
	DISP_FUNCTION(CAppDisp, "ReloadScripts", ReloadScripts, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CAppDisp, "GetMeasureParamCoef", GetMeasureParamCoef, VT_R8, VTS_I4 VTS_VARIANT)
	DISP_FUNCTION(CAppDisp, "GetMeasureParamUnit", GetMeasureParamUnit, VT_BSTR, VTS_I4 VTS_VARIANT)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IAppDisp to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {BBC55473-AE6D-4864-AAF2-26969B0F00CB}
static const IID IID_IAppDisp =
{ 0xbbc55473, 0xae6d, 0x4864, { 0xaa, 0xf2, 0x26, 0x96, 0x9b, 0xf, 0x0, 0xcb } };

BEGIN_INTERFACE_MAP(CAppDisp, CCmdTarget)
	INTERFACE_PART(CAppDisp, IID_IAppDisp, Dispatch)
	INTERFACE_PART(CAppDisp, IID_INamedObject2, Name)
END_INTERFACE_MAP()


// {DBC8FBA9-DFFD-4f86-90B4-664A6C709E76}
GUARD_IMPLEMENT_OLECREATE(CAppDisp, "Shell.App", 
0xdbc8fba9, 0xdffd, 0x4f86, 0x90, 0xb4, 0x66, 0x4a, 0x6c, 0x70, 0x9e, 0x76);

/////////////////////////////////////////////////////////////////////////////
// CAppDisp message handlers

BOOL CAppDisp::ProcessMessage(long lFlags) 
{
	CWinApp* papp = ::AfxGetApp();
	if( !papp )	return false;

	CWnd* pwnd = papp->GetMainWnd();
	if( !pwnd )	return false;

	MSG		msg;
	while( PeekMessage( &msg, pwnd->GetSafeHwnd(), 0, 0, PM_REMOVE ) )
	{
		if( msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE )
			return false;

		::TranslateMessage( &msg );
		::DispatchMessage( &msg );
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BSTR CAppDisp::DoOpenFileDialog( LPCTSTR bstrFileName, LPCTSTR bstrTemplate )
{
	ASSERT_KINDOF(CShellDocManager, theApp.m_pDocManager);
	CShellDocManager	*pman = theApp.GetDocManager();

	CString	strPathName = bstrFileName;
	CString	strDocTemplate = bstrTemplate;

	CShellDocTemplate	*pdocTempl = 0;
	
	bool bCustomFilter = strDocTemplate.Find( '|' ) != -1;
	
	if( bCustomFilter )
		strDocTemplate = _change_chars( strDocTemplate, "|", "\0" );

	if( !bCustomFilter && !strDocTemplate.IsEmpty() )
		pdocTempl = pman->GetDocTemplate( strDocTemplate );

	if( !bCustomFilter && !pdocTempl && !strDocTemplate.IsEmpty() )
	{
		AfxMessageBox( IDS_BAD_DOCUMENT_TEMPLATE );
		return 0;
	}

	if( !pman->DoPromptFileName( strPathName, IDS_FILE_OPEN_DLG_TITLE, 0, TRUE, pdocTempl, bCustomFilter ? strDocTemplate : "" ) )
		return 0;

	return _bstr_t( strPathName ).copy();
}

/////////////////////////////////////////////////////////////////////////////
BSTR CAppDisp::DoSaveFileDialog( LPCTSTR bstrFilePath, LPCTSTR bstrTemplate )
{
	CString	strDocTemplate = bstrTemplate;
	CString strFileName = bstrFilePath;

	ASSERT_KINDOF(CShellDocManager, theApp.m_pDocManager);
	CShellDocManager	*pman = theApp.GetDocManager();

	CShellDocTemplate	*pdocTempl = 0;
	
	bool bCustomFilter = strDocTemplate.Find( '|' ) != -1;

	if( bCustomFilter )
		strDocTemplate = _change_chars( strDocTemplate, "|", "\0" );

	if( !bCustomFilter && !strDocTemplate.IsEmpty() )
		pdocTempl = pman->GetDocTemplate( strDocTemplate );

	if( !bCustomFilter && pdocTempl && !strDocTemplate.IsEmpty() )
	{
		AfxMessageBox( IDS_BAD_DOCUMENT_TEMPLATE );
		return false;
	}
	
	if( !pman->DoPromptFileName( strFileName, IDS_FILE_SAVE_DLG_TITLE, 0x80000000, FALSE, pdocTempl, bCustomFilter ? strDocTemplate : "" ) )
		return 0;

	if( strFileName.IsEmpty() )
		return 0;

	return _bstr_t( strFileName ).copy();
}

/////////////////////////////////////////////////////////////////////////////
LPDISPATCH CAppDisp::GetGlobalScript(LPCTSTR psz_name) 
{
	IActiveScript* pscript = g_script.GetActiveScript();
	if( !pscript )
		return 0;

	IDispatch* pdisp = 0;
	pscript->GetScriptDispatch( 0, &pdisp );
	pscript->Release();	pscript = 0;
	
	return pdisp;
}

/////////////////////////////////////////////////////////////////////////////
double CAppDisp::GetDisplayPixelPerM() 
{
	DisplayCalibration dc;
	return dc.GetPixelPerMM()*1000;
}

/////////////////////////////////////////////////////////////////////////////
void CAppDisp::debug_break()
{
	IDebugHostPtr ptr_debug( g_script.GetControllingUnknown() );
	if( ptr_debug == 0 )	return;

	ptr_debug->ShowDebugger( -1 );
	ptr_debug->BreakIntoDebuger( );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CAppDisp::DoWritePrivateProfileString( LPCTSTR bstrFile, LPCTSTR bstrSection, LPCTSTR bstrKey, LPCTSTR bstrValue )
{
	CString	strFile = bstrFile;
	CString	strSection = bstrSection;
	CString	strKey = bstrKey;
	CString	strValue = bstrValue;

	return ::WritePrivateProfileString(strSection, strKey, strValue, strFile);
}

/////////////////////////////////////////////////////////////////////////////
BSTR CAppDisp::DoGetPrivateProfileString( LPCTSTR bstrFile, LPCTSTR bstrSection, LPCTSTR bstrKey, LPCTSTR bstrValue)
{
	CString	strFile = bstrFile;
	CString	strSection = bstrSection;
	CString	strKey = bstrKey;
	CString	strValue = bstrValue;

	CString s;
	::GetPrivateProfileString(strSection, strKey, strValue, s.GetBuffer(1000), 1000, strFile);
	s.ReleaseBuffer();

	return _bstr_t( s ).copy();
}

/////////////////////////////////////////////////////////////////////////////
double CAppDisp::VtDbl( const VARIANT FAR& Value )
{
//	try
	{
		_variant_t var_t = Value;
		var_t.ChangeType( VT_BSTR );

//		_bstr_t bstr = var_t.bstrVal;
//		_char sz_buf[100];
//		strcpy( sz_buf, (char*)bstr );
		wchar_t* psz = wcschr( var_t.bstrVal, L',' );
		if( psz )
			*psz = L'.';
		HRESULT hr=VariantChangeTypeEx(&var_t,&var_t,LOCALE_INVARIANT,0,VT_R8);
		if(FAILED(hr))
			throw _com_error(hr);
		return var_t;
	}
	//catch(...)
	//{
	//	return 0.;
	//}
}

/////////////////////////////////////////////////////////////////////////////
BSTR CAppDisp::VtStr( const VARIANT FAR& Value, LPCTSTR strFormat )
{
	CString str;

	VARIANT val = Value;

	if( val.vt & VT_BYREF )
		val = *Value.pvarVal;


	if( val.vt == VT_I4 )
	{
		CString strF = strFormat;
		if( strF.IsEmpty()  )
			strF = "%d";

		str.Format( strF, val.llVal );
	}
	else if( val.vt == VT_I2 )
	{
		CString strF = strFormat;
		if( strF.IsEmpty()  )
			strF = "%d";

		str.Format( strF, val.iVal );
	}
	else if( val.vt == VT_R8 )
	{
		CString strF = strFormat;
		if( strF.IsEmpty()  )
			strF = "%f";

		str.Format( strFormat, val.dblVal );
	}

	return str.AllocSysString();
}

/////////////////////////////////////////////////////////////////////////////
BSTR CAppDisp::DoSelectFolderDialog( LPCTSTR strTitle, LPCTSTR strRootFolder, LPCTSTR strCurrFolder )
{
	CWnd *pmainwnd = 0;
	pmainwnd = AfxGetMainWnd( );
	CString stSelectFolder = strCurrFolder;
	if( !GetFolder( strTitle, stSelectFolder.GetBuffer( MAX_PATH ), strRootFolder, pmainwnd ? pmainwnd->m_hWnd : 0 ) )
		stSelectFolder.Empty( );
	stSelectFolder.ReleaseBuffer( );
	
	return stSelectFolder.AllocSysString( );
}

/////////////////////////////////////////////////////////////////////////////
void CAppDisp::OpenHelp( LPCTSTR strCHMFileName, LPCTSTR strHTML )
{
	::HelpDisplayTopic( strCHMFileName, 0, strHTML, "$GLOBAL_main");
	return;
}


//////////////////////////////////////////// for select folder 
static int CALLBACK	BrowseCallbackProc (HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    TCHAR szPath[_MAX_PATH];
    switch (uMsg) {
    case BFFM_INITIALIZED:
		{
			// [vanek] BT2000: 3654	- 09.03.2004
			CString str_text( _T("") );
			str_text.LoadString( IDS_SELECT_FOLDER_DLG_TITLE );
			::SetWindowText( hWnd, str_text );
			if (lpData)
				SendMessage(hWnd,BFFM_SETSELECTION,TRUE,lpData);
		}
        break;
    case BFFM_SELCHANGED:
        SHGetPathFromIDList(LPITEMIDLIST(lParam),szPath);
        SendMessage(hWnd, BFFM_SETSTATUSTEXT, NULL, LPARAM(szPath));
        break;
    }
    return 0;
}

BOOL GetFolder (LPCTSTR szTitle, LPTSTR szPath, LPCTSTR szRoot, HWND hWndOwner)
{
    if (szPath == NULL)
        return false;

    bool result = false;

    LPMALLOC pMalloc;
    if (::SHGetMalloc(&pMalloc) == NOERROR) {
		BROWSEINFO bi = {0};
        
        bi.ulFlags   = BIF_RETURNONLYFSDIRS;//| BIF_STATUSTEXT;

        // дескриптор окна-владельца диалога
        bi.hwndOwner = hWndOwner;

        // добавление заголовка к диалогу
        bi.lpszTitle = szTitle;

        // отображение текущего каталога
        bi.lpfn      = BrowseCallbackProc;
        
        // установка каталога по умолчанию
        bi.lParam    = LPARAM(szPath);

        // установка корневого каталога
        if (szRoot != NULL) {
            IShellFolder *pDF;
            if (SHGetDesktopFolder(&pDF) == NOERROR) {
                LPITEMIDLIST pIdl = NULL;
                ULONG        chEaten;
                ULONG        dwAttributes;

                USES_CONVERSION;
                LPOLESTR oleStr = T2OLE(szRoot);

                pDF->ParseDisplayName(NULL,NULL,oleStr,&chEaten,&pIdl,&dwAttributes);
                pDF->Release();

                bi.pidlRoot = pIdl;
            }
        }	

        LPITEMIDLIST pidl = ::SHBrowseForFolder(&bi);
        if (pidl != NULL) {
            if (::SHGetPathFromIDList(pidl,szPath))
                result = true;
            pMalloc->Free(pidl);
        }
        if (bi.pidlRoot != NULL)
            pMalloc->Free((void*)bi.pidlRoot);
        pMalloc->Release();
    }
    return result;
}

//////////////////////////////////////////////////////////////////
long CAppDisp::GetVMSize()
{
	task_info ti;
	if( !ti.open_process( "shell" ) )
		return 0;
	long lsize = ti.get_vm_size();
	return lsize;
}

/////////////////////////////////////////////////////////////////////////////
long CAppDisp::GetGDICount()
{
	typedef int (CALLBACK * GETGUIRESOURCES_PROC)(HANDLE, DWORD);
	static GETGUIRESOURCES_PROC m_pfnGetGuiResources;

	if( !m_pfnGetGuiResources )
	{
		HMODULE h = ::GetModuleHandle( "user32.dll" );
		if( h )
			m_pfnGetGuiResources = (GETGUIRESOURCES_PROC)::GetProcAddress( h, "GetGuiResources" );
	}

	if( !m_pfnGetGuiResources )	return 0;

	return m_pfnGetGuiResources( GetCurrentProcess(), GR_GDIOBJECTS);
}

/////////////////////////////////////////////////////////////////////////////
void CAppDisp::ShellExecute( LPCTSTR szCommand )
{
	::ShellExecute (NULL, NULL, szCommand, NULL, NULL, SW_SHOWNORMAL);
}

/////////////////////////////////////////////////////////////////////////////
void CAppDisp::ReloadScripts()
{
	g_script.ClearScripts( );
	g_script.LoadScripts( );
}

double CAppDisp::GetMeasureParamCoef( long lCode, const VARIANT FAR& diNamedData )
{
	if(lCode >= MANUAL_PARAMS_FIRST_KEY && lCode <= MANUAL_PARAMS_LAST_KEY
		|| MEAS_USER_PARAMS_FIRST_KEY<=lCode && lCode<MEAS_USER_PARAMS_LAST_KEY)
	{
	INamedDataPtr pND=GetAppUnknown();
		if(VT_DISPATCH==diNamedData.vt){
			INamedDataPtr pTableND=diNamedData.pdispVal;
			if(pTableND)
				pND=pTableND;
		}
		map_meas_params mapMeasParam;
		CreateNDParamDescrCache(mapMeasParam,pND);
		if(ParameterDescriptor_ex*pmp=mapMeasParam(lCode))
{
		return pmp->fCoeffToUnits;
	}				
	}else{
		if(ParameterDescriptor_ex* pmp = m_mapMeasParam(lCode))
	{
		return pmp->fCoeffToUnits;
	}				
	}				
	return 0;
}

BSTR  CAppDisp::GetMeasureParamUnit( long lCode, const VARIANT FAR& diNamedData  )
{
	if(lCode >= MANUAL_PARAMS_FIRST_KEY && lCode <= MANUAL_PARAMS_LAST_KEY
		|| MEAS_USER_PARAMS_FIRST_KEY<=lCode && lCode<MEAS_USER_PARAMS_LAST_KEY)
{
	INamedDataPtr pND=GetAppUnknown();
		if(VT_DISPATCH==diNamedData.vt){
			INamedDataPtr pTableND=diNamedData.pdispVal;
			if(pTableND)
				pND=pTableND;
		}
		map_meas_params mapMeasParam;
		CreateNDParamDescrCache(mapMeasParam,pND);
		if(ParameterDescriptor_ex* pmp = mapMeasParam(lCode))
	{
		CString str = pmp->bstrUnit;
		return str.AllocSysString();
	}				
	}else{
		if(ParameterDescriptor_ex* pmp = m_mapMeasParam(lCode))
	{
		CString str = pmp->bstrUnit;
		return str.AllocSysString();
	}				
	}				
	return 0;
}
	
void CAppDisp::UpdateUnits(void)
{
	CreateParamDescrCache(m_mapMeasParam);
}
