#if !defined(AFX_COLORCONVERTORMANAGER_H__6A1FC3CB_6749_11D3_A4EB_0000B493A187__INCLUDED_)
#define AFX_COLORCONVERTORMANAGER_H__6A1FC3CB_6749_11D3_A4EB_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorConvertorManager.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CColorConvertorManager command target

class CColorConvertorManager : public CCmdTargetEx,
							   public CNamedObjectImpl,
							   public CCompManagerImpl,
							   public CRootedObjectImpl
{
	DECLARE_DYNCREATE(CColorConvertorManager)
	ENABLE_MULTYINTERFACE()

	CColorConvertorManager();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorConvertorManager)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CColorConvertorManager();

	// Generated message map functions
	//{{AFX_MSG(CColorConvertorManager)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CColorConvertorManager)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CColorConvertorManager)
	afx_msg short GetCnvCount();
	afx_msg short GetCnvPos(LPCTSTR szName);
	afx_msg BSTR GetCnvName(short nPos);
	afx_msg short GetCnvPanesCount(short nPos);
	afx_msg BSTR GetCnvPaneName(short nPosCnv, short nPosPane);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORCONVERTORMANAGER_H__6A1FC3CB_6749_11D3_A4EB_0000B493A187__INCLUDED_)
