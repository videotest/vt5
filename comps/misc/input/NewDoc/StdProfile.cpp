#include "StdAfx.h"
#include "StdProfile.h"
#include "StdLib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CStdProfileManager *CStdProfileManager::m_pMgr;

void CStdProfileManager::ExtractSectionAndEntry(LPCTSTR lpName, CString &sSec, CString &sEntry)
{
	CString s(lpName);
	int n = s.Find("\\/");
	if (n == -1)
	{
		sSec = "Settings";
		sEntry = s;
	}
	else
	{
		sSec = s.Left(n);
		sEntry = s.Mid(n+1);
	}
}

CStdProfileManager::CStdProfileManager(bool bSingleProfile)
{
	if (bSingleProfile)
	{
		ASSERT(m_pMgr == NULL);
		m_pMgr = this;
	}
	m_strDefaultIni = "Default.ini";
}

CStdProfileManager::~CStdProfileManager()
{
	m_pMgr = NULL;
}

void CStdProfileManager::InitMethodic(LPCTSTR lpMeth)
{
	m_sMethodic = lpMeth;
}

CString CStdProfileManager::SectionName(LPCTSTR  lpSecDef, bool bIgnoreMethodic)
{
	if (lpSecDef == NULL || *lpSecDef == 0) lpSecDef = _T("General");
	if (m_sMethodic.IsEmpty() || bIgnoreMethodic) return lpSecDef;
	CString sSecName(m_sMethodic);
	sSecName += CString(_T("\\"));
	sSecName += CString(lpSecDef);
	return sSecName;
}

int CStdProfileManager::_GetProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef, bool bIgnoreMethodic)
{
	CString sSection(SectionName(lpSection,bIgnoreMethodic));
	return AfxGetApp()->GetProfileInt(sSection, lpEntry, nDef);
}

double CStdProfileManager::_GetProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dDef, bool *pbDefValueUsed, bool bIgnoreMethodic)
{
	CString sSection(lpSection);
	CString s(_GetProfileString(sSection, lpEntry, NULL));
	double dRet = dDef;
	if (!s.IsEmpty())
	{
		dRet= atof(s);
		if (pbDefValueUsed)
			*pbDefValueUsed = false;
	}
	else
	{
		if (pbDefValueUsed)
			*pbDefValueUsed = true;
	};
	return dRet;
}

CString CStdProfileManager::_GetProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpDefault, bool bIgnoreMethodic)
{
	CString sSection(SectionName(lpSection,bIgnoreMethodic));
	return AfxGetApp()->GetProfileString(sSection, lpEntry, lpDefault);
}

int CStdProfileManager::GetProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef, bool bWriteToReg, bool bIgnoreMethodic)
{
	int n = _GetProfileInt(lpSection, lpEntry, -1, bIgnoreMethodic);
	if (n == -1)
	{
		n = GetDefaultProfileInt(lpSection, lpEntry, -1);
		if (n == -1)
		{
			n = nDef;
			if (bWriteToReg)
				WriteProfileInt(lpSection, lpEntry, n);
		}
	}
	ValidateInt(lpSection, lpEntry, n);
	return n;
}

double CStdProfileManager::GetProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dDef, bool *pbDefValueUsed,
	bool bWriteToReg, bool bIgnoreMethodic)
{
	bool bDefValueUsed;
	double dRet = _GetProfileDouble(lpSection, lpEntry, dDef, &bDefValueUsed, bIgnoreMethodic);
	if (bDefValueUsed)
	{
		dRet = GetDefaultProfileDouble(lpSection, lpEntry, dDef);
		if (bWriteToReg)
			WriteProfileDouble(lpSection, lpEntry, dRet);
	}
	ValidateDouble(lpSection, lpEntry, dRet);
	return dRet;
}


CString CStdProfileManager::GetProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpDefault, bool bWriteToReg, bool bIgnoreMethodic)
{
	CString s = _GetProfileString(lpSection, lpEntry, NULL, bIgnoreMethodic);
	if (s.IsEmpty())
	{
		s = GetDefaultProfileString(lpSection, lpEntry, NULL);
		if (s.IsEmpty() && lpDefault && *lpDefault)
		{
			s = lpDefault;
			if (bWriteToReg)
				WriteProfileString(lpSection, lpEntry, s);
		}
	}
	return s;
}

