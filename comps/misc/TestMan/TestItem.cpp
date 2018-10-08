#include "StdAfx.h"
#include "testitem.h"
#include "data5.h"
#include "resource.h"

#include "shellapi.h"

#include "core5.h"
#include "misc_utils.h"

#define SECTION_COND "\\Condition"
#define SECTION_EXEC_COND "\\ExecCondition"
#define KEY_COND_CNT "ConditionCount"
#define KEY_COND "Condition"

#define SECTION_ALG "\\Algorithm"
#define KEY_ALG_SCRIPT "Script"

#define SECTION_ADD "\\AdditionInfo"
#define KEY_ADD_RUNDATE "RunDate"
#define KEY_ADD_SAVEDATE "SaveDate"
#define KEY_ADD_SAVED "TestDataSaved"
#define KEY_ADD_ERROR "TestDataError"
#define KEY_TIME_RUN "TestRunningTime"
#define KEY_ADDITONAL "AdditionalInfo"
#define KEY_MANUAL_CORRECTION "ManualCorrection"
#define KEY_ENV_LOADFLAGS "EnvLoadFlags"
#define	KEY_ENV_KBDLAYOUT "KbdLayout"

#include "\vt5\common\misc_str.h"
#include "nameconsts.h"

#include "misc_utils.h"

#include "\vt5\comps\general\data\misc_types.h"

#include "FileStorageHelper.h"

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

static void _update_system_settings()
{
	IApplicationPtr	ptrA( GetAppUnknown() );

	FireEvent( ptrA, szEventNewSettings, 0, 0, 0, 0 );

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

static bool _get_test_lang( BSTR *pbstr_lang )
{
	if( !pbstr_lang )
		return false;

	ITestManagerPtr sptr_testman;
	IUnknown *punk_testman = _GetOtherComponent(GetAppUnknown(), IID_ITestManager); 
	sptr_testman = punk_testman;
	if( punk_testman )
		punk_testman->Release(); punk_testman = 0;

	if( sptr_testman == 0 )
		return false;

    return S_OK == sptr_testman->GetLang( pbstr_lang );
}

bool _isfileexist( LPCTSTR lpFileName )
{
	HANDLE	hfile = ::CreateFile( lpFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,  
		FILE_ATTRIBUTE_NORMAL, 0 );

	if( hfile == INVALID_HANDLE_VALUE )
	{
		DWORD	dw = ::GetLastError();
		return false;
	}

	::CloseHandle( hfile );
	return true;
}

static bool _store_file( IStream *pstream, LPCTSTR lpFileName, ULONG *pulWritten )
{
	if( !pstream || !lpFileName )
		return false;

	DWORD	dwbuff_size = 0,
			dwread = 0;
	BYTE	*pbuffer = 0;

	HANDLE	hfile = 0;
	hfile = ::CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hfile == INVALID_HANDLE_VALUE)
		return false;

	dwbuff_size = ::GetFileSize( hfile, NULL );
	if( dwbuff_size == INVALID_FILE_SIZE )
	{
		::CloseHandle( hfile );
		return false;
	}

	BY_HANDLE_FILE_INFORMATION stinfo = {0};
	::GetFileInformationByHandle( hfile, &stinfo );

	pbuffer = new BYTE[ dwbuff_size ];
	::ZeroMemory( pbuffer, dwbuff_size );		   

	BOOL bread_comlete = FALSE;
	bread_comlete = ::ReadFile( hfile, (LPVOID)(pbuffer), dwbuff_size, &dwread, NULL);
	if( bread_comlete && (dwbuff_size == dwread) )
	{
		// store attributes
		pstream->Write( &stinfo.dwFileAttributes, sizeof(stinfo.dwFileAttributes), pulWritten );

		// store time of creation
		pstream->Write( &stinfo.ftCreationTime, sizeof(stinfo.ftCreationTime), pulWritten );

		// store time of last access
		pstream->Write( &stinfo.ftLastAccessTime, sizeof(stinfo.ftLastAccessTime), pulWritten );

		// store time of last write
		pstream->Write( &stinfo.ftLastWriteTime, sizeof(stinfo.ftLastWriteTime), pulWritten );

		// store size
		pstream->Write( &dwbuff_size, sizeof(dwbuff_size), pulWritten );

		// store content        
		pstream->Write( pbuffer, dwbuff_size, pulWritten );
	}

	if( pbuffer )
		delete[] pbuffer; pbuffer = 0;

    ::CloseHandle( hfile );
	return true;
}

// load file
static bool _load_file( IStream *pstream, LPCTSTR lpCreateFileName, CFileInMem *pDest, ULONG *pulRead )
{
	if( !pstream || !pDest )
		return false;

	bool bret_val = false;
    DWORD	dwbuff_size = 0,
			dwwritten = 0;
	BYTE	*pbuffer = 0;
	
	pDest->Empty( );
							   
	// read attributes
	pstream->Read( &pDest->m_dwFileAttributes, sizeof(pDest->m_dwFileAttributes), pulRead );
		
	// read time of creation
	pstream->Read( &pDest->m_ftCreationTime, sizeof(pDest->m_ftCreationTime), pulRead );

	// read time of last access
	pstream->Read( &pDest->m_ftLastAccessTime, sizeof(pDest->m_ftLastAccessTime), pulRead );
		
	// read time of last write
	pstream->Read( &pDest->m_ftLastWriteTime, sizeof(pDest->m_ftLastWriteTime), pulRead );
		    
	// read size
	pstream->Read( &dwbuff_size, sizeof(dwbuff_size), pulRead );

	pDest->m_bytes.alloc( (int)(dwbuff_size) );

	// read content
	pstream->Read( pDest->m_bytes, dwbuff_size, pulRead );
	
	if( lpCreateFileName )
		pDest->Flush( lpCreateFileName );
    
	return true;    
}

//// class CFileInMem
CFileInMem::CFileInMem(void)
{
    Empty( );
}

CFileInMem::~CFileInMem(void)
{
    Empty( );
}

void	CFileInMem::Empty( )
{
	m_dwFileAttributes = 0;
	::ZeroMemory( &m_ftCreationTime, sizeof(m_ftCreationTime) );
	::ZeroMemory( &m_ftLastAccessTime, sizeof(m_ftLastAccessTime) );
	::ZeroMemory( &m_ftLastWriteTime, sizeof(m_ftLastWriteTime) );
	m_bytes.free( );
}

bool	CFileInMem::IsEmpty( )
{
	return !m_bytes.size();
}

bool	CFileInMem::Flush( LPCTSTR lpfilename )
{
    if( !lpfilename || IsEmpty( ) )  
		return false;

	// open file
    HANDLE	hfile = 0;
	hfile = ::CreateFile(lpfilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 
		m_dwFileAttributes ? m_dwFileAttributes : FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hfile == INVALID_HANDLE_VALUE)
		return false;
	
	// write data
	DWORD dwwritten = 0;
	::WriteFile( hfile, (LPVOID)(m_bytes), m_bytes.size(), &dwwritten, NULL );
    
	// set times
	::SetFileTime( hfile, &m_ftCreationTime, &m_ftLastAccessTime, &m_ftLastWriteTime );
    
	// close file
	::CloseHandle( hfile );
    hfile = 0;

	return dwwritten == m_bytes.size( );
}


//// class CTestItem
CTestItem::CTestItem(void) : IDispatchImpl<ITestItemDisp>( "TestMan.tlb" )
{
	m_strPath.Empty();
	m_strInfo.Empty();
	_deinit();
}

CTestItem::~CTestItem(void)
{
}

void CTestItem::_deinit()
{
	m_strScript.Empty();
	m_strScriptFinal.Empty();
	m_strInfo.Empty();

	m_lstCond.clear();
	m_lstExecCond.clear();

	m_lCombCounter = 0;
	m_date = 0;
	m_date_save = 0;

	m_bChecked = 0;
	m_bSaved = 0;
	m_dwTime = 0;
	m_bError = 0;
	m_bManualCorr = FALSE;

	m_bLoadShellState = FALSE;
	m_bLoadSDC = FALSE;
}


