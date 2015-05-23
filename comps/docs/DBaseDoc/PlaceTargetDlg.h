#if !defined(AFX_PLACETARGETDLG_H__5C4B972D_D0A8_4B20_BB69_40942DC79498__INCLUDED_)
#define AFX_PLACETARGETDLG_H__5C4B972D_D0A8_4B20_BB69_40942DC79498__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlaceTargetDlg.h : header file
//

#include "filtercondition.h"
/////////////////////////////////////////////////////////////////////////////
// CPlaceTargetDlg dialog

class CPlaceTargetDlg : public CDialog
{
// Construction
public:
	CPlaceTargetDlg(CWnd* pParent = NULL);   // standard constructor
	~CPlaceTargetDlg();

// Dialog Data
	//{{AFX_DATA(CPlaceTargetDlg)
	enum { IDD = IDD_PLACE_TARGET };
	int		m_nFieldIndex;
	BOOL	m_bNewRecord;
	int		m_nTarget;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlaceTargetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPlaceTargetDlg)
	afx_msg void OnTarget();
	afx_msg void OnPutToDoc();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	IMPLEMENT_HELP( "PlaceTarget" );

	CArray<_CField*, _CField*> m_arrfi;
	IQueryObjectPtr	m_ptrQuery;
	IDBaseStructPtr	m_ptrDBStruct;
	CString			m_strVTObjectType;
	IUnknownPtr     m_punkObject;

	void SetControls();

public:
	CString m_strTable;
	CString m_strField;

	void SetSettings( IUnknown* punkQuery, IUnknown* punkDBStruct, CString strVTObjectType, IUnknown *punkObject = NULL )
	{
		m_ptrQuery			= punkQuery;
		m_ptrDBStruct		= punkDBStruct;		
		m_strVTObjectType	= strVTObjectType;
		m_punkObject        = punkObject;
	}


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLACETARGETDLG_H__5C4B972D_D0A8_4B20_BB69_40942DC79498__INCLUDED_)
