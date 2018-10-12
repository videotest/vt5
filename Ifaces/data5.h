#ifndef __Data5_h__
#define __Data5_h__

//#include "utils.h"
#include "defs.h"
//struct __POSITION {};
typedef __POSITION* POSITION;
typedef POSITION TPOS;
typedef LONG_PTR LPOS;

#if defined(_WIN64)
#define LONG_PTR_VAL llVal
#define VT_LONG_PTR VT_I8
#else
#define LONG_PTR_VAL lVal
#define VT_LONG_PTR VT_I4
#endif

#define szContaintersStatesSection "\\ContainersStates"

	enum NotifyCodes
	{
		ncAddObject,		// add punkNew
		ncRemoveObject,		// remove punkNew
		ncReplaceObject,	// replace punkNew to punkOld
		ncRescan,			// rescan (0, 0)
		ncChangeBase,		// change base key for given punkNew object (lparam contains ptr on old key)
		ncChangeKey,		// change object's key (lparam contains ptr on old key)
		ncEmpty,			// empty context
		ncUpdateSelection,	// update selection in context viewer
		ncEnterFilter,		// filter entered
		ncPassFilter,		// filter ended
		ncCancelFilter,		// filter canceled
		ncFilterUndo,		// filter undo ended
		ncFilterRedo,		// filter redo ended
		ncSyncWithMaster,	// syncronize with document context (after load) punkNew - document context
	};

//DocumentManager
	//file filter1 
	//file filter2
	//DocumentTemplate
		
		//Document1
			//Document data1
			//Document data2
		//


enum ChildNotifyCodes
{
	cncAdd,
	cncRemove,
	cncChange,
	cncReset,
	cncActivate,
	cncDeactivate,
	cncAddParameter,
	cncRemoveParameter,
	cncActivateParameter,
	cncResetParameters,
	cncChangeParameter,
	cncMoveParameter,
};


enum FileFilterPropFlags
{
	ffpfTextAfterProp = 1<<0,
	ffpfNeedBrackets = 1<<1
};

	
	
interface IFileDataObject : public IUnknown
{
	com_call InitNew() = 0;
	com_call LoadFile( BSTR bstrFileName ) = 0;
	com_call SaveFile( BSTR bstrFileName ) = 0;

	com_call IsDirty( BOOL *pbDirty ) = 0;
};

interface IFileDataObject2 : public IFileDataObject
{
	com_call LoadTextFile( BSTR bstrFileName ) = 0;
	com_call SaveTextFile( BSTR bstrFileName ) = 0;

	com_call SetModifiedFlag( BOOL bSet ) = 0;
};

interface IFileDataObject3 : public IFileDataObject2
{
	// bstrFileName - незащищенная часть файла (читается, если не удалось прочитать защищенный)
	// bstrFileName.protected - защищенный файл
	// bstrFileName.protected.chk - подпись защищенного файла
	// (если *.protected не будет - прочитаем, как LoadFile)
	com_call LoadProtectedFile( BSTR bstrFileName ) = 0;
	com_call SaveProtectedFile( BSTR bstrFileName ) = 0;
};

interface IFileDataObject4 : public IFileDataObject3
{
	// Saving of the files in some format (.avi, .vti) isn't supported.
	com_call IsFileFormatReadonly( BOOL *pbIsReadOnly ) = 0;
	// Messages can be different for the different formats.
	com_call ShowSaveNotSupportedMessage() = 0;
};

interface IFileFilter : public IUnknown
{
	com_call AttachNamedData( IUnknown *punkNamedData, IUnknown *punkUsedContext ) = 0;
	com_call MatchFileType( BSTR bstr, DWORD *pdwConfidence ) = 0;
	com_call GetFilterParams( BSTR *pbstrFilter, BSTR *pbstrFilterName, BSTR *pbstrDocTemplates ) = 0;
};

interface IFileFilter2 : public IFileFilter
{
	com_call GetObjectTypesCount( int *pnCount ) = 0;
	com_call GetObjectType( int nType, BSTR *pbstrType ) = 0;

	com_call AttachNamedObject( IUnknown *punkObject ) = 0;

	com_call GetCreatedObjectCount( int *piCount ) = 0;
	com_call GetCreatedObject( int nObject, IUnknown **ppunkObject ) = 0;
};

