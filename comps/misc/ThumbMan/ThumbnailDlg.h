#if !defined(AFX_THUMBNAILDLG_H__ECF75765_CB45_473A_BBE3_83CB82D9A1C7__INCLUDED_)
#define AFX_THUMBNAILDLG_H__ECF75765_CB45_473A_BBE3_83CB82D9A1C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ThumbnailDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CThumbnailDlg dialog

class CThumbnailDlg : public CDialog
{
// Construction
public:
	CThumbnailDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CThumbnailDlg)
	enum { IDD = IDD_THUMBNAIL_DLG };
	CComboBox	m_ctrlObjectName;
	DWORD	m_nFlags;
	int		m_nHeight;
	UINT	m_lParam;
	int		m_nWidth;
	int		m_nX;
	int		m_nY;
	int		m_nGenerateWidth;
	int		m_nGenerateHeight;
	CString	m_strObjectName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThumbnailDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetNamedData( IUnknown* punkND );

protected:
	sptrINamedData m_spNamedData;

	// Generated message map functions
	//{{AFX_MSG(CThumbnailDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THUMBNAILDLG_H__ECF75765_CB45_473A_BBE3_83CB82D9A1C7__INCLUDED_)
