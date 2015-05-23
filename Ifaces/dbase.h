#ifndef __dbaseint_h__
#define __dbaseint_h__


//Query object type name
#define szTypeQueryObject "Query"


#define szTypeBlankCtrl		"BlankCtrl"	
#define szTypeBlankForm		szTypeQueryObject


#define szEventObjectKeyWasChanged	"ObjectKeyWasChanged"


#define szChooserEventStringValueChange			"ChooserEventStringValueChange"
#define szChooserEventUINTValueChange			"ChooserEventUINTValueChange"


#define DBDateTimeProgID	"DBCONTROLS.DBDateTimeCtrl.1"
#define DBNumberProgID		"DBCONTROLS.DBNumberCtrl.1"
#define DBVTObjectProgID	"DBCONTROLS.DBObjectCtrl.1"
#define DBTextProgID		"DBCONTROLS.DBTextCtrl.1"



#define SECTION_DBASEFIELDS				"\\RecordNamedData\\Fields"

/////////////////////////////////////////////////////////////////////////////
enum SortKind{
	skNone = 0,
	skAscending = 1,
	skDescending = 2,
};

/////////////////////////////////////////////////////////////////////////////
enum FieldType
{
	ftDigit = 0,
	ftString = 1,	
	ftDateTime = 2,
	ftVTObject = 3,	
	ftPrimaryKey = 4,
	ftForeignKey = 5,
	ftNotSupported = 6
};

enum WorkingFilterStatus
{
	wfsNotDefine = 0,
	wfsNew = 1,
	wfsSelected = 2,
	wfsSelectedChange = 3,
};


interface IQueryObject : public IUnknown
{
	com_call SetBaseTable( BSTR bstr ) = 0;
	com_call GetBaseTable( BSTR *pbstr ) = 0;

	com_call IsSimpleQuery( BOOL* pbSimple ) = 0;
	com_call SetSimpleQuery( BOOL bSimple ) = 0;

	com_call SetManualSQL( BSTR bstrSQL ) = 0;
	com_call GetManualSQL( BSTR* pbstrSQL ) = 0;

	com_call GetSQL( BSTR* pbstrSQL ) = 0;

	//Fields
	//available when open only
	com_call GetFieldsCount( int* pnCount ) = 0;
	com_call GetField( int nIndex, BSTR* pbstrInnerName, BSTR* pbstrTable, BSTR* pbstrField ) = 0;		

	com_call SetActiveField( int nIndex ) = 0;
	com_call GetActiveField( int* pnIndex ) = 0;

	//Info
	com_call GetFieldLen( int nIndex, long* pLen ) = 0;
};




interface IDBControl : public IUnknown
{
	com_call GetTableName( BSTR* pbstrTableName ) = 0;
	com_call SetTableName( BSTR   bstrTableName ) = 0;

	com_call GetFieldName( BSTR* pbstrFieldName ) = 0;
	com_call SetFieldName( BSTR   bstrFieldName ) = 0;

	com_call GetEnableEnumeration( BOOL* pbEnable ) = 0;
	com_call SetEnableEnumeration( BOOL   bEnable ) = 0;

	com_call GetAutoLabel( BOOL* pbAutoLabel ) = 0;
	com_call SetAutoLabel( BOOL   bAutoLabel ) = 0;
	
	com_call GetValue( tagVARIANT *pvar ) = 0;
	com_call SetValue( const tagVARIANT var ) = 0;

	com_call GetLabelPosition( short* pnLabelPosition ) = 0;
	com_call SetLabelPosition( short  nLabelPosition ) = 0;
	
	com_call GetDBaseDocument( IUnknown** ppunkDBaseDocument ) = 0;

	com_call ChangeValue( const tagVARIANT var ) = 0;

	com_call SetBlankViewMode( short oldBlankViewMode, short newBlankMode ) = 0;
	com_call SetReportViewMode( short oldReportViewMode, short newReportMode ) = 0;
};

// [vanek] BT2000: 3510 - 26.01.2004
interface IDBControl2 : public IDBControl
{
    com_call GetReadOnly( BOOL* pbReadOnly ) = 0;
	com_call SetReadOnly( BOOL	bReadOnly ) = 0;
};

