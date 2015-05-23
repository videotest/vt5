#pragma once

#include <misc_utils.h>

/*class CIniValue
{
	static CIniValue *s_pFirst;
	CIniValue *m_pNext;
protected:
	bool m_bInit;
	LPCTSTR m_lpEntry;
	LPCTSTR m_lpSection;
	CIniValue(LPCTSTR lpSection, LPCTSTR lpEntry);
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
};*/

class CFishColor : public CIniValue
{
	int m_nSysIndex;
	COLORREF m_clr;
	static int m_nCustomize;
protected:
	void OnReset();
public:
	CFishColor(LPCTSTR lpstrName, int nSysIndex);
	operator COLORREF();
};

extern CFishColor g_BackgroundColor,g_ActiveColor,g_3DLightColor,g_3DHilightColor,
	g_3DDKShadowColor,g_PaneText;

extern CIntIniValue g_bDefColors;
extern CColorIniValue g_clrCombi;
