#if !defined(__TwainCol_H__)
#define __TwainCol_H__

#include "twain.h"

class CTwainNotImplException : public CException
{
	DECLARE_DYNAMIC(CTwainNotImplException)
	CString m_sFeature;
public:
	CTwainNotImplException(const char *pszName);
	int ReportError(UINT nType = MB_OK, UINT nMessageID = 0);
};

class CTwainContainer
{
protected:
	HANDLE m_hContainer;
public:
	CTwainContainer();
	~CTwainContainer();
	HANDLE GetHandle() { return m_hContainer;}
};

class CTwainOneValue : public CTwainContainer
{
public:
	// for creation after MSG_GET
	CTwainOneValue(HANDLE hContainer)
		{ m_hContainer = hContainer;}
	// for creation for MSG_SET
	CTwainOneValue(float Value, int nType = TWTY_UINT16);
	float GetValue();
};

class CTwainArray : public CTwainContainer
{
protected:
	float GetValue(pTW_ARRAY pval, int index);
	void SetValue(pTW_ARRAY pval, int index, float value);
public:
	// for creation after MSG_GET
	CTwainArray(HANDLE hContainer)
		{ m_hContainer = hContainer;}
	// for creation for MSG_SET
	CTwainArray(int nNumOfItems, int nType = TWTY_UINT16);
	int GetNumOfItems();
	float GetAt(int index);
	void SetAt(int index, float value);
};

class CTwainEnumeration : public CTwainContainer
{
protected:
	float GetValue(pTW_ENUMERATION pval, int index);
	void SetValue(pTW_ENUMERATION pval, int index, float value);
public:
	// for creation after MSG_GET
	CTwainEnumeration(HANDLE hContainer)
		{ m_hContainer = hContainer;}
	// for creation for MSG_SET
	CTwainEnumeration(int nNumOfItems, float nCurrent = 0, 
		float nDefault = 0, int nType = TWTY_UINT16);
	int GetNumOfItems();
	int GetCurrent();
	int GetDefault();
	float GetAt(int index);
	void SetAt(int index, float value);
	int Find(float value); // returns position or -1
	int FindMoreOrEqual(float value); // returns position or -1
	int FindMax();
};

class CTwainRange : public CTwainContainer
{
public:
	// for creation after MSG_GET
	CTwainRange(HANDLE hContainer)
		{ m_hContainer = hContainer;}
	// for creation for MSG_SET
	CTwainRange(float fMin, float fMax, float fStep, float fCurrent,
		float fDefault, int nType = TWTY_FIX32);
	float GetMin();
	float GetMax();
	float GetStep();
	float GetCurrent();
	float GetDefault();
	float FindMoreOrEqual(float value);
};

class CTwainCapability
{
protected:
	TW_CAPABILITY m_twCapability;
	CTwainContainer *m_pContainer;
public:
	CTwainCapability();
	CTwainCapability(int Cap, int ConType = TWON_DONTCARE16,
		CTwainContainer *pContainer = NULL);
	~CTwainCapability();
	void Create(int Cap, int ConType = TWON_DONTCARE16,
		CTwainContainer *pContainer = NULL);
	void CreateContainer();
	TW_CAPABILITY &GetData() { return m_twCapability;}
	int GetCapType() { return m_twCapability.Cap; }
	int GetConType() { return m_twCapability.ConType; }
	CTwainOneValue    *AsOneValue();
	CTwainEnumeration *AsEnum();
	CTwainArray       *AsArray();
	CTwainRange       *AsRange();
	BOOL IsEmpty();
	// after return from MSG_GET and other
	void Check();
};


inline CTwainOneValue *CTwainCapability::AsOneValue()
{
	ASSERT(m_pContainer);
	ASSERT(GetConType() == TWON_ONEVALUE);
	return (CTwainOneValue *)m_pContainer;
}

inline CTwainEnumeration *CTwainCapability::AsEnum()
{
	ASSERT(m_pContainer);
	ASSERT(GetConType() == TWON_ENUMERATION);
	return (CTwainEnumeration *)m_pContainer;
}

inline CTwainArray *CTwainCapability::AsArray()
{
	ASSERT(m_pContainer);
	ASSERT(GetConType() == TWON_ARRAY);
	return (CTwainArray *)m_pContainer;
}

inline CTwainRange *CTwainCapability::AsRange()
{
	ASSERT(m_pContainer);
	ASSERT(GetConType() == TWON_RANGE);
	return (CTwainRange *)m_pContainer;
}

#endif