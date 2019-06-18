#ifndef __selectquery_h__
#define __selectquery_h__

#include "DBaseListenerImpl.h"
#include "types.h"
#include "querybase.h"

#include "AXCtrlBase.h"
class CDBLocksInfo;
class CObjectInfo
{
public:
	CObjectInfo(){ m_bBaseObject = false; }
	CString m_strObjectName;
	CString strObjectType;

	bool	m_bBaseObject;
	bool	m_bActive;
};

class CViewContext
{
public:
	CViewContext();
	~CViewContext();					

	GuidKey	m_guidKeyView;
	CPtrList m_arObjectInfo;

};


class CQueryObject : public CAXFormBase, public CDBaseListenerImpl, public CEventListenerImpl
{
	PROVIDE_GUID_INFO()
	DECLARE_DYNCREATE(CQueryObject);
	GUARD_DECLARE_OLECREATE(CQueryObject);

	DECLARE_INTERFACE_MAP();

	ENABLE_MULTYINTERFACE();

public:
	CQueryObject();
	virtual ~CQueryObject();
	void		DeInit();
public:							  
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );
	virtual bool CanBeBaseObject();
	virtual DWORD GetNamedObjectFlags();	//return the object flags

	
	bool CanGetValue();
	
	int GetFieldIndexFromSection(BSTR bstrTableField, sptrIDBaseDocument spDBDoc, short& nFieldType );

