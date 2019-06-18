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

	Sync(); // получить инфу о шаге
}

void CMethodManState::Sync()
{ // Засинхронизироваться с m_sptrMethod
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
	
	m_bError = false; // готовы
}

void CMethodManState::StepForward()
{ // перейти на следующий шаг (не обновляя позицию в методике)
	m_bError = true;

	if(m_sptrMethod==0) return;

	m_sptrMethod->GetNextStepPtr(&m_lStepPos, 0); // собственно продвинем указатель

	m_sptrMethod->GetStepIndexByPos(m_lStepPos, &m_lStepIndex);

	m_lNextStepPos = m_lStepPos;
	m_sptrMethod->GetNextStepPtr(&m_lNextStepPos, &m_pStep);
	
	m_bError = false; // готовы
}

void CMethodManState::StepBack()
{ // перейти на предыдущий шаг
	m_bError = true;

	if(m_sptrMethod==0) return;

	m_sptrMethod->GetPrevStep(&m_lStepPos, 0); // собственно продвинем указатель

	m_sptrMethod->GetStepIndexByPos(m_lStepPos, &m_lStepIndex);

	m_lNextStepPos = m_lStepPos;
	m_sptrMethod->GetNextStepPtr(&m_lNextStepPos, &m_pStep);
	
	m_bError = false; // готовы
}

void CMethodManState::SeekTo(int nIndex)
{ // перейти к заданному (0..число шагов) шагу
	m_bError = true;

	if(m_sptrMethod==0) return;

	m_pStep = 0;
	m_sptrMethod->GetStepPosByIndex(nIndex, &m_lStepPos);
	m_sptrMethod->GetStepIndexByPos(m_lStepPos, &m_lStepIndex);

	m_lNextStepPos = m_lStepPos;
	m_sptrMethod->GetNextStepPtr(&m_lNextStepPos, &m_pStep);
	
	m_bError = false; // готовы
}
