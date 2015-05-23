#pragma once


// CUserInfoDlg dialog

class CUserInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CUserInfoDlg)

public:
	CUserInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUserInfoDlg();
	virtual BOOL OnInitDialog();


// Dialog Data
	enum { IDD = IDD_USER_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
#pragma once


// CDDLErrorDlg dialog

class CDDLErrorDlg : public CDialog
{
	DECLARE_DYNAMIC(CDDLErrorDlg)

public:
	CDDLErrorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDDLErrorDlg();

// Dialog Data
	enum { IDD = IDD_DDL_ERROR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void OnUserInfo();
};
