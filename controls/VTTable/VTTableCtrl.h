#pragma once

// VTTableCtrl.h : Declaration of the CVTTableCtrl ActiveX Control class.

#include "RtfTableBase.h"
#include "\vt5\common2\misc_calibr.h"
#include "ax_ctrl_misc.h"

// GDI objects used for drawing operations in module
static CFont   _font;

// CVTTableCtrl : See VTTableCtrl.cpp for implementation.

class CVTTableCtrl : public COleControl
{
	DECLARE_DYNCREATE(CVTTableCtrl)
	
	// vanek - 27.08.2003
    DECLARE_AX_DATA_SITE(CVTTableCtrl)

// Constructor
public:
	CVTTableCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

// Implementation
protected:
	~CVTTableCtrl();

	DECLARE_OLECREATE_EX(CVTTableCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTTableCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTTableCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTTableCtrl)		// Type name and misc status

// Message maps
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

// Event maps
	DECLARE_EVENT_MAP()

	DECLARE_INTERFACE_MAP();
// Dispatch and event IDs
public:
	enum {
		dispidRootSection = 3,		dispidCopyToClipboard = 2L,		dispidPasteFromClipboard = 1L
	};

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
	CRtfTableBase	m_ctrlRichEdit;
	RECT			m_RequestRect;

	CString			m_sTemplateInRtf;
	CString			m_sRootSection;


	BOOL	_update_content( );
	BOOL	_draw_content( CDC* pdc, CRect rect );
 	BOOL	_find_and_replace_values( );
	void	_find_tokenes( long *plBeginTokenIdx, long *plEndTokenIdx );

public:
	virtual void Serialize(CArchive& ar);
protected:
	void	PasteFromClipboard(void);
	void	CopyToClipboard(void);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	BSTR	GetRootSection(void);
	void	SetRootSection(LPCTSTR newVal);
};