protected:
	BEGIN_INTERFACE_PART(Query, IQueryObject)
		com_call SetBaseTable( BSTR bstr );
		com_call GetBaseTable( BSTR *pbstr );

		com_call IsSimpleQuery( BOOL* pbSimple );
		com_call SetSimpleQuery( BOOL bSimple );

		com_call SetManualSQL( BSTR bstrSQL );
		com_call GetManualSQL( BSTR* pbstrSQL );

		com_call GetSQL( BSTR* pbstrSQL );

		//Fields
		//available when open only
		com_call GetFieldsCount( int* pnCount );
		com_call GetField( int nIndex, BSTR* pbstrInnerName, BSTR* pbstrTable, BSTR* pbstrField );

		com_call SetActiveField( int nIndex );
		com_call GetActiveField( int* pnIndex );

		//info
		com_call GetFieldLen( int nIndex, long* pLen );
	END_INTERFACE_PART(Query)



	BEGIN_INTERFACE_PART(SelectQuery, ISelectQuery2)		

		com_call Open();
		com_call Close();
		com_call IsOpen(BOOL* pbOpen);
		///////////////////////////////////
		//Edit
		///////////////////////////////////	
		com_call Edit();
		com_call Update();
		com_call UpdateInteractive( BOOL bAskOnlyObjectChanges );
		com_call Cancel();
		
		///////////////////////////////////
		//Info & state
		///////////////////////////////////	
		com_call GetSupports( short nCursorOptionEnum, BOOL* pbSucceeded );
		com_call GetState( short* pnQueryState );
		com_call IsBOF( BOOL* pBResult );
		com_call IsEOF( BOOL* pBResult );
		
		///////////////////////////////////
		//Manipulation
		///////////////////////////////////	
		com_call Insert();
		com_call Delete();

		///////////////////////////////////
		//Navigation
		///////////////////////////////////
		com_call MoveFirst();
		com_call MoveNext();
		com_call MovePrev();
		com_call MoveLast();	

		com_call GoToRecord( long nRecordNum );
		///////////////////////////////////
		//Error
		///////////////////////////////////
		com_call GetLastError(BSTR* bstrError);	

		com_call IsAvailableField( BSTR bstrTable, BSTR bstrField, BOOL* pbAvailable );

		com_call SetValue( BSTR bstrName, const tagVARIANT var, BOOL* pbPlaceToRecordset );
		com_call GetValue( BSTR bstrName, tagVARIANT *pvar );
		com_call DeleteEntry( BSTR bstrName );


		com_call GetRecordCount( long* pnRecordCount );
		com_call GetCurrentRecord( long* pnCurrentRecord );
		com_call SetCurrentRecord( long nCurRecord, long* pnRealRecord );

		//In this mode only simple data & tumbnail can read
		com_call EnterPrivateMode();
		com_call LeavePrivateMode( BOOL bResotoreBookmark, BOOL bRereadVisibleObject );
		
		
		//Tumbnail support
		com_call GetTumbnail( void** ppTumbnail );
		com_call GetTumbnailCaption( BSTR* pbstrCaption );
		//com_call GenerateTumbnail( IUnknown* pUnkView, BSTR bstrTable, BSTR bstrField );

		com_call GetTumbnailLocation( BSTR* pbstrTable, BSTR* pbstrField );
		com_call SetTumbnailLocation( BSTR bstrTable, BSTR bstrField );

		com_call GetTumbnailCaptionLocation( BSTR* pbstrTable, BSTR* pbstrField );
		com_call SetTumbnailCaptionLocation( BSTR bstrTable, BSTR bstrField );

		com_call GetTumbnailSize( SIZE* pSize );
		com_call SetTumbnailSize( SIZE size );

		com_call GetTumbnailViewSize( SIZE* pSize );
		com_call SetTumbnailViewSize( SIZE size );	


		com_call SetCanProcessFind( BOOL bCanProcessFind );	
		com_call GetCanProcessFind( BOOL* pbCanProcessFind );	

		com_call SetFindSettings( BOOL bFindInField, BSTR bstrTable, BSTR bstrField,
								BOOL bMatchCase, BOOL bRegularExpression, BSTR bstrTextToFind );

		com_call GetFindSettings( BOOL* pbFindInField, BSTR* pbstrTable, BSTR* pbstrField,
								BOOL* pbMatchCase, BOOL* pbRegularExpression, BSTR* pbstrTextToFind );

		com_call GetTumbnailBorderSize( SIZE* pSize );
		com_call SetTumbnailBorderSize( SIZE size );
		
		com_call IsDBaseObject( IUnknown* punkObj, BOOL* pbDBaseObject, GUID* pGuidDBaseParent, BOOL* pbEmpty );

		com_call GetSortFieldsCount( int* pnCount );
		com_call AddSortField( BSTR bstrTable, BSTR bstrField, BOOL bAscending );
		com_call GetSortField( int nIndex, BSTR* pbstrTable, BSTR* pbstrField, BOOL* pbAscending );
		com_call RemoveSortFields( );

		com_call SaveContextsState();
		com_call RestoreContextsState();

		com_call DeleteEx( BOOL bQuiet );

		com_call AttachLockInfo(void*);


	END_INTERFACE_PART(SelectQuery)
		CDBLocksInfo* m_pdbli;

	BEGIN_INTERFACE_PART(DBChild, IDBChild)
		com_call SetParent( IUnknown* punkParent );
		com_call GetParent( IUnknown** ppunkParent );
	END_INTERFACE_PART(DBChild)


	BEGIN_INTERFACE_PART(BlankForm, IBlankForm)
		com_call GetControlsCount( BSTR bstr_table, BSTR bstr_field, long* plcount );
	END_INTERFACE_PART(BlankForm)

	BEGIN_INTERFACE_PART(NotifyObject, INotifyObject)
		com_call NotifyCreate();
		com_call NotifyDestroy();
		com_call NotifyActivate( bool bActivate );
		com_call NotifySelect( bool bSelect );
	END_INTERFACE_PART(NotifyObject)

	BEGIN_INTERFACE_PART(QueryMisc, IQueryMisc)
		com_call GetGridFieldInfo( BSTR bstrTable, BSTR bstrField, int* pnWidth, int* pnPos );
		com_call SetGridFieldInfo( BSTR bstrTable, BSTR bstrField, int nWidth, int nPos );
		com_call EmptyGridFieldInfo();
	END_INTERFACE_PART(QueryMisc)

	void DeleteNotExistGridFields();