IUnknown *CTestItem::DoGetInterface( const IID &iid )
{
	if( iid == __uuidof( ITestItem ) )
		return (ITestItem *)this;
	else if( iid == __uuidof( ITestRunningData ) )
		return (ITestRunningData *)this;
	else if( iid == IID_IDispatch ) 
		return (IDispatch *)this;	
	else if( iid == IID_ISerializableObject )
		return (ISerializableObject *)this;
	else if( iid == IID_ITestEnvironment )
		return (ITestEnvironment *)this;

	return ComObjectBase::DoGetInterface( iid );
}

HRESULT CTestItem::SetPath( /*[in]*/ BSTR bstrPath )
{
	m_strPath = bstrPath;
	return S_OK;
}

HRESULT CTestItem::GetPath( /*[out, retval]*/ BSTR *pbstrPath )
{
	if( !pbstrPath )
		return E_FAIL;
	
	*pbstrPath = m_strPath.AllocSysString();
	return S_OK;
}

HRESULT CTestItem::Rename( /*[in]*/ BSTR bstrNewName )
{
	int nIndex = m_strPath.ReverseFind( '\\' );
	CString strNewPath( _T("") );
	strNewPath = m_strPath.Left( nIndex );
	strNewPath = strNewPath + "\\" + CString( bstrNewName );
    if( 0 != _trename( m_strPath, strNewPath ) )
		return S_FALSE; 

	// [vanek] : утанавливаем новый путь, если переименование завершилось успешно - 21.07.2004
	m_strPath = strNewPath;
	return S_OK;    
}

HRESULT CTestItem::SetScript( /*[in]*/ BSTR bstrScript )
{
	m_strScript = bstrScript;
	return S_OK;
}

HRESULT CTestItem::GetScript( /*[out, retval]*/ BSTR *pbstrScript )
{
	if( !pbstrScript )
		return E_FAIL;
	
	*pbstrScript = m_strScript.AllocSysString();

	return S_OK;
}

HRESULT CTestItem::GetFirstCondPos( /*[out, retval]*/ LPOS *lpPos)
{
	if( !lpPos )
		return E_FAIL;

	*lpPos = (LPOS)m_lstCond.head();

	return S_OK;
}

HRESULT CTestItem::GetNextCond( /*[out]*/ VARIANT *plPos, /*[out, retval]*/ BSTR *pbstr )
{
	if( !plPos )
		return E_FAIL;

	//if( plPos->vt != VT_UINT_PTR )
	//	return E_FAIL;

	return GetNextCond( (LPOS*)&plPos->pulVal, pbstr );
}

HRESULT CTestItem::GetNextCond( /*[out]*/ LPOS *plPos, /*[out, retval]*/ BSTR *pbstr)
{
	if( !plPos )
		return E_FAIL;
	if( !pbstr )
		return E_FAIL;


	CString str = m_lstCond.get( (TPOS)*plPos );
	*plPos = (LPOS)m_lstCond.next((TPOS)*plPos);
	*pbstr = str.AllocSysString();

	return S_OK;
}

HRESULT CTestItem::AddCond( /*[in]*/ BSTR bstrCond )
{
	if( !bstrCond )
		return E_FAIL;
	
	m_lstCond.add_tail( CString( bstrCond ) );

	return S_OK;
}

HRESULT CTestItem::DeleteCond( /*[in]*/ LPOS lPos)
{
	if( !lPos )
		return E_FAIL;
	
	m_lstCond.remove( (TPOS)lPos );

	return S_OK;
}

HRESULT CTestItem::DeleteAllCond()
{
	m_lstCond.clear();
	// [vanek] : clear all conditions - 26.05.2004
	m_lstParsedMultyCond.clear();
	m_lstParsedSingleCond.clear();
	return S_OK;
}

// interface ITestRunningData
HRESULT CTestItem::GetRunTime( /*[out, retval]*/ DATE *pDate )
{
	if( !pDate)
		return E_FAIL;

	*pDate = m_date;

	return S_OK;
}

HRESULT CTestItem::SetRunTime( /*[in]*/ DATE Date )
{
	m_date = Date;
	return S_OK;
}

HRESULT CTestItem::GetSaveTime( /*[out, retval]*/ DATE *pDate )
{
	if( !pDate)
		return E_FAIL;

	*pDate = m_date_save;

	return S_OK;
}

HRESULT CTestItem::SetSaveTime( /*[in]*/ DATE Date )
{
	m_date_save = Date;
	return S_OK;
}

HRESULT CTestItem::SetTestChecked( /*[in]*/ long bSet )
{
	m_bChecked = BOOL( bSet != 0 );
	return S_OK;
}

HRESULT CTestItem::GetTestChecked( /*[out, retval]*/ long *pbSet )
{
	if( !pbSet )
		return E_FAIL;

	*pbSet = long( m_bChecked );
	return S_OK;
}

HRESULT CTestItem::KillThemselvesFromDisk()
{
	if( m_strPath.IsEmpty() )
		return E_FAIL;

	SHFILEOPSTRUCT flStruct = { 0 };

	flStruct.wFunc = FO_DELETE;
	// пошаманим :))
	TCHAR buf[_MAX_PATH + 1];	// allow one more character
    _tcscpy(buf, m_strPath/* + _T("\\*.*")*/);			// copy caller's path name
    buf[_tcslen(buf)+1]=0;		// need two NULLs at end
	flStruct.pFrom = buf;
	flStruct.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;

	::SHFileOperation( &flStruct );

	//::RemoveDirectory( m_strPath );

	return S_OK;
}

void CTestItem::_calc_excl_equal_count( _list_t< X_PARSED_DATA> *pLst, int nId, long *plCounter )
{
	static _list_t< _variant_t > vList;
	long i = 0;
	for (TPOS lPos = pLst->head(); lPos; lPos = pLst->next(lPos), i++)
	{
		if( i == nId )
		{
			X_PARSED_DATA &Data = pLst->get( lPos );
			for( long q = 0; q < Data.lArgCount; q++ )
			{
			
				if( !pLst->count() )
				{
					vList.add_tail( Data.pvarArgs[q] );
					_calc_excl_equal_count( pLst, nId - 1, plCounter );
					vList.remove( vList.tail() );
				}
				else
				{
					bool bFound = false;
					for (TPOS lPos2 = vList.head(); lPos2; lPos2 = vList.next(lPos2))
					{
						_variant_t var = vList.get( lPos2 );
						if( Data.pvarArgs[q] == var )
						{
							bFound = true;
							break;
						}
					}
					if( !bFound )
					{
						if( i )
						{
							vList.add_tail( Data.pvarArgs[q] );
							_calc_excl_equal_count( pLst, nId - 1, plCounter );
							vList.remove( vList.tail() );
						}
						else
							(*plCounter)++;
					}
				}
			}
		}
	}

}

