// tetrisDlg.h : header file
//

#if !defined(AFX_TETRISDLG_H__1BE87E88_54EB_430E_B9B0_7BAF71F4A844__INCLUDED_)
#define AFX_TETRISDLG_H__1BE87E88_54EB_430E_B9B0_7BAF71F4A844__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTetrisDlg dialog
#include "tetwindow.h"
#define	SCOPE_COUNT	10


class CTetrisDlg : public CDialog
{
	struct Scope
	{
		char	szName[100];
		int		nLevel;
		int		nLines;
		int		nScope;
	};
// Construction
public:
	CTetrisDlg(CWnd* pParent = NULL);	// standard constructor
	~CTetrisDlg();

// Dialog Data
	//{{AFX_DATA(CTetrisDlg)
	enum { IDD = IDD_TETRIS_DIALOG };
	CListCtrl	m_list;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTetrisDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	void AddHiScope();
	void UpdateScopeTable();
	void LoadScopeTable();
	void StoreScopeTable();
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTetrisDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnNew();
	afx_msg void OnHelp();
	//}}AFX_MSG
	LRESULT OnUpdateScope( WPARAM, LPARAM );
	DECLARE_MESSAGE_MAP()
protected:
	CTetWindow	m_window;
	Scope		m_table[SCOPE_COUNT];
	bool		m_bFirstFillScopes;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TETRISDLG_H__1BE87E88_54EB_430E_B9B0_7BAF71F4A844__INCLUDED_)
