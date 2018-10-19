// ViewAXObjectPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "viewax.h"
#include "ViewAXObjectPropPage.h"
#include "GridCellCombo.h"

#include "ViewAXCtl.h"

#include "obj_types.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define _LIBNAME_CONTROLS_ "Controls.lib"


/////////////////////////////////////////////////////////////////////////////
// CViewAXObjectPropPage dialog

IMPLEMENT_DYNCREATE(CViewAXObjectPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CViewAXObjectPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CViewAXObjectPropPage)
	ON_BN_CLICKED(IDC_USE_ACTIVE_VIEW, OnUseActiveView)		
	ON_BN_CLICKED(IDC_VIEW_AUTO_ASSIGNED, OnViewAutoAssigned)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid
// {EF4817A4-B3AD-47f6-9CF4-17652866A1B6}
GUARD_IMPLEMENT_OLECREATE_CTRL(CViewAXObjectPropPage, "VIEWAX.ViewAXObjectPropPage.1",
0xef4817a4, 0xb3ad, 0x47f6, 0x9c, 0xf4, 0x17, 0x65, 0x28, 0x66, 0xa1, 0xb6);


/////////////////////////////////////////////////////////////////////////////
// CViewAXObjectPropPage::CViewAXObjectPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CViewAXObjectPropPage

BOOL CViewAXObjectPropPage::CViewAXObjectPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	return UpdateRegistryPage(bRegister, AfxGetInstanceHandle(), IDS_VIEWAX_PPG);
}


/////////////////////////////////////////////////////////////////////////////
// CViewAXPropPage::CViewAXPropPage - Constructor

CViewAXObjectPropPage::CViewAXObjectPropPage() :
	COlePropertyPage(IDD, IDS_VIEWAX_OBJECT_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CViewAXObjectPropPage)
	m_bUseActiveView = FALSE;
	m_strViewName = _T("");
	m_bAutoAssignedView = FALSE;
	//}}AFX_DATA_INIT
}