// [vanek] - 05.08.2004
bool	CTestItem::_serialize_environment( IStream *pStream, bool bstore /*= true*/, ULONG *pulRW /*= 0*/ )
{
	if( bstore )        
	{	// storing
		if( m_strPath.IsEmpty() )
			return false;

		CString str_file_path( _T("") );

		// 1. Storing state
		// storing option of load state
		pStream->Write( &m_bLoadShellState, sizeof( m_bLoadShellState ), pulRW ); 
		
		_bstr_t bstr_lang;
		if( !_get_test_lang( bstr_lang.GetAddress() ) )
			return false;

		// storing state's files
		str_file_path = m_strPath + _T("\\"szTestSavedStateName"_") + (LPTSTR)(bstr_lang) + _T(".state");		
		bool	bfile_exist = false;
		bfile_exist = _isfileexist( str_file_path );
		// storing file's existence
		pStream->Write( &bfile_exist, sizeof(bfile_exist), pulRW);
        if( bfile_exist )
		{
			// storing file
			if( !_store_file( pStream, str_file_path, pulRW ) )
				return false;
		}

		str_file_path = m_strPath + _T("\\"szTestSavedStateName"_") + (LPTSTR)(bstr_lang) + _T(".tbbmp");
		bfile_exist = false;
		bfile_exist = _isfileexist( str_file_path );
		// storing file's existence
		pStream->Write( &bfile_exist, sizeof(bfile_exist), pulRW );
        if( bfile_exist )
		{
			// storing file
			if( !_store_file( pStream, str_file_path, pulRW ) )
				return false;
		}
		
		// 2. Storing shell data and classes	
		// storing option of load shell data and classes 
		pStream->Write( &m_bLoadSDC, sizeof( m_bLoadSDC), pulRW ); 

        // storing shell data
        str_file_path = m_strPath + _T("\\"szTestSavedShellDataName"_") + (LPTSTR)(bstr_lang) + _T(".data");
		bfile_exist = false;
		bfile_exist = _isfileexist( str_file_path );
		// storing file's existence
		pStream->Write( &bfile_exist, sizeof(bfile_exist), pulRW );
        if( bfile_exist )
		{
			// storing file
			if( !_store_file( pStream, str_file_path, pulRW ) )
				return false;
		}

        // storing classes
		str_file_path = m_strPath + _T("\\"szTestSavedClassesName"_") + (LPTSTR)(bstr_lang) + _T(".stg");
		bfile_exist = false;
		bfile_exist = _isfileexist( str_file_path );
		// storing file's existence
		pStream->Write( &bfile_exist, sizeof(bfile_exist), pulRW );
        if( bfile_exist )
		{
			// storing file
			if( !_store_file( pStream, str_file_path, pulRW ) )
				return false;
		}

		// 3. Storing keyboard's layout
		::StoreStringToStream( pStream, m_bstrKbrdLayout );		
    }
    else
	{   // loading

		m_state_tmp.Empty( );
		m_tbbmp_tmp.Empty( );
		m_shdata_tmp.Empty( );
		m_cls_tmp.Empty( );

		bool bres = false;

		// 1. Load state
		// loading option of load state
		pStream->Read( &m_bLoadShellState , sizeof( m_bLoadShellState ), pulRW );

		// loading state's files
		bool	bfile_exist = false;
		// loading file's existence (.state)
        pStream->Read( &bfile_exist, sizeof(bfile_exist), pulRW );
		if( bfile_exist )
		{
			// loading file
            if( !_load_file( pStream, 0, &m_state_tmp, pulRW ) )
			{
				m_state_tmp.Empty( );
				return false;
			}
		}

		bfile_exist = false;
		// loading file's existence (.tbbmp)
        pStream->Read( &bfile_exist, sizeof(bfile_exist), pulRW );
		if( bfile_exist )
		{
			// loading file
			if( !_load_file( pStream, 0, &m_tbbmp_tmp, pulRW ) )
			{
				m_tbbmp_tmp.Empty( );
				return false;
			}
		}

		// 2. Load shell data and classes	
		// loading option of load shell data and classes 
		pStream->Read( &m_bLoadSDC, sizeof( m_bLoadSDC), pulRW );

		bfile_exist = false;
		// loading shelldata's file's existence (.tbbmp)
        pStream->Read( &bfile_exist, sizeof(bfile_exist), pulRW );
		if( bfile_exist )
		{
			// loading shelldata's file
			if( !_load_file( pStream, 0, &m_shdata_tmp, pulRW ) )
			{
				m_shdata_tmp.Empty( );
				return false;
			}
		}
		
		bfile_exist = false;
		// loading shelldata's file's existence (.tbbmp)
        pStream->Read( &bfile_exist, sizeof(bfile_exist), pulRW );
		if( bfile_exist )
		{
			// loading classes's file
			if( !_load_file( pStream, 0, &m_cls_tmp, pulRW ) )
			{
				m_cls_tmp.Empty( );
				return false;
			}
		} 
        
		// 3. Load keyboard's layout's name
		m_bstrKbrdLayout = ::LoadStringFromStream( pStream );
        
	}

	return true;
}

HRESULT CTestItem::ParseCond()
{
	m_lstParsedMultyCond.clear();
	m_lstParsedSingleCond.clear();

	TPOS lPosCond = m_lstCond.head();

	while( lPosCond )
	{
		CString strCond = m_lstCond.get( lPosCond );
		lPosCond = m_lstCond.next( lPosCond );

		if( !strCond.IsEmpty() )
		{
			if( strCond[0] == ';' )
				continue;

			CString strFirst;
			if( strCond[0] == '{' )
			{
				int nId = strCond.Find( '}' );
				if( nId != -1 )
				{
					strFirst   = strCond.Left( nId );
					strCond  = strCond.Right( strCond.GetLength() - 1 - nId );
				}
			}

			CString *pArrStr = 0;
			long lCount = 0;
			split( strCond, ":", &pArrStr, &lCount );

			if( lCount >= 3 )
			{
                if( lCount > 3 )
				{
					for( int i = 3; i < lCount; i++ )
					{
						pArrStr[2] += _T(":") + pArrStr[i];
						pArrStr[i].Empty();
					}
				}

				CString *pArrStr2 = 0;
				long lCount2 = 0;
				split( pArrStr[2], "=", &pArrStr2, &lCount2 );

				if( lCount2 >= 2 )
				{
					// [vanek] SBT:947 - 15.06.2004
                    if( lCount2 > 2 )
					{
						for( int i = 2; i < lCount2; i++ )
						{
							pArrStr2[1] += _T("=") + pArrStr2[i];
							pArrStr2[i].Empty();
						}
					}

					pArrStr2[1] = pArrStr2[1].Left( pArrStr2[1].GetLength() - 1 );
					pArrStr2[1] = pArrStr2[1].Right( pArrStr2[1].GetLength() - 1 );

					CString *pArrStr3 = 0;
					long lCount3 = 0;
					split( pArrStr2[1], "|", &pArrStr3, &lCount3 );

					X_PARSED_DATA item;

 					if( !strFirst.IsEmpty() )
						pArrStr[0] = strFirst + "}" + pArrStr[0];

					item.strName = pArrStr[0];
					item.strPath = pArrStr[1];

					item.lArgCount = lCount3; 

					if( lCount3 > 0 )
					{
						item.pvarArgs = new _variant_t[lCount3];

						for( long i = 0; i < lCount3; i++ )
						{
							_variant_t varSrc( pArrStr3[i] );
							item.pvarArgs[i].ChangeType( StringToVarEnum( pArrStr2[0] ), &varSrc );
						}
					}

					if( lCount3 == 1 )
						m_lstParsedSingleCond.add_tail( item );
					else
						m_lstParsedMultyCond.add_tail( item );

					delete []pArrStr3;
				}
				delete []pArrStr2;
			}
			delete []pArrStr;
		}
	}

	long lCounter = 1;
	long lCounterRC = 0;
	long lCounterGR = 1;

	_list_t< X_PARSED_DATA > lstLastData; // Used for __EXCLUDE_EQUAL;

	for (TPOS lPos = m_lstParsedMultyCond.head(); lPos; lPos = m_lstParsedMultyCond.next(lPos))
	{
		X_PARSED_DATA &data = m_lstParsedMultyCond.get( lPos ); 

		data.dwMultyRunFlag = 0;

		char cFirst = data.strName[0];

		if( cFirst == '~' )
		{
			lstLastData.add_tail( data );

			data.dwMultyRunFlag = __EXCLUDE_EQUAL;
			data.strName  = data.strName.Right( data.strName.GetLength() - 1 );
		}
		else if( cFirst == '#' )
		{
			lCounterRC = data.lArgCount;
			data.dwMultyRunFlag = __RUN_BY_COLUMN;
			data.strName  = data.strName.Right( data.strName.GetLength() - 1 );
		}
		else if( cFirst == '{' )
		{
			lCounterRC = data.lArgCount;
			data.dwMultyRunFlag = __HAS_GROUP;
			int nLast = data.strName.Find( '}' );

			lCounter *= data.lArgCount;

			if( nLast != -1 )
			{
				CString strData = data.strName.Left( nLast );
				data.strName  = data.strName.Right( data.strName.GetLength() - 1 - nLast );

				CString *pstr = 0;
				long lCnt = 0;

				strData  = strData.Right( strData.GetLength() - 1 );

				split( strData, "!=", &pstr, &lCnt );
				if( lCnt < 2 )
				{
					delete []pstr;
					lCnt = 0;
					
					split( strData, "=", &pstr, &lCnt );
				}
				else
					data.dwMultyRunFlag |= __NOT_EQUAL_GROUP;

				data.strMainItemPath = pstr[0];

				if( lCnt == 2 )
				{
					CString *pstr2 = 0;
					long lCnt2 = 0;

					split( pstr[1], "|", &pstr2, &lCnt2 );

					data.lArgCountMain = lCnt2;
					data.pvarArgsMain = new _variant_t[lCnt2];

					for( long i = 0; i < lCnt2; i++ )
					{
						_variant_t varSrc( pstr2[i] );
						data.pvarArgsMain[i] = varSrc;
					}

					delete []pstr2;
				}

				delete []pstr;
			}
		}
		else
			lCounter *= data.lArgCount;
	}

	if( lstLastData.count() )
	{
		long lcnt = 0;
		_calc_excl_equal_count( &lstLastData,  lstLastData.count() - 1, &lcnt ); 

		if( lcnt )
			lCounter *= lcnt;
	}

	if( lCounterRC )
		lCounter *= lCounterRC;

	m_lCombCounter = lCounter > 1 ? lCounter : 0;
	if( m_lCombCounter > MAX_VARS_PER_TEST )
	{
		CString str, str2;
		str.Format( IDS_WARING_MSG,  MAX_VARS_PER_TEST );
		str2.LoadString( IDS_WARING_TITLE );

		::MessageBox( ::GetDesktopWindow(), str, str2, MB_ICONEXCLAMATION | MB_OK );
	}

	return S_OK;
}

