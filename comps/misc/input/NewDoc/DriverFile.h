// DriverFile.h: interface for the CDriverFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRIVERFILE_H__66D29E5A_FCA2_426C_8764_A5785CAD5B0E__INCLUDED_)
#define AFX_DRIVERFILE_H__66D29E5A_FCA2_426C_8764_A5785CAD5B0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DriverBase.h"
#include "Input.h"

class CDriverFile : public CDriverBase, public CInputPreviewImpl  
{
	DECLARE_DYNCREATE(CDriverFile)
	ENABLE_MULTYINTERFACE()

	CString m_fileName;
	CString m_sDrive;
	CString m_sDir;
	CString m_sFName;
	CString m_sExt;
	int m_nFile;
	void ParseFileName();
	void MakeNextFileName(CString &fileName);
	bool DoPromptAndParse();

	CDriverFile();
// Attributes
public:

// Operations
public:

// Overrides
	virtual void OnInputImage(IUnknown *pUnkTarget, IUnknown *pUnkImg, int nDevice, BOOL bDialog);
	virtual bool OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDriverFile)
	public:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDriverFile();

	// Generated message map functions
	//{{AFX_MSG(CDriverFile)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CDriverFile)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CDriverFile)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

#endif // !defined(AFX_DRIVERFILE_H__66D29E5A_FCA2_426C_8764_A5785CAD5B0E__INCLUDED_)