interface IDBObjectControl : public IUnknown
{
	com_call SetViewType( BSTR bstrViewName ) = 0;
	com_call GetViewType( BSTR* pbstrViewName ) = 0;

	com_call SetAutoViewType( BOOL bAutoViewType ) = 0;
	com_call GetAutoViewType( BOOL* pbAutoViewType ) = 0 ;

	com_call Build() = 0;
};






interface IDBaseDocument : public IUnknown
{
	com_call InitNamedData() = 0;
	com_call GetPrivateNamedData(IUnknown** ppunkNamedData) = 0;

	com_call GetNamedDataFieldsSection( BSTR* pbstrSection ) = 0;
	com_call IsDBaseObject( IUnknown* punkObj, BOOL* pbDBaseObject, GUID* pGuidDBaseParent, BOOL* pbEmpty ) = 0;

	//active query	
	com_call SetActiveQuery( IUnknown* punk ) = 0;
	com_call GetActiveQuery( IUnknown** ppunk ) = 0;
	

	//event listener
	com_call RegisterDBaseListener( IUnknown* punk ) = 0;
	com_call UnRegisterDBaseListener( IUnknown* punk ) = 0;
	com_call FireEvent( BSTR bstrEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, 
		BSTR bstrTableName, BSTR bstrFieldName, tagVARIANT var ) = 0;

	//Active image
	com_call SetActiveImage( BSTR bstrName ) = 0;
	com_call GetActiveImage( BSTR* pbstrName ) = 0;

	//is read only?
	com_call IsReadOnly( BOOL* pbReadOnly ) = 0;
};
interface IDBaseDocument2 : public IDBaseDocument
{
	com_call IsReadDeleteOnly( BOOL* pbReadOnly ) = 0;
	com_call SetReadDeleteOnly( BOOL pbReadOnly ) = 0;
};
interface IDBConnection : public IUnknown
{
	com_call LoadAccessDBase( BSTR bstrMDBFilePath) = 0;
	com_call OpenConnectionFromString( BSTR bstrConnection ) = 0;	
	com_call CreateAdvancedConnection( ) = 0;

	com_call CloseConnection() = 0;
	com_call OpenConnection() = 0;
	
	com_call GetConnection( IUnknown** ppUnkConnection ) = 0;

	com_call ExecuteSQL( BSTR bstrSQL, IUnknown** ppUnkRecordset ) = 0;	

	com_call GetMDBFileName( BSTR* bstrFileName ) = 0;
};

interface IDBaseStruct : public IUnknown
{
	///////////////////////////////////
	//Temp table info array state
	///////////////////////////////////
	com_call IsDirtyTableInfo( BOOL* pbDirty ) = 0;
	com_call RefreshTableInfo( BOOL bForceRefresh ) = 0;

	///////////////////////////////////
	//TTable type info
	///////////////////////////////////	

	//Table
	com_call GetTableCount( int* pnCount ) = 0;
	com_call GetTableName( int nIndex, BSTR* pbstrName ) = 0;	

	com_call GetFieldCount( BSTR bstrTable, int* pnFieldsCount ) = 0;
	com_call GetFieldName( BSTR bstrTable, int nFieldIndex, BSTR* pbstrFieldName ) = 0;	

	//Fields	
	com_call GetFieldInfo( BSTR bstrTable, BSTR bstrField, 
		BSTR* pbstrUserName/*CBN*/, short* pnFieldType/*CBN*/,
		BSTR* pbstrPrimaryTable/*CBN*/, BSTR* pbstrPrimaryFieldName/*CBN*/,
		BOOL* pbRequiredValue/*CBN*/, BOOL* pbDefaultValue/*CBN*/,
		BSTR* pbstrDefaultValue/*CBN*/, BSTR* pbstrVTObjectType/*CBN*/
		) = 0; 
	// /*CBN*/ - can be null

	
	com_call AddTable( BSTR bstrTableName ) = 0;
	com_call DeleteTable( BSTR bstrTableName ) = 0;
	com_call AddField( BSTR bstrTableName, BSTR bstrFieldName, short nFieldType,
		BSTR bstrPrimaryTable, BSTR bstrPrimaryFieldName
		) = 0;