void CViewAXObjectPropPage::DoDataExchange(CDataExchange* pDX)
{	
	//{{AFX_DATA_MAP(CViewAXObjectPropPage)
	DDP_Check(pDX, IDC_USE_ACTIVE_VIEW, m_bUseActiveView, _T("UseActiveView") );
	DDX_Check(pDX, IDC_USE_ACTIVE_VIEW, m_bUseActiveView);
	DDP_CBString(pDX, IDC_VIEW_NAME, m_strViewName, _T("ViewName") );
	DDX_CBString(pDX, IDC_VIEW_NAME, m_strViewName);
	DDP_Check(pDX, IDC_VIEW_AUTO_ASSIGNED, m_bAutoAssignedView, _T("ViewAutoAssigned") );
	DDX_Check(pDX, IDC_VIEW_AUTO_ASSIGNED, m_bAutoAssignedView);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CViewAXObjectPropPage message handlers
BOOL CViewAXObjectPropPage::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();			
	

	CRect rcGrid;
	GetDlgItem(IDC_OBJECT_LIST)->GetWindowRect( &rcGrid );
	ScreenToClient( &rcGrid );

	m_grid.Create( rcGrid, this,  IDC_GRID );	

	CString strObject, strType;

	strObject.LoadString( IDS_OBJECT_HEADER );
	strType.LoadString( IDS_TYPE_HEADER );

	m_grid.InsertColumn( strObject, DT_CENTER|DT_VCENTER );		
	m_grid.InsertColumn( strType, DT_CENTER|DT_VCENTER );

	m_grid.SetColumnWidth( 0, rcGrid.Width() / 2 - 10 );
	m_grid.SetColumnWidth( 1, rcGrid.Width() / 2 - 10 );		


	m_grid.EnableTitleTips( FALSE );
	m_grid.EnableDragAndDrop( FALSE );	
	m_grid.EnableSelection( TRUE );
	

	m_grid.SetFixedRowCount(1);	

	ReBuildGridContent( 1, 0 );

	SetOnOf( FALSE );


	CComboBox* pComo = (CComboBox*)GetDlgItem( IDC_VIEW_NAME );
	if( pComo )
	{
		int cur_sel=pComo->GetCurSel();
		CString cur_text;
		if(pComo->GetCount()>0)
			pComo->GetLBText(cur_sel,cur_text);
		else
			cur_text="";
		pComo->ResetContent();
		CStringArray arViewProgID;
		GetAvailableViewProgID( arViewProgID );
		bool buse_filter = ::GetValueInt( ::GetAppUnknown(), "\\ViewAX", "UseViewFilter", 0 ) == 1L;
		CString strFilter = ::GetValueString( ::GetAppUnknown(), "\\ViewAX", "ViewFilter", "" );
		int cell_count=m_grid.GetRowCount();
		CString strObjectType_	= m_grid.GetItemText( cell_count-1, 1 );
		BOOL flag=false;
		for(int i=0;i<cell_count-1;i++)
			if(m_grid.GetItemText(i+1,1)=="Statistics Data Table")
				flag=true;
		for(int i=0;i<cell_count-1;i++)
		{
			sptrIApplication	sptrA_( GetAppUnknown() );
			CString strType;
			CString	strProgID_;
			LONG_PTR	lDocTemplPos_;

			strType = m_grid.GetItemText(i+1,1);
			if(strType=="Статистика")
				strType="Statistics";
			else
				if(strType=="Таблица")
					strType="Statistics Data Table";
				else
					if(strType=="Графика")
						strType="Drawing Object List";
					else
						if(strType=="Изображение")
							strType="Image";
						else
							if(strType=="Объекты")
								strType="Object List";
							else
								if(strType=="Маски")
									strType="Binary";

			if(strType=="Statistics") 
				strType="StatObject";
			if(strType=="Statistics Data Table")
				strType="StatTable";
						
			_bstr_t	bstrType_ = strType;

			sptrA_->GetFirstDocTemplPosition( &lDocTemplPos_ );

			while( lDocTemplPos_ )
			{
				BSTR	bstrTemplName_;
				sptrA_->GetNextDocTempl( &lDocTemplPos_, &bstrTemplName_, 0 );
				CString	strTemplName_( bstrTemplName_ );
				::SysFreeString( bstrTemplName_ );

				strTemplName_=strTemplName_+"\\"+"views";
					
				CCompManager		m( strTemplName_ );
				CCompRegistrator	r( strTemplName_ );

				for( int nView=0; nView < m.GetComponentCount(); nView ++ )
				{
					sptrIView	sptrV( m.GetComponent( nView, false ) );
					DWORD	dwMatch_;

					sptrV->GetMatchType( bstrType_, &dwMatch_ );

					if( dwMatch_ > 0 )
					{
						strProgID_ = m.GetComponentName( nView );
						if(buse_filter)
						{
							if( strFilter.Find( strProgID_ ) != -1 )
							{
								CString stemp;
								CString sres;
								flag=false;
								sres=GetViewNameByProgID(strProgID_, true );
								for(int i=0; i<pComo->GetCount();i++)
								{
									pComo->GetLBText(i,stemp);
									if(stemp==sres)
										flag=true;
								}
								if(!flag)
									pComo->AddString( sres );
							}
						}
						else
						{
							CString stemp;
							CString sres;
							flag=false;
							sres=GetViewNameByProgID(strProgID_, true );
							for(int i=0; i<pComo->GetCount();i++)
							{
								pComo->GetLBText(i,stemp);
								if(stemp==sres)
									flag=true;
							}
							if(!flag)
								pComo->AddString( sres );
						}
					}
				}
			}
		}
		flag=false;
		CString stemp;
		for(int i=0;i<pComo->GetCount();i++)
		{
			pComo->GetLBText(i,stemp);
			if(stemp==cur_text)
			{
				pComo->SetCurSel(i);
				flag=true;
				break;
			}
		}
		if(!flag)
			pComo->SetCurSel(0);
	}	
	
	return TRUE;  	              
}

/////////////////////////////////////////////////////////////////////////////
sptrIViewCtrl CViewAXObjectPropPage::GetViewAXControl()
{
	ULONG nControls = 0;
	LPDISPATCH FAR* ppDisp = GetObjectArray(&nControls);
	
	if( ppDisp[0] == NULL )
		return NULL;

	sptrIViewCtrl sptrV( ppDisp[0] );
	if( sptrV == NULL )
		return NULL;

	return sptrV;
}


