#pragma once
#include "method_int.h"

// ����������� �����, ������� ����� ������� � ���� ���������� � ������� ��������
// � ������� ���� ������������ ���� (����� �� ����������� ����� ����� ���� ���
// ������� � ��� ����� ����������).
// ��� �������� ������� - ����� ���������� � ���� ��� ���������� � ������� ����.
// ����� ����� �������������� ��� ������� �� �������� - ��. ������ StepForward() � StepBack()
// (��� �� ������ ��������� ��������, ������ ������ ������ ���).
// ���� �������� ������ ����� - ���� ��������� Sync();
class CMethodManState
{
public:
	CMethodManState(IMethodMan* pm); // �������� �� ����������� ��������� � ��������� ������ � ���
	void Sync(); // �������������������� � m_sptrMethod
	void StepForward(); // ������� �� ��������� ��� (�� �������� ������� � ��������)
	void StepBack(); // ������� �� ���������� ���
	void SeekTo(int nIndex); // ������� � ��������� (0..����� �����) ����
	long m_lMethodPos;
	IMethodDataPtr m_sptrMethod;
	long m_lStepIndex;
	long m_lStepPos;
	long m_lNextStepPos;
	CMethodStep* m_pStep;
	bool m_bError;
};
