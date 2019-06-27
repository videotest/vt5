#pragma once
#include <comutil.h>
#include "method_int.h"
#include "\vt5\awin\misc_list.h"
#include "\vt5\awin\misc_ptr.h"
#include "MethodStep.h"
#include "MethodDataUtil.h"

enum MethodExecutionState
{
	mesNothing, // ������ �� ������
	mesExecute, // ��������� ����, ���� ������
	mesNext, // ������� � ���������� ����
	mesWait, // �������� ���������� (���� ������������ �� �������� ������������� ����� / �� ������� �����)
	mesRewindDead, // ������ �������� ����� �� ������ (��� �� ������)
	mesRewindLoop, // ������ �������� ����� �� ������ �����
};

class CMethodMan :
	public CCmdTargetEx,
	public CNamedObjectImpl,
	public CEventListenerImpl,
	public CMsgListenerImpl
{
	DECLARE_DYNCREATE(CMethodMan)
	GUARD_DECLARE_OLECREATE(CMethodMan)
	ENABLE_MULTYINTERFACE();
public:
	CMethodMan(void);
	~CMethodMan(void);
	
	virtual BOOL OnCreateAggregates(void);
	virtual void OnFinalRelease(void);

	DECLARE_INTERFACE_MAP()
	BEGIN_INTERFACE_PART(MethodMan, IMethodMan2)
		com_call Run();
		com_call RunFromActive();
		com_call Stop(BOOL bStopRun=true, BOOL bStopRecord=true);
		com_call Record();
		com_call IsRunning(BOOL *pbVal);
		com_call IsRecording(BOOL *pbVal);
		com_call AddMethod(IUnknown* punkMethod, TPOS lInsertBefore, TPOS *plNewPos = 0);
		com_call DeleteMethod(TPOS lPos);
		com_call SetActiveMethodPos(TPOS lPos);
		com_call GetActiveMethodPos(TPOS *plPos);
		com_call GetFirstMethodPos(TPOS *plPos);
		com_call GetNextMethod(TPOS *plPos, IUnknown **pMethod);
		com_call Reload( );

		com_call StoreCache(IUnknown *punkMethod, TPOS lStepPos);
		com_call LoadCache(IUnknown *punkMethod, TPOS lStepPos, DWORD dwFlags = lcfLoadAll);
		com_call DropCache(IUnknown *punkMethod, TPOS lStepPos);
		com_call IsCached(IUnknown *punkMethod, TPOS lStepPos, BOOL *pbCached);
		com_call MoveCache(IUnknown *punkMethod, TPOS lOldStepPos, TPOS lNewStepPos, BOOL bCopy);

		com_call LockNotification(); // ������������� �����������
		com_call UnlockNotification(); // �������������� �����������
		// ���� ������� ����������, ���� ������������� 2 ���� - ����������� �� �����, ���� �� ������������ 2 ����

	END_INTERFACE_PART(MethodMan);

	static CString GetCacheDir();
	static CString GetCachePath(TPOS lMethodPos, TPOS lStepPos);

protected:
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	// override from msg listner
	virtual BOOL OnListenMessage( MSG * pmsg, LRESULT *plResult );

	DECLARE_DISPATCH_MAP();
	//class wizard support
	//{{AFX_DISPATCH(CMethodMan)
	afx_msg void Run();
	afx_msg void Stop();
	afx_msg void Pause();
	afx_msg void Resume();
	afx_msg void Record();
	afx_msg bool IsRunning();
	afx_msg bool IsRecording();
	afx_msg BSTR GetCurrentStartingActionName();
	afx_msg BSTR GetActionNameByIndex(long nIndex);
	afx_msg void StoreStepAs(LPCTSTR strName);
	afx_msg void CancelStepStoring();
	afx_msg void DeleteCurrentMethod();
	afx_msg void RenameCurrentMethod(LPCTSTR strName);
	afx_msg void ClearMethodCache();
	afx_msg long GetCurrentStepFlags();
	afx_msg void SetCurrentStepFlags(long nFlags);
	afx_msg void RenameCurrentStep(LPCTSTR strName);
	afx_msg BSTR GetCurrentMethodName();
	afx_msg void RememberSection(LPCTSTR strName);
	afx_msg void SaveFilteredShellData(LPCTSTR strName);
	afx_msg void ImportShellData(LPCTSTR strName);
	afx_msg LPDISPATCH GetActiveStepNamedData();
	afx_msg LPDISPATCH GetStepNamedData(long nIndex);
	afx_msg void StartSetValueLogging();
	afx_msg void FinishSetValueLogging();
	afx_msg void StartSetValueLogging2(LPCTSTR strName);
	afx_msg void FinishSetValueLogging2(LPCTSTR strName);
	afx_msg void DispStoreSplitter(LPCTSTR strSectionName);
	afx_msg void DispLoadSplitter(LPCTSTR strSectionName);
	//}}AFX_DISPATCH

	IUnknown*			m_punkNotifyCtrl;

	IUnknown* m_punkDocument; // ��������, � ������� �������� ��������
	// ��� AddRef, ������������ ������ ��� ��������� � ActiveDocument

	// ������ ��� IMethodMan
	_list_t<IUnknownPtr> m_Methods;
	_map_t<TPOS, TPOS, cmp<TPOS> > m_MethodPosMap; // ����� Pos'�� ������� - ��� ������ �� ������������ (����� ��������� ������������ ������, �� �������� �� ������)
	TPOS m_lActiveMethodPos;

	_ptr_t<char> m_pDisabledKeys; // ����� ��� ����� � �������/��������, ������� ������ ������� � ��������

	// ���������� ������� ������, ���������� ������/��������������� �������
	MethodExecutionState m_ExecutionState;

	bool m_bPlaying;
	bool m_bRecording;
	bool m_bPaused;

	bool m_bFormShown; // ������ � ���� - �� �������� �� ������ �����
	// ������ ������ �� ������ ����� ��� ���������� ���������� � MainFrame

	bool m_bNoLoop;
	long m_nIndexBreakpoint;

	_bstr_t m_bstrLastLoadedDoc;

	long m_nLockNotificationCounter;

	// ������, ���������� � ������� "�������" � ��������
	CMethodStep* CurrentStep(); // �������� ��������� �� ������� ���
	void StepForward(bool bLoadCache=true); // ����������� ��������� �� ��� ������
	void StepBack(bool bLoadCache=true); // ����������� ��������� �� ��� �����
	bool IsAlive(); // ������� ��� - �����?
	void MakeAlive(); // ������� ������� ��� �����
	void MakeDead(); // ������� ������� ��� �������
	bool IsLoopBegin(); // ������� ��� - ������ �����?
	bool IsLoopEnd(); // ������� ��� - ����� �����?
	bool IsBreakpoint(); // ������� ��� - breakpoint?
	void AddStep(_bstr_t bstrActionName); // �������� ��� � ������� �������
	void FinishAddStep(_bstr_t bstrForcedName); // ��������� ������ ����

	bool NeedWait(); // ��������, �� ���� �� �� ���������� ������������� ������-�� �������� (�����. �����, �����)

	void RunStep(); // ��������� ���� ��� (��������, ������� ����� �� EndLoop)

	bool NeedRepeatMethodLoop();
	bool StartMethodDisabled();
	bool ExecuteStep(CMethodStep &step);
	bool PostExecuteStep(CMethodStep &step); // ������ ���������� �� ExecuteStep, �� � ������ ������������� ����� - ����� � �������� (��� ������������ �������� �� ����� ������ �����)
	bool DropAllCache();

	bool LoadDisabledKeys(); // ��������� ������ ����������� � �������� ������ � m_pDisabledKeys
	bool LoadShellData(BSTR bstrFileName); // ��������� shell.data �� ��������� ����� - ����� m_pDisabledKeys

    bool CheckDocument(); // �������� ��������� ��������� - ��� ��, ��� ������ ��������?
	// ���� ��� - Stop

		bool ApplyStepsToShellData(TPOS lStepPos0 = 0);
	// �������� � shell.data nameddata ���� ����� � ������� 
	// �� lStepPos0 ��� �� �������, ���� lStepPos0==0

	// ��������������� �������
	CMethodStep* GetCurrentStep();

	_list_t<CString> m_NotifQueue;
	void QueueFireScriptEvent(LPCTSTR lpszEventName);
	// ���������� FireScriptEvent - ����� �� ���������� ��� ��������� � MainLoop
	void ProcessScriptEventQueue();
	// ���������� ���������� ���� ������� �� m_NotifQueue
	// ������� ������ �� MainLoop!!! ����� ��������� ���-������ ����������.

	CString m_strSetValueLoggingGroup;
};
