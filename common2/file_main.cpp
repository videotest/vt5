#include "stdafx.h"
#include "file_main.h"
#include "nameconsts.h"
#include "ImageSignature.h"

#include "ansiapi.h"
#include "\vt5\common2\misc_utils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// CFileDataObjectImpl
////////////////////////////////////////////////////////////////////////////////////////////////////
CFileDataObjectImpl::CFileDataObjectImpl()
{
	m_bDirty = false;
	m_bUseTextFileFormat = false;

}

//do notyhig here - only report error
bool CFileDataObjectImpl::ReadFile( const char *pszFileName )
{
	dbg_assert(false, "Error: CFileDataObjectImpl::ReadFile was not overrided\n");
	return false;
}
//do notyhig here - only report error
bool CFileDataObjectImpl::WriteFile( const char *pszFileName )
{
	dbg_assert(false, "Error: CFileDataObjectImpl::WriteFile was not overrided\n");
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
	return false;
}

//create a CStringArrayEx, call StoreToText and store it to file
bool CFileDataObjectImpl::WriteTextFile( const char *pszFileName )
{
	return false;
}

HRESULT CFileDataObjectImpl::InitNew()
{
	SetModifiedFlag( false );

	return _InitNew()?S_OK:E_FAIL;
}

HRESULT CFileDataObjectImpl::LoadFile( BSTR bstrFileName )
{
	try
	{
		W2A(bstrFileName, m_szCurrentFileName, _MAX_PATH);

		// pre read
		if (!BeforeReadFile())
			return E_FAIL;
		
		// read file
		if(m_bUseTextFileFormat)
		{
 			if (FAILED(ReadTextFile(m_szCurrentFileName)))
				return E_FAIL;
		}
		else
		{
			if (ReadFile(m_szCurrentFileName) == false)
				return E_FAIL;
		}

		// post read
		if (!AfterReadFile())
			return E_FAIL;

		SetModifiedFlag( false );

		return S_OK;

	}
	catch(...)
	{
		return E_FAIL;
	}
}

HRESULT CFileDataObjectImpl::SaveFile( BSTR bstrFileName )
{
	try
	{
		W2A(bstrFileName, m_szCurrentFileName, _MAX_PATH);

		// before write
		if (!BeforeWriteFile())
			return S_FALSE;

		// write file
		if( m_bUseTextFileFormat )
		{
			if (!(WriteTextFile(m_szCurrentFileName)))
				return E_FAIL;
		}
		else
		{
			if (!(WriteFile(m_szCurrentFileName)))
				return E_FAIL;
		}
		
		// post write
		if (!AfterWriteFile())
			return E_FAIL;

		SetModifiedFlag( false );

		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}
}

HRESULT CFileDataObjectImpl::LoadTextFile( BSTR bstrFileName )
{
	return E_NOTIMPL;
	
}

HRESULT CFileDataObjectImpl::SaveTextFile( BSTR bstrFileName )
{
	return E_NOTIMPL;
}

HRESULT CFileDataObjectImpl::LoadProtectedFile( BSTR bstrFileName )
{
	return E_NOTIMPL;
	
}

HRESULT CFileDataObjectImpl::SaveProtectedFile( BSTR bstrFileName )
{
	return E_NOTIMPL;
}

HRESULT CFileDataObjectImpl::IsDirty( BOOL *pbDirty )
{
	if(pbDirty)
	{
		*pbDirty = IsModified()?TRUE:FALSE;
		return S_OK;
	}
	return E_INVALIDARG;
};

HRESULT CFileDataObjectImpl::SetModifiedFlag( BOOL bSet )
{
	SetModifiedFlag( bSet==TRUE );
	return S_OK;
};


HRESULT CFileDataObjectImpl::IsFileFormatReadonly( BOOL *pbIsReadOnly )
{
	*pbIsReadOnly = FALSE;
	return S_OK;
}

