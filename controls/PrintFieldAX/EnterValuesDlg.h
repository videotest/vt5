#if !defined(AFX_ENTERVALUESDLG_H__F9B1B0F7_1A71_4973_A9EA_0B54082C8648__INCLUDED_)
#define AFX_ENTERVALUESDLG_H__F9B1B0F7_1A71_4973_A9EA_0B54082C8648__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EnterValuesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnterValuesDlg dialog
#include "pfaction.h"
#include "EditEx.h"
#include <afxtempl.h>


class CEnterValuesDlg : public CDialog
{
	CString m_strTitle;
	CList<CEditEx*,CEditEx*>	m_EditExList;

	void	clear_editex_list( )
	{
		CEditEx *peditex = 0;
		for( POSITION pos = m_EditExList.GetHeadPosition( ); pos != NULL; )
		{
			peditex = m_EditExList.GetNext( pos );
			if( peditex )
			  delete peditex; peditex = 0;
		}

		m_EditExList.RemoveAll();
	}
	int m_nIdPrevActive;
// Construction
public:
	void SetTitle( CString strTitle ) { m_strTitle = strTitle; };
	CEnterValuesDlg(request_impl *p, CWnd* pParent = NULL);   // standard constructor
	virtual ~CEnterValuesDlg();
// Dialog Data
	//{{AFX_DATA(CEnterValuesDlg)
	enum { IDD = IDD_ENTER_VALUES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnterValuesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEnterValuesDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	request_impl	*m_pdata;
public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	afx_msg void OnBnClickedComboDefault();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENTERVALUESDLG_H__F9B1B0F7_1A71_4973_A9EA_0B54082C8648__INCLUDED_)
