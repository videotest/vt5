#ifndef __doc_h__
#define __doc_h__

#include "DBaseEngine.h"
#include "DBaseListenerImpl.h "											 
#include "constant.h"
#include "types.h"
#include "filterbase.h"
#include "thumbnail.h"
#include "notifyspec.h"

#include "docmiscint.h"


#define IDB_DOCUMENT_IMPL(func)\
HRESULT CDBaseDocument::XDBaseDocument::func()\
{\
	_try_nested(CDBaseDocument, DBaseDocument,func )\
	{		\
		pThis->func(); 		\
		return S_OK;\
	}\
	_catch_nested;\
}\

#define IDB_DOCUMENT_IMPL_AVAILABLE(func)\
HRESULT CDBaseDocument::XDBaseDocument::func(BOOL* pbAvailable)\
{\
	_try_nested(CDBaseDocument, DBaseDocument,func )\
	{		\
		*pbAvailable = pThis->func(); 		\
		return S_OK;\
	}\
	_catch_nested;\
}\



#define START_RECORD_IMPL()					\
IUnknown* punkRecordData = NULL;						 \
punkRecordData = pThis->GetRecordNamedData( );			  \
if(punkRecordData)									   \
{												\
	sptrINamedData spRecordND(punkRecordData);			 \
	punkRecordData->Release();						\
	if( spRecordND )							  \
	{											   \


#define STOP_RECORD_IMPL()					 \
		}									  \
	}										   \
												\
	return S_OK;								 \


#define START_DOC_IMPL()					\
IUnknown* punkDocData = NULL;						 \
punkDocData = pThis->GetDocNamedData();			  \
if(punkDocData)									   \
{												\
	sptrINamedData spDocND(punkDocData);			 \
	if( spDocND )							  \
	{											   \


#define STOP_DOC_IMPL()					 \
		}									  \
	}										   \
												\
	return S_OK;								 \



#define DECLARE_RECORD_AND_DOC_IMPL()					\
IUnknown* punkRecordData = NULL;						 \
punkRecordData = pThis->GetRecordNamedData();			  \
if(punkRecordData == NULL)									   \
	return E_FAIL; \
\
sptrINamedData spRecordND(punkRecordData);			 \
if( spRecordND == NULL)									   \
	return E_FAIL;\
\
IUnknown* punkDocData = NULL;						 \
punkDocData = pThis->GetDocNamedData();			  \
if(punkDocData == NULL)									   \
	return E_FAIL;												\
sptrINamedData spDocND(punkDocData);			 \
if( spDocND == NULL)							  \
	return E_FAIL;											   \
	\
sptrINamedData spThisND( pThis->GetControllingUnknown() );\
if( spThisND == NULL )\
	return E_FAIL;	 \
					 \
BSTR bstrCurSection;\
spThisND->GetCurrentSection( &bstrCurSection );\
CString strCurSection = bstrCurSection;			\
::SysFreeString( bstrCurSection );				\



class CDBaseDocument : public CDocBase,
						public CFileDataObjectImpl,
						public CSerializableObjectImpl,
						public CDBaseListenerImpl


{
	DECLARE_DYNCREATE(CDBaseDocument);
	GUARD_DECLARE_OLECREATE(CDBaseDocument);
	DECLARE_INTERFACE_MAP();
	ENABLE_MULTYINTERFACE()


	friend class CDBaseEngine;
public:
	CDBaseDocument();
	~CDBaseDocument();
	virtual void Init();
	virtual void OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var );
protected:
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
	virtual bool OnNewDocument();	

	//CFileDataObjectImpl
	virtual bool InitNew();

protected:	
	//Emulate record name data
	IUnknown* m_punkRecordNData;
	bool InitRecordNamedData();
	bool DeInitRecordNamedData();
public:
	IUnknown* GetRecordNamedData();
protected:

	//Own name data
	IUnknown* m_punkDocNData;
	bool InitDocNamedData();
	bool DeInitDocNamedData();
public:
	IUnknown* GetDocNamedData();
protected:

	//Safe state	
	bool m_bRecordNDataOK;
	bool m_bDocNDataOK;

	bool IsValidNameData(){ return m_bRecordNDataOK && m_bDocNDataOK;}

	//Get object 	
	bool IsSerializePath( CString strPath );

	void OnNewQueryActivate();

	//void OnNewViewActivate(IUnknown* pUnkView);


	bool PrepareWriteData( LPCTSTR path );

	
	//bool LoadAccessDBase( CString strMDBFielPath );

	//Set document interface
	//void InitQueryObjects();

