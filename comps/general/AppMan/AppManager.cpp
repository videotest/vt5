// AppManager.cpp : implementation file
//

#include "stdafx.h"
#include "AppMan.h"
#include "AppManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//from afxpriv.h
BOOL AFXAPI AfxComparePath(LPCTSTR lpszPath1, LPCTSTR lpszPath2);
BOOL AFXAPI AfxFullPath(LPTSTR lpszPathOut, LPCTSTR lpszFileIn);


//single instance of AppManager
CAppManager	*CAppManager::s_pAppManagerInstance = 0;
CTypedPtrList<CPtrList, CAppManager::XMethodic*>	CAppManager::s_ptrAllMethodics;
//IID of dispatch interface
// {B91F7CC6-3B46-11D3-A605-0090275995FE}
static const IID IID_IAppManager =
{ 0xb91f7cc6, 0x3b46, 0x11d3, { 0xa6, 0x5, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };

// {3A752EF1-668C-11d3-A64B-0090275995FE}
static const GUID IID_IAppManagerEvents = 
{ 0x3a752ef1, 0x668c, 0x11d3, { 0xa6, 0x4b, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };

/////////////////////////////////////////////////////////////////////////////
// CAppManager


IMPLEMENT_DYNCREATE(CAppManager, CCmdTargetEx)
IMPLEMENT_OLETYPELIB(CAppManager, IID_ITypeLibID, 1, 0)

// {EFABC941-3B63-11d3-A605-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CAppManager, "AppMan.AppManager", 
0xefabc941, 0x3b63, 0x11d3, 0xa6, 0x5, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);


CAppManager::CAppManager()
{
	m_piidEvents = &IID_IAppManagerEvents;
	m_piidPrimary = &IID_IAppManager;


	EnableAutomation();
	//EnableAggregation();
	EnableConnections();

	//initialize instance pointer
	s_pAppManagerInstance = this;

	m_nActiveApp = -1;
	m_nActiveMethodic = -1;

	m_sName = _T("App");

	Init();
}

CAppManager::~CAppManager()
{
	//de-initialize instance pointer
	s_pAppManagerInstance = 0;
}


void CAppManager::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTargetEx::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CAppManager, CCmdTargetEx)
	//{{AFX_MSG_MAP(CAppManager)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAppManager, CCmdTargetEx)
	//{{AFX_DISPATCH_MAP(CAppManager)
	DISP_PROPERTY_NOTIFY(CAppManager, "ActiveApp", m_nActiveApp, OnActiveAppChanged, VT_I4)
	DISP_PROPERTY_NOTIFY(CAppManager, "ActiveMethodic", m_nActiveMethodic, OnActiveMethodicChanged, VT_I4)
	DISP_FUNCTION(CAppManager, "GetAppsCount", GetAppsCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CAppManager, "GetAppName", GetAppName, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CAppManager, "CreateNewApplication", CreateNewApplication, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CAppManager, "RemoveApplication", RemoveApplication, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CAppManager, "GetMethodicCount", GetMethodicCount, VT_I4, VTS_I4)
	DISP_FUNCTION(CAppManager, "GetMethodicName", GetMethodicName, VT_BSTR, VTS_I4 VTS_I4)
	DISP_FUNCTION(CAppManager, "CreateMethodic", CreateMethodic, VT_EMPTY, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CAppManager, "SetCurrentMethodic", SetCurrentMethodic, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CAppManager, "SetCurrentApplication", SetCurrentApplication, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CAppManager, "RenameMethodic", RenameMethodic, VT_EMPTY, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CAppManager, "RenameApplication", RenameApplication, VT_EMPTY, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CAppManager, "MoveMethodic", MoveMethodic, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CAppManager, "ReloadActiveMethodic", ReloadActiveMethodic, VT_EMPTY, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IAppManager to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.


BEGIN_INTERFACE_MAP(CAppManager, CCmdTargetEx)
	INTERFACE_PART(CAppManager, IID_IAppManager, Dispatch)
	INTERFACE_PART(CAppManager, IID_INamedObject2, Name)
	INTERFACE_PART(CAppManager, IID_IConnectionPointContainer, ConnPtContainer)
	INTERFACE_PART(CAppManager, IID_IProvideClassInfo, ProvideClassInfo)
END_INTERFACE_MAP()

BEGIN_EVENT_MAP(CAppManager, COleEventSourceImpl)
	//{{AFX_EVENT_MAP(CAppManager)
	EVENT_CUSTOM("OnAfterMethodicChange", FireAfterMethodicChange, VTS_NONE)
	EVENT_CUSTOM("OnBeforeMethodicChange", FireBeforeMethodicChange, VTS_NONE)
	EVENT_CUSTOM("OnBeforeAppChange", FireBeforeAppChange, VTS_NONE)
	EVENT_CUSTOM("OnAfterAppChange", FireAfterAppChange, VTS_NONE)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


//initialize the AppManager object, load all applications and methodic
//method called from MethodcBar
const char *szDefPath = _T("Apps");
const char *szPathSection = _T("Paths");
const char *szAppPath = _T("AppPath");
const char *szActiveApp = _T("ActiveApp");
const char *szGeneralSection = _T("General");
const char *szMethSection = _T("Methodics");
const char *szNameEntry = _T("Name");
const char *szActiveMethodic = _T("ActiveMethodic");


CString	g_strAppPath;

void CAppManager::Init()
{
	g_strAppPath = ::MakeAppPathName( szDefPath );
	g_strAppPath+= "\\";
	g_strAppPath = ::GetValueString( GetAppUnknown(), szPathSection, szAppPath, g_strAppPath );
	m_nActiveApp = ::GetValueInt( GetAppUnknown(), szGeneralSection, szActiveApp, m_nActiveApp );

	CString	strFileFind =	::MakeFillFileName( g_strAppPath, _T("*.app") );

	CFileFind	ff;

	BOOL bFound = ff.FindFile( strFileFind );

	//improve preformance
	::EnableActionRegistartion( false );

	while( bFound )
	{
		bFound = ff.FindNextFile();

		XApplication *pa = new XApplication();
		pa->SetFileName( ff.GetFilePath() );
		pa->ReadFile();
		m_ptrApps.Add( pa );
		TRACE( "Application %s was added\n", (const char *)ff.GetFilePath() );
	}

	::EnableActionRegistartion( true );

	if( m_nActiveApp >= GetAppsCount() ) 
		m_nActiveApp = GetAppsCount()-1;

	OnActiveAppChanged();
}

void CAppManager::DeInit()
{
	::EnableActionRegistartion( false );

	for( int i = 0; i < GetAppsCount(); i++ )
		GetApp( i )->WriteFile();

	::EnableActionRegistartion( true );

	for( i = 0; i < GetAppsCount(); i++ )
		delete GetApp( i );

	m_ptrApps.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////////////////
//utility class implementation

CAppManager::XAppMethBase::XAppMethBase()
{
	m_pbi = 0;
}

CAppManager::XAppMethBase::~XAppMethBase()
{
}


void CAppManager::XAppMethBase::SetFileName( const char *szFileName )
{	
	m_strFileName = szFileName;
}

bool CAppManager::XAppMethBase::ReadFile()
{
	_try(CAppManager::XAppMethBase, ReadFile)
	{
		if( m_strFileName.IsEmpty() )
			return false;


		//default init title as file name without extension
		char	szFileName[_MAX_PATH];
		::_splitpath( m_strFileName, 0, 0, szFileName, 0 );
		m_strTitle = szFileName;

		_bstr_t	bstrFileName = m_strFileName;

		sptrIFileDataObject	sptrF;

		sptrF.CreateInstance( szNamedDataProgID );

		if( sptrF == 0 )
			return false;

		if( sptrF->LoadFile( bstrFileName ) != S_OK )
			return false;
		
		if( !LoadFromData( sptrF ) )
			return false;

		return true;
		
	}
	_catch;
	return false;
}

bool CAppManager::XAppMethBase::WriteFile()
{
	_try(CAppManager::XAppMethBase, WriteFile)
	{
		if( m_strFileName.IsEmpty() )
			return false;

		_bstr_t	bstrFileName = m_strFileName;

		sptrIFileDataObject	sptrF;

		sptrF.CreateInstance( szNamedDataProgID );

		if( sptrF == 0 )
			return false;

		sptrF->LoadFile( bstrFileName );

		if( !StoreToData( sptrF ) )
			return false;

		if( sptrF->SaveFile( bstrFileName ) != S_OK )
			return false;

		return true;
		
	}
	_catch;
	return false;
}

bool CAppManager::XAppMethBase::LoadFromData( IUnknown *punkData )
{
	CString strName = ::GetValueString( punkData, szGeneralSection, szNameEntry, m_strTitle );
	if( !strName.IsEmpty())
		m_strTitle = strName;

	return true;
}

bool CAppManager::XAppMethBase::StoreToData( IUnknown *punkData )
{	
	::SetValue( punkData, szGeneralSection, szNameEntry, m_strTitle );

	return true;
}

bool CAppManager::XAppMethBase::ExecutePropertiesDialog()
{
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
//XMethodic
CAppManager::XMethodic::XMethodic()
{
	m_lCount = 1;

	//register in global list
	ASSERT( !s_ptrAllMethodics.Find( this ) );
	s_ptrAllMethodics.AddTail( this );
}

CAppManager::XMethodic::~XMethodic()
{
	POSITION	pos = s_ptrAllMethodics.Find( this );
	ASSERT( pos );
	s_ptrAllMethodics.RemoveAt( pos );
}

/////////////////////////////////////////////////////////////////////////////////////////
//XApplication
CAppManager::XApplication::XApplication()
{
	m_nActiveMeth = -1;
}


CAppManager::XApplication::~XApplication()
{
	for( long nCount = 0; nCount < GetMethodicsCount(); nCount++ )
	{
		XMethodic	*pm = GetMethodic( nCount );
		pm->Release();
	}
}

bool CAppManager::XApplication::LoadFromData( IUnknown *punkData )
{
	if( !XAppMethBase::LoadFromData( punkData ) )
		return false;

	long nCount = ::GetEntriesCount( punkData, szMethSection );
	for( int n = 0; n < nCount; n++ )
	{
		CString	strEntryName = ::GetEntryName( punkData, szMethSection, n );
		CString	strMethName = ::GetValueString( punkData, szMethSection, strEntryName );

		AddCreateMethodic( strMethName );
	}
 
	m_nActiveMeth = ::GetValueInt( punkData, szGeneralSection, szActiveMethodic, m_nActiveMeth );

	return true;
}
bool CAppManager::XApplication::StoreToData( IUnknown *punkData )
{
	if( !XAppMethBase::StoreToData( punkData ) )
		return false;

	::DeleteEntry( punkData, CString( "\\" )+szMethSection );

	for( int n = 0; n < GetMethodicsCount(); n++ )
	{
		XMethodic	*pM = GetMethodic( n );
		::SetValue( punkData, szMethSection, pM->GetTitle(), pM->GetFileName() );

		pM->WriteFile();
	}
	::SetValue( punkData, szGeneralSection, szActiveMethodic, m_nActiveMeth );

	return true;
}

long CAppManager::XApplication::GetMethodicsCount()
{
	return m_arrMethodics.GetSize();
}

CAppManager::XMethodic *CAppManager::XApplication::GetMethodic( int nPos )
{
	if( nPos >= 0 && nPos <  m_arrMethodics.GetSize() )
		return dynamic_cast<XMethodic*>( m_arrMethodics[nPos] );
	return 0;
}

int CAppManager::XApplication::AddCreateMethodic( const char *szFileName, bool bNativePath )
{
	CString	strFullPathName = ::MakeFillFileName( g_strAppPath, szFileName );
	XMethodic	*pmFound = 0;

	if( !bNativePath )
	{
		POSITION	pos = CAppManager::s_ptrAllMethodics.GetHeadPosition();

		char	szFullPath[_MAX_PATH];
		::AfxFullPath( szFullPath, strFullPathName );
		strFullPathName = szFullPath;	

		while( pos )
		{
			XMethodic	*pMTest = CAppManager::s_ptrAllMethodics.GetNext( pos );

			if( AfxComparePath( pMTest->GetFileName(), strFullPathName ) )
				pmFound = pMTest;
		}
	}
	else
		strFullPathName = szFileName;
	
	if( !pmFound )
	{
		pmFound = new XMethodic();
		pmFound->SetFileName( strFullPathName );
		pmFound->ReadFile();

		if( bNativePath )
			pmFound->SetFileName( ::MakeFillFileName( g_strAppPath, szFileName ) );
	}
	else
	{
		for( int i = 0; i < m_arrMethodics.GetSize(); i++ )
			if( pmFound == m_arrMethodics[i] )
				return i;
		pmFound->AddRef();
	}

	return m_arrMethodics.Add( pmFound );
}

void CAppManager::XApplication::DeleteMethodic( int nPos )
{
	XMethodic	*pM = GetMethodic( nPos );

	if( !pM )
		return;

	m_arrMethodics.RemoveAt( nPos );
	pM->Release();
}

void CAppManager::XApplication::InsertMethodic( int nPos, XMethodic *pM )
{
	m_arrMethodics.InsertAt( nPos, pM );
}


/////////////////////////////////////////////////////////////////////////////
// CAppManager message handlers

long CAppManager::GetAppsCount() 
{
	return m_ptrApps.GetSize();
}

BSTR CAppManager::GetAppName(long nPos) 
{
	if( nPos < 0 || nPos >= m_ptrApps.GetSize() )
		return 0;

	CString strResult = m_ptrApps[nPos]->GetTitle();

	return strResult.AllocSysString();
}

long CAppManager::CreateNewApplication(LPCTSTR szName) 
{
	CString	strFileName = CString( szName )+".app";
	CString	strFilePath = ::MakeFillFileName( g_strAppPath, strFileName );

	XApplication *pa = new XApplication();
	pa->SetFileName( strFilePath );
	pa->SetTitle( szName );
	int nIdx = m_ptrApps.Add( pa );

	SetCurrentApplication( nIdx );

	return nIdx;
}

void CAppManager::RemoveApplication(long nPos) 
{
	// TODO: Add your dispatch handler code here

}

long CAppManager::GetMethodicCount(long nAppPos) 
{
	XApplication	*pA = GetApp( nAppPos );

	if( !pA )return 0;

	return pA->GetMethodicsCount();
}

BSTR CAppManager::GetMethodicName(long nAppPos, long nMethPos) 
{
	CString strResult;

	XApplication	*pApp = GetApp( nAppPos );

	if( !pApp )return 0;

	XMethodic	*pM = pApp->GetMethodic( nMethPos );

	if( !pM )return 0;

	strResult = pM->GetTitle();
	
	return strResult.AllocSysString();
}

void CAppManager::CreateMethodic(long nAppPos, LPCTSTR strMethName ) 
{
	SetCurrentApplication( nAppPos );

	CString	strFileName = CString( strMethName )+".config";
	CString	strFilePath = ::MakeFillFileName( g_strAppPath, strFileName );

	XApplication	*papp = GetApp( nAppPos );
	if( !papp )return;
	
	int nPos = papp->AddCreateMethodic( strFilePath );
	SetCurrentMethodic( nPos );
}

void CAppManager::OnActiveAppChanged() 
{
	FireBeforeAppChange();

	::FireEvent( GetAppUnknown(), szEventActiveAppChange, 0, 0, &m_nActiveApp, sizeof(m_nActiveApp) );

	::SetValue( GetAppUnknown(), szGeneralSection, szActiveApp, m_nActiveApp );

	m_nActiveMethodic = -1;
	XApplication	*pApp = GetActiveApp();
	if( !pApp )
		return;
	SetCurrentMethodic( pApp->GetLastActiveMethodic() );

	FireAfterAppChange();
}

void CAppManager::OnActiveMethodicChanged() 
{
	FireBeforeMethodicChange();

	XApplication	*pA = GetActiveApp();

	if( !pA )return;

	pA->SetLastActiveMethodic( m_nActiveMethodic );
	XMethodic *pM = pA->GetMethodic( m_nActiveMethodic );

	if( !pM )
		return;

	::FireEvent( GetAppUnknown(), szEventActiveMethodicChange, 0, 0, &m_nActiveMethodic, sizeof(m_nActiveMethodic) );
	
	FireAfterMethodicChange();

	::ExecuteAction( _T("FileOpen"), CString( "\"" )+pM->GetFileName()+"\"", 0 );
}

void CAppManager::SetCurrentMethodic(long nIndex) 
{
	if( nIndex == m_nActiveMethodic )
		return;
	
	TRACE("FireBeforeMethodicChange\n");
	FireBeforeMethodicChange();

	m_nActiveMethodic = nIndex;
	OnActiveMethodicChanged();

	FireAfterMethodicChange();
}

void CAppManager::SetCurrentApplication(long nIndex) 
{
	if( m_nActiveApp == nIndex )
		return;

	XApplication	*pApp = GetActiveApp();

	m_nActiveApp = nIndex;
	
	OnActiveAppChanged();
}

void CAppManager::RenameMethodic(long nIndex, LPCTSTR szNewName) 
{
	XApplication	*pApp = GetApp( nIndex );

	if( !pApp )return;

	XMethodic	*pM = pApp->GetMethodic( nIndex );

	if( !pM )return;

	pM->SetTitle( szNewName );

	::FireEvent( GetAppUnknown(), szEventRenameMethodic, 0, 0, &nIndex, sizeof(nIndex) );
}

void CAppManager::RenameApplication(long nIndex, LPCTSTR szNewName) 
{
	XApplication	*pApp = GetApp( nIndex );

	if( !pApp )return;

	pApp->SetTitle( szNewName );

	::FireEvent( GetAppUnknown(), szEventRenameApp, 0, 0, &nIndex, sizeof(nIndex) );

}

void CAppManager::MoveMethodic(long nPos, long nPosTo) 
{
	XApplication	*pApp = GetActiveApp();

	if( !pApp )
		return;

	XMethodic	*pMMove = pApp->GetMethodic( nPos );

	if( !pMMove )
		return;

	pMMove->AddRef();	//store here

	bool bActiveMoved = nPos == m_nActiveMethodic;

	pApp->DeleteMethodic( nPos );

	if( nPosTo > nPos )
		nPosTo--;

	if( m_nActiveMethodic > nPos && 
		m_nActiveMethodic <= nPosTo )
		m_nActiveMethodic--;
	else
	if( m_nActiveMethodic < nPos && 
		m_nActiveMethodic >= nPosTo )
		m_nActiveMethodic++;

	pApp->InsertMethodic( nPosTo, pMMove );	//Release not required

	OnActiveMethodicChanged();

	/*if( bActiveMoved )
	{
		m_nActiveMethodic = nPosTo;
		pApp->SetLastActiveMethodic( m_nActiveMethodic );
	}*/
}

void CAppManager::ReloadActiveMethodic() 
{
}

//return the active applicatoin
CAppManager::XApplication	*CAppManager::GetActiveApp()
{
	if( m_nActiveApp >= 0 && m_nActiveApp < GetAppsCount() )
		return GetApp( m_nActiveApp );
	else
		return 0;
}

//find methodic in appmanager by filename
void CAppManager::LocateMethodicByFileName( CString strFileName, long &nAppPos, long &nMethPos )
{
	//1. find in specified application

	if( nAppPos != -1 )
	{
		XApplication *papp = GetApp( nAppPos );

		if( !papp )
			return;

		for( nMethPos = 0; nMethPos < papp->GetMethodicsCount(); nMethPos++ )
		{
			XMethodic	*pmeth = papp->GetMethodic( nMethPos );
			if( pmeth->GetFileName() == strFileName )
				return;
		
		}
	}

	long	nStoreAppPos = nAppPos;

	for( nAppPos = 0; nAppPos < GetAppsCount(); nAppPos++ )
	{
		XApplication *papp = GetApp( nAppPos );
		for( nMethPos = 0; nMethPos < papp->GetMethodicsCount(); nMethPos++ )
		{
			XMethodic	*pmeth = papp->GetMethodic( nMethPos );
			if( pmeth->GetFileName() == strFileName )
				return;
		
		}
	}

	nAppPos = -1;
	nMethPos = -1;

	nAppPos = nStoreAppPos;

	/*if( nAppPos != -1 )
	{
		AfxMessageBox( IDS_NEWMETHADDED );

		XApplication *papp = GetApp( nAppPos );
		papp->AddCreateMethodic( strFileName, true );

		nMethPos = papp->GetMethodicsCount()-1;
		ASSERT( papp->GetMethodic( nMethPos )->GetFileName() == strFileName );

		::FireEvent( GetAppUnknown(), szEventActiveMethodicChange, 0, 0, &_variant_t( nMethPos ) );
	}*/
}