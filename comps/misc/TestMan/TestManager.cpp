#include "StdAfx.h"
#include "testmanager.h"
#include "test_defs.h"
#include "docview5.h"

#include "misc_utils.h"
#include "core5.h"
#include "object5.h"
#include "nameconsts.h"
#include "clone.h"
#include "\vt5\awin\misc_utils.h"
#include "winuser.h"


//#include <winable.h>
#include <shellapi.h>

#include "dbase.h"
#include "MacroRecorder_int.h"

#include "FileStorageHelper.h"

#include "PropBag.h"
/*
void WriteToLog( CString str )
{
	FILE *fl = fopen( "C:\\test.log", "at" );
	
 	if( fl )
		fputs( str + "\n", fl );

	fclose( fl );
}  
*/

static void FireEvent( IUnknown *punkCtrl, const char *pszEvent, IUnknown *punkFrom, IUnknown *punkData, void *pdata, long cbSize )
{
	INotifyControllerPtr	sptr( punkCtrl );

	if( sptr== 0 )
		return;

	_bstr_t	bstrEvent( pszEvent );
	sptr->FireEvent( bstrEvent, punkData, punkFrom, pdata, cbSize );
}


//return key by request interface INumberedObect. If key can't be obtained, return -1 (NOKEY)
static GuidKey GetObjectKey( IUnknown *punk )
{
	GuidKey key;

	INumeredObject * punkNum = 0;
	if (!punk || FAILED(punk->QueryInterface(IID_INumeredObject, (void**)&punkNum)) || !punkNum)
		return key;

	punkNum->GetKey( &key );
	punkNum->Release();

	return key;
}

//extract object name from Unknown
static CString GetObjectName( IUnknown *punk )
{
	INamedObject2Ptr	sptr( punk );
	if (sptr == 0)
		return "";

	BSTR	bstr = 0;
	if (FAILED(sptr->GetName(&bstr)) || !bstr)
		return "";

	CString	str(bstr);
	::SysFreeString(bstr);

	return str;
}

//find child object in namedData by name and return it
static IUnknown *FindChildByName(IUnknown *punkParent, const char *szName)
{
	if (!punkParent || !lstrlen(szName))
		return 0;

	if (::GetObjectName(punkParent) == CString(szName))
		return punkParent;

	INamedDataObject2Ptr sptrParent = punkParent;
	if (sptrParent == 0)
		return 0;

	POSITION lPos = 0;
	sptrParent->GetFirstChildPosition(&lPos);

	while (lPos)
	{
		IUnknown *punkChild = 0;
		sptrParent->GetNextChild(&lPos, &punkChild);
		if (!punkChild)
			continue;
	
		IUnknown * punk = FindChildByName(punkChild, szName);
		if (punk)
		{
			if (::GetObjectKey(punk) != ::GetObjectKey(punkChild))
				punkChild->Release();
			return punk;
		}
		// continue to next child
		punkChild->Release();
	}
	return 0;
}

//find object in namedData by name and return it
static IUnknown *FindObjectByName(IUnknown *punkData, const char *szName)
{
	sptrIDataTypeManager	sptrM(punkData);
	if (sptrM == 0 || !lstrlen(szName))
		return 0;

	long	nTypesCounter = 0;
	sptrM->GetTypesCount(&nTypesCounter);
	// for all types in documentA
	for (long nType = 0; nType < nTypesCounter; nType++)
	{
		LONG_PTR	lpos = 0;
		// for all objects in type
		sptrM->GetObjectFirstPosition(nType, &lpos);

		while (lpos )
		{
			// get next object
			IUnknown	*punkParent = 0;
			sptrM->GetNextObject(nType, &lpos, &punkParent);
			// try find object by name
			IUnknown *punk = FindChildByName(punkParent, szName);
			// if object is found
			if (punk)
			{
				// and this object is not parent object
				if (::GetObjectKey(punk) != ::GetObjectKey(punkParent))
					punkParent->Release();

				return punk;
			}
			// continue
			punkParent->Release();
		}
	}

	return 0;
}


static bool	_load_state_and_update( CString str_state )
{
	if( str_state.IsEmpty( ) )
		return false;

	CString str_curr_state( _T("") );
	// считываем  путь к текущему стейту
	str_curr_state = (LPCTSTR)::GetValueString( GetAppUnknown(), "General", "CurrentState", str_curr_state );    

	bool bresult = false;
	bresult = ::ExecuteAction( "ToolsLoadStateEx", CString( "\"" ) + str_state + "\""  );

	// восстанавливаем путь к текущему стейту
	::SetValue( GetAppUnknown(), "General", "CurrentState", str_curr_state );

	if( !bresult )
		return false;

    IApplicationPtr	sptrApp( GetAppUnknown() );
	if( sptrApp == 0 )
		return false;

    HWND hWnd = 0;
	sptrApp->GetMainWindowHandle( &hWnd );
	if( hWnd )
		::SendMessage( hWnd, WM_USER+791, 0, 0 );

	sptrApp->ProcessMessage();
	return true;
}

class _CManageManualModeMH
{
	IMacroHelperPtr m_sptrMH;
	BOOL m_bPrev;
	long m_bTIManualCorr;
public:
	_CManageManualModeMH(ITestItem *pTestItem)
	{
		IUnknownPtr punkMH(_GetOtherComponent(GetAppUnknown(),IID_IMacroHelper),FALSE);
		m_sptrMH = punkMH;
		pTestItem->GetManualCorrectionMode(&m_bTIManualCorr);
		if (m_bTIManualCorr && m_sptrMH != 0)
		{
			m_sptrMH->GetManualCorrection(&m_bPrev);
			m_sptrMH->SetManualCorrection(TRUE);
		}
	}
	~_CManageManualModeMH()
	{
		if (m_bTIManualCorr && m_sptrMH != 0)
			m_sptrMH->SetManualCorrection(m_bPrev);
	};
};



CTestManager::CTestManager(void) : IDispatchImpl< ITestManagerDisp >( "TestMan.tlb" )
{
	m_bterminate_current_only = false;
	m_strDBPath = (char*)GetValueString( GetAppUnknown(), "\\Paths", "TestDBPath", "" ) ;
	
	m_dwMinTimeDiff = ::GetValueInt( GetAppUnknown(), "\\TestMan", "MinDiffTime", 100 ) ;

	m_bStoreToHidden = 1;//::GetValueInt( GetAppUnknown(), "\\TestMan", "StoreToHiddenDoc", 1 ) != 0 ;

	
	// [vanek] : загрузка базы тестов перенесена на нотификацию AppInit - 13.07.2004
	m_bDBLoaded = false;	

	m_bInterrupt = false;
	m_lState = 0;
	m_bRunning = false;
	m_bSimpleMode = false;

	m_strLang = ::GetValueString( ::GetAppUnknown(), "\\General", "Language", "ru" );
	m_bPaused = false;

	// register 
	Register( GetAppUnknown(), true, 0 );
}

CTestManager::~CTestManager(void)
{
	// [vanek]: чистим временно сохраненные файлы, если такие остались - 14.07.2004
	m_mapTmpFiles.clear( );

	// unregister 
	Register( GetAppUnknown(), false, 0 );
}

IUnknown *CTestManager::DoGetInterface( const IID &iid )
{
	if( iid == __uuidof( ITestManager ) )
		return (ITestManager *)this;
	else if( iid == __uuidof( ITestManErrorDescr ) )
		return (ITestManErrorDescr *)this;
	else if( iid == __uuidof( ITestProcess ) )
		return (ITestProcess *)this;
	else if( iid == IID_IDispatch ) 
		return (IDispatch *)this;	
	else if( iid == IID_IEventListener ) 
		return (IEventListener *)this;	
	return ComObjectBase::DoGetInterface( iid );
}

void CTestManager::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp( pszEvent, "AppInit" ) )
	{
        // [vanek]: подгружаем базу тестов, когда все загружено, в частности дл€ того, чтобы TetsManager на момент
		// подгрузки содержимого базы был доступн по _GetOtherComponent - 13.07.2004
		TCHAR szCurDir [_MAX_PATH];
		::GetCurrentDirectory (_MAX_PATH, szCurDir);
		m_bDBLoaded = ( S_OK == _load_db( m_strDBPath, 0 ) );
		::SetCurrentDirectory ( szCurDir );
	}    
	else if( !strcmp( pszEvent, szEventTerminateTestProcess ) && punkFrom != Unknown() )
	{
		if( pdata && cbSize == sizeof( bool ) && !*((bool*)pdata) )
			m_bterminate_current_only = true;

		_stop_testing( );	// [vanek] BT2000: 3932 - 18.08.2004
}
}

// interface ITestManager
HRESULT CTestManager::GetFirstTestPos( /*[in]*/ LPOS lParentPos, /*[out, retval]*/ LPOS *plPos)
{
	if( !plPos )
		return E_FAIL;

	*plPos = (LPOS)m_treeTests.first_child_pos( lParentPos );

	return S_OK;
}

HRESULT CTestManager::GetNextTest( /*[in]*/ LPOS lParentPos, /*[out]*/ LPOS *plPos, /*[out]*/ IUnknown **ppunk)
{
	if( !ppunk )
		return E_FAIL;
	
	if( !plPos )
		return E_FAIL;

	*ppunk = m_treeTests.get( *plPos );
	*plPos = (LPOS)m_treeTests.next_child( lParentPos, *plPos );

	(*ppunk)->AddRef();

	return S_OK;
}

HRESULT CTestManager::GetNextTest( /*[in]*/ LPOS lParentPos, /*[out]*/ LPOS *plPos, /*[out, retval]*/ IDispatch **ppunk)
{
	if( !ppunk )
		return E_FAIL;
	
	if( !plPos )
		return E_FAIL;
	
	//if( plPos->vt != VT_I4 )
	//	return E_FAIL;

	IUnknown *punk = 0;

	if (GetNextTest(lParentPos, plPos, &punk) != S_OK)
		return E_FAIL;

	if( !punk )
		return E_FAIL;

	punk->QueryInterface( IID_IDispatch, (LPVOID *)ppunk );
	punk->Release();

	return S_OK;
}

HRESULT CTestManager::AddTest( /*[in]*/ LPOS lParent, /*[in]*/ IUnknown *punk,  /*[out,retval]*/ LPOS *plpos)
{
	if( !punk )
		return E_FAIL;
	
	if( !plpos )
		return E_FAIL;

	*plpos = m_treeTests.insert( punk, lParent ); 
	
	return S_OK;
}

HRESULT CTestManager::AddTest( /*[in]*/ LPOS lParent, /*[in]*/ IDispatch *punk,  /*[out,retval]*/ LPOS *plpos )
{
	if( !punk )
		return E_FAIL;

	if( !plpos )
		return E_FAIL;

	IUnknown *punkU = 0;

	punk->QueryInterface( IID_IUnknown, (LPVOID *)&punkU );

	if( !punkU )
		return E_FAIL;

	HRESULT hr = AddTest( lParent, (IUnknown *)punkU, plpos );


	return hr;
}


HRESULT CTestManager::DeleteTest( /*[in]*/ LPOS lPos )
{
	if( !m_treeTests.remove( lPos ) )
		return E_FAIL;

	return S_OK;
}