protected:
	BEGIN_INTERFACE_PART(Data, INamedData)
		com_call SetupSection( BSTR bstrSectionName );

		com_call SetValue( BSTR bstrName, const tagVARIANT var );
		com_call GetValue( BSTR bstrName, tagVARIANT *pvar );

		com_call GetEntriesCount( long *piCount );
		com_call GetEntryName( long idx, BSTR *pbstrName );

		com_call RenameEntry( BSTR bstrOldName, BSTR bstrName  );
		com_call DeleteEntry( BSTR bstrName );

		com_call GetActiveContext( IUnknown **punk ); 
		com_call SetActiveContext( IUnknown *punk );

		com_call AddContext( IUnknown *punk );
		com_call RemoveContext( IUnknown *punk );

		com_call NameExists( BSTR bstrName, long* Exists );
		com_call GetObject( BSTR bstrName, BSTR bstrType, IUnknown **punk );

		com_call NotifyContexts( DWORD dwNotifyCode, IUnknown *punkNew, IUnknown *punkOld, GUID* dwData);
		com_call EnableBinaryStore( BOOL bBinary );
		
		com_call GetCurrentSection( BSTR* pbstrSection );

		com_call GetBaseGroupCount(int * pnCount);
		com_call GetBaseGroupFirstPos(long * plPos);
		com_call GetNextBaseGroup(GUID * pKey, long * plPos);
		com_call GetIsBaseGroup(GUID * pKey, BOOL * pbBase);
		com_call GetBaseGroupBaseObject(GUID * pKey, IUnknown ** ppunkObject);

		com_call GetBaseGroupObjectsCount(GUID * pKey, int * pnCount);
		com_call GetBaseGroupObjectFirstPos(GUID * pKey, long * plPos);
		com_call GetBaseGroupNextObject(GUID * pKey, long * plPos, IUnknown ** ppunkObject);
		com_call SetEmptySection( BSTR* bstrSectionName );
	END_INTERFACE_PART(Data);

	BEGIN_INTERFACE_PART(Types, IDataTypeManager)
		com_call GetTypesCount( long *pnCount );
		com_call GetType( long index, BSTR *pbstrType );
		com_call GetObjectFirstPosition( long nType, LONG_PTR *plpos );
		com_call GetNextObject( long nType, LONG_PTR *plpos, IUnknown **ppunkObj );
	END_INTERFACE_PART(Types);

	BEGIN_INTERFACE_PART(DBaseDocument, IDBaseDocument)
		com_call InitNamedData();
		com_call GetPrivateNamedData(IUnknown** ppunkNamedData);

		com_call GetNamedDataFieldsSection( BSTR* pbstrSection );
		com_call IsDBaseObject( IUnknown* punkObj, BOOL* pbDBaseObject, GUID* pGuidDBaseParent, BOOL* pbEmpty );

		com_call SetActiveQuery( IUnknown* punk );
		com_call GetActiveQuery( IUnknown** ppunk );		

		//event listener
		com_call RegisterDBaseListener( IUnknown* punk );
		com_call UnRegisterDBaseListener( IUnknown* punk );
		com_call FireEvent( BSTR bstrEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, 
			BSTR bstrTableName, BSTR bstrFieldName, tagVARIANT var );

		//Active image
		com_call SetActiveImage( BSTR bstrName );
		com_call GetActiveImage( BSTR* pbstrName );
		
		//is read only?
		com_call IsReadOnly( BOOL* pbReadOnly );
	END_INTERFACE_PART(DBaseDocument);



	BEGIN_INTERFACE_PART(DBConnection, IDBConnection)
		com_call LoadAccessDBase( BSTR bstrMDBFilePath);	
		com_call OpenConnectionFromString( BSTR bstrConnection );
		com_call CreateAdvancedConnection( );

		com_call CloseConnection();
		com_call OpenConnection();
		
		com_call GetConnection( IUnknown** ppUnkConnection );

		com_call ExecuteSQL( BSTR bstrSQL, IUnknown** ppUnkRecordset );	

		com_call GetMDBFileName( BSTR* bstrFileName );		
	END_INTERFACE_PART(DBConnection);

	

	BEGIN_INTERFACE_PART(DBaseStruct, IDBaseStruct)
		///////////////////////////////////
		//Temp table info array state
		///////////////////////////////////
		com_call IsDirtyTableInfo( BOOL* pbDirty );
		com_call RefreshTableInfo( BOOL bForceRefresh );

		///////////////////////////////////
		//TTable type info
		///////////////////////////////////	

		//Table
		com_call GetTableCount( int* pnCount );
		com_call GetTableName( int nIndex, BSTR* pbstrName );	

		com_call GetFieldCount( BSTR bstrTable, int* pnFieldsCount );
		com_call GetFieldName( BSTR bstrTable, int nFieldIndex, BSTR* pbstrFieldName );	

		//Fields	
		com_call GetFieldInfo( BSTR bstrTable, BSTR bstrField, 
			BSTR* pbstrUserName/*CBN*/, short* pnFieldType/*CBN*/,
			BSTR* pbstrPrimaryTable/*CBN*/, BSTR* pbstrPrimaryFieldName/*CBN*/,
			BOOL* pbRequiredValue/*CBN*/, BOOL* pbDefaultValue/*CBN*/,
			BSTR* pbstrDefaultValue/*CBN*/, BSTR* pbstrVTObjectType/*CBN*/
			); 
		// /*CBN*/ - can be null

		
		com_call AddTable( BSTR bstrTableName );
		com_call DeleteTable( BSTR bstrTableName );
		com_call AddField( BSTR bstrTableName, BSTR bstrFieldName, short nFieldType,
							BSTR bstrPrimaryTable, BSTR bstrPrimaryFieldName
							);
	
		com_call SetFieldInfo( BSTR bstrTableName, BSTR bstrFieldName, BSTR bstrUserName,
			BOOL bRequiredValue, BOOL bDefaultValue,
			BSTR bstrDefaultValue, BSTR bstrVTObjectType
			);

		com_call DeleteField( BSTR bstrTableName, BSTR bstrFieldName );
	END_INTERFACE_PART(DBaseStruct);


	BEGIN_INTERFACE_PART(DBaseFilterProvider, IDBaseFilterProvider)
		com_call ReBuildFilterChooserList();	
		com_call OnFilterChooserChange( HWND hWnd );
		com_call OnAddFilterChooser( HWND hWnd );
		com_call OnRemoveFilterChooser( HWND hWnd );
	END_INTERFACE_PART(DBaseFilterProvider);


	BEGIN_INTERFACE_PART(DBaseFilterHolder, IDBaseFilterHolder)
		com_call GetFiltersCount( int* pnCount );
		com_call GetFilter( int nFilterIndex, BSTR* pbstrName	);
		com_call AddFilter( BSTR bstrName );
		com_call EditFilter( BSTR bsrtFilterName, BSTR bstrName );
		com_call DeleteFilter( BSTR bsrtFilterName );

		com_call GetFilterForSQL( BSTR bsrtFilterName, BSTR* pbstrSQL );
		com_call GetFilterForOrganizer( BSTR bsrtFilterName, BSTR* pbstrOrganizer );

		//Filter Condition
		com_call SetFilterInOrganizerMode( BOOL bSetFilterInOrganizerMode );
		com_call GetFilterInOrganizerMode( BOOL* pbSetFilterInOrganizerMode );

		com_call GetFilterConditionCount( BSTR bsrtFilterName, int* pnCount );
		com_call GetFilterCondition( BSTR bsrtFilterName, int nConditionIndex,
							BSTR* pbstrTable, BSTR* pbstrField,
							BSTR* pbstrCondition, BSTR* pbstrValue,
							BSTR* pbstrValueTo, BOOL* pbANDunion,
							int* pnLeftBracketCount, int* pnRightBracketCount
							);
		com_call AddFilterCondition( BSTR bsrtFilterName, 
							BSTR bstrTable, BSTR bstrField,
							BSTR bstrCondition, BSTR bstrValue,
							BSTR bstrValueTo, BOOL bANDunion,
							int nLeftBracketCount, int nRightBracketCount							
							);

		com_call EditFilterCondition( BSTR bsrtFilterName, int nConditionIndex,
							BSTR bstrTable, BSTR bstrField,
							BSTR bstrCondition, BSTR bstrValue,
							BSTR bstrValueTo, BOOL bANDunion,
							int nLeftBracketCount, int nRightBracketCount							
							);

		com_call DeleteFilterCondition( BSTR bsrtFilterName, int nConditionIndex							
							);

		com_call GetFilterConditionForOrganizer( BSTR bsrtFilterName, int nConditionIndex,
							BSTR* pbstrCondition
							);

		com_call SetActiveFilter( BSTR  bstrFilterName );
		com_call GetActiveFilter( BSTR* pbstrFilterName );

		com_call SaveWorkingFilter();
		com_call GetWorkingFilterStatus( short* pnStatus);
		com_call IsValidFilter( BSTR bstrFilterName, BOOL bReportError, BOOL* pbValid );

		com_call SetIsActiveFilterApply( BOOL bApply );
		com_call GetIsActiveFilterApply( BOOL* pbApply );
	END_INTERFACE_PART(DBaseFilterHolder);


	BEGIN_INTERFACE_PART(ThumbnailSurface, IThumbnailSurface)
		com_call Draw( BYTE* pbi, HDC hdc, RECT rcTarget, SIZE sizeThumbnail, IUnknown* punkDataObject );
	END_INTERFACE_PART(ThumbnailSurface);


	BEGIN_INTERFACE_PART(NotifyPlace, INotifyPlace)
		com_call NotifyPutToData( IUnknown* punkObj );
		com_call NotifyPutToDataEx( IUnknown* punkObj, IUnknown** punkUndoObj );
		com_call Undo( IUnknown* punkUndoObj );
		com_call Redo( IUnknown* punkUndoObj );
	END_INTERFACE_PART(NotifyPlace);

	BEGIN_INTERFACE_PART(UserNameProvider, IUserNameProvider)
		com_call GetUserName( IUnknown* punkObject, BSTR* pbstrUserName );
	END_INTERFACE_PART(UserNameProvider);