int VTMessageBox( LPCSTR lpText, LPCSTR lpCaption, UINT uType );
HRESULT CFileDataObjectImpl::ShowSaveNotSupportedMessage()
{
	VTMessageBox("This file can not be saved", "VT5", MB_OK|MB_ICONEXCLAMATION);
	return S_OK;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// CFileFilterBase
////////////////////////////////////////////////////////////////////////////////////////////////////

IUnknown *CFileFilterBase::DoGetInterface( const IID &iid )
{
	if(iid == IID_IFileFilter3)		return (IFileFilter3*)this;
	else if(iid == IID_IFileFilter2)	return (IFileFilter2*)this;
	else if(iid == IID_IFileFilter)	return (IFileFilter*)this;
	else if(iid == IID_IFileDataObject)	return (IFileDataObject*)this;
	else if(iid == IID_IFileDataObject2)	return (IFileDataObject2*)this;
	else if(iid == IID_IFileDataObject3)	return (IFileDataObject3*)this;
	else if(iid == IID_IFileDataObject4)	return (IFileDataObject4*)this;
	else if(iid == IID_ILongOperation)	return (ILongOperation*)this;
	else return ComObjectBase::DoGetInterface( iid );
}


HRESULT CFileFilterBase::MatchFileType( BSTR bstr, DWORD *pdwConfidence )
{
	char szPath[_MAX_PATH];
	if(pdwConfidence)
	{
		W2A(bstr, szPath, _MAX_PATH);

		*pdwConfidence = MatchFile(szPath);

		return S_OK;
	}
	return E_INVALIDARG;
}

HRESULT CFileFilterBase::GetFilterParams( BSTR *pbstrFilter, BSTR *pbstrFilterName, BSTR *pbstrDocTemplates )
{
	if(pbstrFilter)
		*pbstrFilter = m_bstrFilterExt.copy();
	if(pbstrFilterName)
		*pbstrFilterName = m_bstrFilterName.copy();
	if(pbstrDocTemplates)
		*pbstrDocTemplates = m_bstrDocumentTemplate.copy();

	return S_OK;

}

// attach NamedData and Used DataContext to filter
HRESULT CFileFilterBase::AttachNamedData( IUnknown *punkNamedData, IUnknown *punkUsedContext )
{
	_KillAllObjects();
		
	HRESULT hr;
	INamedData* pINamedData = NULL;
	IDataContext* pIDataContext = NULL;

	// release previous interface
	m_sptrINamedData = NULL;
	m_sptrIDataContext = NULL;

	// get INamedData
	if (punkNamedData)
	{
		hr = punkNamedData->QueryInterface(IID_INamedData, (void**)&pINamedData);

		if (FAILED(hr))
			return hr;
		// attach without Addref
		m_sptrINamedData.Attach(pINamedData);    // named data
	}

	// get IDataContext
	if (punkUsedContext)
	{
		hr = punkUsedContext->QueryInterface(IID_IDataContext, (void**)&pIDataContext);

		if (FAILED(hr))
			return hr;

		// attach without Addref
		m_sptrIDataContext.Attach(pIDataContext);
	}
	
	return S_OK;
}

HRESULT CFileFilterBase::GetObjectTypesCount( int *piCount )
{
	if (!piCount)
		return E_INVALIDARG;
	*piCount = m_arrObjKind.GetSize();
	
	return S_OK;
}

HRESULT CFileFilterBase::GetObjectType( int nType, BSTR *pbstrType )
{
	if (!pbstrType)
		return E_INVALIDARG;
	// clear
	*pbstrType = 0;
	// check index validity
	if (nType < 0 || nType >= m_arrObjKind.GetSize())
		return E_INVALIDARG;

	char szKind[255];
	strcpy(szKind, m_arrObjKind[nType]);

	OLECHAR  wch[MAX_STRING*2];
	A2W(szKind, wch, MAX_STRING);
	*pbstrType = ::SysAllocString(wch);
	
		 
	return S_OK;
}


HRESULT CFileFilterBase::AttachNamedObject( IUnknown *punkObject )
{
	if (punkObject)
	{
		INamedDataObject2Ptr strNDO(punkObject);
		if(strNDO == 0)
			return E_INVALIDARG;
	}
		
	m_sptrAttachedObject = punkObject;
	m_bObjectExists = (punkObject) ? true : false;

	return S_OK;
}


HRESULT CFileFilterBase::GetCreatedObjectCount( int *piCount )
{
	
	*piCount = m_CreatedObjects.GetNodeCount();
	
	return S_OK;
}

HRESULT CFileFilterBase::GetCreatedObject( int nObject, IUnknown **ppunkObject )
{
	if (nObject < 0 || nObject >= m_CreatedObjects.GetNodeCount())
		return E_INVALIDARG;

	*ppunkObject = (IUnknown*)m_CreatedObjects.GetByPtrIdx(nObject);
	if(*ppunkObject)
		(*ppunkObject)->AddRef();
	

	return S_OK;
}

HRESULT CFileFilterBase::GetPropertyByName(BSTR bstrName, VARIANT *pvarVal)
{
	
	_GetPropertyByName(bstrName, pvarVal);
	return S_OK;
}

HRESULT CFileFilterBase::SetPropertyByName(BSTR bstrName, VARIANT varVal)
{
	if (_SetPropertyByName(bstrName, varVal))
		return S_OK;
	else
		return E_INVALIDARG;
}

HRESULT CFileFilterBase::GetPropertyCount( int *piCount )
{
	*piCount = _GetPropertyCount();
	return S_OK;
}

HRESULT CFileFilterBase::GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx)
{
	_GetPropertyByNum(nNum, pvarVal, pbstrName, pdwFlags, pGroupIdx);
	return S_OK;
}

