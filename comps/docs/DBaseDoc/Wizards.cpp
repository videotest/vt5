#include "stdafx.h"
#include "dbasedoc.h"

#include "types.h"
#include "Wizards.h"
#include "constant.h"
#include "dbaseengine.h"
#include "oledata.h"


#include "DBaseWiz.h"
#include "QueryWiz.h"
#include "TalbeWiz.h"
#include "SimpleFieldPage.h"


/////////////////////////////////////////////////////////////////////////////
CWizardPool::CWizardPool()
{
	m_wm			= wmNotDefine;
	m_punkActiveDoc	= 0;
}

/////////////////////////////////////////////////////////////////////////////
CWizardPool::~CWizardPool()
{

}

/////////////////////////////////////////////////////////////////////////////
//
//
//	Wizards
//
//
//
/////////////////////////////////////////////////////////////////////////////
bool CWizardPool::QueryProperties( IUnknown* punkDoc, IUnknown* punkQ, bool bshow_query )
{
	TABLEINFO_ARR arrTI;
	if( !::FillTableInfoFromDoc( punkDoc, &arrTI ) )
		return false;

	ISelectQueryPtr spSelectQuery( punkQ );
	IQueryObjectPtr spQueryData( punkQ );

	if( spQueryData == 0 | spSelectQuery == 0 )
		return false;


	CWizardSheet	ps;
	if(bshow_query)
		ps.SetHelpFileName( "QueryProperties" );
	else
		ps.SetHelpFileName( "GalleryOptions" );
	ps.SetWizardPool( this );

	CEditQueryPage	pgSelect;
	CTumbnailPage	pgTumbnail;	
	
	if( bshow_query )
		ps.AddPage( &pgSelect );
	else
		ps.AddPage( &pgTumbnail );	


	CString strPropTile;
	strPropTile.LoadString( IDS_CAPTION_QUERY_PROP );
	ps.SetTitle( strPropTile );

	pgSelect.SetTableInfo( &arrTI );
	pgTumbnail.SetTableInfo( &arrTI );

	CWizardsData dataIn;
	if( !ReadWizardDataFromQuery( spQueryData, &dataIn) )
		return false;


	if( bshow_query )
		Update_EditQueryPage( &dataIn, &pgSelect, true );
	else
		Update_TumbnailPage( &dataIn, &pgTumbnail, true );
					   

	m_wm = wmQueryProp;

	ps.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	INT_PTR nRes = ps.DoModal();	

	m_wm = wmNotDefine;

	for( int i=0;i<arrTI.GetSize();i++ )
		delete arrTI[i];

	arrTI.RemoveAll();

	if( nRes != IDOK )
		return false;

	if( bshow_query )
		Update_EditQueryPage( &dataIn, &pgSelect, false );
	else
		Update_TumbnailPage( &dataIn, &pgTumbnail, false );


	BOOL bWasOpen = false;
	spSelectQuery->IsOpen( &bWasOpen );	

	WriteWizardDataToQuery( spQueryData, &dataIn );


	//if( bNeedRequery )
	{	
		if( bWasOpen )
			spSelectQuery->UpdateInteractive( TRUE );

		spSelectQuery->Close();
		spSelectQuery->Open();
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
bool CWizardPool::NewQueryWizard( IUnknown* punk )
{
	IDBaseDocumentPtr ptrDoc;
	if( !GetDBaseDocument( ptrDoc ) )
		return false;

	TABLEINFO_ARR arrTI;
	if( !::FillTableInfoFromDoc( ptrDoc, &arrTI ) )
		return false;

	m_punkActiveDoc	= punk;

	CWizardSheet	ps;	
	ps.SetHelpFileName( "QueryProperties" );
	ps.SetWizardPool( this );

	CQueryNamePage	pgName;
	CEditQueryPage	pgSelect;
	CTumbnailPage	pgTumbnail;		
	
	ps.AddPage( &pgName );
	ps.AddPage( &pgSelect );
	ps.AddPage( &pgTumbnail );	


	//ps.SetWizardButtons();
	ps.SetWizardMode();

	pgName.m_strQueryName = ::GetObjectName( punk );

	pgSelect.SetTableInfo( &arrTI );
	pgTumbnail.SetTableInfo( &arrTI );


	m_wm = wmNewQuery;

	int nRes = ps.DoModal();

	m_wm = wmNotDefine;

	for( int i=0;i<arrTI.GetSize();i++ )
		delete arrTI[i];

	arrTI.RemoveAll();

	m_punkActiveDoc	= 0;

	if( nRes != ID_WIZFINISH )
		return false;


	CWizardsData dataOut;

	Update_EditQueryPage( &dataOut, &pgSelect, false );
	Update_TumbnailPage( &dataOut, &pgTumbnail, false );

	WriteWizardDataToQuery( punk, &dataOut );

	INamedObject2Ptr ptrND( punk );
	if( ptrND )
		ptrND->SetName( _bstr_t( (LPCSTR) pgName.m_strQueryName ) );


	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CWizardPool::NewDBWizard( IUnknown* punkDBDoc )
{

	CWizardSheet	ps;
	ps.SetWizardPool( this );


	CNewDBTypePage				pgDBType;
	CNewDBAccessPage			pgDBAccess;
	CNewDBAdvPage				pgDBAdv;


	CNewTablePage				pgNewTable;
	CDefineFieldCreationPage	pgDefineFieldCreation;
	CAdvFieldConstrPage			pgAdvFieldConstr;
//	CSimpleFieldConstrPage		pgSimpleFieldConstr;
	CSimpleFieldPage			pgSimpleFieldConstr;


	CQueryNamePage				pgName;
	CEditQueryPage				pgSelect;
	CTumbnailPage				pgTumbnail;


	CDBInfoCreate				pgDBInfoCreate;
	CDBCreateProcess			pgDBCreateProcess;
	
	ps.AddPage( &pgDBType );
	ps.AddPage( &pgDBAccess );
	ps.AddPage( &pgDBAdv );

	ps.AddPage( &pgNewTable );
	ps.AddPage( &pgDefineFieldCreation );
	ps.AddPage( &pgAdvFieldConstr );
	ps.AddPage( &pgSimpleFieldConstr );

	ps.AddPage( &pgName );
	ps.AddPage( &pgSelect );
	ps.AddPage( &pgTumbnail );
	ps.AddPage( &pgDBInfoCreate );
	ps.AddPage( &pgDBCreateProcess );


	ps.SetWizardMode();

	m_wm = wmNewDB;

	m_punkActiveDoc = punkDBDoc;

	int nRes = ps.DoModal( );
	
	m_punkActiveDoc = 0;

	m_wm = wmNotDefine;

	if( nRes != ID_WIZFINISH )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CWizardPool::NewDB_IsAccessDataBase( CWizardSheet* pSheet )
{
	if( !pSheet )
	{
		ASSERT( false );
		return false;
	}

	CNewDBTypePage* pPage = 
		(CNewDBTypePage*)pSheet->FindPage( RUNTIME_CLASS(CNewDBTypePage), 0 );

	if( !pPage )
	{
		ASSERT( false );
		return false;
	}	

	return pPage->m_nDBaseType == 0;
}

/////////////////////////////////////////////////////////////////////////////
bool CWizardPool::NewDB_IsSimpleFieldConstructor( CWizardSheet* pSheet )
{
	if( !pSheet )
	{
		ASSERT( false );
		return false;
	}

	CDefineFieldCreationPage* pPage = 
		(CDefineFieldCreationPage*)pSheet->FindPage( RUNTIME_CLASS(CDefineFieldCreationPage), 0 );

	if( !pPage )
	{
		ASSERT( false );
		return false;
	}	

	return pPage->m_nDefineType == 0;
}

/////////////////////////////////////////////////////////////////////////////
bool CWizardPool::NewDB_NeedCreateNewTable( CWizardSheet* pSheet )
{
	if( !pSheet )
	{
		ASSERT( false );
		return false;
	}

	CNewDBAccessPage* pPage = 
		(CNewDBAccessPage*)pSheet->FindPage( RUNTIME_CLASS(CNewDBAccessPage), 0 );

	if( !pPage )
	{
		ASSERT( false );
		return false;
	}	

	return pPage->m_bCreateNewTable == TRUE;
	
}

/////////////////////////////////////////////////////////////////////////////
CString CWizardPool::NewDB_GetAccesFileName( CWizardSheet* pSheet )
{
	GET_NEW_DB_WIZARD_DATA( "" )

	int nPosFind = pDBAccess->m_strDBPath.Find( '\\' );

	if( nPosFind != -1 )
		return pDBAccess->m_strDBPath;

	
	TCHAR szCurrentPath[MAX_PATH];
	GetCurrentDirectory( sizeof(szCurrentPath)/sizeof(TCHAR), szCurrentPath );

	CString str = szCurrentPath;
	str += '\\';
	str += pDBAccess->m_strDBPath;

	{
		CString strTemp = str;
		strTemp.MakeLower( );
		if( strTemp.Find( ".mdb") == -1 )
		{
			str += ".mdb";
		}
	}

	return str;
}

/////////////////////////////////////////////////////////////////////////////
bool CWizardPool::IsAborted()
{
	MSG	msg;

	while( ::PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
	{
		if( msg.message == WM_CHAR && msg.wParam == VK_ESCAPE )
		{
				return true;
		}

		::TranslateMessage( &msg );
		::DispatchMessage( &msg );
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CWizardPool::_OnWizardSetActivePage( CWizardSheet* pSheet, CPropertyPage* pPage )
{
	if( !pSheet || !pPage )
		return true;
									   
	if( m_wm == wmQueryProp )
	{
	}
	else
	if( m_wm == wmNewQuery )
	{	
		if( pSheet->GetPageIndex( pPage ) == 0 )
			pSheet->SetWizardButtons( PSWIZB_NEXT );
		else
			if( pSheet->GetPageIndex( pPage ) == ( pSheet->GetPageCount() - 1 ) )
				pSheet->SetWizardButtons( PSWIZB_BACK | PSWIZB_FINISH );
		else
			pSheet->SetWizardButtons( PSWIZB_NEXT | PSWIZB_BACK );

	}
	else
	if( m_wm == wmNewDB )
	{	
		if( pPage->IsKindOf( RUNTIME_CLASS(CNewDBTypePage) ) )
		{
			pSheet->SetWizardButtons( PSWIZB_NEXT );
		}
		else
		if( pPage->IsKindOf( RUNTIME_CLASS(CDBInfoCreate) ) )
		{
			if( !NewDB_InfoCreateDB( pSheet, (CDBInfoCreate*)pPage ) )
				pSheet->SetWizardButtons( PSWIZB_BACK );
			else
				pSheet->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );

		}
		else
		if( pPage->IsKindOf( RUNTIME_CLASS(CDBCreateProcess) ) )
		{
		}		
		else		
		{
			pSheet->SetWizardButtons( PSWIZB_NEXT | PSWIZB_BACK );
		}

		if( pPage->IsKindOf( RUNTIME_CLASS(CEditQueryPage) ) )
		{

		}
	}


	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CWizardPool::_OnWizardNext( CWizardSheet* pSheet, CPropertyPage* pPage )
{
	if( !pSheet || !pPage )
		return true;

	if( m_wm == wmNewDB )
	{
		if( pPage->IsKindOf( RUNTIME_CLASS(CNewDBTypePage) ) )
		{			
			if( NewDB_IsAccessDataBase( pSheet ) )
			{
				pSheet->ActivatePage( RUNTIME_CLASS(CNewDBAccessPage) );			
				return false;
			}
			else
			{
				pSheet->ActivatePage( RUNTIME_CLASS(CNewDBAdvPage) );
				return false;
			}
		}
		else
		if( pPage->IsKindOf( RUNTIME_CLASS(CNewDBAccessPage) ) )
		{
			if( NewDB_NeedCreateNewTable( pSheet ) )
			{
				pSheet->ActivatePage( RUNTIME_CLASS(CNewTablePage) );
				return false;
			}
			else
			{
				pSheet->ActivatePage( RUNTIME_CLASS(CQueryNamePage) );
				return false;
			}
		}
		else
		if( pPage->IsKindOf( RUNTIME_CLASS(CNewDBAdvPage) ) )
		{
			pSheet->ActivatePage( RUNTIME_CLASS(CQueryNamePage) );
			return false;
		}
		else
		if( pPage->IsKindOf( RUNTIME_CLASS(CNewTablePage) ) )
		{
			pSheet->ActivatePage( RUNTIME_CLASS(CDefineFieldCreationPage) );
			return false;
		}
		else
		if( pPage->IsKindOf( RUNTIME_CLASS(CDefineFieldCreationPage) ) )
		{			
			if( NewDB_IsSimpleFieldConstructor( pSheet ) )
			{
				pSheet->ActivatePage( RUNTIME_CLASS(CSimpleFieldPage) );
				return false;
			}
			else
			{
				pSheet->ActivatePage( RUNTIME_CLASS(CAdvFieldConstrPage) );
				return false;
			}
		}
		else
		if( pPage->IsKindOf( RUNTIME_CLASS(CSimpleFieldPage) ) || 
			pPage->IsKindOf( RUNTIME_CLASS(CAdvFieldConstrPage) ) )
		{
			pSheet->ActivatePage( RUNTIME_CLASS(CQueryNamePage) );
			return false;
		}
		else
		if( pPage->IsKindOf( RUNTIME_CLASS(CQueryNamePage) ) )
		{
			pSheet->ActivatePage( RUNTIME_CLASS(CEditQueryPage) );
			return false;
		}
		else
		if( pPage->IsKindOf( RUNTIME_CLASS(CEditQueryPage) ) )
		{
			pSheet->ActivatePage( RUNTIME_CLASS(CTumbnailPage) );
			return false;
		}
		else
		if( pPage->IsKindOf( RUNTIME_CLASS(CTumbnailPage) ) )
		{
			pSheet->ActivatePage( RUNTIME_CLASS(CDBInfoCreate) );
			return false;
		}
		else
		if( pPage->IsKindOf( RUNTIME_CLASS(CDBInfoCreate) ) )
		{
			pSheet->ActivatePage( RUNTIME_CLASS(CDBCreateProcess) );
			NewDB_CreateNewDB( pSheet, (CDBCreateProcess*)pSheet->FindPage( RUNTIME_CLASS(CDBCreateProcess), 0 ) );
			return false;
		}

	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CWizardPool::_OnWizardBack( CWizardSheet* pSheet, CPropertyPage* pPage )
{
	if( !pSheet || !pPage )
		return true;

	if( !pSheet || !pPage )
		return true;

	if( m_wm == wmNewDB )
	{		
		if( pPage->IsKindOf( RUNTIME_CLASS(CDBInfoCreate) ) )		
		{
			pSheet->ActivatePage( RUNTIME_CLASS(CTumbnailPage) );
			return false;
		}
		else
		if( pPage->IsKindOf( RUNTIME_CLASS(CTumbnailPage) ) )		
		{
			pSheet->ActivatePage( RUNTIME_CLASS(CEditQueryPage) );
			return false;
		}
		else
		if( pPage->IsKindOf( RUNTIME_CLASS(CEditQueryPage) ) )		
		{
			pSheet->ActivatePage( RUNTIME_CLASS(CQueryNamePage) );
			return false;
		}
		if( pPage->IsKindOf( RUNTIME_CLASS(CQueryNamePage) ) )		
		{
			if( !NewDB_IsAccessDataBase( pSheet ) )
			{
				pSheet->ActivatePage( RUNTIME_CLASS(CNewDBAdvPage) );
				return false;
			}
			else
			{
				if( NewDB_NeedCreateNewTable( pSheet ) )				
				{
					if( NewDB_IsSimpleFieldConstructor( pSheet ) )
					{
						pSheet->ActivatePage( RUNTIME_CLASS(CSimpleFieldPage) );
						return false;
					}
					else
					{
						pSheet->ActivatePage( RUNTIME_CLASS(CAdvFieldConstrPage) );
						return false;
					}
				}	
				else
				{
					pSheet->ActivatePage( RUNTIME_CLASS(CNewDBAccessPage) );
					return false;
				}
			}			
		}
		else
		if( pPage->IsKindOf( RUNTIME_CLASS(CSimpleFieldPage) ) ||
			pPage->IsKindOf( RUNTIME_CLASS(CAdvFieldConstrPage) ) )
		{
			pSheet->ActivatePage( RUNTIME_CLASS(CDefineFieldCreationPage) );
			return false;
		}
		else
		if( pPage->IsKindOf( RUNTIME_CLASS(CDefineFieldCreationPage) ) )
		{
			pSheet->ActivatePage( RUNTIME_CLASS(CNewTablePage) );
			return false;
		}
		else
		if( pPage->IsKindOf( RUNTIME_CLASS(CNewTablePage) ) )
		{
			pSheet->ActivatePage( RUNTIME_CLASS(CNewDBAccessPage) );
			return false;
		}
		else
		if( pPage->IsKindOf( RUNTIME_CLASS(CNewDBAccessPage) ) ||
			pPage->IsKindOf( RUNTIME_CLASS(CNewDBAdvPage) ) )
		{
			pSheet->ActivatePage( RUNTIME_CLASS(CNewDBTypePage) );
			return false;
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CWizardPool::_OnWizardFinish( CWizardSheet* pSheet, CPropertyPage* pPage )
{
	if( m_wm == wmNewDB )
	{		
		return true;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//
//
//	Prop pages update
//
//
//
/////////////////////////////////////////////////////////////////////////////
void CWizardPool::Update_EditQueryPage( CWizardsData* pData, CEditQueryPage* pPage, bool bFillage )
{
	if( !pData || !pPage )
		return;

	if( bFillage )
	{		
		pPage->m_nSimpleQuery	= !pData->m_bSimpleQuery;
		pPage->m_strMainTable	= pData->m_strBaseTable;
		pPage->m_strSQL			= pData->m_strManualSQL;
	}
	else
	{
		pData->m_bSimpleQuery	= !pPage->m_nSimpleQuery;
		pData->m_strBaseTable	= pPage->m_strMainTable;
		pData->m_strManualSQL	= pPage->m_strSQL;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CWizardPool::Update_TumbnailPage( CWizardsData* pData, CTumbnailPage* pPage, bool bFillage )
{
	if( !pData || !pPage )
		return;

	if( bFillage )
	{		

		pPage->m_strThumbnailTable = pData->m_strTableThumbnLocate;
		pPage->m_strThumbnailField = pData->m_strFieldThumbnLocate;

		pPage->m_strCaptionTable = pData->m_strTableThumbnCaption;
		pPage->m_strCaptionField = pData->m_strFieldThumbnCaption;
		
		pPage->m_nGenerateWidth  = pData->m_sizeThumb.cx;
		pPage->m_nGenerateHeight = pData->m_sizeThumb.cy;

		pPage->m_nViewWidth  = pData->m_sizeThumbView.cx;
		pPage->m_nViewHeight = pData->m_sizeThumbView.cy;

		pPage->m_nBorderSize = max( pData->m_sizeThumbBorder.cx, pData->m_sizeThumbBorder.cy );

		//addditional info
		pPage->m_bSimpleQuery	= pData->m_bSimpleQuery == TRUE;
		pPage->m_strMainTable	= pData->m_strBaseTable;
		pPage->m_strSQL			= pData->m_strManualSQL;
	}
	else
	{
		CString strTable, strField;
		CString strTemp;

		pData->m_strTableThumbnCaption = pPage->m_strCaptionTable;
		pData->m_strFieldThumbnCaption = pPage->m_strCaptionField;

		pData->m_strTableThumbnLocate = pPage->m_strThumbnailTable;
		pData->m_strFieldThumbnLocate = pPage->m_strThumbnailField;

		pData->m_sizeThumb.cx		= pPage->m_nGenerateWidth;
		pData->m_sizeThumb.cy		= pPage->m_nGenerateHeight;

		pData->m_sizeThumbView.cx	= pPage->m_nViewWidth;
		pData->m_sizeThumbView.cy	= pPage->m_nViewHeight;

		pData->m_sizeThumbBorder.cx = 
		pData->m_sizeThumbBorder.cy	= pPage->m_nBorderSize;

	}

}

/////////////////////////////////////////////////////////////////////////////
bool CWizardPool::ReadWizardDataFromQuery( IUnknown* punkQ, CWizardsData* pData )
{
	IQueryObjectPtr spQueryData( punkQ );
	ISelectQueryPtr spSelectQuery( punkQ );
	if( spQueryData == 0 || spSelectQuery == 0 || !pData )
		return false;

	BSTR bstr = 0;
	spQueryData->GetBaseTable( &bstr );	
	pData->m_strBaseTable = bstr;	

	if( bstr )
		::SysFreeString( bstr );	bstr = 0;

	spQueryData->GetManualSQL( &bstr );
	pData->m_strManualSQL = bstr;
	
	if( bstr )
		::SysFreeString( bstr );	bstr = 0;
	
	BOOL bSimpleQuery = FALSE;
	spQueryData->IsSimpleQuery( &bSimpleQuery );
	pData->m_bSimpleQuery = bSimpleQuery == TRUE;

	//tumbnail page
	BSTR bstrTableImage = 0;
	BSTR bstrFieldImage = 0;
	spSelectQuery->GetTumbnailLocation( &bstrTableImage, &bstrFieldImage );

	BSTR bstrTableCaption = 0;
	BSTR bstrFieldCaption = 0;
	spSelectQuery->GetTumbnailCaptionLocation( &bstrTableCaption, &bstrFieldCaption );

	pData->m_strTableThumbnLocate = bstrTableImage;	
	if( bstrTableImage )
		::SysFreeString( bstrTableImage );

	pData->m_strFieldThumbnLocate = bstrFieldImage;	

	if( bstrFieldImage )
		::SysFreeString( bstrFieldImage );

	pData->m_strTableThumbnCaption = bstrTableCaption;	
	
	if( bstrTableCaption )
		::SysFreeString( bstrTableCaption );

	pData->m_strFieldThumbnCaption = bstrFieldCaption;

	if( bstrFieldCaption )
		::SysFreeString( bstrFieldCaption );

	
	spSelectQuery->GetTumbnailSize( &pData->m_sizeThumb );
	
	spSelectQuery->GetTumbnailViewSize( &pData->m_sizeThumbView );	
	
	spSelectQuery->GetTumbnailBorderSize( &pData->m_sizeThumbBorder );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CWizardPool::WriteWizardDataToQuery( IUnknown* punkQ, CWizardsData* pData )
{
	IQueryObjectPtr spQueryData( punkQ );
	ISelectQueryPtr spSelectQuery( punkQ );
	if( spQueryData == 0 || spSelectQuery == 0 || !pData )
		return false;

	spQueryData->SetBaseTable( _bstr_t( (LPCSTR)pData->m_strBaseTable ) );
	spQueryData->SetManualSQL( _bstr_t( (LPCSTR)pData->m_strManualSQL ) );
	spQueryData->SetSimpleQuery( pData->m_bSimpleQuery == true );


	spSelectQuery->SetTumbnailCaptionLocation( 
							_bstr_t( (LPCTSTR)pData->m_strTableThumbnCaption ), 
							_bstr_t( (LPCTSTR)pData->m_strFieldThumbnCaption ) );

	spSelectQuery->SetTumbnailLocation( 
							_bstr_t( (LPCTSTR)pData->m_strTableThumbnLocate ), 
							_bstr_t( (LPCTSTR)pData->m_strFieldThumbnLocate ) );
	
	spSelectQuery->SetTumbnailSize( pData->m_sizeThumb );	
	
	spSelectQuery->SetTumbnailViewSize( pData->m_sizeThumbView );

	spSelectQuery->SetTumbnailBorderSize( pData->m_sizeThumbBorder );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CWizardPool::NewDB_InfoCreateDB( CWizardSheet* pSheet, CDBInfoCreate* pPage )
{	
	GET_NEW_DB_WIZARD_DATA( false )


	CString str;
	pPage->m_strTaskList.Empty();

	if( bAccessDB )
	{		
		
		str.Format( IDS_NEWDB_CREATE_ACCESDB, NewDB_GetAccesFileName( pSheet ) );
		pPage->m_strTaskList += str;
		pPage->m_strTaskList += "\n";

		if( bCreateNewTable )
		{
			pPage->m_strTaskList += "\n";

			_CTableInfo tiNew( "none" );
			TABLEINFO_ARR arTI;
			GetDBFieldInfo( pSheet, arTI, &tiNew ); 
			for( int i=0;i<arTI.GetSize();i++ )
				delete arTI[i];

			arTI.RemoveAll();

			int nFieldsCount = tiNew.m_arrFieldInfo.GetSize();
			

			str.Format( IDS_NEWDB_CREATE_TABLE, pNewTable->m_strTableName, nFieldsCount );
			pPage->m_strTaskList += str;
			pPage->m_strTaskList += "\n";
			
			
			for( i=0;i<nFieldsCount;i++ )
			{
				CString strType;

				_CFieldInfo* pfi = tiNew.m_arrFieldInfo[i];				
				str.Format( IDS_NEWDB_FIELDS, pfi->m_strFieldName, 
					CDBStructGrid::GetFieldTypeAsString( pfi ) );				
				pPage->m_strTaskList += str;
				pPage->m_strTaskList += "\n";

			}
			

		}
	}
	else
	{		
		str.Format( IDS_NEWDB_CREATE_ADV_CON, pDBAdv->m_strConnectionString );
		pPage->m_strTaskList += str;
		pPage->m_strTaskList += "\n";		
	}

	pPage->m_strTaskList += "\n";		

	if( pQuerySelect->m_nSimpleQuery == 0 )
	{
		str.Format( IDS_NEWDB_QUERY_SIMPLE, pQueryName->m_strQueryName, pQuerySelect->m_strMainTable );
		pPage->m_strTaskList += str;
		pPage->m_strTaskList += "\n";
	}
	else
	{
		str.Format( IDS_NEWDB_QUERY_ADV, pQueryName->m_strQueryName, pQuerySelect->m_strSQL );
		pPage->m_strTaskList += str;
		pPage->m_strTaskList += "\n";
	}

	pPage->UpdateData( FALSE );
	

	pSheet->SetWizardButtons( PSWIZB_BACK | PSWIZB_FINISH );
	

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CWizardPool::NewDB_CreateNewDBCleanUp( CWizardSheet* pSheet, CDBCreateProcess* pPage, bool bUserAbort )
{
	pSheet->SetWizardButtons( PSWIZB_BACK );
	
	if( bUserAbort )
	{
		CString str;
		str.LoadString( IDS_WAS_ABORT );
		
		pPage->m_strProgress += "\n";
		pPage->m_strProgress += "\n";
		
		pPage->m_strProgress += str;		
	}

	pPage->UpdateData( FALSE );
	
}

/////////////////////////////////////////////////////////////////////////////
bool CWizardPool::NewDB_CreateNewDB( CWizardSheet* pSheet, CDBCreateProcess* pPage )
{
	CString strSucceded;
	strSucceded.LoadString( IDS_SUCCEDED );
	
	CString strFailed;
	strFailed.LoadString( IDS_FAILED );
	

	pSheet->SetWizardButtons( 0 );

	GET_NEW_DB_WIZARD_DATA( false )


	IDBConnectionPtr	ptrDBConn	= m_punkActiveDoc;
	IDBaseStructPtr		ptrDBStruct	= m_punkActiveDoc;
	IDBaseDocumentPtr	ptrDBaseDoc	= m_punkActiveDoc;

	if( ptrDBConn == 0 || ptrDBStruct == 0 || ptrDBaseDoc == 0 )
		return false;


	pPage->m_strProgress.Empty();
	

	CString str;
	pPage->m_strProgress.Empty();

	if( bAccessDB )
	{	
		CString strAccesFile = NewDB_GetAccesFileName( pSheet );
		
		str.Format( IDS_NEWDB_CREATE_ACCESDB, strAccesFile );
		pPage->m_strProgress += str;		
		
		TEST_FOR_USER_CANCELED()

		if( bUseAccessTemplate )//copy from 
		{
			CFileFind ff;
			if( ff.FindFile( strAccesFile ) )
			{
				CString strMessage;
				strMessage.Format( IDS_WARNING_FILE_EXIST, strAccesFile );
				if( AfxMessageBox( strMessage, MB_YESNO ) != IDYES )
				{
					pPage->m_strProgress += strFailed;
					NewDB_CreateNewDBCleanUp( pSheet, pPage );
					return false;
				}
			}

			if( !ff.FindFile( strAccessTemplate ) )
			{
				CString strMessage;
				strMessage.Format( IDS_WARNING_FILE_NOT_FOUND, strAccessTemplate );

				AfxMessageBox( strMessage, MB_ICONERROR );
				pPage->m_strProgress += strFailed;
				NewDB_CreateNewDBCleanUp( pSheet, pPage );
				return false;
			}

			if( IsAborted() )
			{
				NewDB_CreateNewDBCleanUp( pSheet, pPage, true );
				return false;
			}

			//copy file
			BOOL bResult = ::CopyFile( strAccessTemplate, strAccesFile, FALSE );
			if( !bResult )
			{
				CString strError;
				strError.Format( IDS_WARNING_CANTCREATE_FILE, (LPCTSTR)strAccesFile );

				pPage->m_strProgress += strFailed;
				NewDB_CreateNewDBCleanUp( pSheet, pPage );

				AfxMessageBox( strError, MB_ICONSTOP );			
				
				return false;			
			}

			pPage->m_strProgress += strSucceded;

			TEST_FOR_USER_CANCELED()

		}
		else // Create new empty database
		{
			CFileFind ff;
			if( ff.FindFile( strAccesFile ) )
			{
				CString strWarning;
				strWarning.Format( IDS_WARNING_FILE_EXIST, strAccesFile );
				if( AfxMessageBox( strWarning, MB_YESNO ) != IDYES )
				{
					pPage->m_strProgress += strFailed;
					NewDB_CreateNewDBCleanUp( pSheet, pPage );
					return false;
				}
			}

			TEST_FOR_USER_CANCELED()

			try
			{
				CFileFind ff;
				if( ff.FindFile( strAccesFile ) )
				{
					BOOL bRes = ::DeleteFile( strAccesFile );
					if( !bRes )
					{
						CString strError;
						strError.Format( IDS_WARNING_CANTDELETE_FILE, (LPCTSTR)strAccesFile );

						pPage->m_strProgress += strFailed;
						NewDB_CreateNewDBCleanUp( pSheet, pPage );

						AfxMessageBox( strError, MB_ICONSTOP );			
						
						return false;			
					}
				}

				ADOX::_CatalogPtr	ptrCat;
				if( S_OK != ptrCat.CreateInstance(__uuidof (ADOX::Catalog) ) )
				{
					pPage->m_strProgress += strFailed;
					AfxMessageBox( IDS_WARNING_CATALOG_OBJECT_FAILED, MB_ICONSTOP | MB_OK );		 
					NewDB_CreateNewDBCleanUp( pSheet, pPage, false );
					return false;
				}

				CString strConn("Provider=Microsoft.JET.OLEDB.4.0;Data source = ");
				strConn += strAccesFile;
				   
				ptrCat->Create( _bstr_t( (LPCSTR)strConn ) );

				pPage->m_strProgress += strSucceded;

				TEST_FOR_USER_CANCELED()

			}
			catch( _com_error &e )
			{
				pPage->m_strProgress += strFailed;
				pPage->m_strProgress += "\nError:";
				pPage->m_strProgress += CString( (const char*)e.Description() );				
				
				NewDB_CreateNewDBCleanUp( pSheet, pPage, false );
				return false;
			}

		}


		CString strConnection;
		strConnection.LoadString( IDS_NEWDB_OPEN_CONNECTION );

		pPage->m_strProgress += "\n";
		pPage->m_strProgress += strConnection;//strSucceded;

		TEST_FOR_USER_CANCELED()
		
		if( S_OK != ptrDBConn->LoadAccessDBase( _bstr_t( (LPCSTR)strAccesFile ) ) )
		{
			pPage->m_strProgress += strFailed;			
			NewDB_CreateNewDBCleanUp( pSheet, pPage, false );
			return false;
		}

		pPage->m_strProgress += strSucceded;

		TEST_FOR_USER_CANCELED()

		//create fields
		if( bCreateNewTable )
		{

			pPage->m_strProgress += "\n";

			_CTableInfo tiNew( "none" );
			TABLEINFO_ARR arTI;
			GetDBFieldInfo( pSheet, arTI, &tiNew ); 
			for( int i=0;i<arTI.GetSize();i++ )
				delete arTI[i];

			arTI.RemoveAll();

			int nFieldsCount = tiNew.m_arrFieldInfo.GetSize();
			

			str.Format( IDS_NEWDB_CREATE_TABLE, pNewTable->m_strTableName, nFieldsCount );
			pPage->m_strProgress += str;
			TEST_FOR_USER_CANCELED()

			if( ptrDBStruct->AddTable( _bstr_t( (LPCSTR)pNewTable->m_strTableName ) ) != S_OK )
			{
				pPage->m_strProgress += strFailed;
				NewDB_CreateNewDBCleanUp( pSheet, pPage, false );
				return false;
			}

			pPage->m_strProgress += strSucceded;
			pPage->m_strProgress += "\n";

			TEST_FOR_USER_CANCELED()
			
			
			for( i=0;i<nFieldsCount;i++ )
			{

				_CFieldInfo* pfi = tiNew.m_arrFieldInfo[i];				
				str.Format( IDS_NEWDB_FIELDS, pfi->m_strFieldName, 
					CDBStructGrid::GetFieldTypeAsString( pfi ) );				
				pPage->m_strProgress += str;

				if( ptrDBStruct->AddField(	_bstr_t( (LPCSTR)pNewTable->m_strTableName ),
											_bstr_t( (LPCSTR)pfi->m_strFieldName ), 
											pfi->m_FieldType, 
											_bstr_t( (LPCSTR)pfi->m_strPrimaryTable ), 
											_bstr_t( (LPCSTR)pfi->m_strPrimaryFieldName )
									) != S_OK )
				{
					pPage->m_strProgress += strFailed;
				}
				else
				{
					pPage->m_strProgress += strSucceded;
					ptrDBStruct->SetFieldInfo( 
										_bstr_t( (LPCSTR)pNewTable->m_strTableName ),
										_bstr_t( (LPCSTR)pfi->m_strFieldName ), 
										_bstr_t( (LPCSTR)pfi->m_strUserName ),
										FALSE,
										FALSE,
										_bstr_t( (LPCSTR)"" ),
										_bstr_t( (LPCSTR)pfi->m_strVTObjectType ) );


				}
				pPage->m_strProgress += "\n";
				
				TEST_FOR_USER_CANCELED()
			}
		}


		if( bUseAccessTemplate )//need Put info to doc about type of binary fields
		{			
			TABLEINFO_ARR arTI;
			GetDBFieldInfo( pSheet, arTI, 0 ); 

			for( int i=0;i<arTI.GetSize();i++ )
			{
				_CTableInfo* pti = arTI[i];
				_bstr_t bstrTable = pti->m_strTableName;

				for( int j=0;j<pti->m_arrFieldInfo.GetSize();j++ )
				{
					_CFieldInfo* pfi = pti->m_arrFieldInfo[j];					
					_bstr_t bstrField = pfi->m_strFieldName;

					short nType = -1;
					BSTR bstrVTObjType = 0;
					if( S_OK != ptrDBStruct->GetFieldInfo( 
										bstrTable, bstrField,
										0, &nType,
										0, 0,
										0, 0,
										0, &bstrVTObjType
										) )
						continue;

					CString strVTObjectType = bstrVTObjType;

					if( bstrVTObjType )
						::SysFreeString( bstrVTObjType );	bstrVTObjType = 0;

					FieldType ft = (FieldType)nType;
					if( ft != ftVTObject )
						continue;

					if( !strVTObjectType.IsEmpty() )
						continue;

					strVTObjectType = szTypeImage;	

					ptrDBStruct->SetFieldInfo(
										bstrTable, bstrField,
										0, FALSE, FALSE,
										0, _bstr_t( (LPCSTR)strVTObjectType )
										);

					
				}
			}

			

			for( i=0;i<arTI.GetSize();i++ )
				delete arTI[i];

			arTI.RemoveAll();

		}

	}
	else//adv connection
	{		

		CString strConnection;
		strConnection.LoadString( IDS_NEWDB_OPEN_CONNECTION );

		pPage->m_strProgress += "\n";
		pPage->m_strProgress += strConnection;//strSucceded;

		TEST_FOR_USER_CANCELED()
		
		if( S_OK != ptrDBConn->OpenConnectionFromString( _bstr_t( (LPCSTR)strAdvConnString ) ) )
		{
			pPage->m_strProgress += strFailed;
			NewDB_CreateNewDBCleanUp( pSheet, pPage, false );
			return false;
		}

		pPage->m_strProgress += strSucceded;

		TEST_FOR_USER_CANCELED()
	}

	//create query object
	IUnknown* punkQuery = ::CreateTypedObject( szTypeQueryObject );
	if( !punkQuery )
		return false;

	ISelectQueryPtr ptrQ = punkQuery;
	punkQuery->Release();	punkQuery = 0;

	if( ptrQ == 0 )
		return false;

	
	CQueryNamePage* pPageName = 
		(CQueryNamePage*)pSheet->FindPage( RUNTIME_CLASS(CQueryNamePage), 0 );

	
	if( pPageName )
	{
		INamedObject2Ptr ptrND( ptrQ );
		if( ptrND )
			ptrND->SetName( _bstr_t( (LPCSTR) pPageName->m_strQueryName ) );
	}


	CWizardsData wd;
	Update_EditQueryPage( &wd, pQuerySelect, false );
	Update_TumbnailPage( &wd, pQueryTumbnail, false );

	WriteWizardDataToQuery( ptrQ, &wd );

	INamedDataPtr ptrND( ptrDBaseDoc );
	if( ptrND ) 
		ptrND->SetValue( 0, _variant_t( (IUnknown*)ptrQ ) );

	ptrDBaseDoc->SetActiveQuery( ptrQ );	
	
	pSheet->SetWizardButtons( PSWIZB_FINISH );

	ptrQ->Open();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CWizardSheet
//
IMPLEMENT_DYNAMIC(CWizardSheet, CPropertySheet)

CWizardSheet::CWizardSheet()
{
	m_pPool = 0;
}

/////////////////////////////////////////////////////////////////////////////
CWizardSheet::CWizardSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_pPool = 0;
}

/////////////////////////////////////////////////////////////////////////////
CWizardSheet::CWizardSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_pPool = 0;
}

/////////////////////////////////////////////////////////////////////////////
CWizardSheet::~CWizardSheet()
{
}

/////////////////////////////////////////////////////////////////////////////
void CWizardSheet::SetWizardPool( CWizardPool* pWizardPool )
{
	m_pPool = pWizardPool;
}

/////////////////////////////////////////////////////////////////////////////
CPropertyPage* CWizardSheet::FindPage( CRuntimeClass* prc, int* pnPageNum )
{
	int nNum = GetPageCount();
	for( int i=0;i<nNum;i++ )
	{
		CPropertyPage* pPage = GetPage( i );
		if( pPage->IsKindOf(prc) )
		{
			if( pnPageNum )
				*pnPageNum = i;

			return pPage;
		}

	}

	return 0; 
}

/////////////////////////////////////////////////////////////////////////////
bool CWizardSheet::ActivatePage( CRuntimeClass* prc )
{
	CPropertyPage* pPage = FindPage( prc, 0 );
	if( pPage )
	{
		SetActivePage( pPage );
		return false;
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
bool CWizardSheet::_OnWizardSetActivePage( CPropertyPage* pPage )
{
	if( !m_pPool )
		return true;

	return m_pPool->_OnWizardSetActivePage( this, pPage );
}

/////////////////////////////////////////////////////////////////////////////
bool CWizardSheet::_OnWizardBack( CPropertyPage* pPage )
{
	if( !m_pPool )
		return true;

	return m_pPool->_OnWizardBack( this, pPage );
}

/////////////////////////////////////////////////////////////////////////////
bool CWizardSheet::_OnWizardNext( CPropertyPage* pPage )
{
	if( !m_pPool )
		return true;

	return m_pPool->_OnWizardNext( this, pPage );
}

/////////////////////////////////////////////////////////////////////////////
bool CWizardSheet::_OnWizardFinish( CPropertyPage* pPage )
{
	if( !m_pPool )
		return true;

	return m_pPool->_OnWizardFinish( this, pPage );
}


BEGIN_MESSAGE_MAP(CWizardSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CWizardSheet)
	ON_WM_CREATE()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardSheet message handlers

/////////////////////////////////////////////////////////////////////////////
CString MakeNameWithoutColon(LPCTSTR lpstrName)
{
	CString s(lpstrName);
	int n = s.Find(':');
	if (n == -1)
		return s;
	else
		return s.Left(n);
}


bool GetDBFieldInfo( CWizardSheet* pSheet, TABLEINFO_ARR& arrTableOld, _CTableInfo* ptiNew )
{
	if( !pSheet )
		return false;

	GET_NEW_DB_WIZARD_DATA( false )
	
	//if( parrTableOld )
	{
		ADOX::_CatalogPtr ptrCat;

		if( bAccessDB && bUseAccessTemplate )
		{
			ptrCat = OpenCatalog( strAccessTemplate, true, false );
		}		
		else
		if( !bAccessDB )
		{
			ptrCat = OpenCatalog( strAdvConnString, false, false );
		}

		if( ptrCat )
		{
			if( !CDBaseEngine::CreateTableCacheOnCatalog( ptrCat, 0, arrTableOld ) )
				return false;
		}
	}

	if( ptiNew && bAccessDB && bCreateNewTable )
	{
		ptiNew->m_strTableName = pNewTable->m_strTableName;

		if( !bSimpleFieldCreationType )
		{

			int nSizeNew = 0;
			_CFieldInfo* parfi = NULL;
			pAdvFieldConstr->m_grid.GetTableStruct( &parfi, &nSizeNew );			
			
			for( int i=0;i<nSizeNew;i++)
			{
				_CFieldInfo* pfi = new _CFieldInfo;
				ptiNew->m_arrFieldInfo.Add( pfi );
				pfi->CopyFrom( &parfi[i] );
			}			

			if( parfi )
				delete[] parfi;
		}
		else
		{
			{
				_CFieldInfo* pfi = new _CFieldInfo;
				ptiNew->m_arrFieldInfo.Add( pfi );
				pfi->m_strFieldName = CString( "id_" ) +  CString( pNewTable->m_strTableName );
				pfi->m_FieldType = ftPrimaryKey;
				
			}

			for( field_create_info *pcf = pSimpleFieldConstr->m_pcreate; pcf; pcf = pcf->next )
			{
				for( int i=0;i<pcf->count;i++ )
				{
					_CFieldInfo* pfi = new _CFieldInfo;
					ptiNew->m_arrFieldInfo.Add( pfi );
					pfi->m_strFieldName.Format( "%s%d", pcf->sz_fldbase, i+1 );
					CString sName = MakeNameWithoutColon(pcf->sz_caption);
					if (pcf->count == 1)
						pfi->m_strUserName = sName;
					else
						pfi->m_strUserName.Format("%s%d", (LPCTSTR)sName, i+1);
					pfi->m_FieldType = pcf->type;
					pfi->m_strVTObjectType = pcf->sz_objtype;
				}
			}

		}
	}

	return true;

}



bool IsValidWord( CString str )
{
	if( str.IsEmpty() )
		return false;

	CString strTest;
	for( int i=0;i<str.GetLength();i++ )
	{
		if( str[i] != '_' )
			strTest += str[i];
	}


	if( strTest.IsEmpty() )
		return false;


	for( i=0;i<strTest.GetLength();i++ )
	{
		if( !::IsCharAlphaNumeric( (TCHAR)strTest[i] ) )
			return false;
	}

	if( IsSQLReservedWord( str ) )
		return false;

	return true;
}


/////////////////////////////////////////////////////////////////////////////
bool IsValidTableName( CString strTableName, TABLEINFO_ARR& arrTable, CString& strError )
{		
	if( !IsValidWord( strTableName ) )
	{
		strError.Format( IDS_TABLE_NAME_NOT_VALID, strTableName );
		return false;
	}

	for( int i=0;i<arrTable.GetSize();i++ ) 
	{
		_CTableInfo* pti = arrTable[i];

		CString s1, s2;
		s1 = strTableName;
		s2 = pti->m_strTableName;

		s1.MakeLower();
		s2.MakeLower();

		if( s1 == s2 )
		{
			strError.Format( IDS_TABLE_EXIST, strTableName );
			return false;
		}
		
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
ADOX::_CatalogPtr OpenCatalog( CString strConnStr, bool bAccessDB, bool bReportError )
{
	ADOX::_CatalogPtr ptrCat;
	ADO::_ConnectionPtr ptrConn;	

	if( bAccessDB )
	{
		CFileFind ff;
		if( !ff.FindFile( strConnStr ) )
		{
			if( bReportError )
			{
				CString strMessage;
				strMessage.LoadString( IDS_WARNING_FILE_NOT_FOUND );
				CString strError;
				strError.Format( (LPCTSTR)strMessage, (LPCTSTR)strConnStr );
				AfxMessageBox( strError, MB_ICONSTOP );			
			}

			return 0;
		}
	}
	
	try
	{
		
		if( !SUCCEEDED( ptrConn.CreateInstance( __uuidof(ADO::Connection) ) ) )		
		{
			if( bReportError )
				AfxMessageBox( IDS_WARNING_CONNECTION_OBJECT_FAILED, MB_ICONSTOP | MB_OK );

			return 0;
		}

		
		if( S_OK != ptrCat.CreateInstance(__uuidof (ADOX::Catalog) ) )
		{
			if( bReportError )
				AfxMessageBox( IDS_WARNING_CATALOG_OBJECT_FAILED, MB_ICONSTOP | MB_OK );		 

			return 0;
		}

		CString strConnectionString;

		if( bAccessDB )
		{		
			strConnectionString = DB_DEFAULT_CONNECTION_STRING;
			strConnectionString += ";";
			strConnectionString += DB_DATASOURCE;
			strConnectionString += strConnStr;
		}
		else
		{
			strConnectionString = strConnStr;
		}


		_bstr_t ConnectionString( strConnectionString );
		_bstr_t UserID("Admin");
		_bstr_t Password;

		
		ptrConn->Open( ConnectionString, UserID, Password, 0 );

		ptrCat->PutActiveConnection( _variant_t( (IDispatch*)ptrConn ) );
		

		return ptrCat;

	}
	catch (_com_error &e)
	{
		if( bReportError )
			dump_com_error(e);

		return 0;
	}


	return 0;
	
}



/////////////////////////////////////////////////////////////////////////////
//
//	Help support
//
/////////////////////////////////////////////////////////////////////////////
LRESULT CWizardSheet::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( !m_strHelpFileName.IsEmpty() )
	{
		if( CHelpDlgImpl::IsHelpMsg( message ) )
		{
			HelpDisplay( m_strHelpFileName );
			return 1L;//CHelpDlgImpl::FilterHelpMsg( message, wParam, lParam );
		}

	}
	
	return CPropertySheet::WindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
int CWizardSheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;

	if( !m_strHelpFileName.IsEmpty() )
	{
		SetWindowLong( GetSafeHwnd(), GWL_EXSTYLE, 
					GetWindowLong( GetSafeHwnd(), GWL_EXSTYLE ) | WS_EX_CONTEXTHELP );
	
		//CHelpDlgImpl::InitHelpImlp( GetSafeHwnd(), "DBaseDoc", m_strHelpFileName );
	}
	
	return 0;
}
