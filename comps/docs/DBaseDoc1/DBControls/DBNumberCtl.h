#if !defined(AFX_DBNUMBERCTL_H__4660FCF7_B9F5_49D6_8AA4_C265C3B5EEC8__INCLUDED_)
#define AFX_DBNUMBERCTL_H__4660FCF7_B9F5_49D6_8AA4_C265C3B5EEC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// DBNumberCtl.h : Declaration of the CDBNumberCtrl ActiveX Control class.

#include "DBControl.h"
#include "PickerCtrl.h"
#include "EditCtrl.h"
#include "FilterCombo.h"
#include "\vt5\common\ax_ctrl_misc.h"

/////////////////////////////////////////////////////////////////////////////
// CDBNumberCtrl : See DBNumberCtl.cpp for implementation.

class CDBNumberCtrl : public COleControl, public CDBControlImpl
{
	DECLARE_AX_DATA_SITE(CDBNumberCtrl)

	DECLARE_DYNCREATE(CDBNumberCtrl)
	ENABLE_MULTYINTERFACE()	
	DECLARE_INTERFACE_MAP()

	FONTDESC	m_fontDesc;
	CFontHolder m_fontHolder;

// Constructor
public:
	CDBNumberCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBNumberCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CDBNumberCtrl();

	DECLARE_OLECREATE_EX(CDBNumberCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CDBNumberCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CDBNumberCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CDBNumberCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CDBNumberCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CDBNumberCtrl)
	afx_msg BSTR GetTableName();
	afx_msg void SetTableName(LPCTSTR lpszNewValue);
	afx_msg BSTR GetFieldName();
	afx_msg void SetFieldName(LPCTSTR lpszNewValue);
	afx_msg BOOL GetEnableEnumeration();
	afx_msg void SetEnableEnumeration(BOOL bNewValue);
	afx_msg BOOL GetAutoLabel();
	afx_msg void SetAutoLabel(BOOL bNewValue);
	afx_msg short GetLabelPosition();
	afx_msg void SetLabelPosition(short nNewValue);
	afx_msg LPFONTDISP GetTextFont();
	afx_msg void SetTextFont(LPFONTDISP newValue);
	afx_msg BOOL GetReadOnly();
	afx_msg void SetReadOnly(BOOL bNewValue);
	afx_msg BOOL GetPropertyAXStatus();
	afx_msg void SetPropertyAXStatus(BOOL bNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CDBNumberCtrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CDBNumberCtrl)
	dispidTableName = 1L,
	dispidFieldName = 2L,
	dispidEnableEnumeration = 3L,
	dispidAutoLabel = 4L,
	dispidLabelPosition = 5L,
	dispidTextFont = 6L,
	dispidReadOnly = 7L,
	dispidGetPropertyAXStatus = 8L,
	dispidSetPropertyAXStatus = 9L,
	//}}AFX_DISP_ID
	};


	BEGIN_INTERFACE_PART(PrintCtrl, IVTPrintCtrl)				
		com_call Draw( HDC hDC, SIZE sizeVTPixel );
		com_call FlipHorizontal(  );
		com_call FlipVertical(  );
		com_call SetPageInfo( int nCurPage, int nPageCount );
		com_call NeedUpdate( BOOL* pbUpdate );
	END_INTERFACE_PART(PrintCtrl)

	void DrawContext( HDC hDC, SIZE size );

protected:

	CEditCtrl m_ctrlEdit;
	CPickerCtrl m_ctrlPicker;

	void Resize();

	void OnPropertyChange();
	virtual void Serialize(CArchive& ar);

	CString GetValue(sptrIDBaseDocument spDBaseDoc, bool bSetToEditCtrl = true );

	//follow members in CDBControlImpl
	//
	//	CString m_strTableName;
	//	CString m_strFieldName;
	//	bool m_bEnableEnumeration;
	//	bool m_bAutoLabel;
	//
	//	CDBControlImpl implementation

	virtual void _RepaintCtrl(){ if( GetSafeHwnd() ) { Invalidate(); } };
	
	//Notification
	virtual void IDBControl_OnSetTableName(  );
	virtual void IDBControl_OnSetFieldName(  );
	virtual void IDBControl_OnSetEnableEnumeration( );
	virtual void IDBControl_OnSetAutoLabel( );
	virtual void IDBControl2_OnSetReadOnly( );

	virtual void IDBControl_GetValue( tagVARIANT *pvar );
	virtual void IDBControl_SetValue( const tagVARIANT var );

	virtual void IDBControl_BuildAppearanceAfterLoad( );
	virtual void IDBControl_OnSetLabelPosition( );

	virtual void IDBControl_ChangeValue( const tagVARIANT var );

	virtual void OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var );	

	virtual void SetEnableWindow( bool bEnable );
	virtual void EnterViewMode( BlankViewMode newBlankMode );

	void AnalizeInputCtrlAppearance();

	//added by akm 13_12_k5
	BOOL		m_PropertyAXStatus;

public:
	virtual BOOL OnSetObjectRects(LPCRECT lpRectPos, LPCRECT lpRectClip);

	void RefreshAppearance();

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBNUMBERCTL_H__4660FCF7_B9F5_49D6_8AA4_C265C3B5EEC8__INCLUDED)
