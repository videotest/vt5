#pragma once


/////////////////////////////////////////////////////////////////////////////
// CFloodFillPropPage dialog
#include "CommonPropPage.h"
#include "ColorPickerPropPage.h"
#include "vtcolorpicker.h"
class CFloodFillPropPage : public CEditorPropBase
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CFloodFillPropPage);
	GUARD_DECLARE_OLECREATE(CFloodFillPropPage);
	DECLARE_INTERFACE_MAP();
	ENABLE_MULTYINTERFACE();

	void InitColors();
// Construction
public:
	CFloodFillPropPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLine1PropPage)
	enum { IDD = IDD_PROPPAGE_FLOODFILL };
	CVTColorPicker m_clrpickFore;
	CVTColorPicker m_clrpickBack;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFloodFillPropPage)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	virtual void DoLoadSettings();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFloodFillPropPage)
		// NOTE: the ClassWizard will add member functions here
		afx_msg void OnOnChangeVtcolorpickerctrl1();
		afx_msg void OnOnChangeVtcolorpickerctrl2();
		afx_msg void OnDestroy();
		DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BEGIN_INTERFACE_PART(FloodFillPropPage, IColorPickerPropPage)
		com_call ChangeColor( DWORD dwRGB, color H, color S, color L );
	END_INTERFACE_PART(FloodFillPropPage);

	CColorWnd m_wndColor;
};
