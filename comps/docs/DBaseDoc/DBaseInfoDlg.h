#if !defined(AFX_DBASEINFODLG_H__93CB7F2B_7F28_4700_A538_FF9E40FBC36D__INCLUDED_)
#define AFX_DBASEINFODLG_H__93CB7F2B_7F28_4700_A538_FF9E40FBC36D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DBaseInfoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDBaseInfoDlg dialog

class CDBaseInfoDlg : public CDialog
{
// Construction
public:
	CDBaseInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDBaseInfoDlg)
	enum { IDD = IDD_DBASE_INFO };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBaseInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDBaseInfoDlg)
	virtual BOOL OnInitDialog();	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	IMPLEMENT_HELP( "DBaseInfo" );

protected:
	sptrIDBaseDocument m_spDBaseDoc;
public:
	void SetDBaseDocument( IUnknown* pUnkDoc );
	void OnUserInfo();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBASEINFODLG_H__93CB7F2B_7F28_4700_A538_FF9E40FBC36D__INCLUDED_)
