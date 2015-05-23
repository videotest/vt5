#include "stdafx.h"
#include "Actions.h"
#include "Resource.h"
#include "CreateParamDlg.h"
#include "GridView.h"

#include "Utils.h"
#include "NameConsts.h"
#include "ClassBase.h"
#include "Measure5.h"

#include "defs.h"

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionSetParamName,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionGridSetParamWidth,	CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionGridMoveParam,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAlignByDefault,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAlignByCell,			CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionRemoveAllMMObjects,	CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionZoomOutTable,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionZoomInTable,			CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowMeasView,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowClassView,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionCreateParam,			CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionRemoveParam,			CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAlignByImage,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionCopySelected,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionCopyAll,				CCmdTargetEx);
/*
IMPLEMENT_DYNCREATE(CActionSetParamValue,		CCmdTargetEx);
*/
IMPLEMENT_DYNCREATE(CActionGridSetObjectHeight,	CCmdTargetEx);	// vanek

//olecreate

//[ag]3. olecreate debug

// {0AB889B3-DEE5-4c65-B4AA-0D232F60A9E3}
GUARD_IMPLEMENT_OLECREATE(CActionSetParamName, "DataView.SetParamName",
0xab889b3, 0xdee5, 0x4c65, 0xb4, 0xaa, 0xd, 0x23, 0x2f, 0x60, 0xa9, 0xe3);
// {8C53353D-E438-4d73-B8CE-5318EA12C55F}
GUARD_IMPLEMENT_OLECREATE(CActionGridSetParamWidth, "DataView.GridSetParamWidth",
0x8c53353d, 0xe438, 0x4d73, 0xb8, 0xce, 0x53, 0x18, 0xea, 0x12, 0xc5, 0x5f);
// {3482F7D6-BD74-465b-9157-4132E2AC4744}
GUARD_IMPLEMENT_OLECREATE(CActionGridMoveParam, "DataView.GridMoveParam",
0x3482f7d6, 0xbd74, 0x465b, 0x91, 0x57, 0x41, 0x32, 0xe2, 0xac, 0x47, 0x44);
// {AC290DD5-4903-4565-8B25-167A40087397}
GUARD_IMPLEMENT_OLECREATE(CActionAlignByDefault, "DataView.AlignByDefault",
0xac290dd5, 0x4903, 0x4565, 0x8b, 0x25, 0x16, 0x7a, 0x40, 0x8, 0x73, 0x97);
// {949A8F19-7F66-47df-89A6-42CDAAEB3EA1}
GUARD_IMPLEMENT_OLECREATE(CActionAlignByCell, "DataView.AlignByCell",
0x949a8f19, 0x7f66, 0x47df, 0x89, 0xa6, 0x42, 0xcd, 0xaa, 0xeb, 0x3e, 0xa1);
// {5B34AA05-BFAE-4a6d-BAE4-B1C6B8D00E47}
GUARD_IMPLEMENT_OLECREATE(CActionRemoveAllMMObjects, "DataView.RemoveAllMMObjects",
0x5b34aa05, 0xbfae, 0x4a6d, 0xba, 0xe4, 0xb1, 0xc6, 0xb8, 0xd0, 0xe, 0x47);
// {0854E58B-0744-42a5-91BE-068DD249AF21}
GUARD_IMPLEMENT_OLECREATE(CActionZoomOutTable, "DataView.ZoomOutTable",
0x854e58b, 0x744, 0x42a5, 0x91, 0xbe, 0x6, 0x8d, 0xd2, 0x49, 0xaf, 0x21);
// {863B5938-150D-4b50-ACB8-F946E9A94250}
GUARD_IMPLEMENT_OLECREATE(CActionZoomInTable, "DataView.ZoomInTable",
0x863b5938, 0x150d, 0x4b50, 0xac, 0xb8, 0xf9, 0x46, 0xe9, 0xa9, 0x42, 0x50);
// {0E75F56D-7D5C-4bf2-A061-9364030CD9E3}
GUARD_IMPLEMENT_OLECREATE(CActionShowMeasView, "DataView.ShowMeasView",
0xe75f56d, 0x7d5c, 0x4bf2, 0xa0, 0x61, 0x93, 0x64, 0x3, 0xc, 0xd9, 0xe3);
// {EDAF6F23-DFD2-11d3-9D41-009027982EB4}
GUARD_IMPLEMENT_OLECREATE(CActionShowClassView, "DataView.ShowClassView",
0xedaf6f23, 0xdfd2, 0x11d3, 0x9d, 0x41, 0x0, 0x90, 0x27, 0x98, 0x2e, 0xb4);
// {9ED3F471-03B1-4289-A89B-B911634FD09F}
GUARD_IMPLEMENT_OLECREATE(CActionCreateParam, "DataView.CreateParam",
0x9ed3f471, 0x3b1, 0x4289, 0xa8, 0x9b, 0xb9, 0x11, 0x63, 0x4f, 0xd0, 0x9f);

// {D31DB46D-A84E-465b-B64D-6E62F49DFC90}
GUARD_IMPLEMENT_OLECREATE(CActionRemoveParam, "DataView.RemoveParam",
0xd31db46d, 0xa84e, 0x465b, 0xb6, 0x4d, 0x6e, 0x62, 0xf4, 0x9d, 0xfc, 0x90);

// {3014BAB4-9AD5-452d-9503-B26F8CDB94F7}
GUARD_IMPLEMENT_OLECREATE(CActionAlignByImage, "DataView.AlignByImage", 
0x3014bab4, 0x9ad5, 0x452d, 0x95, 0x3, 0xb2, 0x6f, 0x8c, 0xdb, 0x94, 0xf7);

// {9232F6D2-D477-4d2b-B6F7-B0359C5E96AB}
GUARD_IMPLEMENT_OLECREATE(CActionCopySelected, "DataView.CopySelected", 
0x9232f6d2, 0xd477, 0x4d2b, 0xb6, 0xf7, 0xb0, 0x35, 0x9c, 0x5e, 0x96, 0xab);

// {181048FF-19F5-436c-9F8D-8D4CB4609859}
GUARD_IMPLEMENT_OLECREATE(CActionCopyAll, "DataView.CopyAll", 
0x181048ff, 0x19f5, 0x436c, 0x9f, 0x8d, 0x8d, 0x4c, 0xb4, 0x60, 0x98, 0x59);

/*
// {C32681E1-D50D-4469-90CE-228C4A51745A}
GUARD_IMPLEMENT_OLECREATE(CActionSetParamValue, "DataView.SetParamValue", 
0xc32681e1, 0xd50d, 0x4469, 0x90, 0xce, 0x22, 0x8c, 0x4a, 0x51, 0x74, 0x5a);
*/

// {003E8EBF-D177-407a-A5EB-3800337372DB} // vanek
GUARD_IMPLEMENT_OLECREATE(CActionGridSetObjectHeight, "DataView.GridSetObjectHeight", 
0x3e8ebf, 0xd177, 0x407a, 0xa5, 0xeb, 0x38, 0x0, 0x33, 0x73, 0x72, 0xdb);



//guidinfo 

//[ag]3. guidinfo debug

