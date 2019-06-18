#include "stdafx.h"
#include "axactions.h"
#include "resource.h"
#include "other\insctldg.h"
#include "afxpriv2.h"
#include "EventDlg.h"
#include "scriptdocint.h"
#include "dbase.h"
#include "ShowIDDlg.h"
#include "dbase.h"
#include "proppage.h"
#include <PropBag.h>

#if _MSC_VER >= 1300
	#define _L(c)	L##c
#endif

/////////////////////////////////////////////////////////////////////////////////////////
//CAxActionBase
CAxActionBase::CAxActionBase()
{
}

CAxActionBase::~CAxActionBase()
{
	POSITION	pos = GetFirstPosition();

	while( pos )
		GetNext( pos )->Release();
	
	m_ptrList.RemoveAll();
}


POSITION CAxActionBase::GetFirstPosition()
{
	return m_ptrList.GetHeadPosition();
		
}

IUnknown *CAxActionBase::GetNext( POSITION &pos )
{
	return (IUnknown*)m_ptrList.GetNext( pos );
}

int CAxActionBase::GetControlsCount()
{
	return m_ptrList.GetCount();
}

bool CAxActionBase::InitList()
{
	if( m_ptrList.GetCount() != 0 )
		return true;
	//taget is document

	sptrIDocumentSite	sptrDocSite( m_punkTarget );
	IUnknown	*punkV = 0;
	sptrDocSite->GetActiveView( &punkV );

	if( !punkV )
		return false;

	sptrIView	sptrV = punkV;
	punkV->Release();

	if( !CheckInterface( sptrV, IID_ILayoutView ) )
		return false;



	IUnknown	*punkFrame = 0;
	sptrV->GetMultiFrame( &punkFrame, false );

	ASSERT(punkFrame);

	sptrIMultiSelection sptrSel( punkFrame );
	punkFrame->Release();

	CString strFormType = szTypeAXForm;

	if( CheckInterface( sptrV, IID_IReportView ) )
	{
		strFormType = szTypeReportForm;
	}
	else
	if( CheckInterface( sptrV, IID_IBlankView ) )
	{
		strFormType = szTypeBlankForm;
	}

	IUnknown	*punkForm = ::GetActiveObjectFromContext( sptrV, strFormType );

	if( !punkForm )
		return false;

	m_form = punkForm;
	punkForm->Release();


	DWORD nCount = 0;
	sptrSel->GetObjectsCount( &nCount );

	for( DWORD nObj = 0; nObj < nCount; nObj++ )
	{

		IUnknown *punkObj = 0;
		sptrSel->GetObjectByIdx( nObj, &punkObj );

		if( CheckInterface( punkObj, IID_IActiveXCtrl ) )
			m_ptrList.AddTail( punkObj );
		else
			punkObj->Release();
	}

	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////
//CAxActionBase

CAxLayoutActionBase::CAxLayoutActionBase()
{
	m_prectSource = 0;
	m_prectTarget = 0;
	m_bNoResize = false;
}

CAxLayoutActionBase::~CAxLayoutActionBase()
{
	if( m_prectSource )
		delete m_prectSource;

	if( m_prectTarget )
		delete m_prectTarget;
}

bool CAxLayoutActionBase::Invoke()
{
	if( !InitList() )
		return false;

	int	nControlsCount = GetControlsCount();

	m_prectSource = new CRect[nControlsCount];
	m_prectTarget = new CRect[nControlsCount];

	POSITION pos = GetFirstPosition();
	int	nPos = 0;

	while( pos )
	{
		sptrIActiveXCtrl	sptrC( GetNext( pos ) );

		sptrC->GetRect( &m_prectSource[nPos] );
		m_prectTarget[nPos] = m_prectSource[nPos];
		nPos++;
	}

	if( !DoChangeLayout() )
		return false;

	pos = GetFirstPosition();
	nPos = 0;

	while( pos )
	{
		sptrIActiveXCtrl	sptrC( GetNext( pos ) );

		sptrC->SetRect( m_prectTarget[nPos] );
		nPos++;
	}

	m_form->GetSize( &m_sizeOld );
	m_form->AutoUpdateSize();
	m_form->GetSize( &m_sizeNew );

	HANDLE h = 0;
	// если не ресайзили ни один объект - то запретим лишние телодвижения
	if(m_bNoResize) h = CreateMutex( 0, false, "ResizeView.lock.1C6E8B14-678E-4553-A16E-9705EA42F170" );
	::UpdateDataObject( m_punkTarget, m_form );
	if(h) CloseHandle(h);


	{
		sptrIDocumentSite	sptrDocSite( m_punkTarget );
		IUnknown	*punkV = 0;
		sptrDocSite->GetActiveView( &punkV );

		if( !punkV )
			return false;

		sptrIView	sptrV = punkV;
		punkV->Release();

		if( sptrV == NULL )
			return false;

		IUnknown	*punkFrame = 0;
		sptrV->GetMultiFrame( &punkFrame, false );

		if( !punkFrame )
			return false;


		sptrIMultiSelection sptrSel( punkFrame );
		punkFrame->Release();

		if( sptrSel == NULL )
			return false;
		
		sptrSel->SyncObjectsRect();

	}
	return true;
}

bool CAxLayoutActionBase::DoUndo()
{
	POSITION pos = GetFirstPosition();
	int nPos = 0;

	while( pos )
	{
		sptrIActiveXCtrl	sptrC( GetNext( pos ) );

		sptrC->SetRect( m_prectSource[nPos] );
		nPos++;
	}

	m_form->SetSize( m_sizeOld );
	::UpdateDataObject( m_punkTarget, m_form );
	return true;
}

bool CAxLayoutActionBase::DoRedo()
{
	POSITION pos = GetFirstPosition();
	int nPos = 0;

	while( pos )
	{
		sptrIActiveXCtrl	sptrC( GetNext( pos ) );

		sptrC->SetRect( m_prectTarget[nPos] );
		nPos++;
	}
	m_form->SetSize( m_sizeNew );
	::UpdateDataObject( m_punkTarget, m_form );
	return true;
}

bool CAxLayoutActionBase::IsAvaible()
{
	if( !InitList() )
		return false;
	return GetControlsCount() >= GetMinControlsCount();
}

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionShowControlID, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionincMoveSize, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAddEventHandlers, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAxSetSize, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAxMakeSameSize, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAxMakeSameHeight, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAxMakeSameWidth, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAxSpaceDown, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAxSpaceAcross, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAxAlignBottom, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAxAlignRight, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAxAlignTop, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAxAlignLeft, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAxProperties, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAxResize, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAxInsertControl, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowAxLayout, CCmdTargetEx);
//[ag]4. olecreate release

