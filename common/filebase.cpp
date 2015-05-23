#include "stdafx.h"
#include "filebase.h"
//#include "Util.h"
#include "objbase_.h"

#include "StreamEx.h"
#include "ImageSignature.h"
#include "resource.h"

IMPLEMENT_DYNAMIC(CFileFilterBase, CCmdTarget);

//CFileDataObjectImpl	object
IMPLEMENT_UNKNOWN_BASE(CFileDataObjectImpl, File);

CFileDataObjectImpl::CFileDataObjectImpl()
{
	m_bDirty = false;
	m_bUseTextFileFormat = false;
	m_bUseProtectedFormat = false;
}

//do notyhig here - only report error
bool CFileDataObjectImpl::ReadFile( const char *pszFileName )
{
	TRACE( "Error: CFileDataObjectImpl::ReadFile was not overrided\n" );
	ASSERT( FALSE );
	return false;
}
//do notyhig here - only report error
bool CFileDataObjectImpl::WriteFile( const char *pszFileName )
{
	TRACE( "Error: CFileDataObjectImpl::WriteFile was not overrided\n" );
	ASSERT( FALSE );
	return false;
}

bool CFileDataObjectImpl::BeforeReadFile()
{
	return true;
}

bool CFileDataObjectImpl::AfterReadFile()
{
	return true;
}

bool CFileDataObjectImpl::BeforeWriteFile()
{
	return true;
}

bool CFileDataObjectImpl::AfterWriteFile()
{
	return true;
}


//create a CStringArrayEx, load it from file and call LoadFromText virtual 
bool CFileDataObjectImpl::ReadTextFile( const char *pszFileName )
{
	_try(CFileDataObjectImpl, ReadTextFile)
	{
		CStringArrayEx	sa;

		sa.ReadFile( pszFileName );
		
		return LoadFromText( sa );
	}
	_catch_report;

	return false;
}

//create a CStringArrayEx, call StoreToText and store it to file
bool CFileDataObjectImpl::WriteTextFile( const char *pszFileName )
{
	_try(CFileDataObjectImpl, WriteTextFile)
	{
		CStringArrayEx	sa;

		if( !StoreToText( sa ) )
			return false;

		if (IsNeedSaveBackup(pszFileName))
		{
			CString sBakFileName(pszFileName);
			sBakFileName += _T(".bak");
			::DeleteFile(sBakFileName);
			::MoveFile(pszFileName,sBakFileName);
		}
		sa.WriteFile( pszFileName );
		return true;
	}
	_catch_report;

	return false;
}

//do notyhig here - only report error
bool CFileDataObjectImpl::LoadFromText( CStringArrayEx &sa )
{
	TRACE( "Error: CFileDataObjectImpl::LoadFromText was not overrided\n" );
	ASSERT( FALSE );
	return false;
}//do notyhig here - only report error
bool CFileDataObjectImpl::StoreToText( CStringArrayEx &sa )
{
	TRACE( "Error: CFileDataObjectImpl::StoreToText was not overrided\n" );
	ASSERT( FALSE );
	return false;
}

HRESULT CFileDataObjectImpl::XFile::InitNew()
{
	_try_nested_base(CFileDataObjectImpl, File, InitNew)
	{
		pThis->SetModifiedFlag( false );

		return pThis->InitNew()?S_OK:E_FAIL;
	}
	_catch_nested;
}

HRESULT CFileDataObjectImpl::XFile::LoadFile( BSTR bstrFileName )
{
	METHOD_PROLOGUE_BASE(CFileDataObjectImpl, File)
	//_try_nested_base(CFileDataObjectImpl, File, LoadFile)
	try
	{
		pThis->m_strCurrentFileName = bstrFileName;

		// pre read
		if (!pThis->BeforeReadFile())
			return E_FAIL;
		
		// read file
		if( pThis->m_bUseTextFileFormat )
		{
 			if (!pThis->ReadTextFile(pThis->m_strCurrentFileName))
				return E_FAIL;
		}
		else
		{
			if (!pThis->ReadFile(pThis->m_strCurrentFileName) )
				return E_FAIL;
		}

		// post read
		if (!pThis->AfterReadFile())
			return E_FAIL;

		pThis->SetModifiedFlag( false );

		return S_OK;

	}
	catch( CException	*pe )
	{
		pe->ReportError();
		pe->Delete();
		return E_FAIL;
	}
}

