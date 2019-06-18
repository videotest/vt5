// TODO: ������ m_bPlaying
// TODO: ���������, �� ���� �� �� ����� NeedWait ��������� ������ �������

#include "StdAfx.h"
#include "methodman.h"
#include "method.h"
#include "ScriptNotifyInt.h"
#include <stdio.h>
#include "mtd_utils.h"
#include "\vt5\awin\misc_dbg.h"
#include "\vt5\awin\misc_map.h"
#include "MethodDataUtil.h"
#include "Resource.h"
#include "MethodManState.h"
#include "TreeFilter.h"

////////////////////////////////////////////
BEGIN_INTERFACE_MAP(CMethodMan, CCmdTargetEx)
	INTERFACE_PART(CMethodMan, IID_IMethodMan, MethodMan)
	INTERFACE_PART(CMethodMan, IID_IMethodMan2, MethodMan)
	INTERFACE_PART(CMethodMan, IID_INamedObject2, Name)
	INTERFACE_PART(CMethodMan, IID_IEventListener, EvList)
	INTERFACE_PART(CMethodMan, IID_IMsgListener,	MsgList)
	INTERFACE_AGGREGATE(CMethodMan, m_punkNotifyCtrl)
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CMethodMan, MethodMan);

IMPLEMENT_DYNCREATE(CMethodMan, CCmdTargetEx);

GUARD_IMPLEMENT_OLECREATE(CMethodMan, "Metodics.MethodMan",
0xcd764233, 0x96a1, 0x4f71, 0x9c, 0x43, 0x72, 0x4a, 0x90, 0x2a, 0x3d, 0xf8);

