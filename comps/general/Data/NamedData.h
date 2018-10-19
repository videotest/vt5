#if !defined(AFX_NamedData_H__ADBA6C04_1D68_11D3_A5D0_0000B493FF1B__INCLUDED_)
#define AFX_NamedData_H__ADBA6C04_1D68_11D3_A5D0_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CNamedData.h : header file
//

#include "StreamEx.h"
#include "docmiscint.h"

#include "\vt5\awin\misc_map.h"

/////////////////////////////////////////////////////////////////////////////
// CNamedData command target
#include "ObjectMgr.h"
#include "dbase.h"

#include <AfxTempl.h>

struct CBaseGroup
{
	CBaseGroup()
	{
	}

	~CBaseGroup()
	{
	}
	CList<INamedDataObject2*, INamedDataObject2*>	list;
	POSITION	Find(INamedDataObject2 * pObject);
	POSITION	FindBase(GuidKey & key);
};

typedef CMap <GUID, GUID&, CBaseGroup*, CBaseGroup*> CBaseGroupMap;

#define TYPE_FLAG_SINGLEOBJECT	1

//Named Data component class
class CNamedData : public CCmdTargetEx,
					public CFileDataObjectImpl,	//base implementation for IFileDataObject interface
					public CSerializableObjectImpl
{
protected:
	DECLARE_DYNCREATE(CNamedData)
	GUARD_DECLARE_OLECREATE(CNamedData)
			
	ENABLE_MULTYINTERFACE();					//

	CNamedData();           // protected constructor used by dynamic creation

// Attributes
public:

public:
	class XEntry		//data entry class
	{
	public:
		_variant_t	m_variant;		//data object. It should be variant, represent as string, 
									//or IUnknown (VT_IUNKNOWN), who has INamedDataObject interface
		CString		m_strEntry;		//Entry name. 
		CPtrArray	m_arrChildEntries;	//array of child entries. They can be accessed 
									//by specific functions
		XEntry		*m_pparent;		//pointer to parent entry (or NULL if entry is root)
		CNamedData	*m_pData;		//pointer to NamedData object - for Context notification etc
	public:
									//Constructor. zero parent for root entry
		XEntry( CNamedData *pParent, XEntry *pparent, const char *pszEntryName = 0 );
		virtual ~XEntry();					

		void DeInit();				//clean-up value and reset entry name
		bool Serialize( CStreamEx &ar );	//serialize from/to archive

		bool IsEmpty(){return m_variant.vt == VT_EMPTY;}

	public:
		void SetValue( const _variant_t var );	//setup value to entry. 
						//Special processing for IUnknown * value - use works with types
						
	public:
						//interface for child entries access
		int GetEntriesCount();
		XEntry *GetEntry( int ipos );

		XEntry *Find( const char *pszEntryName );
						//find entry function. This path is relative from current entry
						//good call is Find( "data\\data1" );
						//wrong call is Find( "\\data\\data1" );
		CString GetPath();
						//return full path to this entry from root
	public:
		_list_map_t<XEntry*,const char*, cmp_string>	m_map;
	};

					
	class XType			//data type class - we have change it
	{
	public:
		CPtrList	m_objects;			//objects in current NamedData
		int			m_iNamesCounter;	//internal counter for name
		_bstr_t		m_bstrTypeName;
		_bstr_t		m_bstrTypeUserName;
		CNamedData	*m_pData;		//pointer to NamedData object - for Context notification etc
		DWORD		flags;
	public:
		
		XType( CNamedData	*pdata, IUnknown *punk );
		~XType();
						//modify entry name. 
		void RegisterObject( IUnknown *punk );	//register object in object's list
		void UnRegisterObject( IUnknown *punk );//unregister object

												//ajust object name (and path);
		void InitObject( IUnknown *punk, CString &strObjectPath );	

		long GetObjectsCount();				//returns count of objects give type in given named data
		POSITION GetFirstObjectPosition();	//returns first object position
		IUnknown *GetNextObject( POSITION &pos );	//returns object in given position. don't call AddRef()!!!
												
	};
public:

	class XNotifyLock
	{
	protected:
		bool		m_bOldState;
		bool		m_bOldCheckChild;
		CNamedData	*m_pData;
	public:
		XNotifyLock( CNamedData	*pData = 0)
		{
			m_pData = pData;
			if (m_pData)
			{
				m_bOldState = m_pData->EnableContextNotification( false );
				m_bOldCheckChild = m_pData->EnableChildsNotification( false );
			}
			else
			{
				m_bOldState = true;
				m_bOldCheckChild = true;
			}
		}
		~XNotifyLock()
		{
			if (m_pData)
			{
				m_pData->EnableContextNotification( m_bOldState );
				m_pData->EnableChildsNotification( m_bOldCheckChild );
			}
		}
/*		void Lock(CNamedData * pData)
		{
			m_pData = pData;
			if (m_pData)
			{
				m_pData->EnableContextNotification( false );
				m_pData->EnableChildsNotification( false );
			}
		}
		void Unlock(CNamedData * pData)
		{
			m_pData = pData;
			if (m_pData)
			{
				m_pData->EnableContextNotification( true );
				m_pData->EnableChildsNotification( true );
			}
		}
*/	};

	IUnknown *GetDocument();
	void Lock();
	void Unlock();
	//enable/disable context notifications. return old state
	bool EnableContextNotification( bool bEnable = true );
	bool EnableChildsNotification( bool bEnable = true );
	//notify contexts, if enabled
	void NotifyContexts(NotifyCodes nc, IUnknown * punkNew = 0, IUnknown * punkOld = 0, GUID* lParam = 0);
	// set active context and sync namedData's context with it
	bool SetActiveContext(IUnknown *punk);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNamedData)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CNamedData();

	void DeInit();

	// Generated message map functions
	//{{AFX_MSG(CNamedData)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	afx_msg void _SetValue(LPCTSTR Entry, const VARIANT FAR& Value);

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CNamedData)
	afx_msg void SetValue(LPCTSTR Entry, const VARIANT FAR& Value);
	afx_msg VARIANT GetValue(LPCTSTR Entry);
	afx_msg void SetSection(LPCTSTR Section);
	afx_msg BSTR GetSection();
	afx_msg long GetEntriesCount();
	afx_msg BSTR GetEntryName(long index);
	afx_msg long GetTypesCount();
	afx_msg BSTR GetTypeName(long index);
	afx_msg VARIANT GetDefValue(LPCTSTR Entry, const VARIANT FAR& DefVal);
	afx_msg void RemoveValue(LPCTSTR szPath);
	afx_msg BOOL DispWriteTextFile(LPCTSTR fullPath);
	afx_msg BOOL DispWriteFile(LPCTSTR fullPath);
	afx_msg BOOL DispReadFile(LPCTSTR fullPath);
	afx_msg BOOL ExportEntry(LPCTSTR pszEntry, LPCTSTR pszFileName, long dwFlags);
	afx_msg BOOL ImportEntry(LPCTSTR pszEntry, LPCTSTR pszFileName, long dwFlags);
	afx_msg LONG_PTR GetFirstObjectPos(long ltype);
	afx_msg LPDISPATCH GetNextObject(long ltype, VARIANT FAR* var_pos);
	afx_msg BOOL CopyNamedData(LPDISPATCH pDispSrc, LPCTSTR pszSrcEntry, LPCTSTR pszTargetEntry, long dwFlags);
	afx_msg void SetEmptySection( BSTR* bstrSectionName);
	afx_msg BOOL DispWriteProtectedFile(LPCTSTR fullPath);
	afx_msg BOOL DispReadProtectedFile(LPCTSTR fullPath);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	BEGIN_INTERFACE_PART(Data, INamedData)
		com_call SetupSection( BSTR bstrSectionName );
		com_call SetValue( BSTR bstrName, const tagVARIANT var );
		com_call GetValue( BSTR bstrName, tagVARIANT *pvar );
		com_call GetEntriesCount( long *piCount );
		com_call GetEntryName( long idx, BSTR *pbstrName );
		com_call RenameEntry( BSTR bstrName, BSTR bstrOldName );
		com_call DeleteEntry( BSTR bstrName );
		com_call GetActiveContext( IUnknown **punk ); 
		com_call SetActiveContext( IUnknown *punk ); 
		com_call AddContext( IUnknown *punk ); 
		com_call RemoveContext( IUnknown *punk ); 
		com_call NameExists( BSTR bstrName, long* Exists );
		com_call GetObject(	BSTR bstrName, BSTR bstrType, IUnknown **punk );
		com_call NotifyContexts( DWORD dwNotifyCode, IUnknown *punkNew, IUnknown *punkOld, GUID* dwData);
		com_call EnableBinaryStore( BOOL bBinary );
		com_call GetCurrentSection( BSTR* pbstrSection );
