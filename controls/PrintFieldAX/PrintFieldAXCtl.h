#if !defined(AFX_PRINTFIELDAXCTL_H__C8092783_7D22_4302_92A5_9FD8613610FE__INCLUDED_)
#define AFX_PRINTFIELDAXCTL_H__C8092783_7D22_4302_92A5_9FD8613610FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "\vt5\ifaces\printfield.h"
#include "\vt5\ifaces\axint.h"
#include "\vt5\common\ax_ctrl_misc.h"
// PrintFieldAXCtl.h : Declaration of the CPrintFieldAXCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CPrintFieldAXCtrl : See PrintFieldAXCtl.cpp for implementation.

class CPrintFieldAXCtrl : public COleControl
{
	DECLARE_DYNCREATE(CPrintFieldAXCtrl)
	DECLARE_INTERFACE_MAP();
	DECLARE_AX_DATA_SITE(CPrintFieldAXCtrl)
// Constructor
public:
	CPrintFieldAXCtrl();

	void LoadValue( );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintFieldAXCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CPrintFieldAXCtrl();

	DECLARE_OLECREATE_EX(CPrintFieldAXCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CPrintFieldAXCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CPrintFieldAXCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CPrintFieldAXCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CPrintFieldAXCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CPrintFieldAXCtrl)
	afx_msg BSTR GetGroup();
	afx_msg void SetGroup(LPCTSTR lpszNewValue);
	afx_msg long GetEditControlStyle();
	afx_msg void SetEditControlStyle(long nNewValue);
	afx_msg long GetCaptionWidth();
	afx_msg void SetCaptionWidth(long nNewValue);
	afx_msg BSTR GetCaption();
	afx_msg void SetCaption(LPCTSTR lpszNewValue);
	afx_msg BSTR GetKeyToRead();
	afx_msg void SetKeyToRead(LPCTSTR lpszNewValue);
	afx_msg BOOL GetUseKeyToRead();
	afx_msg void SetUseKeyToRead(BOOL bNewValue);
	afx_msg long GetCaptionAlignment();
	afx_msg void SetCaptionAlignment(long nNewValue);
	afx_msg BOOL GetCaptioAutoWidth();
	afx_msg void SetCaptioAutoWidth(BOOL bNewValue);
	afx_msg OLE_COLOR GetTextColor();
	afx_msg void SetTextColor(OLE_COLOR nNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CPrintFieldAXCtrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

	BEGIN_INTERFACE_PART(ActiveXCtrl, IVtActiveXCtrl)
		com_call SetSite( IUnknown *punkVtApp, IUnknown *punkSite );
		com_call GetName( BSTR *pbstrName );	//such as Editor1
	END_INTERFACE_PART(ActiveXCtrl)
	BEGIN_INTERFACE_PART(PrintField, IPrintField)
		com_call GetParams( print_field_params * pparams );
		com_call SetParams( print_field_params * pparams );
		com_call GetNames( BSTR *pbstrCaption, BSTR *pbstrGroup );
		com_call GetCaptionAlingment( long *plAligment/*PrintFieldAligment enum*/ );
		com_call GetCaptionWidth( long *plWidth/*if aligment == pfa_width*/) ;
	END_INTERFACE_PART(PrintField)
// Dispatch and event IDs
	virtual void Serialize( CArchive &ar );
public:
	enum {
	//{{AFX_DISP_ID(CPrintFieldAXCtrl)
	dispidGroup = 1L,
	dispidEditControlStyle = 2L,
	dispidCaptionWidth = 3L,
	dispidCaptioAutoWidth = 4L,
	dispidCaption = 5L,
	dispidKeyToRead = 6L,
	dispidUseKeyToRead = 7L,
	dispidCaptionAlignment = 8L,
	dispidTextColor = 9L,
	//}}AFX_DISP_ID
	};

	//my data
	IUnknownPtr				m_ptrApp, 
							m_ptrSite;
	print_field_params		m_params;

	_bstr_t					m_caption, m_text, m_group;
	long					m_caption_width;
	CString					m_strKeyToRead;
	BOOL					m_bUseKeyToRead;
	long					m_lAlingment;
	OLE_COLOR				m_clrText;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTFIELDAXCTL_H__C8092783_7D22_4302_92A5_9FD8613610FE__INCLUDED)