// {6CAE21A0-A5E5-4dac-987B-B718782D525F}
static const GUID guidSetParamName =
{ 0x6cae21a0, 0xa5e5, 0x4dac, { 0x98, 0x7b, 0xb7, 0x18, 0x78, 0x2d, 0x52, 0x5f } };
// {54979F3A-7002-405f-8A38-43E3DB008658}
static const GUID guidGridSetParamWidth =
{ 0x54979f3a, 0x7002, 0x405f, { 0x8a, 0x38, 0x43, 0xe3, 0xdb, 0x0, 0x86, 0x58 } };
// {B0341D34-EF56-418e-98D3-2ECA5A86DE1C}
static const GUID guidGridMoveParam =
{ 0xb0341d34, 0xef56, 0x418e, { 0x98, 0xd3, 0x2e, 0xca, 0x5a, 0x86, 0xde, 0x1c } };
// {B82A531E-5C42-42fe-9DB9-489EA95FB064}
static const GUID guidAlignByDefault =
{ 0xb82a531e, 0x5c42, 0x42fe, { 0x9d, 0xb9, 0x48, 0x9e, 0xa9, 0x5f, 0xb0, 0x64 } };
// {0437F09E-FD39-4c6c-BEB3-A363571091B5}
static const GUID guidAlignByCell =
{ 0x437f09e, 0xfd39, 0x4c6c, { 0xbe, 0xb3, 0xa3, 0x63, 0x57, 0x10, 0x91, 0xb5 } };
// {6C3B4392-4050-4ccd-A2A9-636A709ABAFB}
static const GUID guidRemoveAllMMObjects =
{ 0x6c3b4392, 0x4050, 0x4ccd, { 0xa2, 0xa9, 0x63, 0x6a, 0x70, 0x9a, 0xba, 0xfb } };
// {D0F5D785-704A-45a8-88AF-0577B1A8BB97}
static const GUID guidRemoveManualMeasObjects =
{ 0xd0f5d785, 0x704a, 0x45a8, { 0x88, 0xaf, 0x5, 0x77, 0xb1, 0xa8, 0xbb, 0x97 } };
// {D477648A-270B-4e59-9539-B05D7F6B3C48}
static const GUID guidZoomOutTable =
{ 0xd477648a, 0x270b, 0x4e59, { 0x95, 0x39, 0xb0, 0x5d, 0x7f, 0x6b, 0x3c, 0x48 } };
// {658331CC-9B8C-4fb6-8A65-A3292047CF0E}
static const GUID guidZoomInTable =
{ 0x658331cc, 0x9b8c, 0x4fb6, { 0x8a, 0x65, 0xa3, 0x29, 0x20, 0x47, 0xcf, 0xe } };
// {08F63FC1-7945-4f1d-87F1-85778F9BCA94}
static const GUID guidRemoveParam =
{ 0x8f63fc1, 0x7945, 0x4f1d, { 0x87, 0xf1, 0x85, 0x77, 0x8f, 0x9b, 0xca, 0x94 } };
// {F1DF2604-2A72-42af-B14D-95232758108E}
static const GUID guidCreateParam =
{ 0xf1df2604, 0x2a72, 0x42af, { 0xb1, 0x4d, 0x95, 0x23, 0x27, 0x58, 0x10, 0x8e } };
// {2E97906C-27BF-4a46-BD42-BD5CA419C956}
static const GUID guidShowMeasView =
{ 0x2e97906c, 0x27bf, 0x4a46, { 0xbd, 0x42, 0xbd, 0x5c, 0xa4, 0x19, 0xc9, 0x56 } };
// {EDAF6F21-DFD2-11d3-9D41-009027982EB4}
static const GUID guidShowClassView =
{ 0xedaf6f21, 0xdfd2, 0x11d3, { 0x9d, 0x41, 0x0, 0x90, 0x27, 0x98, 0x2e, 0xb4 } };
// {6FD131D5-881D-4bf3-91C6-AFDAD20D2BBB}
static const GUID guidSetParamValue = 
{ 0x6fd131d5, 0x881d, 0x4bf3, { 0x91, 0xc6, 0xaf, 0xda, 0xd2, 0xd, 0x2b, 0xbb } };
// {74F72FE3-E18B-4ac2-8819-4673FDCB32B1}
static const GUID guidAlignByImage = 
{ 0x74f72fe3, 0xe18b, 0x4ac2, { 0x88, 0x19, 0x46, 0x73, 0xfd, 0xcb, 0x32, 0xb1 } };

// {E349621B-44D1-414d-9C4A-E53162C1F426}
static const GUID guidCopySelected = 
{ 0xe349621b, 0x44d1, 0x414d, { 0x9c, 0x4a, 0xe5, 0x31, 0x62, 0xc1, 0xf4, 0x26 } };
// {D605354F-9C08-40bd-88C5-A64D3A0B789F}
static const GUID guidCopyAll = 
{ 0xd605354f, 0x9c08, 0x40bd, { 0x88, 0xc5, 0xa6, 0x4d, 0x3a, 0xb, 0x78, 0x9f } };

// vanek
// {4BF305B5-4FF8-4261-BB9F-DF880A7716EC}
static const GUID guidGridSetObjectHeight =											
{ 0x4bf305b5, 0x4ff8, 0x4261, { 0xbb, 0x9f, 0xdf, 0x88, 0xa, 0x77, 0x16, 0xec } };


//[ag]6. action info

ACTION_INFO_FULL(CActionSetParamName, IDS_ACTION_RENAME_PARAM, -1, -1, guidSetParamName);
ACTION_INFO_FULL(CActionGridSetParamWidth, IDS_ACTION_SET_PARAM_WIDTH, -1, -1, guidGridSetParamWidth);
ACTION_INFO_FULL(CActionGridMoveParam, IDS_ACTION_MOVE_PARAM, -1, -1, guidGridMoveParam);
ACTION_INFO_FULL(CActionAlignByDefault, IDS_ACTION_ALIGN_BY_DEFAULT, IDS_MENU_VIEW, -1, guidAlignByDefault);
ACTION_INFO_FULL(CActionAlignByCell, IDS_ACTION_ALIGN_BY_CELL, IDS_MENU_VIEW, -1, guidAlignByCell);
ACTION_INFO_FULL(CActionRemoveAllMMObjects, IDS_ACTION_REMOVE_MANUAL_MEAS, IDS_MENU_VIEW, IDS_TB_DATAVIEW, guidRemoveAllMMObjects);
ACTION_INFO_FULL(CActionZoomOutTable, IDS_ACTION_ZOOM_OUT_TABLE, IDS_MENU_VIEW, -1, guidZoomOutTable);
ACTION_INFO_FULL(CActionZoomInTable, IDS_ACTION_ZOOM_IN_TABLE, IDS_MENU_VIEW, -1, guidZoomInTable);
ACTION_INFO_FULL(CActionShowMeasView, IDS_ACTION_SHOW_MEASVIEW, IDS_MENU_VIEW, IDS_TB_DATAVIEW, guidShowMeasView);
ACTION_INFO_FULL(CActionShowClassView, IDS_ACTION_SHOW_CLASSVIEW, IDS_MENU_VIEW, IDS_TB_DATAVIEW, guidShowClassView);
ACTION_INFO_FULL(CActionCreateParam, IDS_ACTION_CREATE_PARAM, -1, -1, guidCreateParam);
ACTION_INFO_FULL(CActionRemoveParam, IDS_ACTION_REMOVE_PARAM, IDS_MENU_VIEW, IDS_TB_DATAVIEW, guidRemoveParam);
ACTION_INFO_FULL(CActionAlignByImage, IDS_ACTION_ALIGN_IMAGE, -1, -1, guidAlignByImage);
ACTION_INFO_FULL(CActionCopySelected, IDS_ACTION_COPY_SELECTED, -1, -1, guidCopySelected);
ACTION_INFO_FULL(CActionCopyAll, IDS_ACTION_COPY_ALL, -1, -1, guidCopyAll);
/*
ACTION_INFO_FULL(CActionSetParamValue, IDS_ACTION_SET_PARAM_VALUE, -1, -1, guidSetParamValue);
*/
ACTION_INFO_FULL(CActionGridSetObjectHeight, IDS_ACTION_SET_OBJECT_HEIGHT, -1, -1, guidGridSetObjectHeight); // vanek


//[ag]7. targets

ACTION_TARGET(CActionSetParamName,		szTargetAnydoc);
ACTION_TARGET(CActionRemoveParam,		szTargetAnydoc);
ACTION_TARGET(CActionGridSetParamWidth, szTargetAnydoc);
ACTION_TARGET(CActionGridMoveParam,		szTargetAnydoc);
ACTION_TARGET(CActionCreateParam,		szTargetAnydoc);
ACTION_TARGET(CActionShowMeasView,		szTargetFrame);
ACTION_TARGET(CActionAlignByDefault,	szTargetViewSite);
ACTION_TARGET(CActionAlignByCell,		szTargetViewSite);
ACTION_TARGET(CActionRemoveAllMMObjects, szTargetViewSite);
ACTION_TARGET(CActionZoomOutTable,		szTargetViewSite);
ACTION_TARGET(CActionZoomInTable,		szTargetViewSite);
ACTION_TARGET(CActionShowClassView,		szTargetFrame);
ACTION_TARGET(CActionAlignByImage,		szTargetViewSite);
ACTION_TARGET(CActionCopySelected,		szTargetViewSite);
ACTION_TARGET(CActionCopyAll,		szTargetViewSite);
/*
ACTION_TARGET(CActionSetParamValue,		szTargetAnydoc);
*/
ACTION_TARGET(CActionGridSetObjectHeight, /*szTargetViewSite*/ szTargetAnydoc );	// vanek

