// ScriptDataObject.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptDoc.h"
#include "ScriptDataObject.h"
#include "DataBase.h"
#include "ScriptNotifyInt.h"
#include "docmiscint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



char	*CScriptDataObject::m_szType = szArgumentTypeScript;
char	szDelimiter[] = "\n#";

IMPLEMENT_DYNCREATE(CScriptDataObject, CCmdTargetEx)

// {EB533972-24AB-11d3-A5D9-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CScriptDataObject, "ScriptDoc.ScriptDataObject",
0xeb533972, 0x24ab, 0x11d3, 0xa5, 0xd9, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);

// {7D71FE41-39E9-11d3-87C6-0000B493FF1B}
static const GUID clsidCScriptDataObjectInfo = 
{ 0x7d71fe41, 0x39e9, 0x11d3, { 0x87, 0xc6, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };

// define NamedDataObjectInfo
DATA_OBJECT_INFO(IDS_SCRIPT_TYPE, CScriptDataObject, CScriptDataObject::m_szType, clsidCScriptDataObjectInfo, IDI_THUMBNAIL_SCRIPT)

/////////////////////////////////////////////////////////////////////////////
// CScriptDataObject

BSTR remove_comments( CString &strScript )
{
	if( strScript.IsEmpty() ) 
		return 0;

	CString Res;

	int nSz = strScript.GetLength();

	if( strScript.Find( "\'", 0 ) != -1 )
	{
		bool bInside = false;
		for( int i = 0; i < nSz;i++ )
		{
			if( !bInside && strScript[i] == '\'' )
				bInside = true;
			else if( bInside && strScript[i] == char( 10 ) )
			{
				bInside = false;
				Res += strScript[i];
			}
			else if( !bInside )
				Res += strScript[i];
		}
	}
	else
		Res = strScript;

	if( !Res.IsEmpty() )
		return Res.AllocSysString();

	return 0;
}

CScriptDataObject::CScriptDataObject()
{
	_OleLockApp( this );
	EnableAutomation();

	m_buffer.InitNew( CRLF_STYLE_DOS );
}

CScriptDataObject::~CScriptDataObject()
{
	//::TerminateScripts( m_lKey );

	m_buffer.FreeAll();

	_OleUnlockApp( this );
}

void CScriptDataObject::OnChangeName()
{
	if( m_strOldName == m_sName )
		return;

	if( m_sName.IsEmpty() )
		return;

	if(  ::IsDynamicActionRegistrationEnabled() )
		m_info.Init( this );
	m_strOldName = m_sName;
}


void CScriptDataObject::OnScriptChange()
{
	//get the last action in undo buffer. If the target key match,
	//do nothisng. Else add action ScriptChange
}


void CScriptDataObject::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.
	m_info.DeInit();

	CDataObjectBase::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CScriptDataObject, CDataObjectBase)
	//{{AFX_MSG_MAP(CScriptDataObject)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CScriptDataObject, CDataObjectBase)
	//{{AFX_DISPATCH_MAP(CScriptDataObject)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IScriptDataObject to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {39346386-2480-11D3-A5D9-0000B493FF1B}
static const IID IID_IScriptDataObjectDisp =
{ 0x39346386, 0x2480, 0x11d3, { 0xa5, 0xd9, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };


BEGIN_INTERFACE_MAP(CScriptDataObject, CDataObjectBase)
	INTERFACE_PART(CScriptDataObject, IID_IScriptDataObjectDisp, Dispatch)
	INTERFACE_PART(CScriptDataObject, IID_IScriptDataObject, ScriptDataObj)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CScriptDataObject, ScriptDataObj);

