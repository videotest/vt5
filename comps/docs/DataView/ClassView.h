#if !defined(AFX_CLASSVIEW_H__2BF13CC0_CF97_4FCD_89FC_815D62F950D5__INCLUDED_)
#define AFX_CLASSVIEW_H__2BF13CC0_CF97_4FCD_89FC_815D62F950D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClassView.h : header file
//
#include "DataViewDef.h"
#include "VTGridCtrl.h"
#include "GridView.h"

#include "Classes5.h"
#include "menuconst.h"
/////////////////////////////////////////////////////////////////////////////
// CClassView command target


class CColorCell : public CVTGridCell
{
    DECLARE_DYNCREATE(CColorCell)
public:
	CColorCell() {	m_nRow = -1; m_nCol = -1; m_eCellType = egctClassColor; };
	virtual ~CColorCell() {};

public:
    virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
    virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
    virtual void EndEdit();
	virtual EGridCellType GetType()
	{	return egctClassColor;	}
	virtual void SetType(EGridCellType eCellType)
	{	ASSERT(eCellType == egctClassColor);	}

protected:
	int m_nRow;
	int m_nCol;
    static HCURSOR s_hColorCursor;

    virtual void OnClick();
	virtual BOOL OnSetCursor();

	HCURSOR GetCursor();

 
};

class CClassView : public CGridViewBase
{
	DECLARE_DYNCREATE(CClassView)
	GUARD_DECLARE_OLECREATE(CClassView)
	DECLARE_INTERFACE_MAP()
	ENABLE_MULTYINTERFACE();
	PROVIDE_GUID_INFO( )

public:
	CClassView();           // protected constructor used by dynamic creation
	virtual ~CClassView();

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClassView)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

protected:
	BEGIN_INTERFACE_PART(ClassView, IClassView)
	END_INTERFACE_PART(ClassView)

	virtual bool DefaultInit();
	virtual LPCTSTR	GetObjectType();
// Implementation
protected:
	CCellID		m_SelectedColorCell;
	COLORREF	m_cOldColor;
	friend class CColorCell;


	// Generated message map functions
	//{{AFX_MSG(CClassView)
	//}}AFX_MSG
    afx_msg LRESULT OnSelEndOK(WPARAM lParam, LPARAM wParam);
    afx_msg LRESULT OnSelEndCancel(WPARAM lParam, LPARAM wParam);
    afx_msg LRESULT OnSelChange(WPARAM lParam, LPARAM wParam);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

	virtual POSITION GetFisrtVisibleObjectPosition();
	virtual IUnknown * GetNextVisibleObject( POSITION &rPos );
	virtual const char *GetMenuName()	{return szStatisticMenuUN;};
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLASSVIEW_H__2BF13CC0_CF97_4FCD_89FC_815D62F950D5__INCLUDED_)
