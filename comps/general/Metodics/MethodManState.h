#pragma once
#include "method_int.h"

// Специальный класс, который может хранить в себе информацию о текущей методике
// и текущем либо произвольном шаге (чтобы не дублировать всюду куски кода для
// доступа к ним через интерфейсы).
// При создании объекта - сразу затягивает в себя всю информацию о текущем шаге.
// Может также использоваться для прохода по методике - см. методы StepForward() и StepBack()
// (они не меняют состояние методики, только читают другой шаг).
// Если методику меняли извне - надо выполнить Sync();
class CMethodManState
{
public:
	CMethodManState(IMethodMan* pm); // Инитимся из переданного методмана и активного метода в нем
	void Sync(); // Засинхронизироваться с m_sptrMethod
	void StepForward(); // перейти на следующий шаг (не обновляя позицию в методике)
	void StepBack(); // перейти на предыдущий шаг
	void SeekTo(int nIndex); // перейти к заданному (0..число шагов) шагу
	long m_lMethodPos;
	IMethodDataPtr m_sptrMethod;
	long m_lStepIndex;
	long m_lStepPos;
	long m_lNextStepPos;
	CMethodStep* m_pStep;
	bool m_bError;
};
