#ifndef __filebase_h__
#define __filebase_h__

#include "data5.h"
#include "docview5.h"
#include "cmnbase.h"
#include "utils.h"

#ifdef _COMMON_LIB
#endif //_COMMON_LIB*/


class std_dll CFileDataObjectImpl : public CImplBase
{
public:
	CFileDataObjectImpl();
protected:
	BEGIN_INTERFACE_PART_EXPORT(File, IFileDataObject4)
		com_call InitNew();
		com_call LoadFile( BSTR bstrFileName );
		com_call SaveFile( BSTR bstrFileName );
		com_call IsDirty( BOOL *pbDirty );
		com_call LoadTextFile( BSTR bstrFileName );
		com_call SaveTextFile( BSTR bstrFileName );
		com_call SetModifiedFlag( BOOL bSet );

		// bstrFileName - незащищенная часть файла (читается, если не удалось прочитать защищенный)
		// bstrFileName.protected - защищенный файл
		// bstrFileName.protected.chk - подпись защищенного файла
		// (если *.protected не будет - прочитаем, как LoadFile)
		com_call LoadProtectedFile( BSTR bstrFileName );
		com_call SaveProtectedFile( BSTR bstrFileName );
		com_call IsFileFormatReadonly( BOOL *pbIsReadOnly );
		com_call ShowSaveNotSupportedMessage();
	END_INTERFACE_PART(File);

	//function is called from LoadFile 
	virtual bool ReadFile( const char *pszFileName );
	virtual bool BeforeReadFile();
	virtual bool AfterReadFile();
	//function is called from StoreFile
	virtual bool WriteFile( const char *pszFileName );
	virtual bool BeforeWriteFile();
	virtual bool AfterWriteFile();

	virtual bool InitNew() = 0;
	//function works with IFileDataObject2 interface implementation 
	//by default this functions work with CStringArrayEx
	virtual bool ReadTextFile( const char *pszFileName );
	virtual bool WriteTextFile( const char *pszFileName );
	//Function should phrase text from CStringArrayEx
	virtual bool LoadFromText( CStringArrayEx &sa );
	//Function should store object state to string array
	virtual bool StoreToText( CStringArrayEx &sa );
	virtual bool IsFileFormatReadonly() {return false;}
	virtual void ShowSaveNotSupportedMessage();
public:
	virtual bool IsModified()
	{return m_bDirty;};
	virtual void SetModifiedFlag( bool bSet = true )
	{m_bDirty = bSet;}
protected:
	bool	m_bDirty;
public:
	bool	m_bUseTextFileFormat;	//force use text format
	bool	m_bUseProtectedFormat;	//force use protected format
protected:
	CString	m_strCurrentFileName;
};

class CStreamEx;

class std_dll CSerializableObjectImpl : public CImplBase
{
public:
	BEGIN_INTERFACE_PART_EXPORT(Serialize, ISerializableObject)
		com_call Load( IStream *pStream, SerializeParams *pparams );
		com_call Store( IStream *pStream, SerializeParams *pparams );
	END_INTERFACE_PART(Serialize)
	virtual bool Load( IStream *pStream, SerializeParams *pparams );
	virtual bool Store( IStream *pStream, SerializeParams *pparams );

	virtual bool SerializeObject( CStreamEx& stream, SerializeParams *pparams ) = 0;
};

class std_dll CTextObjectImpl : public CImplBase
{
public:
	BEGIN_INTERFACE_PART_EXPORT(Text, ITextObject)
		com_call GetText( BSTR *pbstr );
		com_call SetText( BSTR bstr );
	END_INTERFACE_PART(Text)
protected:
	virtual bool GetTextParams( CString &strParams ){return false;};
	virtual bool SetTextParams( const char *pszParams ){return false;};
};


class std_dll CFileFilterBase : public CCmdTarget,
								public CFileDataObjectImpl,
								public CRootedObjectImpl,
								public CLongOperationImpl
{
	DECLARE_DYNAMIC(CFileFilterBase);
	ENABLE_MULTYINTERFACE();
public:
	CFileFilterBase();
	~CFileFilterBase();
protected:
	DECLARE_INTERFACE_MAP();


	BEGIN_INTERFACE_PART_EXPORT(Filter, IFileFilter3)
		com_call AttachNamedData( IUnknown *punkNamedData, IUnknown *punkUsedContext );
		com_call MatchFileType( BSTR bstr, DWORD *pdwConfidence );
		com_call GetFilterParams( BSTR *pbstrFilter, BSTR *pbstrFilterName, BSTR *pbstrDocTemplates );

		com_call GetObjectTypesCount( int *pnCount );
		com_call GetObjectType( int nType, BSTR *pbstrType );

		com_call AttachNamedObject( IUnknown *punkObject );

		com_call GetCreatedObjectCount( int *piCount );
		com_call GetCreatedObject( int nObject, IUnknown **ppunkObject );


		com_call GetPropertyByName(BSTR bstrName, VARIANT *pvarVal);
		com_call SetPropertyByName(BSTR bstrName, VARIANT varVal);
		com_call GetPropertyCount( int *piCount );
		com_call GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx);
		com_call SetPropertyByNum(int nNum, VARIANT varVal);
		com_call ExecSettingsDialog(DWORD dwFlags);
	END_INTERFACE_PART(Filter);
protected:
	virtual CDocTemplate::Confidence MatchFile( const char *pszFileName );
	bool	AddDataObjectType(LPCTSTR szType);
	LPCTSTR GetDataObjectType(int nType);


	// create object of request type
	virtual IUnknown* CreateNamedObject(LPCTSTR szType);

	// overrides
	virtual bool BeforeReadFile();
	virtual bool AfterReadFile();
	virtual bool BeforeWriteFile();
	virtual bool AfterWriteFile();

	virtual void GetPropertyByName(BSTR bstrName, VARIANT *pvarVal){};
	virtual bool SetPropertyByName(BSTR bstrName, VARIANT varVal){return false;};
	virtual int  GetPropertyCount(){return 0;};
	virtual void GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx);
	virtual bool SetPropertyByNum(int nNum, VARIANT varVal){return false;};
	virtual bool ExecSettingsDialog(DWORD dwFlags){return false;};

	bool						m_bObjectExists; // flag for attachec object exists
	sptrINamedDataObject		m_sptrAttachedObject; // attached NamedDataObject 
	sptrIDataTypeInfoManager	m_sptrTypeInfoManager; // TypeInfoManager
	sptrINamedData				m_sptrINamedData;    // named data
	sptrIDataContext			m_sptrIDataContext; // context 

	CString	m_strDocumentTemplate;
	CString	m_strFilterName;
	CString	m_strFilterExt;

	CStringArray m_arrObjKind; // array of type of NamedDataObjects that can be create by this filter
	CCompManager m_CreatedObjects; // used for keep object between creating and adding to NamedData
};

#endif //__filebase_h__