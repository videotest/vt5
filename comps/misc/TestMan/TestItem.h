#pragma once

#include "\vt5\common2\com_unknown.h"

#include "test_man.h"
#include "data5.h"

#include "idispatchimpl.h"
	
#import "testman.tlb" no_namespace raw_interfaces_only named_guids 

#define MAX_VARS_PER_TEST 1000

#include <atlstr.h>
#include "/vt5/awin/misc_list.h"
#include "/vt5/awin/misc_ptr.h"

// [vanek]
class CFileInMem
{
public:
	DWORD		m_dwFileAttributes; 
	FILETIME	m_ftCreationTime; 
	FILETIME	m_ftLastAccessTime; 
	FILETIME	m_ftLastWriteTime; 
    _ptr_t<BYTE>	m_bytes;              

	CFileInMem(void);
	~CFileInMem( );
    
	void	Empty( );
	bool	IsEmpty( );
	bool	Flush( LPCTSTR lpfilename );
};

class X_PARSED_DATA
{

	CString strName;
	CString strPath;

	_variant_t *pvarArgs;
	long lArgCount;
	DWORD dwMultyRunFlag;

	CString strMainItemPath;	// для группировки
	_variant_t *pvarArgsMain; // для группировки
	long lArgCountMain;			// для группировки
public:
	friend class CTestItem;
	X_PARSED_DATA()
	{
		_init();
	}

	~X_PARSED_DATA()
	{
		if( pvarArgs )
			delete []pvarArgs;
		pvarArgs = 0;

		if( pvarArgsMain )
			delete []pvarArgsMain;
		pvarArgsMain = 0;
	}

	X_PARSED_DATA( const X_PARSED_DATA &item )
	{
		_init();

		*this = item;

	}

	void operator=( const X_PARSED_DATA &item )
	{
		strName			= item.strName;
		strPath			= item.strPath;
		lArgCount		= item.lArgCount;
		dwMultyRunFlag	= item.dwMultyRunFlag;

		strMainItemPath		= item.strMainItemPath;
		lArgCountMain		= item.lArgCountMain;

		if( pvarArgs )
			delete []pvarArgs;
		pvarArgs = 0;

		if( pvarArgsMain )
			delete []pvarArgsMain;
		pvarArgsMain = 0;

		if( lArgCount > 0 )
			pvarArgs = new _variant_t[lArgCount];

		if( lArgCountMain > 0 )
			pvarArgsMain = new _variant_t[lArgCountMain];

		for( int i = 0; i < lArgCount; i++ )
			pvarArgs[i] = item.pvarArgs[i];

		for( int i = 0; i < lArgCountMain; i++ )
			pvarArgsMain[i] = item.pvarArgsMain[i];
	}

	void _init()
	{
		strName.Empty();
		strPath.Empty();
		pvarArgs = 0;
		lArgCount = 0;
		dwMultyRunFlag = 0;

		strMainItemPath.Empty();
		pvarArgsMain			= 0;
		lArgCountMain			= 0;
	}
};


class CTestItem : public ComObjectBase, 
				  public ITestItem,
				  public ITestRunningData,
				  public IDispatchImpl< ITestItemDisp >,
				  public ISerializableObject,
				  public ITestEnvironment