int CStdProfileManager::GetDefaultProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef)
{
	if (lpSection == NULL) lpSection = _T("General");
	if (m_sDefaultProfile.IsEmpty())
		InitSettings();
	return ::GetPrivateProfileInt(lpSection,lpEntry,nDef,m_sDefaultProfile);
}

double CStdProfileManager::GetDefaultProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dDef, bool *pbDefValueUsed)
{
	if (lpSection == NULL) lpSection = _T("General");
	CString s(GetDefaultProfileString(lpSection, lpEntry, NULL));
	double dRet = dDef;
	if (s.IsEmpty())
	{
		if (pbDefValueUsed)
			*pbDefValueUsed = true;
	}
	else
	{
		dRet = atof(s);
		if (pbDefValueUsed)
			*pbDefValueUsed = false;
	}
	return dRet;
}

CString CStdProfileManager::GetDefaultProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpDefault)
{
	if (lpSection == NULL) lpSection = _T("General");
	if (m_sDefaultProfile.IsEmpty())
		InitSettings();
	CString s;
	::GetPrivateProfileString(lpSection, lpEntry, lpDefault, s.GetBuffer(1024), 1024, m_sDefaultProfile);
	s.ReleaseBuffer();
	return s;
}

void CStdProfileManager::WriteProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nValue, bool bIgnoreMethodic)
{
	CString sSection(SectionName(lpSection,bIgnoreMethodic));
	AfxGetApp()->WriteProfileInt(sSection, lpEntry, nValue);
}

void CStdProfileManager::WriteProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dValue, bool bIgnoreMethodic)
{
	CString sSection(lpSection);
	CString s;
	s.Format("%f", dValue);
	WriteProfileString(sSection, lpEntry, s, bIgnoreMethodic);
}


void CStdProfileManager::WriteProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpValue, bool bIgnoreMethodic)
{
	CString sSection(SectionName(lpSection,bIgnoreMethodic));
	AfxGetApp()->WriteProfileString(sSection, lpEntry, lpValue);
}

void CStdProfileManager::InitSettings()
{
	// Produse new .ini file name
	TCHAR szPath[_MAX_PATH];
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	// Produce default file name. It must be in same directory as a driver.
	GetModuleFileName(AfxGetInstanceHandle(), szPath, _MAX_PATH);
	_tsplitpath(szPath, szDrive, szDir, NULL, NULL);
	_tmakepath(szPath, szDrive, szDir, m_strDefaultIni, NULL);
	m_sDefaultProfile = szPath;
}

static void GetAllSections(LPCTSTR lpFileName, CStringArray &arr)
{
	arr.RemoveAll();
	TCHAR szBuffer[512];
	int n = GetPrivateProfileString(NULL, NULL, NULL, szBuffer, 512, lpFileName);
	const TCHAR *p = szBuffer;
	while (*p)
	{
		CString s(p);
		arr.Add(s);
		p += strlen(p)+1;
	}
}

static void GetAllEntries(LPCTSTR lpFileName, LPCTSTR lpSection, CStringArray &arrKeys)
{
	arrKeys.RemoveAll();
	TCHAR szBuffer[512];
	int n = GetPrivateProfileString(lpSection, NULL, NULL, szBuffer, 512, lpFileName);
	const TCHAR *p = szBuffer;
	while (*p)
	{
		CString s(p);
		arrKeys.Add(s);
		p += strlen(p)+1;
	}
}

void CStdProfileManager::ResetSettings()
{
	CStringArray arrSects;
	GetAllSections(m_sDefaultProfile, arrSects);
	for (int i = 0; i < arrSects.GetSize(); i++)
	{
		CString s = arrSects[i];
		CStringArray arrKeys;
		GetAllEntries(m_sDefaultProfile, s, arrKeys);
		for (int j = 0; j < arrKeys.GetSize(); j++)
		{
			CString sKey = arrKeys[j];
			CString sValue;
			TCHAR *p = sValue.GetBuffer(512);
			GetPrivateProfileString(s, sKey, NULL, p, 512, m_sDefaultProfile);
			sValue.ReleaseBuffer();
			CStdProfileManager::m_pMgr->WriteProfileString(s, sKey, sValue);
		}
	}
}


