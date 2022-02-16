#ifndef __utils_h__
#define __utils_h__

#include "defs.h"
#include "object5.h"
#include "data5.h"
#include "core5.h"
#include "misc5.h"
#include "measure5.h"

enum AjustViewFlags	
{
	avf_Force = 1,
};

class CStreamEx;

//CArchive &operator<<(CArchive& ar, GuidKey &g);
//CArchive &operator>>(CArchive& ar, GuidKey &g);

inline int Bright ( int b, int g, int r )	//функция для получения яркости
{
	return (r*60+g*118+b*22)/200;
}

// Замена для некоторых стандартных функций,
// которые в либе MS реализованы с ошибкой
// (для русских букв возвращают мусор, да еще и с ассертом)
inline int __cdecl isdigit_ru(int c)
{ return isdigit(c & 255); }

inline int __cdecl isalpha_ru(int c)
{ return isalpha(c & 255); }

inline int __cdecl isalnum_ru(int c)
{ return isalnum(c & 255); }

inline int __cdecl isspace_ru(int c)
{ return isspace(c & 255); }

inline int __cdecl ispunct_ru(int c)
{ return ispunct(c & 255); }


class std_dll CListLockUpdate
{
public:
	CListLockUpdate( IUnknown	*punkList, bool bNotify = true )
	{
		m_ptrList = punkList;
		m_ptrList->GetObjectUpdateLock( &m_bOldLock );
		m_ptrList->LockObjectUpdate( TRUE );
		m_bNotify = bNotify;

	}
	~CListLockUpdate()
	{
		m_ptrList->LockObjectUpdate( m_bOldLock );

		if( !m_bOldLock && m_bNotify )
			m_ptrList->UpdateObject( 0 );
	}
protected:
	IDataObjectListPtr	m_ptrList;
	BOOL				m_bOldLock;
	bool				m_bNotify;
};

class std_dll CContextLockUpdate
{
public:
	CContextLockUpdate(IUnknown	* punkContext, bool bUpdate = true)
	{
		m_sptrContext = punkContext;
		m_bNeedUpdate = bUpdate;
		if (m_sptrContext != 0)
		{
			m_sptrContext->GetLockUpdate(&m_bOldLock);
			m_sptrContext->LockUpdate(TRUE, false);
		}
	}
	~CContextLockUpdate()
	{
		if (m_sptrContext != 0)
			m_sptrContext->LockUpdate(m_bOldLock, m_bNeedUpdate);
	}
protected:
	IDataContextPtr	m_sptrContext;
	BOOL			m_bOldLock;
	BOOL			m_bNeedUpdate;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//logging
class std_dll CLogFile 
{
protected:
	CStringArray	m_arr;
	CStdioFile		m_file;
	CString			m_sFileName;
	long			m_lLoggingLevel;
	bool			m_bInit;
public:
	CLogFile();
	~CLogFile();
public:
	void Exception( CException *pe );
	void Add( const char *szFunc );
	void RemoveLast();
	void DumpStack();
public:	//low level
	void Write( const char *szCaller, const char *szFunc = 0, int iLevel = 0 );
	void WriteRaw( const char *sz );

	void Init();
};



std_dll CLogFile &operator << (CLogFile &log, double f);
std_dll CLogFile &operator << (CLogFile &log, const char *sz);
std_dll CLogFile &operator << (CLogFile &log, int i);
std_dll CLogFile &operator << (CLogFile &log, const CRect rc);

//return reference to the logging component
std_dll CLogFile &GetLogFile();

#define LOCAL_RESOURCE	CCommonResource	__r_provider;
class std_dll CCommonResource
{
public:
	CCommonResource();
	~CCommonResource();

protected:
	HINSTANCE m_hInstOld;
};


class CImplBase : public IUnknown
{
protected:
	virtual IUnknown *Unknown()
	{	return GetCmdTarget()->GetControllingUnknown();	}
	virtual AFX_MODULE_STATE *GetModuleState() = 0;
	virtual CCmdTarget *GetCmdTarget() = 0;
};

inline bool CheckInterface( IUnknown *punkn, const GUID guid )
{
	if( punkn )
	{
		IUnknown	*ptest = 0;

		if( punkn->QueryInterface( guid, (void**)&ptest ) ==S_OK )
		{
			ptest->Release();
			return true;
		}
		return false;
	}
	else
		return false;
}

inline BOOL TraceIfFailed( HRESULT hRes )
{
	if( !SUCCEEDED(hRes) )
	{
		char	sz[200];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, hRes,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), sz, 200, NULL );
		TRACE( "Interface call error: %s\n", sz );
		GetLogFile()<<sz;
	}

	return SUCCEEDED(hRes);
}

