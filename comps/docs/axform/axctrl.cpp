#include "stdafx.h"
#include "axctrl.h"
#include "resource.h"


IMPLEMENT_DYNCREATE(CActiveXCtrl, CDataObjectBase);
IMPLEMENT_DYNCREATE(CActiveXFrom, CDataObjectBase);






// {590E599B-974D-4c25-AE64-2640B9F3367F}
GUARD_IMPLEMENT_OLECREATE(CActiveXCtrl, "AxForm.AxCtrl", 
0x590e599b, 0x974d, 0x4c25, 0xae, 0x64, 0x26, 0x40, 0xb9, 0xf3, 0x36, 0x7f);
// {A9268DBC-5681-4574-AF6B-FCAE99B1E787}
GUARD_IMPLEMENT_OLECREATE(CActiveXFrom, "AxForm.ActiveXFrom", 
0xa9268dbc, 0x5681, 0x4574, 0xaf, 0x6b, 0xfc, 0xae, 0x99, 0xb1, 0xe7, 0x87);


static const GUID clsidCtrlDataObjectInfo = 
{ 0xd7b567f5, 0x34b9, 0x47f1, { 0x91, 0xf7, 0x67, 0x8c, 0x5c, 0x1e, 0x3d, 0x8a } };
static const GUID clsidFormDataObjectInfo = 
{ 0x128dc377, 0x5aad, 0x4f91, { 0x95, 0x1e, 0x50, 0xee, 0xd1, 0xc2, 0xaf, 0x10 } };


DATA_OBJECT_INFO_FULL(IDS_CTRL_TYPE, CActiveXCtrl, szTypeAXCtrl, szTypeAXForm, clsidCtrlDataObjectInfo, IDI_THUMBNAIL_ICON_AXFORM)
DATA_OBJECT_INFO(IDS_FORM_TYPE, CActiveXFrom, szTypeAXForm, clsidFormDataObjectInfo, IDI_THUMBNAIL_ICON_AXFORM )






CActiveXCtrl::CActiveXCtrl()
{

}

CActiveXCtrl::~CActiveXCtrl()
{

}



CActiveXFrom::CActiveXFrom()
{
	m_strTitle.LoadString( IDS_UNTITLED );
}

CActiveXFrom::~CActiveXFrom()
{
	
}


/*



////////////////////////////////////////////////////////////////////////////////////////////////////
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
*/
/*
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//	class CBlankFrom
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CBlankFrom, CActiveXFrom);


// {A33C1987-C32E-4ff5-8F95-DDB0896A2295}
GUARD_IMPLEMENT_OLECREATE(CBlankFrom, "Blank.BlankForm", 
0xa33c1987, 0xc32e, 0x4ff5, 0x8f, 0x95, 0xdd, 0xb0, 0x89, 0x6a, 0x22, 0x95);
// {07342532-487A-4698-AC6B-C15BA0504D6C}
static const GUID clsidBlankFormDataObjectInfo = 
{ 0x7342532, 0x487a, 0x4698, { 0xac, 0x6b, 0xc1, 0x5b, 0xa0, 0x50, 0x4d, 0x6c } };

DATA_OBJECT_INFO(CBlankFrom, szTypeBlankForm, clsidBlankFormDataObjectInfo,IDI_THUMBNAIL_ICON_BLANK)

BEGIN_INTERFACE_MAP(CBlankFrom, CActiveXFrom)
	INTERFACE_PART(CBlankFrom, I
	_IBlankForm, BlankForm)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CBlankFrom, BlankForm)


////////////////////////////////////////////////////////////////////////////////////////////////////
CBlankFrom::CBlankFrom()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
CBlankFrom::~CBlankFrom()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool CBlankFrom::SerializeObject(CArchive &ar)
{
	CActiveXFrom::SerializeObject( ar );

	long	lVersion = 1;

	if( ar.IsStoring() )
	{
		ar<<lVersion;		
	}
	else
	{
		ar>>lVersion;
	}

	return true;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool CBlankFrom::CanBeBaseObject()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CBlankFrom::XBlankForm::Stub( )
{
	_try_nested( CBlankFrom, BlankForm, Stub )
	{				
		return S_OK;
	}
	_catch_nested;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//	class CBlankCtrl
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CBlankCtrl, CActiveXCtrl);


// {BF960F7D-A5C3-49a8-A830-3E948DE2D06F}
GUARD_IMPLEMENT_OLECREATE(CBlankCtrl, "Blank.BlankCtrl", 
0xbf960f7d, 0xa5c3, 0x49a8, 0xa8, 0x30, 0x3e, 0x94, 0x8d, 0xe2, 0xd0, 0x6f);
// {BABCE2E0-31A8-481a-9A1B-B982A85AB364}
static const GUID clsidBlankCtrlDataObjectInfo = 
{ 0xbabce2e0, 0x31a8, 0x481a, { 0x9a, 0x1b, 0xb9, 0x82, 0xa8, 0x5a, 0xb3, 0x64 } };

DATA_OBJECT_INFO_FULL(CBlankCtrl, szTypeBlankCtrl, szTypeBlankForm, clsidBlankCtrlDataObjectInfo, IDI_THUMBNAIL_ICON_BLANK)

////////////////////////////////////////////////////////////////////////////////////////////////////
CBlankCtrl::CBlankCtrl()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
CBlankCtrl::~CBlankCtrl()
{

}

bool CBlankCtrl::SerializeObject(CArchive &ar)
{
	CActiveXCtrl::SerializeObject( ar );

	long	lVersion = 3;

	if( ar.IsStoring() )
	{
		ar<<lVersion;
	}
	else
	{
		ar>>lVersion;
	}

	

	return true;
}
*/