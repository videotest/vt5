#include "stdafx.h"
#include "resource.h"
#include "PreviewDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// {34432AF3-DD99-4EB6-8042-9F8C8AC422A1}
GUARD_IMPLEMENT_OLECREATE(CPreviewDocument, "PreviewDoc.PreviewDocument",
0x34432af3, 0xdd99, 0x4eb6, 0x80, 0x42, 0x9f, 0x8c, 0x8a, 0xc4, 0x22, 0xa1);
// {B70DD6B0-54BA-493B-A6F0-464F8EEA20BF}
GUARD_IMPLEMENT_OLECREATE(CPreviewDocTemplate, "PreviewDoc.PreviewDocTemplate",
0xb70dd6b0, 0x54ba, 0x493b, 0xa6, 0xf0, 0x46, 0x4f, 0x8e, 0xea, 0x20, 0xbf);


IMPLEMENT_DYNCREATE(CPreviewDocument, CCmdTargetEx)
IMPLEMENT_DYNCREATE(CPreviewDocTemplate, CCmdTargetEx)

BEGIN_INTERFACE_MAP(CPreviewDocument, CDocBase)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////////////////
//CImageDocTemplate
CPreviewDocTemplate::CPreviewDocTemplate()
{
	EnableAggregation();
}

CString CPreviewDocTemplate::GetDocTemplString()
{
	CString	str;
	str.LoadString( IDS_TEMPLATE );
	return str;
}

/////////////////////////////////////////////////////////////////////////////////////////
//CImageDocument
CPreviewDocument::CPreviewDocument()
{
	EnableAggregation();
}

bool CPreviewDocument::OnNewDocument()
{
	return true;
}