BEGIN_DISPATCH_MAP(CMethodMan, CCmdTargetEx)
	//{{AFX_DISPATCH_MAP(CImage)
	DISP_FUNCTION(CMethodMan, "Run", Run, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMethodMan, "Stop", Stop, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMethodMan, "Pause", Pause, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMethodMan, "Resume", Resume, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMethodMan, "Record", Record, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMethodMan, "IsRunning", IsRunning, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CMethodMan, "IsRecording", IsRecording, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CMethodMan, "GetCurrentStartingActionName", GetCurrentStartingActionName, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CMethodMan, "GetActionNameByIndex", GetActionNameByIndex, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CMethodMan, "StoreStepAs", StoreStepAs, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CMethodMan, "CancelStepStoring", CancelStepStoring, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMethodMan, "DeleteCurrentMethod", DeleteCurrentMethod, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMethodMan, "RenameCurrentMethod", RenameCurrentMethod, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CMethodMan, "ClearMethodCache", ClearMethodCache, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMethodMan, "GetCurrentStepFlags", GetCurrentStepFlags, VT_I4, VTS_NONE)
	DISP_FUNCTION(CMethodMan, "SetCurrentStepFlags", SetCurrentStepFlags, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CMethodMan, "RenameCurrentStep", RenameCurrentStep, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CMethodMan, "GetCurrentMethodName", GetCurrentMethodName, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CMethodMan, "RememberSection", RememberSection, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CMethodMan, "SaveFilteredShellData", SaveFilteredShellData, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CMethodMan, "ImportShellData", ImportShellData, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CMethodMan, "GetCurrentStepNamedData", GetActiveStepNamedData, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CMethodMan, "GetStepNamedData", GetStepNamedData, VT_DISPATCH, VTS_I4)
	DISP_FUNCTION(CMethodMan, "StartSetValueLogging", StartSetValueLogging, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMethodMan, "FinishSetValueLogging", FinishSetValueLogging, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMethodMan, "StartSetValueLogging2", StartSetValueLogging2, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CMethodMan, "FinishSetValueLogging2", FinishSetValueLogging2, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CMethodMan, "StoreSplitter", DispStoreSplitter, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CMethodMan, "LoadSplitter", DispLoadSplitter, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CMethodMan, "QueueFireScriptEvent", QueueFireScriptEvent, VT_EMPTY, VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

CMethodMan::CMethodMan(void)
{
	m_nLockNotificationCounter = 0;
	m_lActiveMethodPos = 0;
	m_ExecutionState = mesNothing;
	m_bPlaying = false;
	m_bRecording = false;
	m_bPaused = false;
	m_bNoLoop=false;
	m_bFormShown=false;
	m_nIndexBreakpoint=INT_MAX;
	m_strSetValueLoggingGroup = "";

	m_punkDocument = 0;

	m_punkNotifyCtrl	= 0;

	//Create NotifyController
	CLSID clsid;
	if( ::CLSIDFromProgID( _bstr_t(szNotifyCtrlProgID), &clsid ) != S_OK)
	{
		ASSERT( false );
	}

	HRESULT hr = CoCreateInstance(	clsid, GetControllingUnknown(), 
							CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&m_punkNotifyCtrl );	
	if(hr != S_OK)
	{
		m_punkNotifyCtrl = NULL;
		ASSERT( false );
	}

	EnableAutomation();
	SetName( _T("MethodMan") );
	Register(); // Register as event listener
}

CMethodMan::~CMethodMan(void)
{
	m_nLockNotificationCounter++;

	m_xMethodMan.SetActiveMethodPos(0);

	if (m_punkNotifyCtrl != 0)
	{
		m_punkNotifyCtrl->Release();
		m_punkNotifyCtrl = 0;
	}

	m_nLockNotificationCounter--;
}

void CMethodMan::Run()
{
	Stop();
	m_xMethodMan.Run();
}

void CMethodMan::Stop()
{
	m_xMethodMan.Stop();
}

void CMethodMan::Pause()
{
	m_bPaused = true;
}

void CMethodMan::Resume()
{
	m_bPaused = false;
}

void CMethodMan::Record()
{
	if(m_bPlaying || m_bRecording) Stop();
	m_xMethodMan.Record();
}

bool CMethodMan::IsRunning()
{
	BOOL bRes=false;
	m_xMethodMan.IsRunning(&bRes);
	return bRes!=0;
}

bool CMethodMan::IsRecording()
{
	BOOL bRes=false;
	m_xMethodMan.IsRecording(&bRes);
	return bRes!=0;
}

BSTR CMethodMan::GetCurrentStartingActionName()
{
	_bstr_t bstrName("No Action");
	CMethodManState st(&m_xMethodMan);
	if( (!st.m_bError) && st.m_pStep)
		bstrName = st.m_pStep->m_bstrActionName;
	
	return bstrName.copy();
}

void CMethodMan::RememberSection(LPCTSTR strName)
{
	INamedDataPtr sptrShellData(::GetAppUnknown());
	ASSERT(sptrShellData!=0);
	if(sptrShellData==0) return;

	CMethodManState st(&m_xMethodMan);
	if(st.m_bError || st.m_pStep==0) return;

	_bstr_t bstrPathNamedData;
	_bstr_t bstrPathShellData;
	st.m_pStep->m_ptrNamedData->GetCurrentSection(bstrPathNamedData.GetAddress());
	sptrShellData->GetCurrentSection(bstrPathShellData.GetAddress());

	st.m_pStep->m_ptrNamedData->SetupSection(_bstr_t(strName));
	sptrShellData->SetupSection(_bstr_t(strName));
	RecursiveAddEntry2(st.m_pStep->m_ptrNamedData, sptrShellData);

	// ����� ���� ��������� ��� � �������� ���������
	st.m_pStep->m_ptrNamedData->SetupSection( bstrPathNamedData );
	sptrShellData->SetupSection( bstrPathShellData );
}

void CMethodMan::ImportShellData(LPCTSTR strName)
{ // ������ ����� shell.data - ���, ����� �����
	sptrIFileDataObject2 sptrF;
	sptrF.CreateInstance( szNamedDataProgID );
	if(sptrF==0)
		return;

	sptrF->LoadTextFile( _bstr_t(strName) );
	LoadDisabledKeys();

	::ImportShellData( sptrF, m_pDisabledKeys );
    
	return;
}

void CMethodMan::SaveFilteredShellData(LPCTSTR strName)
{
	INamedDataPtr sptrShellData(::GetAppUnknown());
	ASSERT(sptrShellData!=0);
	if(sptrShellData==0) return;

	INamedDataPtr sptrNamedData;
	sptrNamedData.CreateInstance( szNamedDataProgID ); // ����� ������ NamedData

	LoadDisabledKeys(); // ��������� ������ ����������� ������

	CSubstMap map; // ���������� ������ �����������

	_bstr_t bstrPathShellData;
	_bstr_t bstrPathNamedData;
	sptrShellData->GetCurrentSection(bstrPathShellData.GetAddress());
	sptrNamedData->GetCurrentSection(bstrPathNamedData.GetAddress());

	RecursiveAddEntry(sptrNamedData, sptrShellData, "\\",
		m_pDisabledKeys.ptr(), &map);

	sptrNamedData->SetupSection( bstrPathNamedData ); // ������ ���, ��� ����
	sptrShellData->SetupSection( bstrPathShellData );


	sptrIFileDataObject2 sptrF(sptrNamedData);
	sptrF->SaveTextFile( _bstr_t(strName) );
}

BSTR CMethodMan::GetActionNameByIndex(long nIndex)
{
	_bstr_t bstrName("");
	CMethodManState st(&m_xMethodMan);
	if( st.m_bError ) return bstrName.copy();

	long lStepPos = 0;
	st.m_sptrMethod->GetStepPosByIndex(nIndex, &lStepPos);
	if(!lStepPos) return bstrName.copy();

	CMethodStep* pStep=0;
	st.m_sptrMethod->GetNextStepPtr(&lStepPos, &pStep);
	if(pStep==0) return bstrName.copy();

	bstrName = st.m_pStep->m_bstrActionName;
	return bstrName.copy();
}

void CMethodMan::StoreStepAs(LPCTSTR strName)
{	// �������� ��� ��� ����� ���! ����������� ��, ������!
	CMethodManState st(&m_xMethodMan);
	if(!st.m_pStep) return;
	if( ! (st.m_pStep->m_dwFlags & msfWriting) ) return;
	// ��������, ������ ���� ��� ���������.

	FinishAddStep(_bstr_t(strName)); // �������� ��� - ��� �������� ������

	// �������� ������ ����� ����� - ��� ��, ��� �������� �� ����
	AddStep(st.m_pStep->m_bstrActionName);
}

void CMethodMan::CancelStepStoring()
{
	CMethodManState st(&m_xMethodMan);
	if(!st.m_pStep) return;
	if( ! (st.m_pStep->m_dwFlags & msfWriting) ) return;
	// ��������, ������ ���� ��� ���������.

	// ������� ��� �����
	IMethodChangesPtr sptrChanges(st.m_sptrMethod);
	sptrChanges->Undo();
}

void CMethodMan::DeleteCurrentMethod()
{
	long lpos=0;
	m_xMethodMan.GetActiveMethodPos(&lpos);
	if(lpos)
	{
		m_xMethodMan.DeleteMethod(lpos);
	}
}

void CMethodMan::RenameCurrentMethod(LPCTSTR strName)
{
	long lpos=0;
	m_xMethodMan.GetActiveMethodPos(&lpos);
	if(lpos)
	{
		IUnknownPtr ptrMethod;
		m_xMethodMan.GetNextMethod(&lpos, &ptrMethod);
		IMethodPtr sptrMethod(ptrMethod);

		if(sptrMethod)
		{
			_bstr_t bstrOldName;
			sptrMethod->GetName( bstrOldName.GetAddress() );

			CString	strPathDef = ::MakeAppPathName( "Methods" )+"\\";
			CString strPath = ::GetValueString( GetAppUnknown(), "\\Paths", "Methods", 
				(const char*)strPathDef );

			BOOL bOk = MoveFile(strPath+(char*)(bstrOldName)+".method", strPath+strName+".method");

			sptrMethod->SetName(_bstr_t(strName));
		}
		::FireEventNotify( GetControllingUnknown(), szEventChangeMethod, GetControllingUnknown(), 0, cncReset ); // 
		//::FireEventNotify( GetControllingUnknown(), szEventChangeMethod, GetControllingUnknown(), ptrMethod, cncActivate );
	}
}

void CMethodMan::RenameCurrentStep(LPCTSTR strName)
{
	CMethodManState st(&m_xMethodMan);
	if(st.m_bError || st.m_pStep==0) return;
	
	st.m_pStep->m_bstrForcedName = _bstr_t(strName);
	if(!m_nLockNotificationCounter)
	{
		tagMethodStepNotification msn = {cncChange, st.m_lStepPos};
		::FireEvent( GetControllingUnknown(), szEventChangeMethodStep, st.m_sptrMethod, 0, &msn, sizeof(msn) );
	}
}

BSTR CMethodMan::GetCurrentMethodName()
{
	_bstr_t bstrName;
	long lMethodPos=0;
	m_xMethodMan.GetActiveMethodPos(&lMethodPos);
	if(lMethodPos)
	{
		IUnknownPtr ptrMethod;
		m_xMethodMan.GetNextMethod(&lMethodPos,&ptrMethod);
		IMethodPtr sptrMethod(ptrMethod);
		if(sptrMethod)
		{
			long lStepPos=0;
			sptrMethod->GetName(bstrName.GetAddress());
		}
	}
	
	return bstrName.copy();
}

void CMethodMan::ClearMethodCache()
{
	DropAllCache();
}

long CMethodMan::GetCurrentStepFlags()
{
	CMethodManState st(&m_xMethodMan);
	if(st.m_bError || st.m_pStep==0) return 0;
	
	return st.m_pStep->m_dwFlags;
}

void CMethodMan::SetCurrentStepFlags(long nFlags)
{
	CMethodManState st(&m_xMethodMan);
	if(st.m_bError || st.m_pStep==0) return;
	
	st.m_pStep->m_dwFlags = nFlags;
	if(!m_nLockNotificationCounter)
	{
		tagMethodStepNotification msn = {cncChange, st.m_lStepPos};
		::FireEvent( GetControllingUnknown(), szEventChangeMethodStep, st.m_sptrMethod, 0, &msn, sizeof(msn) );
	}
}

LPDISPATCH CMethodMan::GetActiveStepNamedData()
{
	CMethodManState st(&m_xMethodMan);
	if(st.m_bError || st.m_pStep==0) return 0;
	
	IDispatchPtr disp = st.m_pStep->m_ptrNamedData;
	if(disp!=0) disp.AddRef();

	return disp;
}

LPDISPATCH CMethodMan::GetStepNamedData(long nIndex)
{
	CMethodManState st(&m_xMethodMan);
	if(st.m_bError) return 0;

	st.SeekTo(nIndex);
	if(!st.m_pStep) return 0;

	IDispatchPtr disp = st.m_pStep->m_ptrNamedData;
	if(disp!=0) disp.AddRef();

	return disp;
}

void CMethodMan::StartSetValueLogging2(LPCTSTR strName)
{	// �� ��, ��� StartSetValueLogging, �� ��������� ����������� �� ���� ������ ����
	// (������ �� ���������, ���� �� ��������� FinishSetValueLogging c ��� �� ������)
	// ������ ������ ���� SetValue � �������� ��� ��������
	// ����� � ������ - ����� ������� SetValue �� OnOk ������� StartSetValueLogging,
	// ����� - FinishSetValueLogging, ����� ��� ��������� � ��� � ����� �����������
	// �� ���� � ��� ����������� ����, ���������� �� shell.data ��������

	if ( !(m_bPlaying || m_bRecording) )
		return; // �������� ������ � ������� Play � Record

	if(m_strSetValueLoggingGroup != "")
		return; // ��� ������� �����-�� ������ - ���������� ��

	m_strSetValueLoggingGroup = strName;

	CMethodManState st(&m_xMethodMan);
	if(st.m_bError || st.m_pStep==0) return;

	sptrINamedDataLogger sptrLogger( GetAppUnknown() );
	if(sptrLogger==0) return;

	sptrLogger->SetLog(ndlkWrite, st.m_pStep->m_ptrNamedData);
}

void CMethodMan::FinishSetValueLogging2(LPCTSTR strName)
{
	if(strName != m_strSetValueLoggingGroup)
		return; // ���� ���� ������� �����-�� ������ - ��������� ����� ������ ��, �� �����-�� ������
	m_strSetValueLoggingGroup = ""; // ��������

	sptrINamedDataLogger sptrLogger( GetAppUnknown() );
	if(sptrLogger==0) return;
	sptrLogger->SetLog(ndlkWrite, 0);
}

void CMethodMan::StartSetValueLogging()
{	// ������ ������ ���� SetValue � �������� ��� ��������
	// ����� � ������ - ����� ������� SetValue �� OnOk ������� StartSetValueLogging,
	// ����� - FinishSetValueLogging, ����� ��� ��������� � ��� � ����� �����������
	// �� ���� � ��� ����������� ����, ���������� �� shell.data ��������
	return StartSetValueLogging2("default");
}

void CMethodMan::FinishSetValueLogging()
{
	return FinishSetValueLogging2("default");
}

void CMethodMan::DispStoreSplitter(LPCTSTR strSectionName)
{
	StoreSplitter(strSectionName, ::GetAppUnknown());
}

void CMethodMan::DispLoadSplitter(LPCTSTR strSectionName)
{
	LoadSplitter(strSectionName, ::GetAppUnknown(), 
		lssSwitchViews + lssActivateObjects + lssRestoreZoom );
}

bool CMethodMan::NeedRepeatMethodLoop()
{
	if(m_bNoLoop) return false; // �����, ������ m_bNoLoop ������������ m_bRecording?
	_variant_t var = _exec_script_function("NeedRepeatMethodLoop");
    return long(var)!=0;
}

bool CMethodMan::StartMethodDisabled()
{
	_variant_t var = _exec_script_function("StartMethodDisabled");
    return long(var)!=0;
}

bool CMethodMan::ExecuteStep(CMethodStep &step)
{
	time_test ttt("CMethodMan::ExecuteStep");
	// ������ ������ �������
	if(step.m_bstrActionName == _bstr_t(szBeginMethodLoop)) return true;
	if(step.m_bstrActionName == _bstr_t(szEndMethodLoop)) return true;

	m_bPaused = false; // �� ������ ������

	IApplicationPtr sptrApp = ::GetAppUnknown();
	IUnknownPtr ptrActionMan = 0;
	if(sptrApp!=0) sptrApp->GetActionManager( &ptrActionMan );
	IActionManagerPtr sptrActionMan = ptrActionMan;

	// � ������ � � ����� ������� ���� ����������� ������������� �����
	if(sptrActionMan!=0) sptrActionMan->TerminateInteractiveAction();

	step.ApplyToShellData(); // �������� ��� ��������� � shell.data

	bool bActivateObjects = ::GetValueInt(::GetAppUnknown(),"\\Methodics","ActivateObjectsOnExec", 1) != 0; // ���������� ������� ���, ��� �������� ��� ������ ��������?
	bool bSwitchView = ::GetValueInt(::GetAppUnknown(),"\\Methodics","SwitchViewOnExec", 1) != 0; // ����������� ����� ���, ��� �������� ��� ������ ��������?
	bool bRestoreZoom = ::GetValueInt(::GetAppUnknown(),"\\Methodics","RestoreZoom", 1) != 0; // ��������������� ���?
	DWORD dwLoadSplitterFlags = 0;
	if(bSwitchView) dwLoadSplitterFlags |= lssSwitchViews;
	if(bActivateObjects) dwLoadSplitterFlags |= lssActivateObjects;
	if(bRestoreZoom) dwLoadSplitterFlags |= lssRestoreZoom;

	if( (step.m_dwFlags & msfStateless) == 0 )
	{ // ���� ��� �� stateless (���� ��������)
		if(dwLoadSplitterFlags)
		{
			if(	step.m_dwFlags & msfInteractive )
			{	// ���� ��� ������������� - �� ����� ����������, ��� ����� ��� ��� ������,
				// � ��������� �����������. ���� ���������� - ������, ����� Before.
				LoadSplitter("\\StateAfterAction", step.m_ptrNamedData, dwLoadSplitterFlags);
				DWORD dwflags = get_action_flags( step.m_bstrActionName, afEnabled );
				if( !(dwflags & afEnabled) )
					LoadSplitter("\\StateBeforeAction", step.m_ptrNamedData, dwLoadSplitterFlags | lssActiveViewOnly);
			}
			else
				LoadSplitter("\\StateBeforeAction", step.m_ptrNamedData, dwLoadSplitterFlags | lssActiveViewOnly);
			    // �� BeforeAction ������ ������ �������� ����� - ��� ������������� ������ ���
		}
	}

	DWORD dwflags = get_action_flags( step.m_bstrActionName, afEnabled );
	if( !(dwflags & afEnabled) )
	{
		_variant_t vars[1] = { step.m_bstrActionName };
		::FireScriptEvent("CannotExecuteStep", 1, vars);
		Stop();
		return false;
	}

	if(step.m_dwFlags & msfHasPropPage)
		::SetValue( ::GetAppUnknown(), "\\FormManager", "DisableFormShow", step.m_dwFlags & msfShowPropPage ? 0L : 1L );
		// ������ ��������� � shell.data, ����������� ����� ����

	// 02.11.2004: ��������� ��������� ������������� �����
	if(IsInteractiveAction(step.m_bstrActionName))
	{ // ���� ������������� �����
		if(sptrActionMan!=0) sptrActionMan->ExecuteAction( step.m_bstrActionName, 0, 0 );
	}
	else
	{
		// [vanek] : ��������� ������, ������� �� �������� ������� (������ ��������� ������� �������� �� ����� ������ ���
		// ������ �������� �������) - 22.10.2004
		_bstr_t bstr_script( _T("") );
		if( step.m_bstrActionName.length() )
		{
			if( ((TCHAR*)(step.m_bstrActionName))[0] != _T('\n') && ((TCHAR*)(step.m_bstrActionName))[0] != _T('\r') )
				bstr_script = _T("ActionManager.");
			
			bstr_script += step.m_bstrActionName;
		}
		::ExecuteScript( bstr_script );
	}

	return true;
}

bool CMethodMan::PostExecuteStep(CMethodStep &step)
{	// ����������� ����� ExecuteStep - �� ����� ����� ���� ������������� ���������� (e.g. ��� ������������� �����)
	if(step.m_dwFlags & msfHasPropPage)
		::SetValue( ::GetAppUnknown(), "\\FormManager", "DisableFormShow", long(0) );
		// ������� ������ ��������� � �������� ���������

	return true;
}

bool CMethodMan::DropAllCache()
{
	// ���������� �� ������ ����� �������� CacheDir
	WIN32_FIND_DATA FileData;
	HANDLE hSearch;
	CString strDirPath = GetCacheDir();

	hSearch = FindFirstFile( strDirPath + "????????_CACHE.*", &FileData );
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		do
		{
			DeleteFile( strDirPath + FileData.cFileName );
		}
		while (FindNextFile(hSearch, &FileData));
		FindClose(hSearch);
	} 

	if(!m_nLockNotificationCounter)
	{
		CMethodManState st(&m_xMethodMan);
		if(!st.m_bError)
		{
			tagMethodStepNotification msn = {cncReset, 0};
			::FireEvent( GetControllingUnknown(), szEventChangeMethodStep, st.m_sptrMethod, 0, &msn, sizeof(msn) );
		}
	}

	return true;
}