#ifdef _DEBUG
#define CALL(a) TraceIfFailed(a)
#else
#define CALL(a) SUCCEEDED(a)
#endif


inline HRESULT call( HRESULT hRes )
{
	try{
		if( !SUCCEEDED(hRes) )
		{
			char	sz[200];
			FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, hRes,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), sz, 200, NULL );
			TRACE( "Interface call error: %s\n", sz );
			GetLogFile()<<sz;
		}

		return hRes;
	}
	catch( CException *pe )
	{
		char	sz[200];

		if( pe->GetErrorMessage( sz, 200 ) )
		{
			TRACE( sz );
			TRACE( "\n" );
		}
		else
			TRACE( "Unknown exception (CException) while call interface member\n" );


		pe->Delete();

		return E_POINTER;
	}
	catch( ... )
	{
		TRACE( "Unknown exception while call interface member\n" );	
		return E_POINTER;
	}
}

inline long _AjustValue( double f, double fVal )
{	
	return long(fVal + .5 );
}

		 
class std_dll CLogStruct
{
public:
	CLogStruct( const char *szMainClass, const char *szNestedClass, const char *szMember )
	{
		CString	sFuncNameToLog;

		if( szNestedClass )
			sFuncNameToLog.Format( "%s::X%s::%s()", szMainClass, szNestedClass, szMember );
		else
			sFuncNameToLog.Format( "%s::%s()", szMainClass, szMember );

		GetLogFile().Add( sFuncNameToLog );

	}

	~CLogStruct()
	{
		GetLogFile().RemoveLast();
	}
};							   

