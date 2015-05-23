#pragma once


template<class TYPE, int nOrigSize, int nRealloc> class _CArray
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
	_CArray()
	{
		m_parr = NULL;
		m_nAlloc = m_nSize = 0;
	}
	~_CArray()
	{
		if (m_parr)
			free(m_parr);
	}
	int GetSize() {return m_nSize;}
	TYPE GetAt(int i) {return m_parr[i];}
	TYPE &GetPtrAt(int i) {return m_parr[i];}
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
	void CopyData(_CArray<TYPE,nOrigSize,nRealloc> &f)
	{
		m_nSize = m_nAlloc = f.m_nSize;
		if (m_parr) free(m_parr);
		m_parr = (TYPE*)malloc(m_nSize*sizeof(TYPE));
		memcpy(m_parr,f.m_parr,m_nSize*sizeof(TYPE));
	}
	TYPE *GetData() {return m_parr;}
};