/////////////////////////////////////////////////////////////////////////////
void CViewAXObjectPropPage::ReBuildGridContent( int nFocusedRow, int nFocusedCol )
{
	m_grid.SetRowCount( 1 );

	
	sptrIViewCtrl sptrV( GetViewAXControl() );
	if( sptrV == NULL )
		return;

	int nRowCount = 1;
	CString strActivebject;
	strActivebject.LoadString( IDS_ACTIVE_OBJECT );


	CStringArray arTypes;
	CStringArray arUserNames;
	::GetAllObjectTypes( arTypes, arUserNames );


	LONG_PTR lPos = 0;
	sptrV->GetFirstObjectPosition( &lPos );
	while( lPos )
	{
		BSTR bstrObjectName, bstrObjectType;
		BOOL bActiveObject = FALSE;
		bstrObjectName = bstrObjectType = NULL;
		
		sptrV->GetNextObject( &bstrObjectName, &bActiveObject, 
								&bstrObjectType, &lPos );

		nRowCount++;
		m_grid.SetRowCount( nRowCount );		

		CString strObjectType = bstrObjectType;
		::SysFreeString( bstrObjectType );	bstrObjectType = 0;

		CString strObjectName = bstrObjectName;
		::SysFreeString( bstrObjectName );	bstrObjectName = 0;

		for( int i=0;i<arTypes.GetSize();i++ )
		{
			if( strObjectType == arTypes[i] )
			{
				strObjectType = arUserNames[i];
			}
		}

		m_grid.SetItemText( nRowCount - 1, 0,  ( bActiveObject == TRUE ? strActivebject : strObjectName ) );
		m_grid.SetItemText( nRowCount - 1, 1,  (LPCSTR)_bstr_t( strObjectType ) );		

		m_grid.SetCellType( nRowCount - 1, 0, RUNTIME_CLASS(CGridCellCombo) );			

		m_grid.SetCellType( nRowCount - 1, 1, RUNTIME_CLASS(CGridCellCombo) );
		
	}
	
	
	if( nFocusedRow > 0 && nFocusedRow < m_grid.GetRowCount() && 
		nFocusedCol >= 0 && nFocusedCol < 2 )
	{
		m_grid.SetFocusCell( nFocusedRow, nFocusedCol );
	}



}