// interface ITestProcess
HRESULT CTestManager::RunTestSave()
{
	_trace_file( "test_sys.log", "[~] RunTestSave is started" );

	m_lState = 0;
	m_bRunning = true;

	m_listErr.clear();

	::FireEvent( GetAppUnknown(), szEventBeginTestProcess, Unknown(), 0, 0, 0 );

	long lPrevState = ::GetValueInt( GetAppUnknown(), "\\ImageCompress", "EnableCompress", 0L );
	::SetValue( GetAppUnknown(), "\\ImageCompress", "EnableCompress", 1L );

	long lCompression = ::GetValueInt(GetAppUnknown(), "\\FileFilters2", "JPGCompressionLevel", 100 );
	::SetValue(GetAppUnknown(), "\\FileFilters2", "JPGCompressionLevel", 65L );

	bool blast_terminate = GetValueInt( GetAppUnknown( ), "\\MacroHelper", "AllowTerminate", 0 ) != 0;
//	SetValue( GetAppUnknown( ), "\\MacroHelper", "AllowTerminate", 0L );


	UINT uiOldParam = 0;

	SystemParametersInfo( SPI_SETSCREENSAVEACTIVE, FALSE, (void *)&uiOldParam, SPIF_SENDCHANGE ); 
	
	_recursive_scan( 0, true, 3, 0 );

	SystemParametersInfo( SPI_SETSCREENSAVEACTIVE, uiOldParam, 0, SPIF_SENDCHANGE ); 

	::SetValue( GetAppUnknown(), "\\ImageCompress", "EnableCompress", lPrevState );
	::SetValue(GetAppUnknown(), "\\FileFilters2", "JPGCompressionLevel", lCompression );

	SetValue( GetAppUnknown( ), "\\MacroHelper", "AllowTerminate", (long)blast_terminate );

	::FireEvent( GetAppUnknown(), szEventEndTestProcess, Unknown(), 0, 0, 0 );
	_trace_file( "test_sys.log", "[~] RunTestSave is finished" );

	m_bInterrupt = false;
	m_bRunning = false;

	return S_OK;
}

HRESULT CTestManager::RunTestCompare()
{
	m_lState = 1;
	m_bRunning = true;

	m_listErr.clear();

	_trace_file( "test_sys.log", "[~] RunTestCompare is started" );
	::FireEvent( GetAppUnknown(), szEventBeginTestProcess, Unknown(), 0, 0, 0 );

	long lPrevState = ::GetValueInt( GetAppUnknown(), "\\ImageCompress", "EnableCompress", 0L );
	::SetValue( GetAppUnknown(), "\\ImageCompress", "EnableCompress", 1L );

	long lCompression = ::GetValueInt(GetAppUnknown(), "\\FileFilters2", "JPGCompressionLevel", 100 );
	::SetValue(GetAppUnknown(), "\\FileFilters2", "JPGCompressionLevel", 65L );

	UINT uiOldParam = 0;
	SystemParametersInfo( SPI_SETSCREENSAVEACTIVE, FALSE, (void *)&uiOldParam, SPIF_SENDCHANGE ); 
	
	_recursive_scan( 0, false, 3, 0 );

	SystemParametersInfo( SPI_SETSCREENSAVEACTIVE, uiOldParam, 0, SPIF_SENDCHANGE ); 

	::SetValue( GetAppUnknown(), "\\ImageCompress", "EnableCompress", lPrevState );
	::SetValue(GetAppUnknown(), "\\FileFilters2", "JPGCompressionLevel", lCompression );

	::FireEvent( GetAppUnknown(), szEventEndTestProcess, Unknown(), 0, 0, 0 );

	_trace_file( "test_sys.log", "[~] RunTestSave is finished" );

	m_bInterrupt = false;
	m_bRunning = false;

	return S_OK;
}

HRESULT CTestManager::SetEditMode( bool bEnter )
{
	if( bEnter )
	{
		m_lState = 2;
		m_bSimpleMode = true;
	}
	else
	{
		m_lState = 0;
		m_bSimpleMode = false;
	}
	return S_OK;
}

HRESULT CTestManager::RunTestSimple()
{
	m_lState = 2;
	m_bRunning = true;

	m_listErr.clear();
	_trace_file( "test_sys.log", "[~] RunTestSimple is started" );

	m_bSimpleMode = true;
	long lState = ::GetValueInt( ::GetAppUnknown(), "\\TestMan", "ExecutionParts", 3 );
//	::FireEvent( GetAppUnknown(), szEventBeginTestProcess, Unknown(), 0, 0, 0 );
	_recursive_scan( 0, false, lState, 0 );
//	::FireEvent( GetAppUnknown(), szEventEndTestProcess, Unknown(), 0, 0, 0 );
	m_bSimpleMode = false;
	_trace_file( "test_sys.log", "[~] RunTestSimple is finished" );

	m_bInterrupt = false;
	m_bRunning = false;

	return S_OK;
}

HRESULT CTestManager::StopTesting()
{
	if( !_stop_testing( ) )
		return E_FAIL;

	_trace_file( "test_sys.log", "[~] StopTesting is detected" );
	::FireEvent( GetAppUnknown(), szEventTerminateTestProcess, Unknown(), 0, 0, 0 );
	return S_OK;
}

HRESULT CTestManager::PauseTesting( BOOL bPaused )
{
	m_bPaused = (bool)bPaused;

	while( m_bPaused )
	{
		IApplicationPtr	ptrA( GetAppUnknown() );
		ptrA->ProcessMessage();
	}
	return S_OK;
}

HRESULT CTestManager::GetRunningState( /*[out, retval]*/ long *plState )
{
	if( !plState )
		return E_FAIL;

	*plState = long( m_lState );

	return S_OK;
}



