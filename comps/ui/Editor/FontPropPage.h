//{{AFX_INCLUDES()
#include "pushbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_FONTPROPPAGE_H__45193776_A3DA_11D3_A53C_0000B493A187__INCLUDED_)
#define AFX_FONTPROPPAGE_H__45193776_A3DA_11D3_A53C_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CommentFontPropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCommentPropPage dialog
#include "ColourPicker.h"
#include "FontViewer.h"

class CCommentFontPropPage : public CPropertyPageBase
{
	PROVIDE_GUID_INFO( )
	ENABLE_MULTYINTERFACE();
	DECLARE_DYNCREATE(CCommentFontPropPage);
	GUARD_DECLARE_OLECREATE(CCommentFontPropPage);
// Construction
public:
	CCommentFontPropPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCommentFontPropPage)
	enum { IDD = IDD_PROPPAGE_FONT };
	CFontViewer	m_strFontViewer;
	CButton	m_checkBack;
	CButton	m_checkArrow;
	CComboBox	m_cbFontSize;
	CComboBoxEx	m_cbFont;
	CColourPicker	m_clrBack;
	CColourPicker	m_clrText;
	CPushButton	m_pbAlign1;
	CPushButton	m_pbAlign2;
	CPushButton	m_pbAlign3;
	CPushButton	m_pbAlign4;
	BOOL	m_bArrow;
	BOOL	m_bBack;
	int		m_nFontSize;
	CPushButton	m_pbBold;
	CPushButton	m_pbItalic;
	CPushButton	m_pbUnder;
	CPushButton	m_pbAlignLeft;
	CPushButton	m_pbAlignCenter;
	CPushButton	m_pbAlignRight;
	//}}AFX_DATA

	
	BEGIN_INTERFACE_PART(FontPage, IFontPropPage)
		com_call GetFont(LOGFONT*	pLogFont, int* pnAlign);
		com_call GetColorsAndStuff(OLE_COLOR* pcolorText, OLE_COLOR* pcolorBack, BOOL* pbBack, BOOL* pbArrow);
		com_call SetFont(LOGFONT*	pLogFont, int nAlign);
		com_call SetColorsAndStuff(OLE_COLOR colorText, OLE_COLOR colorBack, BOOL bBack, BOOL bArrow);
	END_INTERFACE_PART(FontPage)
	DECLARE_INTERFACE_MAP();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommentFontPropPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCommentFontPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnCheckArrow();
	afx_msg void OnCheckBack();
	afx_msg void OnSelchangeCbFont();
	afx_msg void OnSelchangeCombo2();
	afx_msg void OnClickPbBold();
	afx_msg void OnClickPbItalic();
	afx_msg void OnClickPbUnder();
	afx_msg void OnClickPbAlignleft();
	afx_msg void OnClickPbAligncenter();
	afx_msg void OnClickPbAlignright();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	afx_msg LRESULT OnSelChange(WPARAM lParam, LPARAM wParam);
	afx_msg LPARAM OnSelEndOK(WPARAM lParam, LPARAM wParam);

	DECLARE_MESSAGE_MAP()


	CImageList	m_imgFont;


public:
	CString		m_strCurFontFamily;
	int			m_nCurHeight;
	BOOL		m_bBold;
	BOOL		m_bItalic;
	BOOL		m_bUnder;
	BOOL		m_bStrikeOut;
	COLORREF	m_color;
	COLORREF	m_backColor;
	int			m_nCurAlign;
	
	CStringArray m_strArray;
	void	_ManageButtonState(int nButton);
	void	_SetFontPreview();

	void	_SetFont(LOGFONT** pplogFont, int nAlign);
	void	_GetFont(LOGFONT** ppLogFont, int** ppnAlign=0);
	void	_InitParams();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTPROPPAGE_H__45193776_A3DA_11D3_A53C_0000B493A187__INCLUDED_)
