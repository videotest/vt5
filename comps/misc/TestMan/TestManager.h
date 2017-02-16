#pragma once

#include "\vt5\common2\com_unknown.h"

#include "test_man.h"

#include "idispatchimpl.h"

#import "testman.tlb" no_namespace raw_interfaces_only named_guids 

#include <atlstr.h>

#include "/vt5/awin/misc_mtree.h"
#include "/vt5/awin/misc_list.h"
#include "/vt5/awin/misc_map.h"
#include "impl_event.h"

inline long cmp_cstring( CString s1, CString s2 )
{	return s1.Compare( s2 ); };

class CTestManager : public ComObjectBase,
					 public ITestManager,
					 public ITestManErrorDescr,
					 public ITestProcess,
					 public IDispatchImpl< ITestManagerDisp >,
					 public CEventListenerImpl
{
	CString m_strDBPath;
	bool m_bDBLoaded, m_bSimpleMode;

	bool m_bInterrupt, m_bPaused;
	bool m_bterminate_current_only;
	long m_lState;
	bool m_bRunning;
	long m_dwMinTimeDiff;
	bool m_bStoreToHidden;
	_bstr_t m_strLang;

	IUnknownPtr m_sptrHiddenDoc;

	static void _clear_err_lst( TEST_ERR_DESCR *pdata )
	{
		::SysFreeString( pdata->bstrPath );

		for( long i = 0; i < pdata->lszImages; i++ )
			::SysFreeString( pdata->pbstrImageNames[i] );

		delete []pdata->pbstrImageNames;

		for( long i = 0; i < pdata->lszArrays; i++ )
			::SysFreeString( pdata->pbstrArrNames[i] );

		delete []pdata->pbstrArrNames;

		delete pdata;
	}

	static void _free_tmp_file( CString str_filename )
	{
		// delete files of state
		::DeleteFile( str_filename );

		// специально для стейта
		if( -1 != str_filename.Find( _T(".state"), 0 ) )
		{
			if( str_filename.Replace( _T(".state"), _T(".tbbmp") )	)
				::DeleteFile( str_filename );
		}
	}

public:
	_multylink_tree_t<IUnknownPtr> m_treeTests; 

	_list_t<TEST_ERR_DESCR *, _clear_err_lst > m_listErr; 

	// [vanek] : мапа, содержащая пути к временно сохранненым файлам
	_map_t< CString, CString, cmp_cstring, _free_tmp_file> m_mapTmpFiles;

public:
	CTestManager(void);
	~CTestManager(void);

	route_unknown();
	virtual IUnknown *DoGetInterface( const IID &iid );
protected:
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

public:
	// interface ITestManager
	com_call GetFirstTestPos( /*[in]*/ TPOS lParentPos, /*[out,retval]*/ TPOS *plPos);
	com_call GetNextTest( /*[in]*/ TPOS lParentPos, /*[out]*/ TPOS *plPos, /*[out]*/ IUnknown **ppunk);
	com_call AddTest( /*[in]*/ TPOS lParent, /*[in]*/ IUnknown *punk,  /*[out,retval]*/ TPOS *plpos);
	com_call DeleteTest( /*[in]*/ TPOS lPos);

	com_call LoadTests( IStream *pStream, long lNewParentPos );
	com_call StoreTests( IStream *pStream, long lFromParentPos );

	com_call GetDocument( IUnknown **punkDoc );
	com_call GetCombinationCont( long lPos, long *plCount );
	com_call GetTestCont( long lParentPos, long lPos, long *plCount );
	com_call GetTestContR( long lParentPos, long lPos, long *plCount );

	com_call GetLang( /*out*/ BSTR *pbstrLang );

    // interface ITestManErrorDescr

	com_call GetFirstErrorPos( /*[out,retval]*/ LPOS *plPos);
	com_call GetNextError( /*[out]*/ LPOS *plPos, /*[out]*/ TEST_ERR_DESCR **plDescr);

	// interface ITestProcess
	com_call RunTestCompare();
	com_call RunTestSave();
	com_call RunTestSimple();
	com_call StopTesting();
	com_call PauseTesting( BOOL bPaused );

	// ITestManagerDisp
	com_call GetNextTest( /*[in]*/ TPOS lParentPos, /*[out]*/ VARIANT *plPos, /*[out,retval]*/ IDispatch **ppunk);
	com_call AddTest( /*[in]*/ TPOS lParent, /*[in]*/ IDispatch *punk,  /*[out,retval]*/ TPOS *plpos);
	com_call GetRunningState( /*[out, retval]*/ long *plState );
	com_call SetEditMode( bool bEnter );
protected:
	typedef HRESULT(*fn_callback)( IUnknown *punkTest );

	// [vanek] - 18.08.2004
	bool	_stop_testing( );

	HRESULT _exec_var_cond( IUnknown *punkTest, long lPos, _bstr_t bstrScript, _bstr_t bstrPath, _bstr_t bstrRootPath, IUnknown *punkF, long *plCounter, bool bSave, long *plFailCounter, long lFlag, long lPrevPos );
	HRESULT _load_db( CString lpstrFolder, long lParentPos );
	HRESULT _recursive_scan( long lParentPos, bool bSave, long lFlag, long *plCounter );

	bool _compare_context(  CString strCmpFile, CString strSaveFile, TEST_ERR_DESCR *ted );

	void _save_environment( IUnknown *punkDoc, CString strPath, CString strName, CString strSuff = "" );
	void _load_environment( IUnknown *punkDoc, CString strPath, CString strName, CString strSuff = "" );
	void _del_environment( IUnknown *punkDoc, CString strPath, CString strName, CString strSuff = "" );
	void _sync_context( IUnknown *punkDoc );

	// [vanek] SBT:1126 save/restore active view's zoom - 03.09.2004
	bool _save_view_zoom( IUnknown *punkDoc, IUnknown *punkSaveTo );
	bool _restore_view_zoom( IUnknown *punkDoc, IUnknown *punkRestoreFrom );

	// [vanek] SBT:1126 save/restore named props bags of doc's views
	bool _save_views_propbags( IUnknown *punkDoc, IUnknown *punkSaveTo );
	bool _restore_views_propbags( IUnknown *punkDoc, IUnknown *punkRestoreFrom );

protected:
	void _update_system_settings();
	HRESULT  _create_hidden_doc();
	void _copy_data_to_hidden();

	void _save_tests_environment( LPCTSTR lpPath, LPCTSTR lpFileName, DWORD dwFlags );
	void _load_tests_environment( LPCTSTR lpPath, LPCTSTR lpFileName, DWORD dwFlags );
	void _del_tests_environment( LPCTSTR lpPath, LPCTSTR lpFileName, DWORD dwFlags );
};
