#include "StdAfx.h"
#include "resource.h"
#include "docs.h"
#include "idiodbasedataobject.h"

IMPLEMENT_DYNCREATE(CIdioDBDocument, CCmdTargetEx)
IMPLEMENT_DYNCREATE(CIdioDBTemplate, CCmdTargetEx)

// {8FE4CF6B-6351-4f59-A4FC-4AC3D8C6130D}
GUARD_IMPLEMENT_OLECREATE(CIdioDBDocument, "IdioDBase.IdioDBDocument", 
0x8fe4cf6b, 0x6351, 0x4f59, 0xa4, 0xfc, 0x4a, 0xc3, 0xd8, 0xc6, 0x13, 0xd);

// {BCCFBDE1-7723-4432-8A1B-3F25E77AFACE}
GUARD_IMPLEMENT_OLECREATE(CIdioDBTemplate, "IdioDBase.IdioDBDocTemplate",
0xbccfbde1, 0x7723, 0x4432, 0x8a, 0x1b, 0x3f, 0x25, 0xe7, 0x7a, 0xfa, 0xce);

BEGIN_INTERFACE_MAP(CIdioDBDocument, CDocBase)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////////////////
//CImageDocTemplate
CIdioDBTemplate::CIdioDBTemplate()
{
	EnableAggregation();
}

CString CIdioDBTemplate::GetDocTemplString()
{
	CString	str;
	str.LoadString( IDS_TEMPLATE );
	return str;
}
/////////////////////////////////////////////////////////////////////////////////////////
//CImageDocument
CIdioDBDocument::CIdioDBDocument()
{
	EnableAutomation();
	EnableAggregation();
}

BEGIN_MESSAGE_MAP(CIdioDBDocument, CCmdTargetEx)
	//{{AFX_MSG_MAP(CIdioDBDocument)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CIdioDBDocument, CCmdTargetEx)
	//{{AFX_DISPATCH_MAP(CIdioDBDocument)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

void CIdioDBDocument::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	__super::OnNotify( pszEvent, punkHit, punkFrom, pdata, cbSize );
}

//create the required dataobjects here
bool CIdioDBDocument::OnNewDocument()
{
	return true;
}

void CIdioDBDocument::OnFinalRelease()
{
	__super::OnFinalRelease();
}