HRESULT CMethodMan::XMethodMan::Run()
{
	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);

	CMethodManState st(this);
	if(st.m_bError || st.m_sptrMethod==0) return E_FAIL; // ��������� ����� ������ �������� ��������

	if(pThis->StartMethodDisabled())
	{
		Stop();
		return S_FALSE;
	}

	if(st.m_lStepPos==0) pThis->DropAllCache();
	// ��� �������� ���� ��������� ����� - �������� � ��������� ������

	if(!pThis->m_nLockNotificationCounter)
	{
		::FireEventNotify( pThis->GetControllingUnknown(), szEventMethodRun, pThis->GetControllingUnknown(), 0, 0 );
		::FireScriptEvent("OnRunMtd");
	}

	pThis->m_bPlaying = true; // � �����-��, �� ���� m_bPlaying - ���� �� m_ExecutionState... �� ���� �������
    pThis->m_nIndexBreakpoint = INT_MAX; // �������� �� �����

	// ���������� ������
	pThis->m_ExecutionState = mesRewindDead; // �������� � ��������� ����� �� ������
	SetValue(::GetAppUnknown(), "\\MainFrame", "FireEventInMainLoop", long(1)); // ����������� � MainLoop; �� ���� ����� ���������

	return S_OK;
	// -----------------------

	// TODO � RunStep: // [vanek] BT2000:4016 ���� ���� ������ - �� �������� ��� ��������� - 17.09.2004

	return S_OK;
}

// ��������������� ������� ��� RunStep
void CMethodMan::StepForward(bool bLoadCache)
{ // ����������� ��������� �� ��� ������
	CMethodManState st(&m_xMethodMan);
	if(st.m_bError) return;

	while(1)
	{
		st.StepForward();
		if(!st.m_pStep) break; // ����� �� ����� �������� - ����
		if(st.m_pStep->m_dwFlags & msfChecked ) break; // ����� �� ������������ ����
	}

	st.m_sptrMethod->SetActiveStepPos(st.m_lStepPos, bLoadCache);
}

void CMethodMan::StepBack(bool bLoadCache)
{ // ����������� ��������� �� ��� �����
	CMethodManState st(&m_xMethodMan);
	if(st.m_bError) return;

	while(1)
	{
		st.StepBack();
		if(!st.m_pStep) break; // ����� �� ������ �������� - ����
		if(st.m_pStep->m_dwFlags & msfChecked ) break; // ����� �� ������������ ����
	}

	st.m_sptrMethod->SetActiveStepPos(st.m_lStepPos, bLoadCache);
}

bool CMethodMan::IsAlive()
{ // ������� ��� - �����?
	CMethodManState st(&m_xMethodMan);
	if(st.m_bError) return false;

	BOOL bCached = FALSE;
	m_xMethodMan.IsCached(st.m_sptrMethod, st.m_lStepPos, &bCached);
	return bCached!=FALSE;
}

void CMethodMan::MakeAlive()
{ // ������� ������� ��� ����� (���� � ���� ���� ����������� ���������)
	CMethodManState st(&m_xMethodMan);
	if(st.m_bError) return;

	if(st.m_pStep && (st.m_pStep->m_dwFlags & msfStateless) )
		return; // ��� stateless ������ �� ������

	m_xMethodMan.StoreCache(st.m_sptrMethod, st.m_lStepPos);
	return;
}

void CMethodMan::MakeDead()
{ // ������� ������� ��� ������� (���� � ���� ���� ����������� ���������)
	CMethodManState st(&m_xMethodMan);
	if(st.m_bError) return;

	m_xMethodMan.DropCache(st.m_sptrMethod, st.m_lStepPos);
	return;
}

bool CMethodMan::IsLoopBegin()
{ // ������� ��� - ������ �����?
	CMethodManState st(&m_xMethodMan);
	if(st.m_bError) return false;
	if(!st.m_pStep) return false;
	return st.m_pStep->m_bstrActionName == _bstr_t(szBeginMethodLoop);
}

bool CMethodMan::IsLoopEnd()
{ // ������� ��� - ����� �����?
	CMethodManState st(&m_xMethodMan);
	if(st.m_bError) return false;
	if(!st.m_pStep) return false;
	return st.m_pStep->m_bstrActionName == _bstr_t(szEndMethodLoop);
}

bool CMethodMan::IsBreakpoint()
{ // ������� ��� - breakpoint?
	CMethodManState st(&m_xMethodMan);
	return st.m_lStepIndex >= m_nIndexBreakpoint;
}

void CMethodMan::AddStep(_bstr_t bstrActionName)
{ // �������� ��� � ������� �������

	// ������������: ���� ��� ������������ ��� - ��������� ��� ������
	CMethodManState st(&m_xMethodMan);
	if( st.m_pStep && (st.m_pStep->m_dwFlags & msfWriting) )
	{ // ������� ��� �� �������� - ������������ ���
		FinishAddStep("");
	}

	// �������� ���, ������� ����� ������
	CMethodStep step;
	step.m_bstrActionName = bstrActionName; // �������� ��� ������������ �����
	step.m_bstrForcedName = bstrActionName;
	step.m_bstrUserName = GetUserActionName(step.m_bstrForcedName); // �������� ��������� ���

	st.Sync();
	if(!st.m_bError)
	{
		IMethodChangesPtr sptrChanges(st.m_sptrMethod);
		sptrChanges->AddStep(st.m_lStepIndex+1, &step); // ������� ��� � ������� ��� ��������
	}

	st.Sync();
	ASSERT(st.m_pStep);
	if(st.m_bError || !st.m_pStep) return;

	st.m_pStep->m_dwFlags |= msfChecked; // ������� ���, ��� �����������
	st.m_pStep->m_dwFlags |= msfWriting; // ������� ���, ��� ������������

	if(st.m_pStep->m_ptrNamedData!=0)
	{
		::DeleteEntry( st.m_pStep->m_ptrNamedData, "\\StateBeforeAction" ); // ������� ������ - ���� ������ �� ����
		bool bSimpleStoreSplitter = ::GetValueInt( ::GetAppUnknown(), "\\Methodics", "SimpleStoreSplitter", 1  ) != 0;
		StoreSplitter("\\StateBeforeAction", st.m_pStep->m_ptrNamedData, bSimpleStoreSplitter); // �������� ��������� ���������
	}
}

