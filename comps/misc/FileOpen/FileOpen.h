// FileOpen.h : main header file for the FILEOPEN DLL
//

#if !defined(AFX_FILEOPEN_H__EB1A83B0_70B4_11D3_A4F5_0000B493A187__INCLUDED_)
#define AFX_FILEOPEN_H__EB1A83B0_70B4_11D3_A4F5_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFileOpenApp
// See FileOpen.cpp for the implementation of this class
//

class CFileOpenApp : public CWinApp
{
public:
	CFileOpenApp();
	~CFileOpenApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileOpenApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CFileOpenApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

// {F360E263-7AF1-11d3-A504-0000B493A187}
static const GUID IID_ITypeLibID = 
{ 0xf360e263, 0x7af1, 0x11d3, { 0xa5, 0x4, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
/*

#ifdef _DEBUG
// {F360E261-7AF1-11d3-A504-0000B493A187}
static const GUID IID_ITypeLibID = 
{ 0xf360e261, 0x7af1, 0x11d3, { 0xa5, 0x4, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
#else
// {F360E263-7AF1-11d3-A504-0000B493A187}
static const GUID IID_ITypeLibID = 
{ 0xf360e263, 0x7af1, 0x11d3, { 0xa5, 0x4, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
#endif
*/
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEOPEN_H__EB1A83B0_70B4_11D3_A4F5_0000B493A187__INCLUDED_)
