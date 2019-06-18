// DataView.h : main header file for the DATAVIEW DLL
//

#if !defined(AFX_DATAVIEW_H__D6D1B34D_8660_4199_AFB8_936626521EDF__INCLUDED_)
#define AFX_DATAVIEW_H__D6D1B34D_8660_4199_AFB8_936626521EDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDataViewApp
// See DataView.cpp for the implementation of this class
//

class CDataViewApp : public CWinApp
{
public:
	CDataViewApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataViewApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDataViewApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifdef _DEBUG
#import "\vt5\ifaces\debug\dataview.tlb" exclude ("IUnknown", "GUID", "_GUID", "tagPOINT") no_namespace raw_interfaces_only named_guids 
#else
#import "\vt5\ifaces\release\dataview.tlb" exclude ("IUnknown", "GUID", "_GUID", "tagPOINT") no_namespace raw_interfaces_only named_guids 
#endif



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATAVIEW_H__D6D1B34D_8660_4199_AFB8_936626521EDF__INCLUDED_)