void CMethodMan::FinishAddStep(_bstr_t bstrForcedName)
{	// ��������� ������ ���� - ��������� ���������������� ���,
	// ��������� ��������� � ����� � �.�.
	CMethodManState st(&m_xMethodMan);
	if(st.m_bError || !st.m_pStep) return;

	if( ! (st.m_pStep->m_dwFlags & msfWriting) ) return;
	// ��������, ������ ���� ��� ���������.

	st.m_pStep->m_dwFlags &= ~msfWriting; // ������� ������� ��������������

	if( bstrForcedName != _bstr_t("") )
        st.m_pStep->m_bstrForcedName = bstrForcedName;
	if( st.m_pStep->m_bstrForcedName == _bstr_t("") )
		st.m_pStep->m_bstrForcedName = st.m_pStep->m_bstrActionName;
    st.m_pStep->m_bstrUserName = GetUserActionName(st.m_pStep->m_bstrForcedName); // �������� ��������� ���

	if( ::GetValueInt( ::GetAppUnknown(), "\\FormManager", "PageExecuted", 0L ) )
		st.m_pStep->m_dwFlags |= msfHasPropPage|msfShowPropPage;
	else
		st.m_pStep->m_dwFlags &= ~msfHasPropPage;

	if(st.m_pStep->m_ptrNamedData!=0)
	{
		::DeleteEntry( st.m_pStep->m_ptrNamedData, "\\StateAfterAction" ); // ������� ������ - ���� ������ �� ����
		bool bSimpleStoreSplitter = ::GetValueInt( ::GetAppUnknown(), "\\Methodics", "SimpleStoreSplitter", 1  ) != 0;
		StoreSplitter("\\StateAfterAction", st.m_pStep->m_ptrNamedData, bSimpleStoreSplitter); // �������� ��������� ���������
	}

	m_xMethodMan.StoreCache(st.m_sptrMethod, st.m_lStepPos);

	//st.m_sptrMethod->UpdateActiveStep(); // �������� � ��� ��������� ����

	st.m_sptrMethod->SetModifiedFlag(TRUE);

	_variant_t varName = st.m_pStep->m_bstrForcedName; // ���, ������������ �������������
	::FireScriptEvent("AfterRecordMethodStep", 1, &varName);
}

CMethodStep* CMethodMan::CurrentStep()
{ // �������� ��������� �� ������� ���
	CMethodManState st(&m_xMethodMan);
	if(st.m_bError) return 0;
	
	return st.m_pStep;
}

bool CMethodMan::NeedWait()
{ // ��������, �� ���� �� �� ���������� ������������� ������-�� �������� (�����. �����, �����)
	if(m_bPaused) return true; // ���� �������� ����� ����� - ����
	if(m_bFormShown) return true; // ���� � ��� ����� �� ������ - ��������� �� ����������
	if(IsInteractiveRunning()!=0) return true; // ���� �������� ������������� ����� - ���� �����

	return false;
}

void CMethodMan::RunStep()
{ // ��������� ���� ��� ��������
	CString s;
	s.Format( "CMethodMan::RunStep %i", m_ExecutionState );
	time_test ttt( s );
	switch(m_ExecutionState)
	{
	case mesNothing: // ����� �� �����
		return;
		break;
	case mesExecute: // ��������� ���
		{
			CMethodStep* pStep = CurrentStep();
			if(pStep)
			{
				m_ExecutionState = mesWait; // ��� ������ ��������� ��� - ����� ��� ����������
				m_bPaused = false; // SBT 2025 - ������������� �� ������,
				// ���� ��������� �����-�� ������� Pause � �� ������� Resume
                
				ExecuteStep( *pStep ); // ���������� ��������� ������ �������� ����
				if(IsInteractiveRunning())
				{
					_bstr_t bstrGroup = GetActionGroup(pStep->m_bstrActionName);
					_variant_t vars[2] = { bstrGroup, pStep->m_bstrActionName };
					::FireScriptEvent("OnInteractiveMethodStep", 2, vars);
				}
			}
			else
			{
				m_ExecutionState = mesNothing;
				m_bPlaying = false; // � �����-��, �� ���� m_bPlaying - ���� �� m_ExecutionState... �� ���� �������
				if(!m_nLockNotificationCounter)
				{
					::FireEventNotify( GetControllingUnknown(), szEventMethodStop, GetControllingUnknown(), 0, 0 );
					::FireScriptEvent("OnRunMtdComplete");
				}
				// 27.04.2006 build 103, BT 5188
				CMethodManState st(&m_xMethodMan);
				st.StepBack(); // �� ��������� ��� ��������
				// �������� �������� � ������������ �� StepAfterAction ���������� ������������ ����
				m_xMethodMan.LoadCache(st.m_sptrMethod, st.m_lStepPos, lcfRestoreView|lcfRestoreSelection|lcfRestoreZoom);
			}
		}
		break;
	case mesWait:
		if(!NeedWait())
		{
			m_ExecutionState = mesNext;
			CMethodStep* pStep = CurrentStep();
			if(pStep) PostExecuteStep( *pStep );
		}
		break;
	case mesNext: // ������� �� ��������� ��� ��� ��� ����������
		if(!IsAlive()) MakeAlive();
		if( IsBreakpoint() )
		{
			m_ExecutionState = mesNothing;
			m_bPlaying = false; // � �����-��, �� ���� m_bPlaying - ���� �� m_ExecutionState... �� ���� �������
			if(!m_nLockNotificationCounter)
			{
				::FireEventNotify( GetControllingUnknown(), szEventMethodStop, GetControllingUnknown(), 0, 0 );
				::FireScriptEvent("OnStopMtd");
				if(m_bRecording)
				{
					::FireEventNotify( GetControllingUnknown(), szEventMethodRecord, GetControllingUnknown(), 0, 0 );
					::FireScriptEvent("OnStartRecordMtd");
				}
			}
			break;
		}
		if(IsLoopEnd() && NeedRepeatMethodLoop() ) m_ExecutionState = mesRewindLoop;
		else
		{
			m_ExecutionState = mesExecute;
			StepForward(false);
		}
		break;
	case mesRewindDead: // ��������� ����� �� ������� ������ ��� �� ������
		if(0==CurrentStep()) m_ExecutionState = mesNext;
		else if(IsAlive()) m_ExecutionState = mesNext;
		else StepBack();
		break;
	case mesRewindLoop: // ��������� ����� �� ������ �����
		if(0==CurrentStep()) m_ExecutionState = mesNext; // ��������� ������ - ��� ������ �����
		else if(IsLoopBegin()) m_ExecutionState = mesNext;
		else
		{
			MakeDead();
			StepBack(false);
		}
		break;
	}

	return;
}

// ������ ���������� �������� � ��������� "������" ����, � ���������� ����������,���� ������ ������� �� 
// ����������� (��� ��������� ��� �������� ��� �� "�����"), �� ��� ����������� ��� � ������ (�.�. ��� ����� 
// �������� ����� Run), � ���� ���� �������� ��� � �� "�����", �� ��������� �� ���������� � ����.
HRESULT CMethodMan::XMethodMan::RunFromActive()
{
	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);
	Stop();

	CMethodManState st(this);
	if(st.m_bError || st.m_sptrMethod==0) return E_FAIL;

	// ������ ����, ������� � ��������� ����
	if(0==st.m_lStepPos) // ���� ��� ��������� ���� - ���� �������� ���� ���
		pThis->DropAllCache();
	else
	{
		while(st.m_lStepPos)
		{ // ����� �� �����, ������� � ��������
			DropCache(st.m_sptrMethod, st.m_lStepPos);
			st.StepForward();
		}
	}

    return Run( );
}

class CLockerFlag
{
public:
	CLockerFlag( BOOL *pbFlag ) { m_pbFlag = pbFlag; *m_pbFlag = TRUE; }
	~CLockerFlag()	{ *m_pbFlag = FALSE; }
	BOOL	*m_pbFlag;
};

HRESULT CMethodMan::XMethodMan::Stop(BOOL bStopRun, BOOL bStopRecord)
{
	static BOOL bLock = FALSE; // �� ������ ������ - ����� �������� ��������
	// (���� �� - ��������� ������-������ ��� ���� Stop �������)
	if(bLock) return E_FAIL;
	CLockerFlag locker( &bLock );

	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);

	IApplicationPtr sptrApp = ::GetAppUnknown();
	IUnknownPtr ptrActionMan = 0;
	if(sptrApp!=0) sptrApp->GetActionManager( &ptrActionMan );
	IActionManagerPtr sptrActionMan = ptrActionMan;
	if(sptrActionMan!=0) sptrActionMan->TerminateInteractiveAction(); // [SBT 1168]

	::ExecuteScript( _bstr_t("FormManager.PressCancel") );

	pThis->CancelStepStoring(); // ���� ������ ��� � ���������� ��� - �������

	if(bStopRun && pThis->m_bPlaying)
	{
		pThis->m_bPlaying = false;
		pThis->m_ExecutionState = mesNothing;
		if(!pThis->m_nLockNotificationCounter)
			pThis->QueueFireScriptEvent("OnStopMtd");
	}

	if(bStopRecord && pThis->m_bRecording)
	{
		pThis->m_bRecording = false;
		if(!pThis->m_nLockNotificationCounter)
			pThis->QueueFireScriptEvent("OnStopRecordMtd");

		{ // BT 4355
			long lpos = pThis->m_lActiveMethodPos;
			IUnknownPtr ptrMethod;
			GetNextMethod(&lpos,&ptrMethod);
			IMethodDataPtr sptrMethodData(ptrMethod);

			if(sptrMethodData!=0)
				sptrMethodData->SetActiveStepPos(0, false); // �� ���� ���������� �������� ���
		}
	}

	// 21.06.2006 build 107 BT 5276
	// �� ������ ������ ��� ����� ������� ������ ������ ���������
	::SetValue( ::GetAppUnknown(), "\\FormManager", "DisableFormShow", long(0) );


	if(!pThis->m_nLockNotificationCounter)
		::FireEventNotify( pThis->GetControllingUnknown(), szEventMethodStop, pThis->GetControllingUnknown(), 0, 0 );

	return S_OK;
}