/////////////////////////////////////////////////////////////////////////////
void CViewAXObjectPropPage::SetComboBox( int nRow, int nColumn )
{
	if( nRow < 1 )
		return;

	if( !m_grid.IsValid( nRow, nColumn ) )
		return;

	CStringArray options;

	sptrIApplication sptrApp( ::GetAppUnknown() );
	if( sptrApp == NULL )
		return;

	IUnknown* punkDoc = NULL;
	sptrApp->GetActiveDocument( &punkDoc );
	if( punkDoc == NULL )
		return;

	sptrIDocument sptrDoc( punkDoc );
	punkDoc->Release();

	if( sptrDoc == NULL )
		return;

	//set combo boxes context
	if( nColumn == 0 )
	{

		CString strActivebject;
		strActivebject.LoadString( IDS_ACTIVE_OBJECT );
		options.Add( strActivebject );
		
		sptrIDataTypeManager sptrM( sptrDoc );		

		if( sptrM == NULL )
			return;


		long	nTypesCounter = 0;
		sptrM->GetTypesCount(&nTypesCounter);
		// for all types in documentA
		for (long nType = 0; nType < nTypesCounter; nType++)
		{

			LONG_PTR	lpos = 0;
			// for all objects in type
			sptrM->GetObjectFirstPosition(nType, &lpos);

			while (lpos)
			{
				// get next object
				IUnknown	*punkParent = 0;
				sptrM->GetNextObject(nType, &lpos, &punkParent);

				{
					INamedObject2Ptr spNO2( punkParent );
					if( spNO2 ) 
					{
						BSTR bstrObjectName;
						spNO2->GetName( &bstrObjectName );
						
						options.Add( CString(bstrObjectName));
					
						::SysFreeString( bstrObjectName );					
					}
				}			
				
				punkParent->Release();
			}
		}
		
	}
	else
	{
		CStringArray arTypes;
		CStringArray arUserNames;
		::GetAllObjectTypes( arTypes, arUserNames );

		bool buse_filter = ::GetValueInt( ::GetAppUnknown(), "\\ViewAX", "UseTypeFilter", 0 ) == 1L;
		CString strFilter = ::GetValueString( ::GetAppUnknown(), "\\ViewAX", "TypeFilter", "" );

		for( int i=0;i<arUserNames.GetSize();i++ )
		{
			if( buse_filter && i < arTypes.GetSize() )
			{
				CString str_type = "";
				str_type += arTypes[i];				
				if( strFilter.Find( str_type ) != -1 )				
					options.Add( arUserNames[i] );
			}
			else
				options.Add( arUserNames[i] );
		}		
	}


	CGridCellCombo *pCell = (CGridCellCombo*) m_grid.GetCell( nRow, nColumn );
	
	pCell->SetOptions( options );

	if( nColumn == 0 )
		pCell->SetStyle( CBS_DROPDOWN );
	else
		pCell->SetStyle( CBS_DROPDOWNLIST );
	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CViewAXObjectPropPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	if( wParam == IDC_GRID )
	{
		NM_GRIDVIEW* pnmgv = (NM_GRIDVIEW*)lParam;
		if( pnmgv )
		{
			if( pnmgv->hdr.code == GVN_SELCHANGED )
			{				
				SetOnOf();
				SetComboBox( pnmgv->iRow, pnmgv->iColumn );


			}
			else
			if( pnmgv->hdr.code == GVN_ENDLABELEDIT )
			{
				int nRow = pnmgv->iRow;	
				LONG_PTR lPos = GetPositionByIndex(nRow - 1);
				int nCol = pnmgv->iColumn;
				if( lPos > 0 && (nCol == 0 || nCol == 1 ) )
				{
					sptrIViewCtrl sptrV( GetViewAXControl() );
					if( sptrV != NULL )
					{
						BSTR bstrObjectName, bstrObjectType;
						BOOL bActiveObject;
						
						bActiveObject = FALSE;

						bstrObjectName = bstrObjectType = 0;

						LONG_PTR nSavePos = lPos;
						sptrV->GetNextObject( &bstrObjectName, &bActiveObject, &bstrObjectType, &lPos );

						CString strObjectName, strObjectType;
						CString strActiveObject;
						strActiveObject.LoadString( IDS_ACTIVE_OBJECT );

						strObjectName	= m_grid.GetItemText( nRow, 0 );
						strObjectType	= m_grid.GetItemText( nRow, 1 );

						CStringArray arTypes;
						CStringArray arUserNames;
						GetAllObjectTypes( arTypes, arUserNames );

						for( int i=0;i<arUserNames.GetSize();i++ )
						{
							if( strObjectType == arUserNames[i] )
							{
								strObjectType = arTypes[i];
								break;
							}
						}

						if( strActiveObject == strObjectName )
							bActiveObject = TRUE;
						else
							bActiveObject = FALSE;

						//auto complete
						if( nCol == 0 && !bActiveObject )//Edit object Name
						{
							sptrIApplication sptrApp( ::GetAppUnknown() );
							if( sptrApp != NULL )
							{
								IUnknown* punkDoc = NULL;
								sptrApp->GetActiveDocument( &punkDoc );
								if( punkDoc != NULL )
								{
									IUnknown* punkObject = ::FindObjectByName( punkDoc, strObjectName );
									CString strAutoObjectType = ::GetObjectKind( punkObject );
									if( punkObject )
										punkObject->Release();
									punkDoc->Release();	

									if( !strAutoObjectType.IsEmpty() )
										strObjectType = strAutoObjectType;
								}
							}
						}
						else
						if( nCol == 1 && !bActiveObject )//Edit type
						{
							sptrIApplication sptrApp( ::GetAppUnknown() );
							if( sptrApp != NULL )
							{
								IUnknown* punkDoc = NULL;
								sptrApp->GetActiveDocument( &punkDoc );
								if( punkDoc != NULL )
								{
									sptrIDataContext2 sptrDC( punkDoc );
									punkDoc->Release();	
									if( sptrDC )
									{										
										LONG_PTR lPos = 0;
										sptrDC->GetFirstObjectPos( _bstr_t( (LPCSTR)strObjectType ), 
																	&lPos );
										
										IUnknown* punkObject = NULL;
										sptrDC->GetNextObject( _bstr_t( (LPCSTR)strObjectType ), 
																	&lPos, &punkObject );
										
										if( punkObject )
										{
											strObjectName = ::GetObjectName( punkObject );											
											punkObject->Release();
										}
									}									
								}
							}
						}

						sptrV->EditAt( nSavePos, _bstr_t( (LPCSTR)strObjectName ), 
													bActiveObject, 
													_bstr_t( (LPCSTR)strObjectType ) );

						::SysFreeString( bstrObjectName );
						::SysFreeString( bstrObjectType );

						ReBuildGridContent( nRow, nCol );
					}
				}
				CComboBox* pComo = (CComboBox*)GetDlgItem( IDC_VIEW_NAME );
				if( pComo )
				{
					int cur_sel=pComo->GetCurSel();
					CString cur_text;
					if(pComo->GetCount()>0)
						pComo->GetLBText(cur_sel,cur_text);
					else
						cur_text="";
					pComo->ResetContent();
					CStringArray arViewProgID;
					GetAvailableViewProgID( arViewProgID );
					bool buse_filter = ::GetValueInt( ::GetAppUnknown(), "\\ViewAX", "UseViewFilter", 0 ) == 1L;
					CString strFilter = ::GetValueString( ::GetAppUnknown(), "\\ViewAX", "ViewFilter", "" );
					int cell_count=m_grid.GetRowCount();
					CString strObjectType_	= m_grid.GetItemText( cell_count-1, 1 );
					BOOL flag=false;
					for(int i=0;i<cell_count-1;i++)
						if(m_grid.GetItemText(i+1,1)=="Statistics Data Table")
							flag=true;
					for(int i=0;i<cell_count-1;i++)
					{
						sptrIApplication	sptrA_( GetAppUnknown() );
						CString strType;
						CString	strProgID_;
						LONG_PTR	lDocTemplPos_;

						strType = m_grid.GetItemText(i+1,1);
						if(strType=="Статистика")
							strType="Statistics";
						else
							if(strType=="Таблица")
								strType="Statistics Data Table";
							else
								if(strType=="Графика")
									strType="Drawing Object List";
								else
									if(strType=="Изображение")
										strType="Image";
									else
										if(strType=="Объекты")
											strType="Object List";
										else
											if(strType=="Маски")
												strType="Binary";
						if(strType=="Statistics") 
							strType="StatObject";
						if(strType=="Statistics Data Table")
							strType="StatTable";
						
						_bstr_t	bstrType_ = strType;

						sptrA_->GetFirstDocTemplPosition( &lDocTemplPos_ );

						while( lDocTemplPos_ )
						{
							BSTR	bstrTemplName_;
							sptrA_->GetNextDocTempl( &lDocTemplPos_, &bstrTemplName_, 0 );
							CString	strTemplName_( bstrTemplName_ );
							::SysFreeString( bstrTemplName_ );

							strTemplName_=strTemplName_+"\\"+"views";
								
							CCompManager		m( strTemplName_ );
							CCompRegistrator	r( strTemplName_ );

							for( int nView=0; nView < m.GetComponentCount(); nView ++ )
							{
								sptrIView	sptrV( m.GetComponent( nView, false ) );
								DWORD	dwMatch_;

								sptrV->GetMatchType( bstrType_, &dwMatch_ );

								if( dwMatch_ > 0 )
								{
									strProgID_ = m.GetComponentName( nView );
									if(buse_filter)
									{
										if( strFilter.Find( strProgID_ ) != -1 )
										{
											CString stemp;
											CString sres;
											flag=false;
											sres=GetViewNameByProgID(strProgID_, true );
											for(int i=0; i<pComo->GetCount();i++)
											{
												pComo->GetLBText(i,stemp);
												if(stemp==sres)
													flag=true;
											}
											if(!flag)
												pComo->AddString( sres );
										}
									}
									else
									{
										CString stemp;
										CString sres;
										flag=false;
										sres=GetViewNameByProgID(strProgID_, true );
										for(int i=0; i<pComo->GetCount();i++)
										{
											pComo->GetLBText(i,stemp);
											if(stemp==sres)
												flag=true;
										}
										if(!flag)
											pComo->AddString( sres );
									}
								}
							}
						}
						/*if(buse_filter)
						{
							if( strFilter.Find( strProgID ) != -1 )
								pComo->AddString( GetViewNameByProgID(strProgID, true ) );
						}
						else
							pComo->AddString( GetViewNameByProgID(strProgID, true ) );*/
					}
					flag=false;
					CString stemp;
					for(int i=0;i<pComo->GetCount();i++)
					{
						pComo->GetLBText(i,stemp);
						if(stemp==cur_text)
						{
							pComo->SetCurSel(i);
							flag=true;
							break;
						}
					}
					if(!flag)
						pComo->SetCurSel(0);
				}	
			}
		}
	}

	return COlePropertyPage::OnNotify(wParam, lParam, pResult);
}