//------------------------------------------------------------------------------------------
HRESULT CTestManager::_recursive_scan( LPOS lParentPos, bool bSave, long lFlag, long *plCounter )
{
	if( m_bInterrupt )
		return E_ABORT; // [vanek] - 03.09.2004

	IApplicationPtr sptrApp = ::GetAppUnknown();

	if( sptrApp != 0 && lFlag & 2 )
	{
		IUnknown *punkDoc = 0;
		sptrApp->GetActiveDocument( &punkDoc );

		if( punkDoc )
		{
			IDataContext2Ptr sptrDocCXT = punkDoc;
			punkDoc->Release(); punkDoc = 0;

			LPOS lPosDoc = 0;
			sptrDocCXT->GetFirstObjectPos( _bstr_t( szTypeImage ), &lPosDoc );

			while( lPosDoc )
			{
				IUnknown *punkObject = 0;
				sptrDocCXT->GetNextObject( _bstr_t( szTypeImage ), &lPosDoc, &punkObject );

				if( punkObject )
				{
					CString strName = ::GetObjectName( punkObject );

					INamedDataObject2Ptr	sptrN = punkObject;
					punkObject->Release(); punkObject = 0;

					if( !strName.IsEmpty() && strName.Find( "WndPhoto" ) != -1 )
					{
						if( sptrN )
						{
							sptrN->SetParent( 0, apfNotifyNamedData|sdfCopyLocalData|sdfCopyParentData );
							::DeleteObject( sptrDocCXT, sptrN );

							lPosDoc = 0;
							sptrDocCXT->GetFirstObjectPos( _bstr_t( szTypeImage ), &lPosDoc );
						}
					}
				}
			}

			lPosDoc = 0;
			sptrDocCXT->GetFirstObjectPos( _bstr_t( szTypeTestDataArray ), &lPosDoc );

			while( lPosDoc )
			{
				IUnknown *punkObject = 0;
				sptrDocCXT->GetNextObject( _bstr_t( szTypeTestDataArray ), &lPosDoc, &punkObject );

				if( punkObject )
				{
					INamedDataObject2Ptr	sptrN = punkObject;
					punkObject->Release(); punkObject = 0;

					if( sptrN )
					{
						sptrN->SetParent( 0, apfNotifyNamedData|sdfCopyLocalData|sdfCopyParentData );
						::DeleteObject( sptrDocCXT, sptrN );
						lPosDoc = 0;
						sptrDocCXT->GetFirstObjectPos( _bstr_t( szTypeTestDataArray ), &lPosDoc );
					}
				}
			}
		}
	}

	LPOS lPos = 0;
	if( GetFirstTestPos( lParentPos, &lPos ) != S_OK )
		return E_FAIL;

	_bstr_t bstrRootPath;
	if( lPos )
	{
		IUnknown *punkTest = m_treeTests.get( lParentPos ); // without AddRef

		if( punkTest )
		{
			ITestItemPtr sptrItem = punkTest;

			_bstr_t bstrPath;
			sptrItem->GetPath( bstrPath.GetAddress() );
			bstrRootPath = bstrPath;
		}
		else if( lParentPos == 0 )
		{

			CString strCD = m_strDBPath; 
			if( strCD[strCD.GetLength() - 1] == '\\' )
				strCD = strCD.Left( strCD.GetLength() - 1 );

			TCHAR szFName[_MAX_FNAME];
			_tsplitpath( strCD, NULL, NULL, szFName, NULL);

			strCD = strCD.Left( strCD.GetLength() - (int)strlen( szFName ) );
			if( strCD[strCD.GetLength() - 1] == '\\' )
				strCD = strCD.Left( strCD.GetLength() - 1 );

			bstrRootPath = strCD;
		}

		IUnknown *punkDoc = 0;
		sptrApp->GetActiveDocument( &punkDoc );

		INamedDataPtr sptrND = punkDoc;

		if( sptrND )
			sptrND->EnableBinaryStore( TRUE );
		
		if( lFlag & 1 )
		{
			_save_environment( punkDoc, (char *)bstrRootPath, "PR" );

			// vanek
			_save_tests_environment( bstrRootPath, _T(szTestTempEverychildName), 
				tefState|tefSDC|tefKbdLayout );
		}

		if( punkDoc )
			punkDoc->Release();
	}

	// [vanek] - 03.09.2004
	HRESULT hr_ret = S_OK;
	while( lPos )
	{
		IUnknown *punkTest = 0;

		LPOS lPrevPos = lPos;
		if( GetNextTest( lParentPos, &lPos, &punkTest ) != S_OK )
			return E_FAIL;

		if( !punkTest )
			continue;

		ITestItemPtr sptrItem = punkTest;
		punkTest->Release();

		if( sptrItem == 0 )
			return E_FAIL;

		ITestRunningDataPtr sptrItemR = sptrItem;
		
		if( sptrItemR == 0 )
			return E_FAIL;

		long lCheck = 0;
		sptrItemR->GetTestChecked( &lCheck );
		if( !lCheck )
			continue;

		// [vanek] SBT:1105 - 24.08.2004
		BOOL bcan_exec = FALSE;
        sptrItem->VerifyExecCond( &bcan_exec );
		if( !bcan_exec )
            continue;

		if( !bSave && !m_bSimpleMode )
		{
			long lSaved = 0;
			sptrItemR->GetTestSavedState( &lSaved );

			if( !lSaved )
				continue;
		}

		IUnknown *punkDoc = 0;
		sptrApp->GetActiveDocument( &punkDoc );

		INamedDataPtr sptrND = punkDoc;

		if( sptrND )
			sptrND->EnableBinaryStore( TRUE );

		sptrND = 0;

		if( punkDoc )
			punkDoc->Release();

		_bstr_t bstrPath;

		sptrItem->GetPath( bstrPath.GetAddress() );

		if( !bstrPath.length() )
			return E_FAIL;

		_bstr_t bstrScript;
		sptrItem->GetScript( bstrScript.GetAddress() );

		_bstr_t bstrScriptFinal;
		sptrItem->GetScriptFinal( bstrScriptFinal.GetAddress() );

        DWORD dwTestEnvFlags = 0;
		ITestEnvironmentPtr sptr_test_env = sptrItem;
		if( sptr_test_env != 0 )
		{	
			// запоминаем, что будет подгружать, когда будет идти начальна€ тестова€ последовательность
			sptr_test_env->GetLoadFlags( &dwTestEnvFlags );
			if( lFlag & 1)
			{		
				// сохран€ем состо€ние в соответсвии с наcтройками, чтобы потом откатитьс€ (после конечной тестовой последовательности)
				//_save_tests_environment( bstrPath, _T(szTestTempStateName), dwTestEnvFlags );

				// подгружаем соответсвующее окружение дл€ текущего теста
				if( dwTestEnvFlags & tefSDC )
				    sptr_test_env->LoadSDC( );
				
				if( dwTestEnvFlags & tefKbdLayout )
					sptr_test_env->LoadKbdLayout( );

				if( dwTestEnvFlags & tefState )
					sptr_test_env->LoadShellState( );
			}
			            			
        }

		{
			// установка посто€нных условий
			{
				LPOS lPosSingle = 0;
				sptrItem->GetFirstSingleCond( &lPosSingle );

				while( lPosSingle )
				{
					_variant_t varName, varPath, varValue;

					_variant_t varPosSingle( (LPOS)lPosSingle );
					sptrItem->NextSingleCond( &varPosSingle, &varName, &varPath, &varValue );
					lPosSingle = (LPOS)varPosSingle;

					CString strName = varName.bstrVal;
					CString strPath	= varPath.bstrVal;
					
					if( strName == "Application" )
						::SetValue( ::GetAppUnknown(), "", strPath, varValue );
					else if( strName == "Document" )
					{
						IUnknown *punkDoc = 0;
						IApplicationPtr sptrApp = ::GetAppUnknown();
						sptrApp->GetActiveDocument( &punkDoc );

						::SetValue( punkDoc, "", strPath, varValue );

						if( punkDoc )
							punkDoc->Release();
					}
					else if( strName == "Object" )
					{
						IUnknown *punkDoc = 0;
						IApplicationPtr sptrApp = ::GetAppUnknown();
						sptrApp->GetActiveDocument( &punkDoc );

						long lfirst = strPath.Find( '[' );
						long llast = strPath.Find( ']' );
						
						strName = strPath.Mid( lfirst + 1, llast - lfirst - 1 );
						strPath = strPath.Right( strPath.GetLength() - llast - 1 );

						IUnknown *punkO = ::FindObjectByName( punkDoc, strName ); 
						if( punkO )
						{
							INamedDataPtr sptrND = punkO;

							// [vanek] SBT:893 - 25.05.2004
							if( sptrND == 0 )
							{
								INamedDataObject2Ptr sptr_ndo = punkO;
								if( sptr_ndo != 0 )
								{
									sptr_ndo->InitAttachedData();
									sptrND = punkO;
								}
							} 

							punkO->Release();

							if( sptrND != 0 )
								::SetValue( sptrND, "", strPath, varValue );
						}
					}
				}

				lPosSingle = 0;
				sptrItem->GetFirstSingleCond( &lPosSingle );

				if( lPosSingle )
					_update_system_settings();
			}

			LPOS lMultyPos = 0;
			sptrItem->GetFirstMultyCond( &lMultyPos );

			if( !lMultyPos )
			{
				// если нет переменных условий
				if( m_lState != 2 )
				{
					_trace_file( "test_sys.log", "[~] TestSystem: BTE no multi_values case" );
					::FireEvent( GetAppUnknown(), szEventBeforeTestExecute, Unknown(), sptrItem, 0, 0 );
				}

				_create_hidden_doc();

				{
					IApplicationPtr	ptrA( GetAppUnknown() );
					ptrA->ProcessMessage();
				}

				DWORD dwBefore = ::GetTickCount();
				if( bstrScript.length() )
					if( lFlag & 1 )
					{
						_CManageManualModeMH ManageManualModeMH(sptrItem);
						::ExecuteScript( bstrScript, "CTestManager::_test_db_cmp" );
					}
				DWORD dwAfter = ::GetTickCount();

				_copy_data_to_hidden();

				{
					IApplicationPtr	ptrA( GetAppUnknown() );
					ptrA->ProcessMessage();
				}

				CString strC;
				if( plCounter )
					strC.Format( ".%x", *plCounter );
				{

					SYSTEMTIME	sysTime;
					DATE	date;
					
					::GetLocalTime( &sysTime );
					::SystemTimeToVariantTime( &sysTime, &date );

					if( bSave )
					{
						sptrItemR->SetTestSavedState( 1 );
						sptrItemR->SetSaveTime( date );
						sptrItemR->SetTestRunTime( dwAfter - dwBefore );

						sptrItem->StoreContent();

						IUnknown *punkDoc = 0;
						sptrApp->GetActiveDocument( &punkDoc );

						if( lFlag & 1 )
						{
							if( m_sptrHiddenDoc )
								_save_environment( m_sptrHiddenDoc, (char *)bstrPath, "S", strC );
							else
								_save_environment( punkDoc, (char *)bstrPath, "S", strC );
						}

						if( punkDoc )
							punkDoc->Release();

					}
					else
					{
						sptrItemR->SetRunTime( date );
						sptrItem->StoreContent();

						if( !m_bSimpleMode )
						{
							IUnknown *punkDoc = 0;
							sptrApp->GetActiveDocument( &punkDoc );

							if( lFlag & 1 )
							{
								if( m_sptrHiddenDoc )
									_save_environment( m_sptrHiddenDoc, (char *)bstrPath, "C", strC );
								else
									_save_environment( punkDoc, (char *)bstrPath, "C", strC );
							}

							if( punkDoc )
								punkDoc->Release();
						}
					}
				}

				if( !bSave && m_lState != 2 )
				{
					TEST_ERR_DESCR *ted = new TEST_ERR_DESCR;
					::ZeroMemory( ted, sizeof( TEST_ERR_DESCR ) );

					bool bError = false;
					if( ( m_bInterrupt && m_bterminate_current_only ) || !_compare_context( (char *)( bstrPath + "\\TDC_" + m_strLang+ _bstr_t( strC ) + ".image"), (char *)( bstrPath + "\\TDS_" + m_strLang + _bstr_t( strC ) + ".image"), ted  ) )
					{

						DWORD dw = MAKEWORD( 0, 0 );
						if( m_lState != 2 )
						{
							_trace_file( "test_sys.log", "[~] TestSystem: ATE no multi_values case / 0" );
							::FireEvent( GetAppUnknown(), szEventAfterTestExecute, Unknown(), sptrItem, &dw, sizeof( dw ) );
						}

						bError = true;
					}
					else
					{
						if( !m_bSimpleMode )
						{
							IUnknown *punkDoc = 0;
							sptrApp->GetActiveDocument( &punkDoc );
				
							if( lFlag & 2 )
							{
								if( m_sptrHiddenDoc )
									_del_environment( m_sptrHiddenDoc, (char*)bstrPath, "C", strC );
								else
									_del_environment( punkDoc, (char*)bstrPath, "C", strC );
							}

							if( punkDoc )
								punkDoc->Release();
						}

						DWORD dw = MAKEWORD( 1, 0 );
						if( m_lState != 2 )
						{
							_trace_file( "test_sys.log", "[~] TestSystem: ATE no multi_values case" );
							::FireEvent( GetAppUnknown(), szEventAfterTestExecute, Unknown(), sptrItem, &dw, sizeof( dw ) );
					}
					}


					DWORD dwTime = 0;
					sptrItemR->GetTestRunTime( &dwTime );

					sptrItemR->SetTestErrorState( bError );
					sptrItem->StoreContent();

					ted->dwDiffTime = ( dwAfter - dwBefore ) - dwTime;
					ted->bTestError = bError;
					if( bError || labs( ted->dwDiffTime ) > m_dwMinTimeDiff )
					{
						ted->bstrPath = bstrPath.copy(); 
						if( plCounter )
							ted->nTryIndex = *plCounter;
						else
							ted->nTryIndex = 0;

						m_listErr.add_tail( ted );
					}
					else
						delete ted;
				}
				else
				{
					DWORD dw = MAKEWORD( 1, 0 );
					if( m_lState != 2 )
					{
						_trace_file( "test_sys.log", "[~] TestSystem: ATE no multi_values case(save) / 1" );
						::FireEvent( GetAppUnknown(), szEventAfterTestExecute, Unknown(), sptrItem, &dw, sizeof( dw ) );
				}
				}

				m_sptrHiddenDoc = 0;

				{
					IApplicationPtr	ptrA( GetAppUnknown() );
					ptrA->ProcessMessage();
				}

				// [vanek]
				//if( _recursive_scan( lPrevPos, bSave, lFlag, plCounter ) != S_OK )
				//	return E_FAIL;
				hr_ret = _recursive_scan( lPrevPos, bSave, lFlag, plCounter );
				
				//[max] support only one way terminate
				if( m_bterminate_current_only && m_bInterrupt )
				{
					m_bInterrupt = false;
					m_bterminate_current_only = false;
					hr_ret = S_OK;
				}

				if( hr_ret != S_OK && hr_ret != E_ABORT )
					return hr_ret;

				{
					IApplicationPtr	ptrA( GetAppUnknown() );
					ptrA->ProcessMessage();
				}

			}
			else
			{	 // executing variant conditions
				long lCounter = 1;
				long lFailCounter = 0;

 				long *pCounter = plCounter ? plCounter : &lCounter;

				IUnknown *punkDoc = 0;
				sptrApp->GetActiveDocument( &punkDoc );

				{
					IApplicationPtr	ptrA( GetAppUnknown() );
					ptrA->ProcessMessage();
				}

				// [vanek] - 03.09.2004
				//if( !_exec_var_cond( sptrItem, lMultyPos, bstrScript, bstrPath, bstrRootPath, punkDoc, pCounter, bSave, &lFailCounter, lFlag, lPrevPos ) )
				//	return E_FAIL;
				hr_ret = _exec_var_cond( sptrItem, lMultyPos, bstrScript, bstrPath, bstrRootPath, punkDoc, pCounter, bSave, &lFailCounter, lFlag, lPrevPos );

				//[max] support only one way terminate
				if( m_bterminate_current_only && m_bInterrupt )
				{
					m_bInterrupt = false;
					m_bterminate_current_only = false;
					hr_ret = S_OK;
				}

				if( hr_ret != S_OK && hr_ret != E_ABORT )
					return hr_ret;

				{
					IApplicationPtr	ptrA( GetAppUnknown() );
					ptrA->ProcessMessage();
				}

				if( punkDoc )
					punkDoc->Release();

			}
		}

		{
			IUnknown *punkDoc = 0;
			sptrApp->GetActiveDocument( &punkDoc );

			IFileDataObject2Ptr ptrFDO = punkDoc;
			if( ptrFDO )
				ptrFDO->SetModifiedFlag( FALSE );

			if( punkDoc )
				punkDoc->Release();
		}

		if( lFlag & 2 )
		{
			if( bstrScriptFinal.length() )
			{
				_CManageManualModeMH ManageManualModeMH(sptrItem);
				::ExecuteScript( bstrScriptFinal, "CTestManager::_test_db_cmp" );
			}

			IUnknown *punkDoc = 0;
			sptrApp->GetActiveDocument( &punkDoc );
			
			_load_environment( punkDoc, (char *)bstrRootPath, "PR" );

			if( punkDoc )
				punkDoc->Release();	punkDoc = 0;

			// [vanek] : восстанавливаем ранее сохраненное окружение - 05.08.2004
			_load_tests_environment( bstrRootPath, _T(szTestTempEverychildName), 
				tefState|tefSDC|tefKbdLayout ); 
		}

		// [vanek] - 03.09.2004
		if( hr_ret == E_ABORT )
			break;
	}

	if( lFlag & 2 && bstrRootPath.length() > 0 )
	{
		// [vanek] : все подтесты выполнены - удал€ем файлы с сохраненным окружением дл€ подтестов - 05.08.2004
		_del_tests_environment( bstrRootPath, _T(szTestTempEverychildName), tefState|tefSDC|tefKbdLayout );

		IUnknown *punkDoc = 0;
		sptrApp->GetActiveDocument( &punkDoc );

		_del_environment( punkDoc, (char *)bstrRootPath, "PR" );
		
		if( punkDoc )
			punkDoc->Release();

	}

	return hr_ret; // [vanek] - 03.09.2004
}

void _fn_cleaner( _list_t< _variant_t > *pData )
{
	pData->clear();
	delete pData;
}

//------------------------------------------------------------------------------------------
bool	CTestManager::_stop_testing( )
{
    if( !m_bRunning )
		return false;

	m_bInterrupt = true;
	return true;
}

