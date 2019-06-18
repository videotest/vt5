#include "StdAfx.h"
#include "MethCombo.h"
#include "StdProfile.h"
#include "misc_utils.h"
#include "Ids.h"
#include "MethNameDlg.h"
#include "debug.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static bool IsMethodicFile(LPCTSTR lpstrFileName)
{
	int nName = _tcslen(lpstrFileName);
	int nDefault = _tcslen(_T("-Default"));
	if (nName <= nDefault) return true;
	if (_tcscmp(lpstrFileName+nName-nDefault,_T("-Default"))==0)
		return false;
	return true;
}


static int Find(CStringArray &arr, LPCTSTR lpstr)
{
	for (int i = 0; i < arr.GetSize(); i++)
	{
		CString s = arr.GetAt(i);
		if (s == lpstr)
			return i;
	}
	return -1;
}

int CMethodicsComboValue::s_idsPredefined = -1;

CMethodicsComboValue g_Methodics(-1);

CMethodicsComboValue::CMethodicsComboValue(int id, int nChain) :
	CCamIntComboValue(NULL, NULL, NULL, 0, 0, nChain, id, 0)
{
	m_nValues = 0;
	m_pDescr = NULL;
}

CMethodicsComboValue::~CMethodicsComboValue()
{
	Deinit();
}

void CMethodicsComboValue::Deinit()
{
	for (int i = 0; i < m_nValues; i++)
	{
		if (m_pDescr[i].lpstrValue)
			delete m_pDescr[i].lpstrValue;
	}
	delete m_pDescr;
	m_nValues = 0;
	m_pDescr = NULL;
	m_saMethodics.RemoveAll();
}

void CMethodicsComboValue::OnInitControl()
{
	Deinit();
	if (CStdProfileManager::m_pMgr->m_sCommonIni.IsEmpty())
		CStdProfileManager::m_pMgr->InitCommonIni();
	CString sCommonIni = CStdProfileManager::m_pMgr->m_sCommonIni;
	TCHAR szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME];
	_tsplitpath(sCommonIni, szDrive, szDir, szFName, NULL);
	_tcsncat(szFName,_T("*"),_MAX_FNAME);
	_tmakepath(szPath,szDrive,szDir,szFName,_T(".ini"));
	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile(szPath,&fd);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (IsMethodicFile(fd.cFileName))
			{
				_tmakepath(szPath, szDrive, szDir, fd.cFileName, NULL);
				m_saMethodics.Add(szPath);
			}
		}
		while (FindNextFile(h, &fd));
		FindClose(h);
		if (Find(m_saMethodics, sCommonIni) == -1)
			m_saMethodics.Add(sCommonIni);
		if (CStdProfileManager::m_pMgr->m_sMethodicIni != sCommonIni &&
			Find(m_saMethodics, CStdProfileManager::m_pMgr->m_sMethodicIni) == -1)
			m_saMethodics.Add(CStdProfileManager::m_pMgr->m_sMethodicIni);
		m_nValues = m_saMethodics.GetSize();
		m_pDescr = new INTCOMBOITEMDESCR[m_nValues];
		memset(m_pDescr, 0, m_nValues*sizeof(INTCOMBOITEMDESCR));
		m_vi.nMax = m_saMethodics.GetSize();
		m_vi.nCurrent = 0;
		for (int i = 0; i < m_saMethodics.GetSize(); i++)
		{
			CString sPath = m_saMethodics[i];
			CString sMeth = MethodicByMethodicIni(sPath);
			if (sMeth == CStdProfileManager::m_pMgr->m_sMethodic)
				m_vi.nCurrent = i;
			CString sMethUsrName = MethUsrNameByMethIni(sPath);
			m_pDescr[i].nValue = i;
			m_pDescr[i].lpstrValue = new TCHAR[sMethUsrName.GetLength()+1];
			_tcscpy((LPTSTR)m_pDescr[i].lpstrValue, sMethUsrName);
		}
		m_CBoxRData.nValues = m_nValues;
		m_CBoxRData.pnValues = m_pDescr;
	}
}

CString CMethodicsComboValue::MethodicByMethodicIni(LPCTSTR lpstrMethIni)
{
	TCHAR szName[_MAX_FNAME],szCName[_MAX_PATH];
	_tsplitpath(lpstrMethIni, NULL, NULL, szName, NULL);
	CString sCommonIni = CStdProfileManager::m_pMgr->m_sCommonIni;
	_tsplitpath(sCommonIni, NULL, NULL, szCName, NULL);
	int n = _tcslen(szCName);
	if (_tcsncmp(szName, szCName, n) == 0)
	{
		if (szName[n] == _T('-'))
			return CString(szName+n+1);
		else
			return CString(szName+n);
	}
	return szName;
}

CString CMethodicsComboValue::MethUsrNameByMethIni(LPCTSTR lpstrMethIni, BOOL *pbDefault)
{
	TCHAR szName[_MAX_FNAME];
	_bstr_t sLangName = ::GetValueString(::GetAppUnknown(), "General", "Language", "en");
	if (pbDefault) *pbDefault = FALSE;
	if (GetPrivateProfileString(_T("MethodicTitles"), sLangName, NULL, szName, _MAX_FNAME,
		lpstrMethIni) == 0)
	{
		CString sName = MethodicByMethodicIni(lpstrMethIni);
		if (sName.IsEmpty())
		{
			if (s_idsPredefined > 0)
				sName.LoadString(s_idsPredefined);
			if (sName.IsEmpty())
				sName = _T("Predefined");
			if (pbDefault) *pbDefault = TRUE;
		}
		return sName;
	}
	return CString(szName);
}