HRESULT CFileFilterBase::SetPropertyByNum(int nNum, VARIANT varVal)
{
	if(_SetPropertyByNum(nNum, varVal))
		return S_OK;
	else
		return E_INVALIDARG;
}

HRESULT CFileFilterBase::ExecSettingsDialog(DWORD dwFlags)
{
	if (_ExecSettingsDialog(dwFlags))
		return S_OK;
	else
		return E_INVALIDARG;
}


CFileFilterBase::CFileFilterBase()
{
	m_bObjectExists = false;
}

CFileFilterBase::~CFileFilterBase()
{
	_KillAllObjects();
}


void CFileFilterBase::_KillAllObjects()
{
	POS pos = m_CreatedObjects.GetHeadPos();
	while(pos)
	{
		((IUnknown*)m_CreatedObjects.GetNextPtr(pos))->Release();
	}

	m_CreatedObjects.KillThemAll();
}

DWORD CFileFilterBase::MatchFile( const char *pszFileName )
{
	char	szExtention[_MAX_PATH];
	::_splitpath( pszFileName, 0, 0, 0, szExtention );

	char*	szAll = m_bstrFilterExt;
	char	szExt[_MAX_PATH];
	char* szNewLine = szAll;
	while(szNewLine != NULL)
	{
		szNewLine = strchr(szAll, '\n');
		if( szNewLine == NULL)	 
			strcpy(szExt, szAll);
		else
		{
			strncpy(szExt, szAll, szNewLine-szAll);
			szExt[szNewLine-szAll] = '\0';
			szAll = szNewLine + 1;
		}
			
		if( !_stricmp(szExt, szExtention) )
			return mfYesAttemptNative;
	}
	return mfNoAttempt;
}



void CFileFilterBase::_GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx)
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
	// try to find this type in array
	for (int i = 0; i < m_arrObjKind.GetSize(); i++)
	{
		if (!strcmp(m_arrObjKind[i], szType))
			return true;
	}
	// add type to array
	m_arrObjKind.Add(szType);
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

#ifdef _DEBUG
	HINSTANCE hDll_Common = GetModuleHandle("common.dll");
#else
	HINSTANCE hDll_Common = GetModuleHandle("common.dll");
#endif

	PCREATE_TYPED_OBJECT pCreateTypedObject = (PCREATE_TYPED_OBJECT)GetProcAddress(hDll_Common, "CreateTypedObject"); 
	DWORD dw = GetLastError();
	punkObject = pCreateTypedObject(szType);
	
	if(punkObject)
	{
		punkObject->AddRef();
		m_CreatedObjects.AddToTail(punkObject);	
	}
	
	return punkObject;
	
}

bool CFileFilterBase::BeforeReadFile()
{
	// clear previously Created object
	
	_KillAllObjects();
	
	return true;
}

// NOTE:
// if this functions must be overrided
// don't forget to call it in rewrited function
bool CFileFilterBase::AfterReadFile()
{
	
	POS pos = m_CreatedObjects.GetHeadPos();
	while(pos)
	{
		_variant_t var((IUnknown*)m_CreatedObjects.GetNextPtr(pos));
		if(m_sptrINamedData)
			m_sptrINamedData->SetValue(NULL, var);

		if( var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH )
			CheckImageSignature(var.punkVal);
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




