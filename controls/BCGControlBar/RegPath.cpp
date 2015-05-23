//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This source code is a part of BCGControlBar library.
// You may use, compile or redistribute it as part of your application 
// for free. You cannot redistribute it as a part of a software development 
// library without the agreement of the author. If the sources are 
// distributed along with the application, you should leave the original 
// copyright notes in the source code without any changes.
// This code can be used WITHOUT ANY WARRANTIES on your own risk.
// 
// For the latest updates to this library, check my site:
// http://welcome.to/bcgsoft
// 
// Stas Levin <bcgsoft@yahoo.com>
//*******************************************************************************

#include "stdafx.h"
#include "bcgcontrolbar.h"
#include "RegPath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString BCGGetRegPath (LPCTSTR lpszPostFix, LPCTSTR lpszProfileName)
{
	//---------------
	// By Alan Fritz:
	//---------------
	ASSERT (lpszPostFix != NULL);

	CString strReg;

	if (lpszProfileName != NULL &&
		lpszProfileName [0] != 0)
	{
		strReg = lpszProfileName;
	}
	else
	{
		CWinApp* pApp = AfxGetApp ();
		ASSERT_VALID (pApp);

		ASSERT (AfxGetApp()->m_pszRegistryKey != NULL);
		ASSERT (AfxGetApp()->m_pszProfileName != NULL);

		strReg = _T("SOFTWARE\\");

		// ***** By Yogesh Jagota
		CString strRegKey = pApp->m_pszRegistryKey;
		if (!strRegKey.IsEmpty ())
		{
			strReg += strRegKey;
			strReg += _T("\\");
		}
		//*****

		strReg += pApp->m_pszProfileName;
		strReg += _T("\\");
		strReg += lpszPostFix ;
		strReg += _T("\\");
	}
    
    return strReg;
}
