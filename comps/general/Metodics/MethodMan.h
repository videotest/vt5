#pragma once
#include <comutil.h>
#include "method_int.h"
#include "\vt5\awin\misc_list.h"
#include "\vt5\awin\misc_ptr.h"
#include "MethodStep.h"
#include "MethodDataUtil.h"

enum MethodExecutionState
{
	mesNothing, // ничего не делать
	mesExecute, // выполняем шаги, если готовы
	mesNext, // переход к следующему шагу
	mesWait, // ожидание готовности (пока пользователь не завершит интерактивную акцию / не закроет форму)
	mesRewindDead, // мотаем методику назад до живого (или до начала)
	mesRewindLoop, // мотаем методику назад до начала цикла
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

		com_call LockNotification(); // заблокировать нотификации
		com_call UnlockNotification(); // разблокировать нотификации
		// есть счетчик блокировок, если заблокировать 2 раза - нотификаций не будет, пока не разблокируем 2 раза

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

	IUnknown* m_punkDocument; // документ, с которым работает методика
	// без AddRef, используется только для сравнения с ActiveDocument

	// данные для IMethodMan
	_list_t<IUnknownPtr> m_Methods;
	_map_t<TPOS, TPOS, cmp<TPOS> > m_MethodPosMap; // карта Pos'ов методов - для защиты от неправильных (чтобы проверять корректность быстро, не пробегая по списку)
	TPOS m_lActiveMethodPos;

	_ptr_t<char> m_pDisabledKeys; // буфер для файла с ключами/секциями, которые нельзя вносить в методику

	// собственно рабочие данные, касающиеся записи/воспроизведения методик
	MethodExecutionState m_ExecutionState;

	bool m_bPlaying;
	bool m_bRecording;
	bool m_bPaused;

	bool m_bFormShown; // Храним у себя - не показана ли сейчас форма
	// Вообще хорошо бы вместо этого для надежности спрашивать у MainFrame

	bool m_bNoLoop;
	long m_nIndexBreakpoint;

	_bstr_t m_bstrLastLoadedDoc;

	long m_nLockNotificationCounter;

	// Методы, работающие с позиции "курсора" в методике
	CMethodStep* CurrentStep(); // получить указатель на текущий шаг
	void StepForward(bool bLoadCache=true); // передвинуть указатель на шаг вперед
	void StepBack(bool bLoadCache=true); // передвинуть указатель на шаг назад
	bool IsAlive(); // текущий шаг - живой?
	void MakeAlive(); // сделать текущий шаг живым
	void MakeDead(); // сделать текущий шаг мертвым
	bool IsLoopBegin(); // текущий шаг - начало цикла?
	bool IsLoopEnd(); // текущий шаг - конец цикла?
	bool IsBreakpoint(); // текущий шаг - breakpoint?
	void AddStep(_bstr_t bstrActionName); // Добавить шаг в позиции курсора
	void FinishAddStep(_bstr_t bstrForcedName); // Завершить запись шага

	bool NeedWait(); // выяснить, не ждем ли мы завершения пользователем какого-то действия (интер. акция, форма)

	void RunStep(); // выполнить один шаг (возможно, переход назад по EndLoop)

	bool NeedRepeatMethodLoop();
	bool StartMethodDisabled();
	bool ExecuteStep(CMethodStep &step);
	bool PostExecuteStep(CMethodStep &step); // обычно вызывается из ExecuteStep, но в случае интерактивной акции - может и отдельно (при приостановке методики на время работы акции)
	bool DropAllCache();

	bool LoadDisabledKeys(); // прочитать список запрещенных к экспорту ключей в m_pDisabledKeys
	bool LoadShellData(BSTR bstrFileName); // прочитать shell.data из заданного файла - кроме m_pDisabledKeys

    bool CheckDocument(); // проверка активного документа - тот ли, что помнит методика?
	// если нет - Stop

		bool ApplyStepsToShellData(TPOS lStepPos0 = 0);
	// закинуть в shell.data nameddata всех шагов с первого 
	// по lStepPos0 или по текущий, если lStepPos0==0

	// вспомогательные функции
	CMethodStep* GetCurrentStep();

	_list_t<CString> m_NotifQueue;
	void QueueFireScriptEvent(LPCTSTR lpszEventName);
	// Отложенный FireScriptEvent - чтобы он выполнился при попадании в MainLoop
	void ProcessScriptEventQueue();
	// Собственно обработать одно событие из m_NotifQueue
	// Вызывть только из MainLoop!!! Иначе технологи что-нибудь напакостят.

	CString m_strSetValueLoggingGroup;
};
