#if !defined(AFX_PRINTDLG_H__77F641C6_EAF9_455E_BA9D_F87A5FC7F9FA__INCLUDED_)
#define AFX_PRINTDLG_H__77F641C6_EAF9_455E_BA9D_F87A5FC7F9FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrintDlg window

class CPrintDlg : public CPrintDialog
{
	DECLARE_DYNAMIC(CPrintDlg)		

public:
	CPrintDlg(BOOL bPrintSetupOnly,		
		DWORD dwFlagsdwFlags = PD_ALLPAGES | PD_USEDEVMODECOPIES 
		| PD_NOPAGENUMS | PD_HIDEPRINTTOFILE 
		| PD_NOSELECTION , 
		CWnd* pParentWnd = NULL 
		);

// Dialog Data
	//{{AFX_DATA(CPrintDlg)
	enum { IDD = 1538 };
	int		m_bUseBaseTemplate;
	int		m_nSelection;
	BOOL	m_bStore;
	int		m_nStoreTarget;
	int		m_nBasedOnTemplate;
	int		m_nExistTemplate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:
	//{{AFX_MSG(CPrintDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTDLG_H__77F641C6_EAF9_455E_BA9D_F87A5FC7F9FA__INCLUDED_)
