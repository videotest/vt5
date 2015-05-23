// sewLoad.h : main header file for the SEWLOAD DLL
//

#if !defined(AFX_SEWLOAD_H__A9A99CCF_297E_45BF_AB6F_DB9D443578DA__INCLUDED_)
#define AFX_SEWLOAD_H__A9A99CCF_297E_45BF_AB6F_DB9D443578DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSewLoadApp
// See sewLoad.cpp for the implementation of this class
//

class CSewLoad :  public CWinApp
{
public:
	CSewLoad();
	virtual ~CSewLoad();

	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSewLoad)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSewLoad)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

EXTERN_C __declspec(dllexport) BOOL SewLoadPic(BSTR pathPicName, IUnknown **ppunkPic);
EXTERN_C __declspec(dllexport) BOOL SewSavePic(BSTR pathPicName, IUnknown *ppunkPic, IUnknown* pNamedData);


static IUnknownPtr LoadObject(const BSTR pathname);


static IUnknownPtr LoadObject(const BSTR pathname)
{
	LPCTSTR sPath=(LPCTSTR)pathname;
	IUnknownPtr punk = GetFilterByFile(sPath);
	
	if (punk)
		punk->Release();
	
	if( punk == 0 )
	{
		return false;
	}

}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEWLOAD_H__A9A99CCF_297E_45BF_AB6F_DB9D443578DA__INCLUDED_)
