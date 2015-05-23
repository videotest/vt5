// Data.h : main header file for the DATA DLL
//

#if !defined(AFX_DATA_H__526B6D37_1CAD_11D3_A5CF_0000B493FF1B__INCLUDED_)
#define AFX_DATA_H__526B6D37_1CAD_11D3_A5CF_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDataApp
// See Data.cpp for the implementation of this class
//

class CDataApp : public CWinApp
{
public:
	CDataApp();
	~CDataApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDataApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CString GetProviderUserName( IUnknown* punkObj, bool bReturnObjNameIfFailed = true );
bool IsDBaseRootObject( IUnknown* punkObj, IUnknown* punkData );


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATA_H__526B6D37_1CAD_11D3_A5CF_0000B493FF1B__INCLUDED_)
