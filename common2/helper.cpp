#include "stdafx.h"
#include "helper.h"


StringArray::StringArray(long nInitSize, long nGrowBy)
{
	m_nDynamicSize = nInitSize;

	if(nGrowBy < 1)
		nGrowBy = 1;

	m_nGrowBy = nGrowBy;

	m_nSize = 0;
	m_pszArray = 0;

	ReAllocArray();
}

StringArray::~StringArray()
{
	if(m_pszArray)
	{
		delete [] m_pszArray;
		m_pszArray = 0;
	}
}

void StringArray::ReAllocArray()
{
	char (*pszOldArray)[MAX_STRING];
	pszOldArray = m_pszArray;

	m_pszArray = new char[m_nDynamicSize+m_nGrowBy][MAX_STRING];

	for(long i = 0; i < m_nSize; i++)
	{
		strcpy(m_pszArray[i], pszOldArray[i]);
	}

	delete [] pszOldArray;

	m_nDynamicSize = m_nDynamicSize+m_nGrowBy;
}

void StringArray::Add(const char* szStr)
{
	if(m_nSize == m_nDynamicSize)
		ReAllocArray();

	strcpy(m_pszArray[m_nSize], szStr);

	m_nSize++;
}

char* StringArray::GetAt(long nIndex)
{
	if(nIndex >= 0 && nIndex < m_nSize)
		return (char*)m_pszArray[nIndex];
	return 0;
}


PtrList::PtrList()
{
	m_pHead = 0;
	m_pTail = 0;
	m_nCounter = 0;
}

PtrList::~PtrList()
{
	KillThemAll();
}

POS PtrList::GetHeadPos()
{
	return (POS)m_pHead;
}

void* PtrList::GetNextPtr(POS& pos)
{
	void* pRet = 0;
	Node* pCur = m_pHead;
	while(pCur)
	{
		if(pCur == (Node*)pos)
		{
			pRet = pCur->pPtr;
			pCur = pCur->pNext;
			pos = (POS)pCur;
			return pRet;
		}
		pCur = pCur->pNext;
	}

	pos = 0;
	return pRet;
}

void PtrList::AddToTail(void* pPtr)
{
	Node* pCur = new Node;
	pCur->pPtr = pPtr;
	pCur->pNext = 0;

	if(m_pTail)
		m_pTail->pNext = pCur;
	m_pTail = pCur;

	if(!m_pHead)
		m_pHead = pCur;

	m_nCounter++;
}

void* PtrList::GetByPtrIdx(long nIdx)
{
	if(nIdx < 0 || nIdx >= m_nCounter)
		return 0;

	Node* pCur = m_pHead;
	long i = 0;
	while(pCur)
	{
		if(i == nIdx)
		{
			return pCur->pPtr;
		}
		i++;
		pCur = pCur->pNext;
	}

	return 0;
}
	
void PtrList::KillThemAll()
{
	Node* pCur = m_pHead;
	while(pCur)
	{
		m_pHead = pCur->pNext;
		delete pCur;
		pCur = m_pHead;
	}

	m_pTail = 0;
	m_nCounter = 0;
}