HRESULT CTestItem::ParseExecCond()
{
	m_lstParsedExecCond.clear();

	TPOS lPosCond = m_lstExecCond.head();

	while( lPosCond )
	{
		CString strCond = m_lstExecCond.get( lPosCond );
		lPosCond = m_lstExecCond.next( lPosCond );

		X_PARSED_DATA item;

		if( !strCond.IsEmpty() )
		{
			if( strCond[0] == _T(';') )
				continue;

			CString *pArrStr = 0;
			long lCount = 0;
			split( strCond, _T(":"), &pArrStr, &lCount );

			if( lCount >= 3 )
			{
                if( lCount > 3 )
				{
					for( int i = 3; i < lCount; i++ )
					{
						pArrStr[2] += _T(":") + pArrStr[i];
						pArrStr[i].Empty();
					}
				}

				CString str_splitter( _T("") );
				int		nfirst_equal = -1,
						nfirst_noequal = -1;

				nfirst_equal = pArrStr[2].Find( _T("=") );
				nfirst_noequal = pArrStr[2].Find( _T("!=") );
				if( nfirst_equal != -1 && nfirst_noequal != -1 )
					nfirst_equal = nfirst_equal < nfirst_noequal ? nfirst_equal : -1;
				
				if( nfirst_equal != -1 )
                    str_splitter = _T("=");
				else if( nfirst_noequal != -1 )
				{
					str_splitter = _T("!=");
					item.dwMultyRunFlag = __NOT_EQUAL_GROUP;
				}
                
				if( !str_splitter.IsEmpty() )
				{
					CString *pArrStr2 = 0;
					long lCount2 = 0;
					split( pArrStr[2], str_splitter, &pArrStr2, &lCount2 );
					if( lCount2 >= 2 )
					{
						if( lCount2 > 2 )
						{
							for( int i = 2; i < lCount2; i++ )
							{
								pArrStr2[1] += str_splitter + pArrStr2[i];
								pArrStr2[i].Empty();
							}
						}

						pArrStr2[1] = pArrStr2[1].Left( pArrStr2[1].GetLength() - 1 );
						pArrStr2[1] = pArrStr2[1].Right( pArrStr2[1].GetLength() - 1 );

						CString *pArrStr3 = 0;
						long lCount3 = 0;
						split( pArrStr2[1], _T("|"), &pArrStr3, &lCount3 );
						
						item.strName = pArrStr[0];
						item.strPath = pArrStr[1];
						
						item.lArgCount = lCount3;
						if( lCount3 > 0 )
						{
							item.pvarArgs = new _variant_t[lCount3];

							for( long i = 0; i < lCount3; i++ )
							{
								_variant_t varSrc( pArrStr3[i] );
								item.pvarArgs[i].ChangeType( StringToVarEnum( pArrStr2[0] ), &varSrc );
							}
						}

						m_lstParsedExecCond.add_tail( item );
						
						if( pArrStr3 ) delete []pArrStr3;
						pArrStr3 = 0;
					}

					if( pArrStr2 ) delete []pArrStr2; 
					pArrStr2 = 0;
				}
			}
			if( pArrStr ) delete []pArrStr;
			pArrStr = 0;
		}
	}    
    
	return S_OK;
}

HRESULT CTestItem::VerifyExecCond( /*[out]*/ BOOL *pbCanExec )
{
	if( !pbCanExec )
		return E_INVALIDARG;

	*pbCanExec = TRUE;
	for (TPOS lpos_cond = m_lstParsedExecCond.head(); lpos_cond; lpos_cond = m_lstParsedExecCond.next(lpos_cond))
	{
        X_PARSED_DATA &data = m_lstParsedExecCond.get( lpos_cond );         
		if( data.strPath.IsEmpty() || data.strName.IsEmpty() )
			continue;

		_variant_t var_curr;
		if( data.strName == _T("Application") )
			var_curr = ::GetValue( ::GetAppUnknown(), 0, (const char*)_bstr_t(data.strPath), var_curr );
		else if( data.strName == _T("Document") )
		{
			IUnknown *punkDoc = 0;
			IApplicationPtr sptrApp = ::GetAppUnknown();
			sptrApp->GetActiveDocument( &punkDoc );
			var_curr = ::GetValue( punkDoc, 0, (const char*)_bstr_t(data.strPath), var_curr );
			if( punkDoc )
				punkDoc->Release();	punkDoc = 0;
		}
		else if( data.strName == _T("Object") )
		{
			IUnknown *punkDoc = 0;
			IApplicationPtr sptrApp = ::GetAppUnknown();
			sptrApp->GetActiveDocument( &punkDoc );

			long lfirst = data.strPath.Find( '[' );
			long llast = data.strPath.Find( ']' );

			_bstr_t bstr_obj_name, bstr_path;
			bstr_obj_name = data.strPath.Mid( lfirst + 1, llast - lfirst - 1 );
			bstr_path = data.strPath.Right( data.strPath.GetLength() - llast - 1 );

			IUnknown *punkO = ::FindObjectByName( punkDoc, bstr_obj_name ); 
			if( punkO )
				var_curr = ::GetValue( punkDoc, 0, (const char*)bstr_path, var_curr );
		}

		BOOL bExec = FALSE;
		for( long larg = 0; larg < data.lArgCount; larg ++ )
		{
			// для вариантов используем опреацию "ИЛИ" 
			bExec |= (data.dwMultyRunFlag & __NOT_EQUAL_GROUP) ?
				var_curr != data.pvarArgs[larg] : var_curr == data.pvarArgs[larg];
		}

		// для условий используем операцию "И"
		*pbCanExec &= bExec;
	}

    return S_OK;
}

HRESULT CTestItem::GetFirstExecCondPos( /*[out]*/ LPOS *lpPos)
{
    if( !lpPos )
		return E_FAIL;

		*lpPos = (LPOS)m_lstExecCond.head();
    return S_OK;
}

HRESULT CTestItem::GetNextExecCond( /*[out]*/ LPOS *plPos, /*[in]*/ BSTR *pbstr)
{
	if( !plPos || !pbstr )
		return E_FAIL;

	CString str = m_lstExecCond.get((TPOS)*plPos);
	*plPos = (LPOS)m_lstExecCond.next( (TPOS)*plPos );
	*pbstr = str.AllocSysString();
	return S_OK;
}

HRESULT CTestItem::AddExecCond( /*[in]*/ BSTR bstrCond )
{
	m_lstExecCond.add_tail( CString(bstrCond) );
	return S_OK;
}

