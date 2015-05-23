#ifndef __file_main_h__
#define __file_main_h__

#include "data5.h"
#include "com_main.h"
#include "com_unknown.h"
#include "helper.h"
#include "impl_long.h"

typedef IUnknown* (*PCREATE_TYPED_OBJECT)(const char *szObjectType);

class CFileDataObjectImpl : public IFileDataObject4
{
public:
	CFileDataObjectImpl();

protected:

//IFileDataObject2
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

	//function is called from LoadFile 
	virtual bool ReadFile( const char *pszFileName );
	virtual bool BeforeReadFile();
	virtual bool AfterReadFile();
	//function is called from StoreFile
	virtual bool WriteFile( const char *pszFileName );
	virtual bool BeforeWriteFile();
	virtual bool AfterWriteFile();
	virtual bool _InitNew() = 0;

	//function works with IFileDataObject2 interface implementation 
	virtual bool ReadTextFile( const char *pszFileName );
	virtual bool WriteTextFile( const char *pszFileName );

public:
	virtual bool IsModified()
	{return m_bDirty;};
	virtual void SetModifiedFlag( bool bSet = true )
	{m_bDirty = bSet;}
protected:
	bool	m_bDirty;
	bool	m_bUseTextFileFormat;	//force use text format
	char	m_szCurrentFileName[_MAX_PATH];
};

class CFileFilterBase : public ComObjectBase,
					   public IFileFilter3,
					   public CFileDataObjectImpl,
					   public CLongOperationImpl
								//public CRootedObjectImpl,
								
{

public:
	CFileFilterBase();
	virtual ~CFileFilterBase();

	virtual IUnknown *DoGetInterface( const IID &iid );

protected:
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


	virtual DWORD MatchFile( const char *pszFileName );
	bool	AddDataObjectType(LPCTSTR szType);
	LPCTSTR GetDataObjectType(int nType);


	// create object of request type
	virtual IUnknown* CreateNamedObject(LPCTSTR szType);

	// overrides
	virtual bool BeforeReadFile();
	virtual bool AfterReadFile();
	virtual bool BeforeWriteFile();
	virtual bool AfterWriteFile();

	virtual void _GetPropertyByName(BSTR bstrName, VARIANT *pvarVal){};
	virtual bool _SetPropertyByName(BSTR bstrName, VARIANT varVal){return false;};
	virtual int  _GetPropertyCount(){return 0;};
	virtual void _GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx);
	virtual bool _SetPropertyByNum(int nNum, VARIANT varVal){return false;};
	virtual bool _ExecSettingsDialog(DWORD dwFlags){return false;};

	void _KillAllObjects();

	bool						m_bObjectExists; // flag for attachec object exists
	sptrINamedDataObject		m_sptrAttachedObject; // attached NamedDataObject 
	sptrIDataTypeInfoManager	m_sptrTypeInfoManager; // TypeInfoManager
	sptrINamedData				m_sptrINamedData;    // named data
	sptrIDataContext			m_sptrIDataContext; // context 

	_bstr_t	m_bstrDocumentTemplate;
	_bstr_t	m_bstrFilterName;
	_bstr_t	m_bstrFilterExt;

	StringArray m_arrObjKind; // array of type of NamedDataObjects that can be create by this filter
	PtrList m_CreatedObjects; // used for keep object between creating and adding to NamedData
};



#endif
