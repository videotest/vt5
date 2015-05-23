#include "StdAfx.h"
#include "ClassTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CString _DirFromPath(CString sPath) // Extracts directory from full path
{
	TCHAR szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME],szExt[_MAX_EXT];
	_tsplitpath(sPath, szDrive, szDir, szFName, szExt);
	_tmakepath(sPath.GetBuffer(_MAX_PATH), szDrive, szDir, NULL, NULL);
	sPath.ReleaseBuffer();
	return sPath;
}

static bool IsImageFile(LPCTSTR lpstrPath) // Uses file extension
{
	char szExt[_MAX_EXT];
	_tsplitpath(lpstrPath, NULL, NULL, NULL, szExt);
	if (!_tcsicmp(szExt, _T(".woolz")) || !_tcsicmp(szExt, _T(".vti")) || !_tcsicmp(szExt, _T(".image")))
		return true;
	return false;
}

static bool IsWoolzFile(LPCTSTR lpstrPath) // Uses file extension
{
	char szExt[_MAX_EXT];
	_tsplitpath(lpstrPath, NULL, NULL, NULL, szExt);
	if (!_tcsicmp(szExt, _T(".woolz")))
		return true;
	return false;
}

static void _MakeScriptAddLearnFile(CStdioFile &file, LPCTSTR pszFilePath) // Add record for one file
{
	CString s;
	s.Format(_T("ActionManager.FileOpen \"%s\"\n"), pszFilePath);
	file.WriteString(s);
	file.WriteString(_T("ActionManager.CalcMeasResult\n"));
//	file.WriteString(_T("ActionManager.CalcNormalParametr\n"));
	file.WriteString(_T("ActionManager.AddAllObjectsToTeach\n"));
	file.WriteString(_T("ActionManager.FileClose\n"));
};


static void _MakeScriptAddLearnDir(CStdioFile &file, CString s, CListBox *pLog) // Adds all files in directory
{
	CFileFind ff;
	BOOL bFound = FALSE;
	BOOL b = ff.FindFile(s);
	while (b)
	{
		b = ff.FindNextFile();
		if (!ff.IsDirectory())
		{
			CString sPath = ff.GetFilePath();
			if (IsImageFile(sPath))
			{
				_MakeScriptAddLearnFile(file,sPath);
				bFound = TRUE;
			}
		}
	}
	if (!bFound)
	{
		CString s1;
		s1.Format(_T("%s contains no image files"), (LPCTSTR)s);
		pLog->AddString(s1);
	}
};

static CString _MakeCompareFilePath(LPCTSTR lpstrPathName, LPCTSTR lpstrCompDir)
{
	CString s;
	TCHAR szDrive[_MAX_DIR],szDir[_MAX_DIR],szFName[_MAX_FNAME],szExt[_MAX_EXT];
	_tsplitpath(lpstrCompDir, szDrive, szDir, NULL, NULL);
	_tsplitpath(lpstrPathName, NULL, NULL, szFName, szExt);
	_tcscat(szFName, _T("-t"));
	_tmakepath(s.GetBuffer(_MAX_PATH), szDrive, szDir, szFName, szExt);
	s.ReleaseBuffer();
	return s;
};


static CString _MakeFeatFilePath(LPCTSTR lpstrPathName)
{
	CString s;
	TCHAR szDrive[_MAX_DIR],szDir[_MAX_DIR],szFName[_MAX_FNAME];
	_tsplitpath(lpstrPathName, szDrive, szDir, szFName, NULL);
	_tmakepath(s.GetBuffer(_MAX_PATH), szDrive, szDir, szFName, _T(".feat"));
	s.ReleaseBuffer();
	return s;
};

static CString _NameExt(LPCTSTR lpstrPathName) // Extract name+extension from full path
{
	CString s;
	TCHAR szFName[_MAX_FNAME],szExt[_MAX_EXT];
	_tsplitpath(lpstrPathName, NULL, NULL, szFName, szExt);
	_tmakepath(s.GetBuffer(_MAX_PATH), NULL, NULL, szFName, szExt);
	s.ReleaseBuffer();
	return s;
}

