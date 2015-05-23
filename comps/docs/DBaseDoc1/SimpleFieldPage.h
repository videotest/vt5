#if !defined(AFX_SIMPLEFIELDPAGE_H__1ED04FF0_BF6C_45CD_AA2E_3309E7B7D002__INCLUDED_)
#define AFX_SIMPLEFIELDPAGE_H__1ED04FF0_BF6C_45CD_AA2E_3309E7B7D002__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SimpleFieldPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSimpleFieldPage dialog
#include "dbase.h"

struct field_create_info
{
	char	sz_fldbase[100];	//base name of field
	FieldType	type;				//type of field
	char	sz_caption[100];	//caption of control	
	int		count;				//count of field top create
	char	sz_objtype[100];	//type of vt object

	field_create_info	*next;
};

class CSimpleFieldPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSimpleFieldPage)

//	int m_nExpandY;
// Construction
public:
	CSimpleFieldPage();
	~CSimpleFieldPage();

// Dialog Data
	//{{AFX_DATA(CSimpleFieldPage)
	enum { IDD = IDD_NEW_TABLE_WIZARD_DEF3 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	virtual BOOL OnSetActive(); 
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSimpleFieldPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSimpleFieldPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	field_create_info	*m_pcreate;
	int					m_count;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLEFIELDPAGE_H__1ED04FF0_BF6C_45CD_AA2E_3309E7B7D002__INCLUDED_)