HRESULT CFileDataObjectImpl::XFile::SaveFile( BSTR bstrFileName )
{
	METHOD_PROLOGUE_BASE(CFileDataObjectImpl, File)
	try
	{
		pThis->m_strCurrentFileName = bstrFileName;
		// before write
		if (!pThis->BeforeWriteFile())
			return S_FALSE;

		// write file
		if( pThis->m_bUseTextFileFormat )
		{
			if( !pThis->WriteTextFile(pThis->m_strCurrentFileName))
				return E_FAIL;
		}
		else
		{
			if( !pThis->WriteFile(pThis->m_strCurrentFileName) )
				return E_FAIL;
		}
		
		// post write
		if (!pThis->AfterWriteFile())
			return E_FAIL;

		pThis->SetModifiedFlag( false );

		return S_OK;
	}
	catch( CException	*pe )
	{
		pe->ReportError();
		pe->Delete();
		return E_FAIL;
	}
}

HRESULT CFileDataObjectImpl::XFile::LoadTextFile( BSTR bstrFileName )
{
	METHOD_PROLOGUE_BASE(CFileDataObjectImpl, File)
	try
	{
		CString	strFileName( bstrFileName );

		if( !pThis->ReadTextFile( strFileName ) )
			return E_FAIL;

		pThis->SetModifiedFlag( false );

		return S_OK;
	}
	catch( CException	*pe )
	{
		pe->ReportError();
		pe->Delete();
		return E_FAIL;
	}
}

HRESULT CFileDataObjectImpl::XFile::SaveTextFile( BSTR bstrFileName )
{
	METHOD_PROLOGUE_BASE(CFileDataObjectImpl, File)
	try
	{
		CString	strFileName( bstrFileName );
		if( !pThis->WriteTextFile( strFileName ) )
			return E_FAIL;

		pThis->SetModifiedFlag( false );

		return S_OK;
	}
	catch( CException	*pe )
	{
		pe->ReportError();
		pe->Delete();
		return E_FAIL;
	}
}


HRESULT CFileDataObjectImpl::XFile::IsDirty( BOOL *pbDirty )
{
	_try_nested_base(CFileDataObjectImpl, File, IsDirty)
	{
		*pbDirty = pThis->IsModified()?TRUE:FALSE;
		return S_OK;
	}
	_catch_nested;
};

HRESULT CFileDataObjectImpl::XFile::SetModifiedFlag( BOOL bSet )
{
	_try_nested_base(CFileDataObjectImpl, File, SetModifiedFlag)
	{
		pThis->SetModifiedFlag( bSet==TRUE );
		return S_OK;
	}
	_catch_nested;
};

HRESULT CFileDataObjectImpl::XFile::LoadProtectedFile( BSTR bstrFileName )
{
	METHOD_PROLOGUE_BASE(CFileDataObjectImpl, File)
	//_try_nested_base(CFileDataObjectImpl, File, LoadFile)
	try
	{
		// Проверим, корректен ли файл bstrFileName.protected
		CString strProtected = CString(bstrFileName) + ".protected";
		if( CheckFileSignature(strProtected, true ) ) // проверяем упрощенную подпись, без ключа
		{ // Подпись файла корректна - можно читать
			pThis->m_strCurrentFileName = strProtected;
			pThis->m_bUseProtectedFormat = true;

			if (pThis->BeforeReadFile() &&
				pThis->ReadFile(strProtected) &&
				pThis->AfterReadFile() ) // Если методика защищена, а в ключе птички нет - на одном из этих этапов обломимся
			{ // Успешно прочитали
				pThis->SetModifiedFlag( false );
				pThis->m_strCurrentFileName = bstrFileName;
				return S_OK;
			}
		}

		// Обработка ошибочных ситуаций:
		// Если не подписан, или подпись некорректна, или не прочитался защищенный файл
		// - читаем незащищенную часть
		pThis->m_bUseProtectedFormat = false;
		return LoadFile( bstrFileName ); 
	}
	catch( CException	*pe )
	{
		pe->ReportError();
		pe->Delete();
		return E_FAIL;
	}
}

HRESULT CFileDataObjectImpl::XFile::SaveProtectedFile( BSTR bstrFileName )
{
	METHOD_PROLOGUE_BASE(CFileDataObjectImpl, File)
	try
	{
		pThis->m_bUseProtectedFormat = false;
		pThis->m_strCurrentFileName = bstrFileName;

		//HRESULT hres = SaveTextFile( bstrFileName );
		HRESULT hres = S_OK; // 29.06.2006 build 107
		// не будем сохранять незащищенную часть...
		if(hres==S_OK)
		{ // Если успешно записалась "незащищенная" часть - записать защищенную и подпись для нее
			pThis->m_bUseProtectedFormat = true;
			CString strProtected = CString(bstrFileName) + ".protected";
			hres = SaveFile( _bstr_t(strProtected) );
			CreateFileSignature( strProtected, true );
		}

		return hres;
	}
	catch( CException	*pe )
	{
		pe->ReportError();
		pe->Delete();
		return E_FAIL;
	}
}