	com_call SetFieldInfo( BSTR bstrTableName, BSTR bstrFieldName, BSTR bstrUserName,
		BOOL bRequiredValue, BOOL bDefaultValue,
		BSTR bstrDefaultValue, BSTR bstrVTObjectType
		) = 0;

	com_call DeleteField( BSTR bstrTableName, BSTR bstrFieldName ) = 0;

};

interface IDBaseFilterProvider : public IUnknown
{
	com_call ReBuildFilterChooserList() = 0;	
	com_call OnFilterChooserChange( HWND hWnd ) = 0;
	com_call OnAddFilterChooser( HWND hWnd ) = 0;
	com_call OnRemoveFilterChooser( HWND hWnd ) = 0;
};

interface IDBLockInfo : public IUnknown
{
	com_call SetLockInfo(void* li)=0;
	com_call GetLockInfoPtr(void** li)=0;
};

interface IDBaseFilterHolder : public IUnknown
{

	com_call GetFiltersCount( int* pnCount ) = 0;
	com_call GetFilter( int nFilterIndex, BSTR* pbstrName	) = 0;
	com_call AddFilter( BSTR bstrName ) = 0;
	com_call EditFilter( BSTR bsrtFilterName, BSTR bstrName ) = 0;
	com_call DeleteFilter( BSTR bsrtFilterName ) = 0;

	com_call GetFilterForSQL( BSTR bsrtFilterName, BSTR* pbstrSQL ) = 0;
	com_call GetFilterForOrganizer( BSTR bsrtFilterName, BSTR* pbstrOrganizer ) = 0;

	//Filter Condition
	com_call SetFilterInOrganizerMode( BOOL bSetFilterInOrganizerMode ) = 0;
	com_call GetFilterInOrganizerMode( BOOL* pbSetFilterInOrganizerMode ) = 0;

	com_call GetFilterConditionCount( BSTR bsrtFilterName, int* pnCount ) = 0;
	com_call GetFilterCondition( BSTR bsrtFilterName, int nConditionIndex,
						BSTR* pbstrTable, BSTR* pbstrField,
						BSTR* pbstrCondition, BSTR* pbstrValue,
						BSTR* pbstrValueTo, BOOL* pbANDunion,
						int* pnLeftBracketCount, int* pnRightBracketCount
						) = 0;
	com_call AddFilterCondition( BSTR bsrtFilterName, 
						BSTR bstrTable, BSTR bstrField,
						BSTR bstrCondition, BSTR bstrValue,
						BSTR bstrValueTo, BOOL bANDunion,
						int nLeftBracketCount, int nRightBracketCount							
						) = 0;

	com_call EditFilterCondition( BSTR bsrtFilterName, int nConditionIndex,
						BSTR bstrTable, BSTR bstrField,
						BSTR bstrCondition, BSTR bstrValue,
						BSTR bstrValueTo, BOOL bANDunion,
						int nLeftBracketCount, int nRightBracketCount							
						) = 0;

	com_call DeleteFilterCondition( BSTR bsrtFilterName, int nConditionIndex							
						) = 0;

	com_call GetFilterConditionForOrganizer( BSTR bsrtFilterName, int nConditionIndex,
						BSTR* pbstrCondition
						) = 0;

	com_call SetActiveFilter( BSTR  bstrFilterName ) = 0;
	com_call GetActiveFilter( BSTR* pbstrFilterName ) = 0;

	com_call SaveWorkingFilter() = 0;
	com_call GetWorkingFilterStatus( short* pnStatus) = 0;
	com_call IsValidFilter( BSTR bstrFilterName, BOOL bReportError, BOOL* pbValid ) = 0;

	com_call SetIsActiveFilterApply( BOOL bApply ) = 0;
	com_call GetIsActiveFilterApply( BOOL* pbApply ) = 0;
};  

enum BlankViewMode{
	bvmDesign = 0,
	bvmPreview = 1,
	//bvmFilter = 2,
	bvmNoInfo = 2
};