interface IFileFilter3 : public IFileFilter2
{
	com_call GetPropertyByName(BSTR bstrName, VARIANT *pvarVal) = 0;
	com_call SetPropertyByName(BSTR bstrName, VARIANT varVal) = 0;
	com_call GetPropertyCount( int *piCount ) = 0;
	com_call GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx) = 0;
	com_call SetPropertyByNum(int nNum, VARIANT varVal) = 0;
	com_call ExecSettingsDialog(DWORD dwFlags) = 0;
};

interface INamedData : public IUnknown
{
	com_call SetupSection( BSTR bstrSectionName ) = 0;

	com_call SetValue( BSTR bstrName, const tagVARIANT var ) = 0;
	com_call GetValue( BSTR bstrName, tagVARIANT *pvar ) = 0;

	com_call GetEntriesCount( long *piCount ) = 0;
	com_call GetEntryName( long idx, BSTR *pbstrName ) = 0;

	com_call RenameEntry( BSTR bstrOldName, BSTR bstrName  ) = 0;
	com_call DeleteEntry( BSTR bstrName ) = 0;

	com_call GetActiveContext( IUnknown **punk ) = 0; 
	com_call SetActiveContext( IUnknown *punk ) = 0;

	com_call AddContext( IUnknown *punk ) = 0;
	com_call RemoveContext( IUnknown *punk ) = 0;

	com_call NameExists( BSTR bstrName, long* Exists ) = 0;
	com_call GetObject( BSTR bstrName, BSTR bstrType, IUnknown **punk ) = 0;

	com_call NotifyContexts( DWORD dwNotifyCode, IUnknown *punkNew, IUnknown *punkOld, GUID* dwData) = 0;
	com_call EnableBinaryStore( BOOL bBinary ) = 0;
	
	com_call GetCurrentSection( BSTR* pbstrSection ) = 0;

	com_call GetBaseGroupCount(int * pnCount) = 0;
	com_call GetBaseGroupFirstPos(long * plPos) = 0;
	com_call GetNextBaseGroup(GUID * pKey, long * plPos) = 0;
	com_call GetIsBaseGroup(GUID * pKey, BOOL * pbBase) = 0;
	com_call GetBaseGroupBaseObject(GUID * pKey, IUnknown ** ppunkObject) = 0;

	com_call GetBaseGroupObjectsCount(GUID * pKey, int * pnCount) = 0;
	com_call GetBaseGroupObjectFirstPos(GUID * pKey, long * plPos) = 0;
	com_call GetBaseGroupNextObject(GUID * pKey, long * plPos, IUnknown ** ppunkObject) = 0;
	com_call SetEmptySection( BSTR* bstrSectionName ) = 0;
};

enum NamedDataLogKind
{
	ndlkRead,
	ndlkWrite
};

interface INamedDataLogger : public IUnknown
{	// ведет лог действий с Named Data
	// на данный момент лог идет в Named Data - т.е. в случае лога записи
	// в нем сохраняются последние значения всех измененных ключей,
	// в случае лога чтения - последние значения всех прочитанных
    com_call SetLog(long nKind, IUnknown *punkLog) = 0;
    com_call GetLog(long nKind, IUnknown **ppunkLog) = 0;
};

interface IDataTypeManager  : public IUnknown
{
	com_call GetTypesCount( long *pnCount ) = 0;
	com_call GetType( long index, BSTR *pbstrType ) = 0;

	com_call GetObjectFirstPosition( long nType, LONG_PTR *plpos ) = 0;
	com_call GetNextObject( long nType, LONG_PTR *plpos, IUnknown **ppunkObj ) = 0;
};



enum GetObjectInfoFlags
{
	oif_SmallIcon = 1<<0,
};

interface INamedDataInfo : public IUnknown
{
	com_call CreateObject( IUnknown **ppunk ) = 0;
	com_call GetContainerType( BSTR *pbstrTypeName ) = 0;
	com_call GetProgID( BSTR *pbstrProgID ) = 0;
	com_call GetObjectIcon(HICON* phIcon, DWORD	dwFlags ) = 0;
};

// [vanek] - 24.12.2003
enum ContainerStates
{
	cntsEnabled = 1<<0,
};

interface INamedDataInfo2 : public INamedDataInfo
{
    com_call GetContainerState( DWORD *pdwState ) = 0;
};


