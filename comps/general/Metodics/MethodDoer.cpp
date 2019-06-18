#include "StdAfx.h"
#include "Method.h"
//#include "methoddoer.h"

CMethodDoer::CMethodDoer(void)
{
	m_sptrMethodData = 0;
}

CMethodDoer::~CMethodDoer(void)
{
}

bool CMethodDoer::AttachMethod(IUnknown *punkMethod)
{
	m_sptrMethodData = punkMethod;
	return true;
}

long CMethodDoer::GetPosByIndex(long nIndex)
{ // получить по номеру шага его pos в методике
	if(m_sptrMethodData==0) return 0;
	long lStepPos = 0;
	if( S_OK != m_sptrMethodData->GetStepPosByIndex( nIndex , &lStepPos) )
		return 0;

	return lStepPos;
}

long CMethodDoer::GetIndexByPos(long nPos)
{ // получить по pos'у шага его номер в методике
	if(m_sptrMethodData==0) return -1;
	if(nPos==0) return -1;

	long lIndex = 0;
	if( S_OK != m_sptrMethodData->GetStepIndexByPos( nPos, &lIndex ) )	
		return -1;

	return lIndex;
}

// -----------------------
CMethodDoerGroup::CMethodDoerGroup(void)
{
	m_bIncomplete = false;
}

CMethodDoerGroup::~CMethodDoerGroup(void)
{
}

bool CMethodDoerGroup::Do()
{
	if(m_sptrMethodData==0) return false;

	long lpos=m_undo.tail();
	while(lpos)
	{
		CMethodDoer *pDoer = m_undo.get(lpos);
		if(!pDoer->Do()) return false;
		lpos = m_undo.prev(lpos);
	}

	return true;
}

bool CMethodDoerGroup::Undo()
{
	if(m_sptrMethodData==0) return false;

	long lpos=m_undo.head();
	while(lpos)
	{
		CMethodDoer *pDoer = m_undo.get(lpos);
		if(!pDoer->Undo()) return false;
		lpos = m_undo.next(lpos);
	}

	return true;
}

// -----------------------
CMethodDoerAddStep::CMethodDoerAddStep(void)
{
	m_nIndex = -1;
}

CMethodDoerAddStep::~CMethodDoerAddStep(void)
{
	// если у нас был сохранен кэш - сбросить
	IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false); 
	IMethodManPtr sptrM(ptr_mtd_man);
	if(sptrM)
	{
		long lThisPos = (long)this;
		sptrM->DropCache(m_sptrMethodData, lThisPos);
	}	

}

bool CMethodDoerAddStep::Do()
{
	if(m_sptrMethodData==0) return false;
	if(m_nIndex<0) return false; // на всякий случай - вдруг нас не проинитили
	long lPos = GetPosByIndex(m_nIndex);
	if(lPos==0)
	{ // 0 - либо надо добавлять в самый хвост, либо ошибка
		long nCount=0;
        m_sptrMethodData->GetStepCount(&nCount);
		if(m_nIndex!=nCount) return false; // ошибка
	}

	long lNewPos=0;
	m_sptrMethodData->AddStep(&m_Step,lPos,&lNewPos);

	// если у нас был сохранен кэш - восстановить его
	IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false); 
	IMethodManPtr sptrM(ptr_mtd_man);
	if(sptrM)
	{
		long lThisPos = (long)this;
		sptrM->MoveCache(m_sptrMethodData, lThisPos, lNewPos);
		// перекинули из нашего кэша в кэш метода
	}	

	m_sptrMethodData->SetActiveStepPos(lNewPos);

	return true;
}