interface IBlankView : public IUnknown
{	
	com_call GetMode( short* pnBlankViewMode ) = 0;
	com_call SetMode( short nBlankViewMode ) = 0;
	com_call GetActiveAXData( IUnknown** ppunkAXData ) = 0;
};

/*
interface IDBaseView : public IUnknown
{
	com_call 
}
*/

#define szDBaseEventConnectionOpen			"ConnectionOpen"
#define szDBaseEventConnectionClose			"ConnectionClose"
#define szDBaseEventConnectionFailed		"ConnectionFailed"

#define szDBaseEventBeforInsertRecord		"BeforInsertRecord"
#define szDBaseEventAfterInsertRecord		"AfterInsertRecord"
#define szDBaseEventBeforEditRecord			"BeforEditRecord"
#define szDBaseEventAfterEditRecord			"AfterEditRecord"
#define szDBaseEventBeforDeleteRecord		"BeforDeleteRecord"
#define szDBaseEventAfterDeleteRecord		"AfterDeleteRecord"
#define szDBaseEventBeforUpdateRecord		"BeforUpdateRecord"
#define szDBaseEventAfterUpdateRecord		"AfterUpdateRecord"
#define szDBaseEventBeforCancelRecord		"BeforCancelRecord"
#define szDBaseEventAfterCancelRecord		"AfterCancelRecord"
#define szDBaseEventBeforNavigation			"BeforNavigation"
#define szDBaseEventMoveFirst				"MoveFirst"
#define szDBaseEventMoveLast				"MoveLast"
#define szDBaseEventMoveNext				"MoveNext"
#define szDBaseEventMovePrev				"MovePrev"
#define szDBaseEventAfterNavigation			"AfterNavigation"


#define szDBaseEventBeforQueryOpen			"BeforQueryOpen"
#define szDBaseEventAfterQueryOpen			"AfterQueryOpen"

#define szDBaseEventBeforQueryClose			"BeforQueryClose"
#define szDBaseEventAfterQueryClose			"AfterQueryClose"

#define szDBaseEventQueryOpenFailed			"szDBaseEventQueryOpenFailed"


#define szDBaseEventFieldChange				"FieldChange"

#define szDBaseEventNewTumbnailGenerate		"NewTumbnailGenerate"

#define szDBaseEventGridColumnChange		"GridColumnChange"


#define szDBaseEventGridOptionsChange		"GridOptionsChange"
#define szDBaseEventGalleryOptionsChange	"GalleryOptionsChange"

#define szDBaseEventActiveFieldLost			"ActiveFieldLost"
#define szDBaseEventActiveFieldSet			"ActiveFieldSet"

#define szDBaseEventCaptionChange			"CaptionChange"
#define szDBaseEventLockedRecord			"Lock"
#define szDBaseEventUnlockedRecord			"Unlock"

interface IDBaseListener : public IUnknown
{			 
	com_call OnNotify( BSTR bstrEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, 
		BSTR bstrTableName, BSTR bstrFieldName, tagVARIANT var ) = 0;
};

/*
enum QuerySupports
{
	qsuppportCanUpdate	= 0,
	qsuppportCanInsert	= 1,
	qsuppportCanDelete	= 2,
};
*/

enum QueryState
{
	qsEdit			= 0,
	qsBrowse		= 1,
	qsInsert		= 2,
	qsPrivateBrowse	= 3,
	qsClose			= 4,	
};

enum QueryScroll
{
	qscrollCanMoveFirst = 0,
	qscrollCanMoveLast = 1,
	qscrollCanMoveNext = 2,
	qscrollCanMovePrev = 3,
	
};

interface ISelectQuery :  public IUnknown
{
	com_call Open() = 0;
	com_call Close() = 0;
	com_call IsOpen(BOOL* pbOpen) = 0;
	///////////////////////////////////
	//Edit
	///////////////////////////////////	
	com_call Edit() = 0;
	com_call Update() = 0;
	com_call UpdateInteractive( BOOL bAskOnlyObjectChanges ) = 0;
	com_call Cancel() = 0;