HRESULT CFileDataObjectImpl::XFile::IsFileFormatReadonly( BOOL *pbIsReadOnly )
{
	METHOD_PROLOGUE_BASE(CFileDataObjectImpl, File)
	*pbIsReadOnly = pThis->IsFileFormatReadonly();
	return S_OK;
}

HRESULT CFileDataObjectImpl::XFile::ShowSaveNotSupportedMessage()
{
	METHOD_PROLOGUE_BASE(CFileDataObjectImpl, File)
	pThis->ShowSaveNotSupportedMessage();
	return S_OK;
}



//CSerializableObjectImpl helper
IMPLEMENT_UNKNOWN_BASE(CSerializableObjectImpl, Serialize);

HRESULT CSerializableObjectImpl::XSerialize::Load( IStream *pStream, SerializeParams *pparams )
{
	METHOD_PROLOGUE_BASE(CSerializableObjectImpl, Serialize)
	return pThis->Load( pStream, pparams )?S_OK:E_FAIL;
}

HRESULT CSerializableObjectImpl::XSerialize::Store( IStream *pStream, SerializeParams *pparams )
{
	METHOD_PROLOGUE_BASE(CSerializableObjectImpl, Serialize)
	return pThis->Store( pStream, pparams )?S_OK:E_FAIL;
}



bool CSerializableObjectImpl::Load( IStream *pStream, SerializeParams *pparams )
{
	CStreamEx stream( pStream, true );
	return SerializeObject( stream, pparams );
}

bool CSerializableObjectImpl::Store( IStream *pStream, SerializeParams *pparams )
{
	CStreamEx stream( pStream, false );
	return SerializeObject( stream, pparams );
}


//CTextObjectImpl helper
IMPLEMENT_UNKNOWN_BASE(CTextObjectImpl, Text);

HRESULT CTextObjectImpl::XText::GetText( BSTR *pbstr )
{
	_try_nested_base(CTextObjectImpl, Text, GetText)
	{
		CString	str;

		if( !pThis->GetTextParams( str ) )
			return E_INVALIDARG;

		*pbstr = str.AllocSysString();

		return S_OK;
	}
	_catch_nested;
}

HRESULT CTextObjectImpl::XText::SetText( BSTR bstr )
{
	_try_nested_base(CTextObjectImpl, Text, SetText)
	{
		CString	s = bstr;

		if( !pThis->SetTextParams( s ) )return E_INVALIDARG;
		return S_OK;
	}
	_catch_nested;
}

//CFileFilterBase - base class

CFileFilterBase::CFileFilterBase()
{
	m_bObjectExists = false;
}

CFileFilterBase::~CFileFilterBase()
{
	m_CreatedObjects.FreeComponents();
	m_arrObjKind.RemoveAll();
}

CDocTemplate::Confidence CFileFilterBase::MatchFile( const char *pszFileName )
{
	ASSERT(! m_strFilterExt.IsEmpty() );

	char	szExt[_MAX_PATH];

	::_splitpath( pszFileName, 0, 0, 0, szExt );

	int	idx = 0;
	CString	strAll = m_strFilterExt;
	while( idx != -1 )
	{
		idx = strAll.Find( "\n" );
		CString	strExt;
		if( idx == -1 )	 
			strExt = strAll;
		else
		{
			strExt = strAll.Left( idx );
			strAll = strAll.Right( strAll.GetLength()-1-idx );
		}
			
		if( !strExt.CompareNoCase( szExt ) )
			return CDocTemplate::yesAttemptNative;
	}
	return CDocTemplate::noAttempt;
}

void CFileDataObjectImpl::ShowSaveNotSupportedMessage()
{
	AfxMessageBox(IDS_THIS_FORMAT_CAN_NOT_BE_SAVED);
}



IMPLEMENT_UNKNOWN(CFileFilterBase, Filter);

BEGIN_INTERFACE_MAP(CFileFilterBase, CCmdTarget)
	INTERFACE_PART(CFileFilterBase, IID_IFileFilter, Filter)
	INTERFACE_PART(CFileFilterBase, IID_IFileDataObject, File)
	INTERFACE_PART(CFileFilterBase, IID_IFileDataObject2, File)
	INTERFACE_PART(CFileFilterBase, IID_IFileDataObject3, File)
	INTERFACE_PART(CFileFilterBase, IID_IFileDataObject4, File)
	INTERFACE_PART(CFileFilterBase, IID_IRootedObject, Rooted)
	INTERFACE_PART(CFileFilterBase, IID_IFileFilter2, Filter)
	INTERFACE_PART(CFileFilterBase, IID_IFileFilter3, Filter)
	INTERFACE_PART(CFileFilterBase, IID_ILongOperation, Long)