interface INamedDataObject : public IUnknown
{
	com_call GetType( BSTR *pbstrType ) = 0;
	com_call GetDataInfo( IUnknown **ppunk ) = 0;	
	com_call IsModified( BOOL *pbModified ) = 0;
	com_call SetModifiedFlag( BOOL bSet ) = 0;
	com_call SetTypeInfo( INamedDataInfo *pinfo ) = 0;
};

/*enum AttachParentFlag
{
	apfNone = 0,
	apfCopyToParent = 1,
	apfAttachParentData = 2,
	apfNotifyNamedData = 4,
	aofDefaultFill = 8
};*/

enum NamedObjectFlags
{
	nofNormal			= 0,
	nofHasData			= 1,
	nofStoreByParent	= 2,	//store object pointer in parent, not in NameData
	nofHasHost			= 4,
	nofCantSendTo		= 8,
	nofDefineParent		= 16,	//parent is only container, it will be removed if remove child
};

enum StoreDataFlags
{
	sdfAttachParentData = 1<<0,	//local -> parent
	sdfCopyParentData	= 1<<1,	//local = parent
	sdfCopyLocalData	= 1<<2,	//parent = local
	sdfClean			= 1<<3,	//local = 0
	//used only in SetParent
	apfNotifyNamedData  = 1<<15,
	//compatiblity
	apfAttachParentData = sdfAttachParentData,
	apfCopyToParent = sdfCopyLocalData,
	apfDefaultFill = sdfClean,
	apfNone = 0,
};

interface INamedDataObject2 : public INamedDataObject
{
	com_call CreateChild( IUnknown **ppunkChild ) = 0;
	com_call GetParent( IUnknown **ppunkParent ) = 0;
	com_call SetParent( IUnknown *punkParent, DWORD dwFlags /*AttachParentFlag*/ ) = 0;
	com_call StoreData( DWORD dwDirection/*StoreDataFlags*/ ) = 0;
//for internal usage - it is called only from framework
	// ВНИМАНИЕ!!! Для удаления дочернего объекта вызывать SetParent( 0, ... )!
	com_call RemoveChild( IUnknown *punkChild ) = 0;	
	com_call AddChild( IUnknown *punkChild ) = 0;

	com_call GetChildsCount( long *plCount ) = 0;
	com_call GetFirstChildPosition(POSITION *plPos) = 0;
	com_call GetNextChild(POSITION *plPos, IUnknown **ppunkChild) = 0;
	com_call GetLastChildPosition(POSITION *plPos) = 0;
	com_call GetPrevChild(POSITION *plPos, IUnknown **ppunkChild) = 0;

	com_call AttachData( IUnknown *punkNamedData ) = 0;	//called from NamedData
	com_call GetObjectFlags( DWORD *pdwObjectFlags ) = 0;

	com_call SetActiveChild(POSITION lPos) = 0;
	com_call GetActiveChild(POSITION *plPos) = 0;
	com_call SetUsedFlag( BOOL bSet ) = 0;				//set flag object used as part of another object, doesn't 
														//require to store object in NamedData
	com_call SetHostedFlag( BOOL bSet ) = 0;			//set flag if object controlled by host
	com_call GetData(IUnknown **ppunkNamedData ) = 0;

	com_call GetObjectPosInParent(POSITION *plPos) = 0; // return object position in parent's object child list
	com_call SetObjectPosInParent(POSITION lPos) = 0;

	com_call GetChildPos(IUnknown *punkChild, POSITION *plPos) = 0;
	com_call InitAttachedData() = 0;
// base and source key functionality
	com_call GetBaseKey(GUID * pBaseKey) = 0;
	com_call SetBaseKey(GUID * pBaseKey) = 0;
	com_call IsBaseObject(BOOL * pbFlag) = 0;
};

interface INamedDataObjectProps : public IUnknown
{
	com_call CreatePropSheet() = 0;
};

enum ActivateObjectFlags
{
	aofActivateDepended = 1<<0,
	aofSkipIfAlreadyActive = 1<<1,
};

interface IDataContext : public IUnknown
{
	com_call GetActiveObject( BSTR bstrType, IUnknown **ppunkObj ) = 0;
	com_call SetActiveObject( BSTR bstrType, IUnknown *punkObj, DWORD dwFlags ) = 0;	

	com_call GetData( IUnknown **ppunk ) = 0;
	com_call AttachData( IUnknown *punkNamedData ) = 0;

