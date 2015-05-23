// IV460Drv.h : main header file for the IV460DRV DLL
//

#if !defined(AFX_IV460DRV_H__A279F66E_2D33_4DCE_B59D_3383D689779F__INCLUDED_)
#define AFX_IV460DRV_H__A279F66E_2D33_4DCE_B59D_3383D689779F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CIV460DrvApp
// See IV460Drv.cpp for the implementation of this class
//
#include "VT5Profile.h"

class CIV460DrvApp : public CWinApp
{
public:
	CIV460DrvApp();
	void InitSettings();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIV460DrvApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CIV460DrvApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CIV460Profile : public CVT5ProfileManager
{
public:
	CIV460Profile();
	virtual int GetDefaultProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef);
	virtual void ValidateInt(LPCTSTR lpSection, LPCTSTR lpEntry, int &nValidate);
//	virtual COLORREF GetProfileColor(LPCTSTR lpSection, LPCTSTR lpEntry, COLORREF clr = RGB(255, 0, 255));
//	virtual void WriteProfileColor(LPCTSTR lpSection, LPCTSTR lpEntry, COLORREF clVal, bool bIgnoreMethodic = false);

};

extern CIV460Profile g_IV460Profile;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IV460DRV_H__A279F66E_2D33_4DCE_B59D_3383D689779F__INCLUDED_)