#define _try_nested( mainclass, localclass, method )							\
	METHOD_PROLOGUE_EX(mainclass, localclass)									\
	CLogStruct	ls( #mainclass, #localclass, #method );							\
	try{
#define _try_nested_base( mainclass, localclass, method )						\
	METHOD_PROLOGUE_BASE(mainclass, localclass)									\
	CLogStruct	ls( #mainclass, #localclass, #method );							\
	try{																		 

#define _catch_nested															\
	}																			\
	catch( CException *pe )														\
	{																			\
		GetLogFile().Exception( pe );											\
		pe->Delete();															\
		return E_INVALIDARG;													\
	}																			\
	catch( ... )																\
	{																			\
		GetLogFile().Exception( 0 );											\
		return E_POINTER;														\
	}

#define _catch_nested_noreturn													\
	}																			\
	catch( CException *pe )														\
	{																			\
		GetLogFile().Exception( pe );											\
		pe->Delete();															\
	}																			\
	catch( ... )																\
	{																			\
		GetLogFile().Exception( 0 );											\
	}

#define _try( mainclass, method )												\
	CLogStruct	ls( #mainclass, 0, #method );									\
	bool	bExceptionFault = false;											\
	try{

#define _catch																	\
	}																			\
	catch( CException *pe )														\
	{																			\
		GetLogFile().Exception( pe );											\
		bExceptionFault = true;													\
		pe->Delete();															\
	}																			\
	catch( ... )																\
	{																			\
		GetLogFile().Exception( 0 );											\
		bExceptionFault = true;													\
	}																			\
	if( bExceptionFault )														

#define _catch_report															\
	}																			\
	catch( CException *pe )														\
	{																			\
		GetLogFile().Exception( pe );													\
		bExceptionFault = true;													\
		pe->ReportError();														\
		pe->Delete();															\
	}																			\
	catch( ... )																\
	{																			\
		GetLogFile().Exception( 0 );													\
		bExceptionFault = true;													\
	}																			\
	if( bExceptionFault )														

#define VALUE( variable )														\
	g_log<<"Value "<<#variable<<" = "<<variable<<"\n";

#pragma warning( disable : 4390 )

#ifndef _DEBUG
inline void _ReportCounter( IUnknown *punk )
{	punk;	}
#else

inline void _ReportCounter( IUnknown *punk )
{	
	if( !punk )
		return;
	punk->AddRef();	
	int	iNo = punk->Release();

	TRACE( "RefCounter = %d\n", iNo );
}
#endif //
#define REPORT_COUNTER( unk );

//CStringArrayEx
class std_dll CStringArrayEx : public CStringArray
{
public:
	CStringArrayEx();

	void SkipComments( const char chComment = '\'' );
	INT_PTR	Find( const char *szString, int iPos = 0 );
public:
	void ReadFile( const char *szFileName );
	void WriteFile( const char *szFileName );

	void ReadFile( CStdioFile *pfile );
	void WriteFile( CStdioFile *pfile );

	int			LockSection( const char *pszSection );
	void		UnlockSection();
	const char *FindEntry( const char *pszEntry, int idxStart = -1, int idxEnd = -1 );
	CString		GetTotalText();

	CString operator[](int nIndex) const;
	CString& operator[](int nIndex);

	INT_PTR		_GetStartPos()	{return (m_nStartSection==-1)?0:m_nStartSection;}
	INT_PTR		_GetEndPos()	{return (m_nEndSection==-1)?GetSize():m_nEndSection;}
protected:
	INT_PTR	m_nStartSection, m_nEndSection;
};


template <class _tY> class pointer
{
public:
	pointer( int nSize )
	{	alloc( nSize );	}
	pointer( _tY *ptr )
	{		m_ptr = ptr;	m_owner = ptr != 0;	}

	~pointer()	
	{	release();	}

	void alloc( int nSize )	
	{	m_ptr = new _tY[nSize];	m_owner = true;	}
	void release()	
	{	if( m_owner )delete m_ptr;	m_owner = false;	}
	_tY *get()	
	{	return m_ptr;	}

	operator _tY*()
	{return m_ptr;}

	operator bool()
	{return m_ptr!=0;}
	bool operator ==( int n )
	{if( n == 0 )return m_ptr==0;else return m_ptr!=0;}
	bool operator !=( int n )
	{if( n == 0 )return m_ptr!=0;else return m_ptr==0;}
protected:
	bool	m_owner;
	_tY		*m_ptr;
};


#define PNC_UNDEFINED	-1
#define PNC_ADD			0
#define PNC_REMOVE		1
#define PNC_CHANGE		2
#define PNC_ACTIVATE	3
#define PNC_MOVE		4

#include <AfxTempl.h>
template<>std_dll UINT AFXAPI HashKey(GUID& key);
//{
//	return key.Data1;
//}
//template<> std_dll UINT AFXAPI HashKey<GUID> (GUID & key);
//template<> std_dll UINT AFXAPI HashKey<GuidKey> (GuidKey & key);

// notify param data
struct std_dll TParamNotifyData
{
	UINT		uCode;			// operation code
	long		lParamKey;		// param key (ID)
	IUnknown *	punkMgr;		// parameter manager from which this notification is outgoing
};

// create and delete TParamNotifyData struct using std IMalloc
std_dll TParamNotifyData * CreatePNData(UINT uCode = PNC_UNDEFINED, long lKey = -1, IUnknown * punk = 0);
std_dll void DeletePNData(TParamNotifyData * pData);



std_dll void FireEvent( IUnknown *punkCtrl, const char *pszEvent = 0, IUnknown *punkFrom = 0, IUnknown *punkData = 0, void *pdata = 0, long cbSize = 0 );
std_dll void FireEventNotify( IUnknown *punkCtrl, const char *pszEvent, IUnknown *punkFrom, IUnknown *punkData, long nCode );
std_dll void FireEvent2(IUnknown *punkCtrl, const char *pszEvent = 0, IUnknown *punkFrom = 0, IUnknown *punkData = 0, void *pdata = 0, long cbSize = 0, IUnknown * punkEvListener = 0);

//find the component with given interface in component's tree
std_dll IUnknown *_GetOtherComponent( IUnknown *punkThis, const IID iid );
//find the component with given interface and name in component's tree
std_dll IUnknown * FindComponentByName(IUnknown *punkThis, const IID iid, LPCTSTR szName);
//return temporaly or permanent window object, associated with given unknown.
std_dll CWnd *GetWindowFromUnknown( IUnknown *punk );

//create the full pathname from filename (add path to executable module)
std_dll CString MakeAppPathName( const char *szFileName );
//create the full filename from filename and filepath
std_dll CString MakeFillFileName( const char *szPath, const char *szFileName );

//initialize global pointer to the application unknonw
//extern "C" std_dll void InitAppUnknown( IUnknown *punkApp );
//return the application's unknown pointer
extern "C" std_dll IApplication *GetAppUnknown( bool bAddRef = false );

//create an instance of specified type
extern "C" _declspec(dllexport) IUnknown *CreateTypedObject( const char *szObjectType );
//extract object kind from Unknown
std_dll CString GetObjectKind( IUnknown *punkObj );
//extract object name from Unknown
std_dll CString GetObjectName( IUnknown *punkObj );
//return key by request interface INumberedObect. If key can't be obtained, return -1 (NOKEY)
std_dll GuidKey GetObjectKey( IUnknown *punk );
//assign new name to the object
std_dll bool SetObjectName( IUnknown *punkObj, const char *szNewName );
//find object in namedData by name and return it
std_dll IUnknown *GetObjectByName( IUnknown *punkData, const char *szName, const char *szType = 0 );
// create unique name for object in NamedData;
std_dll CString CreateUniqueName(IUnknown *punkData, const char *szObjectBase );
std_dll HRESULT GenerateUniqueNameForObject( IUnknown *punkObject, IUnknown *punkData );
//find child object in namedData by name and return it
std_dll IUnknown *FindChildByName(IUnknown *punkParent, const char *szName);
//find object in namedData by name and return it
std_dll IUnknown *FindObjectByName(IUnknown *punkData, const char *szName);

//find object in namedData by key and return it
std_dll IUnknown *GetObjectByKey( IUnknown *punkData, GuidKey lKey );
//extended version of previous function(find object in namedData by key and return it, also try to find in each object children)
std_dll IUnknown* GetObjectByKeyEx(IUnknown* punkData, GuidKey lKey);
//find object from parent's object children by key and return it if any and 0 else
std_dll IUnknown* GetChildObjectByKey(IUnknown* punkParent, GuidKey lKey);
//delete the specified object from NamedData
std_dll bool DeleteObject( IUnknown *punkData, IUnknown *punkObjToDelete );
//rename the specified object
std_dll bool RenameObject( IUnknown *punkData, IUnknown *punkObjToRename, const char *szName );
//return path to the object in NamedData
std_dll CString GetObjectPath( IUnknown *punkData, IUnknown *punkObj );
//return path to the object in NamedData
std_dll CString GetObjectPath( IUnknown *punkData, IUnknown *punkObj, const char *sz );
//return the object from document. Find in all avaible documents
std_dll IUnknown *FindObjectByKey( GuidKey lKey );	
//return the object from document. Find in all avaible documents
std_dll IUnknown *FindObjectByKeyEx(GuidKey lKey);	

// Create stream in memory. Uses ADO stream ==> fast
std_dll IStreamPtr CreateMemoryStream();

//clone the data object
std_dll IUnknown *CloneObject( IUnknown *punk );
//if object has parent, return true
std_dll bool HasParentObject( IUnknown *punk );
// get object path  by name only
CString GetObjectPathByName(IUnknown * pNamedData, LPCTSTR szpath, LPCTSTR szObjName);
// get objects' properties
std_dll bool ObjectProperties(IUnknown *punk);
//store the object data to hglobal
std_dll HGLOBAL StoreObjectToHandle( IUnknown *punkDataObject, CPoint point = CPoint( 0, 0 ) );
//store multi frame to hglobal
std_dll HGLOBAL StoreMultiFrameObjectsToHandle( IUnknown *punkMF, CPoint point, long* pnSize, SerializeParams *pparams );
//restore the object data from hglobal
std_dll IUnknown *RestoreObjectFromHandle( HGLOBAL hObj, CPoint *ppoint = 0 );
//restore the multi frame from hglobal
std_dll void RestoreMultiFrameFromHandle( HGLOBAL hObj, IUnknown* punkMF,  CPoint *ppoint, SerializeParams *pparams );
//return the active object from data context
std_dll IUnknown *GetActiveObjectFromContext( IUnknown *punkContext, const char *szObjectType );
//return the active object from data context one of supported types
std_dll IUnknown* GetActiveObjectFromContextEx(IUnknown *punkContext);
//return the active parent object from context
std_dll IUnknown *GetActiveParentObjectFromContext( IUnknown *punkContext, const char *szObjectType );

//return the active object from document
std_dll IUnknown *GetActiveObjectFromDocument( IUnknown *punkDoc, const char *szObjectType );

//load the data object from the archive
std_dll IUnknown *LoadDataObjectFromArchive( CStreamEx &ar, SerializeParams *pparams );
//store the specified data object to the archive
std_dll void StoreDataObjectToArchive( CStreamEx &ar, IUnknown *punkObject, SerializeParams *pparams );
//return true oif object stored by parent
std_dll bool IsObjectStoredByParent( IUnknown *punkChild );




// check filter support object's type
std_dll bool CheckFilterSupportType(LPCTSTR szType, IUnknown * punk);
// get filter unknown what can work object of 'Type' type
std_dll IUnknown* GetFilterByType(LPCTSTR szType);
// return fileFilter that may be used for open/saeve with requested file
std_dll IUnknown* GetFilterByFile(LPCTSTR szFileName);


// get object name extention from full path
std_dll CString GetFileExtention(LPCTSTR szPath);
// Get Object name from full path
std_dll CString GetObjectNameFromPath(LPCTSTR szPath);



std_dll bool GetDataDump(LPCTSTR szFileName, IUnknown *punkData);

//execute the specified script
enum ExecuteScriptFlags
{
	esfDebugMode	= 1,
	esfBreakOnStart	= 1<<1
};

std_dll bool ExecuteScript( BSTR bstrScript, const char* psz_name = 0, bool bFromActionManager = false, GuidKey key = GuidKey(), DWORD dwFlags = 0);
//execute the specified action
std_dll bool ExecuteAction( const char *szActionName, const char *szActionparam = 0, DWORD dwFlags = 0 );
//terminater the scripts with given thread id
std_dll void TerminateScripts( const GuidKey lKeyToTerminate );
// return action name by Resource ID
std_dll CString GetActionNameByResID(UINT nResID);

//return the document by document key
std_dll IUnknown *GetDocByKey( GuidKey lDocKey );
//return typeinfo by name
std_dll IUnknown *GetTypeByName( const char *szType, bool bAddRef );
//return the active dataobject of document
std_dll IUnknown *GetActiveDataObject( IUnknown *punkDoc );
//return the clipboard format identifier
std_dll UINT GetClipboardFormat();
//return the type of object that supports specified clipboard format  
std_dll CString GetObjectTypeByClipboardFormat(UINT nFormat);

std_dll POSITION GetFirstClipboardFormatPos();

std_dll void GetClipboardFormatByPos(POSITION& pos, UINT& nFormat, CString& strType);



//copy named data
std_dll void CopyNamedData( IUnknown *punkDest, IUnknown *punkSource );
//get pointer value from NamedData. You should free it using "delete"
std_dll byte *GetValuePtr( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long *plSize/* = 0*/ );
//set pointer value to NamedData
std_dll void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, byte *ptr, long lSize );
//get double value from NamedData
extern "C" _declspec(dllexport) double GetValueDouble(IUnknown *punkDoc, const char *pszSection, const char *pszEntry, double fDefault = 0.);
//get integer value from NamedData
extern "C" _declspec(dllexport) long GetValueInt( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lDefault = 0 );
//get integer value from NamedData
extern "C" _declspec(dllexport) __int64 GetValueInt64( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, __int64 lDefault = 0 );
//get string value from NamedData
std_dll CString GetValueString( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszDefault = 0 );
//get any value from NamedData
std_dll _variant_t GetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const VARIANT varDefault );
//set double value to NamedData
std_dll void SetValue(IUnknown *punkDoc, const char *pszSection, const char *pszEntry, double fValue);
//set integer value to NamedData
std_dll void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lValue );
//set __int64 value to NamedData
std_dll void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, __int64 lValue );
//set string value to NamedData
std_dll void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszValue );
//set any value to NamedData
extern "C" _declspec(dllexport) void SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const VARIANT varDefault );
//return entries counter in given section
std_dll long GetEntriesCount( IUnknown *punkDoc, const char *pszSection );
//return the entry name in given section
std_dll CString GetEntryName( IUnknown *punkDoc, const char *pszSection, long nPos );
//delete an entry or section
std_dll bool DeleteEntry( IUnknown *punkDoc, const char *pszEntry );
//set the color value to NamedData
std_dll void SetValueColor( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, COLORREF cr );
//get the color value from NamedData
std_dll COLORREF GetValueColor( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, COLORREF crDefault );

