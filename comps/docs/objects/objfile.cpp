#include "stdafx.h"
#include "objfile.h"
#include "resource.h"
#include "afxpriv2.h"

IMPLEMENT_DYNCREATE(CObjectFileFilter, CCmdTargetEx)

// {99F7AEA6-1621-4d3d-9B83-DDA060339A94}
GUARD_IMPLEMENT_OLECREATE(CObjectFileFilter, "Objects.FileFilter", 
0x99f7aea6, 0x1621, 0x4d3d, 0x9b, 0x83, 0xdd, 0xa0, 0x60, 0x33, 0x9a, 0x94);

CObjectFileFilter::CObjectFileFilter()
{
	_OleLockApp( this );

	m_strFilterName.LoadString( IDS_FILTERNAME );
	m_strFilterExt = _T(".objects");
	AddDataObjectType( szTypeObjectList );

}
CObjectFileFilter::~CObjectFileFilter()
{
	_OleUnlockApp( this );
}

bool CObjectFileFilter::ReadFile( const char *pszFileName )
{
	IUnknown	*punk = 0;
	try
	{
		CString strType;
		if(m_sptrINamedData)
			strType = GetDataObjectType(0);	
		else
			strType = szTypeObjectList;
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
		e->ReportError( MB_ICONERROR );
		e->Delete();
		if(punk)
			punk->Release();
		return false;
	}
	return true;
}


bool CObjectFileFilter::WriteFile( const char *pszFileName )
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


		//make all images no virtual
		TPOS	lpos = 0;
		INamedDataObject2Ptr	ptrO( punk );

		ptrO->GetFirstChildPosition( &lpos );
		while( lpos )
		{
			IUnknown	*punkObj = 0;
			ptrO->GetNextChild( &lpos, &punkObj );

			ASSERT(punkObj);

			CObjectWrp	object( punkObj, false );
			IUnknown	*punkImage = object.GetImage();
			INamedDataObject2Ptr	image( punkImage );
			if( punkImage )punkImage->Release();

			if( image != 0 )
				image->StoreData( sdfCopyParentData );
		}

		if(CheckInterface(punk, IID_ISerializableObject))
		{
			CFile	file( pszFileName, CFile::modeCreate|CFile::modeWrite );
			sptrISerializableObject	sptrO( punk );
			{
				CArchive		ar( &file, CArchive::store );
				CArchiveStream	ars( &ar );


				SerializeParams	params;
				ZeroMemory( &params, sizeof( params ) );
				params.flags = sf_ImageCompressed;

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