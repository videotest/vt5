// QueryFile.cpp: implementation of the CQueryFileFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dbasedoc.h"
#include "QueryFile.h"
#include "constant.h"

#include "AfxPriv.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CQueryFileFilter

IMPLEMENT_DYNCREATE(CQueryFileFilter, CCmdTargetEx)


// {8FBBD046-C050-431f-B805-7206B7BEA33A}
GUARD_IMPLEMENT_OLECREATE(CQueryFileFilter, "FileFilters.QueryFileFilter", 
0x8fbbd046, 0xc050, 0x431f, 0xb8, 0x5, 0x72, 0x6, 0xb7, 0xbe, 0xa3, 0x3a);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CQueryFileFilter::CQueryFileFilter()
{
	_OleLockApp( this );
	m_strDocumentTemplate = szDocumentDBase;
	m_strFilterName.LoadString( IDS_QUERY_FILTERNAME );
	m_strFilterExt = _T(".query");
	AddDataObjectType(szTypeQueryObject);
}

CQueryFileFilter::~CQueryFileFilter()
{
	_OleUnlockApp( this );
}


bool CQueryFileFilter::ReadFile( const char *pszFileName )
{
	IUnknown	*punk = 0;
	try
	{
		CString strType;
		if(m_sptrINamedData)
			strType = GetDataObjectType(0);	
		else
			strType = szTypeQueryObject;
		punk = CreateNamedObject( strType );
		// get object name from path and check it's exists
		CString	strObjName = ::GetObjectNameFromPath(pszFileName);
		_bstr_t bstrName(strObjName);
		long	NameExists = 0;
	
		if(m_sptrINamedData)
			m_sptrINamedData->NameExists(bstrName, &NameExists);
		
		if(CheckInterface(punk, IID_ISerializableObject))
		{
			CFile	file( pszFileName, CFile::modeRead );
			sptrISerializableObject	sptrO( punk );
			{
				SerializeParams	params;
				ZeroMemory( &params, sizeof( params ) );

				CArchive		ar( &file, CArchive::load );
				CArchiveStream	ars( &ar );
				sptrO->Load( &ars, &params );
			}
		}

		// if new name not exists allready
		if (!NameExists && punk)
		{
			// set this name to object
			INamedObject2Ptr sptrObj(punk);
			sptrObj->SetName(bstrName);
		}

		punk->Release();
		punk = 0;
	}
	catch(CException* e)
	{
		e->Delete();
		if(punk)
			punk->Release();
		return false;
	}
	return true;
}

bool CQueryFileFilter::WriteFile( const char *pszFileName )
{
	IUnknown* punk = 0;
	try
	{
		// convert object type to BSTR
		_bstr_t bstrType( GetDataObjectType(0) );

		// get active object in which type == m_strObjKind
		if (!(m_bObjectExists && m_sptrAttachedObject != NULL) && m_sptrIDataContext != NULL)
			m_sptrIDataContext->GetActiveObject( bstrType, &punk );
		else
		{
			punk = (IUnknown*)m_sptrAttachedObject.GetInterfacePtr();
			punk->AddRef();
		}
	
		if( !punk )
			return false;

		if(CheckInterface(punk, IID_ISerializableObject))
		{
			CFile	file( pszFileName, CFile::modeCreate|CFile::modeWrite );
			sptrISerializableObject	sptrO( punk );
			{
				SerializeParams	params;
				ZeroMemory( &params, sizeof( params ) );

				CArchive		ar( &file, CArchive::store );
				CArchiveStream	ars( &ar );

				sptrO->Store( &ars, &params );
			}
		}
		punk->Release();
		punk = 0;

	}
	catch(CException* e)
	{
		e->Delete();
		if(punk)
			punk->Release();
		return false;
	}
	return true;
}

bool CQueryFileFilter::BeforeReadFile()
{
	return CFileFilterBase::BeforeReadFile();
}

bool CQueryFileFilter::AfterReadFile()
{
	return CFileFilterBase::AfterReadFile();
}
