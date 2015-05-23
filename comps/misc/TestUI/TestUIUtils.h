#pragma once

#include "test_man.h"

extern CString _TestName(ITestItem *pTI);
extern CString _TestFullName(ITestItem *pTI, int iCond);
extern CString _TestNameFromPath(LPCTSTR lpstrPath);
extern CString _TestFullNameFromPath(LPCTSTR lpstrPath, int iCond);
extern CString _TestNameFromPath(LPCTSTR lpstrPath);

struct CTestInfoRec
{
	CString m_sTestName;
	CString m_str_info;
	int m_nCond;
	int m_nDeviation;
	bool m_bPassed,m_bSuccess;
	TEST_ERR_DESCR *m_pErrDescr;
	CTestInfoRec()
	{
		m_nCond = 0;
		m_bPassed = false;
		m_bSuccess = false;
		m_nDeviation = 0;
	}
	CTestInfoRec(LPCTSTR lpstrName,	LPCTSTR lpstr_info,int nCond, bool bPassed, bool bSuccess, int nDeviation)
	{
		m_nCond = nCond;
		m_sTestName = lpstrName;
		m_str_info = lpstr_info;
		m_bPassed = bPassed;
		m_bSuccess = bSuccess;
		m_nDeviation = nDeviation;
	}
};

class CTestInfoArray : public CArray<CTestInfoRec,CTestInfoRec&>
{
public:
	CTestInfoRec *FindRec(LPCTSTR lpszName, int nCond);
};