// base objects
		com_call GetBaseGroupCount(int * pnCount);
		com_call GetBaseGroupFirstPos(TPOS *plPos);
		com_call GetNextBaseGroup(GUID * pKey, TPOS *plPos);
		com_call GetIsBaseGroup(GUID * pKey, BOOL * pbBase);
		com_call GetBaseGroupBaseObject(GUID * pKey, IUnknown ** ppunkObject);

		com_call GetBaseGroupObjectsCount(GUID * pKey, int * pnCount);
		com_call GetBaseGroupObjectFirstPos(GUID * pKey, TPOS *plPos);
		com_call GetBaseGroupNextObject(GUID * pKey, TPOS *plPos, IUnknown ** ppunkObject);
		com_call SetEmptySection( BSTR* bstrSectionName );
	END_INTERFACE_PART(Data);
	BEGIN_INTERFACE_PART(DataLogger, INamedDataLogger)
		com_call SetLog(long nKind, IUnknown *punkLog);
		com_call GetLog(long nKind, IUnknown **ppunkLog);
	END_INTERFACE_PART(DataLogger);

	BEGIN_INTERFACE_PART(Types, IDataTypeManager)
		com_call GetTypesCount( long *pnCount );
		com_call GetType( long index, BSTR *pbstrType );
		com_call GetObjectFirstPosition( long nType, LONG_PTR *plpos );
		com_call GetNextObject( long nType, LONG_PTR *plpos, IUnknown **ppunkObj );
	END_INTERFACE_PART(Types);
	BEGIN_INTERFACE_PART(DocP, IDocForNamedData)
		com_call AttachExternalDocument( IUnknown *punkND );
	END_INTERFACE_PART(DocP);
