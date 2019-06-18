// Print.cpp : Defines the initialization routines for the DLL.
//
	//sptrIView	sptrV( m_punkTarget );
	//point = ConvertToClient( sptrV, point);

#include "stdafx.h"
#include "Print.h"
#include "ReportConstructor.h"
#include "Utils.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
#pragma comment( lib, "EditorView_d.lib" )
#else
#pragma comment( lib, "EditorView.lib" )
#endif//

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CPrintApp

BEGIN_MESSAGE_MAP(CPrintApp, CWinApp)
	//{{AFX_MSG_MAP(CPrintApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintApp construction

char g_szPrintPreviewObject[255];

CPrinterSetup g_prnSetup;

CPrintApp::CPrintApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance	
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPrintApp object

CPrintApp theApp;

CReportConstructor g_ReportConstructor;

/////////////////////////////////////////////////////////////////////////////
// CPrintApp initialization

BOOL CPrintApp::InitInstance()
{
	AfxEnableControlContainer();
	VERIFY(LoadVTClass(this) == true);
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	COleObjectFactory::RegisterAll();

	CString str;
	str.LoadString( IDS_PRINT_PREVIEW_NAME );
	
	strcpy( g_szPrintPreviewObject, (LPCSTR)str );

	g_ReportConstructor._CreateAllView();

	g_prnSetup.Read( false );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Special entry points required for inproc servers

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
	
	CString	strDocument = szDocumentScript;
	CCompRegistrator	rv( strDocument+"\\"+szPluginView );
	rv.RegisterComponent( "Print.PrintView" );

	CDataRegistrator	rd;
	rd.RegisterDataObject();

	return S_OK;
}


// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CActionRegistrator	ra;
	ra.RegisterActions( false );

	
	CString	strDocument = szDocumentScript;
	CCompRegistrator	rv( strDocument+"\\"+szPluginView );
	rv.UnRegisterComponent( "Print.PrintView" );

	CDataRegistrator	rd;
	rd.RegisterDataObject( false );

	

	COleObjectFactory::UpdateRegistryAll( false );
	return S_OK;
}


BOOL WriteDirectory(CString dd);

CString GetReportDir()
{
	CString	strPathDef = ::MakeAppPathName( "Templates" )+"\\";
	CString strPath = ::GetValueString( GetAppUnknown(), "Paths", "TemplatesPath", 
						(const char*)strPathDef );
	
	if( strPath.GetLength() > 1 )
	{
		CString strDir = strPath.Left( strPath.GetLength() - 1 );
		WriteDirectory( strDir );
	}

	return strPath;
}

CString GetReportDatDir()
{
	CString	strPathDef = ::MakeAppPathName( "Templates" )+"\\";	
	WriteDirectory( strPathDef );

	return strPathDef;
}

	


BOOL WriteDirectory(CString dd)
{


	HANDLE		fFile;					// File Handle
	WIN32_FIND_DATA fileinfo;			// File Information Structure
	CStringArray	m_arr;				// CString Array to hold Directory Structures
	BOOL tt;							// BOOL used to test if Create Directory was successful
	int x1 = 0;							// Counter
	CString tem = "";					// Temporary CString Object

	// Before we go to a lot of work.  
	// Does the file exist

	fFile = FindFirstFile(dd,&fileinfo);

	// if the file exists and it is a directory
	if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		//  Directory Exists close file and return
		FindClose(fFile);
		return TRUE;
	}

	m_arr.RemoveAll();					// Not really necessary - Just habit
	for(x1=0;x1<dd.GetLength();x1++)	// Parse the supplied CString Directory String
	{									
		if(dd.GetAt(x1) != '\\')		// if the Charachter is not a \ 
			tem += dd.GetAt(x1);		// else add character to Temp String
		else
		{
			m_arr.Add(tem);				// if the Character is a \ Add the Temp String to the CString Array
			tem += "\\";				// Now add the \ to the temp string
		}
		if(x1 == dd.GetLength()-1)		// If we reached the end of the file add the remaining string
			m_arr.Add(tem);
	}


	// Close the file
	FindClose(fFile);
	
	// Now lets cycle through the String Array and create each directory in turn
	for(x1 = 1;x1<m_arr.GetSize();x1++)
	{
		tem = m_arr.GetAt(x1);
		tt = CreateDirectory(tem,NULL);

		// If the Directory exists it will return a false
		if(tt)
			SetFileAttributes(tem,FILE_ATTRIBUTE_NORMAL);
		// If we were successful we set the attributes to normal
	}
	m_arr.RemoveAll();
	//  Now lets see if the directory was successfully created
	fFile = FindFirstFile(dd,&fileinfo);

	// if the file exists and it is a directory
	if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		//  Directory Exists close file and return
		FindClose(fFile);
		return TRUE;
	}
	else
	{
		FindClose(fFile);
		return FALSE;
	}
}



bool UsePresetTransformation()
{
	long l = ::GetValueInt( ::GetAppUnknown(), PRINT_PREVIEW, USE_PRESET_TRANSORMATION, 0L );
	return ( l == 1L ? true : false );
}

long GetPresetTransformation()
{
	return ::GetValueInt( ::GetAppUnknown(), PRINT_PREVIEW, PRESET_TRANSORMATION, 0L );
}

bool UsePresetZoom()
{
	long l = ::GetValueInt( ::GetAppUnknown(), PRINT_PREVIEW, USE_PRESET_ZOOM, 0L );
	return ( l == 1L ? true : false );
}

double GetPresetZoom()
{
	return ::GetValueDouble( ::GetAppUnknown(), PRINT_PREVIEW, PRESET_ZOOM, 1 );
}

bool UsePresetMargins()
{
	long l = ::GetValueInt( ::GetAppUnknown(), PRINT_PREVIEW, USE_PRESET_MARGINS, 0L );
	return ( l == 1L ? true : false );
}

CRect GetGlobalPaperMargins( CString strSection )
{
	long lLeft	= ::GetValueInt( ::GetAppUnknown(), strSection, "Left", 20 );
	long lTop	= ::GetValueInt( ::GetAppUnknown(), strSection, "Top", 10 );
	long lRight	= ::GetValueInt( ::GetAppUnknown(), strSection, "Right", 10 );
	long lBottom= ::GetValueInt( ::GetAppUnknown(), strSection, "Bottom", 10 );	

	return CRect( D2V(lLeft), D2V(lTop), D2V(lRight), D2V(lBottom) );
}

void SetGlobalPaperMargins( CString strSection, CRect rc )
{
	::SetValue( ::GetAppUnknown(), strSection, "Left", V2D(rc.left) );
	::SetValue( ::GetAppUnknown(), strSection, "Top", V2D(rc.top) );
	::SetValue( ::GetAppUnknown(), strSection, "Right", V2D(rc.right) );
	::SetValue( ::GetAppUnknown(), strSection, "Bottom", V2D(rc.bottom) );
}