	com_call Notify( long cod, IUnknown *punkNew, IUnknown *punkOld, GUID* dwData ) = 0;

	com_call LockUpdate(BOOL bLock, BOOL bNeedUpdate) = 0;
	com_call GetLockUpdate(BOOL * pbLock) = 0;

};

interface IDataContext2 : public IDataContext
{
// object types
	com_call GetObjectTypeCount(long * plCount) = 0;
	com_call GetObjectTypeName(LONG_PTR lPos, BSTR * pbstrType) = 0;

// type enables
	com_call GetTypeEnable(BSTR bstrType, BOOL * pbEnable) = 0;
// types enable
	com_call GetTypesEnable(BSTR * pbstrTypes) = 0;

// objects 
	com_call GetObjectCount(BSTR bstrType, long * plCount) = 0;
	com_call GetFirstObjectPos(BSTR bstrType, LONG_PTR * plPos) = 0;
	com_call GetNextObject(BSTR bstrType, LONG_PTR * plPos, IUnknown ** ppunkObject) = 0;
	com_call GetLastObjectPos(BSTR bstrType, LONG_PTR * plPos) = 0;
	com_call GetPrevObject(BSTR bstrType, LONG_PTR * plPos, IUnknown ** ppunkObject) = 0;

	com_call GetObjectPos(BSTR bstrType, IUnknown * punkObject, LONG_PTR * plPos) = 0;
	com_call GetObjectEnable(IUnknown * punkObject, BOOL * pbFlag) = 0;

// child objects
	com_call GetChildrenCount(BSTR bstrType, IUnknown * punkParent, long * plCount) = 0;
	com_call GetFirstChildPos(BSTR bstrType, IUnknown * punkParent, long * plPos) = 0;
	com_call GetNextChild(BSTR bstrType, IUnknown * punkParent, long * plPos, IUnknown ** ppunkChild) = 0;
	com_call GetLastChildPos(BSTR bstrType, IUnknown * punkParent, long * plPos) = 0;
	com_call GetPrevChild(BSTR bstrType, IUnknown * punkParent, long * plPos, IUnknown ** ppunkChild) = 0;
	com_call GetChildPos(BSTR bstrType, IUnknown * punkParent, IUnknown * punkChild, long * plPos) = 0;

// selected objects
	com_call GetObjectSelect(IUnknown * punkObject, BOOL * pbFlag) = 0;
	com_call SetObjectSelect(IUnknown * punkObject, BOOL bFlag) = 0;

	com_call GetSelectedCount(BSTR bstrType, long * pnCount) = 0;
	com_call GetFirstSelectedPos(BSTR bstrType, LONG_PTR * pnPos) = 0;
	com_call GetNextSelected(BSTR bstrType, LONG_PTR * pnPos, IUnknown ** ppunkObject) = 0;
	com_call GetLastSelectedPos(BSTR bstrType, LONG_PTR * plPos) = 0;
	com_call GetPrevSelected(BSTR bstrType, LONG_PTR * plPos, IUnknown ** ppunkObject) = 0;
	com_call GetSelectedObjectNumber(BSTR bstrType, IUnknown * punkObject, long * pnNumber) = 0;

	com_call UnselectAll(BSTR bstrType) = 0;

// last active object
	com_call GetLastActiveObject(BSTR bstrTypes, IUnknown ** ppunkObject) = 0;

	// synchronize
	com_call SynchronizeWithContext(IUnknown * punkContext) = 0;

	// serialize
	com_call StoreToString(BSTR * pbstrString) = 0;
	com_call LoadFromString(BSTR bstrString) = 0;

	com_call StoreToArchive(IUnknown * punkOleStream) = 0;
	com_call LoadFromArchive(IUnknown * punkOleStream) = 0;
};

interface IDataContext3 : public IDataContext2
{
};

// vanek BT:618
interface IDataContextMisc: public IUnknown
{
    com_call	SetLockObjectActivate( BOOL bActivate ) = 0;
	com_call	GetLockObjectActivate( BOOL *pbActivate ) = 0;
};

interface IDataContextSync : public IUnknown
{
	com_call GetObjManagerCount(int * pnCount) = 0;
	com_call GetObjManager(int nPos, BSTR * pbstrType, long * plTime) = 0;
	
