#ifndef __dbactions_h__
#define __dbactions_h__

//CREATE_DB_DOCSITE()//spDBDoc - Document

//GET_ACTIVE_QUERY_FROM_DOC() // spQueryData - active Query data,spSelectQuery - select query

#define CREATE_DB_DOCSITE() \
	sptrIDocumentSite	spDocSite( m_punkTarget );	\
	if( spDocSite == NULL )						\
		return false;								  \
														\
	sptrIDBaseDocument spDBDoc( spDocSite );			\
	if( spDBDoc == NULL )\
		return false;\


#define GET_ACTIVE_QUERY_FROM_DOC() \
	sptrIDocumentSite	spDocSite( m_punkTarget );	\
	if( spDocSite == NULL )						\
		return false;								  \
														\
	sptrIDBaseDocument spDBDoc( spDocSite );			\
	if( spDBDoc == NULL )\
		return false;\
	IUnknown* pUnkQuery = NULL;\
	if( S_OK != spDBDoc->GetActiveQuery( &pUnkQuery ) )\
		return false;\
					  \
	if( pUnkQuery == NULL )\
		return false;\
					  \
	sptrIQueryObject spQueryData( pUnkQuery );\
	sptrISelectQuery spSelectQuery( pUnkQuery );\
																	\
	pUnkQuery->Release();\
																\
	if( spQueryData == NULL || spSelectQuery == NULL )\
		return false;	\




class CFindImpl
{	
public:
	CFindImpl();
	~CFindImpl();

	bool	m_bFindInField;
	CString	m_strFindTable;
	CString	m_strFindField;
	bool	m_bFindMatchCase;
	bool	m_bFindRegularExpression;
	CString	m_strTextToFind;

	bool	m_bFindFromCurRecord;

	

	bool GetSettingsFromQuery( sptrISelectQuery spSelectQuery, sptrIDBaseDocument spDBDoc );
	bool ExecuteSettings( sptrISelectQuery spSelectQuery, sptrIDBaseDocument spDBDoc );

	long  ProcessFind( IUnknown* punkQ, IUnknown* punkDoc, bool bFoundFromCurRecord, int* pnField );


};


class CDBaseActionBase : public CActionBase
{

public:
	CDBaseActionBase();
	virtual ~CDBaseActionBase();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
	virtual bool IsDBaseDocument();
};

class CDBaseFilterBase : public CActionBase
{

public:
	CDBaseFilterBase();
	virtual ~CDBaseFilterBase();
public:
	sptrIQueryObject GetActiveQuery( IUnknown* punkDocument = NULL );

	sptrIQueryObject GetActiveQueryFromViewTarget( );
};

//[ag]1. classes


/*
class CActionDBaseImport : public CActionBase
{
	DECLARE_DYNCREATE(CActionDBaseImport)
	DECLARE_OLECREATE(CActionDBaseImport)

public:
	CActionDBaseImport();
	virtual ~CActionDBaseImport();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();

	sptrIDBaseDocument GetActiveDBaseDocument();
	IUnknown* GetActiveObject();
};
*/