HRESULT CTestItem::DeleteExecCond( /*[in]*/ LONG_PTR lPos )
{
	if( !lPos )
		return E_INVALIDARG;

	m_lstExecCond.remove( (TPOS)lPos );
	return S_OK;
}

HRESULT CTestItem::DeleteAllExecCond()
{
	m_lstExecCond.clear( );
	return S_OK;
}
	
HRESULT CTestItem::GetFirstSingleCond( /*[out,retval]*/ LPOS *lpPos)
{
	if( !lpPos )
		return E_FAIL;

	*lpPos = (LPOS)m_lstParsedSingleCond.head();
	return S_OK;
}

HRESULT CTestItem::NextSingleCond( /*[out]*/ VARIANT *lpPos, /*[out]*/ VARIANT *pVarName, /*[out]*/ VARIANT *pVarPath, /*[out]*/ VARIANT *pVarParams )
{
	if( !lpPos )
		return E_FAIL;

	if( !pVarName )
		return E_FAIL;

	if( !pVarPath )
		return E_FAIL;

	if( !pVarParams )
		return E_FAIL;

	//if( lpPos->vt != VT_I4 )
	//	return E_FAIL;

	X_PARSED_DATA &data = m_lstParsedSingleCond.get( (TPOS)lpPos->LONG_PTR_VAL ); 

	lpPos->LONG_PTR_VAL = (LPOS)m_lstParsedSingleCond.next((TPOS)lpPos->LONG_PTR_VAL);

	::VariantCopy( pVarName, &_variant_t( _bstr_t( data.strName ) ) );
	::VariantCopy( pVarPath, &_variant_t( _bstr_t( data.strPath ) ) );

	::VariantCopy( pVarParams, &data.pvarArgs[0] );

	return S_OK;
}


HRESULT CTestItem::GetFirstMultyCond( /*[out,retval]*/ LPOS *lpPos  )
{
	if( !lpPos )
		return E_FAIL;

	*lpPos = (LPOS)m_lstParsedMultyCond.head();
	return S_OK;
}

HRESULT CTestItem::NextMultyCond( /*[out]*/ VARIANT *lpPos, /*[out]*/ VARIANT *pVarName, /*[out]*/ VARIANT *pVarPath, /*[out]*/ VARIANT **pVarParams, /*[out]*/VARIANT *pVarFlag, /*[out]*/ long *plCount, /*out*/VARIANT *pVarNameMain, /*[out]*/ VARIANT **pVarParamsMain, /*[out]*/ long *plCountMain  )
{
	if( !lpPos )
		return E_FAIL;

	if( !pVarName )
		return E_FAIL;

	if( !pVarPath )
		return E_FAIL;

	if( !pVarPath )
		return E_FAIL;

	if( !pVarParams )
		return E_FAIL;

	if( !pVarFlag )
		return E_FAIL;

	if( !plCount )
		return E_FAIL;

	if( !pVarNameMain )
		return E_FAIL;

	if( !pVarParamsMain )
		return E_FAIL;

	if( !plCountMain )
		return E_FAIL;

	if( lpPos->vt != VT_I4 )
		return E_FAIL;

	X_PARSED_DATA &data = m_lstParsedMultyCond.get( (TPOS)lpPos->LONG_PTR_VAL ); 

	lpPos->LONG_PTR_VAL = (LPOS)m_lstParsedMultyCond.next((TPOS)lpPos->LONG_PTR_VAL);

	::VariantInit( pVarName );
	::VariantInit( pVarPath );

	::VariantCopy( pVarName, &_variant_t( _bstr_t( data.strName ) ) );
	::VariantCopy( pVarPath, &_variant_t( _bstr_t( data.strPath ) ) );

	::VariantInit( pVarNameMain );
	::VariantCopy( pVarNameMain, &_variant_t( _bstr_t( data.strMainItemPath ) ) );

	*plCount = data.lArgCount;
	*plCountMain = data.lArgCountMain;

	pVarFlag->vt = VT_I4;
	pVarFlag->lVal = data.dwMultyRunFlag;

	*pVarParams = new VARIANT[*plCount];

	for( long i = 0; i < *plCount; i++ )
	{
		::VariantInit( &( ( *pVarParams )[i] ) );
		::VariantCopy( &( ( *pVarParams )[i] ), &data.pvarArgs[i] );
	}

	*pVarParamsMain = new VARIANT[*plCountMain];

	for( long i = 0; i < *plCountMain; i++ )
	{
		::VariantInit( &( ( *pVarParamsMain )[i] ) );
		::VariantCopy( &( ( *pVarParamsMain )[i] ), &data.pvarArgsMain[i] );
	}

	return S_OK;
}

HRESULT CTestItem::SetTestSavedState( /*[in]*/ long bSet )
{
	m_bSaved = BOOL( bSet != 0 );
	return S_OK;
}

HRESULT CTestItem::GetTestSavedState( /*[out]*/ long *pbSet )
{
	if( !pbSet )
		return E_FAIL;

	*pbSet = (long)m_bSaved;
	return S_OK;
}

HRESULT CTestItem::SetTestErrorState( /*[in]*/ long bError )
{
	m_bError = BOOL( bError != 0 );
	return S_OK;
}

HRESULT CTestItem::GetTestErrorState( /*[out]*/ long *pbError )
{
	if( !pbError )
		return E_FAIL;

	*pbError = (long)m_bError;
	return S_OK;
}


HRESULT CTestItem::SetTestRunTime( /*[in]*/ unsigned long dwTime )
{
	m_dwTime = dwTime;
	return S_OK;
}

HRESULT CTestItem::GetTestRunTime( /*[out]*/ unsigned long *pdwTime )
{
	if( !pdwTime )
		return E_FAIL;

	*pdwTime = m_dwTime;
	return S_OK;
}

HRESULT CTestItem::SetScriptFinal( /*[in]*/ BSTR bstrScript )
{
	m_strScriptFinal = bstrScript;
	return S_OK;
}