bool CMethodMan::LoadDisabledKeys()
{
	char sz0[] =
		"\\Paths\n"
		"\\_method\n"
		"\\_step\n"
		"\\StateBeforeAction\n"
		"\\StateAfterAction\n";
	m_pDisabledKeys.alloc(strlen(sz0)+1);
	strcpy( m_pDisabledKeys.ptr(), sz0 );
	CString	strDisabledKeysFile = ::MakeAppPathName( "disabled.method.keys" );
	FILE *f=fopen(strDisabledKeysFile, "rb");
	if(!f) return false;
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	if(0<size && size<1000000) // ����� ���� �� ������ ���������
	{
		m_pDisabledKeys.alloc(strlen(sz0)+size+1);
		strcpy( m_pDisabledKeys.ptr(), sz0 );
		fseek(f, 0, SEEK_SET);
		fread(m_pDisabledKeys.ptr()+strlen(sz0), 1, size, f);
		m_pDisabledKeys.ptr()[strlen(sz0)+size]=0;
	}
	fclose(f);
	return true;
}

bool CMethodMan::LoadShellData(BSTR bstrFileName)
{ // ��������� shell.data �� ��������� ����� - ����� m_pDisabledKeys
	// ���� ��������� ����� CMethodStep, �� ���� �� ������� - ���� ��������� ���������� ��������
	// � ��� ����� - ���-������ ������ ����� � LoadTextFile
	sptrIFileDataObject2 sptrF;
	sptrF.CreateInstance( szNamedDataProgID );
	if(sptrF==0) return false;

	sptrF->LoadTextFile( bstrFileName );
	LoadDisabledKeys();

	INamedDataPtr sptrND(sptrF);
	if(sptrND==0) return false;

	CMethodStep step;
	//step.RecursiveAddEntry(sptrND,"\\", m_pDisabledKeys.ptr());

	step.ApplyToShellData(); // �������� ��� ��������� � shell.data
	return true;
}


HRESULT CMethodMan::XMethodMan::Record()
{
	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);

	if(pThis->m_bPlaying || pThis->m_bRecording) Stop();

	CMethodManState st(this);

	if(st.m_bError || (st.m_sptrMethod==0)) return E_FAIL;

	// ��������� ������ ������ shell.data, ����������� � �������� � ��������
	pThis->LoadDisabledKeys();

	pThis->m_bRecording = true;

	// ������ �������� ������ - ��������� ���� undo/redo
	IMethodChangesPtr sptrMethodChanges(st.m_sptrMethod);
	if(sptrMethodChanges) // ��������� undo/redo
		sptrMethodChanges->ClearUndoRedo();

	// -------------

	if(0==st.m_lStepPos)
	{ // ���� ��� ��������� ���� - ����������� ���������, ����� ������ � �����
		st.StepBack();
		st.m_sptrMethod->SetActiveStepPos(st.m_lStepPos);
		st.Sync();
	}

	if(pThis->IsLoopEnd())
	{ // ���� ������� ����� ����� - ������ ������ �����
		st.StepBack();
		st.m_sptrMethod->SetActiveStepPos(st.m_lStepPos);
		st.Sync();
	}

	if(!pThis->IsAlive())
	{ // ���� ������� ��� ������� - ��������� �� ����
		if(!pThis->m_nLockNotificationCounter)
		{
			::FireEventNotify( pThis->GetControllingUnknown(), szEventMethodRun, pThis->GetControllingUnknown(), 0, 0 );
			::FireScriptEvent("OnRunMtd");
		}

		pThis->m_nIndexBreakpoint = st.m_lStepIndex;
		pThis->m_bPlaying = true;
		pThis->m_ExecutionState = mesRewindDead;
		SetValue(::GetAppUnknown(), "\\MainFrame", "FireEventInMainLoop", long(1)); // ����������� � MainLoop; �� ���� ����� ���������
	}
	else
	{ // ���� ������ ����� ������ ������
		if(!pThis->m_nLockNotificationCounter)
		{
			::FireEventNotify( pThis->GetControllingUnknown(), szEventMethodRecord, pThis->GetControllingUnknown(), 0, 0 );
			::FireScriptEvent("OnStartRecordMtd");
		}
	}

	return S_OK;
}

HRESULT CMethodMan::XMethodMan::IsRunning(BOOL *pbVal)
{
	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);
	if(!pbVal) return E_INVALIDARG;
	*pbVal = pThis->m_ExecutionState != mesNothing;
	return S_OK;
}

HRESULT CMethodMan::XMethodMan::IsRecording(BOOL *pbVal)
{
	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);
	if(!pbVal) return E_INVALIDARG;
	*pbVal = pThis->m_bRecording;
	return S_OK;
}

HRESULT CMethodMan::XMethodMan::AddMethod(IUnknown* punkMethod, long lInsertBefore, long *plNewPos /*= 0*/ )
{
	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);
	if(!punkMethod) return E_INVALIDARG;
	long lPos = pThis->m_Methods.insert_before(punkMethod, lInsertBefore);
	pThis->m_MethodPosMap.set(lPos,lPos);
	if( plNewPos )
		*plNewPos = lPos;

	if(!pThis->m_nLockNotificationCounter)
	{
		long lPos1=lPos;
		IUnknownPtr punk;
		GetNextMethod(&lPos1, &punk);
		::FireEventNotify( pThis->GetControllingUnknown(), szEventChangeMethod, pThis->GetControllingUnknown(), punk, cncAdd );
	}

	return S_OK;
}

HRESULT CMethodMan::XMethodMan::DeleteMethod(long lPos)
{
	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);
	if(!lPos) return E_INVALIDARG;

	if(!pThis->m_MethodPosMap.find(lPos)) return E_INVALIDARG;

	// ���� ������� �������� ������ - ������� �� ���������
	if(pThis->m_lActiveMethodPos == lPos)
	{
		long lPos1 = pThis->m_Methods.next(lPos);
		if(!lPos1) // ���� ��������� �������� ��� - ����������� ����������
			lPos1 = pThis->m_Methods.prev(lPos);
		SetActiveMethodPos(lPos1);
	}

	if(!pThis->m_nLockNotificationCounter)
	{
		long lPos1=lPos;
		IUnknownPtr punk;
		GetNextMethod(&lPos1, &punk);
		::FireEventNotify( pThis->GetControllingUnknown(), szEventChangeMethod, pThis->GetControllingUnknown(), punk, cncRemove );
	}

	{
		long lpos2=lPos;
		IUnknownPtr ptrMethod;
		GetNextMethod(&lpos2, &ptrMethod);
		IMethodPtr sptrMethod(ptrMethod);

		if(sptrMethod)
		{
			_bstr_t bstrOldName;
			sptrMethod->GetName( bstrOldName.GetAddress() );

			CString	strPathDef = ::MakeAppPathName( "Methods" )+"\\";
			CString strPath = ::GetValueString( GetAppUnknown(), "\\Paths", "Methods", 
				(const char*)strPathDef );

			BOOL bOk = DeleteFile(strPath+(char*)(bstrOldName)+".method");
		}
	}

	pThis->m_Methods.remove(lPos);
	pThis->m_MethodPosMap.remove_key(lPos);

	return S_OK;
}

