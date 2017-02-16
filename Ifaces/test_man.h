#pragma once

typedef POSITION TPOS;
typedef LONG_PTR LPOS;

struct TEST_ERR_DESCR
{
	BSTR bstrPath;
	UINT nTryIndex;

	BSTR *pbstrImageNames;
	long lszImages;
	
	BSTR *pbstrArrNames;
	long lszArrays;

	long dwDiffTime;
	bool bTestError;
};

interface ITestManager : public IUnknown
{
	com_call GetFirstTestPos( /*[in]*/ long lParentPos, /*[out]*/ long *plPos ) = 0;
	com_call GetNextTest( /*[in]*/ TPOS lParentPos, /*[out]*/ VARIANT *plPos, /*[out]*/ IUnknown **ppunk) = 0;
	com_call AddTest( /*[in]*/ long lParent, /*[in]*/ IUnknown *punk, /*[out]*/ long *plpos ) = 0;
	com_call DeleteTest( /*[in]*/ TPOS lPos) = 0;

	com_call LoadTests( IStream *pStream, long lNewParentPos ) = 0;
	com_call StoreTests( IStream *pStream, long lFromParentPos ) = 0;

	com_call GetDocument( IUnknown **punkDoc ) = 0;

	com_call GetCombinationCont( long lPos, long *plCount ) = 0;
	com_call GetTestCont( long lParentPos, long lPos, long *plCount ) = 0;
	com_call GetTestContR( long lParentPos, long lPos, long *plCount ) = 0;

	com_call GetLang( /*out*/ BSTR *pbstrLang ) = 0;
};

interface ITestManErrorDescr : public IUnknown
{
	com_call GetFirstErrorPos( /*[out]*/ LPOS *plPos ) = 0;
	com_call GetNextError( /*[out]*/ LPOS *plPos, /*[out]*/ TEST_ERR_DESCR **plDescr) = 0;
};

interface ITestProcess : public IUnknown
{
	com_call RunTestCompare() = 0;
	com_call RunTestSave() = 0;
	com_call StopTesting() = 0;
	com_call GetRunningState( /*[out, retval]*/ long *plState ) = 0;
	com_call RunTestSimple() = 0;
	com_call SetEditMode( bool bEnter ) = 0;
	com_call PauseTesting( BOOL bPaused ) = 0;
};

declare_interface( ITestManager,		"A14D84A6-7817-4edc-82C1-BF87D5A9B55F" ) 
declare_interface( ITestManErrorDescr,	"ACD4442D-54F2-4335-B195-3BEA108AB008" )
declare_interface( ITestProcess,		"A679AF5C-16E7-463e-8D52-7838D338CBC0" )

#define __EXCLUDE_EQUAL (1 << 0 )
#define __RUN_BY_COLUMN (1 << 1 )
#define __HAS_GROUP (1 << 2 )
#define __NOT_EQUAL_GROUP (1 << 3 )


interface ITestItem : public IUnknown
{
	com_call SetPath( /*[in]*/ BSTR bstrPath ) = 0;
	com_call GetPath( /*[out]*/ BSTR *pbstrPath ) = 0;

	com_call GetFirstCondPos( /*[out]*/ LPOS *lpPos) = 0;
	com_call GetNextCond( /*[out]*/ LPOS *plPos, /*[in]*/ BSTR *pbstr) = 0;
	com_call AddCond( /*[in]*/ BSTR bstrCond ) = 0;
	com_call DeleteCond( /*[in]*/ LPOS lPos) = 0;
	com_call DeleteAllCond() = 0;
	com_call SetScript( /*[in]*/ BSTR bstrScript ) = 0;
	com_call GetScript( /*[out]*/ BSTR *pbstrScript ) = 0;
	com_call LoadContent() = 0;
	com_call StoreContent() = 0;

	com_call KillThemselvesFromDisk() = 0;

	com_call ParseCond() = 0;

	com_call GetFirstSingleCond( /*[out,retval]*/ long *lpPos ) = 0;
	com_call NextSingleCond( /*[out]*/ VARIANT *lpPos, /*[out]*/ VARIANT *pVarName, /*[out]*/ VARIANT *pVarPath, /*[out]*/ VARIANT *pVarParams ) = 0;

	com_call GetFirstMultyCond( /*[out,retval]*/ long *lpPos ) = 0;

	com_call NextMultyCond( /*[out]*/ VARIANT *lpPos, /*[out]*/ VARIANT *pVarName, /*[out]*/ VARIANT *pVarPath, /*[out]*/ VARIANT **pVarParams, /*[out]*/VARIANT *pVarFlag, /*[out]*/ long *plCount, VARIANT *pVarNameMain, /*[out]*/ VARIANT **pVarParamsMain, /*[out]*/ long *plCountMain ) = 0;

	com_call SetScriptFinal( /*[in]*/ BSTR bstrScript ) = 0;
	com_call GetScriptFinal( /*[out, retval]*/ BSTR *pbstrScript ) = 0;

	com_call Rename( /*[in]*/ BSTR bstrNewName ) = 0;

	com_call GetAdditionalInfo( /*[out, retval]*/ BSTR *pbstrInfo ) = 0;
	com_call SetAdditionalInfo( /*[in]*/ BSTR pbstrInfo ) = 0;