/////////////////////////////////////////////////////////////////////////////
void CViewAXObjectPropPage::SetOnOf( BOOL bUpdate )
{
	BOOL bUseActiveView = FALSE;
	BOOL bAutoAssignedView = FALSE;
	if( !bUpdate )
	{
		bUseActiveView		= m_bUseActiveView;
		bAutoAssignedView	= m_bAutoAssignedView;
	}
	else
	{
		
		bUseActiveView		= ((CButton*)GetDlgItem(IDC_USE_ACTIVE_VIEW))->GetCheck( );
		bAutoAssignedView	= ((CButton*)GetDlgItem(IDC_VIEW_AUTO_ASSIGNED))->GetCheck( );
	}

	if( bUseActiveView )
	{
		m_grid.EnableWindow( FALSE );
		GetDlgItem(IDC_ADD)->EnableWindow( FALSE );
		GetDlgItem(IDC_DELETE)->EnableWindow( FALSE );
		GetDlgItem(IDC_VIEW_AUTO_ASSIGNED)->EnableWindow( FALSE );		
		GetDlgItem(IDC_VIEW_NAME)->EnableWindow( FALSE );
	}	
	else
	{
		m_grid.EnableWindow( TRUE );
		GetDlgItem(IDC_VIEW_AUTO_ASSIGNED)->EnableWindow( TRUE );

		if( bAutoAssignedView )
			GetDlgItem(IDC_VIEW_NAME)->EnableWindow( FALSE );
		else
			GetDlgItem(IDC_VIEW_NAME)->EnableWindow( TRUE );


		GetDlgItem(IDC_ADD)->EnableWindow( TRUE );

		int nRow = m_grid.GetFocusCell().row;		

		int nRowCount = m_grid.GetRowCount();

		if( nRow > 0 && nRow < nRowCount )
			GetDlgItem(IDC_DELETE)->EnableWindow( TRUE );
		else
			GetDlgItem(IDC_DELETE)->EnableWindow( FALSE );		
		

	}

	

}