// {60BB4234-CD41-497d-82E4-2399F9482850}
GUARD_IMPLEMENT_OLECREATE(CActionShowControlID, "axeditor.ShowControlID",
0x60bb4234, 0xcd41, 0x497d, 0x82, 0xe4, 0x23, 0x99, 0xf9, 0x48, 0x28, 0x50);
// {F45C81E4-5E57-4a11-84EF-BEA74C10F683}
GUARD_IMPLEMENT_OLECREATE(CActionincMoveSize, "axeditor.incMoveSize",
0xf45c81e4, 0x5e57, 0x4a11, 0x84, 0xef, 0xbe, 0xa7, 0x4c, 0x10, 0xf6, 0x83);
// {AD5072E3-DA31-11d3-A097-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionAddEventHandlers, "axeditor.AddEventHandlers",
0xad5072e3, 0xda31, 0x11d3, 0xa0, 0x97, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {7C6A9BFE-F457-4b89-A7B8-E74967E350C5}
GUARD_IMPLEMENT_OLECREATE(CActionAxSetSize, "axeditor.AxSetSize",
0x7c6a9bfe, 0xf457, 0x4b89, 0xa7, 0xb8, 0xe7, 0x49, 0x67, 0xe3, 0x50, 0xc5);
// {B8F4F391-3FD0-4f41-BCF9-F7A6AE8DBA59}
GUARD_IMPLEMENT_OLECREATE(CActionAxMakeSameSize, "axeditor.AxMakeSameSize",
0xb8f4f391, 0x3fd0, 0x4f41, 0xbc, 0xf9, 0xf7, 0xa6, 0xae, 0x8d, 0xba, 0x59);
// {F16F2452-68DB-410e-9504-10360628804E}
GUARD_IMPLEMENT_OLECREATE(CActionAxMakeSameHeight, "axeditor.AxMakeSameHeigh",
0xf16f2452, 0x68db, 0x410e, 0x95, 0x4, 0x10, 0x36, 0x6, 0x28, 0x80, 0x4e);
// {18F3F74B-A6F2-4a6b-A105-E67F64251C65}
GUARD_IMPLEMENT_OLECREATE(CActionAxMakeSameWidth, "axeditor.AxMakeSameWidth",
0x18f3f74b, 0xa6f2, 0x4a6b, 0xa1, 0x5, 0xe6, 0x7f, 0x64, 0x25, 0x1c, 0x65);
// {0A9A5C2D-7F90-4600-9CA3-87E13DF0F818}
GUARD_IMPLEMENT_OLECREATE(CActionAxSpaceDown, "axeditor.AxSpaceDown",
0xa9a5c2d, 0x7f90, 0x4600, 0x9c, 0xa3, 0x87, 0xe1, 0x3d, 0xf0, 0xf8, 0x18);
// {A255E4FC-83D5-44e0-A1BD-0372CB94C11E}
GUARD_IMPLEMENT_OLECREATE(CActionAxSpaceAcross, "axeditor.AxSpaceAcross",
0xa255e4fc, 0x83d5, 0x44e0, 0xa1, 0xbd, 0x3, 0x72, 0xcb, 0x94, 0xc1, 0x1e);
// {0006C789-8D87-4154-87B8-C6A022CBA342}
GUARD_IMPLEMENT_OLECREATE(CActionAxAlignBottom, "axeditor.AxAlignBottom",
0x6c789, 0x8d87, 0x4154, 0x87, 0xb8, 0xc6, 0xa0, 0x22, 0xcb, 0xa3, 0x42);
// {228CA037-25A2-4acd-A15E-BE61A6D7C39E}
GUARD_IMPLEMENT_OLECREATE(CActionAxAlignRight, "axeditor.AxAlignRight",
0x228ca037, 0x25a2, 0x4acd, 0xa1, 0x5e, 0xbe, 0x61, 0xa6, 0xd7, 0xc3, 0x9e);
// {67E25FDC-2282-40a4-AA3F-6C79EF794D2A}
GUARD_IMPLEMENT_OLECREATE(CActionAxAlignTop, "axeditor.AxAlignTop",
0x67e25fdc, 0x2282, 0x40a4, 0xaa, 0x3f, 0x6c, 0x79, 0xef, 0x79, 0x4d, 0x2a);
// {381F6D70-38F7-4caa-A0D1-0BC21630C157}
GUARD_IMPLEMENT_OLECREATE(CActionAxAlignLeft, "axeditor.AxAlignLeft",
0x381f6d70, 0x38f7, 0x4caa, 0xa0, 0xd1, 0xb, 0xc2, 0x16, 0x30, 0xc1, 0x57);
// {695995A7-B6F2-45b5-861E-D3D4DED9B948}
GUARD_IMPLEMENT_OLECREATE(CActionAxProperties, "axeditor.AxProperties",
0x695995a7, 0xb6f2, 0x45b5, 0x86, 0x1e, 0xd3, 0xd4, 0xde, 0xd9, 0xb9, 0x48);
// {5BFFF899-256F-4aa4-8AFC-56CB10214CE5}
GUARD_IMPLEMENT_OLECREATE(CActionAxResize, "AxEditor.AxChangeForm",
0x5bfff899, 0x256f, 0x4aa4, 0x8a, 0xfc, 0x56, 0xcb, 0x10, 0x21, 0x4c, 0xe5);
// {B0A398AE-4B4B-4e76-A52B-43C4BA1FC3D9}
GUARD_IMPLEMENT_OLECREATE(CActionAxInsertControl, "AxEditor.AxInsertControl",
0xb0a398ae, 0x4b4b, 0x4e76, 0xa5, 0x2b, 0x43, 0xc4, 0xba, 0x1f, 0xc3, 0xd9);
// {B315BAD4-BCFF-412f-A690-7E4744933109}
GUARD_IMPLEMENT_OLECREATE(CActionShowAxLayout, "AxEditor.ShowAxLayout",
0xb315bad4, 0xbcff, 0x412f, 0xa6, 0x90, 0x7e, 0x47, 0x44, 0x93, 0x31, 0x9);

//[ag]5. guidinfo release

