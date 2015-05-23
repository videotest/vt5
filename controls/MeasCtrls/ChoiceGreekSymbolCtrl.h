#pragma once
#include "misc_list.h"

// ChoiceGreekSymbolCtrl.h : Declaration of the CChoiceGreekSymbolCtrl ActiveX Control class.


// CChoiceGreekSymbolCtrl : See ChoiceGreekSymbolCtrl.cpp for implementation.

class CChoiceGreekSymbolCtrl : public COleControl
{
	DECLARE_DYNCREATE(CChoiceGreekSymbolCtrl)

// Constructor
public:
	CChoiceGreekSymbolCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

	//afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM);
	//afx_msg void MeasureItem ( LPMEASUREITEMSTRUCT lpMeasureItemStruct );


// Implementation
protected:
	~CChoiceGreekSymbolCtrl();

	DECLARE_OLECREATE_EX(CChoiceGreekSymbolCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CChoiceGreekSymbolCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CChoiceGreekSymbolCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CChoiceGreekSymbolCtrl)		// Type name and misc status

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
		eventidOnActiveItemChange = 1L,		dispidGetChar = 5L,		dispidGetItemCount = 4L,		dispidActiveItem = 3,		dispidLastChar = 2,		dispidFirstChar = 1
	};
protected:
	LONG GetFirstChar(void);
	void SetFirstChar(LONG newVal);
	LONG GetLastChar(void);
	void SetLastChar(LONG newVal);
	LONG GetActiveItem(void);
	void SetActiveItem(LONG newVal);
	LONG GetItemCount(void);
	LONG GetChar(LONG nIdx);

	void OnActiveItemChange(LONG nIdx) {	FireEvent(eventidOnActiveItemChange, EVENT_PARAM(VTS_I4), nIdx); }
public:
	virtual void OnFontChanged();
	virtual void OnBackColorChanged();
    virtual void Serialize(CArchive& ar);

private:
	long	m_lFirstChar;
	long	m_lLastChar;
	long	m_lActiveItem;
	
	void	refresh_content( );
	void	init_default( );

	// [vanek] SBT:258 проверка наличия символов в шрифте - 09.06.2004
	void	create_smb_list( );
	_list_t<long>	m_symbols;

	CBrush	m_brushBackground;
	void	create_bk_brush( );
	HBRUSH	get_bk_brush( );

public:
	afx_msg int	OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor ( CDC* pDC, UINT nCtlColor );
	afx_msg HBRUSH OnCtlColor ( CDC* pDC, CWnd *pwnd, UINT nCtlColor );
};

