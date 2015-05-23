#if !defined(AFX_TYPES_H_A54341DA_6309_4CA5_B536_E320673CBB5A__INCLUDED_)
#define AFX_TYPES_H_A54341DA_6309_4CA5_B536_E320673CBB5A__INCLUDED_


#include "dib.h"
#include "dbase.h"

enum EditModeDlg 
{
	emdNotDefine	= 0,
	emdAddTable		= 1,
	emdEditTable	= 2,	
};


/////////////////////////////////////////////////////////////////////////////
struct _RecordHeader
{	
	BYTE	bStructSize;	
	BYTE	bHaveChilds;			
	BYTE	bVersion;
	BYTE	bBaseObjectEmpty;
	DWORD	dwTumbnailSize;
	DWORD	dwObjectSize;
};

struct _RecordList
{
	WORD	wVersion;
	char	szObjectType[30];
	DWORD	dwObjectSize;
	BOOL	bContinue;

};

class CFieldBase{
public:
	CString m_strTable;
	CString m_strField;
};

/////////////////////////////////////////////////////////////////////////////
class _CFieldInfo
{

public:
	_CFieldInfo(
				CString		strFieldName,
				CString		strUserName,	
				FieldType	FieldType,					
				CString		strPrimaryTable,
				CString		strPrimaryFieldName,
				BOOL		bRequiredValue,
				BOOL		bDefaultValue,
				CString		strDefaultValue,
				CString		strVTObjectType,
				BOOL		bCanEditStructure
		);
	_CFieldInfo();
	~_CFieldInfo();
	void CopyFrom( _CFieldInfo* pFieldCopyFrom );

	CString		m_strFieldName;
	CString		m_strUserName;	
	FieldType	m_FieldType;	
	
	CString		m_strPrimaryTable;
	CString		m_strPrimaryFieldName;

	BOOL		m_bRequiredValue;
	BOOL		m_bDefaultValue;
	CString		m_strDefaultValue;
	CString		m_strVTObjectType;

	BOOL		m_bCanEditStructure;	

	CString		m_strFormat;

};


/////////////////////////////////////////////////////////////////////////////
class _CTableInfo
{
public:
	_CTableInfo( CString strTableName );
	~_CTableInfo();	

	CString m_strTableName;
	void AddFieldInfo( _CFieldInfo* pFI );


	CArray<_CFieldInfo*, _CFieldInfo*> m_arrFieldInfo;	
};




/////////////////////////////////////////////////////////////////////////////
class CQueryField
{
public:
	CQueryField();
	~CQueryField();
	
//For virtual fields
	void InitFrom( CQueryField* pFieldSrc );

protected:
	CQueryField* m_pMasterField;
public:
	void SetMasterField( CQueryField* pMasterField );


//Field info
public:	
	CString m_strTable;
	CString m_strField;	
	CString m_strInnerName;	

	CString GetFieldName(){ return m_strField;}
	CString GetTableName(){ return m_strTable;}
	
	FieldType GetFieldType(){ return m_fieldType;}

public:
	IUnknown* GetRecordset();

protected:
	IDBaseDocumentPtr	m_dbaseDocPtr;
	ADO::_RecordsetPtr		m_recordsetPtr;
	
	int					m_nFieldIndex;
	FieldType			m_fieldType;
	_variant_t			m_var;		

	INamedDataPtr		GetPrivateNamedData();

	GuidKey				m_guidRoot;
	GuidKey				m_guidBase;
	CArray<GuidKey, GuidKey> m_arGuid;
	
	bool KillNotSerializeChildGuid();

	bool IsChild( IUnknown* punkChild );

	bool IsRecordsetEmpty();

public:
	ADO::FieldPtr			GetFieldPtr();
//Init
//********************
	

public:
	void Init( FieldType fieldType,
				IUnknown* punkDBaseDoc, IUnknown* punkRecordset, int nFieldIndex );	