HRESULT CTestManager::_exec_var_cond( IUnknown *punkTest, LPOS lPos, _bstr_t bstrScript, _bstr_t bstrPath, _bstr_t bstrRootPath, IUnknown *punkF, long *plCounter, bool bSave, long *plFailCounter, long lFlag, LPOS lPrevPos )
{
	{
		IApplicationPtr	ptrA( GetAppUnknown() );
		ptrA->ProcessMessage();
	}

	if( m_bInterrupt )
		return E_ABORT; // [vanek] - 03.09.2004
	
	static int iColumnIndex = -1; // Used for __RUN_BY_COLUMN;
	static TPOS lListPos = 0; // Used for __RUN_BY_COLUMN;
	static _list_t< _list_t< _variant_t > *, &_fn_cleaner > lstLastData; // Used for __EXCLUDE_EQUAL;

	if( !lPos )
	{
		DWORD dw = MAKEWORD( 0, *plCounter );
		if( m_lState != 2 )
		{
			_trace_file( "test_sys.log", "[~] TestSystem: BTE multi values" );
			::FireEvent( GetAppUnknown(), szEventBeforeTestExecute, Unknown(), punkTest, &dw, sizeof( dw ) );
		}

//		_update_system_settings();

		if( lFlag & 1 )
			_save_environment( punkF, (char *)bstrPath, "MP" );

		_create_hidden_doc();

		DWORD dwBefore = ::GetTickCount();
		if( bstrScript.length() )
			if( lFlag & 1 )
			{
				ITestItemPtr sptrTI(punkTest);
				_CManageManualModeMH ManageManualModeMH(sptrTI);
				::ExecuteScript( bstrScript, "CTestManager::_exec_var_cond" );
			}
		DWORD dwAfter = ::GetTickCount();

		_copy_data_to_hidden();

		{
			IApplicationPtr	ptrA( GetAppUnknown() );
			ptrA->ProcessMessage();
		}

		{
			CString strC;
			strC.Format( ".%x", *plCounter );

			SYSTEMTIME	sysTime;
			DATE	date;
			
			::GetLocalTime( &sysTime );
			::SystemTimeToVariantTime( &sysTime, &date );

			if( bSave )
			{
				ITestRunningDataPtr sptrItemR = punkTest;
				sptrItemR->SetTestSavedState( 1 );
				sptrItemR->SetSaveTime( date );
				sptrItemR->SetTestRunTime( dwAfter - dwBefore );

				ITestItemPtr sptrItem = punkTest;
				sptrItem->StoreContent();

				if( lFlag & 1 )
				{
					if( m_sptrHiddenDoc )
						_save_environment( m_sptrHiddenDoc, (char *)bstrPath, "S", strC );
					else
						_save_environment( punkF, (char *)bstrPath, "S", strC );
				}
			}
			else
			{
				ITestRunningDataPtr sptrItemR = punkTest;
				sptrItemR->SetRunTime( date );

				ITestItemPtr sptrItem = punkTest;
				sptrItem->StoreContent();

				if( !m_bSimpleMode )
				{
					if( lFlag & 1 )
					{
						if( m_sptrHiddenDoc )
							_save_environment( m_sptrHiddenDoc, (char *)bstrPath, "C", strC );
						else
							_save_environment( punkF, (char *)bstrPath, "C", strC );
					}
				}
			}

			if( !bSave && m_lState != 2 )
			{
				TEST_ERR_DESCR *ted = new TEST_ERR_DESCR;
				::ZeroMemory( ted, sizeof( TEST_ERR_DESCR ) );

				bool bError = false;
 				if( !_compare_context( (char *)( bstrPath + "\\TDC_" + m_strLang + _bstr_t( strC ) + ".image" ), (char *)( bstrPath + "\\TDS_" + m_strLang + _bstr_t( strC ) + ".image" ), ted ) )
				{
					(*plFailCounter)++;

					DWORD dw = MAKEWORD( 0, *plCounter );
					if( m_lState != 2 )
					{
						_trace_file( "test_sys.log", "[~] TestSystem: ATE multi values  0 / %d", *plCounter );
						::FireEvent( GetAppUnknown(), szEventAfterTestExecute, Unknown(), punkTest, &dw, sizeof( dw ) );
					}
					
					bError = true;
				}
				else
				{
					if( !m_bSimpleMode )
					{
						if( lFlag & 2 )
						{
							if( m_sptrHiddenDoc )
								_del_environment( m_sptrHiddenDoc, (char *)bstrPath, "C", strC );
							else
								_del_environment( punkF, (char *)bstrPath, "C", strC );
						}
					}

					DWORD dw = MAKEWORD( 1, *plCounter );
					if( m_lState != 2 )
					{
						_trace_file( "test_sys.log", "[~] TestSystem: ATE multi values 1 / %d", *plCounter );
						::FireEvent( GetAppUnknown(), szEventAfterTestExecute, Unknown(), punkTest, &dw, sizeof( dw ) );
					}
				}
			    
				ITestRunningDataPtr sptrItemR = punkTest;
				DWORD dwTime = 0;
				sptrItemR->GetTestRunTime( &dwTime );

				ted->dwDiffTime = ( dwAfter - dwBefore ) - dwTime;
				ted->bTestError = bError;
				if( bError || labs( ted->dwDiffTime ) > m_dwMinTimeDiff )
				{
					ted->bstrPath = bstrPath.copy(); 
					ted->nTryIndex = *plCounter; 

					m_listErr.add_tail( ted );
				}
				else
					delete ted;
			}
			else
			{
					DWORD dw = MAKEWORD( 1, *plCounter );
					if( m_lState != 2 )
					{
						_trace_file( "test_sys.log", "[~] TestSystem: ATE multi values(save)  1/%d", *plCounter );
						::FireEvent( GetAppUnknown(), szEventAfterTestExecute, Unknown(), punkTest, &dw, sizeof( dw ) );
			}
			}

			(*plCounter)++;
		}

		int iColumnIndexTmp = iColumnIndex;
		iColumnIndex = -1;

		TPOS lListPosTmp = lListPos;
		lListPos = 0;

		{
			IApplicationPtr	ptrA( GetAppUnknown() );
			ptrA->ProcessMessage();
		}

		m_sptrHiddenDoc = 0;
		long lCounter = ( *plCounter ) * 1000;

		// [vanek] - 03.09.2004
		//if( _recursive_scan( lPrevPos, bSave, lFlag, &lCounter ) != S_OK )
        //	return false;
		HRESULT hr = S_OK;
		if( (hr = _recursive_scan( lPrevPos, bSave, lFlag, &lCounter )) != S_OK )
			return hr;

		{
			IApplicationPtr	ptrA( GetAppUnknown() );
			ptrA->ProcessMessage();
		}

		iColumnIndex = iColumnIndexTmp;
		lListPos = lListPosTmp;


		if( lFlag & 2 )
		{
			IApplicationPtr sptrApp = ::GetAppUnknown();
		
			IUnknown *punkDoc = 0;
			sptrApp->GetActiveDocument( &punkDoc );

			_load_environment( punkDoc, (char *)bstrPath, "MP" );
			_del_environment( punkDoc, (char *)bstrPath, "MP" );

			if( punkDoc )
				punkDoc->Release();
		}

		// [vanek]
		//return  true; 
		return S_OK;
	}

	{
		IApplicationPtr	ptrA( GetAppUnknown() );
		ptrA->ProcessMessage();
	}

	ITestItemPtr sptrItem = punkTest;

	_variant_t varName, varPath;

	VARIANT *pvarValue = 0;
	long lCount = 0;

	_variant_t varPos( lPos );
	_variant_t varFlag( 0L );

	_variant_t varNameMain;

	VARIANT *pvarValueMain = 0;
	long lCountMain = 0;

	sptrItem->NextMultyCond( &varPos, &varName, &varPath, &pvarValue, &varFlag, &lCount, &varNameMain, &pvarValueMain, &lCountMain );

	lPos= (LPOS)varPos;

	CString strName = varName.bstrVal;
	CString strPath	= varPath.bstrVal;

	DWORD dwFlag = (long)varFlag;

	// [vanek] - 03.09.2004
	HRESULT hr_ret = S_OK;
    do
	{
		if( strName == "Application" )
		{
			for( long i = 0; i < lCount; i++ )
			{
				if( dwFlag & __RUN_BY_COLUMN )
				{
					long lLast = iColumnIndex;

					if( iColumnIndex < 0 )
						iColumnIndex = i;

					if( iColumnIndex == i  )
					{
						::SetValue( ::GetAppUnknown(), "", strPath, pvarValue[i] );
						// [vanek]
						//if( !_exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos ) )
						//	return false;
						if( (hr_ret = _exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos )) != S_OK )
							break;
					}

					iColumnIndex = lLast;
				}
				else if( dwFlag & __EXCLUDE_EQUAL )
				{
					bool bOK = true;
					TPOS lLastPos = 0;

					bool bCreate = false;
					if( lListPos )
					{
						_list_t< _variant_t > *pList = lstLastData.get( (TPOS)lListPos );

						for (TPOS _lpos = pList->head(); _lpos; _lpos = pList->next(_lpos))
						{
							_variant_t &var = pList->get( _lpos );

							if( var == pvarValue[i] )
							{
								bOK = false;
								break;
							}
						}
					}
					else
					{
						lListPos = lstLastData.add_head( new _list_t< _variant_t > );
						bCreate = true;
					}

					_list_t< _variant_t > *pList = lstLastData.get( lListPos );

					if( bOK )
					{
						lLastPos = pList->add_tail( pvarValue[i] );
						::SetValue( ::GetAppUnknown(), "", strPath, pvarValue[i] );
						// [vanek] - 03.09.2004
						//if( !_exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos ) )
						//	return false;
						if( (hr_ret = _exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos )) != S_OK )
							break;
					}

					if( lLastPos )
						pList->remove( lLastPos );

					if( bCreate )
						lListPos = 0;

				}
				else if( dwFlag & __HAS_GROUP )
				{
					_variant_t var;
					_variant_t varRes = GetValueString( ::GetAppUnknown(), 0, (char *)_bstr_t( varNameMain.bstrVal ),  "" );
					bool bOK = false;
					for( int q = 0; q < lCountMain; q++ )
					{
						if( dwFlag & __NOT_EQUAL_GROUP )
						{
							if( varRes != pvarValueMain[q] )
								bOK = true;
						}
						else
						{
							if( varRes == pvarValueMain[q] )
								bOK = true;
						}
					}

					if( bOK )
						::SetValue( ::GetAppUnknown(), "", strPath, pvarValue[i] );

					// [vanek] - 03.09.2004
					//if( !_exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos ) )
					//	return false;
					if( (hr_ret = _exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos )) != S_OK )
						break;

					if( !bOK )
						break;
				}
				else
				{
					::SetValue( ::GetAppUnknown(), "", strPath, pvarValue[i] );
					// [vanek] - 03.09.2004
					//if( !_exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos ) )
					//	return false;
					if( (hr_ret = _exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos )) != S_OK )
						break;
				}
			}

			// [vanek] - 03.09.2004
			if( hr_ret != S_OK )
				break;

		}
		else if( strName == "Document" )
		{
			IUnknown *punkDoc = 0;
			IApplicationPtr sptrApp = ::GetAppUnknown();
			sptrApp->GetActiveDocument( &punkDoc );

			for( long i = 0; i < lCount; i++ )
			{
				if( dwFlag & __RUN_BY_COLUMN )
				{
					long lLast = iColumnIndex;

					if( iColumnIndex < 0 )
						iColumnIndex = i;

					if( iColumnIndex == i  )
					{
						::SetValue( punkDoc, "", strPath, pvarValue[i] );
						// [vanek] - 03.09.2004
						//if( !_exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos ) )
						//	return false;
						if( (hr_ret = _exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos )) != S_OK )
							break;
					}

					iColumnIndex = lLast;
				}
				else if( dwFlag & __EXCLUDE_EQUAL )
				{
					bool bOK = true;
					TPOS lLastPos = 0;

					if( lListPos )
					{
						_list_t< _variant_t > *pList = lstLastData.get( lListPos );

						for (TPOS _lpos = pList->head(); _lpos; _lpos = pList->next(_lpos))
						{
							_variant_t &var = pList->get( _lpos );

							if( var == pvarValue[i] )
							{
								bOK = false;
								break;
							}
						}
					}
					else
						lListPos = lstLastData.add_head( new _list_t< _variant_t > );

					_list_t< _variant_t > *pList = lstLastData.get( lListPos );

					if( bOK )
					{
						lLastPos = pList->add_tail( pvarValue[i] );
						::SetValue( punkDoc, "", strPath, pvarValue[i] );
						// [vanek] - 03.09.2004
						//if( !_exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos ) )
						//	return false;
						if( (hr_ret = _exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos )) != S_OK )
							break;
					}

					if( lLastPos )
						pList->remove( lLastPos );

					if( lListPos )
						pList->remove( lListPos );
					lListPos = 0;
				}
				else if( dwFlag & __HAS_GROUP )
				{
					_variant_t var;
					_variant_t varRes = GetValueString( punkDoc, 0, (char *)_bstr_t( varNameMain.bstrVal ),  "" );
					bool bOK = false;
					for( int q = 0; q < lCountMain; q++ )
					{
						if( varRes == pvarValueMain[q] )
							bOK = true;
					}

					if( bOK )
						::SetValue( punkDoc, "", strPath, pvarValue[i] );

					// [vanek] - 03.09.2004
					//if( !_exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos ) )
					//	return false;
					if( (hr_ret = _exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos )) != S_OK )
						break;

					if( !bOK )
						break;
				}
				else
				{
					::SetValue( punkDoc, "", strPath, pvarValue[i] );
					// [vanek] - 03.09.2004
                    //if( !_exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos ) )
					//	return false;
					if( (hr_ret = _exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos )) != S_OK )
						break;
				}
			}
			if( punkDoc )
				punkDoc->Release();

			// [vanek] - 03.09.2004
			if( hr_ret != S_OK )
				break;
		}
		else if( strName == "Object" )
		{

			long lfirst = strPath.Find( '[' );
			long llast = strPath.Find( ']' );

			strName = strPath.Mid( lfirst + 1, llast - lfirst - 1 );
			strPath = strPath.Right( strPath.GetLength() - llast - 1 );

			for( long i = 0; i < lCount; i++ )
			{
				IUnknown *punkDoc = 0;
				IApplicationPtr sptrApp = ::GetAppUnknown();
				sptrApp->GetActiveDocument( &punkDoc );

				IUnknown *punkO = ::FindObjectByName( punkDoc, strName ); 
				if( punkDoc )
					punkDoc->Release();

				if( punkO )
				{
					INamedDataPtr sptrND = punkO;

					// [vanek] SBT:893 - 25.05.2004
					if( sptrND == 0 )
					{
						INamedDataObject2Ptr sptr_ndo = punkO;
						if( sptr_ndo != 0 )
						{
							sptr_ndo->InitAttachedData();
							sptrND = punkO;
						}
					}

					punkO->Release();

					if( dwFlag & __RUN_BY_COLUMN )
					{
						long lLast = iColumnIndex;

						if( iColumnIndex < 0 )
							iColumnIndex = i;

						if( iColumnIndex == i  )
						{
							if( sptrND != 0 )
								::SetValue( sptrND, "", strPath, pvarValue[i] );
							// [vanek] - 03.09.2004
							//if( !_exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos ) )
							//	return false;
							if( (hr_ret = _exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos )) != S_OK )
								break;
						}

						iColumnIndex = lLast;
					}
					else if( dwFlag & __EXCLUDE_EQUAL )
					{
						bool bOK = true;
						TPOS lLastPos = 0;

						if( lListPos )
						{
							_list_t< _variant_t > *pList = lstLastData.get( lListPos );

							for (TPOS _lpos = pList->head(); _lpos; _lpos = pList->next(_lpos))
							{
								_variant_t &var = pList->get( _lpos );

								if( var == pvarValue[i] )
								{
									bOK = false;
									break;
								}
							}
						}
						else
							lListPos = lstLastData.add_head( new _list_t< _variant_t > );

						_list_t< _variant_t > *pList = lstLastData.get( lListPos );

						if( bOK )
						{
							lLastPos = pList->add_tail( pvarValue[i] );
							if( sptrND )
								::SetValue( sptrND, "", strPath, pvarValue[i] );
							// [vanek] - 03.09.2004
							//if( !_exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos ) )
							//	return false;
							if( (hr_ret = _exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos )) != S_OK )
								break;
						}

						if( lLastPos )
							pList->remove( lLastPos );

						if( lListPos )
							pList->remove( lListPos );
						lListPos = 0;
					}
					else if( dwFlag & __HAS_GROUP )
					{
						_variant_t var;
						_variant_t varRes = GetValueString( sptrND, 0, (char *)_bstr_t( varNameMain.bstrVal ),  "" );
						bool bOK = false;
						for( int q = 0; q < lCountMain; q++ )
						{
							if( varRes == pvarValueMain[q] )
								bOK = true;
						}

						if( bOK )
							::SetValue( sptrND, "", strPath, pvarValue[i] );

						// [vanek] - 03.09.2004
						//if( !_exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos ) )
						//	return false;
						if( (hr_ret = _exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos )) != S_OK )
							break;

						if( !bOK )
							break;
					}
					else
					{
						if( sptrND )
							::SetValue( sptrND, "", strPath, pvarValue[i] );

						// [vanek] - 03.09.2004
						//if( !_exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos ) )
						//	return false;
						if( (hr_ret = _exec_var_cond( punkTest, lPos, bstrScript, bstrPath, bstrRootPath, punkF, plCounter, bSave, plFailCounter, lFlag, lPrevPos )) != S_OK )
                            break;
					}
				}
			}

			// [vanek] - 03.09.2004
			if( hr_ret != S_OK )
				break;
		}

	}while( 0 );

	if( pvarValue )
		delete []pvarValue;

	return hr_ret;	// AAM: ??? ƒолжно быть S_OK?
					// [vanek] SBT:1137 угу - 04.10.2044
}

