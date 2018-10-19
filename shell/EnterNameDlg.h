#if !defined(AFX_ENTERNAMEDLG_H__CDA64C58_CFB7_45FA_8FAD_92EE7614A5BC__INCLUDED_)
#define AFX_ENTERNAMEDLG_H__CDA64C58_CFB7_45FA_8FAD_92EE7614A5BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EnterNameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnterNameDlg dialog

class CEnterNameDlg : public CDialog
{
// Construction
public:
	CEnterNameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEnterNameDlg)
	enum { IDD = IDD_ENTER_NAME };
	CString	m_strName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnterNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEnterNameDlg)
	virtual void OnOK();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}



// CWaitDlg dialog

class CWaitDlg : public CDialog
{
	DECLARE_DYNAMIC(CWaitDlg)
	UINT_PTR m_timerID;
	CRect m_rectAnime;
	CRect m_drawRect, m_redrawRect;
	HBITMAP m_hBitmap;
	BOOL m_bFile;
	CString m_path;
public:
	CWaitDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWaitDlg();

// Dialog Data
	enum { IDD = IDD_WAITDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	void OnTimer( UINT_PTR );
	void OnPaint();
	void SetPath(const CString& str, BOOL bFile =TRUE){this->m_path = str;m_bFile = bFile;};

protected:
	virtual void OnOK();
};

struct PD
{
	HWND hWnd;
	TCHAR* path;
	BOOL bFile;
};

#endif // !defined(AFX_ENTERNAMEDLG_H__CDA64C58_CFB7_45FA_8FAD_92EE7614A5BC__INCLUDED_)
