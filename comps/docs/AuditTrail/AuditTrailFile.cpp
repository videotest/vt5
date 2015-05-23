// AuditTrailFile.cpp : implementation file
//

#include "stdafx.h"
#include "audittrail.h"
#include "AuditTrailFile.h"
#include "afxpriv2.h"


/////////////////////////////////////////////////////////////////////////////
// CAuditTrailFile

IMPLEMENT_DYNCREATE(CAuditTrailFile, CCmdTarget)

CAuditTrailFile::CAuditTrailFile()
{
	_OleLockApp( this );

	m_strFilterName.LoadString(IDS_FILTERNAME);
	m_strFilterExt = _T(".at");
	AddDataObjectType(szTypeAuditTrailObject);
}

CAuditTrailFile::~CAuditTrailFile()
{
	_OleUnlockApp( this );
}

// {18FE2888-4BD6-4abc-9178-BECDA8598F8F}
GUARD_IMPLEMENT_OLECREATE(CAuditTrailFile, "AuditTrail.AuditTrailFile", 
0x18fe2888, 0x4bd6, 0x4abc, 0x91, 0x78, 0xbe, 0xcd, 0xa8, 0x59, 0x8f, 0x8f);

/////////////////////////////////////////////////////////////////////////////
// CAuditTrailFile 


bool CAuditTrailFile::ReadFile( const char *pszFileName )
{
	IUnknown	*punk = 0;
	try
	{
		CString strType;
		if(m_sptrINamedData)
			strType = GetDataObjectType(0);	
		else
			strType = szTypeAuditTrailObject;
		punk = CreateNamedObject( strType );
		// get object name from path and check it's exists
		CString	strObjName = ::GetObjectNameFromPath(pszFileName);
		_bstr_t bstrName(strObjName);
		long	NameExists = 0;
	
		if(m_sptrINamedData)
			m_sptrINamedData->NameExists(bstrName, &NameExists);
		
		if(CheckInterface(punk, IID_ISerializableObject))
		{
			SerializeParams	params;
			ZeroMemory( &params, sizeof( params ) );

			CFile	file( pszFileName, CFile::modeRead );
			sptrISerializableObject	sptrO( punk );
			{
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
		e->ReportError();
		e->Delete();
		if(punk)
			punk->Release();
		return false;
	}
	return true;
}


bool CAuditTrailFile::WriteFile( const char *pszFileName )
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
		e->ReportError();
		e->Delete();
		if(punk)
			punk->Release();
		return false;
	}
	return true;
}