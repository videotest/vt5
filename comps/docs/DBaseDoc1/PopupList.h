#if !defined(AFX_PopupList_H__31D64056_AA1A_434B_8436_AA3DCB0931CB__INCLUDED_)
#define AFX_PopupList_H__31D64056_AA1A_434B_8436_AA3DCB0931CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PopupList.h : header file
//

#include "Popup.h"

/////////////////////////////////////////////////////////////////////////////
// CPopupList window

class CPopupList : public CListBox
{
// Construction
public:
	CPopupList();


public:
    class XPopupCtrlEvent : public IPopupCtrlEvent
    {
		
    public:
        virtual bool OnInit ();
        virtual void OnShow ();
        virtual void OnHide (bool bAbort);
        virtual int OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
        virtual int OnLButtonDown (UINT nFlags, CPoint point);
        virtual int OnLButtonUp (UINT nFlags, CPoint point);
        virtual IPopupCtrlEvent* GetInterfaceOf (HWND hWnd);
		void SetReciever (IPopupCtrlReciever* pReciever);

    } m_xPopupCtrlEvent;
	
	IPopupCtrlReciever* m_pReciever;

protected:
	CFont m_Font;
	
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPopupList)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPopupList();
	CRect GetInitRect( CPoint ptStartPoint, int nRecomendedWidth, int nRecomendedHeight,
						bool bCorrectCtrlPos = true );

	// Generated message map functions
protected:
	//{{AFX_MSG(CPopupList)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PopupList_H__31D64056_AA1A_434B_8436_AA3DCB0931CB__INCLUDED_)
