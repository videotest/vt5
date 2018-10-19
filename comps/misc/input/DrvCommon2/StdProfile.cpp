#include "StdAfx.h"
#include "StdLib.h"
#if !defined(COMMON1)
#include <ComDef.h>
#include "misc_utils.h"
#include "data5.h"
#include "com_main.h"
#endif
#include "StdProfile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CStdProfileManager *CStdProfileManager::m_pMgr;

inline LPCTSTR SecOrGen(LPCTSTR lpstrSecName)
{
	return lpstrSecName==NULL?_T("General"):lpstrSecName;
}

LPCTSTR CStdProfileManager::_MakeIniFileName(LPCTSTR lpstrAdd)
{
	static TCHAR szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME];
	static bool bInit = false;
	if (!bInit)
	{
		char szPath1[_MAX_PATH];
		if (GetModuleFileName(app::instance()->m_hInstance, szPath1, _MAX_PATH) == 0)
		{
			ASSERT(FALSE);
			return NULL;
		}
		GetLongPathName(szPath1, szPath, _MAX_PATH);
		_tsplitpath(szPath, szDrive, szDir, szFName, NULL);
		_bstr_t bstrPath = ::GetValueString(GetAppUnknown(), "\\Input", "IniDir", NULL);
		if (bstrPath.length() >0)
		{
			TCHAR szIniPath[_MAX_PATH];
			_tcsncpy(szIniPath, bstrPath, _MAX_PATH);
			int n = (int)_tcslen(szIniPath);
			if (szIniPath[n-1] != _T('\\') && szIniPath[n-1] != _T('/'))
				_tcsncat(szIniPath, _T("\\"), _MAX_PATH);
			LPCTSTR lpDir = _tcschr(szIniPath, _T(':'));
			if (lpDir)
			{
				_tcsncpy(szDrive, szIniPath, _MAX_DRIVE);
				szDrive[lpDir-szIniPath+1] = 0;
				lpDir++;
			}
			else
				lpDir = szIniPath;
			if (lpDir[0] != 0)
			{
				if (lpDir[0] == '\\' || lpDir[0] == '/')
					_tcscpy(szDir, lpDir);
				else
				{
					int n = (int)_tcslen(szDir);
					if (szDir[n-1] != '\\' && szDir[n-1] != '/')
						_tcsncat(szDir, _T("\\"), _MAX_DIR);
					_tcsncat(szDir, lpDir, _MAX_DIR);
				}
			}
		}
		bInit = true;
	}
	if (lpstrAdd == NULL)
		_tmakepath(szPath, szDrive, szDir, szFName, _T(".ini"));
	else
	{
		TCHAR szFName1[_MAX_FNAME];
		_tcscpy(szFName1, szFName);
		_tcsncat(szFName1, _T("-"), _MAX_FNAME);
		_tcsncat(szFName1, lpstrAdd, _MAX_FNAME);
		_tmakepath(szPath, szDrive, szDir, szFName1, _T(".ini"));
	}
	return szPath;
};

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
}

CStdProfileManager::~CStdProfileManager()
{
	m_pMgr = NULL;
}

void CStdProfileManager::InitMethodic(LPCTSTR lpMeth)
{
	m_sMethodic = lpMeth;
	InitMethodicIni();
}

void CStdProfileManager::InitCommonIni()
{
	m_sCommonIni = _MakeIniFileName(NULL);
	m_sDefaultIni = _MakeIniFileName(_T("Default"));
}

void CStdProfileManager::InitMethodicIni()
{
	if (m_sCommonIni.IsEmpty())
		InitCommonIni();
	if (m_sMethodic.IsEmpty())
	{
		m_sMethodicIni = m_sCommonIni;
		m_sMethDfltIni = m_sDefaultIni;
	}
	else
	{
		m_sMethodicIni = _MakeIniFileName(m_sMethodic);
		m_sMethDfltIni = _MakeIniFileName(m_sMethodic+_T("-Default"));
		WIN32_FIND_DATA fd;
		HANDLE h = FindFirstFile(m_sMethDfltIni, &fd);
		if (h == INVALID_HANDLE_VALUE)
			m_sMethDfltIni = m_sDefaultIni;
		else
			FindClose(h);
	}
}

LPCTSTR CStdProfileManager::GetIniName(bool bIgnoreMethodic)
{
	try
   {

	if (bIgnoreMethodic)
	{
		if (m_sCommonIni.IsEmpty())
			InitCommonIni();
		return m_sCommonIni;
	}
	else
	{
		if (m_sMethodicIni.IsEmpty())
			InitMethodicIni();
		return m_sMethodicIni;
	}
   }
	catch( CException* e )
   {
      
	  e->ReportError();
	  return 0;
   }
}

LPCTSTR CStdProfileManager::GetDefIniName(bool bIgnoreMethodic)
{
	if (bIgnoreMethodic)
	{
		if (m_sCommonIni.IsEmpty())
			InitCommonIni();
		return m_sDefaultIni;
	}
	else
	{
		if (m_sMethodicIni.IsEmpty())
			InitMethodicIni();
		return m_sMethDfltIni;
	}
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

static _variant_t StringToVar(CString &s)
{
	if (s.IsEmpty())
		return _variant_t();
	else if (isdigit_ru(s[0]))
	{
		if (s.Find('.') != -1)
			return _variant_t(_tstof(s), VT_R8);
		else
			return _variant_t(_ttol(s), VT_I4);
	}
	else
		return _variant_t(s);
}

VARIANT CStdProfileManager::_GetProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry, bool bIgnoreMethodic)
{
	CString s = _GetProfileString(lpSection, lpEntry, NULL, bIgnoreMethodic);
	return StringToVar(s);
}


