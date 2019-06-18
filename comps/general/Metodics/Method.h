#pragma once
#include <comutil.h>
#include "method_int.h"
#include "\vt5\awin\misc_list.h"
#include "\vt5\awin\misc_map.h"
#include "PropBagImpl.h"
#include "MethodStep.h"

#include "MethodDoer.h"

bool TestMethod(); // тестирование объектов данных. вызвать и посмотреть, что будет

// вспомогательная функция - по имени акции или переданной технологом строке получить юзерское имя (на текущем языке)
_bstr_t GetUserActionName(char *szName);

#include "MethodDoer.h"

class CMethod :
	public CCmdTargetEx,
	public CNamedPropBagImpl
{
	DECLARE_DYNCREATE(CMethod)
	GUARD_DECLARE_OLECREATE(CMethod)
public: // !!! debug
	ENABLE_MULTYINTERFACE();
	PROVIDE_GUID_INFO( )

	virtual LPUNKNOWN GetInterfaceHook(const void* p)
	{
		if (*(IID*)p == IID_INamedPropBag || *(IID*)p == IID_INamedPropBagSer)
			return (INamedPropBagSer*)this;
		else return __super::GetInterfaceHook(p);
	}

public:
	CMethod(void);
	~CMethod(void);

	void StoreToNamedData(IUnknown* punkND);
	void LoadFromNamedData(IUnknown* punkND);

	IUnknown* m_punkNamedData;

public:
	DECLARE_INTERFACE_MAP()
	BEGIN_INTERFACE_PART(Method, IMethod)
		com_call CreateCompatible(IUnknown **ppunkObject);
		com_call StoreFile(BSTR bstrFile);
		com_call LoadFile(BSTR bstrFile);
		com_call StoreToNamedData(IUnknown* punkND);
		com_call LoadFromNamedData(IUnknown* punkND);
		com_call SetName(BSTR bstrName);
		com_call GetName(BSTR *pbstrName);
		com_call GetFullScript(BSTR *pbstrScript);
	END_INTERFACE_PART(Method);

	BEGIN_INTERFACE_PART(MethodData, IMethodData)
		com_call GetFirstStepPos( long *plPos );
		com_call GetNextStep( long *plPos, CMethodStep *pStep );
		com_call GetNextStepPtr( long *plPos, CMethodStep **ppStep );
		com_call GetPrevStep( long *plPos, CMethodStep *pStep );
		com_call GetStepCount( long *plCount );
	    com_call AddStep( CMethodStep *pStep, long lInsertBefore, long *plNewPos=0 );
		com_call SetStep( long *plPos, CMethodStep *pStep, bool bDontClearCache=false );
		com_call DeleteStep( long lPos );
		com_call SetActiveStepPos( long lPos, BOOL bLoadCache );
		com_call GetActiveStepPos( long *plPos );
		com_call GetModifiedFlag( BOOL *pbFlag );
		com_call SetModifiedFlag( BOOL bFlag );
		com_call GetStepIndexByPos( long lPos, long *plIndex );
		com_call GetStepPosByIndex( long lIndex, long *plPos );
		com_call UpdateMethod(); // обновляет shell.data методики
		com_call UpdateActiveStep(); // обновляет состояние вьюх (\\StateAfterAction) активного шага
	END_INTERFACE_PART(MethodData);

	BEGIN_INTERFACE_PART(MethodChanges, IMethodChanges)
		XMethodChanges(); // конструктор, однако
		com_call SetUndoDepth(long nDepth);
		com_call AddStep(long nStep, CMethodStep *pStep);
		com_call DeleteStep(long nStep);
		com_call MoveStep(long nStepFrom, long nStepTo, long nMoveCode);
		com_call ChangeStep(long nStep, CMethodStep *pStep, bool bDontClearCache=false );
		com_call Undo();
		com_call Redo();
		com_call ClearUndoRedo(bool bClearUndo, bool bClearRedo);
		com_call GetUndoAvailable( long *pnCount ); // возвращает, сколько шагов Undo доступно
		com_call GetRedoAvailable( long *pnCount ); // возвращает, сколько шагов Redo доступно
		com_call BeginGroupUndo(); // Начать группу Undo; допускаются вложенные группы
		com_call EndGroupUndo(); // Завершить группу Undo (при этом все шаги от последнего BeginGroupUndo попадают внутрь единого шага GroupUndo)
	protected:
		_list_t<CMethodDoer*, &CMethodDoer::_self_destroyer > m_undo;
		_list_t<CMethodDoer*, &CMethodDoer::_self_destroyer > m_redo;
		long m_nUndoDepth;
		com_call Do(CMethodDoer* pDoer); // выполнить шаг и запихать в лист
		// pDoer должен быть создан по new - потом сами удалим
	END_INTERFACE_PART(MethodChanges);

private:
	// данные для IMethod
	_bstr_t m_bstrName;

	// данные для IMethodData
	_list_t<CMethodStep> m_Steps; // список шагов методики
	_map_t<long, long, cmp_long> m_StepsPosMap; // карта Pos'ов шагов - для защиты от неправильных (чтобы проверять корректность быстро, не пробегая по списку)
	long m_lActiveStepPos;
	BOOL m_bModifiedFlag;

	BOOL m_bLockUpdateShellData; // если TRUE, то UpdateMethod() вместо реального обновления только выставит флаг m_bNeedUpdateShellData
	BOOL m_bNeedUpdateShellData; // Перед сохранением проверять - если TRUE, то апдейтить
};
