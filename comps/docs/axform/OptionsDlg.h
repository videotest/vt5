#if !defined(AFX_OPTIONSDLG_H__5564C1A3_E521_11D3_A0A7_0000B493A187__INCLUDED_)
#define AFX_OPTIONSDLG_H__5564C1A3_E521_11D3_A0A7_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDlg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog
class CGfxOutBarCtrl;
class CPage;
class CFormManager;

class CSheet : public CPropertySheet
{
public:
	virtual BOOL OnInitDialog()   
	{
		ModifyStyleEx (0, WS_EX_CONTROLPARENT);
		return CPropertySheet::OnInitDialog();   
	}
};

#include <vector>
template <class Type> class CAccocTable
{
	std::vector<CString>m_Keys;
	std::vector<Type>m_Items;
public:
	CAccocTable() {};
	~CAccocTable() { };
	void SetAt( CString Key, Type Item );
	void RemoveAll( bool bDelete = true );
	void Erase( CString Key );
	void Erase( Type Page );
	bool Lookup( CString Key, Type &Item );
	int GetSize() { return m_Keys.size(); }
	CString GetKey( int nID ) { if( nID < (int)m_Keys.size() ) return m_Keys[nID]; return ""; }
	Type GetItem( int nID ) { if( nID < (int)m_Keys.size() ) return (Type)m_Items[nID]; return 0; }
};

class COptionsDlg : public CDialog
{
public:
	COptionsDlg();   // standard constructor
	~COptionsDlg();

	void SetParent( HWND hwnd );
	void ResetControls();
	void LoadImageList(CString strFileName);
	void AddItemToBar(int nImageIdx, CString strTitle);
	void SetActiveCategory(CString strName);
	void GetActiveCategory(CString& strName);
	void AddPageToCategory(CString strCategory, CString strForm, const char* psz_form_title = 0 );
	bool SetActivePage(CString strForm);
	void GetActivePage(CString& strForm);
	bool StoreOptionsConfig(CString strFileName);
	bool LoadOptionsConfig(CString strFileName);
// Dialog Data
	//{{AFX_DATA(COptionsDlg)
	enum { IDD = IDD_DIALOG_OPTIONS };
	CStatic	m_stTab;
	CStatic	m_FakeStatic;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsDlg)
	afx_msg void OnApply();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHelp();
	//}}AFX_MSG
	afx_msg LRESULT OnOutbarNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetInterface(WPARAM, LPARAM){return (LRESULT)GetControllingUnknown();}
	afx_msg LRESULT OnHelpHitTest(WPARAM, LPARAM);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);

	DECLARE_MESSAGE_MAP()


	void _KillPages();
	void _SetSheetByCategory(CString strCategory);
	void _RemoveAllFromSheet();
	CRect _CalcSheetRect();
	void _Initial();
	void _OnOK();	
	void _ChangeExt(CString&  strPath, CString strExt);

	 

	CGfxOutBarCtrl* m_pOutBarCtrl;
	CImageList	m_listLarge;
	int			m_nActiveItem;
	CString		m_strActiveCategory;

	int			m_nActivePage;

	CAccocTable<bool *>m_pActivates;
	CAccocTable<CSheet*>m_pSheets;
	CSheet*			m_pSheet;
	CAccocTable <CPropertyPage *>m_mapPages;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDLG_H__5564C1A3_E521_11D3_A0A7_0000B493A187__INCLUDED_)
