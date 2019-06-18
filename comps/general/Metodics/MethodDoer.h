#pragma once

// ������ ��� �������� ��� ���������, �������������� undo.
// �������������: ������� ������ ������������ ������, ���������,
// ��� ���������, ������� ��� Do().
// ����� ����� ����� - Undo(), ��� Redo ����� ����� ������� Do()
// ����� �������, MethodDoer ��������� � �������� ���������� �����
// IMethodData � ������

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
	long GetPosByIndex(long nIndex); // �������� �� ������ ���� ��� pos � ��������; 0 - ��� ������
	long GetIndexByPos(long nPos); // �������� �� pos'� ���� ��� ����� � ��������; -1 - ��� ������
	IMethodDataPtr m_sptrMethodData;
};

class CMethodDoerGroup : public CMethodDoer
{ // ����� ��� ���������� Undo/Redo; �������� � ���� ������ ������ Doer'��
public:
	CMethodDoerGroup(void);
	virtual ~CMethodDoerGroup(void);
	virtual char *GetKindName() {return "Group";};
	virtual bool Do();
	virtual bool Undo();

	_list_t<CMethodDoer*, &CMethodDoer::_self_destroyer > m_undo; // ������ ��������� �����
	bool m_bIncomplete; // ���� ������������ EndGroupUndo
};

class CMethodDoerAddStep : public CMethodDoer
{
public:
	CMethodDoerAddStep(void);
	virtual ~CMethodDoerAddStep(void);
	virtual char *GetKindName() {return "AddStep";};
	virtual bool Do();
	virtual bool Undo();

	long m_nIndex; // ����� ������������ �����
	CMethodStep m_Step; // ���������� ����������� ����
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

	long m_nIndex; // ����� ����������� �����
	CMethodStep m_Step; // ���������� �������� ��������� (�� ��� ���� ����)
	bool m_bDontClearCache; // ������� �� ��� ��� ��������� ����� ����
};

class CMethodDoerMoveStep : public CMethodDoer
{
public:
	CMethodDoerMoveStep(void);
	virtual char *GetKindName() {return "MoveStep";};
	virtual bool Do();
	virtual bool Undo();

	long m_nIndex0, m_nIndex1; // ���������� ��� m_nIndex0 �� ������� m_nIndex1
};
