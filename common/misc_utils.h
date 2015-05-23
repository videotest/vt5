#ifndef __misc_utils_h__
#define __misc_utils_h__

#include "defs.h"
#include "image5.h"

std_dll bool RecursiveWriteEntry( INamedData* ptrSrc, INamedData* ptrTarget, LPCSTR pszEntry, bool bOvewrite );
std_dll bool CopyObjectNamedData( IUnknown* punk_src, IUnknown* punk_target, LPCSTR pszEntry, bool bOvewrite );
std_dll int GetImagePaneCount( IUnknown *punkImage );
std_dll void CopyPropertyBag(  IUnknown* punk_src, IUnknown* punk_target, bool bOvewrite );
std_dll bool IsNeedSaveBackup( LPCTSTR lpstrPathName );
// nMode == 0 - only object, 1 - summary rectangle object and all children
std_dll RECT GetObjectRectangle(IUnknown *punkObject, int nMode);


class std_dll CIniValue
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

class std_dll CIntIniValue : public CIniValue
{
	int m_nValue;
public:
	CIntIniValue(LPCTSTR lpSection, LPCTSTR lpEntry, int nDefault);
	operator int();
};

class std_dll CColorIniValue : public CIniValue
{
	COLORREF m_clr;
public:
	CColorIniValue(LPCTSTR lpSection, LPCTSTR lpEntry, COLORREF  clrDefault);
	operator COLORREF();
};

class std_dll CDoubleIniValue : public CIniValue
{
	double m_fValue;
public:
	CDoubleIniValue(LPCTSTR lpSection, LPCTSTR lpEntry, double fDefault);
	operator double();
};


class std_dll CDisableAutoSelect
{
	BOOL m_bPrev;
public:
	CDisableAutoSelect();
	~CDisableAutoSelect();
};

#endif//__misc_utils_h__
