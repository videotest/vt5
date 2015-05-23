#if !defined(AFX_VTEDITORL_H__3270DDDD_6711_4A95_B0B3_014426C4B163__INCLUDED_)
#define AFX_VTEDITORL_H__3270DDDD_6711_4A95_B0B3_014426C4B163__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTEditorl.h : Declaration of the CVTEditor ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CVTEditor : See VTEditor.cpp for implementation.



#define STL_FOR_CAPTION			0x00000001L
#define STL_FOR_EDIT			0x00000004L


#include "editorcontrols.h"

class CVTEditor : public COleControl
{			    
	DECLARE_DYNCREATE(CVTEditor)

#ifndef FOR_HOME_WORK
	DECLARE_INTERFACE_MAP();
#endif

// Constructor
public:
	CVTEditor();
	virtual DWORD GetControlFlags();
	virtual DWORD GetActivationPolicy();
	virtual BOOL IsSubclassedControl();

	void OnEditCtrlKillFocus( );
	virtual void OnGetControlInfo(LPCONTROLINFO pControlInfo);
	virtual void OnMnemonic(LPMSG pMsg);

	void OnFireDataChange();

	BOOL _GetClientRect( CRect* lpRect );

protected:

#ifndef FOR_HOME_WORK

	BEGIN_INTERFACE_PART(ActiveXCtrl, IVtActiveXCtrl3)
		com_call SetSite( IUnknown *punkVtApp, IUnknown *punkSite );
		com_call GetName( BSTR *pbstrName );	//such as Editor1
		com_call SetDoc( IUnknown *punkDoc );
		com_call SetApp( IUnknown *punkVtApp );
		com_call OnOK( );
		com_call OnApply( );
		com_call OnCancel( );
	END_INTERFACE_PART(ActiveXCtrl)
	
	IUnknownPtr	m_ptrSite;
	IUnknownPtr	m_ptrApp;
#endif
protected:
	
	// 3 Controls in body:
	CStaticCtrl	m_captionCtrl;
	CEditCtrl	m_editCtrl;	
	CSpinCtrl	m_spinCtrl;


	void InitDefaults();

	CString				m_strName;

	CaptionAlignEnum	m_CaptionAlignEnum;
	CString				m_strCaptionText;
	BOOL				m_bCaptionEnable;

	DataTypeEnum		m_DataTypeEnum;	
	
	CString				m_strEditText;
	short				m_nCaptionFieldWidth;
	
	BOOL				m_bSpinEnable;
	BOOL				m_bValidationEnable;
	float				m_fMaxValue;
	float				m_fMinValue;

	OLE_COLOR			m_clEditTextColor;
	OLE_COLOR			m_clEditBackgroundColor;
	CBrush				m_brushEditBackground;

	LOGFONT				m_logfontEdit;

	CFont				m_fontEdit;
	CFontHolder			m_fontHolderEdit;
	

	OLE_COLOR			m_clCaptionTextColor;
	OLE_COLOR			m_clCaptionBackgroundColor;
	CBrush				m_brushCaptionBackground;

	LOGFONT				m_logfontCaption;
	CFont				m_fontCaption;
	CFontHolder			m_fontHolderCaption;

	OLE_COLOR			m_clBackgroundColor;
		
	
	BOOL				m_bWasCreated;

	BOOL				m_bCaptionAutoSize;


	CString				m_strKeyValue;	
	BOOL				m_bAutoUpdate;

	BOOL				m_bMultiLine;
	BOOL				m_bAlignClient;

	BOOL				m_bInnerRaised;
	BOOL				m_bInnerSunken;
	BOOL				m_bOuterRaised;
	BOOL				m_bOuterSunken;

	BOOL				m_bUseSystemFont;
	BOOL                m_bVTStaticStyle;

	
	void InvalidateAndSetModifiedFlag( /*DWORD dwFlags*/ );
	
	//Move inside vindow
	void RecalcLayout( int cx, int cy );

	//Recalc layout and invalidate
	void RepaintControl();	

	//For serialization and first creation set property to contrls and invalidate
	void BuildAppearence();

	
	void CreateBackgroundBrushes( DWORD dwCreateStyle );
	
	void SetFontToControl( DWORD dwCreateStyle );	
	
	char	m_cAccelKey;
	BOOL 	FindAccelKey();

	virtual void Serialize(CArchive& ar);

	void SwitchMultiSingleLine();

	BOOL Validation( bool bUpdateWindowText );


	BOOL WriteToShellData();
	BOOL RestoreFromShellData();

	HACCEL hAccel;

	BOOL m_bCanEnterValidation;

	bool m_bInternalTextUpdate;

	BOOL m_bMinValue;
	BOOL m_bMinMaxVerify;
	BOOL m_bKillFocus;