//set the colors value from NamedData for fhases extract
std_dll void SetValuePhase( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, COLORREF crBegin,  COLORREF crEnd, COLORREF crChange);
//get the colors value from NamedData for fhases extract
std_dll void GetValuePhase( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, COLORREF& crBegin,  COLORREF& crEnd, COLORREF& crChange );

/*
// Load calibrX & calibrY & measure unit(if any)
std_dll void GetCalibrValues(double * pfCalibrX, double * pfCalibrY, long *plType = 0);
// Save calibrX & calibrY & measure unit(if any)
std_dll void SetCalibrValues(double * pfCalibrX, double * pfCalibrY, long *plType = 0);
// Load OffsetX & OffsetY
std_dll void GetOffsetValues(double * pfOffsetX, double * pfOffsetY);
// Save OffsetX & OffsetY
std_dll void SetOffsetValues(double * pfOffsetX, double * pfOffsetY);
*/

//get zoom of zoom site
std_dll double GetZoom( IUnknown  *punk );
//get scroll position
std_dll CPoint GetScrollPos( IUnknown  *punk );

//coordinate mapping - screen to client
std_dll CRect ConvertToClient( IUnknown *punkSSite, CRect rc );
std_dll CPoint ConvertToClient( IUnknown *punkSSite, CPoint pt );
std_dll CSize ConvertToClient( IUnknown *punkSSite, CSize size );
//coordinate mapping - client to screen
std_dll CRect ConvertToWindow( IUnknown *punkSSite, CRect rc );
std_dll CPoint ConvertToWindow( IUnknown *punkSSite, CPoint pt );
std_dll CSize ConvertToWindow( IUnknown *punkSSite, CSize size );