protected:

	CFilterHolder	m_filterHolder;
	bool ReBuildFilterChooserList();
	bool OnFilterChooserChange( HWND hWnd );
	bool OnAddFilterChooser( HWND hWnd );
	bool OnRemoveFilterChooser( HWND hWnd );

	//void AnalizeRebuildFiltersCombo( const char *pszEvent );


	bool GetFilterChooserHwndArray( CArray<HWND, HWND>& arComboHwnd );
	
	

protected:
	bool ResetFilterChoosers();
	bool FillFilterChoosers();
	bool SetActiveFilterToFilterChoosers();

	sptrIQueryObject GetActiveQuery();

protected:
	bool DoUndoRedo(IUnknown *punkUndoObj, bool bRedo);

public:
	void FireSimpleEvent( const char* szEvent );

	//read only db
	bool	m_bReadOnly;
	CString m_str_dbd_open_file_name;


protected:
	

	bool FilterData(LPCTSTR path);
	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );

	
protected:

	//Objects
	CDBaseEngine m_dbEngine;
	CDBaseEventController m_eventController;
	bool InitDBaseEngine();	

	CString GetConnectionString();
	CString GetsecurityConnectionString();
	void SetConnectionString( CString strConn );

	
	GuidKey	m_activeQueryKey;
	CString GetActiveQueryName();
	void SetActiveQueryName( CString strActiveQuery );

	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );	