//[ag]8. arguments

ACTION_ARG_LIST(CActionGridSetParamWidth)
	ARG_INT("ParamKey", -1)
	ARG_INT("Width", 100)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionSetParamName)
	ARG_INT("ParamKey", -1)
	ARG_STRING("Name", "NewName")
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionGridMoveParam)
	ARG_INT("ParamKey", -1)
	ARG_INT("ParamKeyAfter", 100)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionAlignByCell)
	ARG_INT("CellRow", -1)
	ARG_INT("CellCol", -1)
END_ACTION_ARG_LIST()


ACTION_ARG_LIST(CActionRemoveAllMMObjects)
	ARG_STRING(_T("ObjList"), 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionCreateParam)
	ARG_STRING("ParamType", 0)
	ARG_STRING("ParamName", 0)
	ARG_INT("Number", 1)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionRemoveParam)
	ARG_INT(_T("ParamKey"), -1)
END_ACTION_ARG_LIST()

/*ACTION_ARG_LIST(CActionSetParamValue)
	ARG_STRING(_T("ObjList"), 0)
	ARG_STRING(_T("Object"), 0)
	ARG_INT(_T("ParamID"), -1)
	ARG_DOUBLE(_T("ParamValue"), 0.)
	ARG_BOOL(_T("NeedValue"), TRUE)
END_ACTION_ARG_LIST()*/

// vanek
ACTION_ARG_LIST(CActionGridSetObjectHeight)
	ARG_INT("ObjectRow", -1)
	ARG_INT("Height", 100)
END_ACTION_ARG_LIST()
//


//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionSetParamName implementation
CActionSetParamName::CActionSetParamName()
{
	m_lParam = -1;
}

CActionSetParamName::~CActionSetParamName()
{
}

bool CActionSetParamName::Invoke()
{
	m_lParam = GetArgumentInt( "ParamKey" );
	m_bstrNewName = GetArgumentString( "Name" );

	if( m_lParam == -1 || m_bstrNewName.length() == 0 )
	{
		AfxMessageBox( IDS_WRONG_PARAMETER );
		return false;
	}

	IUnknown	*punk = ::GetActiveObjectFromDocument( m_punkTarget, szTypeObjectList );
	if( !punk  )
	{
		AfxMessageBox( IDS_NO_CATIVE_DATAOBJECT );
		return false;
	}
	m_ptrC = punk;
	punk->Release();
	if( m_ptrC == 0  )
	{
		AfxMessageBox( IDS_INVALIDOBJECT );
		return false;
	}


	ParameterContainer	*p = 0;
	m_ptrC->ParamDefByKey( m_lParam, &p );

	if( p == 0 )
	{
		AfxMessageBox( IDS_INVALIDKEY );
		return false;
	}
	
	m_bstrOldName = p->pDescr->bstrUserName;
	if( p->pDescr->bstrUserName )
		::SysFreeString( p->pDescr->bstrUserName ); p->pDescr->bstrUserName = 0;
	
    p->pDescr->bstrUserName = m_bstrNewName.copy(true);

	m_ptrC->DefineParameterFull( p, 0 );

	return true;
}

bool CActionSetParamName::DoUndo()
{
	ParameterContainer	*p = 0;
	m_ptrC->ParamDefByKey( m_lParam, &p );
	if( p == 0 )return false;
	
	if( p->pDescr->bstrUserName )
		::SysFreeString( p->pDescr->bstrUserName ); p->pDescr->bstrUserName = 0;
	
	p->pDescr->bstrUserName = m_bstrOldName.copy(true);

	m_ptrC->DefineParameterFull( p, 0 );

	return true;
}