void CMethodicsComboValue::SetValueInt(int n, bool bReset)
{
	__super::SetValueInt(n, bReset);
	CString sIni = m_saMethodics[n];
	CString sMeth = MethodicByMethodicIni(sIni);
	CStdProfileManager::m_pMgr->InitMethodic(sMeth);
}

CString CMethodicsComboValue::UniqueName(LPCTSTR lpstrMethName)
{
	int n = _tcslen(lpstrMethName);
	while (n > 0 && _istdigit(lpstrMethName[n-1]))
		n--;
	int nNum = _ttoi(&lpstrMethName[n])+1;
	CString sNameBase(lpstrMethName, n);
	CString sMethName;
	do
	{
		TCHAR szBuff[60];
		_itot(nNum, szBuff, 10);
		sMethName = sNameBase+CString(szBuff);
		bool bFound = false;
		for (int i = 0; i < m_saMethodics.GetSize(); i++)
		{
			CString s = MethodicByMethodicIni(m_saMethodics.GetAt(i));
			if (s == sMethName)
			{
				bFound = true;
				break;
			}
		}
		if (!bFound)
			break;
		nNum++;
	}
	while(1);
	return sMethName;
}

CString CMethodicsComboValue::UniqueValue(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, LPCTSTR lpstrValue)
{
	int n = _tcslen(lpstrValue);
	while (n > 0 && _istdigit(lpstrValue[n-1]))
		n--;
	int nNum = _ttoi(&lpstrValue[n])+1;
	CString sNameBase(lpstrValue, n);
	CString sNewValue;
	do
	{
		TCHAR szBuff[60];
		_itot(nNum, szBuff, 10);
		sNewValue = sNameBase+CString(szBuff);
		bool bFound = false;
		for (int i = 0; i < m_saMethodics.GetSize(); i++)
		{
			TCHAR szValue[256];
			if (GetPrivateProfileString(lpstrSec, lpstrEntry, NULL, szValue, 256,
				m_saMethodics.GetAt(i)) > 0)
			{
				if (sNewValue == szValue)
				{
					bFound = true;
					break;
				}
			}
		}
		if (!bFound)
			break;
		nNum++;
	}
	while(1);
	return sNewValue;
}


int CMethodicsComboValue::NewMethodic()
{
	CString sCurMethName = CStdProfileManager::m_pMgr->GetMethodic();
	CString sNewMethName;
	if (sCurMethName.IsEmpty())
	{
		sNewMethName.LoadString(g_nStringIdNewMethodic);
		sNewMethName = UniqueName(sNewMethName);
	}
	else
		sNewMethName = UniqueName(sCurMethName);
	CString sCurMethIni  = CStdProfileManager::m_pMgr->m_sMethodicIni;
	CString sNewMethIni  = CStdProfileManager::_MakeIniFileName(sNewMethName);
	_bstr_t sLangName = ::GetValueString(::GetAppUnknown(), "General", "Language", "en");
	CString sDefTitle;
	if (sCurMethName.IsEmpty())
		sDefTitle = sNewMethName;
	else
	{
		sDefTitle = MethUsrNameByMethIni(sCurMethIni);
		sDefTitle = UniqueValue(_T("MethodicTitles"), sLangName, sDefTitle);
	}
	CMethNameDlg dlg(sDefTitle);
	if (dlg.DoModal() == IDOK)
		sDefTitle = dlg.m_sName;
	else
		return operator int();
	if (CopyFile(sCurMethIni, sNewMethIni, FALSE))	
	{
		CStringArray saTitles;
		GetAllEntries(sCurMethIni, _T("MethodicTitles"), saTitles);
		BOOL bDefFound = FALSE;
		for (int i = 0; i < saTitles.GetSize(); i++)
		{
			CString sEntry = saTitles.GetAt(i);
			TCHAR szValue[256];
			if (GetPrivateProfileString(_T("MethodicTitles"), sEntry, NULL, szValue, 256, sCurMethIni) > 0)
			{
				CString sNewVal;
				if (sEntry == (LPCTSTR)sLangName)
				{
					sNewVal = sDefTitle;
					bDefFound = TRUE;
				}
				else
					sNewVal = UniqueValue(_T("MethodicTitles"), sEntry, szValue);
				WritePrivateProfileString(_T("MethodicTitles"), sEntry, sNewVal, sNewMethIni);
			}
		}
		if (!bDefFound)
			WritePrivateProfileString(_T("MethodicTitles"), sLangName, sDefTitle, sNewMethIni);
		WritePrivateProfileString(_T("Methodic"), _T("CanDelete"), _T("1"), sNewMethIni);

		OnInitControl();
		int nNew = -1;
		for (int i = 0; i < m_saMethodics.GetSize(); i++)
		{
			if (sNewMethIni == m_saMethodics.GetAt(i))
			{
				nNew = i;
			}
		}
		if (nNew > -1)
		{
			SetValueInt(nNew, false);
			CStdProfileManager::m_pMgr->WriteProfileString(_T("Methodics"), _T("Default"),
				CStdProfileManager::m_pMgr->GetMethodic(), true);
			ReinitControls();
		}
	}
	return -1;
}