HRESULT CTestItem::GetScriptFinal( /*[out, retval]*/ BSTR *pbstrScript )
{
	if( !pbstrScript )
		return E_FAIL;
	
	*pbstrScript = m_strScriptFinal.AllocSysString();

	return S_OK;
}
HRESULT CTestItem::LoadContent()
{
	INamedDataPtr	sptrD( "Data.NamedData", 0, CLSCTX_INPROC_SERVER );
	// [vanek] - берем текущий язык системы тестирования у TestManager - 12.07.2004
	_bstr_t strLang;
	if( !_get_test_lang( strLang.GetAddress() ) )
		return S_FALSE;

	if( sptrD == 0 )
		return E_FAIL;

	if( m_strPath.IsEmpty() )
		return E_FAIL;
	
	IFileDataObject2Ptr	sptrF = sptrD;

	if( sptrF == 0 )
		return E_FAIL;

 	if( !::PathIsDirectory( m_strPath ) )
		return E_FAIL;

	if( sptrF->LoadTextFile( _bstr_t( m_strPath + "\\test.ini" ) ) != S_OK )
		return E_FAIL;

	_deinit();

	sptrD->SetupSection( _bstr_t( SECTION_ALG ) );

	_variant_t varScript;
	if( sptrD->GetValue( _bstr_t( KEY_ALG_SCRIPT ), &varScript ) != S_OK )
		return E_FAIL;

	if( varScript.vt == VT_BSTR )
		m_strScript = varScript.bstrVal;

	_variant_t varScriptFinal;
	if( sptrD->GetValue( _bstr_t( KEY_ALG_SCRIPT "Final" ), &varScriptFinal ) != S_OK )
		return E_FAIL;

	if( varScriptFinal.vt == VT_BSTR )
		m_strScriptFinal = varScriptFinal.bstrVal;

	sptrD->SetupSection( _bstr_t( CString( SECTION_ADD ) ) );

	_variant_t varInfo;
	if( sptrD->GetValue( _bstr_t( CString( KEY_ADDITONAL ) ), &varInfo ) != S_OK )
		return E_FAIL;

	if( varInfo.vt == VT_BSTR )
		m_strInfo = varInfo.bstrVal;

	sptrD->SetupSection( _bstr_t( CString( SECTION_ADD ) + "_" + (char*)strLang ) );

	_variant_t varDate;
	if( sptrD->GetValue( _bstr_t( KEY_ADD_RUNDATE ), &varDate ) != S_OK )
		return E_FAIL;

	if( varDate.vt == VT_R8 )
		m_date = varDate.dblVal;

	_variant_t varDateSve;
	if( sptrD->GetValue( _bstr_t( KEY_ADD_SAVEDATE ), &varDateSve ) != S_OK )
		return E_FAIL;

	if( varDateSve.vt == VT_R8 )
		m_date_save = varDateSve.dblVal;

	_variant_t varSaved;

	if( sptrD->GetValue( _bstr_t( KEY_ADD_SAVED ), &varSaved ) != S_OK )
		return E_FAIL;

	if( varSaved.vt == VT_INT )
		m_bSaved = varSaved.iVal;

	_variant_t varError;

	if( sptrD->GetValue( _bstr_t( KEY_ADD_ERROR ), &varError ) != S_OK )
		return E_FAIL;

	if( varError.vt == VT_INT )
		m_bError = varError.iVal;

	_variant_t varTime;

	if( sptrD->GetValue( _bstr_t( KEY_TIME_RUN ), &varTime ) != S_OK )
		return E_FAIL;

	if( varSaved.vt == VT_I4 )
		m_dwTime = varTime.lVal;

	_variant_t varManualCorr;

	if( sptrD->GetValue( _bstr_t( KEY_MANUAL_CORRECTION ), &varManualCorr ) != S_OK )
		return E_FAIL;

	if (varManualCorr.vt == VT_I4)
		m_bManualCorr = (BOOL)varManualCorr.lVal;
	else if (varManualCorr.vt == VT_I2)
		m_bManualCorr = (BOOL)varManualCorr.iVal;

	// load flags
	_variant_t varLoadFlags;
	if( sptrD->GetValue( _bstr_t( KEY_ENV_LOADFLAGS ), &varLoadFlags ) != S_OK )
		return E_FAIL;

	if( varLoadFlags.vt == VT_I4 )
		SetLoadFlags( varLoadFlags );
	//

	// keyboard layout
	_variant_t varKbdLayout;
	if( sptrD->GetValue( _bstr_t( KEY_ENV_KBDLAYOUT ), &varKbdLayout ) != S_OK )
		return E_FAIL;

	if( varKbdLayout.vt == VT_BSTR )
		m_bstrKbrdLayout = varKbdLayout;
	//

	long lCountCnd = 0;
	sptrD->SetupSection( _bstr_t( SECTION_COND ) );
    sptrD->GetEntriesCount( &lCountCnd );
	// [vanek] SBT:1100 - 01.09.2004
	if( lCountCnd )
	{
		_variant_t varCount;
		if( sptrD->GetValue( _bstr_t( KEY_COND_CNT ), &varCount ) != S_OK )
			return E_FAIL;

		if( (long)varCount != ( lCountCnd - 1 ) )
			return E_FAIL;

		for( int i = 0; i < lCountCnd - 1; i++ )
		{
			CString strCondName;
			strCondName.Format( "%s%d", KEY_COND, i );

			_variant_t varCond;
			if( sptrD->GetValue( _bstr_t( strCondName ), &varCond ) != S_OK )
				break;

			if( varCond.vt == VT_BSTR )
				m_lstCond.add_tail( CString( varCond.bstrVal ) );
		}
		
		if( m_lstCond.count() != ( lCountCnd - 1 ) )
		{
			m_lstCond.clear();
			return E_FAIL;
		}
	}

	// [vanek] : load executing coditions - 24.08.2004
	lCountCnd = 0;
	sptrD->SetupSection( _bstr_t( SECTION_EXEC_COND ) );
	sptrD->GetEntriesCount( &lCountCnd );
	// [vanek] SBT:1100 - 01.09.2004
	if( lCountCnd )
	{
		_variant_t varCount;
		varCount.Clear();
		if( sptrD->GetValue( _bstr_t( KEY_COND_CNT ), &varCount ) != S_OK )
			return E_FAIL;
	    
		if( (long)varCount != ( lCountCnd - 1 ) )
			return E_FAIL;

		for( int i = 0; i < lCountCnd - 1; i++ )
		{
			CString strCondName;
			strCondName.Format( "%s%d", KEY_COND, i );
			_variant_t varCond;
			if( sptrD->GetValue( _bstr_t( strCondName ), &varCond ) != S_OK )
				break;

			if( varCond.vt == VT_BSTR )
				m_lstExecCond.add_tail( CString( varCond.bstrVal ) );
		}
		
		if( m_lstExecCond.count() != ( lCountCnd - 1 ) )
		{
			m_lstExecCond.clear();
			return E_FAIL;
		}
	}
	
	return S_OK;
}

HRESULT CTestItem::Load( IStream *pStream, SerializeParams *pparams )
{
	_deinit();

	if( !pStream )
		return E_FAIL;

	ULONG ulRead = 0;
	long lVersion = 1L;

	pStream->Read( &lVersion, sizeof( long ), &ulRead );

	m_strPath = (char *)::LoadStringFromStream( pStream );
	m_strScript = (char *)::LoadStringFromStream( pStream );
	m_strScriptFinal = (char *)::LoadStringFromStream( pStream );
	m_strInfo = (char *)::LoadStringFromStream( pStream );

	pStream->Read( &m_date, sizeof( m_date ), &ulRead );
	pStream->Read( &m_date_save, sizeof( m_date_save ), &ulRead );
	pStream->Read( &m_bSaved, sizeof( m_bSaved ), &ulRead );
	// pStream->Read( &m_bLoadShellState , sizeof( m_bLoadShellState ), &ulRead ); [vanek] : move to _serialize_environment  - 05.08.2004
	pStream->Read( &m_bError, sizeof( m_bError ), &ulRead );
	pStream->Read( &m_dwTime, sizeof( m_dwTime ), &ulRead );
	if (lVersion >= 2)
		pStream->Read( &m_bManualCorr, sizeof(m_bManualCorr), &ulRead );

	long lCount = 0;
	pStream->Read( &lCount, sizeof( lCount ), &ulRead );

	for( long i = 0; i < lCount; i++ )
	{
		_bstr_t bstrVal = ::LoadStringFromStream( pStream );
		m_lstCond.add_tail( (char *)bstrVal );
	}

	// [vanek]: load state from stream - 13.07.2004
	if( lVersion >= 3 )
	{
        if( !_serialize_environment( pStream, false) )
			return S_FALSE;
	}

	// [vanek]: load executing coditions from stream - 24.08.2004
	if( lVersion >= 4 )
	{
        long lCount = 0;
		pStream->Read( &lCount, sizeof( lCount ), &ulRead );
		for( long i = 0; i < lCount; i++ )
		{
			_bstr_t bstrVal = ::LoadStringFromStream( pStream );
			m_lstExecCond.add_tail( (LPTSTR)bstrVal );
		}
	}
	
	return S_OK;
}

