// ScriptDocument.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptDoc.h"
#include "ScriptDocument.h"
#include "scriptview.h"
#include "scriptdataobject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScriptDocument

bool	g_bActionRegistrationEnabled = false;

IMPLEMENT_DYNCREATE(CScriptDocument, CCmdTargetEx)

// {52E66402-1EFF-11d3-A5D3-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CScriptDocument, "ScriptDoc.ScriptDocument", 
0x52e66402, 0x1eff, 0x11d3, 0xa5, 0xd3, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);

CScriptDocument::CScriptDocument()
{
	EnableAutomation();
	EnableAggregation();
}

CScriptDocument::~CScriptDocument()
{
}


void CScriptDocument::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CDocBase::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CScriptDocument, CCmdTargetEx)
	//{{AFX_MSG_MAP(CScriptDocument)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CScriptDocument, CCmdTargetEx)
	//{{AFX_DISPATCH_MAP(CScriptDocument)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IScriptDocument to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {A66BA363-0F48-11D3-A5BD-0000B493FF1B}
static const IID IID_IScriptDocument =
{ 0xa66ba363, 0xf48, 0x11d3, { 0xa5, 0xbd, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };

BEGIN_INTERFACE_MAP(CScriptDocument, CDocBase)
	INTERFACE_PART(CScriptDocument, IID_IScriptDocument, Dispatch)
/*	INTERFACE_PART(CScriptDocument, IID_IDocument, Doc)
	INTERFACE_PART(CScriptDocument, IID_INumeredObject, Num)
	INTERFACE_PART(CScriptDocument, IID_IEventListener, EvList)
	INTERFACE_PART(CScriptDocument, IID_IRootedObject, Rooted)*/
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CScriptDocument message handlers


void CScriptDocument::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	CDocBase::OnNotify( pszEvent, punkHit, punkFrom, pdata, cbSize );

}


//create the required dataobjects here
bool CScriptDocument::OnNewDocument()
{
//create an object
	//paul
	IUnknown* punkObj = CreateTypedObject( CScriptDataObject::m_szType );
	if( punkObj )
	{
		IUnknownPtr	ptrObj = punkObj;
		punkObj->Release();	punkObj = 0;

		::SetObjectName( ptrObj, "AutoExec" );
		::SetValue( GetControllingUnknown(), 0, 0, _variant_t( (IUnknown*)ptrObj ) );
	}

	punkObj = CreateTypedObject( CScriptDataObject::m_szType );
	if( punkObj )
	{
		IUnknownPtr	ptrObj = punkObj;
		punkObj->Release();	punkObj = 0;
 
		::SetObjectName( ptrObj, "TerminateExec" );
		::SetValue( GetControllingUnknown(), 0, 0, _variant_t( (IUnknown*)ptrObj ) );
	}

	/*
	IUnknownPtr	ptrObj = CreateTypedObject( CScriptDataObject::m_szType );
	::SetObjectName( ptrObj, "AutoExec" );
	::SetValue( GetControllingUnknown(), 0, 0, _variant_t( (IUnknown*)ptrObj ) );
	ptrObj = CreateTypedObject( CScriptDataObject::m_szType );
	::SetObjectName( ptrObj, "TerminateExec" );
	::SetValue( GetControllingUnknown(), 0, 0, _variant_t( (IUnknown*)ptrObj ) );
	*/

	return true;
}