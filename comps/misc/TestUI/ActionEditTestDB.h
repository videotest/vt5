#pragma once
#include "MacroRecorder_int.h"
#include "test_man.h"

class CHourglass
{
	 HCURSOR         m_hCur;
public:
	CHourglass() { m_hCur = SetCursor(LoadCursor(NULL, IDC_WAIT)); }
	virtual ~CHourglass(){ SetCursor(m_hCur); }

};

BOOL	CheckTestBranch( ITestManager *ptest_man, LPOS lpos_test, LPOS lpos_start_parent = 0);
BOOL	HasSavedChild( ITestManager *ptest_man, LPOS lpos_parent );

// lexecution_parts -> настройка "\\TestMan\\ExecutionParts" в shell.data
BOOL	ExecuteBranchTests( ITestManager *ptest_man, LPOS lpos_test, long lexecution_parts );

enum TestOperations
{
	toCheck = 1 << 0,
	toUnCheck = 1 << 1,
	toStoreContent = 1 << 2,
	toLoadContent = 1 << 3,
	toResetSavedState = 1 << 4,
	toResetRunTime = 1 << 5,
	toResetTestRunTime = 1 << 6,
	toResetSaveTime = 1 << 7,
	toResetLastError = 1 << 8,
	toUpdatePath = 1 << 9,
	toParseCond = 1 << 10,
};

BOOL	DoOperationWithChildTests( ITestManager *ptest_man, DWORD dwoperations,LPOS lpos_parent );


class CActionEditTestDB : public CActionBase
{
	DECLARE_DYNCREATE(CActionEditTestDB)
	GUARD_DECLARE_OLECREATE(CActionEditTestDB)
public:
	CActionEditTestDB(void);
	~CActionEditTestDB(void);
public:
	virtual bool Invoke();

protected:
	LPOS m_lworking_test;

	enum XConditionType
	{
		xctExec = 1 << 0,
		xctStart = 1 << 1,
	};

    BOOL _record_test_sequence( ITestManager *ptest_man, BOOL bfinal_sequence, IMacroManager *pmacro_man,
		HWND hwnd_main, LPOS lpos_parent, LPOS lpos_test );
    BOOL _edit_test_sequence( ITestManager *ptest_man, BOOL bfinal_sequence, IMacroManager *pmacro_man,
		IMacroHelper *pMacroHelper, HWND hwnd_main, LPOS lpos_parent, LPOS lpos_test );
		BOOL _input_condition( ITestManager *ptest_man, HWND hwnd_main, LPOS lpos_parent,
		LPOS lpos_test, XConditionType cond_type );
};
