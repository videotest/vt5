#include "StdAfx.h"
#include "resource.h"
#include "actions.h"
#include "carioint.h"
#include "idiodbase_int.h"
#include "galleryviewbase.h"
#include "idio_int.h"
#include "params.h"
#include "globals.h"

#include "\vt5\common2\class_utils.h"

bool SetModifiedFlagToDoc( IUnknown* punkDoc )
{
	IFileDataObject2Ptr ptrFDO2;

	if( punkDoc == 0 )
	{
		IApplicationPtr ptrApp( ::GetAppUnknown() );
		if( ptrApp )
		{
			IUnknown* punk = 0;
			ptrApp->GetActiveDocument( &punk );
			if( punk )
			{
				ptrFDO2 = punk;
				punk->Release();	punk = 0;
			}
		}
	}
	else
		ptrFDO2 = punkDoc;

	if( ptrFDO2 == 0 )
		return false;

	ptrFDO2->SetModifiedFlag( TRUE );


	return true;
}
//////////////////////////////////////////////////////////////////////
//// CActionShowIdioDBView ///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionShowIdioDBView, CCmdTargetEx);
// {10F78411-00E5-4b2d-9AE3-12C736EBE28C}
GUARD_IMPLEMENT_OLECREATE(CActionShowIdioDBView, "IDioDBase.ShowView",
0x10f78411, 0xe5, 0x4b2d, 0x9a, 0xe3, 0x12, 0xc7, 0x36, 0xeb, 0xe2, 0x8c);

// {113CE61D-0A38-44b1-A80D-3C10341D0EFA}
static const GUID guidShowView = 
{ 0x113ce61d, 0xa38, 0x44b1, { 0xa8, 0xd, 0x3c, 0x10, 0x34, 0x1d, 0xe, 0xfa } };

ACTION_INFO_FULL(CActionShowIdioDBView, IDS_ACTION_SHOW_VIEW, IDS_MENU_VIEW, -1, guidShowView);
ACTION_TARGET(CActionShowIdioDBView, szTargetFrame);

CString CActionShowIdioDBView::GetViewProgID()
{	
	return szIdioDBViewProgID; 
}

//////////////////////////////////////////////////////////////////////
//// CActionAddToIdioDB ///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionAddToIdioDB, CCmdTargetEx);

// {55DA4879-9943-49cb-B322-FD44B9F878C1}
GUARD_IMPLEMENT_OLECREATE( CActionAddToIdioDB, "IDioDBase.AddToIdioDB",
0x55da4879, 0x9943, 0x49cb, 0xb3, 0x22, 0xfd, 0x44, 0xb9, 0xf8, 0x78, 0xc1);

// {18E0CEA2-DF72-4f3b-86CA-4A53BC78F277}
static const GUID guidAddToIdioDB = 
{ 0x18e0cea2, 0xdf72, 0x4f3b, { 0x86, 0xca, 0x4a, 0x53, 0xbc, 0x78, 0xf2, 0x77 } };

ACTION_INFO_FULL(CActionAddToIdioDB, IDS_ACTION_ADD_TO_IDIODB, -1, -1, guidAddToIdioDB);
ACTION_TARGET(CActionAddToIdioDB, szTargetApplication );

ACTION_ARG_LIST( CActionAddToIdioDB )
	ARG_STRING( "FileName", "" )
	ARG_STRING( "DBFileName", "" )
END_ACTION_ARG_LIST()

