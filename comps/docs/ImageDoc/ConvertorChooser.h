#if !defined(AFX_CONVERTORCHOOSER_H__54E170D6_B2C5_11D3_A554_0000B493A187__INCLUDED_)
#define AFX_CONVERTORCHOOSER_H__54E170D6_B2C5_11D3_A554_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConvertorChooser.h : header file
//
#include "wndbase.h"

class CImagePane : public CStatusPaneBase
{
	BOOL m_bProc;
public:
	CImagePane();

	void SetImage( IUnknown *punkImage, IUnknown *punkView = NULL );
	void SetPosition( const POINT &point );
protected:
	//{{AFX_MSG(CImagePane)
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	IImage2Ptr				 m_ptrImage;
	IColorConvertor3Ptr		 m_ptrCC;
	IHumanColorConvertor2Ptr m_ptrHCC;
	IUnknownPtr              m_punkView;

	CString				m_strCC;
	CString				m_strColor;
	CRect				m_rectImage;
	HICON				m_hIcon;
	CStringArray        m_saPaneNames;	
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONVERTORCHOOSER_H__54E170D6_B2C5_11D3_A554_0000B493A187__INCLUDED_)