class CActionFilterOrganizer : public CDBaseFilterBase
{
	DECLARE_DYNCREATE(CActionFilterOrganizer)
	GUARD_DECLARE_OLECREATE(CActionFilterOrganizer)

public:
	CActionFilterOrganizer();
	virtual ~CActionFilterOrganizer();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionSaveFilter : public CDBaseFilterBase
{
	DECLARE_DYNCREATE(CActionSaveFilter)
	GUARD_DECLARE_OLECREATE(CActionSaveFilter)

public:
	CActionSaveFilter();
	virtual ~CActionSaveFilter();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionApplyFilter : public CDBaseFilterBase
{
	DECLARE_DYNCREATE(CActionApplyFilter)
	GUARD_DECLARE_OLECREATE(CActionApplyFilter)

public:
	CActionApplyFilter();
	virtual ~CActionApplyFilter();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionCreateFilter : public CDBaseFilterBase
{
	DECLARE_DYNCREATE(CActionCreateFilter)
	GUARD_DECLARE_OLECREATE(CActionCreateFilter)

public:
	CActionCreateFilter();
	virtual ~CActionCreateFilter();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionChooseFilter : public CActionBase
{
	DECLARE_DYNCREATE(CActionChooseFilter)
	GUARD_DECLARE_OLECREATE(CActionChooseFilter)

public:
	CActionChooseFilter();
	virtual ~CActionChooseFilter();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

/*
class CActionDBGenerate : public CActionBase
{
	DECLARE_DYNCREATE(CActionDBGenerate)
	GUARD_DECLARE_OLECREATE(CActionDBGenerate)

public:
	CActionDBGenerate();
	virtual ~CActionDBGenerate();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};
*/

class CActionDBExecuteSQL : public CActionBase
{
	DECLARE_DYNCREATE(CActionDBExecuteSQL)
	GUARD_DECLARE_OLECREATE(CActionDBExecuteSQL)

public:
	CActionDBExecuteSQL();
	virtual ~CActionDBExecuteSQL();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};


class CActionDBGenerateReportByAXForm : public CActionBase
{
	DECLARE_DYNCREATE(CActionDBGenerateReportByAXForm)
	GUARD_DECLARE_OLECREATE(CActionDBGenerateReportByAXForm)
	
	ENABLE_UNDO();

public:
	CActionDBGenerateReportByAXForm();
	virtual ~CActionDBGenerateReportByAXForm();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();	

	IQueryObjectPtr	GetActiveQuery(); 
	bool ProcessReportByQuery( IReportForm* pIReport, IQueryObject* pIQuery );

	virtual bool DoUndo();
	virtual bool DoRedo();

	CObjectListUndoRecord	m_changes;

};

class CActionDBaseInfo : public CActionBase
{
	DECLARE_DYNCREATE(CActionDBaseInfo)
	GUARD_DECLARE_OLECREATE(CActionDBaseInfo)

public:
	CActionDBaseInfo();
	virtual ~CActionDBaseInfo();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionRequery : public CActionBase
{
	DECLARE_DYNCREATE(CActionRequery)
	GUARD_DECLARE_OLECREATE(CActionRequery)

public:
	CActionRequery();
	virtual ~CActionRequery();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

/*
class CActionDBGridOptions : public CActionBase
{
	DECLARE_DYNCREATE(CActionDBGridOptions)
	GUARD_DECLARE_OLECREATE(CActionDBGridOptions)

public:
	CActionDBGridOptions();
	virtual ~CActionDBGridOptions();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};
*/

class CActionShowDBGrid : public CActionShowViewBase
{
	DECLARE_DYNCREATE(CActionShowDBGrid)
	GUARD_DECLARE_OLECREATE(CActionShowDBGrid)
public:
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();	

	virtual CString GetViewProgID();
};

class CActionDBaseFindNext : public CActionBase, public CLongOperationImpl, public CFindImpl
{
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();

	DECLARE_DYNCREATE(CActionDBaseFindNext)
	GUARD_DECLARE_OLECREATE(CActionDBaseFindNext)

public:
	CActionDBaseFindNext();
	virtual ~CActionDBaseFindNext();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionDBaseFind : public CActionBase, public CLongOperationImpl, public CFindImpl
{
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();

	DECLARE_DYNCREATE(CActionDBaseFind)
	GUARD_DECLARE_OLECREATE(CActionDBaseFind)

ENABLE_SETTINGS();
public:
	CActionDBaseFind();
	virtual ~CActionDBaseFind();

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionDBaseRestore : public CActionBase, public CLongOperationImpl
{
	BYTE* m_pBufRaw;
	BYTE* m_pBufPack;

	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();

	DECLARE_DYNCREATE(CActionDBaseRestore)
	GUARD_DECLARE_OLECREATE(CActionDBaseRestore)

public:
	CActionDBaseRestore();
	virtual ~CActionDBaseRestore();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionDBaseBackUp : public CActionBase, public CLongOperationImpl
{
	BYTE* m_pBufRaw;
	BYTE* m_pBufPack;
	
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();

	DECLARE_DYNCREATE(CActionDBaseBackUp)
	GUARD_DECLARE_OLECREATE(CActionDBaseBackUp)

public:
	CActionDBaseBackUp();
	virtual ~CActionDBaseBackUp();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionShowGallery : public CActionShowViewBase
{
	DECLARE_DYNCREATE(CActionShowGallery)
	GUARD_DECLARE_OLECREATE(CActionShowGallery)

public:
	virtual CString GetViewProgID();
	virtual bool IsAvaible();
};

class CActionShowBlankPreview : public CActionShowViewBase
{
	DECLARE_DYNCREATE(CActionShowBlankPreview)
	GUARD_DECLARE_OLECREATE(CActionShowBlankPreview)
public:	
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();	

	virtual CString GetViewProgID();
};

class CActionShowBlankDesign : public CActionShowViewBase
{
	DECLARE_DYNCREATE(CActionShowBlankDesign)
	GUARD_DECLARE_OLECREATE(CActionShowBlankDesign)

public:	
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();	
	
	virtual CString GetViewProgID();
};

/*
class CActionFilterMode : public CActionBase
{
	DECLARE_DYNCREATE(CActionFilterMode)
	GUARD_DECLARE_OLECREATE(CActionFilterMode)

public:
	CActionFilterMode();
	virtual ~CActionFilterMode();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();	
};
*/


class CActionChoiseQuery : public CDBaseActionBase
{
	DECLARE_DYNCREATE(CActionChoiseQuery)
	GUARD_DECLARE_OLECREATE(CActionChoiseQuery)

public:
	CActionChoiseQuery();
	virtual ~CActionChoiseQuery();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};


class CActionCloseQuery : public CDBaseActionBase
{
	DECLARE_DYNCREATE(CActionCloseQuery)
	GUARD_DECLARE_OLECREATE(CActionCloseQuery)

public:
	CActionCloseQuery();
	virtual ~CActionCloseQuery();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionOpenQuery : public CDBaseActionBase
{
	DECLARE_DYNCREATE(CActionOpenQuery)
	GUARD_DECLARE_OLECREATE(CActionOpenQuery)

public:
	CActionOpenQuery();
	virtual ~CActionOpenQuery();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionCancel : public CDBaseActionBase
{
	DECLARE_DYNCREATE(CActionCancel)
	GUARD_DECLARE_OLECREATE(CActionCancel)

public:
	CActionCancel();
	virtual ~CActionCancel();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};


class CActionUpdate : public CDBaseActionBase
{
	DECLARE_DYNCREATE(CActionUpdate)
	GUARD_DECLARE_OLECREATE(CActionUpdate)

public:
	CActionUpdate();
	virtual ~CActionUpdate();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionEditRecord : public CDBaseActionBase
{
	DECLARE_DYNCREATE(CActionEditRecord)
	GUARD_DECLARE_OLECREATE(CActionEditRecord)

public:
	CActionEditRecord();
	virtual ~CActionEditRecord();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};


class CActionInsertRecord : public CDBaseActionBase
{
	DECLARE_DYNCREATE(CActionInsertRecord)
	GUARD_DECLARE_OLECREATE(CActionInsertRecord)

public:
	CActionInsertRecord();
	virtual ~CActionInsertRecord();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionDeleteRecord : public CDBaseActionBase
{
	DECLARE_DYNCREATE(CActionDeleteRecord)
	GUARD_DECLARE_OLECREATE(CActionDeleteRecord)

public:
	CActionDeleteRecord();
	virtual ~CActionDeleteRecord();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionMovePrev : public CDBaseActionBase
{
	DECLARE_DYNCREATE(CActionMovePrev)
	GUARD_DECLARE_OLECREATE(CActionMovePrev)

public:
	CActionMovePrev();
	virtual ~CActionMovePrev();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionMoveNext : public CDBaseActionBase
{
	DECLARE_DYNCREATE(CActionMoveNext)
	GUARD_DECLARE_OLECREATE(CActionMoveNext)

public:
	CActionMoveNext();
	virtual ~CActionMoveNext();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionMoveLast : public CDBaseActionBase
{
	DECLARE_DYNCREATE(CActionMoveLast)
	GUARD_DECLARE_OLECREATE(CActionMoveLast)

public:
	CActionMoveLast();
	virtual ~CActionMoveLast();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionMoveFirst : public CDBaseActionBase
{
	DECLARE_DYNCREATE(CActionMoveFirst)
	GUARD_DECLARE_OLECREATE(CActionMoveFirst)

public:
	CActionMoveFirst();
	virtual ~CActionMoveFirst();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionQueryWizard : public CDBaseActionBase
{
	DECLARE_DYNCREATE(CActionQueryWizard)
	GUARD_DECLARE_OLECREATE(CActionQueryWizard)

public:
	CActionQueryWizard();
	virtual ~CActionQueryWizard();

public:
	virtual bool Invoke();
};

class CActionTableWizard : public CDBaseActionBase
{
	DECLARE_DYNCREATE(CActionTableWizard)
	GUARD_DECLARE_OLECREATE(CActionTableWizard)

public:
	CActionTableWizard();
	virtual ~CActionTableWizard();

public:
	virtual bool Invoke();
};			


class CActionGalleryOptions : public CDBaseActionBase
{
	DECLARE_DYNCREATE(CActionGalleryOptions)
	GUARD_DECLARE_OLECREATE(CActionGalleryOptions)

public:
	CActionGalleryOptions();
	virtual ~CActionGalleryOptions();

public:
	virtual bool Invoke();
};



class CActionLoadDatabase : public CActionBase
{
	DECLARE_DYNCREATE(CActionLoadDatabase)
	GUARD_DECLARE_OLECREATE(CActionLoadDatabase)

public:
	CActionLoadDatabase();
	virtual ~CActionLoadDatabase();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};


class CActionAdvancedConnection : public CDBaseActionBase
{
	DECLARE_DYNCREATE(CActionAdvancedConnection)
	GUARD_DECLARE_OLECREATE(CActionAdvancedConnection)

public:
	virtual bool Invoke();
};


class CActionAddSortFieldAsc : public CDBaseFilterBase
{
	DECLARE_DYNCREATE(CActionAddSortFieldAsc)
	GUARD_DECLARE_OLECREATE(CActionAddSortFieldAsc)

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();

	virtual BOOL	GetSortOrder(){	return TRUE; }
};

class CActionAddSortFieldDesc : public CActionAddSortFieldAsc
{
	DECLARE_DYNCREATE(CActionAddSortFieldDesc)
	GUARD_DECLARE_OLECREATE(CActionAddSortFieldDesc)

	virtual BOOL	GetSortOrder(){	return FALSE; }
};

class CActionRemoveSortFields : public CDBaseFilterBase
{
	DECLARE_DYNCREATE(CActionRemoveSortFields)
	GUARD_DECLARE_OLECREATE(CActionRemoveSortFields)

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();	
};


class CActionRecordInfo : public CActionBase
{
	DECLARE_DYNCREATE(CActionRecordInfo)
	GUARD_DECLARE_OLECREATE(CActionRecordInfo)

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
};


class CActionFieldProperties : public CActionBase
{
	//ENABLE_UNDO();

	CActionFieldProperties();
	DECLARE_DYNCREATE(CActionFieldProperties)
	GUARD_DECLARE_OLECREATE(CActionFieldProperties)

public:
	//virtual bool DoUndo();
	//virtual bool DoRedo();

protected:
	CObjectListUndoRecord	m_changes;
	IUnknownPtr				m_ptrDBaseAXCtrl;

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();	

protected:
	IBlankViewPtr	GetActiveBlankView();
	IUnknownPtr		GetActiveControlData( );
	IUnknownPtr		GetActiveAXControl( IUnknown* punkAXData );


	BOOL	m_nAutoLabel;
	BOOL	m_bEnableEnumeration;
	short	m_nLabelPos;

	_bstr_t m_bstrTable;
	_bstr_t m_bstrField;

	_bstr_t	m_bstrCaption;
	_bstr_t	m_bstrDefValue;
	
	BOOL	m_bHasDefValue;
	
};

class CActionDeleteRecords : public CDBaseActionBase
{
	DECLARE_DYNCREATE(CActionDeleteRecords)
	GUARD_DECLARE_OLECREATE(CActionDeleteRecords)

public:
	CActionDeleteRecords();
	virtual ~CActionDeleteRecords();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
	CString			get_delete_sql();
};


class CActionUpdateRecordsetInfo : public CDBaseActionBase
{
	DECLARE_DYNCREATE(CActionUpdateRecordsetInfo)
	GUARD_DECLARE_OLECREATE(CActionUpdateRecordsetInfo)

public:
	CActionUpdateRecordsetInfo();
	virtual ~CActionUpdateRecordsetInfo();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
};

class CActionEmptyDBField : public CActionBase
{
	DECLARE_DYNCREATE(CActionEmptyDBField)
	GUARD_DECLARE_OLECREATE(CActionEmptyDBField)
	ENABLE_UNDO();
public:	
	virtual bool	Invoke();
	virtual bool	IsAvaible();

//undo interface
	virtual bool	DoUndo();
	virtual bool	DoRedo();

	IUnknownPtr		get_active_db_object();

	CObjectListUndoRecord	m_changes;
};


#endif //__dbactions_h__
