#include "stdafx.h"
#include "common.h"
#include "VideoNameObject.h" 
#include "VT5AVI.h"
#include "resource.h"
//#include "DataBase.h"

const char *CVideoNameObject::c_szType = szVideoObject;

IMPLEMENT_DYNCREATE(CVideoNameObject, CCmdTargetEx);

BEGIN_INTERFACE_MAP(CVideoNameObject, CDataObjectBase)
END_INTERFACE_MAP()

// {2FE03080-5CBC-4D29-9B29-D4DE9CB55A6A}
static const GUID clsidVideoNameObject = 
{ 0x2fe03080, 0x5cbc, 0x4d29, { 0x9b, 0x29, 0xd4, 0xde, 0x9c, 0xb5, 0x5a, 0x6a } };

// {3E69F036-7D91-4B10-A354-4E1D85263653}
GUARD_IMPLEMENT_OLECREATE(CVideoNameObject, "VideoDoc.VideoObject",
0x3e69f036, 0x7d91, 0x4b10, 0xa3, 0x54, 0x4e, 0x1d, 0x85, 0x26, 0x36, 0x53);


DATA_OBJECT_INFO(IDS_VIDEO_TYPE, CVideoNameObject, CVideoNameObject::c_szType, clsidVideoNameObject, IDI_VIDEO );

/////////////////////////////////////////////////////////////////////////////////////////
CVideoNameObject::CVideoNameObject()
{
}

CVideoNameObject::~CVideoNameObject()
{
	TRACE("~CVideoNameObject\n");
}

bool CVideoNameObject::GetTextParams(CString &strParams)
{
	INamedObject2Ptr sptrNO(GetControllingUnknown());
	BSTR bstr;
	sptrNO->GetUserName(&bstr);
	strParams = CString(bstr);
	::SysFreeString(bstr);
	return true;
}

bool CVideoNameObject::SetTextParams(const char *pszParams)
{
	INamedObject2Ptr sptrNO(GetControllingUnknown());
	_bstr_t bstr(pszParams);
	sptrNO->SetUserName(bstr);
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
//overrided virtuals