protected:
	
	void OnFieldChange( CString strTable, CString strField, _variant_t var );	

	bool IsOpen();
	CBaseQuery		m_query;
	QueryState		m_state;
	long			m_lCanModifyDatabase;
	bool			CanModifyDatabase();


	//Register listener on DBaseController & Shell Document Controller
	void _RegisterOnDBController( );
	void _UnRegisterOnDBController();
	bool m_bWasRegister;	

	virtual void OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var );
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	//Error
	CString m_strLastError;
	void  EmptyLastError(){ m_strLastError.Empty(); }

	ADO::_RecordsetPtr m_spRecordset;
	long m_lBookMark;
	

	void InitCacheBuffers();
	void DestroyCacheBuffers();
	CArray<CQueryField*, CQueryField*> m_arCacheFields;	

	
	bool OnSetValue();
	bool OnGetValue();
	
	void InitFieldsBuffers();
	void ClearFieldsBuffers();

	//route to fields
	bool IsRecordsetModified();
	bool InitDefValues( bool bAfterInsert = false );
	bool OnRecordChange();
	bool OnCancel();
	bool OnUpdate();	

	QueryState GetState();
	
	
	bool m_bPrivateMode;

	//Tmbnail
	CString m_strTumbnailTable;
	CString m_strTumbnailField;

	CString m_strTumbnailCaptionTable;
	CString m_strTumbnailCaptionField;
	

	CSize m_sizeTumbnail;
	CSize m_sizeTumbnailView;

	CSize m_sizeTumbnailBorder;

	_variant_t varPrivateBookmark;


	bool		ResolveSortProblem( CString strSQL, ADO::CursorLocationEnum curLocOld, 
									bool& bNeedRequery, ADO::CursorLocationEnum& curLocNew );

	//Find
	bool	m_bCanProcessFind;
	bool	m_bFindInField;
	CString	m_strFindTable;
	CString	m_strFindField;
	bool	m_bFindMatchCase;
	bool	bFindRegularExpression;
	CString	m_strTextToFind;



	sptrIDBaseDocument m_sptrParent;
	INamedDataPtr	GetPrivateNamedData();

		
	bool ClearUndoRedoList();

	
	void SaveContextState();
	void DestroyContextState();
	void RestoreContextState();
	
	CPtrList	m_arViewContext;


	int			m_nActiveField;

	class CCtrlFormData
	{		
	public:
		CCtrlFormData()
		{
			ft = ftNotSupported;
			bDBaseCtrl = false;
			rcPos = NORECT;
		}
		IUnknownPtr ptrCtrlData;
		IUnknownPtr ptrCtrlAX;
		FieldType	ft;
		bool		bDBaseCtrl;
		CRect		rcPos;
		_bstr_t		bstrProgID;
		CString		strTable;
		CString		strField;

	};

	bool		AnalizeQueryProperty();

	//auto ctrls update
	void		AnalizeBlank();
	bool		RemoveNotExistControls();
	bool		AddNewControls();
	bool		AddCtrl( CQueryField* pqf );
	CRect		GetPreferedCtrlArea( CQueryField* pqf );

	bool		GetFormCtrls( CArray<CCtrlFormData*, CCtrlFormData*>& arrCtrls);

	bool		IsSupportedCtrlType( FieldType ft );
	bool		CanSetCtrl( CRect rcNewCtrl, CArray<CCtrlFormData*, CCtrlFormData*>* parrCtrls );
	bool		FindCtrlRect(		CRect& rcNewPos, FieldType ft,
									int nWidth, int nHeight, bool bTryRight/*or bottom*/, 
									CArray<CCtrlFormData*, CCtrlFormData*>* parrCtrls );
	

protected:
	long		m_llast_primary_key;

	long		_get_primary_key_value();
	long		_find_record( long lprimary_key );
	long		_get_primary_key_idx();
	void		_save_primary_key( bool bset_modify_flag );
	long		_get_last_primary_key();

};

class CBlankCtrl : public CAXCtrlBase
{
	PROVIDE_GUID_INFO()
	DECLARE_DYNCREATE(CBlankCtrl);
	GUARD_DECLARE_OLECREATE(CBlankCtrl);	
public:
	CBlankCtrl();
	~CBlankCtrl();
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );


	DECLARE_INTERFACE_MAP();
	//ENABLE_MULTYINTERFACE();

	BEGIN_INTERFACE_PART(NotifyObject, INotifyObject)
		com_call NotifyCreate();
		com_call NotifyDestroy();
		com_call NotifyActivate( bool bActivate );
		com_call NotifySelect( bool bSelect );
	END_INTERFACE_PART(NotifyObject)

	BEGIN_INTERFACE_PART(ViewSubType, IViewSubType)
		com_call GetViewSubType( unsigned long* pViewSubType );
		com_call SetViewSubType( unsigned long ViewSubType );
	END_INTERFACE_PART(ViewSubType)

	ISelectQueryPtr GetQuery();
	IDBaseStructPtr GetDBStruct();
	bool			IsDBaseControl( FieldType* pftype = 0/*out*/);

	IUnknownPtr		GetAXCtrl();
private:
	unsigned long m_ViewSubType;
};



#endif //__selectquery_h__