HRESULT CMethodMan::XMethodMan::SetActiveMethodPos( long lPos )
{
	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);

	IApplicationPtr sptrApp = ::GetAppUnknown();
	IUnknownPtr ptrActionMan = 0;
	if(sptrApp!=0) sptrApp->GetActionManager( &ptrActionMan );
	IActionManagerPtr sptrActionMan = ptrActionMan;
	// ����������� ������������� �����
	if(sptrActionMan!=0) sptrActionMan->TerminateInteractiveAction();

	if(pThis->m_bPlaying || pThis->m_bRecording) Stop();

	// �������� �� ������������� �������� - ���������� shell.data � �������� ����
	if( lPos != pThis->m_lActiveMethodPos)
	{
		if( pThis->m_lActiveMethodPos )
		{ // ������������� �������� �������� - ������� �, ���� ����, ������� �� shell.data
			long lPos1 = pThis->m_lActiveMethodPos;
			IUnknownPtr ptrMethod;
			GetNextMethod(&lPos1,&ptrMethod);
			IMethodDataPtr sptrMethod(ptrMethod);

			// ������� ��������� ��������� � �������� ����, ������ ��� �������������
			//sptrMethod->UpdateActiveStep();  // 06.10.2005 build 91 - ������, ������ �������� ������ ��� ���������� ������

			// ������� shell.data, ���� ���� �����
			//sptrMethod->UpdateMethod();
			// check modified
			IMethodDataPtr sptr_mtd_data = ptrMethod;
			IMethodPtr sptr_mtd = ptrMethod;
			if( sptr_mtd_data != 0 && sptr_mtd != 0 )
			{
				BOOL	bmodified = FALSE;
				sptr_mtd_data->GetModifiedFlag( &bmodified );

				_bstr_t bstrt_name;
				sptr_mtd->GetName( bstrt_name.GetAddress() );
				_bstr_t bstrt_pathname;
				bstrt_pathname = ::GetValueString( GetAppUnknown(), "\\Paths", "Methods",	(const char*)"" );	
				if( bstrt_pathname.length( ) )
				{
					bstrt_pathname += bstrt_name + _T(".method");
				}


				bool bNeedSave = false;

				{ // 1-� ��������: ���� �������� �������� �� AutoSave - ���������
					INamedPropBagPtr sptr_propbag_mtd = ptrMethod;
					if( sptr_propbag_mtd != 0 )
					{
						variant_t var;
						sptr_propbag_mtd->GetProperty( _bstr_t(_T(szMtdPropAutoSave)), var.GetAddress() );
						if( var.vt == VT_I4 )
							bNeedSave = (var.lVal!=0);
					}
				}

				if( bmodified && !bNeedSave ) // 2-� ��������: ���� ���� �������� - ������� �����
				{
					CString str_title( _T("") ),
						str_msg( _T("") );

					str_title.LoadString( IDS_CHANGE_ACTIVE_METHOD_STR );
					str_msg.Format( IDS_METHOD_MODIFIED_STR, (TCHAR*)(bstrt_name) );

					if( bstrt_pathname.length( ) )
					{
						HWND	hwnd_main = 0;
						IApplicationPtr	ptrA(GetAppUnknown());
						if(ptrA!=0) ptrA->GetMainWindowHandle(&hwnd_main);

						bNeedSave = 
							( IDYES == ::MessageBox( hwnd_main, str_msg, str_title, MB_YESNO | MB_ICONQUESTION ) );
					}
				}

				if( bNeedSave )
				{
					// ������� ��������� ��������� � �������� ����, ������ ��� �������������
					//sptr_mtd_data->UpdateActiveStep();  // 06.10.2005 build 91 - ������, ������ �������� ������ ��� ���������� ������
					sptr_mtd->StoreFile( bstrt_pathname );
				}
				else
					sptr_mtd->LoadFile( bstrt_pathname );

			}
		}
		else
		{ // ���������������� ��������� �����
		}
	}

	{ // �������� ��� � ������ undo/redo ��� �������� ��������; "����������" �� ���������
		pThis->DropAllCache();
		long lMethodPos=0;
		GetActiveMethodPos(&lMethodPos);
		if(lMethodPos)
		{
			IUnknownPtr ptrMethod;
			GetNextMethod(&lMethodPos,&ptrMethod);
			IMethodChangesPtr sptrMethodChanges(ptrMethod);
			if(sptrMethodChanges) // ��������� undo/redo
				sptrMethodChanges->ClearUndoRedo();
		}
		pThis->m_punkDocument = 0;
	}

	if(!pThis->m_nLockNotificationCounter)
	{
		long lPos1=pThis->m_lActiveMethodPos;
		IUnknownPtr punk;
		GetNextMethod(&lPos1, &punk);
		::FireEventNotify( pThis->GetControllingUnknown(), szEventChangeMethod, pThis->GetControllingUnknown(), punk, cncDeactivate );
	}

	if( lPos != pThis->m_lActiveMethodPos)
	{ // ����� �������� - ��������� shell.data �� ���
		if( lPos )
		{ // �������������� �������� ��������
			long lPos1 = lPos;
			IUnknownPtr ptrMethod;
			GetNextMethod(&lPos1,&ptrMethod);
			INamedDataPtr sptrNamedData(ptrMethod);
			if(sptrNamedData)
			{
				// pThis->LoadDisabledKeys();
				// ::ImportShellData( sptrNamedData, pThis->m_pDisabledKeys );
				::ImportShellData2( sptrNamedData );
			}
		}
		else
		{ // ��������� ����� - ��������� shell.data ��� ����
		}
	}

	pThis->m_lActiveMethodPos = lPos;

	if(!pThis->m_nLockNotificationCounter)
	{
		long lPos1=lPos;
		IUnknownPtr punk;
		GetNextMethod(&lPos1, &punk);
		::FireEventNotify( pThis->GetControllingUnknown(), szEventChangeMethod, pThis->GetControllingUnknown(), punk, cncActivate );

		_variant_t varName = "";
		IMethodPtr sptrMethod(punk);
		if(sptrMethod!=0)
		{
			_bstr_t bstr;
			sptrMethod->GetName(bstr.GetAddress());
			varName = bstr;
		}
		::FireScriptEvent("OnChangeActiveMethod", 1, &varName);
	}

	return S_OK;
}

HRESULT CMethodMan::XMethodMan::GetActiveMethodPos( long *plPos )
{
	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);
	if(!plPos) return E_INVALIDARG;
	*plPos = pThis->m_lActiveMethodPos;
	return S_OK;
}

HRESULT CMethodMan::XMethodMan::GetFirstMethodPos( long *plPos )
{
	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);
	if(!plPos) return E_INVALIDARG;
	*plPos = pThis->m_Methods.head();
	return S_OK;
}

HRESULT CMethodMan::XMethodMan::GetNextMethod( long *plPos, IUnknown **pMethod )
{
	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);
	if(!plPos) return E_INVALIDARG;

	if(*plPos==0)
	{
		if(pMethod) *pMethod = 0;
		return GetFirstMethodPos(plPos);
	}

	if(!pThis->m_MethodPosMap.find(*plPos))
	{
		*plPos=0;
		if(pMethod) *pMethod = 0;
		return E_INVALIDARG;
	}

	if(pMethod!=0)
	{
		*pMethod = pThis->m_Methods.get(*plPos);
		if(*pMethod) (*pMethod)->AddRef();
	}

	*plPos = pThis->m_Methods.next(*plPos);

	return S_OK;
}

HRESULT CMethodMan::XMethodMan::Reload( )
{
	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);

	// ��������� �������
	SetActiveMethodPos(0);
    pThis->m_Methods.clear();
	pThis->m_MethodPosMap.clear();

	if(!pThis->m_nLockNotificationCounter)
		::FireEventNotify( pThis->GetControllingUnknown(), szEventChangeMethod, pThis->GetControllingUnknown(), 0, cncReset );

	_bstr_t bstrActiveMethod = ::GetValueString( ::GetAppUnknown(),"\\Methodics","ActiveMethod", "");

	// ��������� ��� �������� �� ��������
	CString	strPathDef = ::MakeAppPathName( "Methods" )+"\\";
	CString strPath = ::GetValueString( GetAppUnknown(), "\\Paths", "Methods", 
						(const char*)strPathDef );
    
	CFileFind ff;

	bool bFoundActive = false;

	SetActiveMethodPos( 0 );
	BOOL bFound = ff.FindFile( strPath + "*.method" );
	while( bFound )
	{
		bFound = ff.FindNextFile();

		IUnknownPtr ptr;
		ptr.CreateInstance( "Metodics.Method" );
		IMethodPtr sptrMethod(ptr);

        if(sptrMethod!=0)
		{
			sptrMethod->LoadFile( _bstr_t(ff.GetFilePath()) );
			sptrMethod->SetName( _bstr_t(ff.GetFileTitle()) );
			IMethodDataPtr sptrMethodData(ptr);
			if(sptrMethodData) sptrMethodData->SetModifiedFlag(FALSE);
			long lNewMethodPos = 0;
			AddMethod( sptrMethod, 0, &lNewMethodPos);
			if( _bstr_t(ff.GetFileTitle()) == bstrActiveMethod )
			{
				SetActiveMethodPos( lNewMethodPos );
				bFoundActive = true;
			}
		}
	}

	bool bVirtualFreeMode = 0 != ::GetValueInt( GetAppUnknown(), "\\Methodics", "VirtualFreeMode", 1 );
	if( (!bFoundActive) && (!bVirtualFreeMode) )
	{
		long lPos=0;
		GetFirstMethodPos(&lPos);
		SetActiveMethodPos( lPos );
	}

	return S_OK;
}