END_INTERFACE_MAP()

HRESULT CFileFilterBase::XFilter::MatchFileType( BSTR bstr, DWORD *pdwConfidence )
{
	_try_nested(CFileFilterBase, Filter, MatchFileType)
	{
		CString	strFileName = bstr;
		*pdwConfidence = pThis->MatchFile( strFileName );

		return S_OK;
	}
	_catch_nested;
}

HRESULT CFileFilterBase::XFilter::GetFilterParams( BSTR *pbstrFilter, BSTR *pbstrFilterName, BSTR *pbstrDocTemplates )
{
	_try_nested(CFileFilterBase, Filter, GetFilterParams)
	{
		if( pbstrFilter )
			*pbstrFilter = pThis->m_strFilterExt.AllocSysString();
		if( pbstrFilterName )
			*pbstrFilterName = pThis->m_strFilterName.AllocSysString();
		if( pbstrDocTemplates )
			*pbstrDocTemplates = pThis->m_strDocumentTemplate.AllocSysString();

		return S_OK;
	}
	_catch_nested;
}

// attach NamedData and Used DataContext to filter
HRESULT CFileFilterBase::XFilter::AttachNamedData( IUnknown *punkNamedData, IUnknown *punkUsedContext )
{
	_try_nested(CFileFilterBase, Filter, AttachNamedData)
	{	

		pThis->m_CreatedObjects.FreeComponents();

		HRESULT hr;
		INamedData* pINamedData = NULL;
		IDataContext * pIDataContext = NULL;

		// release previous interface
		pThis->m_sptrINamedData = NULL;
		pThis->m_sptrIDataContext = NULL;

		// get INamedData
		if (punkNamedData)
		{
			hr = punkNamedData->QueryInterface(IID_INamedData, (void**)&pINamedData);

			if (FAILED(hr))
				return hr;
			// attach without Addref
			pThis->m_sptrINamedData.Attach(pINamedData);    // named data
		}

		// get IDataContext
		if (punkUsedContext)
		{
			hr = punkUsedContext->QueryInterface(IID_IDataContext, (void**)&pIDataContext);

			if (FAILED(hr))
				return hr;

			// attach without Addref
			pThis->m_sptrIDataContext.Attach(pIDataContext);
		}

		return S_OK;
	}
	_catch_nested;
}