	HWND  hTip;
	long m_nCharAFP;

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVTEditor)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CVTEditor();

	DECLARE_OLECREATE_EX(CVTEditor)    // Class factory and guid
	DECLARE_OLETYPELIB(CVTEditor)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CVTEditor)     // Property page IDs
	DECLARE_OLECTLTYPE(CVTEditor)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CVTEditor)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CVTEditor)
	afx_msg BSTR GetName();
	afx_msg void SetName(LPCTSTR lpszNewValue);
	afx_msg short GetCaptionPosition();
	afx_msg void SetCaptionPosition(short nNewValue);
	afx_msg short GetDataType();
	afx_msg void SetDataType(short nNewValue);
	afx_msg BSTR GetCaptionText();
	afx_msg void SetCaptionText(LPCTSTR lpszNewValue);
	afx_msg short GetEditFieldWidth();
	afx_msg void SetEditFieldWidth(short nNewValue);
	afx_msg BOOL GetSpinEnable();
	afx_msg void SetSpinEnable(BOOL bNewValue);
	afx_msg float GetMinValue();
	afx_msg void SetMinValue(float newValue);
	afx_msg float GetMaxValue();
	afx_msg void SetMaxValue(float newValue);
	afx_msg BOOL GetValidationEnable();
	afx_msg void SetValidationEnable(BOOL bNewValue);
	afx_msg BOOL GetCaptionEnable();
	afx_msg void SetCaptionEnable(BOOL bNewValue);
	afx_msg OLE_COLOR GetBackgroundColor();
	afx_msg void SetBackgroundColor(OLE_COLOR nNewValue);
	afx_msg OLE_COLOR GetCaptionBackColor();
	afx_msg void SetCaptionBackColor(OLE_COLOR nNewValue);
	afx_msg OLE_COLOR GetCaptionTextColor();
	afx_msg void SetCaptionTextColor(OLE_COLOR nNewValue);
	afx_msg OLE_COLOR GetEditBackColor();
	afx_msg void SetEditBackColor(OLE_COLOR nNewValue);
	afx_msg OLE_COLOR GetEditTextColor();
	afx_msg void SetEditTextColor(OLE_COLOR nNewValue);
	afx_msg LPFONTDISP GetEditFont();
	afx_msg void SetEditFont(LPFONTDISP newValue);
	afx_msg LPFONTDISP GetCaptionFont();
	afx_msg void SetCaptionFont(LPFONTDISP newValue);
	afx_msg BOOL GetCaptionAutoWidth();
	afx_msg void SetCaptionAutoWidth(BOOL bNewValue);
	afx_msg BSTR GetKey();
	afx_msg void SetKey(LPCTSTR lpszNewValue);
	afx_msg BOOL GetAutoUpdate();
	afx_msg void SetAutoUpdate(BOOL bNewValue);
	afx_msg BOOL GetAlignClient();
	afx_msg void SetAlignClient(BOOL bNewValue);
	afx_msg BOOL GetMultiLine();
	afx_msg void SetMultiLine(BOOL bNewValue);
	afx_msg BOOL GetInnerSunken();
	afx_msg void SetInnerSunken(BOOL bNewValue);
	afx_msg BOOL GetOuterRaised();
	afx_msg void SetOuterRaised(BOOL bNewValue);
	afx_msg BOOL GetOuterSunken();
	afx_msg void SetOuterSunken(BOOL bNewValue);
	afx_msg BOOL GetInnerRaised();
	afx_msg void SetInnerRaised(BOOL bNewValue);
	afx_msg BSTR GetText();
	afx_msg void SetText(LPCTSTR lpszNewValue);
	afx_msg long GetValueInt();
	afx_msg void SetValueInt(long nNewValue);
	afx_msg double GetValueDouble();
	afx_msg void SetValueDouble(double newValue);
	afx_msg void StoreToData();
	afx_msg BOOL SetAutoSize();
	afx_msg BOOL GetDisabled();
	afx_msg void SetDisabled(BOOL bNewValue);
	afx_msg BOOL GetVisible();
	afx_msg void SetVisible(BOOL bNewValue);
	afx_msg void SetFocus();
	afx_msg BOOL GetUseSystemFont();
	afx_msg void SetUseSystemFont(BOOL bNewValue);
	afx_msg BOOL GetVT5StaticStyle();
	afx_msg void SetVT5StaticStyle(BOOL bNewValue);
	afx_msg void SetNumberCharAFP(long nCharAFP);
	afx_msg long GetNumberCharAFP();

	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CVTEditor)
	void FireDataChange()
		{FireEvent(eventidDataChange,EVENT_PARAM(VTS_NONE));}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CVTEditor)
	dispidName = 1L,
	dispidCaptionPosition = 2L,
	dispidDataType = 3L,
	dispidCaptionText = 4L,
	dispidCaptionFieldWidth = 5L,
	dispidSpinEnable = 6L,
	dispidMinValue = 7L,
	dispidMaxValue = 8L,
	dispidValidationEnable = 9L,
	dispidCaptionEnable = 10L,
	dispidBackgroundColor = 11L,
	dispidCaptionBackColor = 12L,
	dispidCaptionTextColor = 13L,
	dispidEditBackColor = 14L,
	dispidEditTextColor = 15L,
	dispidEditFont = 16L,
	dispidCaptionFont = 17L,
	dispidCaptionAutoWidth = 18L,
	dispidKey = 19L,
	dispidAutoUpdate = 20L,
	dispidAlignClient = 21L,
	dispidMultiLine = 22L,
	dispidInnerSunken = 23L,
	dispidOuterRaised = 24L,
	dispidOuterSunken = 25L,
	dispidInnerRaised = 26L,
	dispidText = 27L,
	dispidValueInt = 28L,
	dispidValueDouble = 29L,
	dispidStoreToData = 30L,
	dispidSetAutoSize = 31L,
	dispidDisabled = 32L,
	dispidVisible = 33L,
	dispidSetFocus = 34L,
	dispidUseSystemFont = 35L,
	dispidVT5StaticStyle = 36L,
	dispidNumberCharAFP = 37L,
	eventidDataChange = 1L,
	//}}AFX_DISP_ID
	};
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTEDITORL_H__3270DDDD_6711_4A95_B0B3_014426C4B163__INCLUDED)