void CMethodMan::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	//_trace_file("notify.log", "%s", pszEvent);

	if( !strcmp( pszEvent, szEventAppOnMainLoop ) )
	{
		if(IsRunning())
		{
			RunStep();
			SetValue(::GetAppUnknown(), "\\MainFrame", "FireEventInMainLoop", long(1));
		}
		else if(IsRecording())
		{
			if(NeedWait()) // ���� �� ���� - ����������� ����
				SetValue(::GetAppUnknown(), "\\MainFrame", "FireEventInMainLoop", long(1));
			else
			{ // �������� ����������� - �������� ������ ���� (���� ����)
				CMethodManState st(&m_xMethodMan);
				if( st.m_pStep && (st.m_pStep->m_dwFlags & msfWriting) )
					FinishAddStep(""); // ������� ��� �� �������� - ������������ ���
			}
		}
		ProcessScriptEventQueue(); // ���� �� - ����������� ����� ������������, ������� ����������
	}

	else if( !strcmp( pszEvent, szEventBeforeActionExecute ) )
	{ // ����� ������� ����� ����� �� ������ ������ �������� ������������
		SetValue(::GetAppUnknown(), "\\MainFrame", "FireEventInMainLoop", long(1));
		// ����� ��� ���������� ����� - ��������
	}

	else if( !strcmp( pszEvent, szEventAppOnIdle ) )
	{
	}

	else if( !strcmp( pszEvent, szEventMainFrameOnEnterMode ) )
	{
		m_bFormShown = true;
	}

	else if( !strcmp( pszEvent, szEventMainFrameOnLeaveMode ) )
	{
		m_bFormShown = false;
		SetValue(::GetAppUnknown(), "\\MainFrame", "FireEventInMainLoop", long(1));
		// ����� �� ����� ���������� ����� ��������
	}

	else if( !strcmp(pszEvent, szEventBeforeClose ) )
	{
		if( m_punkDocument &&
			::GetObjectKey(m_punkDocument) == ::GetObjectKey(punkFrom) )
		{
			// ������� ��� �������� - ��������� ���
			m_punkDocument = 0; // ����� �� ���� ��������, ����� �������� �� ��������
			if(m_bRecording || m_bPlaying)
				m_xMethodMan.Stop(); // ����� ��������, ����� ������ ���� �����
			DropAllCache();
		}
	}

	else if( !strcmp(pszEvent, "AfterClose"/*szEventBeforeClose*/) )
	{
		if( m_punkDocument &&
			::GetObjectKey(m_punkDocument) == ::GetObjectKey(punkFrom) )
		{
			// ������� ��� �������� - ��������� ���
			m_punkDocument = 0; // ����� �� ���� ��������, ����� �������� �� ��������
			if(m_bRecording || m_bPlaying)
				m_xMethodMan.Stop(); // ����� ��������, ����� ������ ���� �����
			DropAllCache();
		}
	}

	else if( !strcmp( pszEvent, "AppInit" ) )
	{
		// ��������� ��� �������� �� ��������
		m_xMethodMan.Reload();

		IUnknownPtr	sptrUnkMainFrame(::_GetOtherComponent(::GetAppUnknown(), IID_IMainWindow), false);
		IWindow2Ptr sptrMainFrame = sptrUnkMainFrame;
		if (sptrMainFrame != 0)
			sptrMainFrame->AttachMsgListener(GetControllingUnknown());
	}

	else if(strcmp(pszEvent, szEventBeforeRecordMethodStep) == 0)
	{
		m_bPaused = false; // �� ������ ������

		if(m_bRecording && !m_bPlaying)
		{   // �������� ���, ������� ����� ������
			char * pszName = (char*)pdata;
			AddStep(_bstr_t(pszName));
			::SetValue( ::GetAppUnknown(), "\\FormManager", "PageExecuted", 0L ); // ������� ������ "������������ �� �����"
			::SetValue( ::GetAppUnknown(), "\\MainFrame", "FireEventInMainLoop", long(1));
		}
	}

	else if(strcmp(pszEvent, szEventAfterRecordMethodStep) == 0)
	{
		if(IsInteractiveRunning())
		{
			char * pszName = (char*)pdata;
			_bstr_t bstrGroup = GetActionGroup(pszName);
			_variant_t vars[2] = { bstrGroup, _bstr_t(pszName) };
			::FireScriptEvent("OnInteractiveMethodStep", 2, vars);

			CMethodManState st(&m_xMethodMan);
			if( (!st.m_bError) && st.m_pStep && (st.m_pStep->m_dwFlags & msfWriting) )
				st.m_pStep->m_dwFlags |= msfInteractive; // ���� ��� ������� - �������, ��� �������������
		}
	}

	if(strcmp(pszEvent, szEventFormOnCancel) == 0)
	{
		if(m_bPlaying || m_bRecording) Stop();
	}

	if(strcmp(pszEvent, szEventFormOnOk) == 0)
	{
	}

	if(strcmp(pszEvent, szEventAbortLongOperation) == 0)
	{
		if(m_bPlaying || m_bRecording)
		{
			CString str_msg( _T("") );
			if(m_bRecording)
				str_msg.LoadString( (UINT)(IDS_METHOD_ESC_STOP_RECORD) );
			else
				str_msg.LoadString( (UINT)(IDS_METHOD_ESC_STOP) );

			if(IDYES == ::AfxMessageBox( str_msg, MB_YESNO | MB_ICONQUESTION ) )
				Stop(); // ���� ���� ������ "Yes" - ���������
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
static BOOL WriteDirectory(CString dd)
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

static bool _isfileexist( const char *szFileName )
{
//
	HANDLE	h = ::CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,  
		FILE_ATTRIBUTE_NORMAL, 0 );

	if( h == INVALID_HANDLE_VALUE )
	{
		DWORD	dw = ::GetLastError();
		return false;
	}

	::CloseHandle( h );
	return true;
}

CString CMethodMan::GetCacheDir()
{
	CString	strPathDef = ::MakeAppPathName( "MethodCache" )+"\\";
	CString strPath = ::GetValueString( GetAppUnknown(), "\\Paths", "MethodCache", 
						(const char*)strPathDef );
	
	if( strPath.GetLength() > 1 )
	{
		CString strDir = strPath.Left( strPath.GetLength() - 1 );
		WriteDirectory( strDir );
	}	

	return strPath;
}

CString CMethodMan::GetCachePath( long lMethodPos, long lStepPos )
{
	CString strName;
	strName.Format("%08X_CACHE",lStepPos);
	return GetCacheDir() + strName;
}

HRESULT CMethodMan::XMethodMan::StoreCache( IUnknown *punkMethod, long lStepPos )
{
	time_test ttt("MethodMan::StoreCache");
	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);
	_try(CMethodMan::XMethodMan, StoreCache)
	{
		pThis->CheckDocument(); // ��������, ��� ������� ������ ��� ��������

		long lMethodPseudoPos = (long)punkMethod;

		CString strPath = GetCachePath(lMethodPseudoPos, lStepPos);

		// ������� ������
		::DeleteFile(strPath + ".data");
		::DeleteFile(strPath + ".image");

		// ��������, ��� �� ����� ���������� ���������� �����
		{
			unsigned __int64 nfree_space = 0;
			int nmin_free_space = ::GetValueInt( ::GetAppUnknown(), "\\Methodics", "MinSpaceForCache", 10  );

			if( ::GetDiskFreeSpaceEx( GetCacheDir(), (PULARGE_INTEGER)&nfree_space, 0, 0 ) )
			{
				if( nfree_space/(1024*1024) < nmin_free_space )
				{
					CString str_msg;
					str_msg.LoadString( IDS_SPACE_WARNING );
					if( IDYES == ::AfxMessageBox(IDS_SPACE_WARNING, MB_YESNO | MB_ICONWARNING ) )
					{
						Stop();
						return E_FAIL;
					}
				}
			}
		}

        // �������� shell.data
		bool bUseShellDataCache = ::GetValueInt( ::GetAppUnknown(), "\\Methodics", "UseShellDataCache", 1  ) != 0;
		sptrIFileDataObject2 sptrF;
		if(bUseShellDataCache) sptrF = ::GetAppUnknown();
		else sptrF.CreateInstance( szNamedDataProgID );

		HRESULT hres = sptrF->SaveTextFile( _bstr_t(strPath + ".data") );
		if( S_OK != hres )
		{
			DropCache(punkMethod, lStepPos);
			Stop();
			return hres;
		}

		// �������� image
		bool bUseImageCache = ::GetValueInt( ::GetAppUnknown(), "\\Methodics", "UseImageCache", 1  ) != 0;
		if(bUseImageCache)
		{
			IApplicationPtr ptrApp( ::GetAppUnknown() );
			if( ptrApp )
			{
				IUnknownPtr ptr;
				ptrApp->GetActiveDocument(&ptr);
				IFileDataObject2Ptr ptrFDO(ptr);
				INamedDataPtr ptrND( ptr );

				// �������� ����, ������ ����������� �������� � � binary-�������
				if(ptrND) ptrND->EnableBinaryStore( true );
				long nOldEnableCompress = 
					::GetValueInt( ::GetAppUnknown(), "\\ImageCompress", "EnableCompress", 1 );
				::SetValue( ::GetAppUnknown(), "\\ImageCompress", "EnableCompress", long(0) ); // �������� ������
				long nOldWriteUncheckedFiles = GetValueInt(GetAppUnknown(), "\\General", "WriteUncheckedFiles", 1);
				::SetValue( ::GetAppUnknown(), "\\General", "WriteUncheckedFiles", long(0) ); // �������� ������

				HRESULT hres = S_OK;
				if(ptrFDO)
				{
					hres = ptrFDO->SaveFile( _bstr_t(strPath + ".image") );
				}

				::SetValue( ::GetAppUnknown(), "\\ImageCompress", "EnableCompress", nOldEnableCompress ); // ������ ������ ���������
				::SetValue( ::GetAppUnknown(), "\\General", "WriteUncheckedFiles", nOldWriteUncheckedFiles ); // ������ ������ ���������

				if(ptrFDO) ptrFDO->SetModifiedFlag( TRUE );

				if( S_OK != hres )
				{
					DropCache(punkMethod, lStepPos);
					Stop();
					return hres;
				}
			}
		}

		if(!pThis->m_nLockNotificationCounter)
		{
			tagMethodStepNotification msn = {cncChange, lStepPos};
			::FireEvent( pThis->GetControllingUnknown(), szEventChangeMethodStep, punkMethod, 0, &msn, sizeof(msn) );
		}
	}
	_catch return E_FAIL;

	return S_OK;
}

HRESULT CMethodMan::XMethodMan::LoadCache( IUnknown *punkMethod, long lStepPos, DWORD dwFlags )
{
	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);
	_try(CMethodMan::XMethodMan, LoadCache)
	{
		pThis->CheckDocument(); // ��������, ��� ������� ������ ��� ��������

		long lMethodPos = (long)punkMethod;

		// ��������� �� ������� ����; ���� �� StateLess - �� ������� lStepPos, ���� ��������� �� ����������
		CMethodStep* pStep = 0;
		while(1)
		{
			IMethodDataPtr sptrMethod(punkMethod);
			long lStepPos1=lStepPos;
			if((sptrMethod!=0) && lStepPos1) sptrMethod->GetNextStepPtr(&lStepPos1, &pStep);
			if(pStep==0) return E_FAIL; // ���� ���� ��� - �������
			if( ( pStep->m_dwFlags & msfStateless ) == 0 ) break; // ���� ���� � �� �� Stateless - Ok, ��������� ����
			// Stateless ��� - ���������� ��������� �� ����������
			sptrMethod->GetPrevStep(&lStepPos, 0);
			pStep = 0;
		}

		CString strPath = GetCachePath(lMethodPos, lStepPos);

		if(dwFlags & lcfLoadShellData)
		{
			bool bUseShellDataCache = ::GetValueInt( ::GetAppUnknown(), "\\Methodics", "UseShellDataCache", 0  ) != 0;

			if(bUseShellDataCache)
			{
				// ��������� shell.data
				if( _isfileexist(strPath + ".data") )
					pThis->LoadShellData(_bstr_t(strPath + ".data"));
			}
			else
			{
				// pStep->ApplyToShellData(); // �������� ��� ��������� � shell.data
				pThis->ApplyStepsToShellData(lStepPos);
			}
		}

		if(dwFlags & lcfRestoreView)
		{
			// ������� ������ �����
			LoadSplitter( "\\StateAfterAction", pStep->m_ptrNamedData, lssSwitchViews );
		}

		if(dwFlags & lcfLoadImage)
		{
			// ��������� image
			IApplicationPtr ptrApp( ::GetAppUnknown() );
			if( ptrApp )
			{
				if( _isfileexist(strPath + ".image") )
				{
					IUnknownPtr ptr;
					ptrApp->GetActiveDocument(&ptr);
					IFileDataObject2Ptr sptrFDO(ptr);
					if(sptrFDO)
					{
						sptrFDO->LoadFile( _bstr_t(strPath + ".image") );
						sptrFDO->SetModifiedFlag( TRUE );
						IDocumentSitePtr sptrDoc(ptr);
						if(sptrDoc)
						{
							BSTR bstr;
							sptrDoc->GetPathName( &bstr );
							pThis->m_bstrLastLoadedDoc = bstr;
							::SysFreeString(bstr);
						}
					}
				}
			}
		}

		if(dwFlags & lcfRestoreSelection)
		{
			LoadSplitter( "\\StateAfterAction", pStep->m_ptrNamedData, lssActivateObjects);
		}

		if(dwFlags & lcfRestoreZoom)
		{
			LoadSplitter( "\\StateAfterAction", pStep->m_ptrNamedData, lssRestoreZoom);
		}

	}
	_catch return E_FAIL;

	return S_OK;
}

