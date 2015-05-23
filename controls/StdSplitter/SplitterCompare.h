#if !defined(AFX_SPLITTERCOMPARE_H__2A481C8C_A66E_41C3_B56C_1C7BA050D961__INCLUDED_)
#define AFX_SPLITTERCOMPARE_H__2A481C8C_A66E_41C3_B56C_1C7BA050D961__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SplitterCompare.h : header file
//

#define SplitZoneWidth	15


#include "SplitterWindow.h"

/////////////////////////////////////////////////////////////////////////////
// CSplitterCompare command target

class CSplitterCompare : public CSplitterStandard
{
	DECLARE_DYNCREATE(CSplitterCompare)
	GUARD_DECLARE_OLECREATE(CSplitterCompare)
	

	CSplitterCompare();           // protected constructor used by dynamic creation

// Attributes
public:
	XSplitterEntry *GetEntry( int nNo );
	bool			IsSplitted();
	long			GetEntriesCount()	{return GetColsCount()*GetRowsCount();}
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitterCompare)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSplitterCompare();


	virtual void LayoutWindow( XSplitterEntry *pe, CRect rc, HDWP &hdwp );
	virtual CRect CalcWindowRect( XSplitterEntry *pe );
	virtual DragMode HitTest( CPoint pt );
	virtual void AfterRecalcLayout();
	virtual void AfterSplitCol();
	virtual void AfterSplitRow()	{AfterSplitCol();}

	// vanek - 22.09.2003
	virtual	bool IsEnabledMenuButton( );

	// Generated message map functions
	//{{AFX_MSG(CSplitterCompare)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void OnPaint();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
	
	DECLARE_INTERFACE_MAP()

private:
	long		m_nPrevPane0Width;
	bool		m_bDelimiter;
	bool		m_bHorizontal;
	DragMode	m_mode;
	bool		m_bShowSplitButton;

public:
	virtual bool DoSplit( int nRowCount, int nColCount );
	virtual long get_width();
	virtual long get_height();
	virtual long get_row_height(long lRow);
	virtual long get_col_width(long lCol);
	virtual void set_row_height(long lRow, long lHeight);
	virtual void set_col_width(long lCol, long lWidth);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLITTERCOMPARE_H__2A481C8C_A66E_41C3_B56C_1C7BA050D961__INCLUDED_)