// {C153F762-0B08-4895-8732-6D0A1B27B082}
static const GUID guidShowControlID =
{ 0xc153f762, 0xb08, 0x4895, { 0x87, 0x32, 0x6d, 0xa, 0x1b, 0x27, 0xb0, 0x82 } };
// {A74DC47C-36B2-4c93-8046-CF667A1FC8E6}
static const GUID guidincMoveSize =
{ 0xa74dc47c, 0x36b2, 0x4c93, { 0x80, 0x46, 0xcf, 0x66, 0x7a, 0x1f, 0xc8, 0xe6 } };
// {AD5072E1-DA31-11d3-A097-0000B493A187}
static const GUID guidAddEventHandlers =
{ 0xad5072e1, 0xda31, 0x11d3, { 0xa0, 0x97, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {281F2DAD-AA7F-4466-8A68-B0483133C12C}
static const GUID guidAxSetSize =
{ 0x281f2dad, 0xaa7f, 0x4466, { 0x8a, 0x68, 0xb0, 0x48, 0x31, 0x33, 0xc1, 0x2c } };
// {5700D5CD-B42F-4015-AED7-EC3460A8F012}
static const GUID guidAxMakeSameSize =
{ 0x5700d5cd, 0xb42f, 0x4015, { 0xae, 0xd7, 0xec, 0x34, 0x60, 0xa8, 0xf0, 0x12 } };
// {463198D9-98B8-436e-80AB-39253A719039}
static const GUID guidAxMakeSameHeigh =
{ 0x463198d9, 0x98b8, 0x436e, { 0x80, 0xab, 0x39, 0x25, 0x3a, 0x71, 0x90, 0x39 } };
// {E0977835-75D2-422b-88D5-2EE4F5A7D811}
static const GUID guidAxMakeSameWidth =
{ 0xe0977835, 0x75d2, 0x422b, { 0x88, 0xd5, 0x2e, 0xe4, 0xf5, 0xa7, 0xd8, 0x11 } };
// {27BCD43A-F33E-4b12-9D17-2F4E5D9D7AC8}
static const GUID guidAxSpaceDown =
{ 0x27bcd43a, 0xf33e, 0x4b12, { 0x9d, 0x17, 0x2f, 0x4e, 0x5d, 0x9d, 0x7a, 0xc8 } };
// {FCDAB131-7B29-4fb1-A5B9-0F67A903E9A0}
static const GUID guidAxSpaceAcross =
{ 0xfcdab131, 0x7b29, 0x4fb1, { 0xa5, 0xb9, 0xf, 0x67, 0xa9, 0x3, 0xe9, 0xa0 } };
// {BF194EB8-FD38-472e-B169-38C5F706AA9D}
static const GUID guidAxAlignBottom =
{ 0xbf194eb8, 0xfd38, 0x472e, { 0xb1, 0x69, 0x38, 0xc5, 0xf7, 0x6, 0xaa, 0x9d } };
// {CFDFA4A8-CFEA-418d-8A2E-7F64711FF29A}
static const GUID guidAxAlignRight =
{ 0xcfdfa4a8, 0xcfea, 0x418d, { 0x8a, 0x2e, 0x7f, 0x64, 0x71, 0x1f, 0xf2, 0x9a } };
// {23AB840C-D1FE-44ee-A0AE-9790387132E9}
static const GUID guidAxAlignTop =
{ 0x23ab840c, 0xd1fe, 0x44ee, { 0xa0, 0xae, 0x97, 0x90, 0x38, 0x71, 0x32, 0xe9 } };
// {8E134EB1-9EBE-4c9a-A3FB-2F05BAB1773A}
static const GUID guidAxAlignLeft =
{ 0x8e134eb1, 0x9ebe, 0x4c9a, { 0xa3, 0xfb, 0x2f, 0x5, 0xba, 0xb1, 0x77, 0x3a } };
// {5D081DCE-B473-42a2-B50E-4DB8C5917C02}
static const GUID guidAxProperties =
{ 0x5d081dce, 0xb473, 0x42a2, { 0xb5, 0xe, 0x4d, 0xb8, 0xc5, 0x91, 0x7c, 0x2 } };
// {A7AED91F-1098-4871-9F22-7305FFA5B266}
static const GUID guidAxChangeForm =
{ 0xa7aed91f, 0x1098, 0x4871, { 0x9f, 0x22, 0x73, 0x5, 0xff, 0xa5, 0xb2, 0x66 } };
// {7314F016-2BD6-49af-8632-9917082DB9A6}
static const GUID guidAxInsertControl =
{ 0x7314f016, 0x2bd6, 0x49af, { 0x86, 0x32, 0x99, 0x17, 0x8, 0x2d, 0xb9, 0xa6 } };
// {D9C43356-E6A9-4446-A5DE-27BAF3FFA2CE}
static const GUID guidShowAxLayout =
{ 0xd9c43356, 0xe6a9, 0x4446, { 0xa5, 0xde, 0x27, 0xba, 0xf3, 0xff, 0xa2, 0xce } };

//[ag]6. action info

ACTION_INFO_FULL(CActionShowControlID, IDS_ACTION_SHOWID, -1, -1, guidShowControlID);
ACTION_INFO_FULL(CActionincMoveSize, IDS_ACTION_MOVESIZE, -1, -1, guidincMoveSize);
ACTION_INFO_FULL(CActionAddEventHandlers, IDS_ACTION_ADD_EVENT_HANDLERS, IDS_MENU_VIEW, -1, guidAddEventHandlers);
ACTION_INFO_FULL(CActionAxSetSize, IDS_ACTION_SETSIZE, -1, -1, guidAxSetSize);
ACTION_INFO_FULL(CActionAxMakeSameSize, IDS_ACTION_MAKE_SAME_SIZE, -1, -1, guidAxMakeSameSize);
ACTION_INFO_FULL(CActionAxMakeSameHeight, IDS_ACTION_MAKE_SAME_HEIGHT, -1, -1, guidAxMakeSameHeigh);
ACTION_INFO_FULL(CActionAxMakeSameWidth, IDS_ACTION_MAKE_SAME_WIDTH, -1, -1, guidAxMakeSameWidth);
ACTION_INFO_FULL(CActionAxSpaceDown, IDS_ACTION_SPACE_DOWN, -1, -1, guidAxSpaceDown);
ACTION_INFO_FULL(CActionAxSpaceAcross, IDS_ACTION_SPACE_ACCROSS, -1, -1, guidAxSpaceAcross);
ACTION_INFO_FULL(CActionAxAlignBottom, IDS_ACTION_ALIGN_BOTTOM, -1, -1, guidAxAlignBottom);
ACTION_INFO_FULL(CActionAxAlignRight, IDS_ACTION_ALIGN_RIGHT, -1, -1, guidAxAlignRight);
ACTION_INFO_FULL(CActionAxAlignTop, IDS_ACTION_ALIGN_TOP, -1, -1, guidAxAlignTop);
ACTION_INFO_FULL(CActionAxAlignLeft, IDS_ACTION_ALIGN_LEFT, -1, -1, guidAxAlignLeft);
ACTION_INFO_FULL(CActionAxProperties, IDS_ACTION_PROPERTIES, -1, -1, guidAxProperties);
ACTION_INFO_FULL(CActionAxResize, IDS_ACTION_RESIZESIZE, -1, -1, guidAxChangeForm);
ACTION_INFO_FULL(CActionAxInsertControl, IDS_ACTION_INSERT_CONTROL, IDS_MENU_VIEW, -1, guidAxInsertControl);
ACTION_INFO_FULL(CActionShowAxLayout, IDS_ACTION_SHOW_AXLAYOUT, -1, -1, guidShowAxLayout);

//[ag]7. targets

ACTION_TARGET(CActionShowControlID, "anydoc");
ACTION_TARGET(CActionincMoveSize, "anydoc");
ACTION_TARGET(CActionAddEventHandlers, szTargetAnydoc);
ACTION_TARGET(CActionAxSetSize, "anydoc");
ACTION_TARGET(CActionAxMakeSameSize, "anydoc");
ACTION_TARGET(CActionAxMakeSameHeight, "anydoc");
ACTION_TARGET(CActionAxMakeSameWidth, "anydoc");
ACTION_TARGET(CActionAxSpaceDown, "anydoc");
ACTION_TARGET(CActionAxSpaceAcross, "anydoc");
ACTION_TARGET(CActionAxAlignBottom, "anydoc");
ACTION_TARGET(CActionAxAlignRight, "anydoc");
ACTION_TARGET(CActionAxAlignTop, "anydoc");
ACTION_TARGET(CActionAxAlignLeft, "anydoc");
ACTION_TARGET(CActionAxProperties, "anydoc");
ACTION_TARGET(CActionAxResize, szTargetViewSite);
ACTION_TARGET(CActionAxInsertControl, szTargetViewSite);
ACTION_TARGET(CActionShowAxLayout, szTargetFrame);

//[ag]8. arguments

ACTION_ARG_LIST(CActionAxInsertControl)
	ARG_STRING("ProgID", 0)
	ARG_INT (_T("Views"), 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionAxSetSize)
	ARG_INT("left", 10)
	ARG_INT("top", 10)
	ARG_INT("right", 110)
	ARG_INT("bottom", 110)
END_ACTION_ARG_LIST()


ACTION_ARG_LIST(CActionincMoveSize)
	ARG_INT("dx", 0)
	ARG_INT("dy", 0)
	ARG_INT("dcx", 0)
	ARG_INT("dcy", 0)
END_ACTION_ARG_LIST()


ACTION_ARG_LIST(CActionAxResize)
	ARG_INT("width", -1)
	ARG_INT("height", -1)
END_ACTION_ARG_LIST()




//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionShowControlID implementation
CActionShowControlID::CActionShowControlID()
{
}

CActionShowControlID::~CActionShowControlID()
{
}

bool CActionShowControlID::Invoke()
{
	if( !InitList() )
		return false;
	if( m_form == 0 )
	{
		AfxMessageBox( IDS_SELECT_FORM );
		return false;
	}


	CShowIDDlg	dlg( this );
	dlg.DoModal();

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionincMoveSize implementation
bool CActionincMoveSize::DoChangeLayout()
{
	int	dx = GetArgumentInt( "dx" );
	int	dy = GetArgumentInt( "dy" );
	int	dcx = GetArgumentInt( "dcx" );
	int	dcy = GetArgumentInt( "dcy" );

	m_bNoResize = (dcx==0) && (dcy==0);

	int	nMinOffset = 1;

	for( int i = 0; i < GetControlsCount(); i++ )
	{
		CRect	rect = m_prectSource[i];

		if( rect.left+dx >= nMinOffset )
		{
			rect.left += dx;
			rect.right += dx;
		}
		else
		{
			rect.right = rect.Width()+nMinOffset;
			rect.left = nMinOffset;
		}

		if( rect.top+dy >= nMinOffset )
		{
			rect.top += dy;
			rect.bottom += dy;
		}
		else
		{
			rect.top = nMinOffset;
			rect.bottom = rect.Height()+nMinOffset;
		}

		rect.right = max( rect.right+dcx, rect.left + nMinOffset );
		rect.bottom = max( rect.bottom+dcy, rect.top + nMinOffset );

		m_prectTarget[i] = rect;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionAddEventHandlers implementation
CActionAddEventHandlers::CActionAddEventHandlers()
{
}

CActionAddEventHandlers::~CActionAddEventHandlers()
{
	m_arrEventParams.RemoveAll();
}

bool CActionAddEventHandlers::Invoke()
{
	if( !InitList() )
		return false;

	IUnknown	*punkAX = 0;
	HRESULT hr = 0;
	sptrIActiveXCtrl	sptrControl;
	sptrILayoutView	sptrV;
	ITypeInfo* pITypeInfoAll = 0;
	CString strObjName;

	int	nControlsCount = GetControlsCount();

	if(nControlsCount != 0)
	{
		//get activex unknown
		sptrIDocumentSite	sptrDocSite( m_punkTarget );
		IUnknown	*punkView = 0;
		sptrDocSite->GetActiveView( &punkView );
		if( !punkView )
			return false;
		sptrV = punkView;
		punkView->Release();
		POSITION	pos = GetFirstPosition();
		IUnknown	*punkCtrl = GetNext( pos );
		sptrControl = punkCtrl;
		//punkCtrl->Release();
		sptrV->GetControlObject( sptrControl, &punkAX );
		BSTR	bstrName = 0;
		sptrControl->GetName(&bstrName);
		strObjName = bstrName;
		::SysFreeString(bstrName);

		if( strObjName.IsEmpty() )
			AfxMessageBox( IDS_PROPERTY_UNNAMED );
	}
	else
	{
		IProvideClassInfoPtr sptrPCI( _T("axform.ActiveXSite") );
		sptrPCI->GetClassInfo(&pITypeInfoAll);     

		/*if( sptrControl != 0 )
		{
			BSTR	bstrObjectName;
			sptrControl->GetName( &bstrObjectName );
			strObjName = bstrObjectName;
			::SysFreeString( bstrObjectName );

			
		} 
		else
		{ */
			strObjName = _T("Form");
		//}
	}

	if(punkAX)
	{
		//get events
		LPPROVIDECLASSINFO  pIProvideClassInfo = 0;
		hr=punkAX->QueryInterface(IID_IProvideClassInfo, (void**)&pIProvideClassInfo); 
		punkAX->Release();
		if (FAILED(hr))         
			return false;      
		hr = pIProvideClassInfo->GetClassInfo(&pITypeInfoAll);     
		pIProvideClassInfo->Release();
	}
		/*BSTR bstrObjName = 0;
		pITypeInfoAll->GetDocumentation(MEMBERID_NIL, &bstrObjName, NULL, NULL, NULL);
		CString strObjName(bstrObjName);
		if(bstrObjName) ::SysFreeString(bstrObjName);
		*/
	if(pITypeInfoAll)
	{
		if(strObjName.IsEmpty())
		{
			pITypeInfoAll->Release();
			return false;     
		}
		LPTYPEINFO          pITypeInfoEvent = 0;     
		LPTYPEATTR          pTA;      
		if (SUCCEEDED(pITypeInfoAll->GetTypeAttr(&pTA)))  
		{         
			UINT        i;         
			int         iFlags; 
			for (i=0; i < pTA->cImplTypes; i++)             
			{             
				//Get the implementation type for this interface             
				hr=pITypeInfoAll->GetImplTypeFlags(i, &iFlags);              
				if (FAILED(hr))                 
					continue;              
				if ((iFlags & IMPLTYPEFLAG_FDEFAULT) && (iFlags & IMPLTYPEFLAG_FSOURCE)) 
				{                 
					HREFTYPE    hRefType=NULL;                  
					pITypeInfoAll->GetRefTypeOfImplType(i, &hRefType);                 
					hr=pITypeInfoAll->GetRefTypeInfo(hRefType, &pITypeInfoEvent);     
					break;                
				}          
			}         
			pITypeInfoAll->ReleaseTypeAttr(pTA);         
		}

		pITypeInfoAll->Release();   
			
		if(pITypeInfoEvent)
		{
			TYPEATTR* pTypeAttributes = NULL;
			pITypeInfoEvent->GetTypeAttr(&pTypeAttributes);
			CEventDlg dlg;
			BSTR bstrArr[10];
			UINT nParamsWeGet = 0;
			for(int iIter = 0; iIter < pTypeAttributes->cFuncs; iIter++)
			{
				FUNCDESC* pFuncDesc = NULL;
				//Get the function description
				pITypeInfoEvent->GetFuncDesc(iIter, &pFuncDesc) ;
				//Get the name of the method	
				BSTR	bstrMethod = 0;
				pITypeInfoEvent->GetDocumentation(pFuncDesc->memid, &bstrMethod, NULL, NULL, NULL);
				CString str(bstrMethod);
				if(bstrMethod) ::SysFreeString(bstrMethod);
				pITypeInfoEvent->GetNames(pFuncDesc->memid, bstrArr, 10, &nParamsWeGet);
				CString strParams = "";
				for(int i = 1; i < (int)nParamsWeGet; i++)
				{
					strParams += bstrArr[i];
					if(i+1 < (int)nParamsWeGet)
						strParams += ", ";
				}
				m_arrEventParams.Add(strParams);
				//Release our function description stuff
				pITypeInfoEvent->ReleaseFuncDesc(pFuncDesc);
				CString strEventCheck = strObjName + "_";
				strEventCheck += str;
				str.Insert(0, _CheckIsEventPresent(strEventCheck) ? "Y" : "N");

				dlg.m_arrEvents.Add(str);
			}
			pITypeInfoEvent->ReleaseTypeAttr(pTypeAttributes);
			pITypeInfoEvent->Release();
			CString strTitle;
			strTitle.LoadString(IDS_EVENTDLG_TITLE);
			strTitle += strObjName;
			dlg.SetDlgTitle(strTitle);
				
			if(dlg.DoModal() != IDCANCEL)
			{
				bool bEnableKillEvent = false;
				if(dlg.m_bAnyKilled)
				{
					CString strTitle;
					CString strMsg;
					strTitle.LoadString(IDS_EVENTKILL_WARNIG_TITLE);
					strMsg.LoadString(IDS_EVENTKILL_WARNIG);
					bEnableKillEvent = ::MessageBox(NULL, strMsg, strTitle, MB_YESNO|MB_ICONWARNING|MB_APPLMODAL) == IDYES; 
				}
				IUnknown *punkScript = 0;
				m_form->GetScript(&punkScript);
				sptrIScriptDataObject sptrScript(punkScript);
				if(punkScript)
					punkScript->Release();
				long nLineCount = 0;
				//sptrScript->GetLinesCount(&nLineCount);
				int nEventsCount = dlg.m_arrEvents.GetSize();
				for(int i = 0; i < nEventsCount; i++)
				{
					if(dlg.m_arrEvents[i] != "")
					{
						CString strCurEvent = dlg.m_arrEvents[i];
						strCurEvent.Delete(0);
						if(dlg.m_arrEvents[i][0] == 'A')
						{
							//add event handler
							nLineCount = _GetLineNumber("\'"+strObjName + " event handlers.");
							CString strScriptLine;
							if(nLineCount == 0)
							{
								//it's first event
								sptrScript->GetLinesCount(&nLineCount);
								
								strScriptLine = "\'***************************************************************";
								sptrScript->InsertString(_bstr_t(strScriptLine), ++nLineCount);	
								strScriptLine = "\'" + strObjName;
								strScriptLine += " event handlers.";
								sptrScript->InsertString(_bstr_t(strScriptLine), ++nLineCount);	
								strScriptLine = "\'***************************************************************";
								sptrScript->InsertString(_bstr_t(strScriptLine), ++nLineCount);	
							}
							else
								nLineCount++;
							strScriptLine = "sub " + strObjName;
							strScriptLine += "_";
							strScriptLine += strCurEvent;
							strScriptLine += "(";
							strScriptLine += m_arrEventParams[i];
							strScriptLine += ")";
							sptrScript->InsertString(_bstr_t(strScriptLine), ++nLineCount);
							strScriptLine = "\t\'Place here your event handler.";
							sptrScript->InsertString(_bstr_t(strScriptLine), ++nLineCount);
							strScriptLine = "end sub";
							sptrScript->InsertString(_bstr_t(strScriptLine), ++nLineCount);
						}
						else if(dlg.m_arrEvents[i][0] == 'K' && bEnableKillEvent)
						{
							//kill event handler
							CString strCurEvent = dlg.m_arrEvents[i];
							strCurEvent.Delete(0);
							CString strEventToDel = strObjName + "_";
							strEventToDel += strCurEvent;
							_DeleteSub(strEventToDel);
						}
					}
				}
				if(dlg.m_bEditCode)
				{
					//clean-up
					sptrV = 0;
					sptrControl = 0;

					long nLine = _GetLineNumber("\'"+strObjName + " event handlers.");
					nLine += 2;

					//find view
					IDocumentSitePtr	ptrDoc( m_punkTarget );
					
					IViewPtr	ptrScriptView;

					long	lpos = 0;
					ptrDoc->GetFirstViewPosition( &lpos );
					while( lpos )
					{
						IUnknown *punkV = 0;
						ptrDoc->GetNextView( &punkV, &lpos );

						//only script view provides debug ide
						if( CheckInterface( punkV, IID_IProvideDebuggerIDE ) )
						{
							ptrScriptView = punkV;
							//ptrDoc->SetActiveView( punkV );

							punkV->Release();
							break;
						}
						punkV->Release();
					}


					if( ptrScriptView == 0 )
					{
						::ExecuteAction("ShowScript");
						
						IUnknown	*punkView = 0;
						ptrDoc->GetActiveView( &punkView );

						if( !punkView )
							return true;

						ptrScriptView = punkView;
						punkView->Release();
					}

					IDataContextPtr		sptrC( ptrScriptView );

					_bstr_t	bstrType = ::GetObjectKind( m_form );
					sptrC->SetActiveObject( bstrType, m_form, aofActivateDepended );

					::FireEvent(GetAppUnknown(), szEventSetScriptActiveLine, sptrScript, 0, &nLine );
				}
			}
		}			
	}
	
	return true;
}

bool CActionAddEventHandlers::_CheckIsEventPresent(CString str)
{
	IUnknown *punkScript = 0;
	m_form->GetScript(&punkScript);
	sptrIScriptDataObject sptrScript(punkScript);
	if(punkScript)
		punkScript->Release();
	BSTR bstrText = 0;
	sptrScript->GetText(&bstrText);
	CString strText(bstrText);
	::SysFreeString(bstrText);
	if(strText.Find(str) == -1)
		return false;
	return true;
}

long CActionAddEventHandlers::_GetLineNumber(CString strText)
{
	//найти номер строки, содержащую указанную подстроку
	IUnknown *punkScript = 0;
	m_form->GetScript(&punkScript);
	sptrIScriptDataObject sptrScript(punkScript);
	if(punkScript)
		punkScript->Release();
	long nLines = 0;
	sptrScript->GetLinesCount(&nLines);
	for(long i = 0; i < nLines; i++)
	{
		BSTR bstrLine = 0;
		sptrScript->GetLine( i, &bstrLine );
		CString strLine(bstrLine);
		::SysFreeString(bstrLine);
		//if(strLine.Find(strText) != -1)
		strLine = strLine.Left(strText.GetLength());
		if(strLine == strText)
			return i;
	}
	return 0;
}

void CActionAddEventHandlers::_DeleteSub(CString strText)
{
	IUnknown *punkScript = 0;
	m_form->GetScript(&punkScript);
	sptrIScriptDataObject sptrScript(punkScript);
	if(punkScript)
		punkScript->Release();

	long nStartLine = _GetLineNumber(strText);
	BSTR bstrLine = 0;
	sptrScript->GetLine( nStartLine, &bstrLine );
	CString strLine(bstrLine);
	::SysFreeString(bstrLine);
	if(strLine.Find("Sub") == -1)
		return;

	long nCurIdx = nStartLine;
	long nTotalLines = 0;
	sptrScript->GetLinesCount(&nTotalLines);
	sptrScript->DeleteString( nCurIdx );
	while(true)
	{
		BSTR bstrLine = 0;
		sptrScript->GetLine( nCurIdx, &bstrLine );
		CString strLine(bstrLine);
		::SysFreeString(bstrLine);
		if(strLine.Find("Sub") == -1)
			sptrScript->DeleteString( nCurIdx );
		else
		{
			sptrScript->DeleteString( nCurIdx );
			break;
		}
		if(nCurIdx > nTotalLines)
			break;
	}
}

//extended UI
bool CActionAddEventHandlers::IsAvaible()
{

	sptrIDocumentSite	sptrDocSite( m_punkTarget );
	IUnknown	*punkV = 0;
	sptrDocSite->GetActiveView( &punkV );

	if( !punkV )
		return false;

	sptrIView	sptrV = punkV;
	punkV->Release();

	if( !CheckInterface( sptrV, IID_ILayoutView ) )
		return false;

	if( CheckInterface( punkV, IID_IReportView ) )
		return false;


	sptrIBlankView spBlankView( punkV );
	if( spBlankView )
	{
		short nMode = -1;
		spBlankView->GetMode( &nMode );
		if( (BlankViewMode)nMode != bvmDesign )
			return false;
	}


	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionAxSetSize implementation

bool CActionAxSetSize::DoChangeLayout()
{
	CRect	rect( GetArgumentInt( "left" ),	GetArgumentInt( "top" ),
		GetArgumentInt( "right" ),	GetArgumentInt( "bottom" ) );

	for( int i = 0; i < GetControlsCount(); i++ )
		m_prectTarget[i] = m_prectSource[i];

	if( i > 0 )
		m_prectTarget[0] = rect;

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionAxMakeSameSize implementation
bool CActionAxMakeSameSize::DoChangeLayout()
{
	CSize	size = CSize( 0, 0 );

	if( GetControlsCount() )
	{
		size.cx = m_prectSource[0].Width();
		size.cy = m_prectSource[0].Height();
	}

	for( int i = 0; i < GetControlsCount(); i++ )
	{
		m_prectTarget[i].right = m_prectTarget[i].left+size.cx;
		m_prectTarget[i].bottom = m_prectTarget[i].top+size.cy;
	}

	return true;
}

bool CActionAxMakeSameHeight::DoChangeLayout()
{
	CSize	size = CSize( 0, 0 );

	if( GetControlsCount() )
	{
		size.cx = m_prectSource[0].Width();
		size.cy = m_prectSource[0].Height();
	}

	for( int i = 0; i < GetControlsCount(); i++ )
	{
		m_prectTarget[i].bottom = m_prectTarget[i].top+size.cy;
	}

	return true;
}

bool CActionAxMakeSameWidth::DoChangeLayout()
{
	CSize	size = CSize( 0, 0 );

	if( GetControlsCount() )
	{
		size.cx = m_prectSource[0].Width();
		size.cy = m_prectSource[0].Height();
	}

	for( int i = 0; i < GetControlsCount(); i++ )
	{
		m_prectTarget[i].right = m_prectTarget[i].left+size.cx;
	}

	return true;
}

bool CActionAxSpaceDown::DoChangeLayout()
{
	m_bNoResize = true;

	struct RectEntry
	{
		CRect	rect;
		int		pos;
	};

	int	nCount = GetControlsCount();

	ASSERT(nCount>1);
	int	i, j;

	RectEntry	*pe = new RectEntry[nCount];

	for( i = 0; i < nCount; i++ )
	{
		pe[i].rect = m_prectSource[i];
		pe[i].pos = i;
	}
	//sort vertically
	int	nYPos;
	int	nMinYPos;

	for( i = 0; i < nCount-1; i++ )
	{
		nMinYPos = i;
		nYPos = pe[i].rect.CenterPoint().y;

		for( j = i+1; j < nCount; j++ )
		{
			int	nCurPos = pe[j].rect.CenterPoint().y;
			if( nCurPos < nYPos )
			{
				nYPos = nCurPos;
				nMinYPos = j;
			}
		}

		if( nMinYPos != i )
		{
			CRect	rectTemp = pe[i].rect;
			int		posTemp = pe[i].pos;

			pe[i].rect = pe[nMinYPos].rect;
			pe[i].pos = pe[nMinYPos].pos;

			pe[nMinYPos].rect = rectTemp;
			pe[nMinYPos].pos = posTemp;
		}
	}

	int	nInterval = 0;
	for( i = 0; i < nCount-1; i++ )
		nInterval  += pe[i+1].rect.top - pe[i].rect.bottom;

	nInterval/= (nCount-1);

	int	nStartPos = pe[0].rect.bottom;

	for( i = 1; i < nCount; i++ )
	{
		pe[i].rect.bottom  = nStartPos+nInterval+pe[i].rect.Height();
		pe[i].rect.top  = nStartPos+nInterval;
		nStartPos = pe[i].rect.bottom;
	}

	for( i = 0; i < nCount; i++ )
		m_prectTarget[pe[i].pos] = pe[i].rect;

	delete pe;

	return true;
}

bool CActionAxSpaceAcross::DoChangeLayout()
{
	m_bNoResize = true;

	struct RectEntry
	{
		CRect	rect;
		int		pos;
	};

	int	nCount = GetControlsCount();

	ASSERT(nCount>1);
	int	i, j;

	RectEntry	*pe = new RectEntry[nCount];

	for( i = 0; i < nCount; i++ )
	{
		pe[i].rect = m_prectSource[i];
		pe[i].pos = i;
	}
	//sort vertically
	int	nXPos;
	int	nMinXPos;

	for( i = 0; i < nCount-1; i++ )
	{
		nMinXPos = i;
		nXPos = pe[i].rect.CenterPoint().x;

		for( j = i+1; j < nCount; j++ )
		{
			int	nCurPos = pe[j].rect.CenterPoint().x;
			if( nCurPos < nXPos )
			{
				nXPos = nCurPos;
				nMinXPos = j;
			}
		}

		if( nMinXPos != i )
		{
			CRect	rectTemp = pe[i].rect;
			int		posTemp = pe[i].pos;

			pe[i].rect = pe[nMinXPos].rect;
			pe[i].pos = pe[nMinXPos].pos;

			pe[nMinXPos].rect = rectTemp;
			pe[nMinXPos].pos = posTemp;
		}
	}

	int	nInterval = 0;
	for( i = 0; i < nCount-1; i++ )
		nInterval  += pe[i+1].rect.left - pe[i].rect.right;

	nInterval/= (nCount-1);

	int	nStartPos = pe[0].rect.right;

	for( i = 1; i < nCount; i++ )
	{
		pe[i].rect.right  = nStartPos+nInterval+pe[i].rect.Width();
		pe[i].rect.left  = nStartPos+nInterval;
		nStartPos = pe[i].rect.right;
	}

	for( i = 0; i < nCount; i++ )
		m_prectTarget[pe[i].pos] = pe[i].rect;

	delete pe;

	return true;
}

bool CActionAxAlignBottom::DoChangeLayout()
{
	m_bNoResize = true;

	int	nPos = m_prectSource[0].bottom;

	for( int i = 0; i < GetControlsCount(); i++ )
	{
		m_prectTarget[i].top = nPos-m_prectTarget[i].Height();
		m_prectTarget[i].bottom = nPos;
	}
	return true;
}

bool CActionAxAlignTop::DoChangeLayout()
{
	m_bNoResize = true;

	int	nPos = m_prectSource[0].top;

	for( int i = 0; i < GetControlsCount(); i++ )
	{
		m_prectTarget[i].bottom = nPos+m_prectTarget[i].Height();
		m_prectTarget[i].top = nPos;
	}
	return true;
}

bool CActionAxAlignLeft::DoChangeLayout()
{
	m_bNoResize = true;

	int	nPos = m_prectSource[0].left;

	for( int i = 0; i < GetControlsCount(); i++ )
	{
		m_prectTarget[i].right = nPos+m_prectTarget[i].Width();
		m_prectTarget[i].left = nPos;
	}
	return true;
}

bool CActionAxAlignRight::DoChangeLayout()
{
	m_bNoResize = true;

	int	nPos = m_prectSource[0].right;

	for( int i = 0; i < GetControlsCount(); i++ )
	{
		m_prectTarget[i].left = nPos-m_prectTarget[i].Width();
		m_prectTarget[i].right = nPos;
	}
	return true;
}



//////////////////////////////////////////////////////////////////////
//CActionAxProperties implementation
CActionAxProperties::CActionAxProperties()
{
}

CActionAxProperties::~CActionAxProperties()
{
}

bool CActionAxProperties::Invoke()
{

	if( !InitList() )
		return false;


	sptrIDocumentSite	sptrDocSite( m_punkTarget );
	IUnknown	*punkView = 0;
	sptrDocSite->GetActiveView( &punkView );

	if( !punkView )
		return false;

	sptrILayoutView	sptrV( punkView );
	punkView->Release();


	POSITION	pos = GetFirstPosition();

	IUnknown	*punkCtrl = GetNext( pos );	  //without AddRef
	sptrIActiveXCtrl	sptrControl( punkCtrl );
	sptrIReportCtrl		sptrReportControl( punkCtrl );


	BSTR	bstrControlName;
	sptrControl->GetName( &bstrControlName );
	CString	strControlName = bstrControlName;
	::SysFreeString( bstrControlName );
//	punkCtrl->Release();


	BOOL bReportCtrl = FALSE;
	
	bool bDBBlankView = false;

	if( CheckInterface( sptrV, IID_IBlankView ))
		bDBBlankView = true;



	if( CheckInterface( punkCtrl, IID_IReportCtrl ))
		bReportCtrl = TRUE;
	
	IUnknown	*punkAX = 0;
	sptrV->GetControlObject( sptrControl, &punkAX );

	if( !punkAX )
		return false;

	//execute active x properties dialog
	IDispatch* pIDispatch = NULL;			

	ISpecifyPropertyPages  *pISPP;
	CAUUID		caGUID;
	HRESULT		hr;
	LCID		m_lcid = GetUserDefaultLCID(); 
	
	caGUID.cElems = 0;


	if (FAILED(punkAX->QueryInterface( IID_IDispatch, (void **)&pIDispatch )))
	{
		AfxMessageBox("Object has no dispatch interface.");
		return false;
	}
		
	if( pIDispatch->QueryInterface( IID_ISpecifyPropertyPages, (void **)&pISPP ) == S_OK )
	{
		hr = pISPP->GetPages( &caGUID );
		pISPP->Release();

	/*	AfxMessageBox("Object has no property pages.");
		if(pIDispatch) pIDispatch->Release( );
		return false;**/
	}
			
	

	GUID	clsidAllPP;
	::CLSIDFromProgID(_L("PropertyBrowser.AllPropPage"), &clsidAllPP);
	GUID	clsidWindowPP;
	GUID	clsidReportPP;

	::CLSIDFromProgID(_L("axeditor.WindowPage"), &clsidWindowPP);
	::CLSIDFromProgID(_L("axeditor.ReportPage"), &clsidReportPP);

	CAUUID	caGUID2;
	int		nNumPages = caGUID.cElems;
	caGUID2.pElems = (GUID*)CoTaskMemAlloc(sizeof(GUID)*(nNumPages+2));
	caGUID2.cElems = nNumPages + ( bDBBlankView ? 1 : 2 );


	if( !bDBBlankView )
	{
		if( !bReportCtrl )
			caGUID2.pElems[nNumPages] = clsidWindowPP;
		else
			caGUID2.pElems[nNumPages] = clsidReportPP;
	}
		

	if( nNumPages )
		memcpy((LPVOID)&caGUID2.pElems[0], (LPVOID)caGUID.pElems, sizeof(GUID)*nNumPages);
	caGUID2.pElems[nNumPages+1] = clsidAllPP;

						  
	IUnknown	*ppunk[2];
	ppunk[0] = pIDispatch;
	if( !bReportCtrl )
		ppunk[1] = sptrControl;
	else
		ppunk[1] = sptrReportControl;


	CWnd	*pwnd = ::GetWindowFromUnknown( sptrV );

	
	//paul.Target: Set control name to property frame	
	_bstr_t bstrCtrlName;
	bstrCtrlName = "Unrecognized ";
	if( sptrControl )
	{

		BSTR bstrProgID;
		sptrControl->GetProgID( &bstrProgID );
		CLSID clsid;
		
		if( SUCCEEDED( ::CLSIDFromProgID( bstrProgID, &clsid ) ) ) 
		{
			BSTR bstrCLSID;
			if( SUCCEEDED( ::StringFromCLSID(clsid, &bstrCLSID)) )
			{
				CString strCLSID = bstrCLSID;
				HKEY hKey = NULL;
				LONG	 regResult = 0;
				regResult = ::RegOpenKey(HKEY_CLASSES_ROOT, CString("CLSID\\")+strCLSID, &hKey);
				if (regResult == ERROR_SUCCESS)
				{
					char szBuffer[MAX_PATH*2] ;
					LONG lSize = sizeof(szBuffer) ; 
					regResult = ::RegQueryValue(hKey, NULL, szBuffer, &lSize) ;
					if (regResult == ERROR_SUCCESS)
					{
						bstrCtrlName = CString(szBuffer, (int)lSize) ;
					}
				}

			}
			
			CoTaskMemFree( bstrCLSID );
			
		}
		
		::SysFreeString( bstrProgID );
		
	}
	//eofCode paul


	{//Store AXProperties
			
		m_ptrControlData =  punkCtrl;

		VERIFY( SerializeControl( true/*Undo buffer*/, false/*Load*/, false /*Update parent*/ ) );
	}
	

	hr = OleCreatePropertyFrame( *pwnd, 10, 10, bstrCtrlName, 
			1, ppunk, caGUID2.cElems,
			caGUID2.pElems, m_lcid, 0L, NULL);

	if (FAILED(hr))
		AfxMessageBox("OleCreatePropertyFrame failed.");

		//Free GUIDs.
	if(caGUID.cElems > 0)
		CoTaskMemFree((void *)caGUID.pElems);
	if(caGUID2.cElems > 0)
		CoTaskMemFree((void *)caGUID2.pElems);
	if(pIDispatch) pIDispatch->Release( );

	::StoreContainerToDataObject( /*sptrControl*/punkCtrl, punkAX ); //@@@@@@@@@@@@@@@@@@@@@@@@
	punkAX->Release();					

	IUnknown	*punkParent = 0;
	INamedDataObject2Ptr	ptrO( /*sptrControl*/punkCtrl ); //@@@@@@@@@@@@@@@@@@@@@@@@
	ptrO->GetParent( &punkParent );
	ASSERT( punkParent );

	IDataObjectListPtr	ptrList( punkParent );
	punkParent->Release();

	ASSERT( ptrList != 0 );

	ptrList->UpdateObject( /*sptrControl*/punkCtrl ); //@@@@@@@@@@@@@@@@@@@@@@@@
	// не ресайзили ни один объект - запретим лишние телодвижения
//	HANDLE h = CreateMutex( 0, false, "ResizeView.lock.1C6E8B14-678E-4553-A16E-9705EA42F170" );
//	::UpdateDataObject( m_punkTarget, m_form );
//	if(h) CloseHandle(h);
	if (bDBBlankView)
	{
		BOOL bSucceeded;
		IViewCtrlPtr sptrVC(punkAX);
		if (sptrVC)
			sptrVC->Build(&bSucceeded);
	}

	SerializeControl( false/*Undo buffer*/, false/*Load*/, false /*Update parent*/ );
	
	//m_changes.SetToDocData( sptrDoc, punk );

	return IsControlChanged()?true:false;
}

bool CActionAxProperties::SerializeControl( bool bUseUndoBuffer, bool bLoad, bool bUpdateParent )
{
	
	if( m_ptrControlData == NULL )
		return false;


	sptrIDocumentSite	sptrDocSite( m_punkTarget );
	IUnknown	*punkView = 0;
	sptrDocSite->GetActiveView( &punkView );

	if( !punkView )
		return false;

	sptrILayoutView	sptrV( punkView );
	punkView->Release();

	IUnknownPtr ptrAXCtrl;
	if( sptrV )
	{
		IUnknown* punkAXCtrl = NULL;
		sptrV->GetControlObject( m_ptrControlData, &punkAXCtrl );
		if( !punkAXCtrl )
			return false;
		
		ptrAXCtrl = punkAXCtrl;

		punkAXCtrl->Release();
	}



	CMemFile* pFile = ( bUseUndoBuffer ? &m_fileUndo : &m_fileRedo );
	

	try{

		pFile->SeekToBegin();

		CArchive ar( pFile, ( bLoad ? CArchive::load : CArchive::store ) );
		CArchiveStream	stream( &ar );

		IPersistStreamInitPtr	spSIP( ptrAXCtrl );

		if( spSIP )
		{
			if( bLoad )			
				spSIP->Load( &stream );			
			else
				spSIP->Save( &stream, true );
		}

		ar.Flush();
		pFile->Flush();

		if( bLoad )
		{
			::StoreContainerToDataObject( m_ptrControlData, ptrAXCtrl );
		}

	}
	catch(...)
	{
		return false;
	}


	//
	if( bUpdateParent )
	{
		IUnknown	*punkParent = 0;
		INamedDataObject2Ptr	ptrO( m_ptrControlData );
		ptrO->GetParent( &punkParent );
		ASSERT( punkParent );

		IDataObjectListPtr	ptrList( punkParent );
		punkParent->Release();

		ASSERT( ptrList != 0 );

		ptrList->UpdateObject( m_ptrControlData ); 
	}


	return true;
}

bool CActionAxProperties::IsControlChanged()
{
	m_fileUndo.SeekToBegin();
	m_fileRedo.SeekToBegin();
	void *pUndoStart, *pUndoMax, *pRedoStart, *pRedoMax;
	UINT nUndo = m_fileUndo.GetBufferPtr(CFile::bufferRead, -1, &pUndoStart, &pUndoMax);
	UINT nRedo = m_fileRedo.GetBufferPtr(CFile::bufferRead, -1, &pRedoStart, &pRedoMax);
	if (nUndo != nRedo)
		return true;
	int r = memcmp(pUndoStart, pRedoStart, nUndo);
	return r != 0;
}

//undo/redo
bool CActionAxProperties::DoUndo()
{
	VERIFY( SerializeControl( true/*Undo buffer*/, true/*Load*/, true /*Update parent*/ ) );
	return true;
}

bool CActionAxProperties::DoRedo()
{
	VERIFY( SerializeControl( false/*Undo buffer*/, true/*Load*/, true /*Update parent*/ ) );
	return true;
}

bool CActionAxProperties::IsAvaible()
{
	if( !InitList() )
		return false;
	return GetControlsCount() > 0;
}

//////////////////////////////////////////////////////////////////////
//CActionAxResize implementation
CActionAxResize::CActionAxResize()
{
	m_sizeOld = CSize( 0, 0 );
	m_sizeNew = CSize( 0, 0 );
}

CActionAxResize::~CActionAxResize()
{
}

/*
void CActionAxResize::Initialize()
{
	CInteractiveActionBase::Initialize();

	
	BOOL bFindReportView = FALSE;
	sptrIView	sptrV( m_punkTarget );

	if( CheckInterface( sptrV, IID_IReportView ) )
		bFindReportView = TRUE;


	IUnknown	*punk = ::GetActiveObjectFromContext( m_punkTarget, (!bFindReportView ? szArgumenAXForm : szArgumenReportForm) );
	m_form  = punk;

	if( !punk )		return;

	punk->Release();

	m_form->GetSize( &m_sizeOld );

	m_tracker.m_nStyle = CRectTracker::resizeOutside|CRectTracker::hatchedBorder;
	m_tracker.m_nHandleSize = 6;
	m_tracker.m_rect.left = 10;
	m_tracker.m_rect.top = 10;
	m_tracker.m_rect.right = 10+m_sizeOld.cx;
	m_tracker.m_rect.bottom = 10+m_sizeOld.cy;

	CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );
	pwnd->Invalidate();
}


bool CActionAxResize::DoSetCursor( CPoint point )
{
	int	nHitTest = m_tracker.HitTest( point );
	
	if( m_tracker.HitTest( point ) == CRectTracker::hitNothing ||
		m_tracker.HitTest( point ) == CRectTracker::hitMiddle )
		return false;

	CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );
	m_tracker.SetCursor( pwnd, HTCLIENT );
	return true;
}

bool CActionAxResize::DoLButtonDown( CPoint point )
{
	
	if( m_tracker.HitTest( point ) == CRectTracker::hitNothing )
		return false;
		
	if( m_tracker.HitTest( point ) == CRectTracker::hitMiddle )
	{
		LeaveMode();
		return true;
	}

	CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );
	m_tracker.Track( pwnd, point );
	m_sizeNew = m_tracker.m_rect.Size();

	Finalize();

	return true;
}

void CActionAxResize::Finalize()
{
	CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );
	pwnd->Invalidate();

	CInteractiveActionBase::Finalize();
}
void CActionAxResize::DoDraw( CDC &dc )
{
	m_tracker.Draw( &dc );
}
*/
bool CActionAxResize::Invoke()
{
	if( GetArgumentInt( "width" )  == -1 || 
		GetArgumentInt( "height" ) == -1
		)
	{
		m_sizeNew = m_sizeOld;
	}
	else
	{
		m_sizeNew = CSize( GetArgumentInt( "width" ),	GetArgumentInt( "height" ) );
	}


	IUnknown	*punk = ::GetActiveObjectFromContext( m_punkTarget, szArgumenAXForm );	

	if( !punk )		
		return false;

	sptrIActiveXForm spForm( punk );
	punk->Release();

	if( spForm == NULL )
		return false;

	spForm->GetSize( &m_sizeOld );
	spForm->SetSize( m_sizeNew );

	IUnknown	*punkDoc = 0;
	sptrIView	sptrV( m_punkTarget );
	sptrV->GetDocument( &punkDoc );

	//::UpdateDataObject( punkDoc, m_form );
	if (punkDoc)
		punkDoc->Release();
	

	CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );
	if( pwnd )
		pwnd->Invalidate();

	//m_lTargetKey = GetObjectKey( punkDoc );

	return true;
}

//undo/redo
bool CActionAxResize::DoUndo()
{
	IUnknown	*punk = ::GetActiveObjectFromContext( m_punkTarget, szArgumenAXForm );	

	if( !punk )		
		return false;

	sptrIActiveXForm spForm( punk );
	punk->Release();

	if( spForm == NULL )
		return false;
	
	spForm->SetSize( m_sizeOld );

	IUnknown	*punkDoc = 0;
	sptrIView	sptrV( m_punkTarget );
	sptrV->GetDocument( &punkDoc );

	if (punkDoc)
		punkDoc->Release();

	CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );
	if( pwnd )
		pwnd->Invalidate();

	return true;
}

bool CActionAxResize::DoRedo()
{
	//m_form->SetSize( m_sizeOld );
	//::UpdateDataObject( m_punkTarget, m_form );


	IUnknown	*punk = ::GetActiveObjectFromContext( m_punkTarget, szArgumenAXForm );	

	if( !punk )		
		return false;

	sptrIActiveXForm spForm( punk );
	punk->Release();

	if( spForm == NULL )
		return false;
	
	spForm->SetSize( m_sizeNew );
	//If size smaller than controls max pos? correct it
	spForm->AutoUpdateSize();
	spForm->GetSize( &m_sizeNew );



	IUnknown	*punkDoc = 0;
	sptrIView	sptrV( m_punkTarget );
	sptrV->GetDocument( &punkDoc );

	if (punkDoc)
		punkDoc->Release();

	CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );
	if( pwnd )
		pwnd->Invalidate();

	return true;
}

//extended UI
bool CActionAxResize::IsAvaible()
{
	IUnknown	*punk = ::GetActiveObjectFromContext( m_punkTarget, szTypeAXForm );
	if( !punk )
		return false;
	punk->Release();
	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionAxInsertControl implementation
CActionAxInsertControl::CActionAxInsertControl()
{
	m_hcur_add = 0;
}

CActionAxInsertControl::~CActionAxInsertControl()
{
	if( m_hcur_add )
		::DestroyCursor( m_hcur_add );	m_hcur_add = 0;
}

bool CActionAxInsertControl::Initialize()
{
	if( !CInteractiveActionBase::Initialize() )return false;

	if( m_hcur_add )
		::DestroyCursor( m_hcur_add );	m_hcur_add = 0;
	
	m_hcur_add = AfxGetApp()->LoadCursor( IDC_CURSOR_ADD );

//	EnterInteractiveMode();
	DoUpdateSettings();

	IUnknown* punk = NULL;
	int nPageNum = 0;
	if( GetPropertyPageByProgID( szAXInsertPropPageProgID, &punk, &nPageNum ) )
	{
		IAXInsertPropPagePtr ptrAX( punk );
		punk->Release();	punk = 0;
		::FireEvent( ptrAX, "AXInsertControlInit", GetControllingUnknown(), NULL , NULL );		
	}
	else
	{
		ASSERT( FALSE );
	}
	return true;
}

void CActionAxInsertControl::LeaveMode()
{
	CInteractiveActionBase::LeaveMode();
}

void CActionAxInsertControl::Finalize()
{

	IUnknown* punk = NULL;
	int nPageNum = 0;
	if( GetPropertyPageByProgID( szAXInsertPropPageProgID, &punk, &nPageNum ) )
	{
		IAXInsertPropPagePtr ptrAX( punk );
		punk->Release();	punk = 0;

		::FireEvent( ptrAX, szEventAXInsertControlDone, GetControllingUnknown(), NULL , NULL );		
	}
	else
	{
		ASSERT( FALSE );
	}

	CInteractiveActionBase::Finalize(); 
	// Do not call CInteractiveActionBase::Finalize(); to avoid Exclude page;
	//LeaveInteractiveMode();
	//_DeInit();
	//m_state = asReady;


}

bool CActionAxInsertControl::DoStartTracking( CPoint point )
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	//Scroll support
	sptrIView	sptrV( m_punkTarget );
	point = ConvertToWindow( sptrV, point);

	m_tracker.m_nStyle = CRectTracker::resizeOutside|CRectTracker::hatchedBorder;
	m_tracker.m_nHandleSize = 6;
	m_tracker.m_rect.left = point.x;
	m_tracker.m_rect.top = point.y;

	CPoint	pointCurrect;
	GetCursorPos( &pointCurrect );	

	//Scroll support
	//pwnd->ScreenToClient( &pointCurrect );
	pointCurrect = ConvertToWindow( sptrV, pointCurrect);
	


	m_tracker.m_rect.right = pointCurrect.x;
	m_tracker.m_rect.bottom = pointCurrect.y;

	//reset tracking
	DoLeaveMode();
	m_bInTrackMode = false;
	m_bTestTrackMode = false;

	
	CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );
	m_tracker.Track( pwnd, pointCurrect, true );

	m_tracker.m_rect.NormalizeRect();

	if( m_tracker.m_rect.Width() > 10 ||
		m_tracker.m_rect.Height() > 10 )
		Finalize();

	return true;
}

bool CActionAxInsertControl::DoSetCursor( CPoint point )
{
	::SetCursor( m_hcur_add );
	return true;
}

/*bool CActionAxInsertControl::ExecuteSettings( CWnd *pwndParent )
{
	m_strProgID = GetArgumentString( "ProgID" );

	if( !m_strProgID.IsEmpty() )
		return true;

	CInsertControlDlg	dlg( pwndParent );

	if( dlg.DoModal() != IDOK )
		return false;



	BSTR	bstr = 0;

	if( ProgIDFromCLSID( dlg.m_clsid, &bstr ) )
		return false;

	m_strProgID = bstr;
	CoTaskMemFree( bstr );

	return true;
}*/

bool CActionAxInsertControl::Invoke()
{
	if( ::GetAsyncKeyState( VK_LBUTTON ) & 0x8000 )
		return false;

	m_strProgID = GetArgumentString( "ProgID" );

	if(m_strProgID.IsEmpty())
	{
		//try get from property page
		IUnknown *punkPP = GetPropertyPageByTitle(c_szCAXInsertPropPage, 0);
		sptrIAXInsertPropPage sptrPP(punkPP);

		if(sptrPP != 0)
		{
			BSTR	bstrProgID = 0;
			sptrPP->GetCurProgID(&bstrProgID);
			m_strProgID = bstrProgID;
			::SysFreeString(bstrProgID);
		}
		if(punkPP)
			punkPP->Release();
	}


	if( m_strProgID.IsEmpty() )
	{
		CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );
		CInsertControlDlg	dlg( pwnd );

		if( dlg.DoModal() != IDOK )
			return false;
		BSTR	bstr = 0;

		if( ProgIDFromCLSID( dlg.m_clsid, &bstr ) )
			return false;

		m_strProgID = bstr;
		CoTaskMemFree( bstr );
	}


	sptrIView	sptrV( m_punkTarget );
	IUnknown	*punkDoc = 0;
	sptrV->GetDocument( &punkDoc );

	IUnknownPtr	sptrDoc( punkDoc );
	m_lTargetKey = ::GetObjectKey( sptrDoc );

	if (punkDoc)
		punkDoc->Release();


	CString strControlType = szTypeAXCtrl;
	CString strFormType = szTypeAXForm;

	if( CheckInterface( sptrV, IID_IReportView ) )
	{
		strControlType = szTypeReportCtrl;
		strFormType = szTypeReportForm;
	}
	else
	if( CheckInterface( sptrV, IID_IBlankView ) )
	{
		strControlType = szTypeBlankCtrl;
		strFormType = szTypeBlankForm;
	}
		


	IUnknown	*punk = ::GetActiveObjectFromContext( m_punkTarget, strFormType );	

	if( !punk )
	{
		
		punk = ::CreateTypedObject( strFormType );

		if( !punk )
			return false;
	
		m_changes.SetToDocData( sptrDoc, punk );
	}


	sptrIActiveXForm	form = punk;
	punk->Release();

	
	punk = ::CreateTypedObject( strControlType );


	if( !punk ){
		return true;
	}

	BSTR	bstr = m_strProgID.AllocSysString();
	sptrIActiveXCtrl	sptrC( punk );
	punk->Release();

	if( sptrC == 0 )
		return false;

	sptrC->SetProgID( bstr );

	BOOL bUniqueName = FALSE;
	BSTR bstrName;
	sptrC->GetName( &bstrName );
	sptrC->IsUniqueName( bstrName, &bUniqueName );
	if( !bUniqueName )
	{
		BSTR bstrNewName;
		sptrC->GenerateUniqueName( bstrName , &bstrNewName );
		sptrC->SetName( bstrNewName );
		::SysFreeString( bstrNewName );
	}	
	::SysFreeString( bstrName );


	CPoint	pointOffset;
	{
		ILayoutViewPtr	ptrV( m_punkTarget );
		ptrV->GetFormOffset( &pointOffset );
	}
	//Scroll support
	//sptrIView	sptrV( m_punkTarget );
	CRect rcCtrl = m_tracker.m_rect;
	rcCtrl-= pointOffset;
	rcCtrl = ConvertToClient( sptrV, rcCtrl);
	sptrC->SetRect( rcCtrl );
	
	SetArgument( "ProgID", _variant_t( bstr ) );
	::SysFreeString( bstr );

	INotifyObjectPtr ptrNO( sptrC );
	if( ptrNO )
	{
		CComVariant varViews = GetArgument( "Views" );
		if(VT_EMPTY!=varViews.vt)
		{
			HRESULT hr=varViews.ChangeType(VT_I4);
			if(SUCCEEDED(hr))
			{
				if(varViews.lVal>0)
				{
					if(IViewSubTypePtr pViewST=ptrNO)
					{
						pViewST->SetViewSubType(varViews.lVal);
					}
				}
			}
		}
	}

	m_changes.SetToDocData( sptrDoc, sptrC );

	form->GetSize( &m_sizeOld );
	form->AutoUpdateSize();

	::UpdateDataObject( sptrDoc, form );

	if(INotifyObjectPtr ptrNO=sptrC)
	{
		if( ptrNO->NotifyCreate() != S_OK )
		{
			m_changes.RemoveFromDocData( sptrDoc, sptrC );
			form->AutoUpdateSize();
			::UpdateDataObject( sptrDoc, form );
		}
	}
	
	LeaveMode();

	/*

	IUnknown	*punkPropertySheet = ::GetPropertySheet();
	IAXInsertPropPagePtr ptrAXPropPage;


	if( punkPropertySheet )
	{
		sptrIPropertySheet	spPSheet( punkPropertySheet );
		punkPropertySheet->Release();

		int nPagesCount = 0;
		spPSheet->GetPagesCount( &nPagesCount );
		if( spPSheet )
		{
			for( long nPageNo = nPagesCount-1; nPageNo >-1; nPageNo-- )
			{
				if( ptrAXPropPage != NULL )
					continue;
				IUnknown *punkPage = NULL;
				spPSheet->GetPage( nPageNo, &punkPage );
				if( punkPage == NULL )
					continue;
				IUnknownPtr ptrPage = punkPage;
				punkPage->Release();

				if( !CheckInterface(ptrPage, IID_IAXInsertPropPage ) )
					continue;

				ptrAXPropPage = ptrPage;
			}		
		}
	}

	if( ptrAXPropPage )
		::FireEvent( ptrAXPropPage, szEventAXInsertControlDone, GetControllingUnknown(), NULL , NULL );
	*/

	return true;
}

//undo/redo
bool CActionAxInsertControl::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionAxInsertControl::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}

//extended UI
bool CActionAxInsertControl::IsAvaible()
{
	sptrIBlankView spBlankView( m_punkTarget );
	if( spBlankView )
	{
		short nMode = -1;
		spBlankView->GetMode( &nMode );
		if( (BlankViewMode)nMode != bvmDesign )
			return false;
	}

	return CheckInterface( m_punkTarget, IID_ILayoutView );
}


//////////////////////////////////////////////////////////////////////
//CActionShowAxLayout implementation
CString CActionShowAxLayout::GetViewProgID()
{
	return  "AxEditor.AxView";
}

