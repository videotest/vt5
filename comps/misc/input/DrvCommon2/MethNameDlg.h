#pragma once


// CMethNameDlg dialog

class CMethNameDlg : public CDialog
{
	DECLARE_DYNAMIC(CMethNameDlg)

public:
	static int s_idd;
	static int s_idEdit;

	CString m_DefName;
	CMethNameDlg(LPCTSTR lpstrDefName, CWnd* pParent = NULL);   // standard constructor
	virtual ~CMethNameDlg();

// Dialog Data
//	enum { IDD = IDD_DIALOG_SETTING_NAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_sName;
protected:
	virtual void OnOK();
};
