#include "StdAfx.h"
#include "resource.h"
#include "idiofilefilter.h"
#include "idiodbasedataobject.h"

/*
IMPLEMENT_DYNCREATE(CIdioFileFilter, CCmdTarget);

// {3BEB69D8-3D74-48d9-AEB0-2157EF1C31BD}
GUARD_IMPLEMENT_OLECREATE(CIdioFileFilter, "IdioDBase.IdioFileFilter",
0x3beb69d8, 0x3d74, 0x48d9, 0xae, 0xb0, 0x21, 0x57, 0xef, 0x1c, 0x31, 0xbd);

//////////////////////////////////////////////////////////////////////////////////////////
CIdioFileFilter::CIdioFileFilter()
{
	m_strFilter.LoadString( IDS_IDIO_FILTER );
	m_strDocument.LoadString( IDS_IDIO_NAME );

	m_strDocumentTemplate = szDocumentScript;
	m_strFilterName.LoadString( IDS_IDIO_NAME );
	m_strFilterExt.LoadString( IDS_IDIO_FILTER );

	AddDataObjectType(CIdioDBaseDataObject::m_szType);
}

bool CIdioFileFilter::ReadFile(const char *pszFileName)
{
	return true;
}

bool CIdioFileFilter::WriteFile( const char *pszFileName )
{
	return true;
}

bool CIdioFileFilter::InitNew()
{
	_try (CIdioFileFilter, InitNew)
	{
	}
	_catch
	{
		return false;
	}
	return true;
}
*/