//------------------------------------------------------------------------------------------

HRESULT CTestManager::_load_db( CString lpstrFolder, LPOS lParentPos )
{
	if( m_strDBPath.IsEmpty() )
		return S_FALSE;

	HANDLE hFindDescriptor = 0;
	WIN32_FIND_DATA fd = {0};

	CString cstrCurrentDir = lpstrFolder;

	if( cstrCurrentDir[cstrCurrentDir.GetLength() - 1] != '\\' )
		cstrCurrentDir += "\\";

	if( !SetCurrentDirectory( cstrCurrentDir ) )
		return S_FALSE;

	hFindDescriptor = FindFirstFile( CString( "*.*" ), &fd );
	if( hFindDescriptor == INVALID_HANDLE_VALUE )
		return S_FALSE;

	if( !lParentPos )
	{
		IUnknown *punkTest = 0;
		if( ::CoCreateInstance( clsidTestItem, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID *)&punkTest ) != S_OK )
			return S_FALSE;

		if( !punkTest )
			return S_FALSE;

		ITestItemPtr sptrTest = punkTest;
		punkTest->Release();

		if( sptrTest == 0 )
			return S_FALSE;

		lParentPos = m_treeTests.insert( sptrTest, lParentPos );

		CString strCD = cstrCurrentDir; 
		if( strCD[strCD.GetLength() - 1] == '\\' )
			strCD = strCD.Left( strCD.GetLength() - 1 );

		if( sptrTest->SetPath( _bstr_t( strCD ) ) != S_OK )
			return S_FALSE;
	}

	do
	{
		if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if( CString(fd.cFileName) != ".." && CString(fd.cFileName) != ".")
			{
				IUnknown *punkTest = 0;
				if( ::CoCreateInstance( clsidTestItem, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID *)&punkTest ) != S_OK )
					continue;

				if( !punkTest )
					continue;

				ITestItemPtr sptrTest = punkTest;
				punkTest->Release();

				if( sptrTest == 0 )
					continue;

				LPOS lPos = m_treeTests.insert(	sptrTest, lParentPos );

				if( sptrTest->SetPath( _bstr_t( cstrCurrentDir + CString(fd.cFileName) ) ) != S_OK )
					continue;

				if( _load_db( cstrCurrentDir + CString(fd.cFileName), lPos ) != S_OK )
					continue;
			}
		}
		else
		{
			if( CString(fd.cFileName) != "test.ini" )
				continue;

			HANDLE hFile = CreateFile( cstrCurrentDir + CString(fd.cFileName), 
				GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
			
			if( hFile == INVALID_HANDLE_VALUE )
				continue;

			CloseHandle( hFile );
			hFile = INVALID_HANDLE_VALUE;

			ITestItemPtr sptrTest = m_treeTests.get( lParentPos );

			if( sptrTest == 0 )
				continue;

			if( sptrTest->LoadContent() != S_OK )
				continue;

			if( sptrTest->ParseCond() != S_OK )
				continue;

			if( sptrTest->ParseExecCond() != S_OK )
				continue;
		}
	} while( FindNextFile( hFindDescriptor, &fd ) );

	FindClose( hFindDescriptor );
	return S_OK;
}

bool CTestManager::_compare_context( CString strCmpFile, CString strSaveFile, TEST_ERR_DESCR *ted )
{
	bool bRetVal = true;

	_list_t<CString> listImageNames;
	_list_t<CString> listArrNames;

	IFileDataObjectPtr	sptrF( "Data.NamedData", 0, CLSCTX_INPROC_SERVER );
	if( sptrF )
	{
		if( sptrF->LoadFile( _bstr_t( strSaveFile ) ) ==  S_OK )
		{
			IFileDataObjectPtr	sptrF2( "Data.NamedData", 0, CLSCTX_INPROC_SERVER );
			if( sptrF2 )
			{
				if( sptrF2->LoadFile( _bstr_t( strCmpFile ) ) ==  S_OK )
				{

					CLSID	clsidContext = {0};

					if( ::CLSIDFromProgID( _bstr_t( szContextProgID ), &clsidContext ) != S_OK  )
						return false;

					IUnknown *punkContext = 0;
					if( ::CoCreateInstance( clsidContext, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punkContext ) != S_OK )
						return false;

					IDataContext3Ptr sptrDocCXT = punkContext;
					if( punkContext )
						punkContext->Release(); punkContext = 0;

					sptrDocCXT->AttachData( sptrF2 );


					LONG_PTR lPosDoc = 0;
					sptrDocCXT->GetFirstObjectPos( _bstr_t( szTypeImage ), &lPosDoc );

					while( lPosDoc )
					{
						IUnknown *punkObject = 0;
						sptrDocCXT->GetNextObject( _bstr_t( szTypeImage ), &lPosDoc, &punkObject );

						ICompatibleObject3Ptr sptrIC = punkObject;
						if( punkObject )
							punkObject->Release(); punkObject = 0;

						_bstr_t bstrObjectName = ::GetObjectName( sptrIC );

						CString strName = (char *)bstrObjectName;

						if( !strName.IsEmpty() && strName.Find( "WndPhoto" ) != -1 )
						{
							punkObject = ::GetObjectByName( sptrF, bstrObjectName, _bstr_t( szTypeImage ) );

							if( punkObject )
							{
								ICompatibleObject3Ptr sptrIC2 = punkObject;
								punkObject->Release(); punkObject = 0;

								if( sptrIC2 )
								{

									if( sptrIC->IsEqual( sptrIC2 ) != S_OK )
									{
										listImageNames.add_tail( strName );

										bRetVal = false;
										continue;
									}
									
								}
							}
							else
							{
								listImageNames.add_tail( strName );

								bRetVal = false;
								continue;
							}
						}
					}


					lPosDoc = 0;
					sptrDocCXT->GetFirstObjectPos( _bstr_t( szTypeTestDataArray ), &lPosDoc );

					while( lPosDoc )
					{
						IUnknown *punkObject = 0;
						sptrDocCXT->GetNextObject( _bstr_t( szTypeTestDataArray ), &lPosDoc, &punkObject );

						ITestDataArrayPtr sptrTD = punkObject;
						if( punkObject )
							punkObject->Release(); punkObject = 0;

						_bstr_t bstrObjectName = ::GetObjectName( sptrTD );

						CString strName = (char *)bstrObjectName;

						if( !strName.IsEmpty() )
						{
							punkObject = ::GetObjectByName( sptrF, bstrObjectName, _bstr_t( szTypeTestDataArray ) );

							if( punkObject )
							{
								ITestDataArrayPtr sptrTD2 = punkObject;
								punkObject->Release(); punkObject = 0;

								if( sptrTD2 )
								{
									long lCount = 0;
									long lCount2 = 0;

									sptrTD->GetItemCount( &lCount );
									sptrTD2->GetItemCount( &lCount2 );

									if( lCount != lCount2 )
									{
										listArrNames.add_tail( strName );

										bRetVal = false;
										continue;
									}

									for( int i = 0; i < lCount; i++ )
									{
										// [vanek] SBT:1060 - 05.07.2004
										_variant_t var_new_val;
										sptrTD->GetItem( i, &var_new_val );

										// [vanek] SBT: 910 compare items using compare expression - 27.05.2004
										long lequal_items = 0;
										sptrTD2->CompareItem( i, var_new_val, &lequal_items );
										if( !lequal_items )
										{
											listArrNames.add_tail( strName );			
											bRetVal = false;
											break;
										}
									}
								}
							}
							else
							{
								listArrNames.add_tail( strName );

								bRetVal = false;
								continue;
							}
						}
					}

					sptrDocCXT->AttachData( 0 );
				}
				else 
					return false;
			}
		}
		else
			return false;
	}

	if( !bRetVal && ted )
	{
		ted->lszImages = listImageNames.count();

		if( ted->lszImages )
		{
			ted->pbstrImageNames = new BSTR[ ted->lszImages ];

			long i = 0;

			TPOS lPos = listImageNames.head();
			while( lPos )
			{
				CString str = listImageNames.get( lPos );
				ted->pbstrImageNames[i++] = str.AllocSysString();
				lPos = listImageNames.next( lPos );
			}
		}
		else
			ted->pbstrImageNames = 0;

		ted->lszArrays = listArrNames.count();

		if( ted->lszArrays )
		{
			ted->pbstrArrNames = new BSTR[ ted->lszArrays ];
	
			long i = 0;

			TPOS lPos = listArrNames.head();
			while( lPos )
			{
				CString str = listArrNames.get( lPos );
				ted->pbstrArrNames[i++] = str.AllocSysString();
				lPos = listArrNames.next( lPos );
			}
		}
		else
			ted->pbstrArrNames = 0;
	}

	return bRetVal;
}