//helper
	static CString GetMDBFileNameFromConnectionCtring( CString strConnectionString,  CString strDefPath );
	bool	ActivateQueryInBlank( IUnknown* punkView );

	//bool GetTableFieldFromSection( CString strSection, CString& strTable, CString& strField ); 


// Auto Generate support
// See Generate.cpp && Generate.h
	
	/*
	bool Generate( CString strEvent, CString strUIName );
	
	bool OnEventAddTable( CString strTable );
	bool OnEventEditTable( CString strTable );
	bool OnEventDropTable( CString strTable );

	bool OnEventGenerateAXFormByQuery( CString strQuery );
	bool OnEventGenerateReportByQuery( CString strQuery );
	bool OnEventGenerateReportByAXForm( CString strAXForm );

	bool OnEventAnalizeAXForm( CString strAXForm );
	bool OnEventAnalizeReport( CString strReport );
	bool OnEventAnalizeQuery( CString strQuery );

	bool OnEventAnalizeAllAXForms( );
	bool OnEventAnalizeAllReports( );
	bool OnEventAnalizeAllQueries( );

	//helper functions
	bool GetAXForms( CArray<CString, CString>& arAXFrom );
	bool GetReports( CArray<CString, CString>& arReport );
	bool GetQueries( CArray<CString, CString>& arQuery );

	bool CloseActiveQuery( ); //return state( open, close )
	bool OpenActiveQuery( bool bPrevSate );



	bool AddFieldToAXForm( CString strTable, CString strField, 
					CRect& rcCtrl, IUnknown* pUnkForm, bool NewType );

	bool RemoveFieldFromAXForm( CString strTable, CString strField, IUnknown* pUnkForm );



	bool AddFieldToReport( CString strTable, CString strField, 
					CRect& rcCtrl, IUnknown* pUnkReport,
					bool bNewType, int& nCurRow, int nCurCol = 0, int nMainPage = 0 );

	bool RemoveFieldFromReport( CString strTable, CString strField, IUnknown* pUnkReport );


	bool GetLastRectFromAXForm( IUnknown* pUnkForm, CRect& rectCtrl );
	bool GetLastRectFromReport( IUnknown* pUnkRepot, CRect& rectCtrl, 
								int& nRow, int& nCol, int& nMainPage );

	bool GetDBaseFields( CArray<CFieldBase, CFieldBase>& arrFields );
	bool GetAXFormFields( IUnknown* pUnkForm, CArray<CFieldBase, CFieldBase>& arrFields );
	bool GetReportFields( IUnknown* pUnkRepot, CArray<CFieldBase, CFieldBase>& arrFields );
	bool GetQueryFields( IUnknown* pUnkQuery, CArray<CFieldBase, CFieldBase>& arrFields );

	bool GetAddFieldsFrom2Array( CArray<CFieldBase, CFieldBase>& arrFieldsBase,
								 CArray<CFieldBase, CFieldBase>& arrFieldsCompare,
								 CArray<CFieldBase, CFieldBase>& arrFieldsResult
								 );

	bool GetRemoveFieldsFrom2Array( CArray<CFieldBase, CFieldBase>& arrFieldsBase,
								 CArray<CFieldBase, CFieldBase>& arrFieldsCompare,
								 CArray<CFieldBase, CFieldBase>& arrFieldsResult
								 );
	*/
	//Active object
	class CActiveObjectInfo
	{
	public:
		CString m_strObjectType;
		CString m_strObjectName;
	};

	//Active image
	CString GetActiveImage( );
	void SetActiveImage( CString srtActiveObject );

	CString m_strActiveImage;	


	bool m_bNeedSetModifiedFlagAfterOpen;
	bool m_bNeedActivateQueryInView;
	bool m_bNeedActivateSpecificView;

	bool IsOtherDBaseDocExist();

	bool m_bShowUserNameInContext;
	bool m_bAddRootFieldNameInContext;

