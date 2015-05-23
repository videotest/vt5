#if !defined(AFX_PROPPAGEDLG_H__F95A9871_5B1B_4481_BA8B_3DA0F0C50E6F__INCLUDED_)
#define AFX_PROPPAGEDLG_H__F95A9871_5B1B_4481_BA8B_3DA0F0C50E6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropPageDlg.h : header file
//

class CPropertyPageEx_ : public CPropertyPage
{
public:
	CPropertyPageEx_(UINT ui);

	virtual void	OnInit();
	virtual bool	OnPresedOK();
	virtual bool	OnPresedCancel();

	virtual int		ShowModal();

	virtual UINT	GetPageID() = 0;
};

/////////////////////////////////////////////////////////////////////////////
// CPropPageDlg dialog

class CPropPageDlg : public CDialog
{

protected:
	CPropertyPageEx_*	m_pChild;
public:
	void SetPropPage( CPropertyPageEx_* pChild ){ m_pChild = pChild; }
// Construction
public:
	CPropPageDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPropPageDlg)
	enum { IDD = IDD_PROP_PAGE_EMU };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropPageDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGEDLG_H__F95A9871_5B1B_4481_BA8B_3DA0F0C50E6F__INCLUDED_)
