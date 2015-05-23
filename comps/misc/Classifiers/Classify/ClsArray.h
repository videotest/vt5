#pragma once

template<class TYPE, int nOrigSize, int nRealloc> class CArray
{
protected:
	int m_nAlloc;
	int m_nSize;
	TYPE *m_parr;
	void Realloc(int nSize)
	{
		m_parr = (TYPE*)realloc(m_parr,sizeof(TYPE)*nSize);
		if (nSize > m_nAlloc)
			memset(m_parr+m_nAlloc, 0, (nSize-m_nAlloc)*sizeof(TYPE));
		m_nAlloc = nSize;
	}
public:
	CArray()
	{
		m_parr = NULL;
		m_nAlloc = m_nSize = 0;
	}
	~CArray()
	{
		if (m_parr)
			free(m_parr);
	}
	int GetSize() {return m_nSize;}
	TYPE GetAt(int i) {return m_parr[i];}
	void SetAt(int i, TYPE t) {m_parr[i] = t;}
	void SetSize(int nSize)
	{
		Realloc(nSize);
		m_nSize = nSize;
	}
	void Add(TYPE t)
	{
		if (m_nSize >= m_nAlloc)
		{
			if (m_nAlloc == 0)
				Realloc(nOrigSize);
			else
				Realloc(m_nAlloc+nRealloc);
		}
		m_parr[m_nSize++] = t;
	}
	void FreeExtra()
	{
		Realloc(m_nSize);
	}
	void CopyData(CArray<TYPE,nOrigSize,nRealloc> &f)
	{
		m_nSize = m_nAlloc = f.m_nSize;
		if (m_parr) free(m_parr);
		m_parr = (TYPE*)malloc(m_nSize*sizeof(TYPE));
		memcpy(m_parr,f.m_parr,m_nSize*sizeof(TYPE));
	}
};

template <class DATA> class CSimpleListItem
{
public:
	DATA m_Data;
	CSimpleListItem<DATA> *m_pNext;
};

template <class DATA> class CSimpleList;
template <class DATA> class CSimpleListIterator
{
	CSimpleList<DATA> *m_pList;
	CSimpleListItem<DATA> *m_pPos;
public:
	CSimpleListIterator(CSimpleList<DATA> *pList)
	{
		m_pList = pList;
		m_pPos = NULL;
	}
	DATA &First()
	{
		m_pPos = m_pList->m_pFirst;
		return m_pPos->m_Data;;
	}
	DATA &Next()
	{
		m_pPos = m_pPos->m_pNext;
		return m_pPos->m_Data;
	}
	DATA *FirstPtr()
	{
		m_pPos = m_pList->m_pFirst;
		return &m_pPos->m_Data;;
	}
	DATA *NextPtr()
	{
		m_pPos = m_pPos->m_pNext;
		return &m_pPos->m_Data;
	}
	bool End()
	{
		return m_pPos->m_pNext == NULL;
	}
};

template <class DATA> class CSimpleList
{
	friend class CSimpleListIterator<DATA>;
	CSimpleListItem<DATA> *m_pFirst;
	int m_nObjects;
public:
	CSimpleList()
	{
		m_pFirst = NULL;
		m_nObjects = 0;
	}
	~CSimpleList()
	{
		ClearAll();
	}
	void AddTail(DATA Data)
	{
		CSimpleListItem<DATA> *pNew = new CSimpleListItem<DATA>;
		pNew->m_Data = Data;
		pNew->m_pNext = m_pFirst;
		m_pFirst = pNew;
		m_nObjects++;
	}
	void ClearAll()
	{
		for (CSimpleListItem<DATA> *p = m_pFirst; p != NULL;)
		{
			CSimpleListItem<DATA> *p1 = p;
			p = p->m_pNext;
			delete p1;
		}
		m_nObjects = 0;
	}
	int GetItemsCount() {return m_nObjects;}
};