public:
	void	SetNeedSetModifiedFlagAfterOpen( bool bneed )
	{		m_bNeedSetModifiedFlagAfterOpen = bneed; }


	//{{AFX_DISPATCH(CDBaseDocument)
	afx_msg void SetValue(LPCTSTR Entry, const VARIANT FAR& Value);
	afx_msg VARIANT GetValue(LPCTSTR Entry);
	afx_msg void SetSection(LPCTSTR Section);
	afx_msg long GetEntriesCount();
	afx_msg BSTR GetEntryName(long index);
	afx_msg void RemoveValue(LPCTSTR szPath);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CDBaseDocTemplate
class CDBaseDocTemplate : public CDocTemplateBase
{
	DECLARE_DYNCREATE(CDBaseDocTemplate);
	GUARD_DECLARE_OLECREATE(CDBaseDocTemplate);
public:
	CDBaseDocTemplate();
protected:
	virtual CString GetDocTemplString();
};



/////////////////////////////////////////////////////////////////////////////
// CQueryFileFilter
class CMSAccessFileFilter : public CFileFilterBase  
{
	DECLARE_DYNCREATE(CMSAccessFileFilter);
	GUARD_DECLARE_OLECREATE(CMSAccessFileFilter);
public:
	CMSAccessFileFilter();
	virtual ~CMSAccessFileFilter();

	virtual bool InitNew(){return true;}
	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );

	virtual bool BeforeReadFile();
	virtual bool AfterReadFile();
};





#endif //__doc_h__