	com_call GetManualCorrectionMode( /*[out, retval]*/ long *plManualCorr ) = 0;
	com_call SetManualCorrectionMode( /*[in]*/ long lManualCorr ) = 0;

	// [vanek] SBT:1105 - 24.08.2004
	com_call ParseExecCond() = 0;
	com_call VerifyExecCond( /*[out]*/ BOOL *pbCanExec ) = 0;
	com_call GetFirstExecCondPos( /*[out]*/ LPOS *lpPos) = 0;
	com_call GetNextExecCond( /*[out]*/ LPOS *plPos, /*[in]*/ BSTR *pbstr) = 0;
	com_call AddExecCond( /*[in]*/ BSTR bstrCond ) = 0;
	com_call DeleteExecCond( /*[in]*/ LPOS lPos) = 0;
	com_call DeleteAllExecCond() = 0;
};

// [vanek] - 03.8.2004
// Test's environment flags
enum TestEnvFlags
{
	tefState		=	1 << 0,
	tefSDC			=	1 << 1,		// SDC -  shell data and classes
	tefKbdLayout	=	1 << 2,
};

interface ITestEnvironment : public IUnknown
{
	// load flags
	com_call SetLoadFlags( /*[in]*/ DWORD dwFlags ) = 0;
	com_call GetLoadFlags( /*[out]*/ DWORD *pdwFlags) = 0;	

    // state
	com_call SaveShellState( ) = 0;
	com_call LoadShellState( ) = 0;	
	
	// shell data and classes - SDC
	com_call SaveSDC( ) = 0;
	com_call LoadSDC( ) = 0;	
    
    // keyboard's layout
    com_call SaveKbdLayout( ) = 0;
	com_call LoadKbdLayout( ) = 0;	
	com_call GetKbdLayoutName( BSTR *pbstrKbdLayoutName ) = 0;		
	com_call SetKbdLayoutName( BSTR bstrKbdLayoutName ) = 0;			
};

interface ITestRunningData : public IUnknown
{
	com_call GetRunTime( /*[out]*/ DATE *pDate ) = 0;
	com_call SetRunTime( /*[in]*/ DATE Date ) = 0;
	com_call SetTestChecked( /*[in]*/ long bSet ) = 0;
	com_call GetTestChecked( /*[out]*/ long *pbSet ) = 0;

	com_call SetTestSavedState( /*[in]*/ long bSet ) = 0;
	com_call GetTestSavedState( /*[out]*/ long *pbSet ) = 0;

	com_call SetTestRunTime( /*[in]*/ unsigned long dwTime ) = 0;
	com_call GetTestRunTime( /*[out]*/ unsigned long *pdwTime ) = 0;

	com_call GetSaveTime( /*[out]*/ DATE *pDate ) = 0;
	com_call SetSaveTime( /*[in]*/ DATE Date ) = 0;

	com_call SetTestErrorState( /*[in]*/ long bError ) = 0;
	com_call GetTestErrorState( /*[out]*/ long *pbError ) = 0;

	com_call GetCombinationCont( long *plCount ) = 0;
};

interface ITestDataArray : public IUnknown
{
	com_call AddItem( /*[in]*/ VARIANT vData, /*[in]*/ BSTR bstrName) = 0;
	com_call GetItem( /*[in]*/ UINT nID, /*[out]*/ VARIANT *lpvRetVal) = 0;
	com_call GetItemCount( /*[out]*/ long *lplRet) = 0;
	com_call RemoveItem( /*[in]*/ UINT nID) = 0;
	com_call RemoveAll() = 0;
	com_call GetItemName( /*[in]*/ UINT nID, /*[out]*/ BSTR *pbstrName) = 0;
	com_call SetItemCmpExpr( /*[in]*/ UINT nID, /*[in]*/ BSTR bstrCmpExpr) = 0;
	com_call GetItemCmpExpr( /*[in]*/ UINT nID, /*[out]*/ BSTR *pbstrCmpExpr) = 0;
	com_call CompareItem( /*[in]*/ UINT nID, /*[in]*/ VARIANT vCmpData, /*[out]*/ long *plEqual ) = 0;
};

#define szTypeTestDataArray		"Test Data Array"
#define szTestClipboardFormat	"TestItemClipboardFormat_1D69229C-D404-4ea7-AE90-423FBE059D0A"

#define szTestSavedStateName		"TDS"	// test data : state
#define szTestSavedShellDataName	"TDSD"	// test data : shell data
#define szTestSavedClassesName		"TDCL"	// test data : classes
#define szTestTempStateName			"tmp"
#define szTestTempEverychildName	"chld_tmp"

declare_interface( ITestItem,		"CF0D26BE-F0B7-4bb9-B9BD-4D10759FA77C" ) 
declare_interface( ITestDataArray,		"B260416C-05D0-4c28-8D29-AB42B9DF0484" ) 
declare_interface( ITestRunningData,	"2E92B031-B0B5-43f5-8D89-2BFE1C89E661" )
declare_interface( ITestEnvironment,	"8884D973-1818-43b3-9ABF-A1022E0EE294" )

