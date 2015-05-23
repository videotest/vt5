#pragma once

//#include <comutil.h>
#include "_list_idx_t.h"
#include "Params.h"

#define	sMainSection	"\\measurement\\parameters"


// UserParamsComboBoxCtrl.h : Declaration of the CUserParamsComboBoxCtrl ActiveX Control class.


// CUserParamsComboBoxCtrl : See UserParamsComboBoxCtrl.cpp for implementation.

class CUserParamsComboBoxCtrl : public COleControl
{
	DECLARE_DYNCREATE(CUserParamsComboBoxCtrl)

	CMap<long, long&, CString, CString&> m_mapKeyToSection;
	_list_idx_t<long> m_KeyList;
// Constructor
public:
	CUserParamsComboBoxCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

// Implementation
protected:
	~CUserParamsComboBoxCtrl();

	DECLARE_OLECREATE_EX(CUserParamsComboBoxCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CUserParamsComboBoxCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CUserParamsComboBoxCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CUserParamsComboBoxCtrl)		// Type name and misc status

	// Subclassed control support
	BOOL IsSubclassedControl();
	LRESULT OnOcmCommand(WPARAM wParam, LPARAM lParam);

// Message maps
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
		dispidSetSortUserStrings = 9L,		dispidWorksMode = 8,		eventidOnActiveItemChange = 1L,		dispidRefresh = 7L,		dispidActiveItem = 6,		dispidGetParamSection = 5L,		dispidGetItemCount = 4L,		dispidShowUserName = 3,		dispidGetItemData = 2L,		dispidSetItemData = 1L
	};

private:
	void	init_default( );    	
	void	load_params_section( );

	void	create_bk_brush( );
	HBRUSH	get_bk_brush( );
		

	VARIANT_BOOL	m_vbShowUserName;
	long			m_lActiveItem;
	long			m_lWorksMode;
	
	CBrush		m_brushBackground;
	CStringArray m_tStrings;

	BOOL	m_userSort;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg HBRUSH CtlColor ( CDC* pDC, UINT nCtlColor );
	afx_msg HBRUSH OnCtlColor ( CDC* pDC, CWnd *pwnd, UINT nCtlColor );

protected:
	VARIANT_BOOL SetItemData(LONG nIdx, LONG lData);
	LONG GetItemData(LONG nIdx);
	VARIANT_BOOL GetShowUserName(void);
	void SetShowUserName(VARIANT_BOOL newVal);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnFontChanged();
	virtual void OnBackColorChanged();
protected:
	LONG GetItemCount(void);
	BSTR GetParamSection(LONG nIdx);
	LONG GetActiveItem(void);
	void SetActiveItem(LONG newVal);
	void Refresh(void);
	LONG GetWorksMode(void);
	void SetWorksMode(LONG newVal);

	void SetSortUserStrings(BOOL sStr);

	void OnActiveItemChange(LONG nIdx)
	{
		FireEvent(eventidOnActiveItemChange, EVENT_PARAM(VTS_I4), nIdx);
	}
public:
	virtual void Serialize(CArchive& ar);
};