#pragma once

class CIniValue
{
	static CIniValue *s_pFirst;
	CIniValue *m_pNext;
protected:
	bool m_bInit;
	LPCTSTR m_lpEntry;
	LPCTSTR m_lpSection;
	CIniValue(LPCTSTR lpSection, LPCTSTR lpEntry);
	~CIniValue();
	virtual void OnReset();
public:
	static void Reset();
};

class CIntIniValue : public CIniValue
{
	int m_nValue;
public:
	CIntIniValue(LPCTSTR lpSection, LPCTSTR lpEntry, int nDefault);
	operator int();
};

class CColorIniValue : public CIniValue
{
	COLORREF m_clr;
public:
	CColorIniValue(LPCTSTR lpSection, LPCTSTR lpEntry, COLORREF  clrDefault);
	operator COLORREF();
};