HRESULT CFileFilterBase::XFilter::GetObjectTypesCount( int *piCount )
{
	_try_nested(CFileFilterBase, Filter, GetObjectTypesCount)
	{
		if (!piCount)
			return E_INVALIDARG;

		*piCount = pThis->m_arrObjKind.GetSize();
		
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFileFilterBase::XFilter::GetObjectType( int nType, BSTR *pbstrType )
{
	_try_nested(CFileFilterBase, Filter, GetObjectType)
	{
		if (!pbstrType)
			return E_INVALIDARG;
		// clear
		*pbstrType = 0;
		// check index validity
		if (nType < 0 || nType >= pThis->m_arrObjKind.GetSize())
			return E_INVALIDARG;

		*pbstrType = pThis->m_arrObjKind[nType].AllocSysString();

		return S_OK;
	}
	_catch_nested;
}


HRESULT CFileFilterBase::XFilter::AttachNamedObject( IUnknown *punkObject )
{
	_try_nested(CFileFilterBase, Filter, AttachNamedObject)
	{
		if (punkObject)
		{
			if (!CheckInterface(punkObject, IID_INamedDataObject2))
				return E_INVALIDARG;
		}
		
		pThis->m_sptrAttachedObject = punkObject;
		pThis->m_bObjectExists = (punkObject) ? true : false;

		return S_OK;
	}
	_catch_nested;
}


HRESULT CFileFilterBase::XFilter::GetCreatedObjectCount( int *piCount )
{
	_try_nested(CFileFilterBase, Filter, AttachNamedObject)
	{
		*piCount = pThis->m_CreatedObjects.GetComponentCount();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFileFilterBase::XFilter::GetCreatedObject( int nObject, IUnknown **ppunkObject )
{
	_try_nested(CFileFilterBase, Filter, AttachNamedObject)
	{
		if (nObject < 0 || nObject >= pThis->m_CreatedObjects.GetComponentCount())
			return E_INVALIDARG;

		*ppunkObject = pThis->m_CreatedObjects.GetComponent(nObject, true);

		return S_OK;
	}
	_catch_nested;
}

HRESULT CFileFilterBase::XFilter::GetPropertyByName(BSTR bstrName, VARIANT *pvarVal)
{
	_try_nested(CFileFilterBase, Filter, GetPropertyByName)
	{
		pThis->GetPropertyByName(bstrName, pvarVal);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFileFilterBase::XFilter::SetPropertyByName(BSTR bstrName, VARIANT varVal)
{
	_try_nested(CFileFilterBase, Filter, SetPropertyByName)
	{
		if (pThis->SetPropertyByName(bstrName, varVal))
			return S_OK;
		else
			return E_INVALIDARG;
	}
	_catch_nested;
}

HRESULT CFileFilterBase::XFilter::GetPropertyCount( int *piCount )
{
	_try_nested(CFileFilterBase, Filter, GetPropertyCount)
	{
		*piCount = pThis->GetPropertyCount();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFileFilterBase::XFilter::GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx)
{
	_try_nested(CFileFilterBase, Filter, GetPropertyByNum)
	{
		pThis->GetPropertyByNum(nNum, pvarVal, pbstrName, pdwFlags, pGroupIdx);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CFileFilterBase::XFilter::SetPropertyByNum(int nNum, VARIANT varVal)
{
	_try_nested(CFileFilterBase, Filter, SetPropertyByNum)
	{
		if (pThis->SetPropertyByNum(nNum, varVal))
			return S_OK;
		else
			return E_INVALIDARG;
	}
	_catch_nested;
}

HRESULT CFileFilterBase::XFilter::ExecSettingsDialog(DWORD dwFlags)
{
	_try_nested(CFileFilterBase, Filter, ExecSettingsDialog)
	{
		if (pThis->ExecSettingsDialog(dwFlags))
			return S_OK;
		else
			return E_INVALIDARG;
	}
	_catch_nested;
}

void CFileFilterBase::GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx)
{
	if(pvarVal)
		pvarVal->vt = VT_EMPTY;
	if(pbstrName)
		*pbstrName = 0;
	if(pGroupIdx)
		*pGroupIdx = 0;
	if(pdwFlags)
		*pdwFlags = 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
bool CFileFilterBase::AddDataObjectType(LPCTSTR szType)
{
	CString strType(szType);

	if (strType.IsEmpty())
		return false;

	// try to find this type in array
	for (int i = 0; i < m_arrObjKind.GetSize(); i++)
	{
		if (strType == m_arrObjKind[i])
			return true;
	}
	// add type to array
	m_arrObjKind.Add(strType);
	return true;
}

LPCTSTR CFileFilterBase::GetDataObjectType(int nType)
{
	// check index validity
	if (nType < 0 || nType >= m_arrObjKind.GetSize())
		return NULL;

	return m_arrObjKind[nType];
}

// create object of 'szType' type
IUnknown* CFileFilterBase::CreateNamedObject(LPCTSTR szType)
{
	IUnknown *punkObject = NULL;

	_try (CFileFilterBase, CreateNamedObject)
	{
		punkObject = ::CreateTypedObject( szType );

		if( punkObject )
			m_CreatedObjects.AddComponent(punkObject);	
	}
	_catch;
	// return IUnknown
	return punkObject;
}

bool CFileFilterBase::BeforeReadFile()
{
	// clear previously Created object
	m_CreatedObjects.FreeComponents();

	//now NamedData may be = NULL
	/*if (NULL == m_sptrINamedData)
		return false;*/

	return true;
}

// NOTE:
// if this functions must be overrided
// don't forget to call it in rewrited function
bool CFileFilterBase::AfterReadFile()
{
	// add objects to NamedData
	_try (CFileFilterBase, AfterReadFile)
	{
		for (int i = 0; i < m_CreatedObjects.GetComponentCount(); i++)
		{
			_variant_t var(m_CreatedObjects.GetComponent(i, false));

			if( m_sptrINamedData )
				m_sptrINamedData->SetValue(NULL, var);

			if( var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH )
				CheckImageSignature(var.punkVal);

		}
	}
	_catch
	{
		return false;
	}

	return true;
}

bool CFileFilterBase::BeforeWriteFile()
{
	if (NULL == m_sptrINamedData)
		return false;

	if (NULL == m_sptrIDataContext && m_sptrAttachedObject == NULL)
		return false;
	
	return true;
}

// NOTE:
// if this functions must be overrided
// don't forget call it in rewrited function
bool CFileFilterBase::AfterWriteFile()
{
	return true;
}



