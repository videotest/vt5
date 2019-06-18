#if !defined(AFX_FORMMANAGER_H__02CA58D1_36F2_41CC_B3CF_9789633E7F68__INCLUDED_)
#define AFX_FORMMANAGER_H__02CA58D1_36F2_41CC_B3CF_9789633E7F68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormManager.h : header file
//
class CFormPage;
class CWizButton;
#include "OptionsDlg.h"

class CWizardDlg;

struct WizardNode
{
	WizardNode(CString strFN, bool bMM, IUnknown* punk)
	{
		strFormName = strFN;
		bModalMode = bMM;
		sptrForm = punk;
		if(sptrForm != 0)
			sptrForm->GetSize(&sizeForm);
		pNestedWizard = 0;
		pNextPage = 0;
		pPrevPage = 0;
		pPage = 0;
		nLevel = -1;
	};
	long nLevel;
	CString strFormName;
	IActiveXFormPtr sptrForm;
	CFormPage* pPage;
	CSize sizeForm;
	bool bModalMode;
	WizardNode* pNestedWizard;
	WizardNode* pNextPage;
	WizardNode* pPrevPage;
};

struct DeleteNode
{
	DeleteNode(WizardNode* pN)
	{
		pNode = pN;
		pNext = 0;
	};
	WizardNode* pNode;
	DeleteNode* pNext;
};

class CWizard
{
public:
	CWizard( CString strName, CFormManager *pFormMan );
	~CWizard();

	void AddWizardPage(CString strFormName, bool bModal);
	void ExecuteWizard();
	bool StoreWizard(CString strFileName, CString strSectionName);
	bool LoadWizard(CString strFileName, CString strSectionName);
	void ExecuteNestedWizard();

	CFormManager	*GetFormManager()	{return m_pFormMan;}
	CFormPage		*GetActivePage()	{if( !m_pCurrentPage )return 0; return m_pCurrentPage->pPage;}

	void Next();
	void Prev();
	void Cancel();

protected:
	CSize _GetNextPagesSize(bool bIncludeItself = false);
	void _SetCurModalessPage();
	void _KillWizard();
	void _EmptyDeleteList();
	void _AddToDeleteList(WizardNode* pNode);
	void _LocateButtons(bool bModal);
	void _EnterModalMode(CSize sizeOld);
	void _ExitModalMode();
	void _ExitSettingsMode();
	void _ManageMode(bool bWasInModalMode, bool bNested = false);

	void ExitFromWizard(UINT nID);

	
public:
	CWizButton*		m_pBtnNext;
	CWizButton*		m_pBtnCancel;
	CWizButton*		m_pBtnPrev;
	CWizButton*		m_pBtnHelp;

	bool IsNextPresent();
	bool IsPrevPresent();

private:
	CString			m_strName;

	CTypedPtrMap<CMapStringToPtr, CString, WizardNode*> m_mapWizardNodes;

	CArray<CSize, CSize&>	m_arrWizardSizes;
	
	HWND			m_hwndSettings;
	HWND			m_hwndModal;
	bool			m_bNowExecute;

	WizardNode*		m_pWizardPages;
	WizardNode*		m_pCurrentPage;

	WizardNode*		m_pDeleteList;

	CSize			m_sizeLastSiteSize;
	CWizardDlg*		m_pWizardDlg;
	
	long			m_yOffset;
	CFormManager	*m_pFormMan;
};


/////////////////////////////////////////////////////////////////////////////
// CFormManager command target

class CFormManager : public CCmdTargetEx,
					public CNamedObjectImpl
{
	DECLARE_DYNCREATE(CFormManager)
	ENABLE_MULTYINTERFACE();

	CFormManager();           // protected constructor used by dynamic creation

	static IUnknown	*_GetFormByName( const char *szName );
	bool m_bDisableBtnMode;
// Attributes
public:
// Operations
public:
	
	void OnPageOk();
	void OnPageCancel();
	void OnPageApply();
	void OnPageHelp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormManager)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFormManager();

	// Generated message map functions
	//{{AFX_MSG(CFormManager)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CFormManager)
public:
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CFormManager)
	afx_msg BOOL GetModalMode();
	afx_msg void SetModalMode(BOOL bNewValue);
	afx_msg long ExecutePage(LPCTSTR szFormName);
	afx_msg long ExecuteModal(LPCTSTR szFormName);
	afx_msg long ExecuteDockBar(LPCTSTR szFormName);
	afx_msg long ExecuteDockBar2(LPCTSTR szFormName, LONG lDockSite, BOOL bCaption);
	afx_msg void Finalize(LPCTSTR nPageID);
	afx_msg long ExecuteOptions();
	afx_msg void ResetOptions();
	afx_msg void LoadCategoryImage(LPCTSTR szFileName);
	afx_msg void AddOptionsCategory(short nImageIdx, LPCTSTR szTitle);
	afx_msg void SetActiveCategory(LPCTSTR szName);
	afx_msg BSTR GetActiveCategory();
	afx_msg void AddOptionsPage(LPCTSTR szCategory, LPCTSTR szFormName);
	afx_msg BOOL SetActivePage(LPCTSTR szFormName);
	afx_msg BSTR GetActivePage();
	afx_msg BOOL StoreOptionsConfig(LPCTSTR szFileName);
	afx_msg BOOL LoadOptionsConfig(LPCTSTR szFileName);
	afx_msg void CreateNewWizard(LPCTSTR szWizardName);
	afx_msg void ExecuteWizard(LPCTSTR szWizardName);
	afx_msg BOOL StackCurrentPage();
	afx_msg void ExecutePage2(LPCTSTR bstrName, BOOL EnableOK, BOOL EnableCancel, BOOL EnableApply);
	afx_msg void AddWizardPage(LPCTSTR szFormName, BOOL bModalMode);
	afx_msg void ExecuteNestedWizard();
	afx_msg void EnableNext(BOOL bEnable);
	afx_msg void EnablePrev(BOOL bEnable);
	afx_msg void EnableOk(BOOL bEnable);
	afx_msg void EnableApply(BOOL bEnable);
	afx_msg void PressNext();
	afx_msg void PressPrev();
	afx_msg void PressOK();
	afx_msg void PressCancel();
	afx_msg void ExecutePageModal(LPCTSTR szFormName);
	afx_msg long CreateHideDockBar(LPCTSTR szFormName);
	afx_msg void PressHelp();
	afx_msg BOOL GetPageActive();
	afx_msg void SetPageActive(BOOL bNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	afx_msg void FinalizeEx(LPCTSTR nPageID, bool bOk);


	void _KillButtons();
	void _ProcessStackPages();

protected:
	
	CFormPage*		m_pFormPage;
	COptionsDlg*	m_dlgOptions;
	CWizard*		m_pWizard;
	
	CWizButton*		m_pBtnOk;
	CWizButton*		m_pBtnCancel;
	CWizButton*		m_pBtnApply;
	CWizButton*		m_pBtnHelp;

	CDialog*		m_pFormDialog;

	CStringArray	m_strStackPages;

	bool			m_bModalMode;
public:
	static CFormManager*	m_pFormManager;
	bool	IsPageRunning();
	bool	SetDisableButtonsMode(bool bMode);
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMMANAGER_H__02CA58D1_36F2_41CC_B3CF_9789633E7F68__INCLUDED_)
