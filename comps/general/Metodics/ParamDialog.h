#pragma once


// CAddPaneParamDlg dialog
namespace MetodicsTabSpace
{

class CAddPaneParamDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddPaneParamDlg)

public:
	enum { IDD = IDD_DLG_ADD_PANE };

	CAddPaneParamDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddPaneParamDlg();

// Dialog Data
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	CString m_strName;
};

}