///////////////////////////////////////////////////////////////////////////////
IUnknownPtr _GetObjectByName( IUnknown *punkFrom, const BSTR bstrObject, const BSTR bstrType, bool bThrowException = true)
{
	_bstr_t	bstrObjectName = bstrObject;
	_bstr_t	bstrTypeName = bstrType;
	sptrIDataTypeManager	ptrDataType( punkFrom );

	long	nTypesCounter = 0;
	ptrDataType->GetTypesCount( &nTypesCounter );

	for( long nType = 0; nType < nTypesCounter; nType ++ )
	{
		BSTR	bstrCurTypeName = 0;
		ptrDataType->GetType( nType, &bstrCurTypeName );

		_bstr_t	str( bstrCurTypeName, false );

		if( bstrType && str != bstrTypeName )
			continue;

		IUnknown	*punkObj = 0;
		long	dwPos = 0;

		ptrDataType->GetObjectFirstPosition( nType, &dwPos );

		while( dwPos )
		{
			punkObj = 0;
			ptrDataType->GetNextObject( nType, &dwPos, &punkObj );

			if (bstrObjectName.length() == 0 || ::GetObjectName( punkObj ) == (char*)bstrObjectName)
			{
				IUnknownPtr sptr(punkObj);
				if (punkObj) punkObj->Release();
				return sptr;
			}

			INamedDataObject2Ptr	ptrN( punkObj );
			punkObj->Release();

			long	lpos = 0;
			ptrN->GetFirstChildPosition( &lpos );

			while( lpos )
			{
				IUnknown	*punkObject = 0;
				ptrN->GetNextChild( &lpos, &punkObject );

				if (bstrObjectName.length() == 0 || ::GetObjectName( punkObject ) == (char*)bstrObjectName)
				{
					IUnknownPtr sptrO(punkObject);
					punkObject->Release();
					return sptrO;
				}

				punkObject->Release();


			}

			
		}
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////

bool CActionAddToIdioDB::Invoke()
{
	CString strFileName = GetArgumentString( "FileName" );
	CString strDBFileName = GetArgumentString( "DBFileName" );

	int  l = 0, c = 0, o = 0;
	IUnknown *punkIdio = 0;
	long lClass = -1;

	if( strFileName.IsEmpty() )
	{
		IApplicationPtr sptrApp = m_punkTarget;

		IUnknown *punkDoc = 0;
		sptrApp->GetActiveDocument( &punkDoc );

		IDocumentSitePtr sptrDoc = punkDoc;
		if( punkDoc )
			punkDoc->Release();

        if( sptrDoc == 0 )
			return false;

		IUnknown *punkView = 0;
		sptrDoc->GetActiveView( &punkView );

		ICarioView2Ptr ptrView = punkView;

		if( punkView )
			punkView->Release();

		if( ptrView != 0 )
		{
			ptrView->GetActiveChromo( &l, &c, &o );
			ptrView->GetIdioObject(  l, c, o, idtEditable, &punkIdio );

			if( !punkIdio )
				return false;

			INamedDataObject2Ptr	ptrNamed = punkIdio;

			IUnknown *punkParent = 0;
			ptrNamed->GetParent( &punkParent );

			if( !punkParent )
				return false;

			IIdiogramPtr ptrI = punkIdio;
			punkIdio->Release();

			ICalcObjectPtr	ptrCalc = punkParent;
			punkParent->Release();

			if( ptrCalc == 0 )
				return false;

			lClass = get_object_class( ptrCalc );
		}
	}
	else
	{
		FILE *fl = fopen( strFileName, "r" );
		if( !fl )
			return false;
		fclose( fl );
	}
	IUnknown *punkDB = 0;
	
	IUnknownPtr sptrTarget;

	if( strDBFileName.IsEmpty() )
	{
		IApplicationPtr sptrApp = m_punkTarget;

		IUnknown *punkDoc = 0;
		sptrApp->GetActiveDocument( &punkDoc );

		IDocumentSitePtr sptrDoc = punkDoc;
		if( punkDoc )
			punkDoc->Release();

        if( sptrDoc == 0 )
			return false;

		IUnknown *punkView = 0;
		sptrDoc->GetActiveView( &punkView );

		if( punkView )
		{
			punkDB = GetActiveObjectFromContext( punkView, szTypeIdioDBase );
			sptrTarget = punkView;

			punkView->Release();
		}
	}
	else
	{
		FILE *fl = fopen( strDBFileName, "r" );
		if( !fl )
		{
			::ExecuteAction( "FileNew", "\"IdioDBase\"" );

			IApplicationPtr sptrApp = ::GetAppUnknown();

			IUnknown *punkDoc = 0;
			sptrApp->GetActiveDocument( &punkDoc );

			if( punkDoc )
			{
				IUnknown *punkObject = ::CreateTypedObject( _bstr_t( szTypeIdioDBase ) );

				if( punkObject )
				{
					::SetValue( punkDoc, 0, 0, _variant_t( punkObject ) );
					punkObject->Release(); 
					punkObject = 0;

					::ExecuteAction( "FileSave", "\"" + strDBFileName + "\",\"\"" );
				}
			}
		}
		else
			fclose( fl );

		::ExecuteAction( "FileOpen", "\"" + strDBFileName + "\"" );
		::ExecuteAction( "WindowActivate", CString( "\"" )+strDBFileName.Right( strDBFileName.GetLength() - strDBFileName.ReverseFind( '\\' ) - 1 )+CString( "\"" ) );

		IApplicationPtr sptrApp = ::GetAppUnknown();

		IUnknown *punkDoc = 0;
		sptrApp->GetActiveDocument( &punkDoc );

		if( punkDoc )
		{
			IDocumentSitePtr sptrSite = punkDoc;
			punkDoc->Release();

			if( sptrSite )
			{
				_bstr_t bstrName;
				sptrSite->GetPathName( bstrName.GetAddress() );

				if( (char *)bstrName != strDBFileName )
				{
					long lTemplPos = 0;
					
					sptrApp->GetFirstDocTemplPosition( &lTemplPos );
					while( lTemplPos )
					{
						sptrApp->GetNextDocTempl( &lTemplPos, 0, 0 );

						long lPosDoc = 0;

						sptrApp->GetFirstDocPosition( lTemplPos, &lPosDoc );
						
						if( lPosDoc < 0 )
							lPosDoc = 0;

						punkDoc = 0;
						while( lPosDoc )
						{
							sptrApp->GetNextDoc( lTemplPos, &lPosDoc, &punkDoc );

							if( punkDoc )
							{
								IDocumentSitePtr sptrDocSite = punkDoc;
								punkDoc->Release();

								if( sptrDocSite )
								{
									_bstr_t name;
									sptrDocSite->GetPathName( name.GetAddress() );

									if( (char *)name == strDBFileName )
										break;
									else
										punkDoc = 0;
								}
							}
						}
						if( punkDoc )
						{
							CString str = strDBFileName.Right( strDBFileName.GetLength() - strDBFileName.ReverseFind( '\\' ) - 1 );

							ExecuteAction( "WindowActivate", CString( "\"" )+str+CString( "\"" ) );
							break;
						}
					}
				}
			}

			if( punkDoc )
			{
				IUnknownPtr sptr = _GetObjectByName( punkDoc, 0, _bstr_t( szTypeIdioDBase ) );

				sptrTarget = punkDoc;
				punkDB = sptr;
				punkDB->AddRef();
			}
		}
	}

	if( punkDB )
	{
		IUnknown *punk = 0;
		
		if( strFileName.IsEmpty() )
		{
			punk = CloneObject( punkIdio );

			IIdiogramPtr ptrI = punk;
			ptrI->set_class( lClass );
			ptrI->set_flags( IDIT_INSERTED );
		}
		else
		{
			punk = ::CreateTypedObject( _bstr_t( sz_idio_type ) );
			IIdiogramPtr sptr = punk;
			if( sptr )
				sptr->open_text( _bstr_t( strFileName ) );

		}

		if( punk )
		{
			INamedDataObject2Ptr sptrN = punk;
			punk->Release();

			INumeredObjectPtr ptrNumObj = sptrN;
			ptrNumObj->GenerateNewKey( 0 );

			sptrN->SetParent( punkDB, 0 );
			
			long lParam = cncAdd;
			::FireEvent( sptrTarget, szEventChangeObjectList, punkDB, punk, &lParam, 0 );
			SetModifiedFlagToDoc( sptrTarget );
		}
		punkDB->Release();
	}

	return true;
}

bool CActionAddToIdioDB::IsAvaible()
{
	return true;
}

//////////////////////////////////////////////////////////////////////
//// CActionFindObject ///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionFindObject, CCmdTargetEx);

// {CED40559-BD25-4484-BE95-BA6EB793B9C0}
GUARD_IMPLEMENT_OLECREATE( CActionFindObject, "IDioDBase.FindByText",
0xced40559, 0xbd25, 0x4484, 0xbe, 0x95, 0xba, 0x6e, 0xb7, 0x93, 0xb9, 0xc0);

// {C27BE7CE-E23D-4637-BE61-2B0D5005B72D}
static const GUID guidFindByText = 
{ 0xc27be7ce, 0xe23d, 0x4637, { 0xbe, 0x61, 0x2b, 0xd, 0x50, 0x5, 0xb7, 0x2d } };

ACTION_INFO_FULL( CActionFindObject, IDS_ACTION_SEARCH_BY_TEXT, -1, -1, guidFindByText);
ACTION_TARGET( CActionFindObject, szTargetViewSite );
///////////////////////////////////////////////////////////////////////////////
#include "SearchDlg.h"

bool CActionFindObject::Invoke()
{
	static CString strLast = "";
	CSearchDlg dlg;

	dlg.m_Text = strLast;
	if( dlg.DoModal() == IDOK )
	{ 
		IIdioDBaseViewPtr sptrView = m_punkTarget;
		if( sptrView )
			sptrView->SelectByText( _bstr_t( dlg.m_Text ) );

		strLast = dlg.m_Text;
	}
	return true;
}

bool CActionFindObject::IsAvaible()
{
	IIdioDBaseViewPtr sptrView = m_punkTarget;
	if( sptrView )
		return true;
	return false;
}