	bool OnGetValue( BSTR bstrName, tagVARIANT *pvar );
	bool OnSetValue( BSTR bstrName, const tagVARIANT var, bool bNeedSetValue, bool& bWasSetValue );
	bool OnDeleteEntry( BSTR bstrName );


	void DeInit( );
	void ResetContent();

	bool InitDefValues( bool bAfterInsert );
	bool RecordChange();	
	bool Cancel();	

public:

//For quick access
//********************	
	bool GetPrivateValue( _variant_t& var ); //Load header, Load Thumbnail, Load Object
	void* GetTumbnail();   //Load header, Load Thumbnail


	CString GetAsTableField();
	CString GetFieldFullPath();

	void Serialize( CArchive& ar );	

//Exchange
//********************
	bool Load( bool bPutToNamedData );
	bool LoadHeader();
	bool LoadThumbnail();
	bool LoadObject( bool bPutToNamedData );
	
	bool Store(); 

protected:
	bool SerializeChilds( bool bLoad, bool bClearModifiedFlag );

protected:
	bool LoadObjectContext( ISerializableObjectPtr ptrso, UINT dwObjectSize, ADO::FieldPtr fieldPtr, bool bRootObject = false );
protected:	
	bool KillChilds();
	


//Status	
//********************
protected:
	bool m_bModified;
public:
	bool GetModifiedFlag( );
	bool NeedAskUpdateRecord();

private:
	void SetModifiedFlag( bool bModified );

protected:
	bool m_bWasReadHeader;
public:
	bool GetWasReadHeader( ){ return m_bWasReadHeader;}	

protected:
	bool m_bWasReadTumbnail;
	bool m_bWasReadData;	
public:
	bool GetWasReadData( ){ return m_bWasReadData;}	
	bool GetWasReadTumbnail(){ return m_bWasReadTumbnail;}	

protected:
	bool	m_bBaseObjectEmpty;
	bool	m_bNeedAskUpdateRecord;

	
//********************
//Tumbnail
protected:
	CDibWrapper m_Tumbnail;
	_RecordHeader m_recordHeader;

	bool TumbnailStore( IStream* pStream );
	bool TumbnailLoad( IStream* pStream );

	bool TumbnailDraw( CDC* pDC, CRect rcOutput );
public:
	bool CreateThumbnail( IUnknown* punkDataObject, CSize sizeTumbnail );

protected:   
	bool WriteFromStreamToBlob( IStream* pStream, DWORD dwFromPos, DWORD dwSize );

///Misc
	static void OnNewObjectSet2Data( IUnknown* punkCtrl, IUnknown* punkObject, 
					GUID prevKey, BOOL bFlag, IUnknown* punkContext );

public:
	bool IsDBaseObject( IUnknown* punkObj, GUID* pGuidDBaseParent, bool& bForceExit, bool& bEmpty );

	void AttachData2Object( IUnknown* punk );

protected:
	bool WriteDataToField( void* pData, DWORD dwSize );
	bool ReadDataFromField( ADO::FieldPtr fieldPtr, void* pData, DWORD dwSize, bool* pbEmpty );
  /*
	{

	}
*/
protected:
	bool	m_bLockUpdateObject;

public:
	bool	ProcessDataObjectChange( IUnknown* punkObject );



};

/////////////////////////////////////////////////////////////////////////////
class CQuerySortField
{
public:
	CQuerySortField();
	~CQuerySortField();

	CString m_strTable;
	CString m_strField;
	void Serialize( CArchive& ar );
	BOOL m_bASC;
};


class CGridColumnInfo
{
public:
	CGridColumnInfo();
	~CGridColumnInfo();

	CString m_strTable;
	CString m_strField;
	
	CString m_strColumnCaption;
	int m_nColumnWidth;
	

	void Serialize( CArchive& ar );	
	
};






/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_TYPES_H_A54341DA_6309_4CA5_B536_E320673CBB5A__INCLUDED_)