HRESULT CTestItem::StoreContent()
{
	INamedDataPtr	sptrD( "Data.NamedData", 0, CLSCTX_INPROC_SERVER );

	if( sptrD == 0 )
		return E_FAIL;

	if( m_strPath.IsEmpty() )
		return E_FAIL;
	
	IFileDataObject2Ptr	sptrF = sptrD;

	if( sptrF == 0 )
		return E_FAIL;

	sptrD->SetupSection( _bstr_t( SECTION_ALG ) );

	if( sptrD->SetValue( _bstr_t( KEY_ALG_SCRIPT ), _variant_t( _bstr_t( m_strScript ) ) ) != S_OK )
		return E_FAIL;
	
	if( sptrD->SetValue( _bstr_t( KEY_ALG_SCRIPT "Final" ), _variant_t( _bstr_t( m_strScriptFinal ) ) ) != S_OK )
		return E_FAIL;

	sptrD->SetupSection( _bstr_t( CString( SECTION_ADD ) ) );

	if( sptrD->SetValue( _bstr_t( CString( KEY_ADDITONAL ) ), _variant_t( _bstr_t( m_strInfo ) ) ) != S_OK )
		return E_FAIL;

	// [vanek] - берем текущий язык системы тестирования у test manager - 12.07.2004
	_bstr_t strLang;
	if( !_get_test_lang( strLang.GetAddress() ) )
		return S_FALSE;

	sptrD->SetupSection( _bstr_t( CString( SECTION_ADD ) + "_" + (char*)strLang ) );

	if( sptrD->SetValue( _bstr_t( KEY_ADD_RUNDATE ), _variant_t( m_date ) ) != S_OK )
		return E_FAIL;

	if( sptrD->SetValue( _bstr_t( KEY_ADD_SAVEDATE ), _variant_t( m_date_save ) ) != S_OK )
		return E_FAIL;

	if( sptrD->SetValue( _bstr_t( KEY_ADD_SAVED ), _variant_t( m_bSaved ) ) != S_OK )
		return E_FAIL;

	if( sptrD->SetValue( _bstr_t( KEY_ADD_ERROR ), _variant_t( m_bError ) ) != S_OK )
		return E_FAIL;

	if( sptrD->SetValue( _bstr_t( KEY_TIME_RUN ), _variant_t( (long)m_dwTime ) ) != S_OK )
		return E_FAIL;

	_variant_t varManualCorr;

	if( sptrD->SetValue( _bstr_t( KEY_MANUAL_CORRECTION ), _variant_t((long)m_bManualCorr) ) != S_OK )
		return E_FAIL;

	DWORD dwload_flags = 0;
	GetLoadFlags( &dwload_flags );
	if( sptrD->SetValue( _bstr_t( KEY_ENV_LOADFLAGS ), _variant_t((long)dwload_flags) ) != S_OK )
		return E_FAIL;

	if( sptrD->SetValue( _bstr_t( KEY_ENV_KBDLAYOUT ), _variant_t(m_bstrKbrdLayout) ) != S_OK )
		return E_FAIL;

	sptrD->SetupSection( _bstr_t( SECTION_COND ) );

	if( sptrD->SetValue( _bstr_t( KEY_COND_CNT ), _variant_t( m_lstCond.count() ) ) != S_OK )
		return E_FAIL;
	long i = 0;
	for( TPOS lPos = m_lstCond.head(); lPos; lPos = m_lstCond.next( lPos ), i++ )
	{
		CString strCondName;
		strCondName.Format( "%s%ld", KEY_COND, i );

		_variant_t varCond;
		if( sptrD->SetValue( _bstr_t( strCondName ), _variant_t( _bstr_t( m_lstCond.get( lPos ) ) ) ) != S_OK )
			break;
	}
	
	if( i && m_lstCond.count() != i )
		return E_FAIL;

	// [vanek] : store executing conditions - 24.08.2004
	sptrD->SetupSection( _bstr_t( SECTION_EXEC_COND ) );
	if( sptrD->SetValue( _bstr_t( KEY_COND_CNT ), _variant_t( m_lstExecCond .count() ) ) != S_OK )
		return E_FAIL;
	i = 0;
	for( TPOS lPos = m_lstExecCond.head(); lPos; lPos = m_lstExecCond.next( lPos ), i++ )
	{
		CString strCondName;
		strCondName.Format( "%s%ld", KEY_COND, i );
		_variant_t varCond;
		if( sptrD->SetValue( _bstr_t( strCondName ), _variant_t( _bstr_t( m_lstExecCond.get( lPos ) ) ) ) != S_OK )
			break;
	}
	
	if( i && m_lstExecCond.count() != i )
		return E_FAIL;
	//

	if( !::PathIsDirectory( m_strPath ) )
		if( !::CreateDirectory( m_strPath, 0 ) )
			return E_FAIL;

	if( sptrF->SaveTextFile( _bstr_t( m_strPath + "\\test.ini" ) ) != S_OK )
		return E_FAIL;

	// [vanek]: если есть стейт в памяти - сливаем его на диск - 13.07.2004
	if( !m_strPath.IsEmpty( ) )
	{
		CString str_path( _T("") );
		if( !m_state_tmp.IsEmpty( ) )
		{
			str_path = m_strPath + _T("\\"szTestSavedStateName"_") + (LPTSTR)(strLang) + _T(".state");        
			if( m_state_tmp.Flush( str_path ) )
				m_state_tmp.Empty( );
		}

		if( !m_tbbmp_tmp.IsEmpty( ) )
		{
            str_path = m_strPath + _T("\\"szTestSavedStateName"_") + (LPTSTR)(strLang) + _T(".tbbmp");
			if( m_tbbmp_tmp.Flush( str_path ) )
				m_tbbmp_tmp.Empty( );
		}
		
		if( !m_shdata_tmp.IsEmpty( ) )
		{            
			str_path = m_strPath + _T("\\"szTestSavedShellDataName"_") + (LPTSTR)(strLang) + _T(".data");
			if( m_shdata_tmp.Flush( str_path ) )
				m_shdata_tmp.Empty( );
		}

		if( !m_cls_tmp.IsEmpty( ) )
		{            
			str_path = m_strPath + _T("\\"szTestSavedClassesName"_") + (LPTSTR)(strLang) + _T(".stg");
			if( m_cls_tmp.Flush( str_path ) )
				m_cls_tmp.Empty( );
		}
	}

	return S_OK;
}

HRESULT CTestItem::Store( IStream *pStream, SerializeParams *pparams )
{
	if( !pStream )
		return E_FAIL;

	ULONG ulWritten = 0;
	long lVersion = 4L;

	pStream->Write( &lVersion, sizeof( long ), &ulWritten );

	::StoreStringToStream( pStream, _bstr_t( m_strPath )  );
	::StoreStringToStream( pStream, _bstr_t( m_strScript )  );
	::StoreStringToStream( pStream, _bstr_t( m_strScriptFinal )  );
	::StoreStringToStream( pStream, _bstr_t( m_strInfo ) );
			   
	pStream->Write( &m_date, sizeof( m_date ), &ulWritten );
	pStream->Write( &m_date_save, sizeof( m_date_save ), &ulWritten );
	pStream->Write( &m_bSaved, sizeof( m_bSaved ), &ulWritten );
	// pStream->Write( &m_bLoadShellState, sizeof( m_bLoadShellState ), &ulWritten ); [vanek] : move to _serialize_environment  - 05.08.2004
	pStream->Write( &m_bError, sizeof( m_bError ), &ulWritten );
	pStream->Write( &m_dwTime, sizeof( m_dwTime ), &ulWritten );
	pStream->Write( &m_bManualCorr, sizeof(m_bManualCorr), &ulWritten );

	long lCount = m_lstCond.count();
	pStream->Write( &lCount, sizeof( lCount ), &ulWritten );
	for( TPOS lPos = m_lstCond.head(); lPos; lPos = m_lstCond.next( lPos ) )
		::StoreStringToStream( pStream, _bstr_t( m_lstCond.get( lPos ) )  );

	// [vanek]: store state to stream - 13.07.2004
	if( !_serialize_environment( pStream, true) )
		return S_FALSE;

	// [vanek]: store executing conditions to stream - 24.08.2004
	lCount = m_lstExecCond.count();
	pStream->Write( &lCount, sizeof( lCount ), &ulWritten );
	for( TPOS lPos = m_lstExecCond.head(); lPos; lPos = m_lstExecCond.next( lPos ) )
		::StoreStringToStream( pStream, _bstr_t( m_lstExecCond.get( lPos ) )  );
	
	return S_OK;
}

HRESULT CTestItem::GetAdditionalInfo( /*[out, retval]*/ BSTR *pbstrInfo )
{
	if( !pbstrInfo )
		return S_FALSE;

	if( m_strInfo.IsEmpty() )
		return S_FALSE;

	*pbstrInfo = m_strInfo.AllocSysString();

	return S_OK;
}