bool CMethodDoerAddStep::Undo()
{
	if(m_sptrMethodData==0) return false;
	if(m_nIndex<0) return false; // на всякий случай - вдруг нас не проинитили

	long lPos = GetPosByIndex(m_nIndex);
	if(lPos==0) return false; // на всякий случай

	// перед удалением степа - заберем себе его кэш
	IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false); 
	IMethodManPtr sptrM(ptr_mtd_man);
	if(sptrM)
	{
		long lThisPos = (long)this;
		sptrM->MoveCache(m_sptrMethodData, lPos, lThisPos);
		// перекинули из кэша метода в наш кэша
	}

	long lPos1 = lPos;
	m_sptrMethodData->GetPrevStep(&lPos1, &m_Step);
	// забрали Step - для Redo; lPos1 - предыдущий шаг, на него переставим активность

	m_sptrMethodData->DeleteStep(lPos); // ну и удалили нафиг

	while(1)
	{	// если должно заактивизиться начало или конец цикла - идем вверх
		if(!lPos1) break; // уперлись в начало методики
		long lPos2=lPos1;
		CMethodStep step2;
		m_sptrMethodData->GetPrevStep(&lPos2,&step2);
		if(	step2.m_bstrActionName != _bstr_t(szBeginMethodLoop) &&
			step2.m_bstrActionName != _bstr_t(szEndMethodLoop) )
			break; // нашли нормальный шаг
		lPos1 = lPos2;
	}
	if(0==lPos1) m_sptrMethodData->GetFirstStepPos(&lPos1); // если удаляли первый шаг - то заактивизим новый первый

	while(1)
	{	// если должно заактивизиться начало или конец цикла - идем вниз
		if(!lPos1) break; // уперлись в конец методики
		long lPos2=lPos1;
		CMethodStep step2;
		m_sptrMethodData->GetNextStep(&lPos2,&step2);
		if(	step2.m_bstrActionName != _bstr_t(szBeginMethodLoop) &&
			step2.m_bstrActionName != _bstr_t(szEndMethodLoop) )
			break; // нашли нормальный шаг
		lPos1 = lPos2;
	}

	if(0==lPos1) m_sptrMethodData->GetFirstStepPos(&lPos1); // если удаляли первый шаг - то заактивизим новый первый

	m_sptrMethodData->SetActiveStepPos(lPos1); // и поставили указатель на предыдущий

	return true;
}

CMethodDoerChangeStep::CMethodDoerChangeStep(void)
{
	m_nIndex = -1;
	m_bDontClearCache = false;
}

bool CMethodDoerChangeStep::Do()
{
	if(m_sptrMethodData==0) return false;
	if(m_nIndex<0) return false; // на всякий случай - вдруг нас не проинитили

	long lPos = GetPosByIndex(m_nIndex);
	if(lPos==0) return false; // на всякий случай

	CMethodStep step1;
	step1.m_bSkipData = m_Step.m_bSkipData;

	long lPos1 = lPos;
	m_sptrMethodData->GetNextStep(&lPos1, &step1); // забрали Step - для Redo
	lPos1 = lPos;
	m_sptrMethodData->SetStep(&lPos1, &m_Step, m_bDontClearCache ); // установили step
	m_Step = step1; // и запомнили то, что было до изменения
	// по ходу, do и undo одинаковы

	return true;
}

bool CMethodDoerChangeStep::Undo()
{
	return Do();
}

CMethodDoerMoveStep::CMethodDoerMoveStep(void)
{
	m_nIndex0 = -1;
	m_nIndex1 = -1;
}

bool CMethodDoerMoveStep::Do()
{
	if(m_sptrMethodData==0) return false;
	if(m_nIndex0<0) return false; // на всякий случай - вдруг нас не проинитили
	if(m_nIndex1<0) return false; // на всякий случай - вдруг нас не проинитили

	if(m_nIndex1==m_nIndex0) return true; // просто защита от дурака...

	long lPos0 = GetPosByIndex(m_nIndex0);
	if(lPos0==0) return false; // на всякий случай

	CMethodStep step;
	long lPos0tmp = lPos0;
	m_sptrMethodData->GetNextStep(&lPos0tmp, &step); // забрали Step
	m_sptrMethodData->DeleteStep(lPos0); // ну и удалили нафиг

	// [vanek]: а теперь берем позицию до которой надо воткнуть
	long lPos1 = GetPosByIndex(m_nIndex1);
	
	long lNewPos=0;
	m_sptrMethodData->AddStep(&step,lPos1, &lNewPos);
	m_sptrMethodData->SetActiveStepPos(lNewPos, FALSE);

	return true;
}

bool CMethodDoerMoveStep::Undo()
{
	{ long nTmp=m_nIndex0; m_nIndex0=m_nIndex1; m_nIndex1=nTmp; } // поменяли местами
	bool bRet = Do();
	{ long nTmp=m_nIndex0; m_nIndex0=m_nIndex1; m_nIndex1=nTmp; } // поменяли местами
	return bRet;
}