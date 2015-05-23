#if !defined(AFX_FILTERPROPPAGE_H__C05FE9C9_C363_4F67_AB06_3E83B423CBEE__INCLUDED_)
#define AFX_FILTERPROPPAGE_H__C05FE9C9_C363_4F67_AB06_3E83B423CBEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilterPropPage.h : header file

#include "DBaseListenerImpl.h"
#include "types.h"
#include "filterbase.h"


class CFilterListBox : public CListBox
{
public:
	CFilterListBox();
	~CFilterListBox();
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterListBox)
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	virtual void MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct );
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CFilterListBox)	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


protected:
	CFont m_fontRegular, m_fontBold;

	void DrawText( CDC* pDC, CString strText, CRect rcText );
	int GetStringWidth( CString strText );
	void GetStringArray(CString strSource, CArray<CString, CString>& arStr );

	bool IsBoldWord( CString strWord );
	int GetWordWidth( CString strWord );

	CArray<CString,CString> m_arReservList;

	int m_nLastActiveSelection;	
	bool IsValidIndex( int nIndex );

public:
	void SetToLastSelection();
	void StoreIndex();

};

//

/////////////////////////////////////////////////////////////////////////////
// CFilterPropPage 
class CFilterPropPage : public CPropertyPageBase
						//public CDBaseListenerImpl
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CFilterPropPage);
	GUARD_DECLARE_OLECREATE(CFilterPropPage);
// Construction
public:
	CFilterPropPage();   // standard constructor
	~CFilterPropPage();

	//virtual void OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var );

// Dialog Data
	//{{AFX_DATA(CFilterPropPage)
	enum { IDD = IDD_PAGE_FILTERS };
	CFilterListBox	m_ctrlContext;
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterPropPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilterPropPage)	
	afx_msg void OnSelchangeCondition();
	afx_msg void OnNew();
	afx_msg void OnEdit();
	afx_msg void OnCopy();
	afx_msg void OnRemove();
	afx_msg void OnLeftBracket();
	afx_msg void OnRightBracket();
	afx_msg void OnResetBrackets();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	DECLARE_INTERFACE_MAP()
	BEGIN_INTERFACE_PART(DBFilterPage, IDBFilterPage)
		com_call BuildAppearance( );
	END_INTERFACE_PART(DBFilterPage);

	IDBaseFilterHolderPtr GetActiveFilterHolder();

	bool BuildAppearance();

	void SetVisibleButton();

//Some helpers	
	bool IsFilterAvailable();
	int GetActiveConditionIndex();


	CString m_strLastActiveString;


};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTERPROPPAGE_H__C05FE9C9_C363_4F67_AB06_3E83B423CBEE__INCLUDED_)