// interface ITestManErrorDescr
HRESULT CTestManager::GetFirstErrorPos( /*[out, retval]*/ LPOS *plPos)
{
	if( !plPos )
		return E_FAIL;

	*plPos = (LPOS)m_listErr.head();

	return S_OK;
}

HRESULT CTestManager::GetNextError( /*[out]*/ LPOS *plPos, /*[out]*/ TEST_ERR_DESCR **plDescr)
{
	if( !plPos )
		return E_FAIL;

	if( !plDescr )
		return E_FAIL;

	*plDescr = m_listErr.get( (TPOS)*plPos );
	*plPos = (LPOS)m_listErr.next( (TPOS)*plPos );

	return S_OK;
}

HRESULT CTestManager::LoadTests( IStream *pStream, LPOS lNewParentPos )
{
	if( !pStream )
		return E_FAIL;

	ULONG ulRead = 0;
	long lVer = 1;
	
	pStream->Read( &lVer, sizeof( long ), &ulRead );

	long lChilds = 0;
	pStream->Read( &lChilds, sizeof( long ), &ulRead );

	IUnknown *punkRootTest = m_treeTests.get( lNewParentPos );// without addref

	ITestItemPtr sptrRootTest = punkRootTest;
	punkRootTest = 0;

	_bstr_t bstrPath;
	sptrRootTest->GetPath( bstrPath.GetAddress() );

	if( lChilds )
	{
		LPOS lPos = 0;

		do
		{
			IUnknown *punkTest = 0;
			if( ::CoCreateInstance( clsidTestItem, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID *)&punkTest ) != S_OK )
				continue;

			if( !punkTest )
				continue;

			ISerializableObjectPtr sptrSr = punkTest;
			punkTest->Release();

			if( sptrSr == 0 )
				continue;

			sptrSr->Load( pStream, 0 );

			LPOS lParentPos = m_treeTests.insert( punkTest, lNewParentPos ); 
			
			ITestItemPtr sptrTest = sptrSr;

			_bstr_t bstrPathOld;
			sptrTest->GetPath( bstrPathOld.GetAddress() );

			CString strPath = (char*)bstrPathOld;

			int nPos = strPath.ReverseFind( '\\' );

			if( nPos )
			{
				CString strName = strPath.Right( strPath.GetLength() - nPos - 1 );
				sptrTest->SetPath( bstrPath + _bstr_t( "\\" ) + _bstr_t( strName ) );
			}

			LoadTests( pStream, lParentPos );

			pStream->Read( &lPos, sizeof( long ), &ulRead );
		} while( lPos );
	}

	return S_OK;
}

HRESULT CTestManager::StoreTests( IStream *pStream, LPOS lFromParentPos )
{
	if( !pStream )
		return E_FAIL;

	ULONG ulWritten = 0;
	long lVer = 1;
	
	pStream->Write( &lVer, sizeof( long ), &ulWritten );

	LPOS lPos = 0;
	if( GetFirstTestPos( lFromParentPos, &lPos ) != S_OK )
		return E_FAIL;

	pStream->Write( &lPos, sizeof( long ), &ulWritten );

	while( lPos )
	{
		IUnknown *punkTest = 0;

		LPOS lPrev = lPos;
		if( GetNextTest( lFromParentPos, &lPos, &punkTest ) != S_OK )
			return E_FAIL;

		if( !punkTest )
			continue;

		ISerializableObjectPtr sptrSr = punkTest;
		punkTest->Release();

		if( sptrSr == 0 )
			continue;

		sptrSr->Store( pStream, 0 );

		StoreTests( pStream, lPrev );
		pStream->Write( &lPos, sizeof( long ), &ulWritten );
	}

	return S_OK;
}

void CTestManager::_save_environment( IUnknown *punkDoc, CString strPath, CString strName, CString strSuff )
{
	_sync_context( punkDoc );

	_bstr_t dbFileName;
	IFileDataObjectPtr	sptrF = punkDoc;
	IDBConnectionPtr sptrDBCon = sptrF;
	if( sptrDBCon )
		sptrDBCon->GetMDBFileName( dbFileName.GetAddress() );

	_bstr_t bstrPathDB = strPath + "\\" + strName + "_" + (char*)m_strLang + strSuff + ".mdb";
	_bstr_t bstrNameDB = strName + "_" + (char*)m_strLang + strSuff + ".mdb";

	if( sptrDBCon )
	{
		::CopyFile( (char*)dbFileName, (char*)bstrPathDB, FALSE );
		::SetFileAttributes( (char*)bstrPathDB, ::GetFileAttributes( (char*)bstrPathDB ) & ~FILE_ATTRIBUTE_READONLY );
		// [vanek] BT2000:4008 - 20.09.2004
		sptrDBCon->LoadAccessDBase( bstrPathDB );
	}

	CLSID	clsidData = {0};
	if( S_OK == ::CLSIDFromProgID( _bstr_t( "Data.NamedData" ), &clsidData ) )
	{
		IUnknown *punk_vs = 0;
		::CoCreateInstance( clsidData, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&punk_vs );
		if( punk_vs )
		{
		 	// [vanek]: save active view's zoom - 03.09.2004
			if( _save_view_zoom( punkDoc, punk_vs ) )
			{
                // [vanek] : save view's propbags - 06.09.2004
				if( _save_views_propbags( punkDoc, punk_vs ) )
				{
                    IFileDataObjectPtr sptr_fdo = punk_vs;
					if( sptr_fdo != 0)
						sptr_fdo->SaveFile( _bstr_t( strPath + "\\VS" + strName + "_" + (char*)m_strLang + strSuff + ".data" ) );
				}
			}
			punk_vs->Release(); punk_vs = 0;
		}
	}	
		
	if( sptrF )
		sptrF->SaveFile( _bstr_t( strPath + "\\TD" + strName + "_" + (char*)m_strLang + strSuff + ".image" ) );

	if( sptrDBCon )
		sptrDBCon->LoadAccessDBase( dbFileName );

	sptrF = ::GetAppUnknown();
	if( sptrF )
		sptrF->SaveFile( _bstr_t( strPath + "\\SD" + strName + "_"  + (char*)m_strLang + strSuff + ".data" ) );

	IApplicationPtr	ptrA( GetAppUnknown() );
	ptrA->ProcessMessage();

}

void CTestManager::_del_environment( IUnknown *punkDoc, CString strPath, CString strName, CString strSuff  )
{
	::DeleteFile( strPath + CString( "\\TD" ) + strName + "_" + (char*)m_strLang + strSuff + ".image" );
	::DeleteFile( strPath + CString( "\\SD" ) + strName + "_" + (char*)m_strLang + strSuff + ".data" );
	// [vanek] : delete file of views settings
	::DeleteFile( strPath + CString( "\\VS" ) + strName + "_" + (char*)m_strLang + strSuff + ".data" );

	IDBConnectionPtr sptrDBCon = punkDoc;
	if( sptrDBCon )
	{
		_bstr_t bstrPathDB = strPath + "\\" + strName + "_" + (char*)m_strLang + strSuff + ".mdb";
		::DeleteFile( (char*)bstrPathDB );
	}
	IApplicationPtr	ptrA( GetAppUnknown() );
	ptrA->ProcessMessage();
}

void CTestManager::_load_environment( IUnknown *punkDoc, CString strPath, CString strName, CString strSuff  )
{
	IFileDataObjectPtr sptrF = ::GetAppUnknown();
	if( sptrF != 0 )
	{
		sptrF->LoadFile( _bstr_t( strPath + "\\SD" + strName + "_" + (char*)m_strLang + strSuff + ".data" ) );
		_update_system_settings();
	}

	_bstr_t dbFileName;
	sptrF = punkDoc;
	IDBConnectionPtr sptrDBCon = sptrF;
	if( sptrDBCon )
		sptrDBCon->GetMDBFileName( dbFileName.GetAddress() );

	if( sptrF != 0 )
	{
		// [vanek] : open file of views settings - 06.09.2004
		IUnknownPtr sptrunk_vs;
		_bstr_t bstrVSPath( strPath + "\\VS" + strName + "_" + (char*)m_strLang + strSuff + ".data" );
		FILE *fl_vs = fopen( (char *)bstrVSPath, "r" );
        if( fl_vs )
		{	// file exist
			fclose( fl_vs );

			CLSID	clsidData = {0};
			if( S_OK == ::CLSIDFromProgID( _bstr_t( "Data.NamedData" ), &clsidData ) )
			{
				IUnknown *punk_vs = 0;
				::CoCreateInstance( clsidData, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&sptrunk_vs );                        
				IFileDataObjectPtr sptr_fdo = sptrunk_vs;
				if( sptr_fdo != 0 )
					sptr_fdo->LoadFile( bstrVSPath );
			}
		}
        
		// [vanek] : restoring view's propbags before load of document - 06.09.2004
		if( sptrunk_vs != 0 )
			_restore_views_propbags( punkDoc, sptrunk_vs );

        _bstr_t bstrPath( strPath + "\\TD" + strName + "_" + (char*)m_strLang + strSuff + ".image" );
        FILE *fl = fopen( (char *)bstrPath, "r" );
        if( fl )
		{
			fclose( fl );
			sptrF->LoadFile( bstrPath );
		}
		
		// [vanek]: load active view's zoom and set it - 03.09.2004
		if( sptrunk_vs != 0 )
			_restore_view_zoom( punkDoc, sptrunk_vs );       
	}

	if( sptrDBCon )
	{
		_bstr_t bstrPathDB = strPath + "\\" + strName + "_" + (char*)m_strLang + strSuff + ".mdb";
		::CopyFile( (char*)dbFileName, (char*)bstrPathDB, FALSE );
		sptrDBCon->LoadAccessDBase( dbFileName );

		// [vanek] : имитаци€ открыти€ документа Ѕƒ (активизаци€ нужного запроса, вьюхи и т.д. - см. CDBaseDocument::OnNotify) - 20.09.2004
		IDBaseDocumentPtr sptr_dbd = punkDoc;
		if( sptr_dbd )
		{
			_trace_file( "test_sys.log", "[~] TestSystem: on open document" );
			::FireEvent( sptr_dbd, "OnOpenDocument", 0, 0, 0, 0 );
    }
    }

	IApplicationPtr	ptrA( GetAppUnknown() );
	ptrA->ProcessMessage();
	
}