// create copy of menu by HMENU
std_dll HMENU CopyMenu(HMENU hSrc);
std_dll bool AppendMenu(HMENU & hDst, HMENU hSrc);
std_dll HMENU CopyMenuSkipSubMenu(HMENU hSrc, UINT uiSkip, UINT& nPosition);

//action registration
//return true if dynamic action registration enabled
std_dll bool IsDynamicActionRegistrationEnabled();
//manage  the action registration state
std_dll void EnableActionRegistartion( bool bEnable );
//find the better view for given object type
std_dll CString FindMostMatchView( const char *szType, DWORD &dwMatch );
//return Unknown of color convertor by it name 
std_dll IUnknown* GetCCByName(const char* pszName, bool bfirst_if_fail = true);
//it's execute advanced file open/save dialog
std_dll BOOL ExecuteFileDialog(BOOL bOpenFileDialog, OPENFILENAME& ofn, const char *pszSection = 0 );

//return the property page by title
std_dll IUnknown *GetPropertyPageByTitle( const char *szPageTitle, long *plPageNo = 0 );
//return the propertysheet dockbar unknown 
std_dll IUnknown *GetPropertySheet();

//return the ColorChooser dockbar unknown 
std_dll IUnknown *GetColorChooser();

//determinate is this CC name valid (we have such CC)
std_dll bool IsCCValid(const char* pszName);

