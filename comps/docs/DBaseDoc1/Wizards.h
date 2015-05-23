#ifndef __DBWIZARDS_H__
#define __DBWIZARDS_H__


class CWizardSheet;

bool IsValidWord( CString str );

bool IsValidTableName( CString strTableName, TABLEINFO_ARR& arrTable, CString& strError );

bool GetDBFieldInfo( CWizardSheet* pSheet, TABLEINFO_ARR& arrTableOld, _CTableInfo* ptiNew  ); 


ADOX::_CatalogPtr OpenCatalog( CString strConnectionString, bool bAccessDB, bool bReportError );


class CWizardSheet;


class CWizardsData
{
public:
	//Query
	CString		m_strQueryName;
	CString		m_strBaseTable;
	bool		m_bSimpleQuery;
	CString		m_strManualSQL;
	
	//Thumbnail
	CString		m_strTableThumbnLocate;
	CString		m_strFieldThumbnLocate;
	
	CString		m_strTableThumbnCaption;
	CString		m_strFieldThumbnCaption;

	CSize		m_sizeThumb;
	CSize		m_sizeThumbView;
	CSize		m_sizeThumbBorder;

	TABLEINFO_ARR	m_arrTable;

};


enum WizardMode
{
	wmNotDefine		= 0,
	wmQueryProp		= 1,
	wmNewQuery		= 2,
	wmNewTable		= 3,//advanced
	wmNewDB			= 4,
};

class CEditQueryPage;
class CTumbnailPage;

class CDBInfoCreate;
class CDBCreateProcess;

class CWizardPool
{

public:

	CWizardPool();
	~CWizardPool();

	bool QueryProperties( IUnknown* punkDoc, IUnknown* punkQ, bool bshow_query );
	bool NewQueryWizard( IUnknown* punk );
	bool NewDBWizard( IUnknown* punkDBDoc );
	bool IsAborted();

protected:
	WizardMode		m_wm;

	
	IUnknown*		m_punkActiveDoc;

	bool	NewDB_IsAccessDataBase( CWizardSheet* pSheet );
	bool	NewDB_IsSimpleFieldConstructor( CWizardSheet* pSheet );
	bool	NewDB_NeedCreateNewTable( CWizardSheet* pSheet );

	bool	NewDB_InfoCreateDB( CWizardSheet* pSheet, CDBInfoCreate* pPage );
	bool	NewDB_CreateNewDB( CWizardSheet* pSheet, CDBCreateProcess* pPage );
	void	NewDB_CreateNewDBCleanUp( CWizardSheet* pSheet, CDBCreateProcess* pPage, bool bUserAbort = false );
	CString NewDB_GetAccesFileName( CWizardSheet* pSheet );
	


protected:
	void Update_EditQueryPage( CWizardsData* pData, CEditQueryPage* pPage, bool bFillage );
	void Update_TumbnailPage( CWizardsData* pData, CTumbnailPage* pPage, bool bFillage );

	bool ReadWizardDataFromQuery( IUnknown* punkQ, CWizardsData* pData );
	bool WriteWizardDataToQuery( IUnknown* punkQ, CWizardsData* pData );

public:
	bool _OnWizardSetActivePage( CWizardSheet* pSheet, CPropertyPage* pPage );
	bool _OnWizardBack( CWizardSheet* pSheet, CPropertyPage* pPage );
	bool _OnWizardNext( CWizardSheet* pSheet, CPropertyPage* pPage );
	bool _OnWizardFinish( CWizardSheet* pSheet, CPropertyPage* pPage );

};


#include "HelpDlgImpl.h"

/////////////////////////////////////////////////////////////////////////////
// CWizardSheet

class CWizardSheet : public CPropertySheet,
					public CHelpDlgImpl
{

	DECLARE_DYNAMIC(CWizardSheet)

	CWizardPool*	m_pPool;

// Construction
public:
	CWizardSheet();
	CWizardSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CWizardSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	void SetWizardPool( CWizardPool* pWizardPool );
	bool _OnWizardSetActivePage( CPropertyPage* pPage );	//route 2 wizard pool
	bool _OnWizardBack( CPropertyPage* pPage );				//route 2 wizard pool
	bool _OnWizardNext( CPropertyPage* pPage );				//route 2 wizard pool
	bool _OnWizardFinish( CPropertyPage* pPage );			//route 2 wizard pool

	CPropertyPage* FindPage( CRuntimeClass* prc, int* pnPageNum );
	bool ActivatePage( CRuntimeClass* prc );
	
protected:
	CString m_strHelpFileName;
// Attributes
public:
	void SetHelpFileName( CString strHelpFileName){ m_strHelpFileName = strHelpFileName;}

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWizardSheet)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWizardSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWizardSheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#define PROCESS_SET_ACTIVE_MSG()\
CWnd* pParent = GetParent();\
CPropertyPage::OnSetActive();\
if( pParent && pParent->IsKindOf(RUNTIME_CLASS(CWizardSheet)))\
{\
	CWizardSheet* pSheet = (CWizardSheet*)pParent;\
	return pSheet->_OnWizardSetActivePage( this ) == true;\
}\
\
return TRUE;\