bool CTestManager::_save_view_zoom( IUnknown *punkDoc, IUnknown *punkSaveTo )
{
	if( !punkDoc || !punkSaveTo )
		return false;

	INamedDataPtr sptr_nd = punkSaveTo;
	if( sptr_nd == 0 )
        return false;

    double fzoom = 1.;
	IDocumentSitePtr sptr_doc_site = punkDoc;
	if( sptr_doc_site == 0 )
		return false;
	
	IUnknown *punk_actview = 0;
	sptr_doc_site->GetActiveView( &punk_actview );
	IScrollZoomSitePtr sptr_szs = punk_actview;
	if( punk_actview )
		punk_actview->Release( );	punk_actview = 0;

	if( sptr_szs == 0 )
		return false;
	
	sptr_szs->GetZoom( &fzoom );
	::SetValue( sptr_nd, "\\ActiveViewSettings", "Zoom", fzoom );				
	return true;
}

bool CTestManager::_restore_view_zoom( IUnknown *punkDoc, IUnknown *punkRestoreFrom )
{
	if( !punkDoc || !punkRestoreFrom )
        return false;

	double fzoom = 1.;
	INamedDataPtr sptr_nd = punkRestoreFrom;
	if( sptr_nd == 0 )
		return false;

	fzoom = ::GetValue( sptr_nd, "\\ActiveViewSettings", "Zoom", _variant_t(fzoom) );

	IDocumentSitePtr sptr_doc_site = punkDoc;
	if( sptr_doc_site == 0 )
		return false;

	IUnknown *punk_actview = 0;
	sptr_doc_site->GetActiveView( &punk_actview );
	IScrollZoomSitePtr sptr_szs = punk_actview;
	if( punk_actview )
		punk_actview->Release( );	punk_actview = 0;

	if( sptr_szs != 0 )
		sptr_szs->SetZoom( fzoom );

	return true;
}

bool CTestManager::_save_views_propbags( IUnknown *punkDoc, IUnknown *punkSaveTo )
{
	if( !punkDoc || !punkSaveTo )
		return false;

	IDocumentSitePtr sptr_doc_site = punkDoc;
	if( sptr_doc_site == 0 )
		return false;

	IUnknown* punk = 0;
	sptr_doc_site->GetActiveView( &punk );
	IViewSitePtr	sptr_view_site = punk;	
	if( punk )
		punk->Release( ); punk = 0;

	if( sptr_view_site == 0 )
		return false;
	
	IUnknown*	punk_frame = 0;
	sptr_view_site->GetFrameWindow( &punk_frame );
	sptrIFrameWindow	sptr_frame( punk_frame ) ;
	if( punk_frame )
		punk_frame->Release();

	if( sptr_frame == 0 )
		return false;

	IUnknown* punk_splitter = 0;		
	sptr_frame->GetSplitter( &punk_splitter );

	ISplitterWindowPtr sptr_splitter = punk_splitter;
	if( punk_splitter )
		punk_splitter->Release(); punk_splitter = 0;

	if( sptr_splitter == 0 )
		return false;
	
	int nrow_count = 0,
		ncol_count = 0;	
    sptr_splitter->GetRowColCount( &nrow_count, &ncol_count);

	CString str_path( _T("\\ViewsPropBags") );
	::SetValue( punkSaveTo,  str_path, "RowCount", _variant_t(nrow_count) );
	::SetValue( punkSaveTo,  str_path, "ColCount", _variant_t(ncol_count) );

    for( int nrow = 0; nrow < nrow_count; nrow ++ )
	{
		for( int ncol = 0; ncol < ncol_count; ncol ++ )
		{	// store view's named propbag to doc's named data

			// get view by row, col
			IUnknownPtr  sptr_view = 0;
			sptr_splitter->GetViewRowCol( nrow, ncol, &sptr_view );			
			
            INamedPropBagSerPtr sptr_vew_pbser = sptr_view;
            if( sptr_vew_pbser == 0 )			
				continue;

			// create IStream* from global memory
			IStreamPtr sptr_stm;
			if( S_OK != CreateStreamOnHGlobal( NULL, TRUE, &sptr_stm ) )
				continue;

			// serialize named propbag to stream
            if( S_OK != sptr_vew_pbser->Store( sptr_stm ) )
				continue;       		

			byte *pbuffer = 0;
			long lbuff_size = 0;
            
			STATSTG st_stat = {0};
			sptr_stm->Stat( &st_stat, STATFLAG_NONAME );
			lbuff_size = st_stat.cbSize.LowPart;

			// move to begin
			LARGE_INTEGER liseek = {0};
			sptr_stm->Seek( liseek, STREAM_SEEK_SET, NULL );	

            pbuffer = new byte[ lbuff_size ];
			// read to buffer
            sptr_stm->Read( pbuffer, lbuff_size, NULL);
            						            
			CString	str_entry( _T("") );
			str_entry.Format( "View%d%d", nrow, ncol );
			::SetValue( punkSaveTo, str_path, str_entry, pbuffer, lbuff_size );

			if( pbuffer )
				delete[] pbuffer; pbuffer = 0;
		}
	}

	return true;
}

bool CTestManager::_restore_views_propbags( IUnknown *punkDoc, IUnknown *punkRestoreFrom )
{
	if( !punkDoc || !punkRestoreFrom )
		return false;

	IDocumentSitePtr sptr_doc_site = punkDoc;
	if( sptr_doc_site == 0 )
		return false;

	IUnknown* punk = 0;
	sptr_doc_site->GetActiveView( &punk );
	IViewSitePtr	sptr_view_site = punk;	
	if( punk )
		punk->Release( ); punk = 0;

	if( sptr_view_site == 0 )
		return false;
	
	IUnknown*	punk_frame = 0;
	sptr_view_site->GetFrameWindow( &punk_frame );
	sptrIFrameWindow	sptr_frame( punk_frame ) ;
	if( punk_frame )
		punk_frame->Release();

	if( sptr_frame == 0 )
		return false;

	IUnknown* punk_splitter = 0;		
	sptr_frame->GetSplitter( &punk_splitter );

	ISplitterWindowPtr sptr_splitter = punk_splitter;
	if( punk_splitter )
		punk_splitter->Release(); punk_splitter = 0;

	if( sptr_splitter == 0 )
		return false;
	
	int nrow_count = 0,
		ncol_count = 0;	
    sptr_splitter->GetRowColCount( &nrow_count, &ncol_count);

	CString str_path( _T("\\ViewsPropBags") );
    nrow_count = min( nrow_count, (int)(::GetValue(punkRestoreFrom,  str_path, "RowCount", _variant_t(int(0)))) );
	ncol_count = min( ncol_count, (int)(::GetValue(punkRestoreFrom,  str_path, "ColCount", _variant_t(int(0)))) );

    for( int nrow = 0; nrow < nrow_count; nrow ++ )
	{
		for( int ncol = 0; ncol < ncol_count; ncol ++ )
		{	// restore view's named propbag form doc's named data

			// get view by row, col
			IUnknownPtr  sptr_view = 0;
			sptr_splitter->GetViewRowCol( nrow, ncol, &sptr_view );			
			INamedPropBagSerPtr sptr_vew_pbser = sptr_view;
            if( sptr_vew_pbser == 0 )			
				continue;
            
			CString	str_entry( _T("") );
			str_entry.Format( "View%d%d", nrow, ncol );
			byte *pbuffer = 0;
			long lbuff_size = 0;
			pbuffer = ::GetValuePtr( punkRestoreFrom, str_path, str_entry, &lbuff_size );
            if( !pbuffer )                     
				continue;

			// alloc memory based on buffer's size
			HGLOBAL hglobal = 0;
			hglobal = ::GlobalAlloc(GMEM_MOVEABLE, lbuff_size);
			if( hglobal )
			{
				void *pvdata = 0;
				pvdata = GlobalLock(hglobal);
				if( pvdata )
					::memcpy( pvdata, pbuffer, lbuff_size );
				GlobalUnlock(hglobal);

				IStreamPtr sptr_stm;
				// create IStream* from global memory
				if( S_OK == CreateStreamOnHGlobal( hglobal, TRUE, &sptr_stm ) )
					sptr_vew_pbser->Load( sptr_stm );	// load propbag from stream
			}

            delete[] pbuffer; pbuffer = 0;
		}
	}

    return true;        
}

HRESULT CTestManager::GetDocument( IUnknown **punkDoc )
{
	if( !m_bRunning )
		return E_FAIL;

	if( !punkDoc )
		return E_FAIL;

	if( m_sptrHiddenDoc == 0 )
		return E_FAIL;

	*punkDoc = m_sptrHiddenDoc;
	(*punkDoc)->AddRef();

	return S_OK;
}

void CTestManager::_update_system_settings()
{
	IApplicationPtr	ptrA( GetAppUnknown() );

	_trace_file( "test_sys.log", "[~] TestSystem: New Settings" );
	::FireEvent( ptrA, szEventNewSettings, 0, 0, 0, 0 );

	LONG_PTR	lposTempl = 0;

	ptrA->GetFirstDocTemplPosition( &lposTempl );

	while( lposTempl )
	{
		LONG_PTR	lposDoc = 0;
		ptrA->GetFirstDocPosition( lposTempl, &lposDoc );


		while( lposDoc )
		{
			IUnknown	*punk = 0;
			ptrA->GetNextDoc( lposTempl, &lposDoc, &punk );

			::FireEvent( punk, szEventNewSettings, 0, 0, 0, 0 );

			if( punk )
				punk->Release();
		}
		ptrA->GetNextDocTempl( &lposTempl, 0, 0 );
	}
	
	ptrA->ProcessMessage();
}

HRESULT CTestManager::_create_hidden_doc()
{
	if( !m_bStoreToHidden )
		return E_FAIL;

	IUnknown *punkHiddenDoc = 0;

	CLSID	clsidData = {0};

	if( ::CLSIDFromProgID( _bstr_t( "Data.NamedData" ), &clsidData ) != S_OK  )
		return E_FAIL;

	if( ::CoCreateInstance( clsidData, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punkHiddenDoc ) != S_OK )
		return E_FAIL;

	m_sptrHiddenDoc = punkHiddenDoc;

	INamedDataPtr sptrND = m_sptrHiddenDoc;
	sptrND->EnableBinaryStore( TRUE );

	if( punkHiddenDoc )
		punkHiddenDoc->Release();

	return S_OK;
}

LPSTREAM _AfxCreateMemoryStream()
{
	LPSTREAM lpStream = NULL;

	// Create a stream object on a memory block.
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, 0);
	if (hGlobal != NULL)
	{
		if (FAILED(CreateStreamOnHGlobal(hGlobal, TRUE, &lpStream)))
		{
			GlobalFree(hGlobal);
			return NULL;
		}
	}
	else
		return 0;

	return lpStream;
}

void CTestManager::_copy_data_to_hidden()
{
	if( !m_bStoreToHidden )
		return;

	IUnknown *punkDoc = 0;

	IApplicationPtr	sptrApp = GetAppUnknown();
	sptrApp->GetActiveDocument( &punkDoc );

	if( punkDoc )
	{
		IDataContext2Ptr sptrDocCXT = punkDoc;
		punkDoc->Release(); punkDoc = 0;

		LONG_PTR lPosDoc = 0;
		sptrDocCXT->GetFirstObjectPos( _bstr_t( szTypeTestDataArray ), &lPosDoc );

		while( lPosDoc )
		{
			IUnknown *punkObject = 0;
			sptrDocCXT->GetNextObject( _bstr_t( szTypeTestDataArray ), &lPosDoc, &punkObject );

			if( punkObject )
			{
				INamedDataObject2Ptr	sptrN = punkObject;
				punkObject->Release(); punkObject = 0;

				if( sptrN )
				{
					IUnknown *punkArray = ::CreateTypedObject( _bstr_t( szTypeTestDataArray ) );

					ISerializableObjectPtr sptrSer = punkArray;
					if( punkArray )
						punkArray->Release(); punkArray = 0;

					ISerializableObjectPtr sptrSerSrc = sptrN;

					IStream *punkStream = _AfxCreateMemoryStream();

					if( punkStream )
					{
						sptrSerSrc->Store( punkStream, 0 );

						LARGE_INTEGER liOff = {0};
						LISet32(liOff, 0);

						punkStream->Seek( liOff, 0, 0 );
						sptrSer->Load( punkStream, 0 );
						punkStream->Release();

						long nCount = ::GetValueInt( GetAppUnknown(), "\\types", "Test Data Array", 0L );
						::SetValue( GetAppUnknown(), "\\types", "Test Data Array", 0L );

						::SetValue( m_sptrHiddenDoc, 0,0, _variant_t( (IUnknown*)sptrSer ) );

						::SetValue( GetAppUnknown(), "\\types", "Test Data Array", nCount );
					}
				}
			}
		}
	}
}

