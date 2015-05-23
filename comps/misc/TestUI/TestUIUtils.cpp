#include "StdAfx.h"
#include "TestUI.h"
#include "TestUIUtils.h"

CString _TestName(ITestItem *pTI)
{
	BSTR bstrPath = NULL;
	pTI->GetPath(&bstrPath);
	CString sName = _TestNameFromPath(CString(bstrPath));
	::SysFreeString(bstrPath);
	return sName;
}

CString _TestFullName(ITestItem *pTI, int iCond)
{
	BSTR bstrPath = NULL;
	pTI->GetPath(&bstrPath);
	CString sPath(bstrPath);
	::SysFreeString(bstrPath);
	return _TestFullNameFromPath(sPath,iCond);
}


CString _TestNameFromPath(LPCTSTR lpstrPath)
{
	//TCHAR szFName[_MAX_FNAME];
	//_tsplitpath(lpstrPath, NULL, NULL, szFName, NULL);

	CString strPath = lpstrPath;

	int nIndex = strPath.ReverseFind( '\\' );
	CString strName = strPath.Right( strPath.GetLength() - nIndex - 1 );

	return strName;
}

CString _TestPathFromPath(LPCTSTR lpstrPath)
{
	//TCHAR szFName[_MAX_FNAME];
	//_tsplitpath(lpstrPath, NULL, NULL, szFName, NULL);

	CString strPath = lpstrPath;

	int nIndex = strPath.ReverseFind( '\\' );
	CString strName = strPath.Left( nIndex - 1 );

	return strName;
}

CString _TestFullNameFromPath(LPCTSTR lpstr, int iCond)
{
	CString sPath(lpstr);
	CString sStartPath = ::GetValueString(GetAppUnknown(), "Paths", "TestDBPath", NULL);
	if (_tcsncmp(sPath,sStartPath,sStartPath.GetLength())==0)
		sPath = sPath.Mid(sStartPath.GetLength());
	if (sPath.GetLength() > 0 && (sPath[0] == '\\' || sPath[0] == '/'))
		sPath = sPath.Mid(1);
	CString sName;
	LPCTSTR lpstrPath = sPath;
	int iPrev = 0;
	for (int i = 0; lpstrPath[i]; i++)
	{
		if (lpstrPath[i] == '\\' || lpstrPath[i] == '/')
		{
			if (i > iPrev)
			{
				CString s(&lpstrPath[iPrev], i-iPrev);
				sName += s;
				sName += _T("::");
			}
			iPrev = i+1;
		}
	}
	if (iPrev < i)
		sName += CString(&lpstrPath[iPrev]);
	if (iCond > 0)
	{
		CString sSuff;
		sSuff.Format(_T("::%d"), iCond);
		sName += sSuff;
	}
	return sName;
}

CTestInfoRec *CTestInfoArray::FindRec(LPCTSTR lpszName, int nCond)
{
	for (int i = GetSize()-1; i >= 0; i--)
	{
		if (ElementAt(i).m_sTestName == lpszName && ElementAt(i).m_nCond == nCond)
			return &ElementAt(i);
	}
	return NULL;
}