HRESULT CMethodMan::XMethodMan::DropCache( IUnknown *punkMethod, long lStepPos )
{
	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);
	_try(CMethodMan::XMethodMan, DropCache)
	{
		CString strPath = GetCachePath( (long)punkMethod, lStepPos);

		bool bCached = _isfileexist(strPath + ".data");
		if(bCached)
		{
			::DeleteFile(strPath + ".data");
			::DeleteFile(strPath + ".image");

			if(!pThis->m_nLockNotificationCounter)
			{
				tagMethodStepNotification msn = {cncChange, lStepPos};
				::FireEvent( pThis->GetControllingUnknown(), szEventChangeMethodStep, punkMethod, 0, &msn, sizeof(msn) );
			}
		}
	}
	_catch return E_FAIL;

	return S_OK;
}

HRESULT CMethodMan::XMethodMan::IsCached( IUnknown *punkMethod, long lStepPos, BOOL *pbCached )
{
	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);
	if(pbCached==0) return E_INVALIDARG;
	_try(CMethodMan::XMethodMan, IsCached)
	{
		long lMethodPos = (long)punkMethod;

		// ��������� �� ������� ����; ���� �� StateLess - �� ������� lStepPos, ���� ��������� �� ����������
		CMethodStep* pStep = 0;
		while(1)
		{
			IMethodDataPtr sptrMethod(punkMethod);
			long lStepPos1=lStepPos;
			if((sptrMethod!=0) && lStepPos1) sptrMethod->GetNextStepPtr(&lStepPos1, &pStep);
			if(pStep==0) break; // ���� ���� ��� - �������
			if( ( pStep->m_dwFlags & msfStateless ) == 0 ) break; // ���� ���� � �� �� Stateless - Ok, ��������� ����
			// Stateless ��� - ���������� ��������� �� ����������
			sptrMethod->GetPrevStep(&lStepPos, 0);
			pStep = 0;
		}

		*pbCached = FALSE;
		CString strPath = GetCachePath(lMethodPos, lStepPos);
		*pbCached = _isfileexist(strPath + ".data");
	}
	_catch return E_FAIL;

	return S_OK;
}

// ����������� ��� ����������� ��� � ����� ������� �� ������ (��������� ��� MethodDoer::Undo())
HRESULT CMethodMan::XMethodMan::MoveCache( IUnknown *punkMethod, long lOldStepPos, long lNewStepPos, BOOL bCopy )
{
	_try(CMethodMan::XMethodMan, MoveCache)
	{
		long lMethodPos = (long)punkMethod;

		CString strOldPath = GetCachePath(lMethodPos, lOldStepPos);
		CString strNewPath = GetCachePath(lMethodPos, lNewStepPos);

		if(!bCopy)
		{
			if( !MoveFile(strOldPath + ".data", strNewPath + ".data") ) return E_FAIL;
			if( _isfileexist(strOldPath + ".image") )
			{
				if( !MoveFile(strOldPath + ".image", strNewPath + ".image") ) return E_FAIL;
			}
		}
		else
		{
			if( !CopyFile(strOldPath + ".data", strNewPath + ".data", TRUE) ) return E_FAIL;
			if( _isfileexist(strOldPath + ".image") )
			{
				if( !CopyFile(strOldPath + ".image", strNewPath + ".image", TRUE) ) return E_FAIL;
			}
		}
	}
	_catch return E_FAIL;

	return S_OK;
}

HRESULT CMethodMan::XMethodMan::LockNotification()
{	// ������������� �����������
	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);
	pThis->m_nLockNotificationCounter ++ ;
	return S_OK;
}

HRESULT CMethodMan::XMethodMan::UnlockNotification()
{	// �������������� �����������
	METHOD_PROLOGUE_EX(CMethodMan, MethodMan);
	if(pThis->m_nLockNotificationCounter)
	{
		pThis->m_nLockNotificationCounter -- ;
		return S_OK;
	}
	return E_FAIL;
}

bool CMethodMan::CheckDocument()
{	// �������� ��������� ��������� - ��� ��, ��� ������ ��������?
	// ���� ��� - Stop
	IApplicationPtr ptrApp( ::GetAppUnknown() );
	if( ptrApp )
	{
		IUnknownPtr ptrDoc;
		ptrApp->GetActiveDocument(&ptrDoc);
		if (m_punkDocument != ptrDoc)
		{
			if(m_punkDocument!=0)
			{ // �������� �������� - � �� ������������� �� ������
				IDocumentSitePtr sptrDoc(m_punkDocument); //!!! ���������! �����, ���� �������� ��� ��� ��� - ��� ��� ����������� ������ �� BeforeClose

				IUnknownPtr ptrView;
				if(sptrDoc!=0) sptrDoc->GetActiveView( &ptrView );
				IViewSitePtr sptrViewSite(ptrView);
				IUnknownPtr ptrFrame;
				if(sptrViewSite) sptrViewSite->GetFrameWindow( &ptrFrame );
				IWindowPtr sptrWnd(ptrFrame);
				HANDLE hwnd = 0;
				if(sptrWnd!=0) sptrWnd->GetHandle(&hwnd);
				HANDLE hwndParent = 0;
				if(hwnd) hwndParent = ::GetParent(HWND(hwnd));
				if(hwndParent)
					::SendMessage(HWND(hwndParent), WM_MDIACTIVATE, (WPARAM)hwnd, 0L);
				else
				{	// �� ������� ������������ ������ ��������
					if(m_bRecording || m_bPlaying)
					{
						m_xMethodMan.Stop(); // ����� ��������, ����� ������ ���� �����
					}
					DropAllCache();
				}
			}
			else m_punkDocument = ptrDoc; // ������������ ����� ��������
		}
	}

	return true;
}

//---------------------------------------------------------------------
// ������� ��� �������� - ��������� �������� ��������/��������� ����
CMethodStep* CMethodMan::GetCurrentStep()
{ // �������� ��������� �� ������� ��� ������� ��������
	CMethodManState st(&m_xMethodMan);
	if(st.m_bError) return 0;
	
	return st.m_pStep;
}

bool CMethodMan::ApplyStepsToShellData(long lStepPos0)
{	// �������� � shell.data nameddata ���� ����� � ������� 
	// �� lStepPos0 ��� �� �������, ���� lStepPos0==0
	CMethodManState st(&m_xMethodMan);

	if(st.m_bError) return false;

	if(lStepPos0==0) lStepPos0=st.m_lStepPos;

	if(lStepPos0==0) return false; // ���� �������� - ���� ��� ��������� ����, ��������� ��� ��� �� ������?

	long lStepPos=0;
	st.m_sptrMethod->GetFirstStepPos(&lStepPos);
	CMethodStep* pStep=0;

	bool bStop = false;
	while(lStepPos && !bStop)
	{
		bStop = (lStepPos==lStepPos0);
		st.m_sptrMethod->GetNextStepPtr(&lStepPos, &pStep);
		pStep->ApplyToShellData();
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMethodMan::OnListenMessage( MSG * pmsg, LRESULT *plResult )
{
	if (!pmsg)
		return FALSE;

	if( /*(pmsg->message == WM_KEYDOWN && pmsg->wParam == VK_ESCAPE) ||*/
		(pmsg->message == WM_CHAR && pmsg->wParam == VK_ESCAPE) )
	{
		if(m_bPlaying || m_bRecording)
		{
			CString str_msg( _T("") );
			if(m_bRecording)
				str_msg.LoadString( (UINT)(IDS_METHOD_ESC_STOP_RECORD) );
			else
				str_msg.LoadString( (UINT)(IDS_METHOD_ESC_STOP) );

			if(IDYES == ::AfxMessageBox( str_msg, MB_YESNO | MB_ICONQUESTION ) )
				Stop(); // ���� ���� ������ "Yes" - ���������
		}
	}

	return FALSE;
}

void CMethodMan::QueueFireScriptEvent(LPCTSTR lpszEventName)
{ // ���������� FireScriptEvent - ����� �� ���������� ��� ��������� � MainLoop
	m_NotifQueue.add_tail(lpszEventName);
	SetValue(::GetAppUnknown(), "\\MainFrame", "FireEventInMainLoop", long(1)); // ����������� � MainLoop; �� ���� ����� ���������
}

void CMethodMan::ProcessScriptEventQueue()
{
	while(m_NotifQueue.head())
	{
		CString str = m_NotifQueue.get(m_NotifQueue.head());
		m_NotifQueue.remove( m_NotifQueue.head() );
		FireScriptEvent( str );
	}
}