void CTestManager::_sync_context( IUnknown *punkDoc )
{
	IDocumentSitePtr sptrDocSite = punkDoc;

	if( sptrDocSite == 0 )
		return;

	IUnknown *punkView = 0;
	sptrDocSite->GetActiveView( &punkView );

	IDataContext3Ptr ptrContextView = punkView;
	IDataContext3Ptr ptrContextDoc = punkDoc;

	if( punkView )
		punkView->Release(); punkView = 0;

	IDataTypeManagerPtr	ptrMan = punkDoc;

	long	lTypesCount = 0;
	ptrMan->GetTypesCount( &lTypesCount );

	BOOL	bOldLock = 0;

	ptrContextDoc->GetLockUpdate( &bOldLock );
	ptrContextDoc->LockUpdate( true, false );

	for( int nType = 0; nType < lTypesCount; nType++ )
	{
		_bstr_t	bstrType;
		ptrMan->GetType( nType, bstrType.GetAddress() );

		IUnknown	*punkObject = 0;
		ptrContextView->GetActiveObject( bstrType, &punkObject );
		ptrContextDoc->SetActiveObject( bstrType, punkObject, 0 );

		if( punkObject )
			punkObject->Release();
	}

	ptrContextDoc->LockUpdate( bOldLock, true );
}

HRESULT CTestManager::GetCombinationCont( LPOS lPos, long *plCount )
{
	IUnknown *punkTRD = 0;
	GetNextTest( 0, &lPos, &punkTRD);

	ITestRunningDataPtr sptrTRD	=	punkTRD;

	if( punkTRD )
		punkTRD->Release(); punkTRD = 0;

	if( sptrTRD )
		sptrTRD->GetCombinationCont( plCount );

	return S_OK;
}

HRESULT CTestManager::GetTestCont( LPOS lParentPos, LPOS lPos, long *plCount )
{
	long lCount = 0;
	IUnknown *punkTRD = 0;
	LPOS lCurPos = lPos;
	GetNextTest( 0, &lPos, &punkTRD);

	ITestRunningDataPtr sptrTRD	=	punkTRD;

	if( punkTRD )
		punkTRD->Release(); punkTRD = 0;

	if( sptrTRD )
		sptrTRD->GetCombinationCont( &lCount );

	if( !lCount )
		lCount++;

	LPOS lChildPos = 0;
	GetFirstTestPos( lCurPos, &lChildPos );

	long lChildCnd = 0;
	while( lChildPos )
	{
		long lCnt = 0;
		GetTestCont( lCurPos, lChildPos, &lCnt );

		lChildCnd += lCnt;

		IUnknown *punkT = 0;
		GetNextTest( lCurPos, &lChildPos, &punkT );

		if( punkT )
			punkT->Release();
	}

	if( lChildCnd )
	{
		if( lCount > 1 )
			*plCount = lChildCnd * lCount + lCount;
		else
			*plCount = lChildCnd + lCount;
	}
	else
		*plCount = lCount;

	return S_OK;
}

HRESULT CTestManager::GetTestContR( LPOS lParentPos, LPOS lPos, long *plCount )
{
	long lCount = 1;

	LPOS lCurPos = lPos;
	LPOS lChildPos = 0;
	GetFirstTestPos( lCurPos, &lChildPos );

	long lChildCnd = 0;
	while( lChildPos )
	{
		long lCnt = 0;
		GetTestContR( lCurPos, lChildPos, &lCnt );

		lCount += lCnt;

		IUnknown *punkT = 0;
		GetNextTest( lCurPos, &lChildPos, &punkT );

		if( punkT )
			punkT->Release();
	}

	*plCount = lCount;

	return S_OK;
}

HRESULT CTestManager::GetLang( /*out*/ BSTR *pbstrLang )
{
    if( !pbstrLang )
		return E_INVALIDARG;

	*pbstrLang = m_strLang.copy( );
	return S_OK;
}

void CTestManager::_save_tests_environment( LPCTSTR lpPath, LPCTSTR lpFileName, DWORD dwFlags )
{
	if( !_tcsclen( lpPath ) || !_tcsclen( lpFileName ) )
		return;

    CString str_file_path( _T("") );

	// 1. —охранение shell.data и классификаторов
	if( dwFlags & tefSDC )
	{
		//str_file_path = strPath + _T("\\"szTestTempEverychildName".data");	
		str_file_path.Format( "%s\\%s.data", lpPath, lpFileName);	
		IFileDataObjectPtr	sptrF = ::GetAppUnknown();
		if( sptrF )
		{
			if( S_OK == sptrF->SaveFile( _bstr_t(str_file_path) ) )
				m_mapTmpFiles.set( str_file_path, str_file_path );	// добавл€ем в список временных файлов
		}
	
		CString str_cls_path( _T("") );
		str_cls_path = (TCHAR *)(::GetValueString( ::GetAppUnknown(), "\\Paths", "Classes", "" ));
		if( !str_cls_path.IsEmpty() )
		{
			str_file_path.Format( "%s\\%s.stg", lpPath, lpFileName);	
			//str_file_path = strPath + _T("\\"szTestTempEverychildName".stg");	
			CFileStorageHelper hf;	
			if( S_OK == hf.CreateStorage( str_file_path ) )
			{
				if( S_OK == hf.FillByPath( str_cls_path ) )
					m_mapTmpFiles.set( str_file_path, str_file_path );	// добавл€ем в список временных файлов
				else
				{
					hf.CloseStorage( );
					::DeleteFile( str_cls_path );                            
				}
			}
		}
	}

	// 2. —охранение раскладки клавиатуры в файл с настройками
	if( dwFlags & tefKbdLayout )
	{
		str_file_path.Format( "%s\\%s.ini", lpPath, lpFileName);	

		CString strKeyb( _T("") );
		::GetKeyboardLayoutName( strKeyb.GetBuffer( 100 ) );
		strKeyb.ReleaseBuffer();
		if( ::WritePrivateProfileString( _T("misc"), _T("KbdLayoutName"), strKeyb, str_file_path ) )
			m_mapTmpFiles.set( str_file_path, str_file_path );	// добавл€ем в список временных файлов
	}

	// 3. —охранние state
	if( dwFlags & tefState )
	{
		str_file_path.Format( "%s\\%s.state", lpPath, lpFileName);	
		long lbackup_old = 0;
		lbackup_old = GetValueInt( GetAppUnknown(), "General", "BackupStates", 0 );
		SetValue( GetAppUnknown(), "General", "BackupStates", (long)(0));
		if( ::ExecuteAction( "ToolsSaveAsState", CString( "\"" ) + str_file_path + "\""  ) )
			m_mapTmpFiles.set( str_file_path, str_file_path );	// добавл€ем в список временных файлов
		SetValue( GetAppUnknown(), "General", "BackupStates", lbackup_old);
	}
}

void CTestManager::_load_tests_environment( LPCTSTR lpPath, LPCTSTR lpFileName, DWORD dwFlags )
{
	if( !_tcsclen( lpPath ) || !_tcsclen( lpFileName ) )
		return;

	CString str_file_path( _T("") );

	// 1. «агрузка shell.data и классификаторов
	if( dwFlags & tefSDC )
	{
		//str_file_path = strPath + _T("\\"szTestTempEverychildName".data");	
		str_file_path.Format( "%s\\%s.data", lpPath, lpFileName);	
		if( m_mapTmpFiles.find( str_file_path ) )
		{
			IFileDataObjectPtr sptrF = ::GetAppUnknown();
			if( sptrF != 0 )
			{
				sptrF->LoadFile( _bstr_t( str_file_path ) );
				_update_system_settings();
			}
		}
	
		//str_file_path = strPath + _T("\\"szTestTempEverychildName".stg");	
		str_file_path.Format( "%s\\%s.stg", lpPath, lpFileName);	
		if( m_mapTmpFiles.find( str_file_path ) )
		{
			HRESULT hr = S_FALSE;
			CString str_classes_path(_T(""));

			str_classes_path = (TCHAR *)::GetValueString( ::GetAppUnknown(), "\\Paths", "Classes", "" );
			if( !str_classes_path.IsEmpty( ) )
			{
				int nlength = 0;
				nlength = str_classes_path.GetLength( );
				if( _T('\\') == str_classes_path.GetAt( nlength - 1 ) )
					str_classes_path.Delete( nlength - 1, 1 );

				// -> rename current directory of classes
				if( 0 == _trename( str_classes_path, str_classes_path + _T("_bak") ) )
				{
					// -> load from storage
					CFileStorageHelper filehlp;	
					if( S_OK == (hr = filehlp.OpenStorage( str_file_path )) )      
						hr = filehlp.FlushContent( str_classes_path );                        		
				}
			}

			// если все успешно прошло - удал€ем директорию "<classes_path>_bak", иначе - откатываемс€ назад: удал€ем
			// директорию "<classes_path>" и переименовываем дирректорию "<classes_path>_bak" в "<classes_path>"
			SHFILEOPSTRUCT flStruct = { 0 };
			flStruct.wFunc = FO_DELETE;
			TCHAR buf[_MAX_PATH + 1];
			_tcscpy( buf, hr == S_OK ? str_classes_path + _T("_bak") : str_classes_path );
			buf[_tcslen(buf)+1] = 0;
			flStruct.pFrom = buf;
			flStruct.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
			::SHFileOperation( &flStruct );
			if( hr != S_OK )
				_trename( str_classes_path + _T("_bak"), str_classes_path ); 
		}
	}

	// 2. «агрузка раскладки клавиатуры
	if( dwFlags & tefKbdLayout )
	{
		str_file_path.Format( "%s\\%s.ini", lpPath, lpFileName);
		if( m_mapTmpFiles.find( str_file_path ) )
		{
			CString str_kbdlayout( _T("") );
			::GetPrivateProfileString( _T("misc"), _T("KbdLayoutName"), _T(""), 
				str_kbdlayout.GetBuffer( 100 ), 100, str_file_path );
			str_kbdlayout.ReleaseBuffer( );
			if( !str_kbdlayout.IsEmpty() )
				::LoadKeyboardLayout( str_kbdlayout, KLF_ACTIVATE );
		}
	}

	// 3. «агрузка state
	if( dwFlags & tefState )
	{
		//str_file_path = strPath + _T("\\"szTestTempEverychildName".state");	
		str_file_path.Format( "%s\\%s.state", lpPath, lpFileName);	
		if( m_mapTmpFiles.find( str_file_path ) )
			_load_state_and_update( str_file_path );
	}
}

void CTestManager::_del_tests_environment( LPCTSTR lpPath, LPCTSTR lpFileName, DWORD dwFlags )
{
	if( !lpPath || !lpFileName )
		return;

	if( !_tcsclen( lpPath ) || !_tcsclen( lpFileName ) )
		return;

	CString str_file_path( _T("") );

    // 1. ”даление  state
	if( dwFlags & tefState )
	{
		str_file_path.Format( "%s\\%s.state", lpPath, lpFileName);
		m_mapTmpFiles.remove_key( str_file_path );
	}
    
	// 2. ”даление shell.data и классификаторов
	if( dwFlags & tefSDC )
	{
		str_file_path.Format( "%s\\%s.data", lpPath, lpFileName);
		m_mapTmpFiles.remove_key( str_file_path );

		str_file_path.Format( "%s\\%s.stg", lpPath, lpFileName);
		m_mapTmpFiles.remove_key( str_file_path );			
	}

	// 4. ”даление файла с настройками: раскладка клавы
	if( dwFlags & tefKbdLayout)	
	{
        str_file_path.Format( "%s\\%s.ini", lpPath, lpFileName);
		m_mapTmpFiles.remove_key( str_file_path );
	}

}