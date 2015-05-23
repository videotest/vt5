// ChromoActions.cpp : implementation file
//

#include "stdafx.h"
#include "ChromosEx.h"
#include "ChromoActions.h"
#include "chromo_utils.h"
#include "params.h"

#include "consts.h"
#include "chromosome.h"
#include "math.h"
#include "idio_int.h"

#include "\vt5\comps\misc\chromo\utils\chromo_misc.h"
#include "\vt5\common2\class_utils.h"

//#include "image5.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IUnknownPtr GetActiveViewFromDoc( IUnknown* punkDoc )
{
	IDocumentSitePtr ptrDS( punkDoc );
	if( ptrDS == 0 )
		return 0;

	IUnknown* punkView = 0;
	ptrDS->GetActiveView( &punkView );
	if( !punkView )
		return 0;

	IUnknownPtr ptrView = punkView;
	punkView->Release();	punkView = 0;

	return ptrView;
}

void SetModifyFlag( IUnknownPtr sptrUnk )
{
	IUnknown *punk = ::GetActiveObjectFromContext( sptrUnk, szTypeObject );

	if( punk )
	{
		IFileDataObject2Ptr ptrFDO2 = punk;
		punk->Release();
		if( ptrFDO2 )
			ptrFDO2->SetModifiedFlag( TRUE );
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//CActionShowCarioView
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionShowCarioView, CCmdTargetEx);

// {A10B5EBE-DCD5-4691-A6E0-E134AFB6C2ED}
GUARD_IMPLEMENT_OLECREATE( CActionShowCarioView, "ChromosEx.ShowKaryoView", 
0xa10b5ebe, 0xdcd5, 0x4691, 0xa6, 0xe0, 0xe1, 0x34, 0xaf, 0xb6, 0xc2, 0xed);

// {2D9C50E6-C1E5-45f4-8C3C-F4F9314DE77F}
static const GUID guidShowCarioView = 
{ 0x2d9c50e6, 0xc1e5, 0x45f4, { 0x8c, 0x3c, 0xf4, 0xf9, 0x31, 0x4d, 0xe7, 0x7f } };

ACTION_INFO_FULL(CActionShowCarioView, IDS_SHOW_CARIO_VIEW, -1, -1, guidShowCarioView);
ACTION_TARGET(CActionShowCarioView, szTargetFrame);


/////////////////////////////////////////////////////////////////////////////
bool CActionShowCarioView::Invoke()
{
	if( !CActionShowViewBase::Invoke() )
		return false;

	ICarioViewPtr ptrV( get_view() );
	if( ptrV )
	{
		ptrV->SetMode( get_view_mode() );			
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
IUnknownPtr CActionShowCarioView::get_view()
{
	IFrameWindowPtr ptrFrame( m_punkTarget );
	if( ptrFrame == 0 )
		return 0;

	IUnknown* punkView = 0;
	ptrFrame->GetActiveView( &punkView );

	if( !punkView )
		return 0;

	IUnknownPtr ptrView = punkView;
	punkView->Release();	punkView = 0;

	return ptrView;
}
/////////////////////////////////////////////////////////////////////////////
CString CActionShowCarioView::GetViewProgID()
{
	return szCarioViewProgID;	
}

/////////////////////////////////////////////////////////////////////////////
bool CActionShowCarioView::IsAvaible()
{
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionShowCarioView::IsChecked()
{
	ICarioViewPtr ptrV( get_view() );
	if( ptrV == 0 )
		return false;

	CarioViewMode cvm = cvmPreview;
	ptrV->GetMode( &cvm );

	return ( cvm == get_view_mode() );
}



/////////////////////////////////////////////////////////////////////////////
//
//CActionShowCarioViewDesign
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionShowCarioViewDesign, CCmdTargetEx);

// {8E89886F-F8E3-4146-83D4-C865B2B3A3FA}
GUARD_IMPLEMENT_OLECREATE(CActionShowCarioViewDesign, "ChromosEx.ShowKaryoViewDesign", 
0x8e89886f, 0xf8e3, 0x4146, 0x83, 0xd4, 0xc8, 0x65, 0xb2, 0xb3, 0xa3, 0xfa);

// {B2F3AB03-DF8D-468e-A455-6BC09702446B}
static const GUID guidShowCarioViewDesign = 
{ 0xb2f3ab03, 0xdf8d, 0x468e, { 0xa4, 0x55, 0x6b, 0xc0, 0x97, 0x2, 0x44, 0x6b } };

ACTION_INFO_FULL(CActionShowCarioViewDesign, IDS_SHOW_CARIO_VIEW_DESIGN, -1, -1, guidShowCarioViewDesign);
ACTION_TARGET(CActionShowCarioViewDesign, szTargetFrame);



/////////////////////////////////////////////////////////////////////////////
//
//CActionShowCarioAnalize
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionShowCarioAnalize, CCmdTargetEx);


// {7CF8891C-B3AE-42a1-AF42-2D4BD8ECA6E6}
GUARD_IMPLEMENT_OLECREATE(CActionShowCarioAnalize, "ChromosEx.ShowKaryoAnalize", 
0x7cf8891c, 0xb3ae, 0x42a1, 0xaf, 0x42, 0x2d, 0x4b, 0xd8, 0xec, 0xa6, 0xe6);

// {13D4DCD6-E594-44f5-9060-23EF394D1B92}
static const GUID guidShowCarioAnalize = 
{ 0x13d4dcd6, 0xe594, 0x44f5, { 0x90, 0x60, 0x23, 0xef, 0x39, 0x4d, 0x1b, 0x92 } };

ACTION_INFO_FULL(CActionShowCarioAnalize, IDS_SHOW_CARIO_ANLIZE, -1, -1, guidShowCarioAnalize);
ACTION_TARGET(CActionShowCarioAnalize, szTargetFrame);

/////////////////////////////////////////////////////////////////////////////
CString CActionShowCarioAnalize::GetViewProgID()
{
	return szCarioAnalizeViewProgID;	
}

/////////////////////////////////////////////////////////////////////////////
//add delete line & cell
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CActionInsertCarioLine, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAppendCarioLine, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionInsertCarioCell, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAppendCarioCell, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionDeleteCarioLine, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionDeleteCarioCell, CCmdTargetEx);

// {0590D7F5-8AA4-4f21-AAF6-41A48F6A3BF0}
GUARD_IMPLEMENT_OLECREATE(CActionInsertCarioLine, "ChromosEx.InsertKaryoLine", 
0x590d7f5, 0x8aa4, 0x4f21, 0xaa, 0xf6, 0x41, 0xa4, 0x8f, 0x6a, 0x3b, 0xf0);
// {22E36643-D52C-4336-BD53-9E31F49390EB}
GUARD_IMPLEMENT_OLECREATE(CActionAppendCarioLine, "ChromosEx.AppendKaryoLine", 
0x22e36643, 0xd52c, 0x4336, 0xbd, 0x53, 0x9e, 0x31, 0xf4, 0x93, 0x90, 0xeb);
// {E605D90F-ECDF-455c-A0B7-3E9572D2EADF}
GUARD_IMPLEMENT_OLECREATE(CActionInsertCarioCell, "ChromosEx.InsertKaryoCell", 
0xe605d90f, 0xecdf, 0x455c, 0xa0, 0xb7, 0x3e, 0x95, 0x72, 0xd2, 0xea, 0xdf);
// {0D8EA448-DFCB-450b-9E2C-12A120CCC71B}
GUARD_IMPLEMENT_OLECREATE(CActionAppendCarioCell, "ChromosEx.AppendKaryoCell", 
0xd8ea448, 0xdfcb, 0x450b, 0x9e, 0x2c, 0x12, 0xa1, 0x20, 0xcc, 0xc7, 0x1b);
// {21F3B0EE-4C74-49da-8B48-832C614308EF}
GUARD_IMPLEMENT_OLECREATE(CActionDeleteCarioLine, "ChromosEx.DeleteKaryoLine", 
0x21f3b0ee, 0x4c74, 0x49da, 0x8b, 0x48, 0x83, 0x2c, 0x61, 0x43, 0x8, 0xef);
// {E79394DB-2DB6-4279-879B-26EF22D39F33}
GUARD_IMPLEMENT_OLECREATE(CActionDeleteCarioCell, "ChromosEx.DeleteKaryoCell", 
0xe79394db, 0x2db6, 0x4279, 0x87, 0x9b, 0x26, 0xef, 0x22, 0xd3, 0x9f, 0x33);


// {AA767002-BA66-4f56-A6CF-2192F84BF84E}
static const GUID guidInsertCarioLine = 
{ 0xaa767002, 0xba66, 0x4f56, { 0xa6, 0xcf, 0x21, 0x92, 0xf8, 0x4b, 0xf8, 0x4e } };
// {B11D8BE8-4DFB-42d2-9A25-1F3B446E6F75}
static const GUID guidAppendCarioLine = 
{ 0xb11d8be8, 0x4dfb, 0x42d2, { 0x9a, 0x25, 0x1f, 0x3b, 0x44, 0x6e, 0x6f, 0x75 } };
// {20735283-12EA-4b43-9641-375E7B594FDA}
static const GUID guidInsertCarioCell = 
{ 0x20735283, 0x12ea, 0x4b43, { 0x96, 0x41, 0x37, 0x5e, 0x7b, 0x59, 0x4f, 0xda } };
// {C518311E-93CC-4f80-9222-98FCABF6A372}
static const GUID guidAppendCarioCell = 
{ 0xc518311e, 0x93cc, 0x4f80, { 0x92, 0x22, 0x98, 0xfc, 0xab, 0xf6, 0xa3, 0x72 } };
// {BC06A253-B1E9-46f9-AF0E-821EBBE3DB4A}
static const GUID guidDeleteCarioLine = 
{ 0xbc06a253, 0xb1e9, 0x46f9, { 0xaf, 0xe, 0x82, 0x1e, 0xbb, 0xe3, 0xdb, 0x4a } };
// {205187B6-1028-4c35-B978-A55013BC9DE2}
static const GUID guidDeleteCarioCell = 
{ 0x205187b6, 0x1028, 0x4c35, { 0xb9, 0x78, 0xa5, 0x50, 0x13, 0xbc, 0x9d, 0xe2 } };


ACTION_INFO_FULL(CActionInsertCarioLine, IDS_ACTION_INSERT_CARIO_LINE, -1, -1, guidInsertCarioLine);
ACTION_INFO_FULL(CActionAppendCarioLine, IDS_ACTION_APPEND_CARIO_LINE, -1, -1, guidAppendCarioLine);
ACTION_INFO_FULL(CActionInsertCarioCell, IDS_ACTION_INSERT_CARIO_CELL, -1, -1, guidInsertCarioCell);
ACTION_INFO_FULL(CActionAppendCarioCell, IDS_ACTION_APPEND_CARIO_CELL, -1, -1, guidAppendCarioCell);
ACTION_INFO_FULL(CActionDeleteCarioLine, IDS_ACTION_DELETE_CARIO_LINE, -1, -1, guidDeleteCarioLine);
ACTION_INFO_FULL(CActionDeleteCarioCell, IDS_ACTION_DELETE_CARIO_CELL, -1, -1, guidDeleteCarioCell);
ACTION_TARGET(CActionInsertCarioLine, szTargetViewSite);
ACTION_TARGET(CActionAppendCarioLine, szTargetViewSite);
ACTION_TARGET(CActionInsertCarioCell, szTargetViewSite);
ACTION_TARGET(CActionAppendCarioCell, szTargetViewSite);
ACTION_TARGET(CActionDeleteCarioLine, szTargetViewSite);
ACTION_TARGET(CActionDeleteCarioCell, szTargetViewSite);

/////////////////////////////////////////////////////////////////////////////
//
//CActionCarioViewActionBase
//
/////////////////////////////////////////////////////////////////////////////
bool CActionCarioEditBase::Invoke()
{
	if( !InvokeEdit() )return false;
	ICarioViewPtr	ptrView = get_view();
	ptrView->ArrageObjectsByClasses();
	ptrView->StoreState( GetAppUnknown() );//OnAttachTarget( pThis->m_punkTarget, true );
	return true;
}

void CActionCarioEditBase::OnAttachTarget( IUnknown *punkTarget, bool bAttach )
{
	//OnAttachTarget( pThis->m_punkTarget, true );
}

ICarioViewPtr CActionCarioEditBase::get_view()
{
	ICarioViewPtr ptrView( m_punkTarget );
	return ptrView;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionCarioEditBase::IsAvaible()
{
	ICarioViewPtr ptrView = get_view();
	if( ptrView == 0 )
		return false;

	CarioViewMode	mode;
	ptrView->GetMode( &mode );
	if( mode != cvmDesign )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
int CActionCarioEditBase::get_line_count()
{
	ICarioViewPtr ptrView = get_view();
	if( ptrView == 0 )
		return -1;

	int nCount = 0;
	ptrView->GetLineCount( &nCount );

	return nCount;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionCarioEditBase::set_line_at_pos( int nPos )
{
	ICarioViewPtr ptrView = get_view();
	if( ptrView == 0 )
		return false;

	return ( S_OK == ptrView->AddLineAtPos( nPos ) );
}

/////////////////////////////////////////////////////////////////////////////
int CActionCarioEditBase::get_cell_count( int nLine )
{
	ICarioViewPtr ptrView = get_view();
	if( ptrView == 0 )
		return -1;

	int nCount = -1;
	ptrView->GetCellCount( nLine, &nCount );

	return nCount;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionCarioEditBase::set_cell_at_pos( int nLine, int nPos )
{
	ICarioViewPtr ptrView = get_view();
	if( ptrView == 0 )
		return false;

	return ( S_OK == ptrView->AddCellAtPos( nLine, nPos ) );
}

/////////////////////////////////////////////////////////////////////////////
bool CActionCarioEditBase::delete_line( int nLine )
{
	ICarioViewPtr ptrView = get_view();
	if( ptrView == 0 )
		return false;

	return ( S_OK == ptrView->DeleteLine( nLine ) );
}

/////////////////////////////////////////////////////////////////////////////
bool CActionCarioEditBase::delete_cell( int nLine, int nCell )
{
	ICarioViewPtr ptrView = get_view();
	if( ptrView == 0 )
		return false;

	return ( S_OK == ptrView->DeleteCell( nLine, nCell ) );
}

/////////////////////////////////////////////////////////////////////////////
int CActionCarioEditBase::get_active_line()
{
	ICarioViewPtr ptrView = get_view();
	if( ptrView == 0 )
		return -1;

	int nActiveLine = -1;
	ptrView->GetActiveLine( &nActiveLine );
	return nActiveLine;
}

/////////////////////////////////////////////////////////////////////////////
int	CActionCarioEditBase::get_active_cell( )
{
	ICarioViewPtr ptrView = get_view();
	if( ptrView == 0 )
		return -1;

	int nActiveCell = 0;
	ptrView->GetActiveCell( &nActiveCell );
	
	return nActiveCell;
}



/////////////////////////////////////////////////////////////////////////////
//
//CActionInsertCarioLine
//
/////////////////////////////////////////////////////////////////////////////
bool CActionInsertCarioLine::InvokeEdit()
{
	ICarioViewPtr ptrView = get_view();
	if( ptrView == 0 )
		return false;

	int nLine = get_active_line();
	int nCount = get_line_count();
	
	if( nLine < 0 || nLine >= nCount )
		return false;

	set_line_at_pos( nLine );

	return true;

}

/////////////////////////////////////////////////////////////////////////////
bool CActionInsertCarioLine::IsAvaible()
{
	if( !CActionCarioEditBase::IsAvaible() )
		return false;

	int nLine = get_active_line();
	int nCount = get_line_count();

	if( nLine < 0 || nLine >= nCount )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//
//CActionAppendCarioLine
//
/////////////////////////////////////////////////////////////////////////////
bool CActionAppendCarioLine::InvokeEdit()
{	 
	ICarioViewPtr ptrView = get_view();
	if( ptrView == 0 )
		return false;

	int nCount = get_line_count();
	if( nCount < 1 )//where the cellar ???
		return false;

	set_line_at_pos( nCount - 1	);

	return true;

}

/////////////////////////////////////////////////////////////////////////////
bool CActionAppendCarioLine::IsAvaible()
{
	if( !CActionCarioEditBase::IsAvaible() )
		return false;

	int nLine = get_active_line();
	int nCount = get_line_count();

	if( nCount < 1 || nLine == nCount -1 )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//
//CActionInsertCarioCell
//
/////////////////////////////////////////////////////////////////////////////
bool CActionInsertCarioCell::InvokeEdit()
{
	ICarioViewPtr ptrView = get_view();
	if( ptrView == 0 )
		return false;

	int nLine		= get_active_line();
	int nLineCount	= get_line_count();

	if( nLine < 0 || nLine >= nLineCount - 1 )
		return false;

	int nCell		= get_active_cell( );
	int nCellCount	= get_cell_count( nLine );

	if( nCellCount == 0 )
	{
		set_cell_at_pos( nLine, 0 );
		return true;
	}

	if( nCell < 0 || nCell >= nCellCount )
		return false;	

	set_cell_at_pos( nLine, nCell );

	return true;

}

/////////////////////////////////////////////////////////////////////////////
bool CActionInsertCarioCell::IsAvaible()
{
	if( !CActionCarioEditBase::IsAvaible() )
		return false;

	int nLine		= get_active_line();
	int nLineCount	= get_line_count();

	if( nLine < 0 || nLine >= nLineCount - 1 )
		return false;

	int nCell		= get_active_cell( );
	int nCellCount	= get_cell_count( nLine );

	if( nCellCount == 0 )
		return true;

	if( nCell < 0 || nCell >= nCellCount )
		return false;	

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//
//CActionAppendCarioCell
//
/////////////////////////////////////////////////////////////////////////////
bool CActionAppendCarioCell::InvokeEdit()
{
	ICarioViewPtr ptrView = get_view();
	if( ptrView == 0 )
		return false;

	int nLine		= get_active_line();
	int nLineCount	= get_line_count();

	if( nLine < 0 || nLine >= nLineCount - 1 )
		return false;

	int nCellCount	= get_cell_count( nLine );
	if( nCellCount < 0 )
		return false;

	set_cell_at_pos( nLine, nCellCount );

	return true;

}

/////////////////////////////////////////////////////////////////////////////
bool CActionAppendCarioCell::IsAvaible()
{
	if( !CActionCarioEditBase::IsAvaible() )
		return false;

	int nLine		= get_active_line();
	int nLineCount	= get_line_count();

	if( nLine < 0 || nLine >= nLineCount - 1 )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//
//CActionDeleteCarioLine
//
/////////////////////////////////////////////////////////////////////////////
bool CActionDeleteCarioLine::InvokeEdit()
{
	ICarioViewPtr ptrView = get_view();
	if( ptrView == 0 )
		return false;

	int nLine		= get_active_line();
	int nLineCount	= get_line_count();

	if( nLine < 0 || nLine >= nLineCount - 1 )
		return false;

	delete_line( nLine );

	return true;

}

/////////////////////////////////////////////////////////////////////////////
bool CActionDeleteCarioLine::IsAvaible()
{
	if( !CActionCarioEditBase::IsAvaible() )
		return false;

	int nLine		= get_active_line();
	int nLineCount	= get_line_count();

	if( nLine < 0 || nLine >= nLineCount - 1 )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//
//CActionDeleteCarioCell
//
/////////////////////////////////////////////////////////////////////////////
bool CActionDeleteCarioCell::InvokeEdit()
{
	ICarioViewPtr ptrView = get_view();
	if( ptrView == 0 )
		return false;

	int nLine		= get_active_line();
	int nLineCount	= get_line_count();

	if( nLine < 0 || nLine >= nLineCount - 1 )
		return false;

	int nCell		= get_active_cell( );
	int nCellCount	= get_cell_count( nLine );

	if( nCell < 0 || nCell >= nCellCount )
		return false;	

	delete_cell( nLine, nCell );

	return true;

}

/////////////////////////////////////////////////////////////////////////////
bool CActionDeleteCarioCell::IsAvaible()
{
	if( !CActionCarioEditBase::IsAvaible() )
		return false;

	int nLine		= get_active_line();
	int nLineCount	= get_line_count();

	if( nLine < 0 || nLine >= nLineCount - 1 )
		return false;

	int nCell		= get_active_cell( );
	int nCellCount	= get_cell_count( nLine );

	if( nCell < 0 || nCell >= nCellCount )
		return false;	

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//
//CActionDeleteChromosome
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionDeleteChromosome, CCmdTargetEx);

GUARD_IMPLEMENT_OLECREATE(CActionDeleteChromosome, "ChromosEx.DeleteChromosome", 
0xa3b7411c, 0x701, 0x4443, 0xab, 0x3e, 0x63, 0x6, 0x5a, 0xe4, 0x54, 0xe5);

static const GUID  guidDeleteChromosome = 
{ 0x9fa99477, 0x6169, 0x4937, { 0xa2, 0xc3, 0xc0, 0xe1, 0x49, 0xa7, 0x60, 0x2e } };


ACTION_INFO_FULL(CActionDeleteChromosome, IDS_ACTION_DELETE_CHROMOSOME, -1, -1, guidDeleteChromosome);
ACTION_TARGET(CActionDeleteChromosome, szTargetViewSite);

/////////////////////////////////////////////////////////////////////////////
bool CActionDeleteChromosome::IsAvaible()
{
	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;	

	CarioViewMode cvm;
	ptrView->GetMode( &cvm );

	//удалять пока можем только из подвала
	if( cvm != cvmCellar )
		return false;

	int nLine, nCell, nObj;
	nLine = nCell = nObj = -1;
	ptrView->GetActiveChromo( &nLine, &nCell, &nObj );
	if( nObj < 0 )
		return false;

	return true; 
}

/////////////////////////////////////////////////////////////////////////////
bool CActionDeleteChromosome::Invoke()
{
	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;	
	
	int nLine, nCell, nObj;
	nLine = nCell = nObj = -1;
	ptrView->GetActiveChromo( &nLine, &nCell, &nObj );
	if( nObj < 0 )
		return false;

	ptrView->DeleteObj( nLine, nCell, nObj );

	return true; 

}

/////////////////////////////////////////////////////////////////////////////
//
//CActionCarioBuild
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionCarioBuild, CCmdTargetEx);



// {CA00063B-1045-4ca0-9076-06115B6DFC38}
GUARD_IMPLEMENT_OLECREATE(CActionCarioBuild, "ChromosEx.BuildKaryo", 
0xca00063b, 0x1045, 0x4ca0, 0x90, 0x76, 0x6, 0x11, 0x5b, 0x6d, 0xfc, 0x38);

// {5E95C053-1DF7-42dd-A0D3-B8D275A04FCA}
static const GUID guidBuildCario = 
{ 0x5e95c053, 0x1df7, 0x42dd, { 0xa0, 0xd3, 0xb8, 0xd2, 0x75, 0xa0, 0x4f, 0xca } };


ACTION_INFO_FULL(CActionCarioBuild, IDS_CARIO_BUILD, -1, -1, guidBuildCario);
ACTION_TARGET(CActionCarioBuild, szTargetViewSite);


ACTION_ARG_LIST(CActionCarioBuild)
	ARG_STRING("ObjectList", 0)
END_ACTION_ARG_LIST()


bool recursive_copy( INamedData* pISrc, INamedData* pITarget, const char* psz_path )
{
	if( !psz_path || !pISrc || !pITarget )
		return false;

	_bstr_t bstrPath( psz_path );

	// set section
	pISrc->SetupSection( bstrPath );
	
	// get count of entries
	long lEntryCount = 0;
	pISrc->GetEntriesCount( &lEntryCount );

	// for all entries
	for (int  i = 0; i < (int)lEntryCount; i++)
	{
		BSTR bstr = NULL;			
		pISrc->GetEntryName( i, &bstr );			
		_bstr_t bstrName( bstr );
		::SysFreeString( bstr );	bstr = 0;			
		
		_bstr_t bstrPathNew = bstrPath + "\\" + bstrName;
		
		_variant_t var;
		pISrc->GetValue( bstrPathNew, &var );
		
		pITarget->SetupSection( bstrPathNew );
		pITarget->SetValue( bstrPathNew, var );

		if( lEntryCount > 0 )
			recursive_copy( pISrc, pITarget, bstrPathNew );

		pISrc->SetupSection( bstrPath );
	}

	return true;
	
}
/////////////////////////////////////////////////////////////////////////////
bool CActionCarioBuild::Invoke()
{
	IUnknown *punkObj = get_object_list();
	if( !punkObj )
		return false;

	IUnknownPtr ptrObj = punkObj;
	punkObj->Release();	punkObj = 0;
	
	INamedDataObject2Ptr ptrNDO( ptrObj );
	if( ptrNDO == 0 )
		return false;

	INamedDataPtr ptrNDObj( ptrObj );
	if( ptrNDObj == 0 )
		ptrNDO->InitAttachedData();

	ptrNDObj = ptrObj;
	
	if( ptrNDObj == 0 )
		return false;

	INamedDataPtr ptrShellData = ::GetAppUnknown();
	if( ptrShellData == 0 )
		return false;

	//[AY] здесь удаляем только секцию расположения хромосом в кариограмме - иначе баг
	//Если анализ проводится до Cario Build, то после него состояние предыдущего анализа удаляется. 
	ptrNDObj->DeleteEntry( _bstr_t( CARIO_VIEW_PREVIEW ) );	
	//все остальное скопируется поверх, а анализ останется нетронутым
	recursive_copy( ptrShellData, ptrNDObj, CARIO_ROOT );

	::SetValue( ptrNDObj, CARIO_ROOT, CARIO_WAS_BUILD, 1L );
	::SetValue( ptrNDObj, CARIO_ROOT, CLASS_FILE_NAME, GetCurrentClassFileName() );

	IViewPtr ptrView( m_punkTarget );
	if( ptrView )
	{
		IUnknown* punkDoc = 0;
		ptrView->GetDocument( &punkDoc );
		if( punkDoc )
		{			
			IFileDataObject2Ptr ptrFDO2( punkDoc );
			if( ptrFDO2 )	ptrFDO2->SetModifiedFlag( TRUE );
				
			::FireEvent( punkDoc, szEventCarioBuild, ptrView, ptrObj, 0, 0 );
			punkDoc->Release();	punkDoc = 0;
		}
	}

	return true;	
}

/////////////////////////////////////////////////////////////////////////////
IUnknown* CActionCarioBuild::get_object_list()
{
	IUnknown *punkObj = 0;
	{
		IViewPtr ptr_view = m_punkTarget;
		if( ptr_view )
		{
			IUnknown* punk_doc = 0;
			ptr_view->GetDocument( &punk_doc );
			if( punk_doc )
			{
				punkObj = ::FindObjectByName( punk_doc, GetArgumentString( "ObjectList" ));
				punk_doc->Release();	punk_doc = 0;
			}
		}
	}

	if( !punkObj )
		punkObj = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );

	return punkObj;

}

/////////////////////////////////////////////////////////////////////////////
bool CActionCarioBuild::IsAvaible()
{
	IUnknown* punkObj = get_object_list();
	if( !punkObj )
		return false;

	INamedDataObject2Ptr ptrNDO2( punkObj );
	punkObj->Release();	punkObj = 0;
	if( ptrNDO2 == 0 )
		return false;

	bool bFound = false;
	POSITION lPosChild = 0;

	ptrNDO2->GetFirstChildPosition( &lPosChild );
	while( lPosChild ) 
	{
		IUnknown* punkChild = 0;
		ptrNDO2->GetNextChild( &lPosChild, &punkChild );
		if( !punkChild )
			continue;

		INamedDataObject2Ptr ptrNDOCromoParent( punkChild );
		punkChild->Release();	punkChild = 0;
		if( ptrNDOCromoParent == 0 )
			continue;

		POSITION lPosCromo = 0;
		ptrNDOCromoParent->GetFirstChildPosition( &lPosCromo );
		while( lPosCromo )
		{
			IUnknown* punkChromo = 0;
			ptrNDOCromoParent->GetNextChild( &lPosCromo, &punkChromo );
			if( !punkChromo )
				continue;

			if (::CheckInterface( punkChromo, IID_IChromosome ) )
			{
				bFound = true;
			}

			punkChromo->Release();
			if( bFound )
				break;
		}

		if( bFound )
			break;
	}

	return bFound;
}
				

/////////////////////////////////////////////////////////////////////////////
//
//CActionShowChromoIdiogramm
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionShowChromoIdiogramm, CCmdTargetEx);



// {339C2705-AFBE-41c1-B622-ABEA30C9DE69}
GUARD_IMPLEMENT_OLECREATE(CActionShowChromoIdiogramm, "ChromosEx.ShowChromoIdiogramm", 
0x339c2705, 0xafbe, 0x41c1, 0xb6, 0x22, 0xab, 0xea, 0x30, 0xc9, 0xde, 0x69);


// {492C2128-AFBD-4ef9-B73E-5F17F0071798}
static const GUID guidShowChromoIdiogramm = 
{ 0x492c2128, 0xafbd, 0x4ef9, { 0xb7, 0x3e, 0x5f, 0x17, 0xf0, 0x7, 0x17, 0x98 } };


ACTION_INFO_FULL(CActionShowChromoIdiogramm, IDS_SHOW_CHROMO_IDIOGRAMM, -1, -1, guidShowChromoIdiogramm);
ACTION_TARGET(CActionShowChromoIdiogramm, szTargetViewSite);


//arguments
ACTION_ARG_LIST(CActionShowChromoIdiogramm)
	ARG_INT(_T("Show"), -1 )	
END_ACTION_ARG_LIST();

/////////////////////////////////////////////////////////////////////////////
bool CActionShowChromoIdiogramm::Invoke()
{
	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;

	int bShowIdiorgamm	= GetArgumentInt( _T("Show") );
	int bUseArguments	= bShowIdiorgamm >= 0;

	int nLine	= 0;
	int nCell	= 0;
	int nChromo	= 0;
	ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );
	if( nChromo < 0 )
		return false;
	
	BOOL bShow = FALSE;
	ptrView->GetShowChromoIdiogramm( nLine, nCell, nChromo, &bShow );
	bShow = !bShow;

	if( bUseArguments )
		bShow = bShowIdiorgamm;

	ptrView->SetShowChromoIdiogramm( nLine, nCell, nChromo, bShow );

	return true;	
}

/////////////////////////////////////////////////////////////////////////////
bool CActionShowChromoIdiogramm::IsAvaible()
{
	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;
		
	CarioViewMode cvm = cvmPreview;
	ptrView->GetMode( &cvm );

	if( !( cvm == cvmPreview || cvm == cvmCellar ) )
		return false;

//	if( !IsActiveObjectListHasChromoObject( m_punkTarget ) )
//		return false;	

	{
		IUnknown* punk_ol = GetActiveObjectList( m_punkTarget );
		if( !punk_ol )
			return false;

		bool b = IsObjectListHasChromoObject( punk_ol );
		punk_ol->Release();	punk_ol = 0;

		if( !b )
			return false;
	}

	int nLine	= 0;
	int nCell	= 0;
	int nChromo	= 0;
	ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );
	
	BOOL bShow = FALSE;
	return ptrView->GetShowChromoIdiogramm( nLine, nCell, nChromo, &bShow ) == S_OK;

}

/////////////////////////////////////////////////////////////////////////////
bool CActionShowChromoIdiogramm::IsChecked()
{
	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;

	int nLine	= 0;
	int nCell	= 0;
	int nChromo	= 0;
	ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );
	
	BOOL bShow = FALSE;
	ptrView->GetShowChromoIdiogramm( nLine, nCell, nChromo, &bShow );

	return ( bShow == TRUE );	
}


/////////////////////////////////////////////////////////////////////////////
//
//CActionShowAllChromoIdiogramm
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionShowAllChromoIdiogramm, CCmdTargetEx);



// {B7188280-3D22-4709-942A-426A294730C6}
GUARD_IMPLEMENT_OLECREATE(CActionShowAllChromoIdiogramm, "ChromosEx.ShowAllChromoIdiogramm", 
0xb7188280, 0x3d22, 0x4709, 0x94, 0x2a, 0x42, 0x6a, 0x29, 0x47, 0x30, 0xc6);


// {A841821B-FD72-4192-9781-0E541727C21E}
static const GUID guidShowAllChromoIdiogramm = 
{ 0xa841821b, 0xfd72, 0x4192, { 0x97, 0x81, 0xe, 0x54, 0x17, 0x27, 0xc2, 0x1e } };

ACTION_INFO_FULL(CActionShowAllChromoIdiogramm, IDS_SHOW_CHROMO_ALL_IDIOGRAMM, -1, -1, guidShowAllChromoIdiogramm);
ACTION_TARGET(CActionShowAllChromoIdiogramm, szTargetViewSite);


//arguments
ACTION_ARG_LIST(CActionShowAllChromoIdiogramm)	
	ARG_INT(_T("Show"), -1 )	
END_ACTION_ARG_LIST();

/////////////////////////////////////////////////////////////////////////////
bool CActionShowAllChromoIdiogramm::Invoke()
{
	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;

	int bShowIdiorgamm	= GetArgumentInt( _T("Show") );

	if( bShowIdiorgamm == -1 )
		bShowIdiorgamm = _next_state();

	ptrView->SetShowChromoAllIdiogramm( bShowIdiorgamm );

	return true;	
}

/////////////////////////////////////////////////////////////////////////////
bool CActionShowAllChromoIdiogramm::IsAvaible()
{
	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;

	if( !IsActiveObjectListHasChromoObject( m_punkTarget ) )
		return false;

	CarioViewMode cvm = cvmPreview;
	ptrView->GetMode( &cvm );

	if(  cvm != cvmPreview && cvm != cvmCellar )
		return false;

	if( ptrView->IsEmpty() == S_OK )
		return false;

	return true;
}

bool CActionShowAllChromoIdiogramm::IsChecked()
{
	return _next_state( false );
}

bool  CActionShowAllChromoIdiogramm::_next_state( bool bChange )
{
	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;

	BOOL	fShow;
	ptrView->GetShowChromoAllIdiogramm( &fShow );

	return fShow == (bChange==false);
}

/////////////////////////////////////////////////////////////////////////////
//
//CCarioSetChromoZoom
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionSetChromoZoom, CCmdTargetEx);

// {0554F2CD-0B9E-4e7e-867B-20B1DFB44BF2}
GUARD_IMPLEMENT_OLECREATE(CActionSetChromoZoom, "ChromosEx.SetChromoZoom", 
0x554f2cd, 0xb9e, 0x4e7e, 0x86, 0x7b, 0x20, 0xb1, 0xdf, 0xb4, 0x4b, 0xf2);

// {B30B3DEA-8CAB-401b-9A80-BC0BE1FCCEC6}
static const GUID guidSetChromoZoom = 
{ 0xb30b3dea, 0x8cab, 0x401b, { 0x9a, 0x80, 0xbc, 0xb, 0xe1, 0xfc, 0xce, 0xc6 } };


ACTION_INFO_FULL(CActionSetChromoZoom, IDS_SET_CHROMO_ZOOM, -1, -1, guidSetChromoZoom);
ACTION_TARGET(CActionSetChromoZoom, szTargetViewSite);

//arguments


/////////////////////////////////////////////////////////////////////////////
CActionSetChromoZoom::CActionSetChromoZoom()
{
	m_ptStart			= CPoint( 0, 0 );
	m_ptFinish			= CPoint( 0, 0 );
	m_nArrowSize		= 5;
	m_nLineSize			= 20;
	m_bTrack = false;
}

ACTION_ARG_LIST(CActionSetChromoZoom)	
	ARG_DOUBLE(_T("Zoom"), 0 )		
END_ACTION_ARG_LIST();

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoZoom::activate_chromosome( CPoint pt )
{
	ICarioViewPtr	ptrView( m_punkTarget );
	if( ptrView == 0 )return false;

	int	line, cell, pos;
	ptrView->GetCellByPoint( pt, &line, &cell, &pos, TRUE );
	ptrView->SetActiveChromo( line, cell, pos );

	return pos !=-1;
}
bool CActionSetChromoZoom::DoLButtonDown( CPoint pt )
{
	m_bTrack = false;
	activate_chromosome( pt );

	return false;
}
bool CActionSetChromoZoom::DoLButtonUp( CPoint pt )
{
	//if( !m_bTrack )
	//	activate_chromosome( pt );
	return false;
}

bool CActionSetChromoZoom::Invoke()
{	

	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;

	double fZoom		= GetArgumentDouble( _T("Zoom") );
	bool	bUseArguments = fZoom > 0;

	{
		IUnknown	*punkDoc = 0;
		sptrIView	sptrV( m_punkTarget );
		sptrV->GetDocument( &punkDoc );
		sptrIDocument	sptrD( punkDoc );

		m_lTargetKey = ::GetObjectKey( sptrD );

		if( !punkDoc )
			return false;

		punkDoc->Release();
	}

	int nLine	= 0;
	int nCell	= 0;
	int nChromo	= 0;
	if( S_OK != ptrView->GetActiveChromo( &nLine, &nCell, &nChromo ) )
		return false;

	IUnknown* punk = 0;
	ptrView->GetChromoObject( nLine, nCell, nChromo, &punk );
	if( !punk )	return false;

	IChromosomePtr ptrChromo( punk );
	punk->Release();	punk = 0;

	ptrChromo->GetRotatedImage( TRUE, &punk, 0 );
	if( !punk )	return false;

	IImagePtr ptrImage = punk;
	punk->Release();	punk = 0;

	if( ptrImage == 0 ) return false;

	int cx, cy;
	cx = cy = 0;
	ptrImage->GetSize( &cx, &cy );
	if( cy < 1 )	return false;	

	if( !bUseArguments )
	{
		_fill_undo_redo();
		CRect rc = get_rect();
		if( rc.Height() < 10 )
			return false;

		fZoom = (double)rc.Height() / ( (double)cy );
	}

	if( fZoom <= 0.0 )
		return false;

	ptrView->SetChromoZoom( nLine, nCell, nChromo, fZoom );

	_fill_undo_redo( false );

	LeaveMode();

	SetModifyFlag( m_punkTarget );

	return true;	
}

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoZoom::IsAvaible()
{	
	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;

	if( !IsActiveObjectListHasChromoObject( m_punkTarget ) )
		return false;

	if( ptrView->IsEmpty() == S_OK )
		return false;

	CarioViewMode ViewMode;
	ptrView->GetMode( &ViewMode );

	if( ViewMode == cvmCellar )
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoZoom::Initialize()
{	
	m_clrLine		= ::GetValueColor( ::GetAppUnknown(), CARIO_VIEW_SHARED, "ZoomLineColor", RGB( 0, 0, 0 ) );

	m_nArrowSize	= ::GetValueInt( ::GetAppUnknown(), CARIO_VIEW_SHARED, "ArrowSize", 5 );
	
	if( m_nArrowSize < 1 )
		m_nArrowSize = 5;

	m_nLineSize		= ::GetValueInt( ::GetAppUnknown(), CARIO_VIEW_SHARED, "ArrowLineSize", 20 );
	if( m_nLineSize < 1 )
		m_nLineSize = 20;

	bool bres = CInteractiveActionBase::Initialize();
	if( !bres )
		return false;

	if( GetArgumentDouble( "Zoom" ) > 0 )
	{
		Finalize();
		return false;						
	}	


	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoZoom::DoStartTracking( CPoint pt )
{
	m_bTrack = true;
	_fill_undo_redo();
	m_ptFinish = m_ptStart = pt;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoZoom::DoFinishTracking( CPoint pt )
{	
	m_ptFinish = pt;
	Finalize();

	invalidate_rect( true );
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoZoom::DoTrack( CPoint pt )
{
	invalidate_rect( false );
	m_ptFinish = pt;
	_Draw();
	return false;
}

/////////////////////////////////////////////////////////////////////////////
void CActionSetChromoZoom::DoDraw( CDC &dc )
{
	CRect rc = get_rect();
	rc = ConvertToWindow( m_punkTarget, rc );


	CPoint ptUp( rc.left + rc.Width() / 2,  rc.top );
	CPoint ptDown( rc.left + rc.Width() / 2,  rc.bottom );

	CPen pen( PS_SOLID, 1, m_clrLine );
	
	CPen* pOldPen = dc.SelectObject( &pen );	

	dc.MoveTo( rc.left, rc.top );
	dc.LineTo( rc.right, rc.top );

	dc.MoveTo( rc.left, rc.bottom );
	dc.LineTo( rc.right, rc.bottom );

	dc.MoveTo( ptUp );
	dc.LineTo( ptDown );

	if( rc.Height() > 2 * m_nArrowSize + 10 )
	{
		dc.MoveTo( ptUp );
		dc.LineTo( ptUp.x - m_nArrowSize, ptUp.y + m_nArrowSize );

		dc.MoveTo( ptUp );
		dc.LineTo( ptUp.x + m_nArrowSize, ptUp.y + m_nArrowSize );
		
		dc.MoveTo( ptDown );
		dc.LineTo( ptDown.x - m_nArrowSize, ptDown.y - m_nArrowSize );

		dc.MoveTo( ptDown );
		dc.LineTo( ptDown.x + m_nArrowSize, ptDown.y - m_nArrowSize );
	}

	dc.SelectObject( pOldPen );

}

/////////////////////////////////////////////////////////////////////////////
CRect CActionSetChromoZoom::get_rect()
{
	CRect rect;
	rect.left	= m_ptStart.x	- m_nLineSize;
	rect.right	= m_ptStart.x	+ m_nLineSize;
	rect.top	= m_ptStart.y;
	rect.bottom	= m_ptFinish.y;

	rect.NormalizeRect();

	return rect;
}
/////////////////////////////////////////////////////////////////////////////
void CActionSetChromoZoom::invalidate_rect( bool bUpdateWindow )
{	
	CRect rect = get_rect();
	
	rect.InflateRect( 1, 1, 1, 1 );
	rect = ::ConvertToWindow( m_punkTarget, rect );
	CWnd	*pwnd = GetWindowFromUnknown( m_punkTarget );
	
	if( pwnd )
	{
		pwnd->InvalidateRect( rect );
		if( bUpdateWindow )
			pwnd->UpdateWindow();

	}

}

void CActionSetChromoZoom::_fill_undo_redo( bool bUndo )
{
	ICarioViewPtr	ptrView( m_punkTarget );
	if( ptrView == 0 )
		return;

	int nLine	= 0;
	int nCell	= 0;
	int nChromo	= 0;
	if( S_OK != ptrView->GetActiveChromo( &nLine, &nCell, &nChromo ) )
		return;

	if( nChromo < 0 )
		return;

//	chromo_object *pobject = 0;
	double fZoom = 0.;
	if( S_OK != ptrView->GetChromoZoom( nLine, nCell, nChromo, &fZoom ) )
		return;

	char *update_p = new char[4096];
	if( S_OK != ptrView->GetChromoPath( nLine, nCell, nChromo, &update_p ) )
	{
		delete []update_p;
		return;
	}


	_object_params param;
	
	param.UpdatePath = CString( update_p );

	param.fZoom = fZoom;

	if( bUndo )
		m_paramOld = param;
	else
		m_paramNew = param;

	delete []update_p;
}

bool CActionSetChromoZoom::DoUndo()
{
	_undo( m_paramOld );

	return true;
}

void CActionSetChromoZoom::_undo( _object_params &param )
{
	IUnknown* punkObj = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );

	if( !punkObj )
		return;

	::SetValue( punkObj, param.UpdatePath, "ZoomObject", param.fZoom );

	::FireEvent( m_punkTarget, szEventCarioChange, 0, punkObj, 0, 0 );
	
	if( punkObj )
		punkObj->Release();

}

bool CActionSetChromoZoom::DoRedo()
{
	_undo( m_paramNew );
	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//CActionSetChromoAngle
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionSetChromoAngle, CCmdTargetEx);

// {394AAF4C-F865-4856-9E86-68A33CE81A4D}
GUARD_IMPLEMENT_OLECREATE(CActionSetChromoAngle, "ChromosEx.SetChromoAngle", 
0x394aaf4c, 0xf865, 0x4856, 0x9e, 0x86, 0x68, 0xa3, 0x3c, 0xe8, 0x1a, 0x4d);

// {A7066183-1395-45a0-8858-FFA0E3EBC7A2}
static const GUID guidSetChromoAngle = 
{ 0xa7066183, 0x1395, 0x45a0, { 0x88, 0x58, 0xff, 0xa0, 0xe3, 0xeb, 0xc7, 0xa2 } };


ACTION_INFO_FULL(CActionSetChromoAngle, IDS_SET_CHROMO_ANGLE, -1, -1, guidSetChromoAngle);
ACTION_TARGET(CActionSetChromoAngle, szTargetViewSite);

/////////////////////////////////////////////////////////////////////////////
CActionSetChromoAngle::CActionSetChromoAngle()
{
}

CActionSetChromoAngle::~CActionSetChromoAngle()
{
}

ACTION_ARG_LIST(CActionSetChromoAngle)	
	ARG_DOUBLE(_T("Angle"), 0 )
	ARG_INT(_T("TerminateAfter"), -1 )
END_ACTION_ARG_LIST();

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoAngle::activate_chromosome( CPoint pt )
{
	ICarioViewPtr	ptrView( m_punkTarget );
	if( ptrView == 0 )return false;

	int	line, cell, pos;
	ptrView->GetCellByPoint( pt, &line, &cell, &pos, FALSE );
	ptrView->SetActiveChromo( line, cell, pos );

	return pos !=-1;
}
bool CActionSetChromoAngle::DoLButtonDown( CPoint pt )
{
	activate_chromosome( pt );

	if( GetArgumentInt( _T("TerminateAfter") ) != -1 )
		return false;

	if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
	{
		_fill_undo_redo();
		mirror_chromo();
		Finalize();
		return true;
	}
	else if( GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
	{
		_fill_undo_redo();
		m_fNewAngle = get_chromo_angle() + PI;
		set_chromo_angle( m_fNewAngle );
		Finalize();
		return true;
	}
	return false;
}

bool CActionSetChromoAngle::Invoke()
{	
	double fAngle		= GetArgumentDouble( _T("Angle") );
	int bUseArguments	= fAngle != 0;

	{
		IUnknown	*punkDoc = 0;
		sptrIView	sptrV( m_punkTarget );
		sptrV->GetDocument( &punkDoc );
		sptrIDocument	sptrD( punkDoc );

		m_lTargetKey = ::GetObjectKey( sptrD );

		if( !punkDoc )
			return false;

		punkDoc->Release();
	}

	if( bUseArguments )
	{
		_fill_undo_redo();
		set_chromo_angle( fAngle*PI/180 );
	}

	_fill_undo_redo( false );

	if( GetArgumentInt( _T("TerminateAfter") ) != -1 )
		LeaveMode();

	IUnknown* punkObj = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	m_sptrList = punkObj;
	punkObj->Release();

	SetModifyFlag( m_punkTarget );

//	LeaveMode();
	return true;	
}

void CActionSetChromoAngle::_fill_undo_redo( bool bUndo )
{
	ICarioViewPtr	ptrView( m_punkTarget );
	if( ptrView == 0 )
		return;

	int nLine	= 0;
	int nCell	= 0;
	int nChromo	= 0;
	if( S_OK != ptrView->GetActiveChromo( &nLine, &nCell, &nChromo ) )
		return;

	if( nChromo < 0 )
		return;

	double fAngle = 0.;
	if( S_OK != ptrView->GetChromoAngle( nLine, nCell, nChromo, &fAngle ) )
		return;

//	_object_params *param = new _object_params;

	_object_params param;

	param.fAngle  = fAngle;

	if( S_OK != ptrView->GetChromoObject( nLine, nCell, nChromo, &param.punkObject ) )
		return;

//	m_UndoRedoStack.add( param, bUndo );

	if( bUndo )
		m_paramOld = param;
	else
		m_paramNew = param;
}



/////////////////////////////////////////////////////////////////////////////
void CActionSetChromoAngle::Terminate()
{
//	set_chromo_angle( m_fStartAngle );
	CInteractiveActionBase::Terminate();
}

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoAngle::IsAvaible()
{
	if( !( /*IsActiveObjectListWasBuild( m_punkTarget ) && */IsActiveObjectListHasChromoObject( m_punkTarget ) ) )
		return false;

	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;
	
	CarioViewMode cvm = cvmPreview;
	ptrView->GetMode( &cvm );

	if( !( cvm == cvmPreview || cvm == cvmCellar ) )
		return false;

//	int nChromo = -1;
//	ptrView->GetActiveChromo( 0, 0, &nChromo );
//	return ( nChromo >= 0 );

	BOOL	fShow = true;

	ICarioView2Ptr sptrV = ptrView;
	sptrV->GetShowAllChromoObject( &fShow );

	if( !fShow )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoAngle::Initialize()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;

	if( GetArgumentInt( _T("TerminateAfter") ) != -1 )
	{

		POINT pt;
		::GetCursorPos( &pt );

		CWnd *pWnd = ::GetWindowFromUnknown( m_punkTarget );

		if( pWnd )
			pWnd->ScreenToClient( &pt );

		CPoint ptCl = ::ConvertToClient( m_punkTarget, CPoint( pt ) );
		pWnd->PostMessage( WM_LBUTTONDOWN, 0, MAKEWORD( pt.x, pt.y ) );

	}


	bool bres = CInteractiveActionBase::Initialize();


	if( !bres )
		return false;

	if( GetArgumentDouble( _T("Angle") ) != 0 )
	{
		Finalize();
		return false;						
	}	

	m_hcurActive = AfxGetApp()->LoadCursor( IDC_CURSOR_ROTATE );


	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoAngle::DoStartTracking( CPoint pt )
{
 	if( !activate_chromosome( pt ) )
		return false;


	CRect	rect;
	ICarioViewPtr	ptrView( m_punkTarget );
	if( ptrView == 0 )return false;

	int	line, cell, pos;
	ptrView->GetActiveChromo( &line, &cell, &pos );

	ptrView->GetChromoRect( GCR_CHROMOSOME, line, cell, pos, &rect );
	m_pointCenter = rect.CenterPoint();

	m_fNewAngle = get_chromo_angle();
	m_fAngleStart = ::atan2( double(pt.y-m_pointCenter.y), double(pt.x-m_pointCenter.x) )-m_fNewAngle;
	_fill_undo_redo();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoAngle::DoFinishTracking( CPoint pt )
{
	Finalize();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoAngle::DoTrack( CPoint pt )
{
	m_fNewAngle = ::atan2( double(pt.y-m_pointCenter.y), double(pt.x-m_pointCenter.x) )-m_fAngleStart;
	set_chromo_angle( m_fNewAngle );

	GetWindowFromUnknown( m_punkTarget )->UpdateWindow();
	return true;
}



/////////////////////////////////////////////////////////////////////////////
double CActionSetChromoAngle::get_chromo_angle( )
{
	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;

	int nLine	= 0;
	int nCell	= 0;
	int nChromo	= 0;
	if( S_OK != ptrView->GetActiveChromo( &nLine, &nCell, &nChromo ) )
		return false;

	if( nChromo < 0 )
		return false;

	double fAngle = 0.;
	if( S_OK != ptrView->GetChromoAngle( nLine, nCell, nChromo, &fAngle ) )
		return 0.;

	return fAngle;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoAngle::set_chromo_angle( double fAngle )
{
	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;

	int nLine	= 0;
	int nCell	= 0;
	int nChromo	= 0;
	if( S_OK != ptrView->GetActiveChromo( &nLine, &nCell, &nChromo ) )
		return false;

	if( nChromo < 0 )
		return false;

	return ( S_OK == ptrView->SetChromoAngle( nLine, nCell, nChromo, fAngle ) );
}

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoAngle::mirror_chromo()
{
	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;

	int nLine	= 0;
	int nCell	= 0;
	int nChromo	= 0;
	if( S_OK != ptrView->GetActiveChromo( &nLine, &nCell, &nChromo ) )
		return false;

	if( nChromo < 0 )
		return false;

	return ( S_OK == ptrView->MirrorChromo( nLine, nCell, nChromo ) );
}

bool CActionSetChromoAngle::DoUndo()
{
	_undo( m_paramOld );

	return true;
}

void CActionSetChromoAngle::_undo( _object_params &param )
{
	IChromosomePtr ptrChromo = param.punkObject;
	
	ROTATEPARAMS rp;
	ptrChromo->GetRotateParams( &rp );

	ptrChromo->GetRotateParams( &rp );

	rp.dAngleVisible = param.fAngle-rp.dAngle;
	rp.bManual = TRUE;

	ptrChromo->SetRotateParams( &rp );		
	ptrChromo->SetRotatedImage( 0 );
	ptrChromo->SetEqualizedRotatedImage( 0 );

	::FireEvent( m_punkTarget, szEventCarioChange, 0, m_sptrList, 0, 0 );
}

bool CActionSetChromoAngle::DoRedo()
{
	_undo( m_paramNew );
	return true;
}

/////////////////////////////////////////////////////////////////////////////
//
//CCarioSetChromoMirror
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionSetChromoMirror, CCmdTargetEx);

// {D49819E3-EE1C-47cb-823B-6E05BC8A7A13}
GUARD_IMPLEMENT_OLECREATE(CActionSetChromoMirror, "ChromosEx.SetChromoMirror", 
0xd49819e3, 0xee1c, 0x47cb, 0x82, 0x3b, 0x6e, 0x5, 0xbc, 0x8a, 0x7a, 0x13);

// {D7B6B9AA-B20A-44fa-836B-FB6A3D78929A}
static const GUID guidSetChromoMirror = 
{ 0xd7b6b9aa, 0xb20a, 0x44fa, { 0x83, 0x6b, 0xfb, 0x6a, 0x3d, 0x78, 0x92, 0x9a } };


ACTION_INFO_FULL(CActionSetChromoMirror, IDS_SET_CHROMO_MIRROR, -1, -1, guidSetChromoMirror);
ACTION_TARGET(CActionSetChromoMirror, szTargetViewSite);

//arguments


/////////////////////////////////////////////////////////////////////////////
CActionSetChromoMirror::CActionSetChromoMirror()
{
}

ACTION_ARG_LIST(CActionSetChromoMirror)	
	ARG_INT(_T("Mirror"), -1 )		
END_ACTION_ARG_LIST();

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoMirror::Invoke()
{	
	{
		IUnknown	*punkDoc = 0;
		sptrIView	sptrV( m_punkTarget );
		sptrV->GetDocument( &punkDoc );
		sptrIDocument	sptrD( punkDoc );

		m_lTargetKey = ::GetObjectKey( sptrD );

		if( !punkDoc )
			return false;

		punkDoc->Release();
	}

	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;

	int nMirror = GetArgumentInt( _T("Mirror") );
	
	int nLine	= 0;
	int nCell	= 0;
	int nChromo	= 0;
	if( S_OK != ptrView->GetActiveChromo( &nLine, &nCell, &nChromo ) )
		return false;

	if( nChromo < 0 )
		return false;

	

	IUnknown* punk = 0;
	ptrView->GetChromoObject( nLine, nCell, nChromo, &punk );
	
	if( !punk )	
		return false;

	m_sptrChromo = punk;
	punk->Release();
	punk = 0;

	int nLastMirror;

	ptrView->GetChromoMirror( nLine, nCell, nChromo, &nLastMirror );

	m_bUndoState = nLastMirror != 0;

	if( nMirror == -1 )
	{
		ptrView->SetChromoMirror( nLine, nCell, nChromo, !nLastMirror );
		m_bRedoState = !nLastMirror;
	}
	else
	{
		ptrView->SetChromoMirror( nLine, nCell, nChromo, nMirror );
		m_bRedoState = nMirror != 0;
	}

	IUnknown* punkObj = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	m_sptrList = punkObj;
	punkObj->Release();

	SetModifyFlag( m_punkTarget );

//	LeaveMode();

	return true;	
}

bool CActionSetChromoMirror::DoRedo()
{
	if( m_sptrChromo == 0 )
		return false;

	ROTATEPARAMS rp = {0};

	m_sptrChromo->GetRotateParams( &rp );
	rp.bMirrorH = m_bRedoState;
	m_sptrChromo->SetRotateParams( &rp );

	::FireEvent( m_punkTarget, szEventCarioChange, 0, m_sptrList, 0, 0 );
	
	return true;
}

bool CActionSetChromoMirror::DoUndo()
{
	if( m_sptrChromo == 0 )
		return false;

	ROTATEPARAMS rp = {0};

	m_sptrChromo->GetRotateParams( &rp );
	rp.bMirrorH = m_bUndoState;
	m_sptrChromo->SetRotateParams( &rp );

	::FireEvent( m_punkTarget, szEventCarioChange, 0, m_sptrList, 0, 0 );
	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoMirror::IsAvaible()
{	
	if( !( IsActiveObjectListWasBuild( m_punkTarget ) && IsActiveObjectListHasChromoObject( m_punkTarget ) ) )
		return false;

	if( !IsChecked() )
	{
		ICarioViewPtr ptrView( m_punkTarget );
		if( ptrView == 0 )
			return false;

/*		int nLine	= 0;
		int nCell	= 0;
		int nChromo	= 0;
		ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );
		if( nChromo < 0 )
			return false;
*/
	}

	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;

	CarioViewMode cvm = cvmPreview;
	ptrView->GetMode( &cvm );

	if( !( cvm == cvmPreview || cvm == cvmCellar ) )
		return false;

	BOOL	fShow = true;
	ICarioView2Ptr sptrV = ptrView;
	sptrV->GetShowAllChromoObject( &fShow );

	if( !fShow )
		return false;



	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoMirror::Initialize()
{	
	bool bres = CInteractiveActionBase::Initialize();
	if( !bres )
		return false;

	if( GetArgumentInt( _T("Mirror") ) != -1 )
		Finalize();

	m_hcurActive = AfxGetApp()->LoadCursor( IDC_CURSOR_CROMO_MIRROR );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoMirror::DoLButtonDown( CPoint pt )
{
	if( activate_chromosome( pt ) )
	{
		Finalize();
		GetWindowFromUnknown( m_punkTarget )->UpdateWindow();
		return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoMirror::activate_chromosome( CPoint pt )
{
	ICarioViewPtr	ptrView( m_punkTarget );
	if( ptrView == 0 )return false;

	int	line, cell, pos;
	ptrView->GetCellByPoint( pt, &line, &cell, &pos, FALSE );

	if( pos == -1 )
		return false;

	ptrView->SetActiveChromo( line, cell, pos );

	return true;
}

/*************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////
//
//CActionShowAllChromoBrightness
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionShowAllChromoBrightness, CCmdTargetEx);

// {FCC50296-D773-4196-A186-917A4DA1EDF5}
GUARD_IMPLEMENT_OLECREATE(CActionShowAllChromoBrightness, "ChromosEx.ShowAllChromoBrightness", 
0xfcc50296, 0xd773, 0x4196, 0xa1, 0x86, 0x91, 0x7a, 0x4d, 0xa1, 0xed, 0xf5);

// {91058314-C812-4911-AAB5-B8801D846365}
static const GUID guidShowAllChromoBrightness = 
{ 0x91058314, 0xc812, 0x4911, { 0xaa, 0xb5, 0xb8, 0x80, 0x1d, 0x84, 0x63, 0x65 } };

ACTION_INFO_FULL(CActionShowAllChromoBrightness, IDS_SHOW_CHROMO_ALL_BRIGHTNESS, -1, -1, guidShowAllChromoBrightness );
ACTION_TARGET(CActionShowAllChromoBrightness, szTargetViewSite);


//arguments
ACTION_ARG_LIST(CActionShowAllChromoBrightness)	
	ARG_INT(_T("Show"), -1 )	
END_ACTION_ARG_LIST();

/////////////////////////////////////////////////////////////////////////////
bool CActionShowAllChromoBrightness::Invoke()
{
	ICarioView2Ptr ptrView = m_punkTarget;
	if( ptrView == 0 )
		return false;

	int bShow	= GetArgumentInt( _T("Show") );

	if( bShow == -1 )
		bShow = _next_state();

	ptrView->SetShowChromoAllBrightness( bShow );

	return true;	
}

/////////////////////////////////////////////////////////////////////////////
bool CActionShowAllChromoBrightness::IsAvaible()
{
	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;

	if( !IsActiveObjectListHasChromoObject( m_punkTarget ) )
		return false;

	CarioViewMode cvm = cvmPreview;
	ptrView->GetMode( &cvm );

	if( !( cvm == cvmPreview || cvm == cvmCellar ) )
		return false;


	return true;
}

bool CActionShowAllChromoBrightness::IsChecked()
{
	return _next_state( false );
}

bool  CActionShowAllChromoBrightness::_next_state( bool bChange )
{
	ICarioView2Ptr ptrView = m_punkTarget;
	if( ptrView == 0 )
		return false;

	BOOL	fShow;
	ptrView->GetShowChromoAllBrightness( &fShow );

	return fShow == (bChange==false);
}

/////////////////////////////////////////////////////////////////////////////
//
//CActionShowAllChromoRation
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionShowAllChromoRatio, CCmdTargetEx);

// {9C9A7B7F-20E9-4bd3-9EB0-18EB17ADD028}
GUARD_IMPLEMENT_OLECREATE(CActionShowAllChromoRatio, "ChromosEx.ShowAllChromoRation", 
0x9c9a7b7f, 0x20e9, 0x4bd3, 0x9e, 0xb0, 0x18, 0xeb, 0x17, 0xad, 0xd0, 0x28);

// {079C2288-5782-46dc-8F66-8A6A6B94E6A9}
static const GUID guidShowAllChromoRation = 
{ 0x79c2288, 0x5782, 0x46dc, { 0x8f, 0x66, 0x8a, 0x6a, 0x6b, 0x94, 0xe6, 0xa9 } };


ACTION_INFO_FULL(CActionShowAllChromoRatio, IDS_SHOW_CHROMO_ALL_RATIO, -1, -1, guidShowAllChromoRation );
ACTION_TARGET(CActionShowAllChromoRatio, szTargetViewSite);

//arguments
ACTION_ARG_LIST(CActionShowAllChromoRatio)	
	ARG_INT(_T("Show"), -1 )	
END_ACTION_ARG_LIST();

/////////////////////////////////////////////////////////////////////////////
bool CActionShowAllChromoRatio::Invoke()
{
	ICarioView2Ptr ptrView = m_punkTarget;
	if( ptrView == 0 )
		return false;

	int bShow	= GetArgumentInt( _T("Show") );

	if( bShow == -1 )
		bShow = _next_state();

	ptrView->SetShowChromoAllRatio( bShow );

	return true;	
}

/////////////////////////////////////////////////////////////////////////////
bool CActionShowAllChromoRatio::IsAvaible()
{
	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;

	if( !IsActiveObjectListHasChromoObject( m_punkTarget ) )
		return false;

	CarioViewMode cvm = cvmPreview;
	ptrView->GetMode( &cvm );

	if( !( cvm == cvmPreview || cvm == cvmCellar ) )
		return false;


	return true;
}

bool CActionShowAllChromoRatio::IsChecked()
{
	return _next_state( false );
}

bool  CActionShowAllChromoRatio::_next_state( bool bChange )
{
	ICarioView2Ptr ptrView = m_punkTarget;
	if( ptrView == 0 )
		return false;

	BOOL	fShow;
	ptrView->GetShowChromoAllRatio( &fShow );

	return fShow == (bChange==false);
}


/////////////////////////////////////////////////////////////////////////////
//
//CActionShowAllChromoAmplification
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionShowAllChromoAmplification, CCmdTargetEx);

// {D7A299A4-2695-4fd6-9087-18ABCB70628E}
GUARD_IMPLEMENT_OLECREATE(CActionShowAllChromoAmplification, "ChromosEx.ShowAllChromoAmplification", 
0xd7a299a4, 0x2695, 0x4fd6, 0x90, 0x87, 0x18, 0xab, 0xcb, 0x70, 0x62, 0x8e);

// {90281D18-BD64-4044-8057-98182986AA2C}
static const GUID guidShowAllChromoAmplification = 
{ 0x90281d18, 0xbd64, 0x4044, { 0x80, 0x57, 0x98, 0x18, 0x29, 0x86, 0xaa, 0x2c } };

ACTION_INFO_FULL(CActionShowAllChromoAmplification, IDS_SHOW_CHROMO_ALL_AMPLIFICATION, -1, -1, guidShowAllChromoAmplification );
ACTION_TARGET(CActionShowAllChromoAmplification, szTargetViewSite);

//arguments
ACTION_ARG_LIST(CActionShowAllChromoAmplification)	
	ARG_INT(_T("Show"), -1 )	
END_ACTION_ARG_LIST();

/////////////////////////////////////////////////////////////////////////////
bool CActionShowAllChromoAmplification::Invoke()
{
	ICarioView2Ptr ptrView = m_punkTarget;
	if( ptrView == 0 )
		return false;

	int bShow	= GetArgumentInt( _T("Show") );

	if( bShow == -1 )
		bShow = _next_state();

	ptrView->SetShowChromoAllAmplification( bShow );

	return true;	
}

/////////////////////////////////////////////////////////////////////////////
bool CActionShowAllChromoAmplification::IsAvaible()
{
	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;

	if( !IsActiveObjectListHasChromoObject( m_punkTarget ) )
		return false;


	BOOL	fShow = 0;
	ptrView->GetShowChromoAllIdiogramm( &fShow );

	if( !fShow )
		return false;


	CarioViewMode cvm = cvmPreview;
	ptrView->GetMode( &cvm );

	if( !( cvm == cvmPreview || cvm == cvmCellar ) )
		return false;

	return true;
}

bool CActionShowAllChromoAmplification::IsChecked()
{
	return _next_state( false );
}

bool  CActionShowAllChromoAmplification::_next_state( bool bChange )
{
	ICarioView2Ptr ptrView = m_punkTarget;
	if( ptrView == 0 )
		return false;

	BOOL	fShow;
	ptrView->GetShowChromoAllAmplification( &fShow );

	return fShow == (bChange==false);
}


/////////////////////////////////////////////////////////////////////////////
//
//CActionShowAllChromoObjects
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE( CActionShowAllChromoObjects, CCmdTargetEx);

// {5E700A2F-1E0D-4fbb-AB1B-0A41065BAEAD}
GUARD_IMPLEMENT_OLECREATE( CActionShowAllChromoObjects, "ChromosEx.ShowAllChromoObjects", 
0x5e700a2f, 0x1e0d, 0x4fbb, 0xab, 0x1b, 0xa, 0x41, 0x6, 0x5b, 0xae, 0xad);



// {3F27FA7C-E1A8-49ba-8E7A-312EF55F3DB8}
static const GUID guidShowAllChromoObjects = 
{ 0x3f27fa7c, 0xe1a8, 0x49ba, { 0x8e, 0x7a, 0x31, 0x2e, 0xf5, 0x5f, 0x3d, 0xb8 } };

ACTION_INFO_FULL(CActionShowAllChromoObjects, IDS_SHOW_CHROMO_ALL, -1, -1, guidShowAllChromoObjects );
ACTION_TARGET(CActionShowAllChromoObjects, szTargetViewSite);

//arguments
ACTION_ARG_LIST(CActionShowAllChromoObjects)	
	ARG_INT(_T("Show"), -1 )	
END_ACTION_ARG_LIST();

/////////////////////////////////////////////////////////////////////////////
bool CActionShowAllChromoObjects::Invoke()
{
	ICarioView2Ptr ptrView = m_punkTarget;
	if( ptrView == 0 )
		return false;

	int bShow	= GetArgumentInt( _T("Show") );

	if( bShow == -1 )
		bShow = _next_state();

	ptrView->SetShowAllChromoObject( bShow );

	return true;	
}

/////////////////////////////////////////////////////////////////////////////
bool CActionShowAllChromoObjects::IsAvaible()
{
	ICarioViewPtr ptrView( m_punkTarget );
	if( ptrView == 0 )
		return false;

	if( !IsActiveObjectListHasChromoObject( m_punkTarget ) )
		return false;

	CarioViewMode cvm = cvmPreview;
	ptrView->GetMode( &cvm );

	if( !( cvm == cvmPreview || cvm == cvmCellar ) )
		return false;


	return true;
}

bool CActionShowAllChromoObjects::IsChecked()
{
	return _next_state( false );
}

bool  CActionShowAllChromoObjects::_next_state( bool bChange )
{
	ICarioView2Ptr ptrView = m_punkTarget;
	if( ptrView == 0 )
		return false;

	BOOL	fShow;
	ptrView->GetShowAllChromoObject( &fShow );

	return fShow == (bChange==false);
}
/***************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////
//
//CActionShowAllChromoAddLines
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE( CActionShowAllChromoAddLines, CCmdTargetEx);

// {46040EC6-733F-42e4-9D79-F5DFB0E0FD89}
GUARD_IMPLEMENT_OLECREATE( CActionShowAllChromoAddLines, "ChromosEx.ShowAllChromoAddLines", 
0x46040ec6, 0x733f, 0x42e4, 0x9d, 0x79, 0xf5, 0xdf, 0xb0, 0xe0, 0xfd, 0x89);

// {5846C8A4-4DBC-42e1-9963-ABA12F89A4A6}
static const GUID guidShowAllChromoAddLines = 
{ 0x5846c8a4, 0x4dbc, 0x42e1, { 0x99, 0x63, 0xab, 0xa1, 0x2f, 0x89, 0xa4, 0xa6 } };

ACTION_INFO_FULL(CActionShowAllChromoAddLines, IDS_SHOW_CHROMO_ADD_LINES, -1, -1, guidShowAllChromoAddLines );
ACTION_TARGET(CActionShowAllChromoAddLines, szTargetViewSite);

//arguments
ACTION_ARG_LIST(CActionShowAllChromoAddLines)	
	ARG_INT(_T("Show"), -1 )	
END_ACTION_ARG_LIST();

/////////////////////////////////////////////////////////////////////////////
bool CActionShowAllChromoAddLines::Invoke()
{
	ICarioView2Ptr ptrView = m_punkTarget;
	if( ptrView == 0 )
		return false;

	int bShow	= GetArgumentInt( _T("Show") );

	if( bShow == -1 )
		bShow = _next_state();

	ptrView->SetShowAllChromoAddLines( bShow );

	return true;	
}

/////////////////////////////////////////////////////////////////////////////
bool CActionShowAllChromoAddLines::IsAvaible()
{
	ICarioView2Ptr ptrView = m_punkTarget;

	if( ptrView == 0 )
		return false;

	if( ptrView->IsEmpty() == S_OK )
		return false;

	CarioViewMode cvm = cvmPreview;
	ptrView->GetMode( &cvm );

	if( cvm != cvmCellar )
		return false;

	return true;
}

bool CActionShowAllChromoAddLines::IsChecked()
{
	return _next_state( false );
}

bool  CActionShowAllChromoAddLines::_next_state( bool bChange )
{
	ICarioView2Ptr ptrView = m_punkTarget;
	if( ptrView == 0 )
		return false;

	BOOL	fShow;
	ptrView->GetShowAllChromoAddLines( &fShow );

	return fShow == (bChange==false);
}
/***************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////
//
//CActionAddToAnalize
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionAddToAnalize, CCmdTargetEx);


// {A328C73B-6CA2-451a-B04D-0500B1821F6D}
GUARD_IMPLEMENT_OLECREATE(CActionAddToAnalize, "ChromosEx.AddToAnalize", 
0xa328c73b, 0x6ca2, 0x451a, 0xb0, 0x4d, 0x5, 0x0, 0xb1, 0x82, 0x1f, 0x6d);

// {22F7CECF-9D1F-4c99-9406-9F15B52267B3}
static const GUID guidAddToAnalize = 
{ 0x22f7cecf, 0x9d1f, 0x4c99, { 0x94, 0x6, 0x9f, 0x15, 0xb5, 0x22, 0x67, 0xb3 } };


ACTION_INFO_FULL(CActionAddToAnalize, IDS_ADD_TO_ANALIZE, -1, -1, guidAddToAnalize );
ACTION_TARGET(CActionAddToAnalize, szTargetViewSite);

ACTION_ARG_LIST( CActionAddToAnalize )	
	ARG_INT(_T("AddAfter"), -1 )	
END_ACTION_ARG_LIST();

/////////////////////////////////////////////////////////////////////////////
bool CActionAddToAnalize::Invoke()
{
	{
		IUnknown	*punkDoc = 0;
		sptrIView	sptrV( m_punkTarget );
		sptrV->GetDocument( &punkDoc );
		sptrIDocument	sptrD( punkDoc );

		m_lTargetKey = ::GetObjectKey( sptrD );

		if( !punkDoc )
			return false;

		punkDoc->Release();
	}

	int nAddToPos  = GetArgumentInt( _T("AddAfter") );

	IDataContext3Ptr ptrC = m_punkTarget;

	IUnknown *punkList = 0;
	ptrC->GetActiveObject( _bstr_t( szTypeObjectList ), &punkList );

	INamedDataObject2Ptr	ptrList = punkList;

	INamedDataPtr ptrData = punkList;
	
	if( punkList )
		punkList->Release();

	if( ptrData == 0 )
		return false;

	POSITION lPos = 0;
	ptrList->GetActiveChild( &lPos );

	IUnknown *punkChild = 0;
	ptrList->GetNextChild( &lPos, &punkChild );

	ptrData->SetupSection( _bstr_t( CARIO_ANALIZE ) );				
	ptrData->SetValue( _bstr_t( CARIO_COUNT_PREFIX ), _variant_t( long( 1 ) ) );
	
	CString strSect,tmp;
	strSect.Format( "%s\\%s%d", CARIO_ANALIZE, CARIO_LINE_PREFIX, int(0) );

	ptrData->SetupSection( _bstr_t( strSect ) );				
	ptrData->SetValue( _bstr_t( CARIO_CELLAR_PREFIX), _variant_t( long( 1 ) ) );

	ptrData->SetupSection( _bstr_t( strSect + "\\" + CARIO_CELLS_PREFIX ) );				
	ptrData->SetValue( _bstr_t( CARIO_COUNT_PREFIX ), _variant_t( long( 1 ) ) );

	tmp.Format( "\\%s\\%s%d", CARIO_CELLS_PREFIX, CARIO_CELL_PREFIX, int(0) );
	strSect += tmp;
	
	m_section = strSect;
	tmp.Format( "\\%s", CARIO_CHROMOS_PREFIX );
	strSect += tmp;

	m_section += tmp;
	m_undoState.Load( ptrData, m_section);

	CString strKey = _get_key( punkChild );

	if( punkChild )
		punkChild->Release();

	long lCount = ::GetValueInt( ptrData, strSect, CARIO_COUNT_PREFIX, 0 );

	BOOL	fShowR = -1, fShowA = -1, fShowB = -1, fShowC = -1, fShowD = -1;

	for( long i = 0; i < lCount; i++ )
	{
		CString strSect2;
		strSect2.Format( "\\%s%d", CARIO_CHROMO_PREFIX, i );
		
		CString key = ::GetValueString( ptrData, strSect + strSect2, CARIO_GUID_CHROMO_PREFIX, "" );

		if( fShowR == -1 )
			fShowR = ::GetValueInt( ptrData, strSect + strSect2, CARIO_SHOW_CGHRATIO, 0 );

		if( fShowC == -1 )
			fShowC = ::GetValueInt( ptrData, strSect + strSect2, CARIO_SHOW_OBJECT, 0 );

		if( fShowB == -1 )
			fShowB = ::GetValueInt( ptrData, strSect + strSect2, CARIO_SHOW_CGHBRIGHT, 0 );

		if( fShowA == -1 )
			fShowA = ::GetValueInt( ptrData, strSect + strSect2, CARIO_SHOW_CGHAMPL, 0 );

		if( fShowD == -1 )
			fShowD = ::GetValueInt( ptrData, strSect + strSect2, CARIO_EDITABLE_IDIO, 0 );

		if( key == strKey ) 
			return false;
	}

	ptrData->SetupSection( _bstr_t( strSect ) );				
	ptrData->SetValue( _bstr_t( CARIO_COUNT_PREFIX ), _variant_t( long( lCount + 1 ) ) );

	tmp.Format( "\\%s%d", CARIO_CHROMO_PREFIX, lCount );
	strSect += tmp;

	ptrData->SetupSection( _bstr_t( strSect ) );				

	ptrData->SetValue( _bstr_t( CARIO_GUID_CHROMO_PREFIX ), _variant_t( _bstr_t( strKey ) ) );


	if( fShowR != -1 )
		ptrData->SetValue( _bstr_t( CARIO_SHOW_CGHRATIO ), _variant_t( long( fShowR ) ) );

	if( fShowC != -1 )
		ptrData->SetValue( _bstr_t( CARIO_SHOW_OBJECT ),  _variant_t( long( fShowC ) ) );

	if( fShowB != -1 )
		ptrData->SetValue( _bstr_t( CARIO_SHOW_CGHBRIGHT ), _variant_t( long( fShowB ) ) );

	if( fShowA != -1 )
		ptrData->SetValue( _bstr_t( CARIO_SHOW_CGHAMPL ), _variant_t( long( fShowA ) ) );

	if( fShowD != -1 )
		ptrData->SetValue( _bstr_t( CARIO_EDITABLE_IDIO ), _variant_t( long( fShowD ) ) );

	m_redoState.Load( ptrData, m_section );

	if( nAddToPos != -1 )
	{
		CNDataItem item = m_redoState[lCount];
		m_redoState.Add( item, nAddToPos );
		m_redoState.Erase( lCount + 1);
		m_redoState.Store( ptrData, m_section );
	}

	IViewPtr ptrV = m_punkTarget;
	if( ptrV != 0 )
	{
		IUnknown* punkDoc = 0;
		ptrV->GetDocument( &punkDoc );

		if( punkDoc )
		{
			int mode = cvmCellar;
			::FireEvent( punkDoc, szEventCarioChange, 0, punkList, &mode, nAddToPos == -1 ? lCount : nAddToPos );
			punkDoc->Release();
		}
	}

	return true;	
}

bool CActionAddToAnalize::DoRedo()
{
	return _refresh_data( false );
}

bool CActionAddToAnalize::DoUndo()
{
	return _refresh_data( true );
}

bool CActionAddToAnalize::_refresh_data( bool bUndo )
{
	IDataContext3Ptr ptrC = m_punkTarget;

	IUnknown *punkList = 0;
	ptrC->GetActiveObject( _bstr_t( szTypeObjectList ), &punkList );

	INamedDataObject2Ptr	ptrList = punkList;

	INamedDataPtr ptrData = punkList;
	
	if( punkList )
		punkList->Release();

	if( ptrData == 0 )
		return false;

	if( bUndo )
		m_undoState.Store( ptrData, m_section );
	else
		m_redoState.Store( ptrData, m_section );

	IUnknown* punkObj = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	::FireEvent( m_punkTarget, szEventCarioChange, 0, punkObj, 0, 0 );

	if( punkObj )
		punkObj->Release();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionAddToAnalize::IsAvaible()
{
	bool bEnable = GetValueInt( GetAppUnknown(), "\\Karyo", "HiddenAdd", 0 ) != 0;

	if( bEnable )
		return true;

	IDataContext3Ptr ptrC = m_punkTarget;

	if( ptrC == 0 )
		return 0;


	IUnknown *punkList = 0;
	ptrC->GetActiveObject( _bstr_t( szTypeObjectList ), &punkList );

	if( !punkList )
		return false;

	INamedDataObject2Ptr	ptrList = punkList;

	if( punkList )
		punkList->Release();

	INamedDataPtr ptrData = punkList;
	
	if( ptrData == 0 )
		return false;

	if( ptrList == 0 )
		return 0;

	POSITION lPos = 0;
	ptrList->GetActiveChild( &lPos );

	if( !lPos )
		return false;
/*
	{
		IUnknown *punkChild = 0;
		ptrList->GetNextChild( &lPos, &punkChild );

		CString strSect,tmp;
		strSect.Format( "%s\\%s%d", CARIO_ANALIZE, CARIO_LINE_PREFIX, int(0) );
		

		tmp.Format( "\\%s\\%s%d", CARIO_CELLS_PREFIX, CARIO_CELL_PREFIX, int(0) );
		strSect += tmp;
		
		tmp.Format( "\\%s", CARIO_CHROMOS_PREFIX );
		strSect += tmp;

		CNDataCell cell;
		cell.Load( ptrData, strSect );

		CString strKey = _get_key( punkChild );

		bool bFound = cell.Find( _variant_t( strKey ) );

		if( punkChild )
			punkChild->Release();

		if( bFound )
			return false;
	}
*/


	ICarioViewPtr    ptrView = m_punkTarget;

	if( ptrView != 0)
	{
		CarioViewMode ViewMode;
		ptrView->GetMode( &ViewMode );

		if( ViewMode == cvmCellar )
			return false;
	}

	IViewPtr sptrView = m_punkTarget;
	
	IUnknown *punkDoc = 0;
	sptrView->GetDocument( &punkDoc );

	if( punkDoc )
	{
		IDocumentSitePtr sptrDoc = punkDoc;
		punkDoc->Release();

		TPOS lPos = 0;
		sptrDoc->GetFirstViewPosition( &lPos );
		while( lPos )
		{
			IUnknown *punkView = 0;
			sptrDoc->GetNextView( &punkView, &lPos );

			if( punkView != 0 )
				punkView->Release();

			ICarioViewPtr   ptrView = punkView;

			if( ptrView != 0)
			{
				CarioViewMode ViewMode;
				ptrView->GetMode( &ViewMode );

				if( ViewMode == cvmCellar )
					return true;
			}
		}
	}

	return false;
}

CString CActionAddToAnalize::_get_key( IUnknown *punk )
{
	BSTR bstr = 0;
	StringFromCLSID(::GetObjectKey( punk ) , &bstr );

	CString strKey = bstr;

	::CoTaskMemFree( bstr );	
	bstr = 0;

	return strKey;
}
/***************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////
//
//CActionAddToAnalize2
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionAddToAnalize2, CCmdTargetEx);

// {721DBE4D-3BC0-485e-8DCF-D1ED6789F7C7}
GUARD_IMPLEMENT_OLECREATE(CActionAddToAnalize2, "ChromosEx.AddToAnalize2", 
0x721dbe4d, 0x3bc0, 0x485e, 0x8d, 0xcf, 0xd1, 0xed, 0x67, 0x89, 0xf7, 0xc7);

// {84C49C83-795C-48b8-9940-6C9BA71ACFCE}
static const GUID guidAddToAnalize2 = 
{ 0x84c49c83, 0x795c, 0x48b8, { 0x99, 0x40, 0x6c, 0x9b, 0xa7, 0x1a, 0xcf, 0xce } };


ACTION_INFO_FULL(CActionAddToAnalize2, IDS_ADD_TO_ANALIZE2, -1, -1, guidAddToAnalize2 );
ACTION_TARGET(CActionAddToAnalize2, szTargetViewSite);

ACTION_ARG_LIST( CActionAddToAnalize2 )	
END_ACTION_ARG_LIST();

/////////////////////////////////////////////////////////////////////////////
bool CActionAddToAnalize2::Invoke()
{
	{
		IUnknown	*punkDoc = 0;
		sptrIView	sptrV( m_punkTarget );
		sptrV->GetDocument( &punkDoc );
		sptrIDocument	sptrD( punkDoc );

		m_lTargetKey = ::GetObjectKey( sptrD );

		if( !punkDoc )
			return false;

		punkDoc->Release();
	}

	ICarioView2Ptr sptrView = m_punkTarget;

	int lActiveLine = -1, lActiveCell = -1, lActiveChromo = -1;

	sptrView->GetActiveChromo( &lActiveLine, &lActiveCell, &lActiveChromo );

	IDataContext3Ptr ptrC = m_punkTarget;

	IUnknown *punkList = 0;
	ptrC->GetActiveObject( _bstr_t( szTypeObjectList ), &punkList );

	INamedDataObject2Ptr	ptrList = punkList;

	INamedDataPtr ptrData = punkList;
	
	if( punkList )
		punkList->Release();

	if( ptrData == 0 )
		return false;

	POSITION lPos = 0;
	ptrList->GetActiveChild( &lPos );

	IUnknown *punkChild = 0;
	ptrList->GetNextChild( &lPos, &punkChild );

	ptrData->SetupSection( _bstr_t( CARIO_ANALIZE ) );				
	ptrData->SetValue( _bstr_t( CARIO_COUNT_PREFIX ), _variant_t( long( 1 ) ) );
	
	CString strSect,tmp;
	strSect.Format( "%s\\%s%d", CARIO_ANALIZE, CARIO_LINE_PREFIX, int(0) );

	ptrData->SetupSection( _bstr_t( strSect ) );				
	ptrData->SetValue( _bstr_t( CARIO_CELLAR_PREFIX), _variant_t( long( 1 ) ) );

	ptrData->SetupSection( _bstr_t( strSect + "\\" + CARIO_CELLS_PREFIX ) );				
	ptrData->SetValue( _bstr_t( CARIO_COUNT_PREFIX ), _variant_t( long( 1 ) ) );

	tmp.Format( "\\%s\\%s%d", CARIO_CELLS_PREFIX, CARIO_CELL_PREFIX, int(0) );
	strSect += tmp;
	
	m_section = strSect;
	tmp.Format( "\\%s", CARIO_CHROMOS_PREFIX );
	strSect += tmp;

	m_section += tmp;
	m_undoState.Load( ptrData, m_section);

/*
	CString strKey = _get_key( punkChild );

	if( punkChild )
		punkChild->Release();

	ptrData->SetupSection( _bstr_t( strSect ) );				
	ptrData->SetValue( _bstr_t( CARIO_COUNT_PREFIX ), _variant_t( long( lCount + 1 ) ) );

	tmp.Format( "\\%s%d", CARIO_CHROMO_PREFIX, lCount );
	strSect += tmp;

	ptrData->SetupSection( _bstr_t( strSect ) );				

	ptrData->SetValue( _bstr_t( CARIO_GUID_CHROMO_PREFIX ), _variant_t( _bstr_t( strKey ) ) );

  /*
	if( fShowR != -1 )
		ptrData->SetValue( _bstr_t( CARIO_SHOW_CGHRATIO ), _variant_t( long( fShowR ) ) );

	if( fShowC != -1 )
		ptrData->SetValue( _bstr_t( CARIO_SHOW_OBJECT ),  _variant_t( long( fShowC ) ) );

	if( fShowB != -1 )
		ptrData->SetValue( _bstr_t( CARIO_SHOW_CGHBRIGHT ), _variant_t( long( fShowB ) ) );

	if( fShowA != -1 )
		ptrData->SetValue( _bstr_t( CARIO_SHOW_CGHAMPL ), _variant_t( long( fShowA ) ) );

	if( fShowD != -1 )
		ptrData->SetValue( _bstr_t( CARIO_EDITABLE_IDIO ), _variant_t( long( fShowD ) ) );

/*
	m_redoState.Load( ptrData, m_section );

	if( nAddToPos != -1 )
	{
		CNDataItem item = m_redoState[lCount];
		m_redoState.Add( item, nAddToPos );
		m_redoState.Erase( lCount + 1);
		m_redoState.Store( ptrData, m_section );
	}

	IViewPtr ptrV = m_punkTarget;
	if( ptrV != 0 )
	{
		IUnknown* punkDoc = 0;
		ptrV->GetDocument( &punkDoc );

		if( punkDoc )
		{
			int mode = cvmCellar;
			::FireEvent( punkDoc, szEventCarioChange, 0, punkList, &mode, nAddToPos == -1 ? lCount : nAddToPos );
			punkDoc->Release();
		}
	}
 */
	return true;	
}

bool CActionAddToAnalize2::DoRedo()
{
	return _refresh_data( false );
}

bool CActionAddToAnalize2::DoUndo()
{
	return _refresh_data( true );
}

bool CActionAddToAnalize2::_refresh_data( bool bUndo )
{
	IDataContext3Ptr ptrC = m_punkTarget;

	IUnknown *punkList = 0;
	ptrC->GetActiveObject( _bstr_t( szTypeObjectList ), &punkList );

	INamedDataObject2Ptr	ptrList = punkList;

	INamedDataPtr ptrData = punkList;
	
	if( punkList )
		punkList->Release();

	if( ptrData == 0 )
		return false;

	if( bUndo )
		m_undoState.Store( ptrData, m_section );
	else
		m_redoState.Store( ptrData, m_section );

	IUnknown* punkObj = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	::FireEvent( m_punkTarget, szEventCarioChange, 0, punkObj, 0, 0 );

	if( punkObj )
		punkObj->Release();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CActionAddToAnalize2::IsAvaible()
{
	IDataContext3Ptr ptrC = m_punkTarget;

	if( ptrC == 0 )
		return 0;


	IUnknown *punkList = 0;
	ptrC->GetActiveObject( _bstr_t( szTypeObjectList ), &punkList );

	if( !punkList )
		return false;

	INamedDataObject2Ptr	ptrList = punkList;

	if( punkList )
		punkList->Release();

	INamedDataPtr ptrData = punkList;
	
	if( ptrData == 0 )
		return false;

	if( ptrList == 0 )
		return 0;

	POSITION lPos = 0;
	ptrList->GetActiveChild( &lPos );

	if( !lPos )
		return false;
/*
	{
		IUnknown *punkChild = 0;
		ptrList->GetNextChild( &lPos, &punkChild );

		CString strSect,tmp;
		strSect.Format( "%s\\%s%d", CARIO_ANALIZE, CARIO_LINE_PREFIX, int(0) );
		

		tmp.Format( "\\%s\\%s%d", CARIO_CELLS_PREFIX, CARIO_CELL_PREFIX, int(0) );
		strSect += tmp;
		
		tmp.Format( "\\%s", CARIO_CHROMOS_PREFIX );
		strSect += tmp;

		CNDataCell cell;
		cell.Load( ptrData, strSect );

		CString strKey = _get_key( punkChild );

		bool bFound = cell.Find( _variant_t( strKey ) );

		if( punkChild )
			punkChild->Release();

		if( bFound )
			return false;
	}
*/


	ICarioViewPtr    ptrView = m_punkTarget;

	if( ptrView != 0)
	{
		CarioViewMode ViewMode;
		ptrView->GetMode( &ViewMode );

		if( ViewMode == cvmCellar )
			return false;
	}

	IViewPtr sptrView = m_punkTarget;
	
	IUnknown *punkDoc = 0;
	sptrView->GetDocument( &punkDoc );

	if( punkDoc )
	{
		IDocumentSitePtr sptrDoc = punkDoc;
		punkDoc->Release();

		TPOS lPos = 0;
		sptrDoc->GetFirstViewPosition( &lPos );
		while( lPos )
		{
			IUnknown *punkView = 0;
			sptrDoc->GetNextView( &punkView, &lPos );

			if( punkView != 0 )
				punkView->Release();

			ICarioViewPtr   ptrView = punkView;

			if( ptrView != 0)
			{
				CarioViewMode ViewMode;
				ptrView->GetMode( &ViewMode );

				if( ViewMode == cvmCellar )
					return true;
			}
		}
	}

	return false;
}

CString CActionAddToAnalize2::_get_key( IUnknown *punk )
{
	BSTR bstr = 0;
	StringFromCLSID(::GetObjectKey( punk ) , &bstr );

	CString strKey = bstr;

	::CoTaskMemFree( bstr );	
	bstr = 0;

	return strKey;
}
/***************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////
//
//CActionSetChromoClass
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionSetChromoClass, CCmdTargetEx);

// {2F9E6449-4DB8-40d1-AA37-5A0D53D21A4D}
GUARD_IMPLEMENT_OLECREATE(CActionSetChromoClass, "ChromosEx.SetChromoClass", 
0x2f9e6449, 0x4db8, 0x40d1, 0xaa, 0x37, 0x5a, 0xd, 0x53, 0xd2, 0x1a, 0x4d);

// {A836035E-A4F1-44db-923B-08711DE2449F}
static const GUID guidSetChromoClass = 
{ 0xa836035e, 0xa4f1, 0x44db, { 0x92, 0x3b, 0x8, 0x71, 0x1d, 0xe2, 0x44, 0x9f } };

ACTION_INFO_FULL(CActionSetChromoClass, IDS_SET_CHROMO_CLASS, -1, -1, guidSetChromoClass );
ACTION_TARGET(CActionSetChromoClass, szTargetViewSite);

//arguments
ACTION_ARG_LIST( CActionSetChromoClass )	
	ARG_INT(_T("OldLine"), -1 )	
	ARG_INT(_T("OldCell"), -1 )	
	ARG_INT(_T("OldChromo"), -1 )	

	ARG_INT(_T("NewLine"), -1 )	
	ARG_INT(_T("NewCell"), -1 )	
	ARG_INT(_T("NewChromo"), -1 )	
END_ACTION_ARG_LIST();

/////////////////////////////////////////////////////////////////////////////
bool CActionSetChromoClass::Invoke()
{
	{
		IUnknown	*punkDoc = 0;
		sptrIView	sptrV( m_punkTarget );
		sptrV->GetDocument( &punkDoc );
		sptrIDocument	sptrD( punkDoc );

		m_lTargetKey = ::GetObjectKey( sptrD );

		if( !punkDoc )
			return false;

		punkDoc->Release();
	}

	int l  = GetArgumentInt( _T("OldLine") );
	int c  = GetArgumentInt( _T("OldCell") );
	int ch = GetArgumentInt( _T("OldChromo") );

	ICarioView2Ptr ptrView = m_punkTarget;
	if( ptrView == 0 )
		return 0;

	char *update_p = new char[4096];
	ptrView->GetChromoPath( l, c, ch, &update_p );


	m_sOldPath = CString( update_p );

	ptrView->GetCellClass( l, c, &n_OldClass );

	l  = GetArgumentInt( _T("NewLine") );
	c  = GetArgumentInt( _T("NewCell") );
	ch = GetArgumentInt( _T("NewChromo") );
	ptrView->GetChromoPath( l, c, ch, &update_p );

	m_sNewPath = CString( update_p );
	ptrView->GetCellClass( l, c, &n_NewClass );

	delete []update_p;

	IUnknown *punkList = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	m_sptrList = punkList;

	
	INamedDataObject2Ptr ptrNDO = m_sptrList;

	if( ptrNDO == 0 )
		return false;

	ptrNDO->SetModifiedFlag( TRUE );

	punkList->Release();
	return true;	
}

bool CActionSetChromoClass::IsAvaible()
{
	return true;
}

bool CActionSetChromoClass::DoUndo()
{
	_swap_section( m_sNewPath, m_sOldPath, n_OldClass );

	return true;
}

bool CActionSetChromoClass::DoRedo()
{
	_swap_section( m_sOldPath, m_sNewPath, n_NewClass );

	return true;
}

void CActionSetChromoClass::_swap_section( CString strFrom, CString strTo, int nClass )
{
	INamedDataObject2Ptr	ptrList = m_sptrList;
	INamedDataPtr ptrData = m_sptrList;

	if( ptrData == 0 )
		return;
	
	ptrData->SetupSection( _bstr_t( strFrom ) );

	GUID guid;
	_variant_t var;
	ptrData->GetValue( _bstr_t( CARIO_GUID_CHROMO_PREFIX ), &var );
	if( var.vt == VT_BSTR )
		CLSIDFromString( var.bstrVal, &guid );

	CString strA = strFrom.Left( strFrom.ReverseFind( '\\' ) );
	CString strB = strTo.Left( strTo.ReverseFind( '\\' ) );

	CNDataCell cellFrom, cellTo;

	cellFrom.Load( ptrData, strA );
	cellTo.Load( ptrData, strB );

	if( strA != strB )
	{
		CString from;
		{
			int nSz = strFrom.GetLength();
			for( int i = nSz - 1; i >= 0; i-- )
			{
				char ch = strFrom[i];
				if( ch >= '0' && ch <= '9' )
					from.Insert( 0, ch );
				else
					break;
			}
		}


		CString to;
		{
			int nSz = strTo.GetLength();
			for( int i = nSz - 1; i >= 0; i-- )
			{
				char ch = strTo[i];
				if( ch >= '0' && ch <= '9' )
					to.Insert( 0, ch );
				else
					break;
			}
		}

		int nFrom = atoi( from );
		int nTo = atoi( to );

		cellTo.Move( cellFrom, nFrom, nTo );

		cellFrom.Store( ptrData, strA );
		cellTo.Store( ptrData, strB );
	}
	else
	{
		
		CString from;
		{
			int nSz = strFrom.GetLength();
			for( int i = nSz - 1; i >= 0; i-- )
			{
				char ch = strFrom[i];
				if( ch >= '0' && ch <= '9' )
					from.Insert( 0, ch );
				else
					break;
			}
		}


		CString to;
		{
			int nSz = strTo.GetLength();
			for( int i = nSz - 1; i >= 0; i-- )
			{
				char ch = strTo[i];
				if( ch >= '0' && ch <= '9' )
					to.Insert( 0, ch );
				else
					break;
			}
		}

		int nFrom = atoi( from );
		int nTo = atoi( to );

		cellTo.Erase( nFrom );
		cellTo.Move( cellFrom, nFrom, nTo );

		cellTo.Store( ptrData, strA );
	}

	IUnknownPtr ptrObj;
	POSITION	lpos = 0;
	ptrList->GetFirstChildPosition( &lpos );
	while( lpos )
	{
		IUnknown	*punk = 0;
		ptrList->GetNextChild( &lpos, &punk );

		if( punk )
		{
			punk->Release();

			if( ::GetObjectKey( punk ) == guid )
			{
				ICalcObjectPtr ptrCalc = punk;

				set_object_class(ptrCalc, nClass);

				ptrObj = punk;

				break;
			}
		}
	}

	::FireEvent( m_punkTarget, szEventCarioChange, 0, m_sptrList, 0, 0 );
	::FireEventNotify( m_punkTarget, szEventChangeObjectList, m_sptrList, ptrObj, cncActivate );
}


/***************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////
//
//CActionSetEditableIdio
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionSetEditableIdio, CCmdTargetEx);

// {1A730D28-36B3-47bf-803B-F831802B6839}
GUARD_IMPLEMENT_OLECREATE( CActionSetEditableIdio, "ChromosEx.ShowEditableIdio",
0x1a730d28, 0x36b3, 0x47bf, 0x80, 0x3b, 0xf8, 0x31, 0x80, 0x2b, 0x68, 0x39);

// {43892E2D-4484-4453-BF26-4A12A2F7BC16}
static const GUID guidSetEditobleIdio = 
{ 0x43892e2d, 0x4484, 0x4453, { 0xbf, 0x26, 0x4a, 0x12, 0xa2, 0xf7, 0xbc, 0x16 } };

ACTION_INFO_FULL(CActionSetEditableIdio, IDS_SET_EDITABLE_IDIO, -1, -1, guidSetEditobleIdio );
ACTION_TARGET(CActionSetEditableIdio, szTargetViewSite);

//arguments
ACTION_ARG_LIST( CActionSetEditableIdio )
	ARG_INT(_T("State"), -1 )	
END_ACTION_ARG_LIST();

bool CActionSetEditableIdio::Invoke()
{
	ICarioView2Ptr ptrView = m_punkTarget;

	int bShow = GetArgumentInt( _T( "State" ) );

	if( bShow != -1 )
		ptrView->SetShowAllEditableIdio( BOOL( bShow != 0 ) );
	else
	{
		BOOL bShow = 0;
		ptrView->GetShowAllEditableIdio( &bShow );
		ptrView->SetShowAllEditableIdio( !bShow );
	}

	return true;
}

bool CActionSetEditableIdio::IsAvaible()
{
	ICarioView2Ptr ptrView = m_punkTarget;

	if( ptrView == 0 )
		return false;


	if( ptrView->IsEmpty() == S_OK )
		return false;

	CarioViewMode cvm = cvmPreview;
	ptrView->GetMode( &cvm );

	if( cvm != cvmCellar )
		return false;

	return true;
}

bool CActionSetEditableIdio::IsChecked()
{
	ICarioView2Ptr ptrView = m_punkTarget;

	if( ptrView == 0 )
		return false;

	BOOL bShow = 0;
	ptrView->GetShowAllEditableIdio( &bShow );
	return bShow != 0;
}
/***************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////
//
//CActionRotateIdio
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionRotateIdio, CCmdTargetEx);

// {8F774912-8E61-48df-B673-1B44FCAE6542}
GUARD_IMPLEMENT_OLECREATE( CActionRotateIdio, "ChromosEx.RotateIdio",
0x8f774912, 0x8e61, 0x48df, 0xb6, 0x73, 0x1b, 0x44, 0xfc, 0xae, 0x65, 0x42);

// {BCA2B1A9-E29B-4ad5-9EFF-D048760AD388}
static const GUID guidRotateIdio = 
{ 0xbca2b1a9, 0xe29b, 0x4ad5, { 0x9e, 0xff, 0xd0, 0x48, 0x76, 0xa, 0xd3, 0x88 } };


ACTION_INFO_FULL(CActionRotateIdio, IDS_ROTATE_EDITABLE_IDIO, -1, -1, guidRotateIdio );
ACTION_TARGET(CActionRotateIdio, szTargetViewSite);

bool CActionRotateIdio::Invoke()
{
	ICarioView2Ptr ptrView = m_punkTarget;

	int nLine = -1, nCell = -1, nChromo = -1;
	ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );

	if( nLine == -1 || nCell == -1 || nChromo == -1 )
		return false;

	IUnknown *punkIdio = 0;
	ptrView->GetIdioObject( nLine, nCell, nChromo, idtEditable, &punkIdio );

	IIdioBendsPtr sptrBend = punkIdio;

	long lCount = 0;
	sptrBend->GetBengsCount( &lCount );

	double fPrev = 0;
	double fPrev2 = 1;


	IIdiogramPtr sptrI = punkIdio;
	double fLen, fCentr; 
	sptrI->get_params( &fLen, &fCentr );

	double lPrev = 0;
	for( long i = 0; i < lCount / 2; i++ )
	{
		double pos1 = 0, pos2 = 0;
		double _pos1 = 0, _pos2 = 0;
		
		sptrBend->GetBendRange( i, &pos1, &pos2 );
		sptrBend->GetBendRange( lCount - i - 1, &_pos1, &_pos2 );

		sptrBend->SetBendRange( i, fPrev, fPrev + _pos2 - _pos1 );
		sptrBend->SetBendRange( lCount - i - 1, fPrev2 - (pos2 - pos1), fPrev2 );

		fPrev += _pos2 - _pos1;
		fPrev2 -= pos2 - pos1;

	}

	if( lCount % 2 )
	{
		double pos1 = 0, pos2 = 0;
		
		sptrBend->GetBendRange( lCount / 2, &pos1, &pos2 );
		sptrBend->SetBendRange( lCount / 2, 1 - pos2, 1 - pos1 );
	}

	for( long i = 0; i < lCount / 2; i++ )
	{
		int color = 0;
		_bstr_t sz_number;
		int flags = 0;

		int _color = 0;
		_bstr_t _sz_number;
		int _flags = 0;

		sptrBend->GetBendParams( i, 0, 0, &color, sz_number.GetAddress(), &flags );
		sptrBend->GetBendParams( lCount - i - 1, 0, 0, &_color, _sz_number.GetAddress(), &_flags );

		sptrBend->SetBendFillStyle( i, _color );
		sptrBend->SetBendFillStyle( lCount - i - 1, color );

		sptrBend->SetBendNumber( i, _sz_number );
		sptrBend->SetBendNumber( lCount - i - 1, sz_number );

		sptrBend->SetBendFlag( i, _flags );
		sptrBend->SetBendFlag( lCount - i - 1, flags );
	}

	for( long i = 0; i < lCount - 1; i++ )
	{
		int flags = 0;
		sptrBend->GetBendParams( i, 0, 0, 0, 0, &flags );

		if( ( flags & IDIB_CENTROMERE ) == IDIB_CENTROMERE )
		{
			int _flags = 0;
			sptrBend->GetBendFlag( i + 1, &_flags );

			sptrBend->SetBendFlag( i, flags & ~IDIB_CENTROMERE );
			sptrBend->SetBendFlag( i + 1, _flags | IDIB_CENTROMERE  );
			break;
		}
	}


	{
		ICarioView2Ptr sptrV = m_punkTarget;

		BOOL bState = 0;
		sptrV->GetShowAllChromoAddLines( &bState );
		sptrV->SetShowAllChromoAddLines( bState );

		CWnd *pwnd = GetWindowFromUnknown( m_punkTarget );
		if( pwnd )
			pwnd->InvalidateRect( 0 );
	}

	return true;
}

bool CActionRotateIdio::IsAvaible()
{
	ICarioView2Ptr ptrView = m_punkTarget;

	if( ptrView == 0 )
		return false;

	if( ptrView->IsEmpty() == S_OK )
		return false;

	int nLine = -1, nCell = -1, nChromo = -1;
	ptrView->GetActiveChromo( &nLine, &nCell, &nChromo );

	if( nLine == -1 || nCell == -1 || nChromo == -1 )
		return false;

	CarioViewMode cvm = cvmPreview;
	ptrView->GetMode( &cvm );

	if( cvm != cvmCellar )
		return false;

	IUnknown *punkIdio = 0;
	ptrView->GetIdioObject( nLine, nCell, nChromo, idtEditable, &punkIdio );

	if( punkIdio )
	{
		punkIdio->Release();
		
		BOOL bShow = 0;
		ptrView->GetShowAllEditableIdio( &bShow );

		if( !bShow )
			return false;

		return true;
	}

	return false;
}

/***************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////
//
//CActionAddToAnalize3
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionAddToAnalize3, CCmdTargetEx);

// {AB87FDBE-9900-4d19-A7DB-F5BB57BACE6E}
GUARD_IMPLEMENT_OLECREATE(CActionAddToAnalize3, "ChromosEx.AddToAnalize3", 
0xab87fdbe, 0x9900, 0x4d19, 0xa7, 0xdb, 0xf5, 0xbb, 0x57, 0xba, 0xce, 0x6e);

// {91227C92-54B6-40c5-A3D2-AE337EBDA3EB}
static const GUID guidAddToAnalize3 = 
{ 0x91227c92, 0x54b6, 0x40c5, { 0xa3, 0xd2, 0xae, 0x33, 0x7e, 0xbd, 0xa3, 0xeb } };

ACTION_INFO_FULL(CActionAddToAnalize3, IDS_ADD_TO_ANALIZE3, -1, -1, guidAddToAnalize3 );
ACTION_TARGET(CActionAddToAnalize3, szTargetViewSite);

ACTION_ARG_LIST( CActionAddToAnalize3 )	
END_ACTION_ARG_LIST();

/////////////////////////////////////////////////////////////////////////////
bool CActionAddToAnalize3::Invoke()
{
	{
		IUnknown	*punkDoc = 0;
		sptrIView	sptrV( m_punkTarget );
		sptrV->GetDocument( &punkDoc );
		sptrIDocument	sptrD( punkDoc );

		m_lTargetKey = ::GetObjectKey( sptrD );

		if( !punkDoc )
			return false;

		punkDoc->Release();
	}

	ICarioVDnDPtr sptrView = m_punkTarget;

	if( sptrView )
	{
		_bstr_t bstrS;

		sptrView->GetUndoData( bstrS.GetAddress(), &m_undoState, &m_redoState );
		m_section = (char*)bstrS;
	}



	return true;	
}

bool CActionAddToAnalize3::DoRedo()
{
	return _refresh_data( false );
}

bool CActionAddToAnalize3::DoUndo()
{
	return _refresh_data( true );
}

bool CActionAddToAnalize3::_refresh_data( bool bUndo )
{
	IDataContext3Ptr ptrC = m_punkTarget;

	IUnknown *punkList = 0;
	ptrC->GetActiveObject( _bstr_t( szTypeObjectList ), &punkList );

	INamedDataObject2Ptr	ptrList = punkList;

	INamedDataPtr ptrData = punkList;
	
	if( punkList )
		punkList->Release();

	if( ptrData == 0 )
		return false;

	if( bUndo )
		m_undoState.Store( ptrData, m_section );
	else
		m_redoState.Store( ptrData, m_section );

	IUnknown* punkObj = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	::FireEvent( m_punkTarget, szEventCarioChange, 0, punkObj, 0, 0 );

	if( punkObj )
		punkObj->Release();

	return true;
}

CString CActionAddToAnalize3::_get_key( IUnknown *punk )
{
	BSTR bstr = 0;
	StringFromCLSID(::GetObjectKey( punk ) , &bstr );

	CString strKey = bstr;

	::CoTaskMemFree( bstr );	
	bstr = 0;

	return strKey;
}