/////////////////////////////////////////////////////////////////////////////////////////
//mouse controller helpers 
std_dll IUnknown	*GetMouseController();						//return the mouse controller pointer
std_dll void RegisterMouseCtrlTarget( IUnknown *punkView );		//register view as target
std_dll void UnRegisterMouseCtrlTarget( IUnknown *punkView );	//unregister view as target
std_dll void DispatchMouseToCtrl( IUnknown *punkView, POINT point );	//dispatch wm_mousemove to controller
std_dll void RepaintMouseCtrl( IUnknown *punkView, HDC hDC, bool bDraw = true );	//draw the controller from wm_paint
std_dll void CheckMouseInWindow( IUnknown *punkView );					//check on wm_timer
std_dll void InstallController( IUnknown *punkCtrl );					//install the draw handler
std_dll void UnInstallController( IUnknown *punkCtrl );					//remove the draw handler


std_dll void _MyCheckMemory( bool bAlloc = true );

std_dll GuidKey GetParentKey( IUnknown *punkO );	//return the object flags
std_dll DWORD GetObjectFlags( IUnknown *punkO );//return the object parent key
std_dll void UpdateDataObject( IUnknown *punkD, IUnknown *punkO );//send the update hint for the object


//restore ActiveX control state from persist stream, specified by DataObject
std_dll void RestoreContainerFromDataObject( IUnknown *punkCtrlObject, IUnknown *punkCtrl );
//store ActiveX control state to persist stream, specified by DataObject
std_dll void StoreContainerToDataObject( IUnknown *punkCtrlObject, IUnknown *punkCtrl );

