#pragma once

// классы для операций над методикой, поддерживающих undo.
// использование: создать объект производного класса, настроить,
// что требуется, сделать ему Do().
// после этого откат - Undo(), для Redo можно снова вызвать Do()
// Таким образом, MethodDoer выступает в качестве посредника между
// IMethodData и юзером

#include "method_int.h"
#include "MethodStep.h"

class CMethodDoer
{
public:
	CMethodDoer(void);
	virtual ~CMethodDoer(void);
	virtual char *GetKindName() {return "Unknown";};
	virtual bool Do() =0;
	virtual bool Undo() =0;
	virtual bool AttachMethod(IUnknown *punkMethod);
	static void _self_destroyer( CMethodDoer *pItem ) { if(pItem!=0) delete pItem; }
protected:
	TPOS GetPosByIndex(long nIndex); // получить по номеру шага его pos в методике; 0 - нет такого
	long GetIndexByPos(TPOS nPos); // получить по pos'у шага его номер в методике; -1 - нет такого
	IMethodDataPtr m_sptrMethodData;
};

class CMethodDoerGroup : public CMethodDoer
{ // класс для группового Undo/Redo; включает в себя список других Doer'ов
public:
	CMethodDoerGroup(void);
	virtual ~CMethodDoerGroup(void);
	virtual char *GetKindName() {return "Group";};
	virtual bool Do();
	virtual bool Undo();

	_list_t<CMethodDoer*, &CMethodDoer::_self_destroyer > m_undo; // список вложенных шагов
	bool m_bIncomplete; // ждет закрывающего EndGroupUndo
};

class CMethodDoerAddStep : public CMethodDoer
{
public:
	CMethodDoerAddStep(void);
	virtual ~CMethodDoerAddStep(void);
	virtual char *GetKindName() {return "AddStep";};
	virtual bool Do();
	virtual bool Undo();

	long m_nIndex; // номер добавляемого степа
	CMethodStep m_Step; // собственно добавленный степ
};

class CMethodDoerDeleteStep : public CMethodDoerAddStep
{
public:
	virtual char *GetKindName() {return "DeleteStep";};
	virtual bool Do() { return CMethodDoerAddStep::Undo(); };
	virtual bool Undo() { return CMethodDoerAddStep::Do(); };
};

class CMethodDoerChangeStep : public CMethodDoer
{
public:
	CMethodDoerChangeStep(void);
	virtual char *GetKindName() {return "ChangeStep";};
	virtual bool Do();
	virtual bool Undo();

	long m_nIndex; // номер изменяемого степа
	CMethodStep m_Step; // собственно вносимые изменения (ну или весь степ)
	bool m_bDontClearCache; // чистить ли кэш при изменении этого шага
};

class CMethodDoerMoveStep : public CMethodDoer
{
public:
	CMethodDoerMoveStep(void);
	virtual char *GetKindName() {return "MoveStep";};
	virtual bool Do();
	virtual bool Undo();

	long m_nIndex0, m_nIndex1; // перемещаем шаг m_nIndex0 на позицию m_nIndex1
};