#define PROCESS_WIZARD_BACK_MSG()\
if( !UpdateData(TRUE) ) return -1;\
CWnd* pParent = GetParent();\
if( pParent && pParent->IsKindOf(RUNTIME_CLASS(CWizardSheet)))\
{\
	CWizardSheet* pSheet = (CWizardSheet*)pParent;\
	return ( pSheet->_OnWizardBack( this ) == true ? 0 : -1 );\
}\
\
return CPropertyPage::OnWizardBack();\

#define PROCESS_WIZARD_NEXT_MSG()\
if( !UpdateData(TRUE) ) return -1;\
CWnd* pParent = GetParent();\
if( pParent && pParent->IsKindOf(RUNTIME_CLASS(CWizardSheet)))\
{\
	CWizardSheet* pSheet = (CWizardSheet*)pParent;\
	return ( pSheet->_OnWizardNext( this ) == true ? 0 : -1 );\
}\
\
return CPropertyPage::OnWizardNext();\

#define PROCESS_WIZARD_FINISH_MSG()\
if( !UpdateData(TRUE) ) return false;\
CWnd* pParent = GetParent();\
if( pParent && pParent->IsKindOf(RUNTIME_CLASS(CWizardSheet)))\
{\
	CWizardSheet* pSheet = (CWizardSheet*)pParent;\
	return pSheet->_OnWizardFinish( this ) == true;\
}\
\
return CPropertyPage::OnWizardFinish();\

#define GET_NEW_DB_WIZARD_DATA(FaultResult)\
if( !pSheet )\
	return FaultResult;\
CNewDBTypePage*				pDBTypePage = \
	(CNewDBTypePage*)pSheet->FindPage( RUNTIME_CLASS(CNewDBTypePage), 0 );\
\
CNewDBAccessPage*			pDBAccess = \
	(CNewDBAccessPage*)pSheet->FindPage( RUNTIME_CLASS(CNewDBAccessPage), 0 );\
\
CNewDBAdvPage*				pDBAdv = \
	(CNewDBAdvPage*)pSheet->FindPage( RUNTIME_CLASS(CNewDBAdvPage), 0 );\
\
CNewTablePage*				pNewTable = \
	(CNewTablePage*)pSheet->FindPage( RUNTIME_CLASS(CNewTablePage), 0 );\
\
CDefineFieldCreationPage*	pDefineFieldCreation = \
	(CDefineFieldCreationPage*)pSheet->FindPage( RUNTIME_CLASS(CDefineFieldCreationPage), 0 );\
\
CAdvFieldConstrPage*		pAdvFieldConstr = \
	(CAdvFieldConstrPage*)pSheet->FindPage( RUNTIME_CLASS(CAdvFieldConstrPage), 0 );\
\
CSimpleFieldPage*		pSimpleFieldConstr = \
	(CSimpleFieldPage*)pSheet->FindPage( RUNTIME_CLASS(CSimpleFieldPage), 0 );\
\
CQueryNamePage*		pQueryName = \
	(CQueryNamePage*)pSheet->FindPage( RUNTIME_CLASS(CQueryNamePage), 0 );\
\
CEditQueryPage*		pQuerySelect = \
	(CEditQueryPage*)pSheet->FindPage( RUNTIME_CLASS(CEditQueryPage), 0 );\
\
CTumbnailPage*		pQueryTumbnail = \
	(CTumbnailPage*)pSheet->FindPage( RUNTIME_CLASS(CTumbnailPage), 0 );\
\
if( !pDBTypePage || !pDBAccess || !pDBAdv ||\
	!pNewTable || !pDefineFieldCreation || !pAdvFieldConstr || !pSimpleFieldConstr ||\
	!pQueryName || !pQuerySelect || !pQueryTumbnail )\
	return FaultResult;\
\
bool bAccessDB					= pDBTypePage->m_nDBaseType == 0;\
bool bUseAccessTemplate			= pDBAccess->m_nTemplate == 0;\
CString strAccessTemplate		= pDBAccess->m_strTemplatePath;\
CString strAdvConnString		= pDBAdv->m_strConnectionString;\
\
bool bCreateNewTable			= pDBAccess->m_bCreateNewTable == TRUE;\
\
bool bSimpleFieldCreationType	=  pDefineFieldCreation->m_nDefineType == 0;\


#define TEST_FOR_USER_CANCELED()\
pPage->UpdateData( FALSE );\
if( IsAborted() )\
{\
	NewDB_CreateNewDBCleanUp( pSheet, pPage, true );\
	return false;\
}\



/////////////////////////////////////////////////////////////////////////////
#endif __DBWIZARDS_H__