	///////////////////////////////////
	//Info & state
	///////////////////////////////////	
	com_call GetSupports( short nCursorOptionEnum, BOOL* pbSucceeded ) = 0;
	com_call GetState( short* pnQueryState ) = 0;
	com_call IsBOF( BOOL* pBResult ) = 0;
	com_call IsEOF( BOOL* pBResult ) = 0;
	///////////////////////////////////
	//Manipulation
	///////////////////////////////////	
	com_call Insert() = 0;
	com_call Delete() = 0;

	///////////////////////////////////
	//Navigation
	///////////////////////////////////
	com_call MoveFirst() = 0;
	com_call MoveNext() = 0;
	com_call MovePrev() = 0;
	com_call MoveLast() = 0;	

	com_call GoToRecord( long nRecordNum ) = 0;

	///////////////////////////////////
	//Error
	///////////////////////////////////
	com_call GetLastError(BSTR* bstrError) = 0;	

	com_call IsAvailableField( BSTR bstrTable, BSTR bstrField, BOOL* pbAvailable) = 0;

	com_call SetValue( BSTR bstrName, const tagVARIANT var, BOOL* pbPlaceToRecordset ) = 0;
	com_call GetValue( BSTR bstrName, tagVARIANT *pvar ) = 0;
	com_call DeleteEntry( BSTR bstrName ) = 0;


	com_call GetRecordCount( long* pnRecordCount ) = 0;	
	com_call GetCurrentRecord( long* pnCurrentRecord ) = 0;
	com_call SetCurrentRecord( long nCurRecord, long* pnRealRecord ) = 0;

	//In this mode only simple data & tumbnail can read
	com_call EnterPrivateMode() = 0;
	com_call LeavePrivateMode( BOOL bResotoreBookmark, BOOL bRereadVisibleObject/* as image*/ ) = 0;	
	

	//Tumbnail support
	com_call GetTumbnail( void** ppTumbnail ) = 0;
	com_call GetTumbnailCaption( BSTR* pbstrCaption ) = 0;
	//com_call GenerateTumbnail( IUnknown* pUnkView, BSTR bstrTable, BSTR bstrField ) = 0;

	com_call GetTumbnailLocation( BSTR* pbstrTable, BSTR* pbstrField ) = 0;
	com_call SetTumbnailLocation( BSTR bstrTable, BSTR bstrField ) = 0;

	com_call GetTumbnailCaptionLocation( BSTR* pbstrTable, BSTR* pbstrField ) = 0;
	com_call SetTumbnailCaptionLocation( BSTR bstrTable, BSTR bstrField ) = 0;

	com_call GetTumbnailSize( SIZE* pSize ) = 0;
	com_call SetTumbnailSize( SIZE size ) = 0;	

	com_call GetTumbnailViewSize( SIZE* pSize ) = 0;
	com_call SetTumbnailViewSize( SIZE size ) = 0;	

	
	com_call SetCanProcessFind( BOOL bCanProcessFind ) = 0;	
	com_call GetCanProcessFind( BOOL* pbCanProcessFind ) = 0;	

	com_call SetFindSettings( BOOL bFindInField, BSTR bstrTable, BSTR bstrField,
								BOOL bMatchCase, BOOL bRegularExpression, BSTR bstrTextToFind ) = 0;

	com_call GetFindSettings( BOOL* pbFindInField, BSTR* pbstrTable, BSTR* pbstrField,
								BOOL* pbMatchCase, BOOL* pbRegularExpression, BSTR* pbstrTextToFind  ) = 0;

	com_call GetTumbnailBorderSize( SIZE* pSize ) = 0;
	com_call SetTumbnailBorderSize( SIZE size ) = 0;

	com_call IsDBaseObject( IUnknown* punkObj, BOOL* pbDBaseObject, GUID* pGuidDBaseParent, BOOL* pbEmpty ) = 0;	

	//sorting
	com_call GetSortFieldsCount( int* pnCount ) = 0;
	com_call AddSortField( BSTR bstrTable, BSTR bstrField, BOOL bAscending ) = 0;
	com_call GetSortField( int nIndex, BSTR* pbstrTable, BSTR* pbstrField, BOOL* pbAscending ) = 0;
	com_call RemoveSortFields( ) = 0;
	com_call SaveContextsState() = 0;
	com_call RestoreContextsState() = 0;