static void _MakeScriptTestFile(CStdioFile &file, LPCTSTR pszTestPath, LPCTSTR pszComparePath)
{
	CString sTestNameExt(_NameExt(pszTestPath));
	CString sCompareNameExt(_NameExt(pszComparePath));
	CString s;
	s.Format(_T("ActionManager.FileOpen \"%s\"\n"), pszTestPath);
	file.WriteString(s);
	file.WriteString(_T("ActionManager.CalcMeasResult\n"));
//	file.WriteString(_T("ActionManager.CalcNormalParametr\n"));
	file.WriteString(_T("ActionManager.ClassifyObjects\n"));
	s.Format(_T("ActionManager.FileOpen \"%s\"\n"), pszComparePath);
	file.WriteString(s);
	file.WriteString(_T("ActionManager.CompareObjectsList 1,0\n"));
	s.Format(_T("ActionManager.FileClose \"%s\"\n"), sTestNameExt);
	file.WriteString(s);
	s.Format(_T("ActionManager.FileClose \"%s\"\n"), sCompareNameExt);
	file.WriteString(s);
}


static void _MakeScriptTestDir(CStdioFile &file, LPCTSTR pszTest, LPCTSTR pszCompare, CListBox *pLog)
{
	BOOL bFound = FALSE;
	file.WriteString(_T("ActionManager.CompareObjectsList 1,1\n"));
	CFileFind ff;
	BOOL b = ff.FindFile(pszTest);
	while (b)
	{
		b = ff.FindNextFile();
		if (!ff.IsDirectory())
		{
			CString sTestPath = ff.GetFilePath();
			if (IsImageFile(sTestPath))
			{
				CString sCmpPath = _MakeCompareFilePath(sTestPath,pszCompare);
				if (!CopyFile(sTestPath, sCmpPath, FALSE))
				{
					CString s;
					s.Format(_T("%s is absent and can not be created"), (LPCTSTR) sCmpPath);
					pLog->AddString(s);
				}
				if (IsWoolzFile(sTestPath))
				{
					CString sTestPathFeat = _MakeFeatFilePath(sTestPath);
					CFileFind ff1;
					if (ff1.FindFile(sTestPathFeat))
					{
						CString sCmpPathFeat = _MakeCompareFilePath(sTestPathFeat,pszCompare);
						if (!CopyFile(sTestPathFeat, sCmpPathFeat, FALSE))
						{
							CString s;
							s.Format(_T("%s is absent and can not be created"), (LPCTSTR) sCmpPathFeat);
							pLog->AddString(s);
						}
					}
					else
					{
						CString s;
						s.Format(_T("%s is absent"), (LPCTSTR) sTestPathFeat);
						pLog->AddString(s);
					}
				}
				_MakeScriptTestFile(file,sTestPath,sCmpPath);
				bFound = TRUE;
			}
		}
	}
	file.WriteString(_T("ActionManager.CompareObjectsList 1,2\n"));
	if (!bFound)
	{
		CString s1;
		s1.Format(_T("%s contains no image files"), pszTest);
		pLog->AddString(s1);
	}
};

static CString _MakeWC(LPCTSTR pszPath)
{
	CString s = pszPath;
	TCHAR szDrive[_MAX_DRIVE],szDir[_MAX_DIR];
	_tsplitpath(s, szDrive, szDir, NULL, NULL);
	_tmakepath(s.GetBuffer(_MAX_PATH), szDrive, szDir, NULL, NULL);
	s.ReleaseBuffer();
	s += _T("*.*");
	return s;
}


CString _AskForPath(BOOL bDir, LPCTSTR pszInitPath, LPCTSTR pszDefExt, LPCTSTR pszFilters) // Ask user for path
{
	CFileDialog FileDlg(TRUE, pszDefExt, bDir?NULL:pszInitPath, OFN_OVERWRITEPROMPT, pszFilters);
	FileDlg.m_ofn.lpstrInitialDir = pszInitPath;
	if (FileDlg.DoModal() == IDOK)
	{
		CString s = FileDlg.GetPathName();
		if (bDir)
			s = _DirFromPath(s);
		return s;
	}
	return pszInitPath;
}

void _MakeScriptAddLearn(CStdioFile &file, LPCTSTR pszTeachFilePath, CListBox *pLog)
{
	_MakeScriptAddLearnDir(file, _MakeWC(pszTeachFilePath), pLog);
}

void _MakeScriptTest(CStdioFile &file, LPCTSTR pszTestPath, LPCTSTR pszCompPath, CListBox *pLog)
{
	_MakeScriptTestDir(file, _MakeWC(pszTestPath), _MakeWC(pszCompPath), pLog);
}