bool CActionSetParamName::DoRedo()
{
	ParameterContainer	*p = 0;
	m_ptrC->ParamDefByKey( m_lParam, &p );
	if( p == 0 )return false;

	if( p->pDescr->bstrUserName )
		::SysFreeString( p->pDescr->bstrUserName ); p->pDescr->bstrUserName = 0;
	
	p->pDescr->bstrUserName = m_bstrNewName.copy(true);

	m_ptrC->DefineParameterFull( p, 0 );

	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionGridSetParamWidth implementation
CActionGridSetParamWidth::CActionGridSetParamWidth()
{
	m_lParam = m_lOldWidth = m_lNewWidth = -1;
}


bool CActionGridSetParamWidth::Invoke()
{
	m_lParam = GetArgumentInt( "ParamKey" );
	m_lNewWidth = GetArgumentInt( "Width" );

	if( m_lParam == -1 || m_lNewWidth == -1 )
	{
		AfxMessageBox( IDS_WRONG_PARAMETER );
		return false;
	}

	IUnknown	*punk = ::GetActiveObjectFromDocument( m_punkTarget, szTypeObjectList );
	if( !punk  )
	{
		AfxMessageBox( IDS_NO_CATIVE_DATAOBJECT );
		return false;
	}
	m_ptrC = punk;
	punk->Release();
	if( m_ptrC == 0  )
	{
		AfxMessageBox( IDS_INVALIDOBJECT );
		return false;
	}


	ParameterContainer	*p = 0;
	m_ptrC->ParamDefByKey( m_lParam, &p );

	if( p == 0 )
	{
		AfxMessageBox( IDS_INVALIDKEY );
		return false;
	}
	
	m_lOldWidth = p->cxWidth;
	p->cxWidth = m_lNewWidth;
	m_ptrC->DefineParameterFull( p, 0 );

	return true;
}

bool CActionGridSetParamWidth::DoUndo()
{
	ParameterContainer	*p = 0;
	m_ptrC->ParamDefByKey( m_lParam, &p );
	if( p == 0 )return false;

	p->cxWidth = m_lOldWidth;
	m_ptrC->DefineParameterFull( p, 0 );

	return true;
}

bool CActionGridSetParamWidth::DoRedo()
{
	ParameterContainer	*p = 0;
	m_ptrC->ParamDefByKey( m_lParam, &p );
	if( p == 0 )return false;

	p->cxWidth = m_lNewWidth;
	m_ptrC->DefineParameterFull( p, 0 );

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionGridSetObjectHeight implementation
CActionGridSetObjectHeight::CActionGridSetObjectHeight( )
{
	m_lOldHeight = 0;
	m_lNewHeight = 0;
}

bool CActionGridSetObjectHeight::Invoke()
{
    long lrow = GetArgumentInt( "ObjectRow" ),
		 lheight = GetArgumentInt( "Height" );


	IDocumentSitePtr ptrDS( m_punkTarget );
	if( ptrDS == 0 )
		return 0;

	IUnknown* punkView = 0;
	ptrDS->GetActiveView( &punkView );
	if( !punkView )
		return 0;

	IGridViewPtr sptrGridView( punkView );            
	punkView->Release( );
	punkView = 0;
	if( sptrGridView == 0 )
		return false;

	IUnknown *punkObject = 0;
	sptrGridView->GetObjectByRow( lrow, &punkObject );
	if( !punkObject )
		return false;
    
	bool bRetVal = false;
	m_sptrPropBag = punkObject; 
	if( bRetVal = m_sptrPropBag != 0 )
	{
     	_variant_t var;
		m_sptrPropBag->GetProperty( bstr_t( ROW_HEIGHT ), &var );
		m_lOldHeight=-1; // по умолчанию - -1
		if( var.vt == VT_I4 )  m_lOldHeight = var.lVal;

		m_lNewHeight = lheight;
		var=_variant_t(long(m_lNewHeight));
		
		m_sptrPropBag->SetProperty( bstr_t( ROW_HEIGHT ), var );

		IUnknown	*punkObjList = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
		if( bRetVal = punkObjList != 0 )
		{
			::FireEventNotify( m_punkTarget, szEventChangeObjectList, punkObjList, punkObject, cncChange );
			punkObjList->Release();
			punkObjList = 0;
		}
	}
	
	if( punkObject )
	{
		punkObject->Release( );
		punkObject = 0;
	}
    
    return bRetVal;
}

bool CActionGridSetObjectHeight::DoUndo()
{
	m_sptrPropBag->SetProperty( bstr_t( "RowHeight" ), _variant_t( m_lOldHeight ) );

	IUnknown	*punkObjList = 0;
	INamedDataObject2Ptr sptrObject = m_sptrPropBag;

	if( sptrObject )
	{
		sptrObject->GetParent( &punkObjList );

		if( punkObjList )
		{
			::FireEventNotify( m_punkTarget, szEventChangeObjectList, punkObjList, m_sptrPropBag, cncChange );
	
			punkObjList->Release();
			punkObjList = 0;
		}
	}

	return true;
}

bool CActionGridSetObjectHeight::DoRedo()
{
	m_sptrPropBag->SetProperty( bstr_t( "RowHeight" ), _variant_t( m_lNewHeight ) );

	IUnknown	*punkObjList = 0;
	INamedDataObject2Ptr sptrObject = m_sptrPropBag;

	if( sptrObject )
	{
		sptrObject->GetParent( &punkObjList );

		if( punkObjList )
		{
			::FireEventNotify( m_punkTarget, szEventChangeObjectList, punkObjList, m_sptrPropBag, cncChange );
	
			punkObjList->Release();
			punkObjList = 0;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionGridMoveParam implementation
CActionGridMoveParam::CActionGridMoveParam()
{
	m_lParam = m_lOldPrevParam = m_lNewPrevParam = -1;
}

bool CActionGridMoveParam::Invoke()
{
	m_lParam = GetArgumentInt( "ParamKey" );
	m_lNewPrevParam = GetArgumentInt( "ParamKeyAfter" );

	if( m_lNewPrevParam == m_lParam )
		return true;

	if( m_lParam == -1 )
	{
		AfxMessageBox( IDS_WRONG_PARAMETER );
		return false;
	}

	IUnknown	*punk = ::GetActiveObjectFromDocument( m_punkTarget, szTypeObjectList );
	if( !punk  )
	{
		AfxMessageBox( IDS_NO_CATIVE_DATAOBJECT );
		return false;
	}
	m_ptrC = punk;
	punk->Release();
	if( m_ptrC == 0  )
	{
		AfxMessageBox( IDS_INVALIDOBJECT );
		return false;
	}


	ParameterContainer	*p = 0;
	m_ptrC->ParamDefByKey( m_lParam, &p );

	if( p == 0 )
	{
		AfxMessageBox( IDS_INVALIDKEY );
		return false;
	}

	//get the prev parameter key
	LONG_PTR	lpos = p->lpos;
	ParameterContainer	*pprev = 0;
	m_ptrC->GetPrevParameter( &lpos, &pprev );
	// Now only lpos prev, pprev is same (pprev == p)
	if (lpos)
		m_ptrC->GetPrevParameter( &lpos, &pprev );
	else
		pprev = NULL;
	m_lOldPrevParam = pprev?pprev->pDescr->lKey:-1;

	long	lposAfter = 0;
	ParameterContainer	*pnew = 0;
	m_ptrC->ParamDefByKey( m_lNewPrevParam, &pnew );
	if( pnew )lposAfter = pnew->lpos;

	m_ptrC->MoveParameterAfter( lposAfter, p );

	return true;
}

bool CActionGridMoveParam::DoUndo()
{
	ParameterContainer	*p = 0;
	m_ptrC->ParamDefByKey( m_lParam, &p );
	if( p == 0 )return false;

	long	lposAfter = 0;
	ParameterContainer	*pnew = 0;
	m_ptrC->ParamDefByKey( m_lOldPrevParam, &pnew );
	if( pnew )lposAfter = pnew->lpos;

	m_ptrC->MoveParameterAfter( lposAfter, p );

	return true;
}

bool CActionGridMoveParam::DoRedo()
{
	ParameterContainer	*p = 0;
	m_ptrC->ParamDefByKey( m_lParam, &p );
	if( p == 0 )return false;

	long	lposAfter = 0;
	ParameterContainer	*pnew = 0;
	m_ptrC->ParamDefByKey( m_lNewPrevParam, &pnew );
	if( pnew )lposAfter = pnew->lpos;

	m_ptrC->MoveParameterAfter( lposAfter, p );

	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionAlignByDefault implementation
CActionAlignByDefault::CActionAlignByDefault()
{
}

CActionAlignByDefault::~CActionAlignByDefault()
{
}

bool CActionAlignByDefault::Invoke()
{
	if (!m_punkTarget)
		return false;

	if (!CheckInterface(m_punkTarget, IID_IGridView))
		return false;

	CWnd * pWnd = ::GetWindowFromUnknown(m_punkTarget);
	if (!pWnd || !pWnd->IsKindOf(RUNTIME_CLASS(CGridViewBase)))
		return false;

	CGridViewBase* pView = (CGridViewBase*)pWnd;
	if (!pView)
		return false;

	return pView->AlignByDefault();
}

//extended UI
bool CActionAlignByDefault::IsAvaible()
{
	if (!CheckInterface(m_punkTarget, IID_IGridView))
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionAlignByCell implementation
CActionAlignByCell::CActionAlignByCell()
{
	nRow = nCol = -1;
	m_pView = 0;
}

CActionAlignByCell::~CActionAlignByCell()
{
}

bool CActionAlignByCell::ExecuteSettings(CWnd *pwndParent)
{
	nRow = (int)GetArgumentInt(_T("CellRow"));
	nCol = (int)GetArgumentInt(_T("CellCol"));
	if (!CheckInterface(m_punkTarget, IID_IGridView))
		return false;

	CWnd * pWnd = ::GetWindowFromUnknown(m_punkTarget);
	if (!pWnd || !pWnd->IsKindOf(RUNTIME_CLASS(CGridViewBase)))
		return false;

	m_pView = (CGridViewBase*)pWnd;

	if (!m_pView)
		return false;

//	if ((nRow == -1 || nCol == -1) && !m_pView->GetRClickedCell(nRow, nCol))
//		return false;

	SetArgument(_T("CellRow"), _variant_t((long)nRow));
	SetArgument(_T("CellCol"), _variant_t((long)nCol));

	return true;
}

bool CActionAlignByCell::Invoke()
{
	if (!m_punkTarget)
		return false;

	if (!m_pView || nRow == -1 || nCol == -1)
		return false;

	return m_pView->AlignByCell(nRow, nCol);
}

//extended UI
bool CActionAlignByCell::IsAvaible()
{
	if (!CheckInterface(m_punkTarget, IID_IGridView))
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionRemoveAllMMObjects implementation
CActionRemoveAllMMObjects::CActionRemoveAllMMObjects()
{
	m_punkDoc = 0;
}

CActionRemoveAllMMObjects::~CActionRemoveAllMMObjects()
{
	m_punkDoc = 0;
}

bool CActionRemoveAllMMObjects::ExecuteSettings(CWnd * pwndParent)
{
	ASSERT (m_punkTarget);
	if (!m_punkTarget)
		return false;

	if (!(CheckInterface(m_punkTarget, IID_IMeasureView) || CheckInterface(m_punkTarget, IID_IImageView)))
		return false;

	sptrIView	sptrV(m_punkTarget);
	if (sptrV == 0)
		return false;

	if (FAILED(sptrV->GetDocument(&m_punkDoc)) || !m_punkDoc)
	{
		SetError(IDS_ERR_NODOCUMENT);
		return false;
	}

	m_lTargetKey = ::GetObjectKey(m_punkDoc);

	//get the active object list
	CString strName = GetArgumentString(_T("ObjList"));
	
	if (!strName.IsEmpty())
		m_list.Attach(::GetObjectByName(m_punkTarget, strName, szTypeObjectList), false);

	if (strName.IsEmpty() || !m_list.CheckState())
		m_list.Attach(::GetActiveObjectFromContext(m_punkTarget, szTypeObjectList), false);
	
	if (!m_list.CheckState())
	{
		m_punkDoc->Release();
		m_punkDoc = 0;
		return false;
	}

	strName = ::GetObjectName(m_list);
	SetArgument(_T("ObjList"), _variant_t(strName));

	m_punkDoc->Release();
	m_punkDoc = 0;

	return true;
}

bool CActionRemoveAllMMObjects::Invoke()
{
	ASSERT (m_punkTarget);
	if (!m_punkTarget || !m_list.CheckState())
		return false;
	
	// if list has no objects
	if (!m_list.GetCount())
		return true;

	// get doc
	sptrIView	sptrV(m_punkTarget);
	if (sptrV == 0)
		return false;

	if (FAILED(sptrV->GetDocument(&m_punkDoc)) || !m_punkDoc)
	{
		SetError(IDS_ERR_NODOCUMENT);
		return false;
	}

	// object_list lock update
	CListLockUpdate lock(m_list);

	bool bRet = true;
	// for all meas objects 
	for (POSITION pos = m_list.GetLastObjectPosition(); pos != NULL; )
	{
		// egt next meas_obj
		IUnknown * punkObj = m_list.GetPrevObject(pos);
		ICalcObjectPtr sptrObj = punkObj;
		INamedDataObject2Ptr sptrN = sptrObj;
		if (punkObj)
			punkObj->Release();

		if (sptrObj == 0 || sptrN == 0)
			continue;

		CObjectWrp	object = sptrObj;
		CImageWrp	image = object.GetImage();

		// flag need remove empty object ? 
		bool bNeedRemoveObject = (image == 0 || image.IsEmpty()) ? true : false;

		// if has no children -> continue
		long lCount = 0;
		if (SUCCEEDED(sptrN->GetChildsCount(&lCount)) && lCount == 0)
		{
			if (bNeedRemoveObject)
				m_changes.RemoveFromDocData(m_punkDoc, sptrObj);
			continue;
		}

		// get first child pos 
		TPOS lPos = 0;
		if (FAILED(sptrN->GetLastChildPosition(&lPos)))
		{
			bRet = false;
			if (bNeedRemoveObject)
				m_changes.RemoveFromDocData(m_punkDoc, sptrObj);
			continue;
		}

		// for all params 
		while (lPos)
		{
			// get next child
			long lParamKey = -1;
			IUnknown * punkChild = 0;
			if (FAILED(sptrN->GetPrevChild(&lPos, &punkChild)) || !punkChild)
			{
				bRet = false;
				break;
			}
			
			IManualMeasureObjectPtr sptrMM = punkChild;
			punkChild->Release();
			if (sptrMM == 0)
				continue;

			// get param key
			if (FAILED(sptrMM->GetParamInfo(&lParamKey, 0)) || lParamKey == -1)
			{
				bRet = false;
				break;
			}

			// remove value from meas_object
			sptrObj->RemoveValue(lParamKey);
			// remove mmobject from document
			m_changes.RemoveFromDocData(m_punkDoc, sptrMM);

		}// for all params in meas_object

		if (bNeedRemoveObject)
			m_changes.RemoveFromDocData(m_punkDoc, sptrObj);

		// to next object 

	} // for meas_objects

	m_punkDoc->Release();
	m_punkDoc = 0;

	return true;
}

//undo/redo
bool CActionRemoveAllMMObjects::DoUndo()
{
	ASSERT (m_punkTarget);
	if (!m_list.CheckState())
		return false;

	// object_list lock update
	CListLockUpdate lock(m_list);

	// restore manual measure objects
	m_changes.DoUndo(m_punkTarget);
	return true;
}

bool CActionRemoveAllMMObjects::DoRedo()
{
	ASSERT (m_punkTarget);
	if (!m_list.CheckState())
		return false;

	// object_list lock update
	CListLockUpdate lock(m_list);

	// restore manual measure objects
	m_changes.DoRedo(m_punkTarget);
	return true;
}

bool CActionRemoveAllMMObjects::IsAvaible()
{
	if (!(CheckInterface(m_punkTarget, IID_IMeasureView) || CheckInterface(m_punkTarget, IID_IImageView)))
		return false;

	m_list.Attach(::GetActiveObjectFromContext(m_punkTarget, szTypeObjectList), false);
	
	if (!m_list.CheckState())
		return false;

	if (!m_list.GetCount())
		return false;

	// A.M. SBT1554. Check for existance of the manual measure objects.
	bool bFoundMM = false;
	POSITION pos = m_list.GetFirstObjectPosition();
	while(pos)
	{
		IUnknown* punkObj = m_list.GetNextObject(pos);
		INamedDataObject2Ptr sptrNDO2(punkObj);
		if(punkObj)
			punkObj->Release();
		IUnknownPtr punkMM(GetActiveMeasureObject(sptrNDO2), false);
		if (punkMM != 0)
		{
			bFoundMM = true;
			break;
		}
	}	
	return bFoundMM;
}

//////////////////////////////////////////////////////////////////////
//CActionZoomOutTable implementation
CActionZoomOutTable::CActionZoomOutTable()
{
	m_nMinFontSize = GetValueInt(GetAppUnknown(), "ZoomOutTable", "MinFontHeight", 2);
}

CActionZoomOutTable::~CActionZoomOutTable()
{
}

bool CActionZoomOutTable::Invoke()
{
	if (!CheckInterface(m_punkTarget, IID_IGridView))
		return false;

	CWnd * pWnd = ::GetWindowFromUnknown(m_punkTarget);
	if (!pWnd || !pWnd->IsKindOf(RUNTIME_CLASS(CGridViewBase)))
		return false;

	HFONT hFont = (HFONT)pWnd->SendMessage(WM_GETFONT, 0, 0);
	if (!hFont) // system font
		hFont = (HFONT)::GetStockObject(SYSTEM_FONT);

	CFont * pFont = CFont::FromHandle(hFont);

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	if (!pFont->GetLogFont(&lf))
		return false;

	lf.lfHeight = (LONG)((double)lf.lfHeight * 0.9);
	lf.lfWidth = 0;

	CFont font;
	if (!font.CreateFontIndirect(&lf))
		return false;

	pWnd->SendMessage(WM_SETFONT, (WPARAM)((HFONT)font), MAKELPARAM(true, 0));
	font.DeleteObject();

	return true;
}

//extended UI
bool CActionZoomOutTable::IsAvaible()
{
	if (!CheckInterface(m_punkTarget, IID_IGridView))
		return false;
	CWnd * pWnd = ::GetWindowFromUnknown(m_punkTarget);
	if (!pWnd || !pWnd->IsKindOf(RUNTIME_CLASS(CGridViewBase)))
		return false;
	HFONT hFont = (HFONT)pWnd->SendMessage(WM_GETFONT, 0, 0);
	if (!hFont) // system font
		hFont = (HFONT)::GetStockObject(SYSTEM_FONT);
	CFont * pFont = CFont::FromHandle(hFont);
	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	if (!pFont->GetLogFont(&lf))
		return false;
	int n = abs(lf.lfHeight);
	return n >=m_nMinFontSize;
}

//////////////////////////////////////////////////////////////////////
//CActionZoomInTable implementation
CActionZoomInTable::CActionZoomInTable()
{
}

CActionZoomInTable::~CActionZoomInTable()
{
}

bool CActionZoomInTable::Invoke()
{
	if (!CheckInterface(m_punkTarget, IID_IGridView))
		return false;

	CWnd * pWnd = ::GetWindowFromUnknown(m_punkTarget);
	if (!pWnd || !pWnd->IsKindOf(RUNTIME_CLASS(CGridViewBase)))
		return false;

	HFONT hFont = (HFONT)pWnd->SendMessage(WM_GETFONT, 0, 0);
	if (!hFont) // system font
		hFont = (HFONT)::GetStockObject(SYSTEM_FONT);

	CFont * pFont = CFont::FromHandle(hFont);

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	if (!pFont->GetLogFont(&lf))
		return false;

	LONG lNewHeight = (LONG)((double)lf.lfHeight * 1.1);
	if (lNewHeight == lf.lfHeight)
	{
		if (lNewHeight < 0)
			lNewHeight--;
		else
			lNewHeight++;
	}
	lf.lfHeight = lNewHeight;
	lf.lfWidth = 0;

	CFont font;
	if (!font.CreateFontIndirect(&lf))
		return false;

	pWnd->SendMessage(WM_SETFONT, (WPARAM)((HFONT)font), MAKELPARAM(true, 0));
	font.DeleteObject();

	return true;
}

//extended UI
bool CActionZoomInTable::IsAvaible()
{
	if (!CheckInterface(m_punkTarget, IID_IGridView))
		return false;

	return true;
}



//////////////////////////////////////////////////////////////////////
//CActionCActivateClassView implementation
CString CActionShowMeasView::GetViewProgID()
{
	return "DataView.MeasView";
}

//////////////////////////////////////////////////////////////////////
//CActionCActivateClassView implementation
CString CActionShowClassView::GetViewProgID()
{
	return "DataView.ClassView";
}




//////////////////////////////////////////////////////////////////////
//CActionSetParamValue implementation
/*
CActionSetParamValue::CActionSetParamValue()
{
	m_lParamKey = -1;
	m_fNewValue = m_fPrevValue = 0;
	m_bNeedNewValue = m_bHasPrevValue = false; 
}

CActionSetParamValue::~CActionSetParamValue()
{
}

bool CActionSetParamValue::ExecuteSettings(CWnd *pwndParent)
{
	if (!m_punkTarget)
		return false;

	m_lParamKey = GetArgumentInt(_T("ParamID"));

	CString strListName = GetArgumentString(_T("ObjList"));
	CString strObjName = GetArgumentString(_T("Object"));

	m_lParamKey = GetArgumentInt(_T("ParamID"));
	m_fNewValue = GetArgumentDouble(_T("ParamValue"));

	_variant_t var = GetArgument(_T("NeedValue"));
	m_bNeedNewValue = (bool)var;

	// find object list
	if (!strListName.IsEmpty())
		m_list.Attach(::GetObjectByName(m_punkTarget, strListName, szTypeObjectList), false);
	else 
		m_list.Attach(GetActiveList(), false);

	if (m_list == 0)
		return false;

	strListName = ::GetObjectName(m_list);
	SetArgument(_T("ObjList"), _variant_t((LPCTSTR)strListName));

	// find object
	if (!strObjName.IsEmpty())
	{
		IUnknownPtr sptr(::FindChildByName(m_list, strObjName), false);
		m_sptrObject = sptr;
	}
	if (m_sptrObject == 0)
	{
		POSITION pos = m_list.GetCurPosition();
		if (pos)
		{
			IUnknownPtr sptr(m_list.GetNextObject(pos), false);
			m_sptrObject = sptr;
		}
	}
	if (m_sptrObject == 0)
		return false;

	strObjName = ::GetObjectName(m_sptrObject);
	SetArgument(_T("Object"), _variant_t((LPCTSTR)strObjName));

	// find param key
	if (m_lParamKey == -1)
	{
		IMeasParamGroup2Ptr sptrG = m_list;
		long lPos = 0;
		if (sptrG != 0 && SUCCEEDED(sptrG->GetActiveParamPos(&lPos)) && lPos)
			sptrG->GetParamInfo(lPos, &m_lParamKey, 0, 0, 0);
	}

	if (m_lParamKey == -1)
		return false;

	SetArgument(_T("ParamID"), _variant_t((long)m_lParamKey));
	// GetPrev value
	if (SUCCEEDED(m_sptrObject->GetValue(m_lParamKey, &m_fPrevValue)))
		m_bHasPrevValue = true;

	return true;
}

bool CActionSetParamValue::Invoke()
{
	if (!m_punkTarget || m_sptrObject == 0 || m_lParamKey == -1)
		return false;

	if (m_bNeedNewValue == m_bHasPrevValue && m_bNeedNewValue == false)
		return true;

	// find manual measure object (if any)
	IMeasureObjectPtr sptrM = m_sptrObject;
	IUnknownPtr spMM;
	if (sptrM != 0 && SUCCEEDED(sptrM->GetChildByParam(m_lParamKey, &spMM)) && spMM != 0)
		m_changes.RemoveFromDocData(m_punkTarget, spMM);

	if (m_bNeedNewValue)
	{
		if (FAILED(m_sptrObject->SetValue(m_lParamKey, m_fNewValue)))
			return false;
	}
	else
	{
		m_sptrObject->RemoveValue(m_lParamKey);
	}

	IMeasureObjectPtr sptrMeas = m_sptrObject;
	if (sptrMeas)
		sptrMeas->UpdateParent();
	return true;
}

//undo/redo
bool CActionSetParamValue::DoUndo()
{
	if (!m_punkTarget || m_sptrObject == 0 || m_lParamKey == -1)
		return false;

	m_changes.DoUndo(m_punkTarget);

	if (m_bNeedNewValue == m_bHasPrevValue && m_bNeedNewValue == false)
		return true;

	if (m_bHasPrevValue)
	{
		if (FAILED(m_sptrObject->SetValue(m_lParamKey, m_fPrevValue)))
			return false;
	}
	else if (m_bNeedNewValue)
	{
		m_sptrObject->RemoveValue(m_lParamKey);
	}

	IMeasureObjectPtr sptrMeas = m_sptrObject;
	if (sptrMeas)
		sptrMeas->UpdateParent();

	return true;
}

bool CActionSetParamValue::DoRedo()
{
	if (!m_punkTarget || m_sptrObject == 0 || m_lParamKey == -1)
		return false;

	if (m_bNeedNewValue == m_bHasPrevValue && m_bNeedNewValue == false)
		return true;

	m_changes.DoRedo(m_punkTarget);

	if (m_bNeedNewValue)
	{
		if (FAILED(m_sptrObject->SetValue(m_lParamKey, m_fNewValue)))
			return false;
	}
	else if (m_bHasPrevValue)
	{
		m_sptrObject->RemoveValue(m_lParamKey);
	}

	IMeasureObjectPtr sptrMeas = m_sptrObject;
	if (sptrMeas)
		sptrMeas->UpdateParent();
	return true;
}

//return the active object list
IUnknown * CActionSetParamValue::GetActiveList()
{
	return ::GetActiveObjectFromDocument(m_punkTarget, szTypeObjectList);
}

//////////////////////////////////////////////////////////////////////
//CActionShowMeasView implementation
*/



//////////////////////////////////////////////////////////////////////
//CActionRemoveParam implementation
CActionRemoveParam::CActionRemoveParam()
{
}

CActionRemoveParam::~CActionRemoveParam()
{
}

bool CActionRemoveParam::Invoke()
{
	long	lParamKey = GetArgumentInt( "ParamKey" );
	//get context
	IUnknown	*punkC = 0;
	IDocumentSitePtr	ptrDocSite( m_punkTarget );
	ptrDocSite->GetActiveView( &punkC );
	ASSERT( &punkC );
	IDataContext2Ptr	ptrContext( punkC );
	punkC->Release();


	IUnknown * punkList = ::GetActiveObjectFromContext( ptrContext, szTypeObjectList);

	if( !punkList  )
	{
		AfxMessageBox( IDS_NO_CATIVE_DATAOBJECT );
		return false;
	}

	m_ptrC = punkList;
	punkList->Release();


	if( lParamKey == -1 )
		m_ptrC->GetCurentPosition( 0, &lParamKey );

	ParameterContainer	*p = 0;
	m_ptrC->ParamDefByKey( lParamKey, &p );

	if( p == 0 )
	{
		AfxMessageBox( IDS_INVALIDKEY );
		return false;
	}

	if( p->pDescr->pos != 0 )
	{
		AfxMessageBox( IDS_NO_MANUAL_PARAMETER );
		return false;
	}

	// A.M. BT4304
	// Save parameter info for Undo.
	m_bAnyObjectsWasExist = false;
	m_lKey = lParamKey;
	m_sName = p->pDescr->bstrName;
	m_sUserName = p->pDescr->bstrUserName;
	m_type = p->type;

	m_ptrC->RemoveParameterAt( p->lpos );
    

	INamedDataObject2Ptr	ptrNamedObject( m_ptrC );
	long lCount = 0;
	int nObject = 0;
	ptrNamedObject->GetChildsCount(&lCount);
	m_Values.alloc(lCount);
	m_ValuesInit.alloc(lCount);
	memset(m_ValuesInit.ptr(),0,lCount*sizeof(BOOL));

	TPOS	lpos = 0;
	ptrNamedObject->GetFirstChildPosition( &lpos );

	while( lpos )
	{
		IUnknown	*punk = 0;
		ptrNamedObject->GetNextChild( &lpos, &punk );
		if( !punk )continue;
		INamedDataObject2Ptr	ptrMeas = punk;
		punk->Release();

		ICalcObjectPtr	ptrCalc( ptrMeas );
		double dValue;
		if (ptrCalc->GetValue(lParamKey, &dValue) == S_OK)
		{
			m_Values.ptr()[nObject] = dValue;
			m_ValuesInit.ptr()[nObject] = TRUE;
		}
		else
			m_ValuesInit.ptr()[nObject] = FALSE;
		nObject++;

		TPOS	lpos_m = 0;
		ptrMeas->GetFirstChildPosition( &lpos_m );

		while( lpos_m ) 
		{
			IUnknown	*punkMeasMan = 0;
			ptrMeas->GetNextChild( &lpos_m, &punkMeasMan );
			if( !punkMeasMan )continue;
			IManualMeasureObjectPtr ptrMeasO( punkMeasMan );
			punkMeasMan->Release();

			if( ptrMeasO == 0 )continue;

			long	lKey = 0;
			ptrMeasO->GetParamInfo( &lKey, 0 );

			if( lKey == lParamKey )
			{
				m_changes.RemoveFromDocData( m_punkTarget, ptrMeasO );
				m_bAnyObjectsWasExist = true;
			}
		}
		if( ptrCalc != 0 )
			ptrCalc->RemoveValue( lParamKey );

	}

	return true;
}

//undo/redo
bool CActionRemoveParam::DoUndo()
{
	if (m_bAnyObjectsWasExist)
		m_changes.DoUndo(m_punkTarget);
	else
	{

		_bstr_t	bstrMeasGroup("{E3985D92-5C77-4644-8EA5-9707360F133D}" );
		GUID	guid;
		::CLSIDFromString( bstrMeasGroup, &guid );
		guid = ::GetExternalGuid( guid );
		IUnknownPtr	punkGroup(FindMeasGroup( guid ), false);
		IMeasParamGroupPtr		ptrG = punkGroup;
		if( ptrG == 0 )
			return false;

		LONG_PTR	lpos = 0;
		m_ptrC->DefineParameter( -1, m_type, ptrG, &lpos );
		ParameterContainer	*pc = 0;
		m_ptrC->GetNextParameter( &lpos, &pc );
		m_lKey = pc->pDescr->lKey;
		ASSERT(pc);
		::SysFreeString( pc->pDescr->bstrName );
		::SysFreeString( pc->pDescr->bstrUserName );
		pc->pDescr->bstrName = m_sName.AllocSysString();
		pc->pDescr->bstrUserName = m_sUserName.AllocSysString();
		m_ptrC->DefineParameterFull( pc, 0 );

		int nObject = 0;
		INamedDataObject2Ptr	ptrNamedObject( m_ptrC );
		TPOS	lpos1 = 0;
		ptrNamedObject->GetFirstChildPosition( &lpos1 );
		while( lpos1 )
		{
			IUnknownPtr	punk;
			ptrNamedObject->GetNextChild( &lpos1, &punk );
			if( punk == 0 )continue;
			ICalcObjectPtr	ptrCalc( punk );
			if (m_ValuesInit.ptr()[nObject])
				ptrCalc->SetValue(m_lKey, m_Values.ptr()[nObject]);
			nObject++;
		}
	}
	return true;
}

bool CActionRemoveParam::DoRedo()
{
	m_changes.DoRedo(m_punkTarget);
	ParameterContainer	*p = 0;
	m_ptrC->ParamDefByKey( m_lKey, &p );
	m_ptrC->RemoveParameterAt( p->lpos );
	return true;
}

bool CActionRemoveParam::IsAvaible()
{
	IUnknownPtr punkList(GetActiveObjectFromDocument(m_punkTarget, szTypeObjectList), false);
	if( punkList == 0) return false;

	//get the action manager
	IUnknown *punkAM = ::_GetOtherComponent( GetAppUnknown(), IID_IActionManager );
	sptrIActionManager	sptrAM( punkAM );
	punkAM->Release();
	DWORD	dwExecuteFlags = 0;
	sptrAM->GetCurrentExecuteFlags( &dwExecuteFlags );
	if(!(dwExecuteFlags&aefRunning) && !(dwExecuteFlags&aefScriptRunning))
	{
		long lParamKey;
		ICalcObjectContainerPtr ptrC(punkList);
		ptrC->GetCurentPosition( 0, &lParamKey );
		ParameterContainer	*p = 0;
		ptrC->ParamDefByKey( lParamKey, &p );
		if( p == 0 )
			return false;
		if( p->pDescr->pos != 0 )
			return false;
	}
	return true;
}


//[ag]4. olecreate release
//[ag]5. guidinfo release
// {06A873F3-9D2E-46a6-817A-8C8A9F71B24F}
// {17D1DD6C-32F8-4391-BDF7-43513561A25C}



//////////////////////////////////////////////////////////////////////
//CActionCreateParam implementation

CActionCreateParam::CActionCreateParam()
{
	m_pCreateKeys = 0;
}

CActionCreateParam::~CActionCreateParam()
{
	if( m_pCreateKeys )
		delete m_pCreateKeys;
}


static CString GenerateParameterName(ICalcObjectContainer *pco, LPCTSTR pszParamName)
{
	CString sName;
	for (int i = 0; ; i++)
	{
		if (i == 0)
			sName = pszParamName;
		else
			sName.Format(_T("%s%d"), pszParamName, i);
		bool bFound = false;
		LONG_PTR lpos;
		pco->GetFirstParameterPos(&lpos);
		while (lpos)
		{
			ParameterContainer *pCont;
			pco->GetNextParameter(&lpos, &pCont);
			if (pCont)
			{
				_bstr_t bstrName(pCont->pDescr->bstrName);
				if (_tcsicmp(sName, (LPCTSTR)bstrName) == 0)
				{
					bFound = true;
					break;
				}
			}
		}
		if (!bFound)
			break;
	}
	return sName;
}


bool CActionCreateParam::Invoke()
{
	if (!m_punkTarget)
		return false;

	CString	strType = GetArgumentString(_T("ParamType"));

	if( strType == "Linear" )m_type = etLinear;
	else if( strType == "Angle" )m_type = etAngle;
	else if( strType == "Counter" )m_type = etCounter;
	else if( strType == "Undefined" )m_type = etUndefined;
	else 
	{
		AfxMessageBox( IDS_INVALID_PARAM_TYPE );
		return false;
	}


	m_strParamName = GetArgumentString(_T("ParamName"));

	if( m_strParamName.IsEmpty() )
	{
		AfxMessageBox( IDS_INVALID_PARAM_NAME );
		return false;
	}

	m_nCount = GetArgumentInt(_T("Number"));

	if( m_nCount <= 0 || m_nCount >= 100 ) 
	{
		CString	str;
		str.Format( "Parameter count %d is invalid", m_nCount );
		AfxMessageBox( str );
		return false;
	}

	//get context
	IUnknown	*punkC = 0;
	IDocumentSitePtr	ptrDocSite( m_punkTarget );
	ptrDocSite->GetActiveView( &punkC );
	ASSERT( &punkC );
	IDataContext2Ptr	ptrContext( punkC );
	punkC->Release();


	IUnknown * punkList = ::GetActiveObjectFromContext( ptrContext, szTypeObjectList);
	if (!punkList)
	{
		//create an object list
		punkList = ::CreateTypedObject(szTypeObjectList);
		if (!punkList)
			return false;

		IUnknown	*punkImage = ::GetActiveObjectFromContext( ptrContext, szTypeImage );
		if( punkImage )
		{
			GUID	baseKey;
			INamedDataObject2Ptr	
			ptrNamed = punkImage;
			ptrNamed->GetBaseKey( &baseKey );
			ptrNamed = punkList;
			ptrNamed->SetBaseKey( &baseKey );

			punkImage->Release();
		}
		m_changes.SetToDocData( m_punkTarget, punkList);

		ptrContext->SetActiveObject(_bstr_t(szTypeObjectList), punkList, aofActivateDepended);
		::AjustViewForObject( ptrContext, punkList );
	}
	m_ptrC = punkList;
	punkList->Release();

	_bstr_t	bstrMeasGroup("{E3985D92-5C77-4644-8EA5-9707360F133D}" );
	GUID	guid;
	::CLSIDFromString( bstrMeasGroup, &guid );
	guid = ::GetExternalGuid( guid );

	IUnknown	*punkGroup = FindMeasGroup( guid );
	if( !punkGroup )
	{
		AfxMessageBox("No manual measurement group found" );
		return false;
	}

	m_ptrG = punkGroup;
	punkGroup->Release();

	m_pCreateKeys = new long[m_nCount];
	//DefineParamerers();
	
	for( int i = 0; i < m_nCount; i++ )
	{
		CString	str;
//		if( i == 0 )str = m_strParamName;
//		else str.Format( "%s%d", (const char*)m_strParamName, i );
		str = GenerateParameterName(m_ptrC, m_strParamName);

		LONG_PTR	lpos = 0;
		m_ptrC->DefineParameter( -1, m_type, m_ptrG, &lpos );
		ParameterContainer	*pc = 0;
		m_ptrC->GetNextParameter( &lpos, &pc );
		ASSERT(pc);
		m_pCreateKeys[i] = pc->pDescr->lKey;
		::SysFreeString( pc->pDescr->bstrName );
		::SysFreeString( pc->pDescr->bstrUserName );
		pc->pDescr->bstrName = str.AllocSysString();
		pc->pDescr->bstrUserName = str.AllocSysString();
		m_ptrC->DefineParameterFull( pc, 0 );
	}

	return true;
}


bool CActionCreateParam::DoUndo()
{
	for( long	l = 0; l <m_nCount; l++ )
		m_ptrC->RemoveParameter( m_pCreateKeys[l] );
	return false;
}
bool CActionCreateParam::DoRedo()
{
	for( int i = 0; i < m_nCount; i++ )
	{
		CString	str;
		if( i == 0 )str = m_strParamName;
		else str.Format( "%s%d", (const char*)m_strParamName, i );

		LONG_PTR	lpos = 0;
		m_ptrC->DefineParameter( -1, m_type, m_ptrG, &lpos );
		ParameterContainer	*pc = 0;
		m_ptrC->GetNextParameter( &lpos, &pc );
		ASSERT(pc);
		m_pCreateKeys[i] = pc->pDescr->lKey;
		::SysFreeString( pc->pDescr->bstrName );
		::SysFreeString( pc->pDescr->bstrUserName );
		pc->pDescr->bstrName = str.AllocSysString();
		pc->pDescr->bstrUserName = str.AllocSysString();
		m_ptrC->DefineParameterFull( pc, 0 );
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
bool CHeightUndoImpl::Remember(IUnknown *punkList )
{
	m_sptrObjList = punkList;
	INamedDataObject2Ptr ptrList = punkList;

	if( ptrList == 0 )
		return false;
    	
	TPOS lPos = 0;
	ptrList->GetFirstChildPosition( &lPos );
	
	m_Heights.clear();
	m_Objects.clear();

	while( lPos )
	{
		IUnknownPtr sptrObject;
		ptrList->GetNextChild( &lPos, &sptrObject );

		INamedPropBagPtr sptrBag = sptrObject;
		
		if( sptrBag != 0 )
		{
			_variant_t var;
			sptrBag->GetProperty( _bstr_t( ROW_HEIGHT ), &var );
			if( var.vt == VT_EMPTY )
				var = -1;

			m_Heights.add_tail( (long)var );
			m_Objects.add_tail( sptrBag );
		}
	}

	return true;
}

bool CHeightUndoImpl::_undo_redo( IUnknown *punkDoc )
{
	if( !punkDoc )
		return false;

	for (TPOS lPos = m_Objects.head(), lPos2 = m_Heights.head(); lPos; lPos = m_Objects.next(lPos), lPos2 = m_Heights.next(lPos2))
	{
		INamedPropBag *ptrBag = m_Objects.get( lPos );
		long *plVal = &m_Heights.get( lPos2 );

		_variant_t var;
		ptrBag->GetProperty( _bstr_t( ROW_HEIGHT ), &var );
		if( var.vt == VT_EMPTY )
			var = -1;

		swap( &var.lVal, plVal );

		ptrBag->SetProperty( _bstr_t( ROW_HEIGHT ), var );
	}

	::FireEventNotify( punkDoc, szEventChangeObjectList, m_sptrObjList, 0, cncReset );
    return true;
}

void CHeightUndoImpl::get_named_data_object_list( IUnknown **ppunkND )
{
	if( !ppunkND || m_sptrObjList == 0 )
		return;

	INamedDataObject2Ptr pstrNDObject2 = m_sptrObjList;
	if( pstrNDObject2 == 0 )
		return;

	*ppunkND = 0;
	pstrNDObject2->GetData( ppunkND );
	if( !(*ppunkND) )
	{
		pstrNDObject2->InitAttachedData( );
		pstrNDObject2->GetData( ppunkND );
	}    
}



//////////////////////////////////////////////////////////////////////
CActionAlignByImage::CActionAlignByImage()
{
	m_lold_img_col_width = -1;
}

CActionAlignByImage::~CActionAlignByImage(){}

bool CActionAlignByImage::Invoke()
{
	IMeasureViewPtr ptr_view( m_punkTarget );
	if( ptr_view == 0 )
		return false;

	IUnknown	*punk = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	if( !punk  )
	{
		AfxMessageBox( IDS_NO_CATIVE_DATAOBJECT );
		return false;
	}

	IViewPtr ptrView( m_punkTarget );
	if(ptrView==0) return false;
	IUnknownPtr ptrDoc;
	ptrView->GetDocument( &ptrDoc );
	if(ptrDoc==0) return false;
	
	IActionPtr ptrAction(GetControllingUnknown());
	ptrAction->AttachTarget( ptrDoc );

	bool bRetVal = Remember( punk );
	get_image_column_width( &m_lold_img_col_width );

	punk->Release( );
	punk = 0;
	if( bRetVal )
		bRetVal = (S_OK == ptr_view->AlignByImage());

	return true;
}

void CActionAlignByImage::get_image_column_width( long *plwidth )
{
	if( !plwidth || m_sptrObjList == 0 )
		return;

	*plwidth = -1;
	IUnknown *punkND = 0;
	get_named_data_object_list( &punkND );
	INamedDataPtr sptrND = punkND;
	if( punkND )
		punkND->Release(); punkND = 0;

	if( sptrND == 0 )
		return;

    _variant_t _var( long( -1 ) );
	sptrND->GetValue( _bstr_t( szMeasViewImageColumnWidthSection ), &_var );
	*plwidth = _var.lVal;
}

void CActionAlignByImage::set_image_column_width( long lwidth )
{
	if( m_sptrObjList == 0 )
		return;

	IUnknown *punkND = 0;
	get_named_data_object_list( &punkND );
	INamedDataPtr sptrND = punkND;
	if( punkND )
		punkND->Release(); punkND = 0;

	if( sptrND == 0 )
		return;

	_variant_t _var( lwidth );
	sptrND->SetValue( _bstr_t( szMeasViewImageColumnWidthSection ), _var );
}

bool CActionAlignByImage::IsAvaible()
{
	IMeasureViewPtr ptr_view( m_punkTarget );
	if( ptr_view == 0 )
		return false;

	return true;
}

bool CActionAlignByImage::_undo_redo( IUnknown *punkDoc )
{
	long lwidth = 0;
	get_image_column_width( &lwidth );
	set_image_column_width( m_lold_img_col_width );
	m_lold_img_col_width = lwidth;
	return __super::_undo_redo( punkDoc ) ;    
}

//////////////////////////////////////////////////////////////////////
CActionCopySelected::CActionCopySelected()
{
	m_cf = cf_selected;
}

//////////////////////////////////////////////////////////////////////
bool CActionCopySelected::Invoke()
{
	IMeasureViewPtr ptr_view( m_punkTarget );
	if( ptr_view == 0 )
		return false;

	ptr_view->CopyToClipboard( m_cf );

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CActionCopySelected::IsAvaible()
{
	IMeasureViewPtr ptr_view( m_punkTarget );
	if( ptr_view == 0 )
		return false;

	return true;
}


//////////////////////////////////////////////////////////////////////
CActionCopyAll::CActionCopyAll()
{
	m_cf = cf_all;
}