std_dll CString GetDocTypesString(IUnknown *punkDoc);
std_dll CString GetDocTypesStringFromView(IUnknown *punkView);
//return the document from view
std_dll IUnknown *GetDocumentFromView( IUnknown *punkView );


// notify parent about child selection changed
std_dll void NotifyObjectSelect(IUnknown *punk, bool bSelect = false);
std_dll void NotifyObjectSelect(IUnknown *punk, IUnknown * punkEvListener, bool bSelect = false);

//work with classes list
/*
std_dll COLORREF	GetClassColor( long lClass );
std_dll long GetClassName( long lClass, char *psz, size_t cb );
std_dll long GetObjectClass( ICalcObject *pcalc );
std_dll void SetObjectClass( ICalcObject *pcalc, long lClass );
std_dll COLORREF GetObjectClassColor(ICalcObject *pcalc );
std_dll CString GetObjectClassName( ICalcObject *pcalc );
*/


// return param name by its type (for manual measure params)
std_dll CString GetParamNameByParamType(DWORD dwParamType);
// get name of calibr units 
std_dll CString GetCalibrUnitName(LPCTSTR szGroup, long lParamKey, long lMeasUnit = 0);

//init the NamedData attached to DataObject 
std_dll void InitAttachedData( IUnknown *punkObject );

//generate the unique name for data argument
std_dll CString	GenerateNameForArgument( CString strArgName, IUnknown *punkDocData );

//////////////////////////////////////////////////////////////////////////////////////////////////////
// helper function that is provide help to restore relation between prev object key and new object key
// after named data loaded from file
/*std_dll void InitKeyCache();
std_dll void ReleaseKeyCache();
std_dll long GetKeyFromCache(long lOldKey);*/


std_dll bool ChangeBaseKeyForDocument(IUnknown * punkDoc, GuidKey OldKey, GuidKey NewKey);
std_dll void _VarChangeType( VARIANT &var, long lType, VARIANT *psrc = 0 );

//return the SyncManager component unknown
std_dll IUnknown *GetSyncManager();

std_dll void SetBaseObject( IUnknown *punk, IUnknown *punkBase, IUnknown *punkDocument );
std_dll CString GetDirectory( const char *pszRecord, const char *pszFolder );
std_dll byte *TextToMem( CStringArrayEx &sa, int &nPos, long &nLength );
std_dll void MemToText( byte *ptr, long nLength, CStringArrayEx &sa );

//find the type aliase
std_dll CString GetTypeAliase( const char *pszType );
//find the type using aliase
std_dll CString FindTypeByAliase( const char *pszAliase );
//AjustViewForObject - change view typa according object, show object
std_dll void AjustViewForObject( IUnknown *punkView, IUnknown *punkObject, DWORD dwFlags = 0);


extern "C" std_dll void GetRegCompanyName(char * szCompanyName, long * plSize);
extern "C" std_dll void GetRegAppName(char * szAppName, long * plSize);
extern "C" std_dll void GetRegProgKey(char * szRegKey, long * plSize);

std_dll bool LoadVTClass(CWinApp * pApp);


std_dll bool InitMeasGroups(IUnknown* punkList);
std_dll void CalcObject(IUnknown* punkCalc, IUnknown* punkImage, bool bSetParams, ICompManager *pGroupManager = 0 );

#pragma pack(push, guard_component_info, 1)
struct std_dll __GuardComponentInfo
{
	int		nSize;
	DWORD_PTR	lpos ;
	BSTR	bstrName;
	GUID	guid;
	bool	bIsAction;
};
#pragma pack(pop, guard_component_info)

#define ACTION_NAME_PREFIX "VideoTesT5Action."

std_dll bool NeedComponentInfo(REFCLSID guid1, REFIID guid2);
std_dll HRESULT GetComponentInfo(LPVOID * ppv);
//internal ussage
std_dll void SetSourceObject( IUnknown *punkObject );
std_dll IUnknown *FindSourceObject( IUnknown *punkData, IUnknown *punkObject );


std_dll INamedDataInfo	*FindTypeInfo( BSTR	bstrType );