{
	CString m_strPath;
	CString m_strScript;
	CString m_strScriptFinal;

	CString m_strInfo;

	_list_t<CString> m_lstCond;
	DATE m_date, m_date_save;
	BOOL m_bChecked, m_bSaved, m_bError, m_bManualCorr;
 
	typedef _list_t<X_PARSED_DATA> typeCondList;
	
	typeCondList m_lstParsedMultyCond;
	typeCondList m_lstParsedSingleCond;

	DWORD m_dwTime;

	// [vanek] : test environment - 04.08.2004
	BOOL		m_bLoadShellState;	// state
	BOOL		m_bLoadSDC;			// shell data and classes - SDC
	_bstr_t		m_bstrKbrdLayout;	// keyboard's layout

	long m_lCombCounter;

	// [vanek]: данные переменные содержат содержимое файлов окружения теста (state, shelldata, classes) после
	// Load до первого StoreContent:
	CFileInMem m_state_tmp;		// state:	".state"
	CFileInMem m_tbbmp_tmp;		//			".tbbmp"
	CFileInMem m_shdata_tmp;	// shell data (".data")
	CFileInMem m_cls_tmp;		// classes (storage file: ".stg")

	// [vanek]
	_list_t<CString> m_lstExecCond;
	typeCondList m_lstParsedExecCond;

public:
	CTestItem(void);
	~CTestItem(void);

	route_unknown();
	virtual IUnknown *DoGetInterface( const IID &iid );

	// interface ITestItem
	com_call SetPath( /*[in]*/ BSTR bstrPath );
	com_call GetPath( /*[out,retval]*/ BSTR *pbstrPath );
	com_call GetFirstCondPos( /*[out,retval]*/ LPOS *lpPos );
	com_call GetNextCond( /*[out]*/ VARIANT *plPos, /*[out, retval]*/ BSTR *pbstr );
	com_call GetNextCond( /*[out]*/ LPOS *plPos, /*[out, retval]*/ BSTR *pbstr );
	com_call AddCond( /*[in]*/ BSTR bstrCond );
	com_call DeleteCond( /*[in]*/ LPOS lPos );
	com_call DeleteAllCond();
	com_call SetScript( /*[in]*/ BSTR bstrScript );
	com_call GetScript( /*[out,retval]*/ BSTR *pbstrScript );
	com_call LoadContent();
	com_call StoreContent();
	
	com_call KillThemselvesFromDisk();

	com_call ParseCond();

	// [vanek]
	com_call ParseExecCond();
	com_call VerifyExecCond( /*[out]*/ BOOL *pbCanExec );
	com_call GetFirstExecCondPos( /*[out]*/ LPOS *lpPos);
	com_call GetNextExecCond( /*[out]*/ LPOS *plPos, /*[in]*/ BSTR *pbstr);
	com_call AddExecCond( /*[in]*/ BSTR bstrCond );
	com_call DeleteExecCond( /*[in]*/ LPOS lPos);
	com_call DeleteAllExecCond();

	com_call GetFirstSingleCond( /*[out,retval]*/ LPOS *lpPos );
	com_call NextSingleCond( /*[out]*/ VARIANT *lpPos, /*[out]*/ VARIANT *pVarName, /*[out]*/ VARIANT *pVarPath, /*[out]*/ VARIANT *pVarParams );

	com_call GetFirstMultyCond( /*[out,retval]*/ LPOS *lpPos );
	com_call NextMultyCond( /*[out]*/ VARIANT *lpPos, /*[out]*/ VARIANT *pVarName, /*[out]*/ VARIANT *pVarPath, /*[out]*/ VARIANT **pVarParams,/*[out]*/ VARIANT *pVarFlag, /*[out]*/ long *plCount, VARIANT *pVarNameMain, /*[out]*/ VARIANT **pVarParamsMain, /*[out]*/ long *plCountMain  );

	com_call Rename( /*[in]*/ BSTR bstrNewName );

	com_call GetAdditionalInfo( /*[out, retval]*/ BSTR *pbstrInfo );
	com_call SetAdditionalInfo( /*[in]*/ BSTR pbstrInfo );

	com_call GetManualCorrectionMode( /*[out, retval]*/ long *plManualCorr );
	com_call SetManualCorrectionMode( /*[in]*/ long lManualCorr );

	// interface ITestRunningData
	com_call GetRunTime( /*[out,retval]*/ DATE *pDate );
	com_call SetRunTime( /*[in]*/ DATE Date );

	com_call GetSaveTime( /*[out,retval]*/ DATE *pDate );
	com_call SetSaveTime( /*[in]*/ DATE Date );

	com_call SetTestChecked( /*[in]*/ long bSet );
	com_call GetTestChecked( /*[out,retval]*/ long *pbSet );

	com_call SetTestSavedState( /*[in]*/ long bSet );
	com_call GetTestSavedState( /*[out]*/ long *pbSet );

	com_call SetTestErrorState( /*[in]*/ long bError );
	com_call GetTestErrorState( /*[out]*/ long *pbError );

	com_call SetTestRunTime( /*[in]*/ unsigned long dwTime );
	com_call GetTestRunTime( /*[out]*/ unsigned long *pdwTime );

	com_call SetScriptFinal( /*[in]*/ BSTR bstrScript );
	com_call GetScriptFinal( /*[out, retval]*/ BSTR *pbstrScript );
	com_call GetCombinationCont( long *plCount );

	// interface ISerializableObject
	com_call Load( IStream *pStream, SerializeParams *pparams );
	com_call Store( IStream *pStream, SerializeParams *pparams );

	// interface ITestEnvironment
	com_call SetLoadFlags( /*[in]*/ DWORD dwFlags );
	com_call GetLoadFlags( /*[out]*/ DWORD *pdwFlags);	

    // state
	com_call SaveShellState( );
	com_call LoadShellState( );	
	
	// shell data and classes - SDC
	com_call SaveSDC( );
	com_call LoadSDC( );	
    
    // keyboard's layout
    com_call SaveKbdLayout( );
	com_call LoadKbdLayout( );	
    com_call GetKbdLayoutName( BSTR *pbstrKbdLayoutName );		
	com_call SetKbdLayoutName( BSTR bstrKbdLayoutName );			

protected:
	void _deinit();
	void _calc_excl_equal_count( _list_t< X_PARSED_DATA> *pLst, int nId, long *plCounter );

	bool	_serialize_environment( IStream *pStream, bool bstore = true, ULONG *pulRW = 0 );
};
