//{{AFX_INCLUDES()
#include "palette.h"
#include "acivecolorsview.h"
//}}AFX_INCLUDES
#if !defined(AFX_COLORCHOOSEDLG_H__BE0B02A2_A0AE_11D3_A538_0000B493A187__INCLUDED_)
#define AFX_COLORCHOOSEDLG_H__BE0B02A2_A0AE_11D3_A538_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorChooseDlg.h : header file
//

#include "editorint.h"
//#include "ConvertorChooser.h"


/////////////////////////////////////////////////////////////////////////////
// CColorChooseDlg dialog

class CColorChooseDlg : public CDialog,
						public CDockWindowImpl,
						public CWindowImpl,
						public CRootedObjectImpl,
						public CNamedObjectImpl,
						public CHelpInfoImpl
{
	ENABLE_MULTYINTERFACE();
// Construction
public:
	CColorChooseDlg(CWnd* pParent = NULL);   // standard constructor
	~CColorChooseDlg();

	virtual CWnd *GetWindow();
	virtual bool DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID );

	void UpdateSheet();

// Dialog Data
	//{{AFX_DATA(CColorChooseDlg)
	enum { IDD = IDD_DIALOG1 };
	CStatic	m_stConvert;
	CPaletteCtrl	m_ctrlPalette;
	CAciveColorsView	m_ctrlColorsView;
	//}}AFX_DATA


	//CConvertorChooser*	m_pwndConvert;
	//CConvertPane*	m_pConvertPane;
	int				m_nPaneNum;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorChooseDlg)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColorChooseDlg)
	afx_msg void OnLeftClickPalettectrl(OLE_COLOR Color);
	afx_msg void OnRightClickPalettectrl(OLE_COLOR Color);
	virtual BOOL OnInitDialog();
	afx_msg void OnClickPushbuttonctrl1();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	afx_msg LRESULT OnGetInterface(WPARAM, LPARAM){return (LRESULT)GetControllingUnknown();}
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CColorChooseDlg)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	GUARD_DECLARE_OLECREATE(CColorChooseDlg)
	DECLARE_DYNCREATE(CColorChooseDlg)

	BEGIN_INTERFACE_PART(Choose, IChooseColor)
		com_call SetLButtonColor(OLE_COLOR color);
		com_call SetRButtonColor(OLE_COLOR color);
	END_INTERFACE_PART(Choose);
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORCHOOSEDLG_H__BE0B02A2_A0AE_11D3_A538_0000B493A187__INCLUDED_)