	//additional deletion
	com_call DeleteEx( BOOL bQuiet ) = 0;
};
interface ISelectQuery2: public ISelectQuery
{
	com_call AttachLockInfo(void*) = 0;
};

interface IDBGridView :  public IUnknown
{
	com_call SaveGridSettings() = 0;
};

interface IDBGalleryView : public IUnknown
{
	com_call Stub() = 0;
};

interface IDBFilterView : public IUnknown
{	
	com_call GetActiveField( BSTR* pbstrTable, BSTR* pbstrField ) = 0;
	com_call GetActiveFieldValue( BSTR* pbstrTable, BSTR* pbstrField, BSTR* pbstrValue ) = 0;
};

interface IDBFilterPage : public IUnknown
{
	com_call BuildAppearance( ) = 0;	
};

interface IDBChild : public IUnknown
{
	com_call SetParent( IUnknown* punkParent ) = 0;	
	com_call GetParent( IUnknown** ppunkParent ) = 0;	
};


interface IBlankForm : public IUnknown
{
	com_call GetControlsCount( BSTR bstr_table, BSTR bstr_field, long* plcount ) = 0;		
};

interface IQueryMisc : public IUnknown
{
	com_call GetGridFieldInfo( BSTR bstrTable, BSTR bstrField, int* pnWidth, int* pnPos ) = 0;
	com_call SetGridFieldInfo( BSTR bstrTable, BSTR bstrField, int nWidth, int nPos ) = 0;
	com_call EmptyGridFieldInfo() = 0;
};


declare_interface( IBlankForm, "290424E4-E3B1-4c5c-BC28-494048BA2936" );
declare_interface( IQueryObject, "E0A799B7-4EB5-462c-9D7F-3E2EF044CDDD" );
declare_interface( IDBControl, "8464D66F-FDB5-4cd1-8C72-470F0A01099A" );
declare_interface( IDBControl2, "F56D411D-29C4-4066-96D2-CD203E9F0E61" );
declare_interface( IDBObjectControl, "6233B7B6-77ED-4831-99AD-0D6C40416D7E" );
declare_interface( IBlankView, "BCE0F3C3-03E9-4208-B82C-5000DC62341E" );
declare_interface( IDBaseDocument, "B30D46E7-2675-484a-91E6-108A0D0F760A" );
declare_interface( IDBaseDocument2, "3549C2EE-01F4-40fb-A094-AA68BD1EBB2A" );
declare_interface( IDBaseListener, "EC81F8CC-BB0D-480e-8107-1BFA3B3B19F4" );
declare_interface( ISelectQuery, "FB1F168D-E7A8-4c87-8C24-E84E4A5BA76A" );
declare_interface( ISelectQuery2, "C1A2B040-14A3-4c04-834D-4499A24D3FF5" );
declare_interface( IDBGridView, "FB80F797-0FDF-4c13-ADB4-FA3C60E968FA" );
declare_interface( IDBGalleryView, "60A76A41-66B8-11d4-AF09-0000E8DF68C3" );
declare_interface( IDBFilterView, "E99C8FB0-80A9-4352-BF0D-B4310A6E4B37" );
declare_interface( IDBFilterPage, "729D3FD8-F302-4c1a-9F3E-4D469889DAFB" );
declare_interface( IDBChild, "DF93508E-0400-4a17-8353-A801DABCCE66" );
declare_interface( IDBaseFilterProvider, "4BA2648F-74E1-40af-8D76-4824EB929790" );
declare_interface( IDBaseStruct, "7847D690-18C9-42c8-ABFC-38109D0FA8A7" );
declare_interface( IDBConnection, "89FA771C-119B-42d1-BE12-840D55E9CA62" );
declare_interface( IDBaseFilterHolder, "4C43A74C-E1CF-41a6-9C26-398B4D9F20F1" );
declare_interface( IQueryMisc, "971BA7D8-FE02-4d4a-B22C-12685DD6C639" );
declare_interface( IDBLockInfo, "3C2920A4-A0D9-4c00-BD6C-8A832FA7E8F6");


#endif//__dbaseint_h__
