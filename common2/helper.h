#ifndef __helper_h__
#define __helper_h__


#define MAX_STRING 255
class StringArray
{
public:
	StringArray(long nInitSize = 30, long nGrowBy = 20);
	virtual ~StringArray();

	void Add(const char* szStr);
	char* GetAt(long nIndex);
	char* operator[](long nIndex){return GetAt(nIndex);};
	long GetSize(){return m_nSize;};

protected:

	void ReAllocArray();

private:
	char (*m_pszArray)[MAX_STRING];

	long m_nDynamicSize;		//allocated memory
	long m_nSize;				//ordinary array size
	long m_nGrowBy;
};



struct _POS{};
typedef _POS* POS;

class PtrList
{

public:
	PtrList();
	virtual ~PtrList();

	POS GetHeadPos();
	void* GetNextPtr(POS& pos);
	void AddToTail(void* pPtr);

	long GetNodeCount(){return m_nCounter;};
	void* GetByPtrIdx(long nIdx);

	void KillThemAll();

private:

	struct Node
	{
		void* pPtr;
		Node* pNext;
	}*m_pHead, *m_pTail;

	long  m_nCounter;

};

#endif