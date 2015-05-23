#include "stdafx.h"
#include "resource.h"
#include "doc.h"


IMPLEMENT_DYNCREATE(CImageDocument, CCmdTargetEx)
IMPLEMENT_DYNCREATE(CImageDocTemplate, CCmdTargetEx)

// {88765C72-4355-11d3-A60F-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CImageDocument, "ImageDoc.ImageDocument", 
0x88765c72, 0x4355, 0x11d3, 0xa6, 0xf, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {88765C74-4355-11d3-A60F-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CImageDocTemplate, "ImageDoc.ImageDocTemplate",
0x88765c74, 0x4355, 0x11d3, 0xa6, 0xf, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);

BEGIN_INTERFACE_MAP(CImageDocument, CDocBase)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////////////////
//CImageDocTemplate
CImageDocTemplate::CImageDocTemplate()
{
	EnableAggregation();
}

CString CImageDocTemplate::GetDocTemplString()
{
	CString	str;
	str.LoadString( IDS_TEMPLATE );
	return str;
}

/////////////////////////////////////////////////////////////////////////////////////////
//CImageDocument
CImageDocument::CImageDocument()
{
	EnableAggregation();
}

bool CImageDocument::OnNewDocument()
{
	/*IUnknown *punk = 0;
	IUnknown *punk1 = 0;

	{
		CImageWrp	wrp( ::CreateTypedObject(_T("Image")), false );
		wrp.CreateNew( 100, 100, _T("RGB") );
		CImageWrp	wrpChild( wrp.CreateVImage( CRect( 0, 0, 100, 100 ) ), false );

		punk = wrp;
		punk1 = wrpChild;

		::SetValue( GetControllingUnknown(), 0, 0, _variant_t( (IUnknown*)wrp ) );
		::DeleteEntry( GetControllingUnknown(), ::GetObjectPath( GetControllingUnknown(), wrp ) );
	}*/

//	::SetValue( GetControllingUnknown(), 0, 0, _variant_t( (IUnknown*)wrpChild ) );

	return true;
}