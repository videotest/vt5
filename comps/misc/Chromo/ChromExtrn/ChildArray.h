#if !defined(__ChildArray_H__)
#define __ChildArray_H__


class CChildrenArray : public sptrINamedDataObject2
{
	POSITION *m_pPositions;
	long lObjects;
public:
	CChildrenArray(IUnknown *punk, bool bCache = false) : sptrINamedDataObject2(punk)
	{
		lObjects = 0;
		m_pPositions = NULL;
		if (bCache)
			Cache();
	}
	~CChildrenArray()
	{
		if (m_pPositions)
			delete m_pPositions;
	}
	IUnknownPtr GetAt(int iNum)
	{
		if (m_pPositions)
		{
			POSITION pos = m_pPositions[iNum];
			IUnknownPtr sptr;
			GetInterfacePtr()->GetNextChild(&pos, &sptr);
			return sptr;
		}
		else
		{
			POSITION posCur = 0;
			int iCur = 0;
			GetInterfacePtr()->GetFirstChildPosition(&posCur);
			while (posCur)
			{
				IUnknownPtr sptr;
				GetInterfacePtr()->GetNextChild(&posCur, &sptr);
				if (iNum == iCur++)
					return sptr;
			}
			return 0;
		}
	}
	long ChildrenCount()
	{
		if (m_pPositions)
			return lObjects;
		else
		{
			long l;
			GetInterfacePtr()->GetChildsCount(&l);
			return l;
		}
	}
	void Cache()
	{
		if (m_pPositions)
			return;
		GetInterfacePtr()->GetChildsCount(&lObjects);
		m_pPositions = new POSITION[lObjects];
		POSITION posCur = 0;
		int iCur = 0;
		GetInterfacePtr()->GetFirstChildPosition(&posCur);
		while (posCur)
		{
			m_pPositions[iCur++] = posCur;
			IUnknownPtr sptr;
			GetInterfacePtr()->GetNextChild(&posCur, &sptr);
		}
	}
	void Uncache()
	{
		if (m_pPositions)
			delete m_pPositions;
	}
};

inline int _getclassnum(CChildrenArray &arrClasses, GuidKey lClassKeySample)
{
	for (int i = 0; i < arrClasses.ChildrenCount(); i++)
	{
		IUnknownPtr sptr = arrClasses.GetAt(i);
		INumeredObjectPtr sptrCNO(sptr);
		GuidKey lClassKey;
		sptrCNO->GetKey(&lClassKey);
		if (lClassKey == lClassKeySample)
			return i;
	}
	return -1;
}

#endif