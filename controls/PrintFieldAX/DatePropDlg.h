#pragma once

#include "PrintFieldAXCtl.h"

// CDatePropDlg dialog

class CDatePropDlg : public CDialog
{
	DECLARE_DYNAMIC(CDatePropDlg)

public:
	CDatePropDlg(CWnd* pParent, print_field_params *pparams);   // standard constructor
	virtual ~CDatePropDlg();

// Dialog Data
	enum { IDD = IDD_DATE_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
protected:
	print_field_params	*m_pparams;
};
