// ActionMergeCGH.cpp: implementation of the CActionSetPane class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ActionMergeCGH.h"
#include "resource.h"
#include "misc_utils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE( CActionSetPane, CCmdTargetEx );

// {6DBC6698-4676-4ba0-962F-A190637CC888}
GUARD_IMPLEMENT_OLECREATE( CActionSetPane, "CGH_Chomos.SetCGHPanes", 
0x6dbc6698, 0x4676, 0x4ba0, 0x96, 0x2f, 0xa1, 0x90, 0x63, 0x7c, 0xc8, 0x88);

// {6F8AC5CA-E04D-411f-A175-979053259061}
static const GUID GUID_SetCGHPage = 
{ 0x6f8ac5ca, 0xe04d, 0x411f, { 0xa1, 0x75, 0x97, 0x90, 0x53, 0x25, 0x90, 0x61 } };


ACTION_INFO_FULL( CActionSetPane, IDS_ACTION_MERGE, -1, -1, GUID_SetCGHPage );
ACTION_TARGET( CActionSetPane, szTargetViewSite );


ACTION_ARG_LIST( CActionSetPane )
	ARG_STRING("TargetImage", 0 )
	ARG_STRING("SourceImage", 0 )
	ARG_INT("PaneToSet", 0)
	ARG_INT("xOffest", 0)
	ARG_INT("yOffest", 0)
	ARG_STRING("Convertor", "CGH" )
	ARG_INT("PanesCount", -1)
END_ACTION_ARG_LIST()

CActionSetPane::CActionSetPane()
{
}

CActionSetPane::~CActionSetPane()
{

}

bool CActionSetPane::Initialize() { COperationBase::Initialize(); 	Finalize();	return true; }
bool CActionSetPane::IsAvaible()
{
	return true;
}

bool CActionSetPane::Invoke()
{
	CString strDestName = GetArgumentString( _T( "TargetImage" ) );
	CString strSrcName = GetArgumentString( _T( "SourceImage" ) );

	int nPane = GetArgumentInt( _T( "PaneToSet" ) );
	int xOffset = -GetArgumentInt( _T( "xOffest" ) );
	int yOffset = -GetArgumentInt( _T( "yOffest" ) );
	int nPanesCount = GetArgumentInt( _T( "PanesCount" ) );
	if (nPanesCount != -1)
		nPanesCount = max(nPanesCount,nPane+1);

	if( StoreDestImage( strDestName ) != S_OK )
	{
		CString strConvert = GetArgumentString( _T( "Convertor" ) );
		if( CreateDestByImage( strDestName, strSrcName, strConvert, nPanesCount ) != S_OK )
			return false;
	}
	else
	{
		if( ReCreateDest(false) != S_OK )
			return false;
	}

	bool bGray = false;
	if( _chech_cc( GetContextImage( strSrcName ), "GRAY", false ) == S_OK )
		bGray = true;
	else if( _chech_cc( GetContextImage( strSrcName ), "RGB", false ) == S_OK )
		bGray = false;
	else
	{
		_chech_cc( GetContextImage( strSrcName ), "RGB" );
		return false;
	}

	int nPaneCount = ::GetImagePaneCount( m_sptrImage );

	if( nPane >= 32 )
		AfxMessageBox( IDS_SET_ERROR );
	else if( nPane >= nPaneCount )
	{
		IImage4Ptr ptrImage = m_sptrImage;

		int nDiff = nPane - nPaneCount + 1;
		for( int i = 0; i < nDiff; i++ )
			ptrImage->AddPane( -1 );
	}

	if( CopyImagePane( strSrcName, nPane, bGray ? 0 : -1, xOffset, yOffset ) != S_OK )
		return false;

	if( RemoveFromContext( m_sptrPrevImg ) != S_OK )
		return false;

	if( StoreToContext() != S_OK )
		return false;

	CString strPane;
	strPane.Format( "%d", nPane );

	StoreToNamedData( _bstr_t( KARYO_ACTIONROOT ), _bstr_t( CString( CGH_PANE ) + strPane ), _variant_t( long(1) ) );

	return true;
}

/********************************************************************************************************/
IMPLEMENT_DYNCREATE( CActionRemovePane, CCmdTargetEx );

// {E82946A8-6549-4e24-B5B2-B5C7057FA590}
GUARD_IMPLEMENT_OLECREATE( CActionRemovePane, "CGH_Chomos.RemoveCGHPanes", 
0xe82946a8, 0x6549, 0x4e24, 0xb5, 0xb2, 0xb5, 0xc7, 0x5, 0x7f, 0xa5, 0x90);


// {3E50F793-B427-4f53-8B90-30F0D2E7858F}
static const GUID GUID_RemoveCGHPane = 
{ 0x3e50f793, 0xb427, 0x4f53, { 0x8b, 0x90, 0x30, 0xf0, 0xd2, 0xe7, 0x85, 0x8f } };

ACTION_INFO_FULL( CActionRemovePane, IDS_ACTION_REMOVE_PANE, -1, -1, GUID_RemoveCGHPane );
ACTION_TARGET( CActionRemovePane, szTargetViewSite );


ACTION_ARG_LIST( CActionRemovePane )
	ARG_STRING("TargetImage", 0 )
	ARG_INT("PaneToSet", 0)
	ARG_INT("FullRemove", 0 )
END_ACTION_ARG_LIST()

CActionRemovePane::CActionRemovePane()
{
}

CActionRemovePane::~CActionRemovePane()
{

}

bool CActionRemovePane::Initialize() {	COperationBase::Initialize();	Finalize();	return true; }
bool CActionRemovePane::IsAvaible() {	return true; }

bool CActionRemovePane::Invoke()
{
	CString strDestName = GetArgumentString( _T( "TargetImage" ) );

	int nPane = GetArgumentInt( _T( "PaneToSet" ) );
	bool bFullRemove = GetArgumentInt( _T( "FullRemove" ) ) != 0;

	if( StoreDestImage( strDestName ) != S_OK )
		return false;

	if( ReCreateDest() != S_OK )
		return false;


	if( !bFullRemove )
	{
		if( ClearPane( nPane ) != S_OK )
			return false;
	}
	else
	{
		int nPaneCount = ::GetImagePaneCount( m_sptrImage );

		if( nPane > nPaneCount || nPane < 0 )
			AfxMessageBox( IDS_REMOVE_ERROR );
		else
		{
			IImage4Ptr ptrImage = m_sptrImage;
			ptrImage->RemovePane( nPane );
		}
	}
	if( StoreToContext() != S_OK )
		return false;

	CString strPane;
	strPane.Format( "%d", nPane );

	StoreToNamedData( _bstr_t( KARYO_ACTIONROOT ), _bstr_t( CString( CGH_PANE ) + strPane ), _variant_t( long(1) ) );

	return true;
}