int CStdProfileManager::_GetProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef, bool bIgnoreMethodic)
{
	return GetPrivateProfileInt(SecOrGen(lpSection), lpEntry, nDef, GetIniName(bIgnoreMethodic));
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
	CString s;
	GetPrivateProfileString(SecOrGen(lpSection), lpEntry, lpDefault, s.GetBuffer(4096), 4095,
		GetIniName(bIgnoreMethodic));
	s.ReleaseBuffer();
	return s;
}

VARIANT CStdProfileManager::GetProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry, bool bIgnoreMethodic)
{
	VARIANT var = _GetProfileValue(lpSection, lpEntry, bIgnoreMethodic);
	if (var.vt == VT_EMPTY)
		var = GetDefaultProfileValue(lpSection, lpEntry, bIgnoreMethodic);
	return var;
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
				WriteProfileInt(lpSection, lpEntry, n, bIgnoreMethodic);
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

COLORREF CStdProfileManager::GetProfileColor(LPCTSTR lpSection, LPCTSTR lpEntry, COLORREF clrDefault, bool bWriteToReg, bool bIgnoreMethodic)
{
	CString s;
	s.Format(_T("(%d,%d,%d)"), (int)GetRValue(clrDefault), (int)GetGValue(clrDefault), (int)GetBValue(clrDefault));
	s = GetProfileString(lpSection, lpEntry, s, bWriteToReg, bIgnoreMethodic);
	int r=0,g=0,b=0;
	_stscanf(s, _T("(%d,%d,%d)"), &r, &g, &b);
	return RGB(r,g,b);
}

VARIANT CStdProfileManager::GetDefaultProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry, bool bIgnoreMethodic)
{
	CString s = GetDefaultProfileString(lpSection, lpEntry, NULL, bIgnoreMethodic);
	return StringToVar(s);
}


int CStdProfileManager::GetDefaultProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef, bool bIgnoreMethodic)
{
	return ::GetPrivateProfileInt(SecOrGen(lpSection),lpEntry,nDef,GetDefIniName(bIgnoreMethodic));
}

double CStdProfileManager::GetDefaultProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dDef, bool *pbDefValueUsed, bool bIgnoreMethodic)
{
	if (lpSection == NULL) lpSection = _T("General");
	CString s(GetDefaultProfileString(lpSection, lpEntry, NULL, bIgnoreMethodic));
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

CString CStdProfileManager::GetDefaultProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpDefault,
	bool bIgnoreMethodic)
{
	CString s;
	::GetPrivateProfileString(SecOrGen(lpSection), lpEntry, lpDefault, s.GetBuffer(4096), 4095,
		GetDefIniName(bIgnoreMethodic));
	s.ReleaseBuffer();
	return s;
}

void CStdProfileManager::WriteProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nValue, bool bIgnoreMethodic)
{
	TCHAR szBuff[100];
	_itot(nValue, szBuff, 10);
	WritePrivateProfileString(SecOrGen(lpSection), lpEntry, szBuff, GetIniName(bIgnoreMethodic));
}

void CStdProfileManager::WriteProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry, VARIANT varValue, bool bIgnoreMethodic)
{
	switch (varValue.vt)
	{
	case VT_I4:
		WriteProfileInt(lpSection, lpEntry, varValue.lVal, bIgnoreMethodic);
		break;
	case VT_I2:
		WriteProfileInt(lpSection, lpEntry, varValue.iVal, bIgnoreMethodic);
		break;
	case VT_BSTR:
		WriteProfileString(lpSection, lpEntry, _bstr_t(varValue.bstrVal), bIgnoreMethodic);
		break;
	case VT_R8:
		WriteProfileDouble(lpSection, lpEntry, varValue.dblVal, bIgnoreMethodic);
		break;
	};
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
	WritePrivateProfileString(SecOrGen(lpSection), lpEntry, lpValue, GetIniName(bIgnoreMethodic));
}

/*void CStdProfileManager::InitSettings()
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
}*/

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

void GetAllEntries(LPCTSTR lpFileName, LPCTSTR lpSection, CStringArray &arrKeys)
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
	GetAllSections(GetDefIniName(false), arrSects);
	for (int i = 0; i < arrSects.GetSize(); i++)
	{
		CString s = arrSects[i];
		CStringArray arrKeys;
		GetAllEntries(GetDefIniName(false), s, arrKeys);
		for (int j = 0; j < arrKeys.GetSize(); j++)
		{
			CString sKey = arrKeys[j];
			CString sValue;
			TCHAR *p = sValue.GetBuffer(512);
			GetPrivateProfileString(s, sKey, NULL, p, 512, GetDefIniName(false));
			sValue.ReleaseBuffer();
			CStdProfileManager::m_pMgr->WriteProfileString(s, sKey, sValue);
		}
	}
}