protected:
	//setup the current section to the specified path. 
	void SetupCurrentSection( const char *pszPath );
	//get entry by name. Possible use with relative or absolute paths ("\\Datas\Data1" or "Data1" )
	XEntry *GetEntry( const char *pszName );
protected:
	bool DoReadFile( const char *pszFileName, bool bSilent = false );
	//function is called from LoadFile 
	virtual bool BeforeReadFile();
	virtual bool ReadFile( const char *pszFileName );
	virtual bool AfterReadFile();

	bool DoWriteFile( const char *pszFileName, BOOL bUnchecked );
	//function is called from StoreFile
	virtual bool BeforeWriteFile();
	virtual bool WriteFile( const char *pszFileName );
	virtual bool AfterWriteFile();

	virtual bool InitNew();

	// restore relation between classes keys and object's class_key 
	bool RestoreRelationObjectsAndClasses();
	// restore base and source keys for all objects after file load
	bool RestoreBaseSourceKeys();

	// data context load/save
	bool LoadContextFromText(CStringArray & sa);
	bool StoreContextToText(CStringArray & sa);
	bool LoadContextFromArchive(IStream* pStream);
	bool StoreContextToArchive(IStream* pStream);


//overrided virtuals. Works with text file format - phrase CStringArrayEx contents
	//load from CStringArrayEx 
	virtual bool LoadFromText( CStringArrayEx &sa );
	//store to  CStringArrayEx 
	virtual bool StoreToText( CStringArrayEx &sa );
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );
public:
	//ask NamedData objects is data modified
	virtual bool IsModified();
	//reset modified flag for every object
	virtual void SetModifiedFlag( bool bSet = true );
public:
		//get name from Unknown and find type by name
	XType	*GetType( IUnknown *punk );
		//get type entry by type name
	XType	*GetType( const char *psz );
		//get type by index
	XType	*GetType( long nPos );

	void _DeleteChilds( IUnknown *punk );	//delete all childs of this data object from NamedData
	void _InsertChilds( IUnknown *punk );	//insert all childs of this data object to NamedData
protected:
	XEntry	*m_pEntryRoot;	//root of entries tree
	XEntry	*m_pEntryCurrent;	//current entry. Should 

	CPtrArray		m_ptrsTypes;
	CBaseGroupMap	m_mapBase;

	CContextManager m_Contexts; // contexts
	bool			m_bContextNotificationEnabled;
	bool			m_bEnableCheckChilds;
	IUnknown		*m_punkExternalDoc;

protected: // для INamedDataLogger
	IUnknownPtr		m_ptrLogRead;
	IUnknownPtr		m_ptrLogWrite;

/*
public:
	virtual BOOL OnCreateAggregates();
	*/
};




/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NamedData_H__ADBA6C04_1D68_11D3_A5D0_0000B493FF1B__INCLUDED_)