	com_call GetObjManagerTime(BSTR bstrType, long * plTime) = 0;
	com_call SetObjManagerTime(BSTR bstrType, long lTime) = 0;

	com_call GetObjManagerTime(int nPos, long * plTime) = 0;
	com_call SetObjManagerTime(int nPos, long lTime) = 0;

	com_call GetDataKey(GUID * pDataKey) = 0;
};

interface IDataTypeInfoManager : public IUnknown
{
	com_call GetTypesCount( long *pnCount ) = 0;
	com_call GetType( long index, BSTR *pbstrType ) = 0;
	com_call GetTypeInfo( BSTR bstrType, INamedDataInfo **ppunkObj ) = 0; // create info object with 'Type' type
	com_call GetTypeInfo( long index, INamedDataInfo **ppunkObj ) = 0; // create info object with 'Type' type
};

enum SerializeFlags
{
	sf_ImageCompressed = 1<<0,
	sf_OnlyMaskOfImage = 1<<1,
	sf_DetachParent = 1<<2,
	sf_DontCheckSignature = 1<<3,
	sf_DontCheckSignatureInBig = 1<<4
};

struct SerializeParams
{
	unsigned flags;
	INamedData			*punkNamedData;
	INamedDataObject2	*punkCurrentParent;
};

interface ISerializableObject : public IUnknown
{
	com_call Load( IStream *pStream, SerializeParams *pparams ) = 0;
	com_call Store( IStream *pStream, SerializeParams *pparams ) = 0;
};

interface ITextObject : public IUnknown
{
	com_call GetText( BSTR *pbstr ) = 0;
	com_call SetText( BSTR bstr ) = 0;
};


declare_interface( IDataContextMisc,		"2B1C20C6-0CCB-45f9-8BD8-1D19AAFEE28B" )	// [vanek] BT:618	
declare_interface( IDataContextSync,		"5517CD74-BBA4-4c08-93F2-0FA5B8F835EF" )
declare_interface( IDataContext3,			"D5C85A96-E88C-41b4-B3D8-DD48BCE02D4C" )
declare_interface( IDataContext2,			"C5435701-5174-11d3-A624-0090275995FE" )
declare_interface( IDataContext,			"1E71E160-38EE-11d3-87C5-0000B493FF1B" )
declare_interface( INamedDataObjectProps,	"76977801-5A09-11d3-8816-0000B493FF1B" )
declare_interface( IDataTypeInfoManager,	"F7AA87D1-39F2-11d3-87C6-0000B493FF1B" )
declare_interface( INamedDataInfo,			"1E71E161-38EE-11d3-87C5-0000B493FF1B" )
declare_interface( INamedDataInfo2,			"D10CC476-3260-4b64-9BE1-B3352D67F11C" )	// [vanek] 24.12.2003
declare_interface( IFileDataObject,			"BD04FF41-232B-11d3-A5D7-0000B493FF1B" )
declare_interface( IFileDataObject2,		"5BD76CE1-39E8-11d3-A603-0090275995FE" )
declare_interface( IFileDataObject3,		"3E1590F0-DBB3-4176-A192-C2B11C4F0483" )
declare_interface( IFileFilter,				"BD04FF42-232B-11d3-A5D7-0000B493FF1B" )
declare_interface( INamedData,				"BD04FF43-232B-11d3-A5D7-0000B493FF1B" )
declare_interface( IDataTypeManager,		"BD04FF44-232B-11d3-A5D7-0000B493FF1B" )
declare_interface( ISerializableObject,		"BD04FF45-232B-11d3-A5D7-0000B493FF1B" )
declare_interface( ITextObject,				"BD04FF46-232B-11d3-A5D7-0000B493FF1B" )
declare_interface( INamedDataObject,		"BD04FF47-232B-11d3-A5D7-0000B493FF1B" )
declare_interface( INamedDataObject2,		"8747E2D1-53B2-11d3-A626-0090275995FE" )
declare_interface( IFileFilter2,			"3F92AD91-5AF5-11d3-A635-0090275995FE" )
declare_interface( IFileFilter3,			"2AC57C00-7721-11d3-A501-0000B493A187" )
declare_interface( INamedDataLogger,        "F4D95991-41A7-490b-8F1A-C6910F1D1C43" )
declare_interface( IFileDataObject4,			"2CA1502E-4BB0-4cb3-9B64-BFF17B7D8A2B" )


#endif //__Data5_h__