/////////////////////////////////////////////////////////////////////////////
void CViewAXObjectPropPage::OnUseActiveView() 
{
	SetOnOf();	
}

/////////////////////////////////////////////////////////////////////////////
void CViewAXObjectPropPage::OnAutoAssignedView() 
{
	SetOnOf();
}

/////////////////////////////////////////////////////////////////////////////
void CViewAXObjectPropPage::OnAutoAssigned() 
{
	SetOnOf();	
}

/////////////////////////////////////////////////////////////////////////////
void CViewAXObjectPropPage::OnViewAutoAssigned() 
{
	SetOnOf();	
}

void CViewAXObjectPropPage::OnAdd() 
{
	int nRow = m_grid.GetFocusCell().row;		
	int nRowCount = m_grid.GetRowCount();

	sptrIViewCtrl sptrV( GetViewAXControl() );
	if( sptrV == NULL )
		return;

	LONG_PTR lPos = GetPositionByIndex(nRow - 1);

	sptrV->InsertAfter( lPos, _bstr_t(""), TRUE, _bstr_t(szTypeImage) );

	ReBuildGridContent( 1, 0 );	
}


LONG_PTR CViewAXObjectPropPage::GetPositionByIndex(int nIndex)
{
	sptrIViewCtrl sptrV( GetViewAXControl() );
	if( sptrV == NULL )
		return -1;
		
	int i = 0;
	LONG_PTR nPos = 0;
	sptrV->GetFirstObjectPosition( &nPos );
	while( nPos )
	{
		if( nIndex == i )
			return nPos;
		sptrV->GetNextObject( NULL, NULL, NULL, &nPos );	
		i++;
	}

	return -1;

}

/////////////////////////////////////////////////////////////////////////////
void CViewAXObjectPropPage::OnDelete() 
{
	sptrIViewCtrl sptrV( GetViewAXControl() );
	if( sptrV == NULL )
		return;

	int nRow = m_grid.GetFocusCell().row;	
	if( nRow < 0 )
		return;

	LONG_PTR lPos = GetPositionByIndex(nRow - 1);

	sptrV->RemoveAt( lPos );
	
	ReBuildGridContent( 1, 0 );	
}
