#include "stdafx.h"
#include "resource.h"
#include "configfile.h"
#include "scriptdocint.h"

#include "scriptdataobject.h"

//IMPLEMENT_DYNCREATE(CConfigFileFilter, CCmdTarget);
IMPLEMENT_DYNCREATE(CScriptFileFilter, CCmdTarget);

// {E8C7ABD2-1DAE-11d3-A5D0-0000B493FF1B}
//GUARD_IMPLEMENT_OLECREATE(CConfigFileFilter, "ScriptDoc.ConfigFileFilter",
//0xe8c7abd2, 0x1dae, 0x11d3, 0xa5, 0xd0, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {15BA1FE3-1E55-11d3-A5D1-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CScriptFileFilter, "ScriptDoc.ScriptFileFilter",
0x15ba1fe3, 0x1e55, 0x11d3, 0xa5, 0xd1, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);

/*CConfigFileFilter::CConfigFileFilter()
{
	m_strFilter.LoadString( IDS_CONFIG_FILTER );
	m_strDocument.LoadString( IDS_CONFIG_NAME );
}

const char *CConfigFileFilter::GetDocumentString()
{
	return m_strDocument;
}

const char *CConfigFileFilter::GetFilterString()
{
	return m_strFilter;
}

const char *szEndOfScript = "!end of script";

bool CConfigFileFilter::ReadSectionHook( CStringArrayEx &sa, const char *pszSection, int &ipos )
{
	if( strncmp( "script", pszSection, strlen( "script" ) ) )
		return false;

	CString	strScript, strScriptName;

	strScriptName = sa[ipos++];

	CString str = sa[ipos++];

	while( str != szEndOfScript )
	{
		strScriptName += str;
		strScriptName += "\n";

		str = sa[ipos++];
	}

	m_pNamedData->SetValue( CString( pszSection )+"Data", strScript );
	m_pNamedData->SetValue( CString( pszSection )+"Name", strScriptName );

	return true;
}

bool CConfigFileFilter::WriteSectionHook( CStringArrayEx &sa, const char *pszSection )
{
	if( strncmp( "script", pszSection, strlen( "script" ) ) )
		return false;

	CString	strScript, strScriptName;

	m_pNamedData->GetValue( CString( pszSection )+"Data", strScript );
	m_pNamedData->GetValue( CString( pszSection )+"Name", strScriptName );

	sa.Add( strScriptName );

	while( !strScript.IsEmpty() )
	{
		CString	strLine;

		int	idx = strScript.Find( "\n" );

		if( idx != -1 )
		{
			strLine = strScript.Left( idx );
			strScript = strScript.Right( strScript.GetLength()-idx-1 );
		}
		else
		{
			strLine = strScript;
			strScript.Empty();
		}
		sa.Add( strScript );
	}
	sa.Add( szEndOfScript );

	return true;
}*/
//////////////////////////////////////////////////////////////////////////////////////////

CScriptFileFilter::CScriptFileFilter()
{
	m_strFilter.LoadString( IDS_SCRIP_FILTER );
	m_strDocument.LoadString( IDS_SCRIPT_NAME );

	m_strDocumentTemplate = szDocumentScript;
	m_strFilterName.LoadString( IDS_SCRIPT_NAME );
	m_strFilterExt.LoadString( IDS_SCRIP_FILTER );
// !!! klimov	
	AddDataObjectType(CScriptDataObject::m_szType);
// !!!
}

bool CScriptFileFilter::ReadFile(const char *pszFileName)
{
	IUnknown * punk = 0;
	try
	{
		_bstr_t	bstr( pszFileName );
	
		// create Object
		punk = (this)->CreateNamedObject(GetDataObjectType(0));
		sptrIScriptDataObject sptr(punk);
		if (punk)
		{
			punk->Release();
			punk = 0;
		}

		// get object name from path and check it's exists
		CString	strObjName = ::GetObjectNameFromPath(pszFileName);
		_bstr_t bstrName(strObjName);

		INamedObject2Ptr sptrObj((IUnknown*)sptr.GetInterfacePtr());
		sptrObj->SetName(bstrName);

		if( sptr->ReadFile(bstr) != S_OK )
			return false;
	}
	catch( CException *pe )
	{
		pe->ReportError();
		pe->Delete();
		if (punk)punk->Release();
		return false;
	}
	return true;
}

bool CScriptFileFilter::WriteFile( const char *pszFileName )
{
	try
	{
		_bstr_t	bstr( pszFileName );
		
		// convert object type to BSTR
		_bstr_t bstrType(GetDataObjectType(0));

		// get active object in which type == m_strObjKind
		IUnknown* punk = NULL;

		if (!(m_bObjectExists && m_sptrAttachedObject != NULL))
			m_sptrIDataContext->GetActiveObject(bstrType, &punk);
		else 
			punk = (IUnknown*)m_sptrAttachedObject.GetInterfacePtr();

		// addition check interface
		if (!CheckInterface(punk, IID_IScriptDataObject))
			return false;

		sptrIScriptDataObject	sptr(punk);
		punk->Release();
		// and write file
		sptr->WriteFile( bstr );
	}
	catch( CException *pe )
	{
		pe->ReportError();
		pe->Delete();
		return false;
	}
	return true;
}

bool CScriptFileFilter::InitNew()
{
	_try (CScriptFileFilter, WriteFile)
	{
//		sptrIScriptDataObject sptr( m_sptrINamedDataObject );
//		sptr->InitNew();
	}
	_catch
	{
		return false;
	}
	return true;
}
