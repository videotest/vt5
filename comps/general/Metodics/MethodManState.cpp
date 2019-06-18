#include "StdAfx.h"
#include "methodmanstate.h"

CMethodManState::CMethodManState(IMethodMan* pm) :
m_bError(true), m_lMethodPos(0), m_sptrMethod(0),
m_lStepIndex(-1), m_lStepPos(0), m_lNextStepPos(0), m_pStep(0)
{
	m_lMethodPos = 0;

	pm->GetActiveMethodPos(&m_lMethodPos);
	if(!m_lMethodPos)
		return;

	IUnknownPtr ptrMethod;
	long lMethodPos2 = m_lMethodPos;
	pm->GetNextMethod(&lMethodPos2,&ptrMethod);
	m_sptrMethod = ptrMethod;

	Sync(); // �������� ���� � ����
}

void CMethodManState::Sync()
{ // �������������������� � m_sptrMethod
	m_bError = true;
	m_lStepIndex = -1;
	m_lStepPos = 0;
	m_lNextStepPos = 0;
	m_pStep = 0;

	if(m_sptrMethod==0) return;

	m_sptrMethod->GetActiveStepPos(&m_lStepPos);
	m_sptrMethod->GetStepIndexByPos(m_lStepPos, &m_lStepIndex);

	m_lNextStepPos = m_lStepPos;
	m_sptrMethod->GetNextStepPtr(&m_lNextStepPos, &m_pStep);
	
	m_bError = false; // ������
}

void CMethodManState::StepForward()
{ // ������� �� ��������� ��� (�� �������� ������� � ��������)
	m_bError = true;

	if(m_sptrMethod==0) return;

	m_sptrMethod->GetNextStepPtr(&m_lStepPos, 0); // ���������� ��������� ���������

	m_sptrMethod->GetStepIndexByPos(m_lStepPos, &m_lStepIndex);

	m_lNextStepPos = m_lStepPos;
	m_sptrMethod->GetNextStepPtr(&m_lNextStepPos, &m_pStep);
	
	m_bError = false; // ������
}

void CMethodManState::StepBack()
{ // ������� �� ���������� ���
	m_bError = true;

	if(m_sptrMethod==0) return;

	m_sptrMethod->GetPrevStep(&m_lStepPos, 0); // ���������� ��������� ���������

	m_sptrMethod->GetStepIndexByPos(m_lStepPos, &m_lStepIndex);

	m_lNextStepPos = m_lStepPos;
	m_sptrMethod->GetNextStepPtr(&m_lNextStepPos, &m_pStep);
	
	m_bError = false; // ������
}

void CMethodManState::SeekTo(int nIndex)
{ // ������� � ��������� (0..����� �����) ����
	m_bError = true;

	if(m_sptrMethod==0) return;

	m_pStep = 0;
	m_sptrMethod->GetStepPosByIndex(nIndex, &m_lStepPos);
	m_sptrMethod->GetStepIndexByPos(m_lStepPos, &m_lStepIndex);

	m_lNextStepPos = m_lStepPos;
	m_sptrMethod->GetNextStepPtr(&m_lNextStepPos, &m_pStep);
	
	m_bError = false; // ������
}