#ifndef std_dll
#ifdef _COMMON_LIB
#define std_dll /*extern "C"*/ __declspec(dllexport)
#else
#define std_dll /*extern "C" */ __declspec(dllimport)
#endif //_COMMON_DLL
#endif //std_dll

/////////////////////////////////////////////////////////////////////////////
//Serialize thumbnail. 	
std_dll bool ThumbnailStore( CArchive& ar, BYTE* pbi );
std_dll bool ThumbnailLoad( CArchive& ar, BYTE** ppbi );

/////////////////////////////////////////////////////////////////////////////
// Draw thumbnail
std_dll bool ThumbnailDraw( BYTE* pbi, ::CDC* pDC, CRect rcTarget, IUnknown* punkDataObject );

/////////////////////////////////////////////////////////////////////////////
// Get Render Object from Data Object
std_dll IUnknown* GetRenderObject( IUnknown* pUnkDataObject );

/////////////////////////////////////////////////////////////////////////////
// Get Thumbnail Size
std_dll DWORD ThumbnailGetSize( BYTE* pbi );

/////////////////////////////////////////////////////////////////////////////
// Allocate Thumbnail Size
std_dll bool ThumbnailAllocate( BYTE** ppbi, CSize sizeThumbnail );

/////////////////////////////////////////////////////////////////////////////
// Free thumbnail
std_dll bool  ThumbnailFree( BYTE* pbi );	

/////////////////////////////////////////////////////////////////////////////
std_dll CSize ThumbnailGetRatioSize( CSize sizeThumbnail, CSize sizeImage );



//call in action Initialize() function for set active view that compatible with action
enum AppropriateViewType
{
	avtSource	= 1<<0,
	avtObject	= 1<<1,
	avtBinary	= 1<<2,
};
std_dll bool SetAppropriateView(IUnknown* punkTarget, DWORD dwType);

std_dll void _OleTraceLeaks();
std_dll void _OleLockAppDbg( CCmdTarget *ptarget, int nLine, const char *pszFile );
std_dll void _OleUnlockAppDbg( CCmdTarget *ptarget );

#ifndef _DEBUG
#define _OleLockApp( ptarget )		AfxOleLockApp()
#define _OleUnlockApp( ptarget )	AfxOleUnlockApp()
#else
#define _OleLockApp( ptarget )		_OleLockAppDbg( ptarget, __LINE__, __FILE__ )
#define _OleUnlockApp( ptarget )	_OleUnlockAppDbg( ptarget )
#endif


std_dll IUnknown *GetActiveMeasureObject( INamedDataObject2Ptr	ptrNamedObject );


std_dll GUID GetInternalGuid( GUID guidExternal );
std_dll GUID GetExternalGuid( GUID guidInternal );
std_dll IUnknown* FindMeasGroup( GUID clsidGroup );	//defined in utils.h


struct std_dll SyncViewData
{
	DWORD		dwStructSize;
	IUnknown*	punkObject2Activate;
	bool		bCanChangeViewType;
	bool		bSyncAllView;
	bool		bOnlyViewInterfaceMatch;
	IID			iidViewMatch;
};

std_dll bool SyncSplitterView( IUnknown* punkViewFocused, SyncViewData* pData );

inline char *local_filename( char *psz_dest, const char *psz_src, int cb )
{
	GetModuleFileName( 0, psz_dest, cb );
	strcpy( strrchr( psz_dest, '\\' )+1, psz_src );
	return psz_dest;
}

//get error descr
CString GetWin32ErrorDescr( DWORD dw_error );
std_dll BOOL IsGoodExt( CString &strExt, BOOL bOpenFileDialog, LPCTSTR lpstrSubSec = NULL );
std_dll CString MakeAllFilesStringForDocument(LPCTSTR lpstrDocName);


class CLock
{
	bool *m_pbLock;
public:
	CLock(bool *pbLock)
	{
		m_pbLock = pbLock;
		*m_pbLock = true;
	}
	~CLock()
	{
		*m_pbLock = false;
	}
};

#define DO_LOCK() static bool s_bLock11 = false;\
	if (s_bLock11) return;\
	CLock Lock(&s_bLock11);

#define DO_LOCK_R(r) static bool s_bLock11 = false;\
	if (s_bLock11) return r;\
	CLock Lock(&s_bLock11);


#endif //__utils_h__