/////////////////////////////////////////////////////////////////////////////
// CScriptDataObject message handlers
HRESULT CScriptDataObject::XScriptDataObj::InitNew()
{
	_try_nested(CScriptDataObject, ScriptDataObj, InitNew);
	{
		pThis->m_buffer.FreeAll();
		pThis->m_buffer.InitNew( CRLF_STYLE_DOS );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptDataObject::XScriptDataObj::ReadFile( BSTR bstr )
{
	_try_nested(CScriptDataObject, ScriptDataObj, ReadFile);
	{
		_bstr_t	bstrFileName( bstr );

		pThis->m_buffer.FreeAll();
		if( !pThis->m_buffer.LoadFromFile( bstrFileName ) )
			return E_FAIL;

		pThis->m_info.CheckScriptInfo();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptDataObject::XScriptDataObj::WriteFile( BSTR bstr )
{
	_try_nested(CScriptDataObject, ScriptDataObj, WriteFile);
	{
		_bstr_t	bstrFileName( bstr );
		pThis->m_buffer.SaveToFile( bstrFileName );
		pThis->m_info.CheckScriptInfo();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptDataObject::XScriptDataObj::GetText( BSTR *pbstr )
{
	_try_nested(CScriptDataObject, ScriptDataObj, GetText);
	{
		int	iLinesCount = pThis->m_buffer.GetLineCount();

		if( iLinesCount < 1 )
		{
			*pbstr = 0;
			return S_OK;
		}

		int	iLineLength = pThis->m_buffer.GetLineLength( iLinesCount - 1 );

		CString	str;

		if( iLineLength != 0 ||iLinesCount != 1 )
			pThis->m_buffer.GetText( 0, 0, iLinesCount-1, iLineLength, str, "\n" );

		*pbstr = remove_comments( str );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptDataObject::XScriptDataObj::GetFullText( BSTR *pbstr )
{
	_try_nested(CScriptDataObject, ScriptDataObj, GetFullText);
	{
		int	iLinesCount = pThis->m_buffer.GetLineCount();

		if( iLinesCount < 1 )
		{
			*pbstr = 0;
			return S_OK;
		}

		int	iLineLength = pThis->m_buffer.GetLineLength( iLinesCount - 1 );

		CString	str;

		if( iLineLength != 0 ||iLinesCount != 1 )
			pThis->m_buffer.GetText( 0, 0, iLinesCount-1, iLineLength, str, "\n" );

		*pbstr = str.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptDataObject::XScriptDataObj::GetLinesCount( long *pnCount )
{
	_try_nested(CScriptDataObject, ScriptDataObj, GetLinesCount);
	{
		if(pnCount)
			*pnCount = pThis->m_buffer.GetLineCount();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptDataObject::XScriptDataObj::GetLine( long index, BSTR *pbstrText )
{
	_try_nested(CScriptDataObject, ScriptDataObj, GetLine);
	{
		if(pbstrText)
		{
			CString str = pThis->m_buffer.GetLineChars(index);
			*pbstrText = str.AllocSysString();
		}
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptDataObject::XScriptDataObj::InsertString( BSTR bstr, long index )
{
	_try_nested(CScriptDataObject, ScriptDataObj, InsertString);
	{
		CString str(bstr);
		str+="\n";

		int nEndLine, nEndChar;
		pThis->m_buffer.InsertText( 0, min( index, pThis->m_buffer.GetLineCount()-1), 
			0, str, nEndLine, nEndChar, CE_ACTION_TYPING );
		
		//pThis->m_buffer.InsertLine(str, -1, index);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptDataObject::XScriptDataObj::DeleteString( long index )
{
	_try_nested(CScriptDataObject, ScriptDataObj, DeleteString);
	{
		pThis->m_buffer.RemoveLine(index);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptDataObject::XScriptDataObj::GetPrivateDataPtr( DWORD *pdwDataPtr )
{
	_try_nested(CScriptDataObject, ScriptDataObj, GetPrivateDataPtr);
	{
		*pdwDataPtr = (DWORD)&pThis->m_buffer;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptDataObject::XScriptDataObj::RegisterScriptAction()
{
	_try_nested(CScriptDataObject, ScriptDataObj, RegisterScriptAction);
	{
		pThis->m_info.Init( pThis );
		return S_OK;
	}
	_catch_nested;
}

bool CScriptDataObject::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	CDataObjectBase::SerializeObject( ar, pparams );

	if( ar.IsStoring() )
	{
		long	nLinesCount = m_buffer.GetLineCount();
		ar<<nLinesCount;

		for( long i = 0; i < nLinesCount; i++ )
		{
			CString	str( m_buffer.GetLineChars( i ), m_buffer.GetLineLength( i ) );
			ar<<str;
		}
	}
	else
	{
		m_buffer.FreeAll();
		m_buffer.InitNew( CRLF_STYLE_DOS );

		long	nLinesCount = 0;
		ar>>nLinesCount;

		for( long i = 0; i < nLinesCount; i++ )
		{
			CString	str;
			ar>>str;

			if( i == 0 )
				m_buffer.AppendLine( i, str );
			else
				m_buffer.InsertLine( str );

		}

	}

	m_info.CheckScriptInfo();

	return true;
}

bool CScriptDataObject::CanBeBaseObject()
{
	return true;
}

bool CScriptDataObject::GetTextParams( CString &strParams )
{
	int	iLinesCount = m_buffer.GetLineCount();

	if( iLinesCount < 1 )
		return true;

	int	iLineLength = m_buffer.GetLineLength( iLinesCount - 1 );

	if( iLineLength != 0 ||iLinesCount != 1 )
		m_buffer.GetText( 0, 0, iLinesCount-1, iLineLength, strParams, "\n" );

	return true;
}

bool CScriptDataObject::SetTextParams( const char *pszParams )
{
	int	nLines, nChars;
	m_buffer.FreeAll();
	m_buffer.InitNew( CRLF_STYLE_DOS );
	m_buffer.InsertText( 0, 0, 0, pszParams, nLines, nChars, CE_ACTION_TYPING );

	m_info.CheckScriptInfo();

	return true;
}

bool CScriptDataObject::IsModified()
{
	return m_buffer.IsModified() == TRUE;
}

void CScriptDataObject::SetModifiedFlag( bool bSet )
{
	m_buffer.SetModified( bSet );
}

/////////////////////////////////////////////////////////////////////////////////////////
///ActionInfo
BEGIN_INTERFACE_MAP(CScriptActionInfo, CCmdTargetEx)
	INTERFACE_PART(CScriptActionInfo, IID_IActionInfo, Info)
	INTERFACE_PART(CScriptActionInfo, IID_IActionInfo2, Info)
	INTERFACE_PART(CScriptActionInfo, IID_IActionInfo3, Info)
	INTERFACE_PART(CScriptActionInfo, IID_INumeredObject, Num)
	INTERFACE_PART(CScriptActionInfo, IID_IHelpInfo, Help)
	INTERFACE_PART(CScriptActionInfo, IID_IHelpInfo2, Help)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CScriptActionInfo, Info);
IMPLEMENT_UNKNOWN(CScriptActionInfo, Help);

CScriptActionInfo::CScriptActionInfo()
{
	m_bInit = false;
	m_StateAnswer = Nothing;
	m_pDataObject = 0;
	m_nLocalID = -1;
	m_bStringsLoaded = false;
	m_spunkBmpHelper = 0;
	m_dwID = 0;
}

CScriptActionInfo::~CScriptActionInfo()
{
	//::TerminateScripts( m_lKey );
	ASSERT(!m_bInit);
}

void CScriptActionInfo::Init( CScriptDataObject	*p )
{
	if( m_bInit )	
	{
		DeInit();
	}

	{
		m_pDataObject = p;
		CheckScriptInfo();
	//get the ActionManager
		IUnknown	*punkA = 0;
		sptrIApplication	sptrA( GetAppUnknown() );
		sptrA->GetActionManager( &punkA );

		m_ptrAM = punkA;
		m_ptrCM = punkA;

		// [vanek] : generate id - 13.12.2004
		m_dwID = generateScriptActionID();

		ASSERT(m_ptrCM!= 0 );
		punkA->Release();

		m_ptrCM->AddComponent( GetControllingUnknown(), 0 );

		m_bInit = true;
	}
}
					
void CScriptActionInfo::DeInit()
{
	if( !m_bInit )
		return;

	int	nCompCount = 0;

	GuidKey nKey = m_key;
	bool	bFound = false;

	m_ptrCM->GetCount( &nCompCount );

	for( int nComp = 0; nComp < nCompCount; nComp++ )
	{
		IUnknown *punk = 0;
		m_ptrCM->GetComponentUnknownByIdx( nComp, &punk );

		if( !punk )
			continue;

		if( ::GetObjectKey( punk ) == nKey )
		{
			m_ptrCM->RemoveComponent( nComp );
			punk->Release();

			bFound = true;
			break;
		}

		punk->Release();
	}

	m_dwID = 0;
	m_bInit = false;
	ASSERT(bFound );
}

HRESULT CScriptActionInfo::XInfo::GetLocalID( int *pnID )
{
	METHOD_PROLOGUE_EX(CScriptActionInfo, Info);
	*pnID = pThis->m_nLocalID;
	return S_OK;
}
HRESULT CScriptActionInfo::XInfo::SetLocalID( int nID )
{
	METHOD_PROLOGUE_EX(CScriptActionInfo, Info);
	pThis->m_nLocalID = nID;
	return S_OK;
}


HRESULT CScriptActionInfo::XInfo::GetArgsCount( int *piParamCount )
{
	_try_nested(CScriptActionInfo, Info, GetArgsCount)
	{
		*piParamCount = 0;
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////////////////
//CHelpInfoImpl



HRESULT CScriptActionInfo::XHelp::GetHelpInfo( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags )
{
	METHOD_PROLOGUE_EX(CScriptActionInfo, Help)

	CString	str = ::GetObjectName( pThis->m_pDataObject->GetControllingUnknown() );
	CString	strHelpFileName = "as";
	strHelpFileName+=str;
	//strHelpFileName+=".chm";
	
	//::HelpGenerateFileName( strHelpFileName, "as", str );
	
	if( pbstrHelpFile )
		*pbstrHelpFile = strHelpFileName.AllocSysString();
	if( pbstrHelpTopic ) 
		*pbstrHelpTopic = str.AllocSysString();
	if( pdwFlags )
		*pdwFlags = 0;

	return S_OK;
}

HRESULT CScriptActionInfo::XHelp::GetHelpInfo2( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags, BSTR *pbstrHelpPrefix )
{
    METHOD_PROLOGUE_EX(CScriptActionInfo, Help)

	CString	str = ::GetObjectName( pThis->m_pDataObject->GetControllingUnknown() );
	CString	strHelpFileName = str;
	CString strPrefix( _T("as") );
	
	if( pbstrHelpFile )	*pbstrHelpFile = strHelpFileName.AllocSysString();
	if( pbstrHelpTopic ) *pbstrHelpTopic = str.AllocSysString();
	if( pdwFlags )       *pdwFlags = 0;
	if( pbstrHelpPrefix ) *pbstrHelpPrefix = strPrefix.AllocSysString();

	return S_OK;
}



HRESULT CScriptActionInfo::XInfo::GetArgInfo( int iParamIdx, 
											 BSTR *pbstrName, 
											 BSTR *pbstrKnid, 
											 BSTR *pbstrDefValue )
{
	_try_nested(CScriptActionInfo, Info, GetArgsCount)
	{
		if( pbstrName ) *pbstrName = 0;
		if( pbstrKnid ) *pbstrKnid = 0;
		if( pbstrDefValue ) *pbstrDefValue = 0;

		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptActionInfo::XInfo::GetOutArgsCount( int *piParamCount )
{
	_try_nested(CScriptActionInfo, Info, GetOutArgsCount)
	{
		*piParamCount = 0;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptActionInfo::XInfo::GetOutArgInfo( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKind )
{
	_try_nested(CScriptActionInfo, Info, GetOutArgInfo)
	{
		if( pbstrName ) *pbstrName = 0;
		if( pbstrKind ) *pbstrKind = 0;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptActionInfo::XInfo::IsFunction( BOOL *pboolAction )
{
	_try_nested(CScriptActionInfo, Info, IsFunction)
	{
		*pboolAction = FALSE;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptActionInfo::XInfo::GetCommandInfo( 
	//general information
	BSTR	*pbstrActionName,
	BSTR	*pbstrActionUserName,
	BSTR	*pbstrActionHelpString, //"Image rotation\nRotation
	BSTR	*pbstrGroupName	)
{
	_try_nested(CScriptActionInfo, Info, GetCommandInfo)
	{

		if( pbstrActionName )
		{
			INamedObject2Ptr	sptrN( pThis->m_pDataObject->GetControllingUnknown() );
			sptrN->GetName( pbstrActionName );
		}

		if( !pThis->m_bStringsLoaded )
		{
			INamedObject2Ptr	sptrN( pThis->m_pDataObject->GetControllingUnknown() );

			BSTR	bstr = 0;
			sptrN->GetUserName( &bstr );

			pThis->m_strUserName = bstr;
			pThis->m_strHelpString = bstr;
			pThis->m_strGroupName.LoadString( IDS_SCRIPTS );

			::SysFreeString( bstr );

			pThis->m_bStringsLoaded = true;
		}

		if( pbstrActionUserName )
			*pbstrActionUserName = pThis->m_strUserName.AllocSysString();
		if( pbstrActionHelpString )
			*pbstrActionHelpString = pThis->m_strHelpString.AllocSysString();
		if( pbstrGroupName )
			*pbstrGroupName = pThis->m_strGroupName.AllocSysString();
	
		return S_OK;
	}
	_catch_nested;
}


HRESULT CScriptActionInfo::XInfo::GetTBInfo(
	//toolbar information
	BSTR	*pbstrDefTBName,	//default toolbar name (as "edit operaion")
	IUnknown **ppunkBitmapHelper )
{
	_try_nested(CScriptActionInfo, Info, GetTBInfo)
	{
		if( pbstrDefTBName )
			*pbstrDefTBName = 0;

		if( ppunkBitmapHelper )
		{
			*ppunkBitmapHelper = 0;

			// [vanek] : support IActionInfo3 interface - 13.12.2004
			if( pThis->m_spunkBmpHelper != 0 )
			{
				*ppunkBitmapHelper = pThis->m_spunkBmpHelper;
				(*ppunkBitmapHelper)->AddRef();
			}
		}

		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptActionInfo::XInfo::GetHotKey( DWORD *pdwKey, BYTE *pbVirt )
{
	_try_nested(CScriptActionInfo, Info, GetHotKey)
	{
		*pdwKey = 0;
		*pbVirt = 0;
		return S_FALSE;
	}
	_catch_nested;
}

	//menu information
HRESULT CScriptActionInfo::XInfo::GetRuntimeInformaton( DWORD *pdwDllCode, DWORD *pdwCmdCode )
{
	_try_nested(CScriptActionInfo, Info, GetArgsCount)
	{
		*pdwDllCode = 0;
		*pdwCmdCode = /*0*/pThis->m_dwID;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptActionInfo::XInfo::GetMenuInfo(
	BSTR	*pbstrMenus,		//"general", "image doc", ... if action avaible in several menus, it should 
	DWORD	*pdwMenuFlag )		//not used, must be zero. Will be used later, possible, 
								//for special sub-menu (such as "Recent files")
{
	_try_nested(CScriptActionInfo, Info, GetArgsCount)
	{
		*pbstrMenus = 0;
		*pdwMenuFlag = 0;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptActionInfo::XInfo::ConstructAction( IUnknown **ppunk )
{
	_try_nested(CScriptActionInfo, Info, GetArgsCount)
	{
		CScriptAction	*psa = new CScriptAction( pThis->m_pDataObject );

		IUnknown	*punk = psa->GetControllingUnknown();

		if( CheckInterface( punk, IID_IRootedObject ) )
		{
			IRootedObjectPtr	sptr( punk );
			sptr->AttachParent( this );
		}
		*ppunk = punk;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptActionInfo::XInfo::GetTargetName( BSTR *pbstr )
{
	_try_nested(CScriptActionInfo, Info, GetArgsCount)
	{
		CString	str( "app" );
		*pbstr = str.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptActionInfo::XInfo::IsArgumentDataObject( int iParamIdx, BOOL bInArg, BOOL *pbIs )
{
	*pbIs = false;
	return S_OK;
}
HRESULT CScriptActionInfo::XInfo::GetArgInfo2( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKnid, BSTR *pbstrDefValue, int* pNumber )
{
	return E_INVALIDARG;
}

HRESULT CScriptActionInfo::XInfo::GetOutArgInfo2( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKind, int* pNumber )
{
	return E_INVALIDARG;
}

HRESULT CScriptActionInfo::XInfo::GetUIUnknown( IUnknown **ppunkinterface )
{
	*ppunkinterface = 0;
	return S_OK;
}

HRESULT CScriptActionInfo::XInfo::SetActionString( BSTR	bstr )
{
	METHOD_PROLOGUE_EX(CScriptActionInfo, Info);

	_bstr_t	bstrT( bstr );
	
	pThis->m_strUserName = strtok( bstrT, szDelimiter );
	pThis->m_strGroupName = strtok( 0, szDelimiter );
	pThis->m_strHelpString = strtok( 0, szDelimiter );
	pThis->m_strHelpString +="\n";
	pThis->m_strHelpString +=strtok( 0, szDelimiter );
	pThis->m_bStringsLoaded = true;

	return S_OK;
}

HRESULT CScriptActionInfo::XInfo::SetBitmapHelper( IUnknown *punkBitmapHelper )
{
    METHOD_PROLOGUE_EX(CScriptActionInfo, Info);

	if( !punkBitmapHelper )
		return E_INVALIDARG;

	pThis->m_spunkBmpHelper = punkBitmapHelper;
	return S_OK;
}

const char szInfoSign[] = "'info:";
const char szResourceSign[] = "'resource:";
const char szStateActionSign[] = "'state:action=";
const char szStateDockSign[] = "'state:dock=";
const char szStateToolSign[] = "'state:toolbar=";
const char szStateFunctionSign[] = "'state:function=";

void CScriptActionInfo::CheckScriptInfo()
{
	m_StateAnswer = Nothing;
	char	szLine[255];
	if( !m_pDataObject )return;
	if( !m_pDataObject->m_buffer.GetLineCount() )return;

	int	nLinesCount = m_pDataObject->m_buffer.GetLineCount(), nLine;

	CStringArray	sarrFirstComments;

	for( nLine = 0; nLine < nLinesCount && nLine < 10 ; nLine++ )
	{
		strncpy( szLine, m_pDataObject->m_buffer.GetLineChars( nLine ), 255 );
		szLine[min( m_pDataObject->m_buffer.GetLineLength( nLine ), 254 )] = 0;
		if( szLine[0] != '\'' )break;

		if( !strncmp( szLine, szInfoSign, strlen( szInfoSign ) ) )
		{
			char	*pszAliase = szLine+strlen( szInfoSign );

			if( *pszAliase == '(' )
			{
				char	*pe = strchr( pszAliase, ')' );
				if( pe )
				{
					static CString	str;
					if( str.IsEmpty() )
						str = GetValueString( GetAppUnknown(), "\\General", "Language" );

					if( !str.IsEmpty() && strncmp( pszAliase+1, str, (long)pe-(long)pszAliase-1 ) )
						continue;
				}
				pszAliase = pe+1;

			}

			m_strUserName = strtok( pszAliase, szDelimiter );
			m_strGroupName = strtok( 0, szDelimiter );
			m_strHelpString = strtok( 0, szDelimiter );
			m_strHelpString +="\n";
			m_strHelpString +=strtok( 0, szDelimiter );
			m_bStringsLoaded = true;
			continue;
		}
		else if( !strncmp( szLine, szResourceSign, strlen( szResourceSign ) ) )
		{
			char* psz_resource = szLine + strlen( szResourceSign );

			IFormNamesDataPtr ptrFrmNms = ::GetAppUnknown();
			if( ptrFrmNms )
			{
				BSTR bstr = 0;
				ptrFrmNms->GetFormName( _bstr_t( psz_resource ), &bstr );
				CString str_value = bstr;
				::SysFreeString( bstr );	bstr = 0;
				static char sz_buf[256];	sz_buf[0] = 0;
				if( str_value.GetLength() > sizeof(sz_buf) - 1 )
					continue;
				strcpy( sz_buf, str_value );

				m_strUserName = strtok( sz_buf, szDelimiter );
				m_strGroupName = strtok( 0, szDelimiter );
				m_strHelpString = strtok( 0, szDelimiter );
				m_strHelpString +="\n";
				m_strHelpString +=strtok( 0, szDelimiter );
				m_bStringsLoaded = true;
				continue;
			}
		}
		else if( !strncmp( szLine, szStateActionSign, strlen( szStateActionSign ) ) )
		{
			m_bstrStateAction = szLine+strlen( szStateActionSign );
			m_StateAnswer = Action;
			continue;
		}
		else if( !strncmp( szLine, szStateDockSign, strlen( szStateDockSign ) ) )
		{
			m_bstrStateDockBar = szLine+strlen( szStateDockSign );
			m_StateAnswer = DockBar;
			continue;
		}
		else if( !strncmp( szLine, szStateToolSign, strlen( szStateToolSign ) ) )
		{
			m_bstrStateToolBar = szLine+strlen( szStateToolSign );
			m_StateAnswer = ToolBar;
			continue;
		}
		else if( !strncmp( szLine, szStateFunctionSign, strlen( szStateFunctionSign ) ) )
		{
			m_bstrFunction = szLine+strlen( szStateFunctionSign );
			m_StateAnswer = Function;
		}
	}

}

DWORD CScriptActionInfo::GetActionState()
{
	if( m_StateAnswer == Action )
	{
		DWORD	dwState = 0;
		m_ptrAM->GetActionFlags( m_bstrStateAction, &dwState );
		return dwState;
	}
	else if( m_StateAnswer == ToolBar )
	{
		IUnknown	*punkMainWnd = _GetOtherComponent( GetAppUnknown(), IID_IMainWindow );
		if( !punkMainWnd )return 0;
		IMainWindowPtr	ptrMainWindow = punkMainWnd;
		punkMainWnd->Release();

		long	lpos = 0;
		ptrMainWindow->GetFirstToolbarPosition( &lpos );
		DWORD	dwState = 0;

		while( lpos )
		{
			HWND	hwnd;
			ptrMainWindow->GetNextToolbar( &hwnd, &lpos );

			char	sz[100];
			::GetWindowText( hwnd, sz, 100 );

			if( !stricmp( sz, m_bstrStateToolBar ) )
			{
				dwState |= afEnabled;
				if( ::IsWindowVisible( hwnd ) )
					dwState |= afChecked;
			}
		}
		return dwState;
	}
	else if( m_StateAnswer == DockBar )
	{
		IUnknown	*punkMainWnd = _GetOtherComponent( GetAppUnknown(), IID_IMainWindow );
		if( !punkMainWnd )return 0;
		IMainWindowPtr	ptrMainWindow = punkMainWnd;
		punkMainWnd->Release();

		long	lpos = 0;
		ptrMainWindow->GetFirstDockWndPosition( &lpos );
		DWORD	dwState = 0;

		while( lpos )
		{
			IUnknown	*punkDock = 0;
			ptrMainWindow->GetNextDockWnd( &punkDock, &lpos );

			if( !punkDock )
				continue;

			IDockableWindowPtr	ptrDock( punkDock );
			CString	str = GetObjectName( punkDock );
			punkDock->Release( );

			if( str == (char*)m_bstrStateDockBar )
			{
				HWND	hwnd = 0;
				IWindowPtr	ptrWindow( ptrDock );
				ptrWindow->GetHandle( (HANDLE*)&hwnd );

				dwState |= afEnabled;
				if( ::IsWindowVisible( hwnd ) )
					dwState |= afChecked;
				break;
			}
		}
		return dwState;
	}
	else if( m_StateAnswer == Function && m_bstrFunction.length() )
	{
		IScriptSitePtr ptr_ss( GetAppUnknown() );	
		if( ptr_ss )
		{
			_variant_t var;
			HRESULT hr = ptr_ss->Invoke( m_bstrFunction, 0, 0, &var, fwAppScript );
			if( hr == S_OK && var.vt == VT_I4 )
				return var.lVal;
		}
	}
	return afEnabled;
}


/////////////////////////////////////////////////////////////////////////////////////////
///ActionInfo
BEGIN_INTERFACE_MAP(CScriptAction, CCmdTargetEx)
	INTERFACE_PART(CScriptAction, IID_IAction, Action)
	INTERFACE_PART(CScriptAction, IID_IRootedObject, Rooted)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CScriptAction, Action);

CScriptAction::CScriptAction( CScriptDataObject	*p )
{
	m_pinfo = &p->m_info;
	BSTR	bstr = 0;
	sptrIScriptDataObject sptr( p->GetControllingUnknown() );

	sptr->GetText( &bstr );

	if( bstr )
	{
		m_bstrScriptText = bstr;
	    ::SysFreeString( bstr );
	}

	m_str_script_name = ::GetObjectName( sptr );
}
//interface implementation
HRESULT CScriptAction::XAction::AttachTarget( IUnknown *punkTarget )
{
	_try_nested(CScriptAction, Action, AttachTarget)
	{
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptAction::XAction::SetArgument( BSTR bstrArgName, VARIANT *pvarVal )
{
	_try_nested(CScriptAction, Action, AttachTarget)
	{
		return E_NOTIMPL;
	}
	_catch_nested;
}

HRESULT CScriptAction::XAction::SetResult( BSTR bstrArgName, VARIANT *pvarVal )
{
	_try_nested(CScriptAction, Action, AttachTarget)
	{
		return E_NOTIMPL;
	}
	_catch_nested;
}

HRESULT CScriptAction::XAction::Invoke()
{
	_try_nested(CScriptAction, Action, AttachTarget)
	{
		IUnknown	*punk = pThis->m_punkParent;
		::ExecuteScript( pThis->m_bstrScriptText, pThis->m_str_script_name, false, GetObjectKey( punk ) );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptAction::XAction::GetActionState( DWORD *pdwState )
{
	METHOD_PROLOGUE_EX(CScriptAction, Action)

	ASSERT( pThis->m_pinfo );
	*pdwState = pThis->m_pinfo->GetActionState();

	return S_OK;
}

HRESULT CScriptAction::XAction::StoreParameters( BSTR *pbstr )
{
	_try_nested(CScriptAction, Action, StoreParameters)
	{
		return E_NOTIMPL;
	}
	_catch_nested;
}

HRESULT CScriptAction::XAction::GetActionInfo(IUnknown** ppunkAI)
{
	_try_nested(CScriptAction, Action, GetActionInfo)
	{
		*ppunkAI = pThis->m_punkParent;
		if( *ppunkAI )(*ppunkAI)->AddRef();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CScriptAction::XAction::GetTarget( IUnknown **ppunkTarget )
{
	*ppunkTarget = 0;
	return S_OK;
}

