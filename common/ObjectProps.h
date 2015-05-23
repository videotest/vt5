// ObjectProps.h: interface for the CDataObjectPropsImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTPROPS_H__09DAC561_59E8_11D3_8816_0000B493FF1B__INCLUDED_)
#define AFX_OBJECTPROPS_H__09DAC561_59E8_11D3_8816_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "defs.h"
#include "utils.h"
#include "data5.h"
#include "resource.h"




/////////////////////////////////////////////////////////////////////////////
// CObjectPropPageBase dialog

class CObjectPropPageBase : public CPropertyPage
{
	DECLARE_DYNCREATE(CObjectPropPageBase)

// Construction
public:
	CObjectPropPageBase()
	{}

	CObjectPropPageBase(UINT nIDTemplate, UINT nIDCaption = 0) : CPropertyPage(nIDTemplate, nIDCaption)
	{}

	CObjectPropPageBase(LPCTSTR lpszTemplateName, UINT nIDCaption = 0) : CPropertyPage(lpszTemplateName, nIDCaption)
	{}

	virtual ~CObjectPropPageBase()
	{}


	bool		AttachObject(IUnknown * punk)
	{	
		if (!CheckInterface(punk, IID_INamedDataObject2))
			return false;

		m_sptr = punk;
		return true;
	}

	IUnknown*	GetObject()
	{	return (IUnknown*)m_sptr.GetInterfacePtr();	}


// Implementation
protected:
	sptrINamedDataObject2	m_sptr;

	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CObjectPropPageGeneral dialog

class CObjectPropPageGeneral : public CObjectPropPageBase
{
	DECLARE_DYNCREATE(CObjectPropPageGeneral)

// Construction
public:
	CObjectPropPageGeneral();
	virtual ~CObjectPropPageGeneral();

// Dialog Data
	//{{AFX_DATA(CObjectPropPageGeneral)
	enum { IDD = IDD_PROPPAGE_GENERAL };
	CStatic	m_Icon;
	CString	m_strObjectName;
	CString	m_strObjectType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CObjectPropPageGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool LoadTemplate();

	// Generated message map functions
	//{{AFX_MSG(CObjectPropPageGeneral)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CObjectPropSheet
class CObjectPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CObjectPropSheet)

// Construction
public:
	CObjectPropSheet(IUnknown* punk = NULL, LPCTSTR lpszTitle = NULL);

// Attributes
protected:
	sptrINamedDataObject2	m_sptr;

// Operations
public:
	bool		AttachObject(IUnknown * punk)
	{	
		if (!CheckInterface(punk, IID_INamedDataObject2))
			return false;

		m_sptr = punk;
		return true;
	}

	IUnknown*	GetObject()
	{	return (IUnknown*)m_sptr.GetInterfacePtr();	}

	void RemovePages();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectPropSheet)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CObjectPropSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CObjectPropSheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////////
// impl class for creating property sheet with object properties
class std_dll CDataObjectPropsImpl : public CImplBase
{
protected:
	CDataObjectPropsImpl();
	virtual ~CDataObjectPropsImpl();

	virtual void ConstructPropertySheet(CObjectPropSheet* pSheet);
	
	bool CreateProperties();


protected:
	BEGIN_INTERFACE_PART_EXPORT(Props, INamedDataObjectProps)
		com_call CreatePropSheet();
	END_INTERFACE_PART(Props)

protected:
	CObjectPropSheet* m_pSheet;

};




/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_OBJECTPROPS_H__09DAC561_59E8_11D3_8816_0000B493FF1B__INCLUDED_)