HRESULT CTestItem::SetAdditionalInfo( /*[in]*/ BSTR pbstrInfo )
{
	if( !pbstrInfo )
		return S_FALSE;

	m_strInfo = pbstrInfo;

	return S_OK;
}

HRESULT CTestItem::GetManualCorrectionMode( /*[out, retval]*/ long *plManualCorr )
{
	if (!plManualCorr)
		return E_FAIL;
	*plManualCorr = m_bManualCorr;
	return S_OK;
}

HRESULT CTestItem::SetManualCorrectionMode( /*[in]*/ long lManualCorr )
{
	m_bManualCorr = (BOOL)lManualCorr;
	return S_OK;
}

HRESULT CTestItem::GetCombinationCont( long *plCount )
{
	*plCount = m_lCombCounter;
	return S_OK;
}

// ITestEnvironment implementation
// load flags
HRESULT CTestItem::SetLoadFlags( /*[in]*/ DWORD dwFlags )
{
	m_bLoadShellState = dwFlags & tefState;
	m_bLoadSDC = dwFlags & tefSDC;
	return S_OK;
}

HRESULT CTestItem::GetLoadFlags( /*[out]*/ DWORD *pdwFlags)
{
    if( !pdwFlags )        
		return E_INVALIDARG;

	if( m_bstrKbrdLayout.length() )
		*pdwFlags |= tefKbdLayout;

	if( m_bLoadShellState )
		*pdwFlags |= tefState;

	if( m_bLoadSDC )
		*pdwFlags |= tefSDC;

	return S_OK;
}

// state
HRESULT CTestItem::SaveShellState( )
{
	if( m_strPath.IsEmpty() )
		return S_FALSE;

	_bstr_t bstr_lang;
	if( !_get_test_lang( bstr_lang.GetAddress() ) )
		return S_FALSE;

	CString strState( _T("") );
	strState = m_strPath + _T("\\"szTestSavedStateName"_") + (LPTSTR)(bstr_lang) + _T(".state");

    return ::ExecuteAction( "ToolsSaveAsState", "\"" + strState + "\""  ) ? S_OK : S_FALSE;
}

HRESULT CTestItem::LoadShellState( )
{
	if( !m_bLoadShellState || m_strPath.IsEmpty() )
		return S_FALSE;

	_bstr_t bstr_lang;
	if( !_get_test_lang( bstr_lang.GetAddress() ) )
		return S_FALSE;

	CString strState( _T("") );
	strState = m_strPath + _T("\\"szTestSavedStateName"_") + (LPTSTR)(bstr_lang) + _T(".state");

	if( !_isfileexist( strState ) )
		return S_FALSE;

	CString str_curr_state( _T("") );
	// считываем  путь к текущему стейту
	str_curr_state = (LPCTSTR)::GetValueString( GetAppUnknown(), "General", "CurrentState", str_curr_state );    
	
	HRESULT hr = S_FALSE;
	hr = ::ExecuteAction( "ToolsLoadStateEx", "\"" + strState + "\"" ) ? S_OK : S_FALSE;

	// восстанавливаем путь к текущему стейту
	::SetValue( GetAppUnknown(), "General", "CurrentState", str_curr_state );

	if( hr != S_OK )
		return hr;

	IApplicationPtr	sptr_app( GetAppUnknown() );
	HWND hWnd = 0;
	sptr_app->GetMainWindowHandle( &hWnd );

	if( hWnd )
		::SendMessage( hWnd, WM_USER+791, 0, 0 );

	sptr_app->ProcessMessage();
	return S_OK;
}

// shell data and classes - SDC
HRESULT CTestItem::SaveSDC( )
{
	if( m_strPath.IsEmpty() )
		return S_FALSE;

	_bstr_t bstr_lang;
	if( !_get_test_lang( bstr_lang.GetAddress() ) )
		return S_FALSE;

	HRESULT hr = S_FALSE;
	// 1. Save current shell data
	IFileDataObjectPtr sptrF = ::GetAppUnknown();
	if( sptrF == 0 ) 
		return S_FALSE;

	_bstr_t bstr_sdpath;
	bstr_sdpath = m_strPath + _T("\\"szTestSavedShellDataName"_") + (LPTSTR)(bstr_lang) + _T(".data");
	hr = sptrF->SaveFile( bstr_sdpath );
	if( hr != S_OK )
		return hr;
	
	// 2. Save current classes directory
	CString str_classes_path = (TCHAR *)::GetValueString( ::GetAppUnknown(), "\\Paths", "Classes", "" );
	if( !str_classes_path.IsEmpty( ) )
	{
		CString str_classes_stg_path( _T("") );
		str_classes_stg_path = m_strPath + _T("\\"szTestSavedClassesName"_") + (LPTSTR)(bstr_lang) + _T(".stg");
        CFileStorageHelper filehlp;
		hr = filehlp.CreateStorage( str_classes_stg_path );
		if( hr != S_OK )
			return hr;

		hr = filehlp.FillByPath( str_classes_path );
		if( hr != S_OK )
			return hr;
	}

    return S_OK;
}

HRESULT CTestItem::LoadSDC( )
{
	if( m_strPath.IsEmpty() )
		return S_FALSE;

	_bstr_t bstr_lang;
	if( !_get_test_lang( bstr_lang.GetAddress() ) )
		return S_FALSE;

	HRESULT hr = S_FALSE;

	// 1. Load shell data
	IFileDataObjectPtr sptrF = ::GetAppUnknown();
	if( sptrF == 0 )
		return S_FALSE;

	_bstr_t bstr_sdpath;
	bstr_sdpath = m_strPath + _T("\\"szTestSavedShellDataName"_") + (LPTSTR)(bstr_lang) + _T(".data");
	hr = sptrF->LoadFile( bstr_sdpath );
	if( hr != S_OK )
		return hr;
	
	// 2. Load classes
	CString str_classes_path(_T(""));
	str_classes_path = (TCHAR *)::GetValueString( ::GetAppUnknown(), "\\Paths", "Classes", "" );
	if( str_classes_path.IsEmpty( ) )
		return S_FALSE;
	
	int nlength = 0;
	nlength = str_classes_path.GetLength( );
	if( _T('\\') == str_classes_path.GetAt( nlength - 1 ) )
		str_classes_path.Delete( nlength - 1, 1 ) ;

	// -> rename current directory of classes
	if( 0 != _trename( str_classes_path, str_classes_path + _T("_bak") ) )
		return S_FALSE;
    
	// -> load from storage
	CString str_classes_stg_path( _T("") );
	str_classes_stg_path = m_strPath + _T("\\"szTestSavedClassesName"_") + (LPTSTR)(bstr_lang) + _T(".stg");
    CFileStorageHelper filehlp;	
	if( S_OK == (hr = filehlp.OpenStorage( str_classes_stg_path )) )      
        hr = filehlp.FlushContent( str_classes_path );                        		
	
	_update_system_settings();
    	
	// если все успешно прошло - удаляем директорию "<classes_path>_bak", иначе - откатываемся назад: удаляем
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
    
    return hr;
}

// keyboard's layout
HRESULT CTestItem::SaveKbdLayout( )
{
	CString str_kbd_layout( _T("") );
	HRESULT hr = S_FALSE;
	hr = ::GetKeyboardLayoutName( str_kbd_layout.GetBuffer(100) ) ? S_OK : FALSE;
	str_kbd_layout.ReleaseBuffer( );	
	m_bstrKbrdLayout = str_kbd_layout;
	return hr;
}

HRESULT CTestItem::LoadKbdLayout( )
{
	if( m_bstrKbrdLayout.length( ) )
		::LoadKeyboardLayout( m_bstrKbrdLayout, KLF_ACTIVATE );

	return S_OK;
}

HRESULT CTestItem::GetKbdLayoutName( BSTR *pbstrKbdLayoutName )
{
	if( !pbstrKbdLayoutName ) 
		return E_INVALIDARG;

	*pbstrKbdLayoutName = m_bstrKbrdLayout.copy( );
	return S_OK;
}

HRESULT CTestItem::SetKbdLayoutName( BSTR bstrKbdLayoutName )
{
	m_bstrKbrdLayout = bstrKbdLayoutName;
	return S_OK;
}