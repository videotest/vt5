// MeasManual.h : main header file for the MEASMANUAL DLL
//

#if !defined(AFX_MEASMANUAL_H__AEC5B060_AE07_4B73_8834_6497E606317F__INCLUDED_)
#define AFX_MEASMANUAL_H__AEC5B060_AE07_4B73_8834_6497E606317F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#define CROSS_OFFSET	5
#define ARC_SIZE		15
#define SENSZONE_LENGHT	5


/////////////////////////////////////////////////////////////////////////////
// CMeasManualApp
// See MeasManual.cpp for the implementation of this class
//

class CMeasManualApp : public CWinApp
{
public:
	CMeasManualApp();
	~CMeasManualApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeasManualApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMeasManualApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
extern GUID IID_ITypeLibID;


IUnknown* GetMeasManualGroup();

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEASMANUAL_H__AEC5B060_AE07_4B73_8834_6497E606317F__INCLUDED_)
