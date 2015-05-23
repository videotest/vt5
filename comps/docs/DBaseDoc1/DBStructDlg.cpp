// DBStructDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "DBStructDlg.h"

#include "ChoiceTable.h"
#include "Wizards.h"

#include "constant.h"
#include "oledata.h"

/////////////////////////////////////////////////////////////////////////////
// CDBStructDlg dialog
CDBStructDlg::CDBStructDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDBStructDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDBStructDlg)
	m_strTable = _T("");
	//}}AFX_DATA_INIT	
	m_editMode = emdNotDefine;	
	m_pti = 0;
	m_bShowButtons = false;
}

CDBStructDlg::~CDBStructDlg()
{
	if( m_pti )
		delete m_pti;	m_pti = 0;
}


void CDBStructDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDBStructDlg)
	DDX_Text(pDX, IDC_CURENT_TABLE, m_strTable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDBStructDlg, CDialog)
	//{{AFX_MSG_MAP(CDBStructDlg)
	ON_WM_SIZE()	
	ON_BN_CLICKED(IDC_ADD_TABLE, OnAddTable)
	ON_BN_CLICKED(IDC_CHOICE_TABLE, OnChoiceTable)
	ON_BN_CLICKED(IDC_DELETE_TABLE, OnDeleteTable)
	ON_BN_CLICKED(IDC_DELETE_ROW, OnDeleteRow)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
void CDBStructDlg::SetDBaseDoc( IUnknown* punkDoc )
{
	m_ptrDB = punkDoc;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBStructDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	VERIFY( m_grid.Create( 
							CRect(0,0,0,0), this, IDC_GRID_CTRL, 
							WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER
							) );
	m_grid.Init();
	

	CRect rcDesktop;
	CWnd* pDesktop = GetDesktopWindow();
	pDesktop->GetWindowRect( &rcDesktop );

	int nX, nY, nWidth, nHeight;

	nX = GetValueInt( GetAppUnknown(), "\\DBStructDlg", "Dlg placement x", 
		rcDesktop.Width() / 4 );
	nY = GetValueInt( GetAppUnknown(), "\\DBStructDlg", "Dlg placement y", 
		rcDesktop.Height() / 4 );

	nWidth = GetValueInt( GetAppUnknown(), "\\DBStructDlg", "Dlg placement width", 
		rcDesktop.Width() / 2 );

	nHeight = GetValueInt( GetAppUnknown(), "\\DBStructDlg", "Dlg placement height", 
		rcDesktop.Height() / 2 );

	m_bShowButtons = ( 1L == GetValueInt( GetAppUnknown(), "\\DBStructDlg", "Show buttons", 0L ) );
	
	if( !m_bShowButtons )
	{
		if( GetDlgItem( IDC_STATIC_TABLE ) )
			GetDlgItem( IDC_STATIC_TABLE )->ShowWindow( SW_HIDE );

		if( GetDlgItem( IDC_STATIC_NAME ) )
			GetDlgItem( IDC_STATIC_NAME )->ShowWindow( SW_HIDE );		

		if( GetDlgItem( IDC_CURENT_TABLE ) )
			GetDlgItem( IDC_CURENT_TABLE )->ShowWindow( SW_HIDE );

		if( GetDlgItem( IDC_ADD_TABLE ) )
			GetDlgItem( IDC_ADD_TABLE )->ShowWindow( SW_HIDE );

		if( GetDlgItem( IDC_CHOICE_TABLE ) )
			GetDlgItem( IDC_CHOICE_TABLE )->ShowWindow( SW_HIDE );
		
		if( GetDlgItem( IDC_DELETE_TABLE ) )
			GetDlgItem( IDC_DELETE_TABLE )->ShowWindow( SW_HIDE );

		if( GetDlgItem( IDC_FIELDS ) )
			GetDlgItem( IDC_FIELDS )->ShowWindow( SW_HIDE );

		if( GetDlgItem( IDC_STATIC_FIELDS_LINE ) )
			GetDlgItem( IDC_STATIC_FIELDS_LINE )->ShowWindow( SW_HIDE );
	
		if( GetDlgItem( IDC_DELETE_ROW ) )
			GetDlgItem( IDC_DELETE_ROW )->ShowWindow( SW_HIDE );
	}

	MoveWindow( nX, nY, nWidth, nHeight, FALSE );

	OnResizeDlg();

	if( m_ptrDB == 0 )
		return FALSE;

	//Wanna edit struct
	if( m_strTable.IsEmpty() )
	{
		int nCount = 0;
		m_ptrDB->GetTableCount( &nCount );

		if( nCount > 0 )
		{
			BSTR bstr = 0;
			if( S_OK != m_ptrDB->GetTableName( 0, &bstr ) )
				return false;

			CString strTableName = bstr;

			if( bstr )
				::SysFreeString( bstr );

			if( !strTableName.IsEmpty() )
			{				
				SetMode( emdEditTable, strTableName );
			}
		}
	}
	else
		SetMode( emdAddTable, m_strTable );



	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
bool CDBStructDlg::SetMode( EditModeDlg editMode, CString strTable )
{
	if( m_ptrDB == 0 )
		return false;

	strTable.MakeLower();

	m_strTable = strTable;	
	UpdateData( FALSE );	

	m_grid.Clear();

	m_editMode = editMode;

	if( editMode == emdAddTable )
	{
		m_grid.CreateKeyRow( m_strTable );
	}
	else
	if( editMode == emdEditTable )
	{		
		if( ReadTableInfo( false ) )
			m_grid.Build( &m_pti->m_arrFieldInfo );
	}
		

	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
bool CDBStructDlg::TryToSave( )
{
	if( m_ptrDB == 0 )
		return true;

	if( !m_grid.m_bWasModified )
		return true;


	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int nRes = AfxMessageBox(IDS_PROMPT_SAVE_QUERY, MB_YESNOCANCEL );
	if( nRes == IDYES )
	{	
		if( m_editMode == emdAddTable )
		{
			bool bProcessOK = false;

			CString strError;
			if( !m_grid.VerifyStruct( true, strError ) )
			{
				AfxMessageBox( strError, MB_ICONSTOP );
				return false;
			}

			int nSizeNew = 0;
			_CFieldInfo* parfi = NULL;
			m_grid.GetTableStruct( &parfi, &nSizeNew );

			
			if( nSizeNew > 0 )
			{

				//if( VerifyTableStruct( m_strTable, parfi, nSizeNew ) )
				{
					if( S_OK == m_ptrDB->AddTable( _bstr_t(m_strTable) ) )
					{
						for( int i=0;i<nSizeNew;i++ )
						{
							m_ptrDB->AddField( 
								_bstr_t(m_strTable), 
								_bstr_t(parfi[i].m_strFieldName), 
								(short)parfi[i].m_FieldType,
								_bstr_t(parfi[i].m_strPrimaryTable), 
								_bstr_t(parfi[i].m_strPrimaryFieldName)
											);

							m_ptrDB->SetFieldInfo(
								_bstr_t(m_strTable), 
								_bstr_t(parfi[i].m_strFieldName), 
								_bstr_t(parfi[i].m_strUserName),
								parfi[i].m_bRequiredValue, 
								parfi[i].m_bDefaultValue,
								_bstr_t(parfi[i].m_strDefaultValue), 
								_bstr_t(parfi[i].m_strVTObjectType)
												);
						}

						
					}
				}				
				
				delete[] parfi;
				parfi = NULL;

				bProcessOK = true;
			}

			return bProcessOK;
			
		}
		else
		if( m_editMode == emdEditTable )
		{

			bool bProcessOK = false;

			CString strError;
			if( !m_grid.VerifyStruct( false, strError ) )
			{
				AfxMessageBox( strError, MB_ICONSTOP );
				return false;
			}


			_CFieldInfo* pArrFieldInfoNew = NULL;
			int nSizeNew = 0;

			CArray<_CFieldInfo*,_CFieldInfo*> arAddFields;
			CArray<_CFieldInfo*,_CFieldInfo*> arDeleteFields;


			m_grid.GetTableStruct( &pArrFieldInfoNew, &nSizeNew );
			
			if( pArrFieldInfoNew && m_pti )
			{
				//determine field to delete
				for( int i=0;i<m_pti->m_arrFieldInfo.GetSize();i++ )
				{					

					CString strFieldOld = m_pti->m_arrFieldInfo[i]->m_strFieldName;
					strFieldOld.MakeLower();

					bool bFindField = false;
					for( int j=0;j<nSizeNew;j++ )
					{
						CString strNewField = pArrFieldInfoNew[j].m_strFieldName;
						strNewField.MakeLower();
						if( strNewField == strFieldOld )
						{
							bFindField = true;
							break;
						}
					}

					if( !bFindField )
					{						
						_CFieldInfo* pi = new _CFieldInfo;
						pi->CopyFrom( m_pti->m_arrFieldInfo[i] );
						arDeleteFields.Add( pi );
					}
				}

				//determine field to add
				for( i=0;i<nSizeNew;i++ )
				{					
					CString strNewField = pArrFieldInfoNew[i].m_strFieldName;					
					strNewField.MakeLower();					

					bool bFindField = false;
					for( int j=0;j<m_pti->m_arrFieldInfo.GetSize();j++ )
					{
						CString strFieldOld = m_pti->m_arrFieldInfo[j]->m_strFieldName;						
						strFieldOld.MakeLower();
						
						if( strNewField == strFieldOld )
						{
							bFindField = true;
							break;
						}
					}

					if( !bFindField )
					{	
						//[AY]
						/*m_ptrDB->AddField( 
											_bstr_t(m_strTable), 
											_bstr_t(pArrFieldInfoNew[i].m_strFieldName), 											
											(short)pArrFieldInfoNew[i].m_FieldType,
											_bstr_t(pArrFieldInfoNew[i].m_strPrimaryTable), 
											_bstr_t(pArrFieldInfoNew[i].m_strPrimaryFieldName)
										 );*/

						_CFieldInfo* pi = new _CFieldInfo;
						pi->CopyFrom( &(pArrFieldInfoNew[i]) );
						arAddFields.Add( pi );

											
					}
				}

				bProcessOK = true;

				if( arAddFields.GetSize() > 0 || arDeleteFields.GetSize() > 0 )
				{
					CStructEditConfirmationDlg dlg;	

					for( int i=0;i<arAddFields.GetSize();i++ )
						dlg.AddField2Add( arAddFields[i]->m_strFieldName );

					for( i=0;i<arDeleteFields.GetSize();i++ )
						dlg.AddField2Remove( arDeleteFields[i]->m_strFieldName );

					if( dlg.DoModal() != IDOK )
						bProcessOK = false;
				}

				if( bProcessOK )
				{

					//delete field
					for( i=0;i<arDeleteFields.GetSize();i++ )
					{
						_CFieldInfo* pi = arDeleteFields[i];
						m_ptrDB->DeleteField( _bstr_t(m_strTable), 
							_bstr_t( (LPCSTR)pi->m_strFieldName ) );
					}

					//add field
					for( i=0;i<arAddFields.GetSize();i++ )
					{
						_CFieldInfo* pi = arAddFields[i];

						m_ptrDB->AddField( 
											_bstr_t( (LPCSTR)m_strTable), 
											_bstr_t( (LPCSTR)pi->m_strFieldName), 											
											(short)pi->m_FieldType,
											_bstr_t( (LPCSTR)pi->m_strPrimaryTable), 
											_bstr_t((LPCSTR)pi->m_strPrimaryFieldName)
										 );
					}

					//set field info
					for( i=0;i<nSizeNew;i++ )
					{					
						m_ptrDB->SetFieldInfo( 
											_bstr_t(m_strTable), 
											_bstr_t(pArrFieldInfoNew[i].m_strFieldName), 
											_bstr_t(pArrFieldInfoNew[i].m_strUserName), 											
											pArrFieldInfoNew[i].m_bRequiredValue, 
											pArrFieldInfoNew[i].m_bDefaultValue,
											_bstr_t(pArrFieldInfoNew[i].m_strDefaultValue), 
											_bstr_t(pArrFieldInfoNew[i].m_strVTObjectType) 
											);
					}
				}
				
			}

			if( pArrFieldInfoNew )
			{
				delete[] pArrFieldInfoNew;
				pArrFieldInfoNew = 0;
			}

			for( int i=0;i<arAddFields.GetSize();i++ )
				delete arAddFields[i];

			arAddFields.RemoveAll();

			for( i=0;i<arDeleteFields.GetSize();i++ )
				delete arDeleteFields[i];

			arDeleteFields.RemoveAll();

			return bProcessOK;
		}
		
	}

	else
	if( nRes == IDCANCEL )
	{
		return false;
	}	

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDBStructDlg::ReadTableInfo( bool bForceReread )
{
	if( m_ptrDB == 0 )
		return false;

	if( m_pti )
		delete m_pti;	m_pti = 0;

	m_pti = new _CTableInfo( m_strTable );

	if( bForceReread )
	{
		m_ptrDB->RefreshTableInfo( TRUE );
	}



	_bstr_t bstrTable( m_strTable ); 
	int nCount = 0;
	if( S_OK != m_ptrDB->GetFieldCount( bstrTable, &nCount ) )
		return false;	
	

	for( int i=0;i<nCount;i++ )
	{
		BSTR _bstrFieldName = 0;
		if( S_OK != m_ptrDB->GetFieldName( bstrTable, i, &_bstrFieldName ) )
			continue;

		_bstr_t bstrFieldName = _bstrFieldName;

		if( _bstrFieldName )
			::SysFreeString( _bstrFieldName );	_bstrFieldName = 0;
		
		
		BSTR bstrUserName			= 0; 
		short nFieldType			= 0; 
		BSTR bstrPrimaryTable		= 0; 
		BSTR bstrPrimaryFieldName	= 0; 
		BOOL bRequiredValue			= 0; 
		BOOL bDefaultValue			= 0; 
		BSTR bstrDefaultValue		= 0; 
		BSTR bstrVTObjectType		= 0; 

		if( S_OK != m_ptrDB->GetFieldInfo( bstrTable, bstrFieldName, 
							&bstrUserName, &nFieldType,
							&bstrPrimaryTable, &bstrPrimaryFieldName,
							&bRequiredValue, &bDefaultValue,
							&bstrDefaultValue, &bstrVTObjectType
			) )
			continue;


		_CFieldInfo* pfi = new _CFieldInfo;
		m_pti->AddFieldInfo( pfi );

		pfi->m_strFieldName			= (LPSTR)bstrFieldName;
		pfi->m_strUserName			= bstrUserName;
		pfi->m_FieldType			= (FieldType)nFieldType;
		
		pfi->m_strPrimaryTable		= bstrPrimaryTable;
		pfi->m_strPrimaryFieldName	= bstrPrimaryFieldName;

		pfi->m_bRequiredValue		= bRequiredValue;
		pfi->m_bDefaultValue		= bDefaultValue;
		pfi->m_strDefaultValue		= bstrDefaultValue;
		pfi->m_strVTObjectType		= bstrVTObjectType;

		pfi->m_bCanEditStructure	= FALSE;
		

		if( bstrUserName )
			::SysFreeString( bstrUserName );

		if( bstrPrimaryTable )
			::SysFreeString( bstrPrimaryTable );

		if( bstrPrimaryFieldName )
			::SysFreeString( bstrPrimaryFieldName );

		if( bstrDefaultValue )
			::SysFreeString( bstrDefaultValue );

		if( bstrVTObjectType )
			::SysFreeString( bstrVTObjectType );

	}


	return true;
}


/////////////////////////////////////////////////////////////////////////////
void CDBStructDlg::OnAddTable() 
{
	if( !TryToSave( ) )
		return;		

	m_grid.m_bWasModified = false;

	CAddTableDlg dlg;
	if( dlg.DoModal() == IDOK )
		SetMode( emdAddTable, dlg.m_strTableName );


}

/////////////////////////////////////////////////////////////////////////////
void CDBStructDlg::OnChoiceTable() 
{
	if( m_ptrDB == 0 )
		return;

	if( !TryToSave( ) )
		return;

	m_grid.m_bWasModified = false;

	CChoiceTable dlg;
	CString strTitle;
	strTitle.LoadString( IDS_TITLE_CHOICE_TABLE );
	dlg.SetTitle( strTitle );	

	int nCount = 0;
	m_ptrDB->GetTableCount( &nCount );

	for( int i=0;i<nCount;i++ )
	{
		BSTR bstr = 0;
		if( S_OK == m_ptrDB->GetTableName( i, &bstr ) )
		{			
			dlg.AddTable( bstr );
			if( bstr )
				::SysFreeString( bstr );
		}
	}	

	dlg.SetCurTable( m_strTable );


	if( dlg.DoModal() == IDOK )
	{
		if( !dlg.m_strChoiceTable.IsEmpty() )
			SetMode( emdEditTable, dlg.m_strChoiceTable );
	}

}

/////////////////////////////////////////////////////////////////////////////
void CDBStructDlg::OnDeleteTable() 
{
	if( m_ptrDB == 0 )
		return;

	if( !TryToSave( ) )
		return;

	m_grid.m_bWasModified	= false;


	CChoiceTable dlg;
	CString strTitle;
	strTitle.LoadString( IDS_TITLE_DROP_TABLE );
	dlg.SetTitle( strTitle );	

	int nCount = 0;
	m_ptrDB->GetTableCount( &nCount );

	for( int i=0;i<nCount;i++ )
	{
		BSTR bstr = 0;
		if( S_OK == m_ptrDB->GetTableName( 0, &bstr ) )
		{			
			dlg.AddTable( bstr );
			
			if( bstr )
				::SysFreeString( bstr );
		}
	}	

	dlg.SetCurTable( m_strTable );


	if( dlg.DoModal() == IDOK )
	{
		if( !dlg.m_strChoiceTable.IsEmpty() ) 
			m_ptrDB->DeleteTable( _bstr_t(dlg.m_strChoiceTable) );

		SetMode( emdNotDefine, "" );
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDBStructDlg::OnOK() 
{
	if( !TryToSave() )
		return;

	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
void CDBStructDlg::OnCancel() 
{
	CDialog::OnCancel();
}
/////////////////////////////////////////////////////////////////////////////
void CDBStructDlg::OnDeleteRow() 
{
	CCellRange rng = m_grid.GetSelectedCellRange();
	int nID = rng.GetMinRow();

	if( m_grid.IsValidRow( nID ) )
	{
		m_grid.m_bWasModified	= false;
		m_grid.DeleteRow( nID );
		m_grid.Invalidate();

		if( m_grid.IsValidRow( nID ) )
			m_grid.SetSelectedRange( nID, 1, nID, 1 );
		else if( m_grid.IsValidRow( nID - 1 ) )
			m_grid.SetSelectedRange( nID - 1, 1, nID - 1, 1 );
	}
}



/////////////////////////////////////////////////////////////////////////////
void CDBStructDlg::OnResizeDlg()
{
	CWnd* pWndFields	= GetDlgItem( IDC_FIELDS );
	CWnd* pWndOK		= GetDlgItem( IDOK );
	CWnd* pWndCancel	= GetDlgItem( IDCANCEL );
	CWnd* pWndHelp		= GetDlgItem( IDHELP );
	
	CWnd* pWndDelete	= GetDlgItem( IDC_DELETE_ROW );


	if( !pWndFields || !pWndOK || !pWndCancel || !pWndHelp || !pWndDelete ||
		!pWndFields->GetSafeHwnd() || !pWndOK->GetSafeHwnd() || !pWndCancel->GetSafeHwnd() || 
		!m_grid.GetSafeHwnd() || !pWndDelete->GetSafeHwnd() || !pWndHelp->GetSafeHwnd()
		)
		return;

	CRect rcFields, rcOK, rcCancel, rcHelp, rcDelete;
	
	pWndFields->GetWindowRect( &rcFields );
	pWndOK->GetWindowRect( &rcOK );
	pWndCancel->GetWindowRect( &rcCancel );
	pWndHelp->GetWindowRect( &rcHelp );
	pWndDelete->GetWindowRect( &rcDelete );

	ScreenToClient( &rcFields );
	ScreenToClient( &rcOK );
	ScreenToClient( &rcCancel );
	ScreenToClient( &rcHelp );
	ScreenToClient( &rcDelete );

	CRect rcClient;
	GetClientRect( &rcClient );	

	CPoint ptOffsetOK( rcOK.Width() + rcCancel.Width() + rcHelp.Width() + 15, rcOK.Height() + 5 );
	CPoint ptOffsetCancel( rcCancel.Width() + rcHelp.Width() + 10, rcCancel.Height() + 5 );
	CPoint ptOffsetHelp( rcHelp.Width() + 5, rcHelp.Height() + 5 );


	pWndCancel->MoveWindow( 
							rcClient.Width() - ptOffsetCancel.x, 
							rcClient.Height() - ptOffsetCancel.y, 
							rcCancel.Width(), rcCancel.Height()
							);

	pWndOK->MoveWindow(
							rcClient.Width() - ptOffsetOK.x, 
							rcClient.Height() - ptOffsetOK.y, 
							rcOK.Width(), rcOK.Height() 
							);

	pWndHelp->MoveWindow(
							rcClient.Width() - ptOffsetHelp.x, 
							rcClient.Height() - ptOffsetHelp.y, 
							rcHelp.Width(), rcHelp.Height() 
							);

	pWndCancel->Invalidate();
	pWndOK->Invalidate();
	pWndHelp->Invalidate();


	CRect _rcClient;
	CRect _rcGrid;

	GetClientRect( &_rcClient );
	m_grid.GetWindowRect( &_rcGrid );
	ScreenToClient( &_rcGrid );

	int noffset = 10;	

	CRect rcGrid;

	rcGrid.left		= noffset;
	rcGrid.top		= ( m_bShowButtons ? rcFields.bottom + 5 : noffset );	

	rcGrid.right	= rcClient.Width() - 10;
	rcGrid.bottom	= rcClient.Height() - ptOffsetOK.y - 10 - ( m_bShowButtons ? rcDelete.Height() : 0 );

	m_grid.MoveWindow( &rcGrid );

	pWndDelete->MoveWindow( rcClient.Width() / 2 - rcOK.Width() / 2, 
							rcGrid.bottom  + 5,
							rcOK.Width(),
							rcDelete.Height()
 							);

	pWndDelete->Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
void CDBStructDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize( nType, cx, cy );
	OnResizeDlg();
}

/////////////////////////////////////////////////////////////////////////////
bool CDBStructDlg::VerifyTableStruct( CString strTableName, _CFieldInfo* pArrFieldInfo, int nSize )
{
	if( nSize < 1 )
		return false;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( !IsPossibleName( strTableName ) )
	{		
		AfxMessageBox( IDS_WARNING_BAD_TABLE_NAME, MB_ICONSTOP );
		return false;
	}

	bool bFoundPrimaryKey = false;

	//Verify for bad value & primary key exist
	for( int i=0;i<nSize;i++ )
	{
		_CFieldInfo* pField = &pArrFieldInfo[i];
		if( pField->m_FieldType == ftPrimaryKey )
		{
			bFoundPrimaryKey = true;
		}
		
		if( !IsPossibleName( pField->m_strFieldName ) )
		{
			CString strError;
			CString strMessage;
			strMessage.LoadString( IDS_WARNING_BAD_TABLE_NAME );
			strError.Format( (LPCTSTR)strMessage, (LPCTSTR)pField->m_strFieldName, i + 1 );
			AfxMessageBox( strError, MB_ICONSTOP );
			return false;
		}

		if( pField->m_FieldType == ftNotSupported )
		{
			CString strError;
			CString strMessage;
			strMessage.LoadString( IDS_WARNING_BAD_FIELD_TYPE );
			strError.Format( (LPCTSTR)strMessage, (LPCTSTR)pField->m_strFieldName, i + 1 );
			AfxMessageBox( strError, MB_ICONSTOP );
			return false;
		}
	}

	if( !bFoundPrimaryKey )
	{				
		AfxMessageBox( IDS_WARNING_TABLE_NEED_PRIMARY_KEY, MB_ICONSTOP );
		return false;
	}

	//Verify for uniq field names
	for( i=0;i<nSize-1;i++ )
	{
		_CFieldInfo* pField1 = &pArrFieldInfo[i];
		for( int j=i+1;j<nSize;j++ )
		{
			_CFieldInfo* pField2 = &pArrFieldInfo[j];
			if( pField2->m_strFieldName == pField1->m_strFieldName ) 
			{
				CString strError;
				CString strMessage;
				strMessage.LoadString( IDS_WARNING_NOT_UNIQUE_VALUE );
				strError.Format( (LPCTSTR)strMessage, (LPCTSTR)pField2->m_strFieldName, i + 1, j + 1 );
				AfxMessageBox( strError, MB_ICONSTOP );
				return false;
			}
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDBStructDlg::IsPossibleName( CString strName )
{
	if( strName.IsEmpty() )
		return false;

	return true;
}



/////////////////////////////////////////////////////////////////////////////
// CDBStructGrid


BEGIN_MESSAGE_MAP(CDBStructGrid, CGridCtrl)
	//{{AFX_MSG_MAP(CDBStructGrid)		
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CDBStructGrid::CDBStructGrid()
{
	m_bWasModified = false;
	EnableSelection( FALSE );
	SetHandleTabKey( FALSE );
}
/////////////////////////////////////////////////////////////////////////////
void CDBStructGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	CGridCtrl::OnLButtonDown( nFlags, point );
	
	{
		CCellID cell = GetCellFromPt(point);
		if( !IsValid( cell) )
		{
			//ASSERT(FALSE);
			return;
		}
		CGridCellBase *pCell = GetCell( cell.row, cell.col );
		if( pCell->IsKindOf( RUNTIME_CLASS(CGridCellCombo) ) )
		{
			CGridCellCombo* pCombo = (CGridCellCombo*)pCell;
			CPoint pointClickedRel;
			pointClickedRel = GetPointClicked( cell.row, cell.col, point);
			CRect rcCell = NORECT;
			GetCellRect( cell.row, cell.col, &rcCell );			

			if (IsCellEditable(cell) && pointClickedRel.x > rcCell.Width() - GetSystemMetrics(SM_CXHSCROLL) )
			{
				OnEditCell(cell.row, cell.col, pointClickedRel, VK_LBUTTON);
			}			
		}
	}
	
}


/////////////////////////////////////////////////////////////////////////////
void CDBStructGrid::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( nChar == VK_TAB )
		return;

	CGridCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

	if( nChar == VK_DELETE )
	{
		CCellRange cr = GetSelectedCellRange();
		if( !cr.IsValid() )
			return;

		int nStartRow	= cr.Top();
		int nEndRow		= cr.Bottom();

		if( nStartRow == GetAdditionRow() )
			return;

		if( nEndRow >= GetAdditionRow() )
			nEndRow = GetAdditionRow() - 1;

		for( int i=nStartRow/*nEndRow*/;i>=nStartRow;i-- )
		{
			DeleteRow( i );
			int nIndex = i - 1;
			if( nIndex >= 0 && nIndex < m_arFieldInfo.GetSize() )
			{
				delete m_arFieldInfo[ nIndex ];
				m_arFieldInfo.RemoveAt( nIndex );

			}
			else
			{
				ASSERT( false );
			}

			m_bWasModified = true;
		}

		Invalidate();
		UpdateWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDBStructGrid::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	
	if( nChar == VK_TAB )
		return;

	CGridCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}


/////////////////////////////////////////////////////////////////////////////
CDBStructGrid::~CDBStructGrid()
{
	Clear();
}

/////////////////////////////////////////////////////////////////////////////
void CDBStructGrid::Clear()
{
	for( int i=0;i<m_arFieldInfo.GetSize();i++ )
		delete m_arFieldInfo[i];

	m_arFieldInfo.RemoveAll();	

	if( GetSafeHwnd() )
	{
		if( GetSafeHwnd() )
			SetRowCount( 1 );

		m_bWasModified = false;

		InitComboRow( GetAdditionRow(), true );
	}
}

/////////////////////////////////////////////////////////////////////////////
COLORREF CDBStructGrid::GetReadOnlyColor()
{
	COLORREF clrBk = ::GetSysColor(COLOR_3DFACE);
	COLORREF clrHighlight = ::GetSysColor(COLOR_3DHILIGHT);
	COLORREF clrDither = RGB(GetRValue(clrBk) + ((GetRValue(clrHighlight) - GetRValue(clrBk)) / 2),
		   GetGValue(clrBk) + ((GetGValue(clrHighlight) - GetGValue(clrBk)) / 2),
		   GetBValue(clrBk) + ((GetBValue(clrHighlight) - GetBValue(clrBk)) / 2));

	return clrDither;	
}

/////////////////////////////////////////////////////////////////////////////
bool CDBStructGrid::Build( CArray<_CFieldInfo*, _CFieldInfo*>* parFieldInfo )
{
	Clear();

	if( parFieldInfo == NULL )
		return false;


	for( int i=0;i<parFieldInfo->GetSize();i++ )
	{
		_CFieldInfo* pfi = new _CFieldInfo;
		pfi->CopyFrom( (*parFieldInfo)[i] );
		pfi->m_bCanEditStructure = false;

		m_arFieldInfo.Add( pfi );
	}

	SetRowCount( m_arFieldInfo.GetSize() + 1 );
	
	CString strYes, strNo;
	strYes.LoadString( IDS_YES );
	strNo.LoadString( IDS_NO );

	for( i=0;i<m_arFieldInfo.GetSize();i++ )
	{
		_CFieldInfo* pfi = m_arFieldInfo[i];
		FillRow( pfi, i + 1 );

	}

	int nRow = GetAdditionRow();
	if( nRow )
	{		
		InitComboRow( nRow, true );		
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDBStructGrid::FillRow(_CFieldInfo* pfi, int nRow)
{
	int nRowCount = GetRowCount();
	if( !(nRow > 0 && nRow < nRowCount ) )
		return false;

	CString strYes, strNo;
	strYes.LoadString( IDS_YES );
	strNo.LoadString( IDS_NO );

	SetItemText( nRow, COL_FIELD,			pfi->m_strFieldName );
	SetItemText( nRow, COL_USER,			pfi->m_strUserName );
	SetItemText( nRow, COL_FIELD_TYPE,		GetFieldTypeAsString(pfi) );
	SetItemText( nRow, COL_IS_REQ,			pfi->m_bRequiredValue ? strYes : strNo );
	SetItemText( nRow, COL_IS_DEF,			pfi->m_bDefaultValue  ? strYes : strNo );
	SetItemText( nRow, COL_DEF_VALUE,		pfi->m_strDefaultValue );
	SetItemText( nRow, COL_FIELD_FORMAT,	pfi->m_strFormat );
	
	SetItemText( nRow, COL_REF_TABLE,		pfi->m_strPrimaryTable );
	SetItemText( nRow, COL_REF_TABLE_FIELD,pfi->m_strPrimaryFieldName );

	InitComboRow( nRow, pfi->m_bCanEditStructure == TRUE );


	for( int i=1;i<COL_REF_TABLE_FIELD+1;i++ )
	{
		SetItemBkColour( nRow, i, ::GetSysColor( COLOR_WINDOW ) );
		SetItemState( nRow, i, GetItemState( nRow, COL_FIELD ) & ~GVIS_READONLY );

		SetItemFormat( nRow, i, DT_LEFT );
	}


	if( !pfi->m_bCanEditStructure )
	{
		SetItemBkColour( nRow, COL_FIELD, GetReadOnlyColor() );
		SetItemState( nRow, COL_FIELD, GetItemState( nRow, COL_FIELD ) | GVIS_READONLY );

		SetItemBkColour( nRow, COL_REF_TABLE, GetReadOnlyColor() );
		SetItemState( nRow, COL_REF_TABLE, GetItemState( nRow, COL_REF_TABLE ) | GVIS_READONLY );

		SetItemBkColour( nRow, COL_REF_TABLE_FIELD, GetReadOnlyColor() );
		SetItemState( nRow, COL_REF_TABLE_FIELD, GetItemState( nRow, COL_REF_TABLE_FIELD ) | GVIS_READONLY );

		if( pfi->m_FieldType != ftVTObject )
		{
			SetItemBkColour( nRow, COL_FIELD_TYPE, GetReadOnlyColor() );
			SetItemState( nRow, COL_FIELD_TYPE, GetItemState( nRow, COL_FIELD_TYPE ) | GVIS_READONLY );
		}
		
	}


	if( pfi->m_FieldType == ftVTObject )
	{
		SetItemState( nRow, COL_IS_REQ, GetItemState( nRow, COL_IS_REQ ) | GVIS_READONLY );
		SetItemBkColour( nRow, COL_IS_REQ, GetReadOnlyColor() );

		SetItemState( nRow, COL_IS_DEF, GetItemState( nRow, COL_IS_DEF ) | GVIS_READONLY );
		SetItemBkColour( nRow, COL_IS_DEF, GetReadOnlyColor() );

		SetItemState( nRow, COL_DEF_VALUE, GetItemState( nRow, COL_DEF_VALUE ) | GVIS_READONLY );
		SetItemBkColour( nRow, COL_DEF_VALUE, GetReadOnlyColor() );

		SetItemState( nRow, COL_FIELD_FORMAT, GetItemState( nRow, COL_FIELD_FORMAT ) | GVIS_READONLY );
		SetItemBkColour( nRow, COL_FIELD_FORMAT, GetReadOnlyColor() );
		
	}

	if( pfi->m_FieldType != ftForeignKey || !pfi->m_bCanEditStructure )
	{
		SetItemState( nRow, COL_REF_TABLE, GetItemState( nRow, COL_REF_TABLE ) | GVIS_READONLY );
		SetItemBkColour( nRow, COL_REF_TABLE, GetReadOnlyColor() );

		SetItemState( nRow, COL_REF_TABLE_FIELD, GetItemState( nRow, COL_REF_TABLE_FIELD ) | GVIS_READONLY );
		SetItemBkColour( nRow, COL_REF_TABLE_FIELD, GetReadOnlyColor() );
	}

	return true;
	
}

/////////////////////////////////////////////////////////////////////////////
void CDBStructGrid::InitComboRow( int nRow, bool bIncludeDBaseType )
{

	CString strYes, strNo;
	strYes.LoadString( IDS_YES );
	strNo.LoadString( IDS_NO );


	SetCellType( nRow, COL_FIELD_TYPE, RUNTIME_CLASS(CGridCellCombo) );
	CGridCellCombo *pCell = (CGridCellCombo*) GetCell( nRow, COL_FIELD_TYPE );
	
	CStringArray arTypes, arUserNames;
	GetAvailableTypes( arTypes, arUserNames, ( bIncludeDBaseType ? FIELD_TYPE_DBASE : 0 ) | FIELD_TYPE_VT | FIELD_TYPE_FILTER_VT );
	pCell->SetOptions( arUserNames );
	pCell->SetStyle( CBS_DROPDOWNLIST );

	CStringArray arYesNo;
	arYesNo.Add( strYes );
	arYesNo.Add( strNo );

	SetCellType( nRow, COL_IS_REQ, RUNTIME_CLASS(CGridCellCombo) );
	pCell = (CGridCellCombo*) GetCell( nRow, COL_IS_REQ );
	pCell->SetOptions( arYesNo );
	pCell->SetStyle( CBS_DROPDOWNLIST );


	SetCellType( nRow, COL_IS_DEF, RUNTIME_CLASS(CGridCellCombo) );
	pCell = (CGridCellCombo*) GetCell( nRow, COL_IS_DEF );

	pCell->SetOptions( arYesNo );
	pCell->SetStyle( CBS_DROPDOWNLIST );

}

/////////////////////////////////////////////////////////////////////////////
void CDBStructGrid::OnSetFocusCell(int nRow, int nCol)
{
	CGridCtrl::OnSetFocusCell( nRow, nCol );
}

/////////////////////////////////////////////////////////////////////////////
void CDBStructGrid::OnEndEditCell(int nRow, int nCol, CString str)
{
	CGridCtrl::OnEndEditCell( nRow, nCol, str );

	m_bWasModified = true;

	if( nRow == GetAdditionRow() )
	{
		if( IsValidRow( nRow ) )
		{
			_CFieldInfo* pfi = new _CFieldInfo;
			pfi->m_bCanEditStructure = TRUE;

			VERIFY( FillFieldInfoFromRow( GetAdditionRow(), pfi ) );
			m_arFieldInfo.Add( pfi );
			
			int nNewRow = InsertRow( NULL, -1 );
			InitComboRow( nNewRow, true );
			FillRow( pfi, nNewRow );

			InitComboRow( GetAdditionRow(), true );

			Invalidate();
			UpdateWindow();			
		}
	}
	else
	{
		_CFieldInfo fi;
		FillFieldInfoFromRow( nRow, &fi );
		FillRow( &fi, nRow );
		Invalidate();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDBStructGrid::CreateKeyRow( CString strTableName )
{
	CString strKey;
	strKey.LoadString( IDS_TYPE_PRIMARY_KEY );

	int nAddRow = GetAdditionRow();

	CString strFieldName = CString( "id_" ) + strTableName;

	SetItemText( nAddRow, COL_FIELD, strFieldName );
	SetItemText( nAddRow, COL_USER, strFieldName );
	SetItemText( nAddRow, COL_FIELD_TYPE, strKey );

	_CFieldInfo* pfi = new _CFieldInfo;
	pfi->m_bCanEditStructure = TRUE;

	VERIFY( FillFieldInfoFromRow( nAddRow, pfi ) );
	m_arFieldInfo.Add( pfi );

	int nNewRow = InsertRow( NULL, -1 );
	InitComboRow( nNewRow, true );
	FillRow( pfi, nNewRow );

	InitComboRow( GetAdditionRow(), true );

	Invalidate();
	UpdateWindow();			



}

/////////////////////////////////////////////////////////////////////////////
void CDBStructGrid::GetTableStruct( _CFieldInfo** parfi, int* pnSize )
{
	int nSize = 0;
	*pnSize = 0;
	for( int nRow=0;nRow<GetRowCount();nRow++ )
	{
		if( IsValidRow( nRow ) )
			nSize++;
	}

	if( nSize <= 0 )
		return;

	*parfi = new _CFieldInfo[nSize];

	int nIndex = 0;
	for( nRow=0;nRow<GetRowCount();nRow++ )
	{
		if( IsValidRow( nRow ) )
		{
			FillFieldInfoFromRow( nRow, &((*parfi)[nIndex]) );
			nIndex++;
		}		
	}

	*pnSize = nSize;
}

/////////////////////////////////////////////////////////////////////////////
bool CDBStructGrid::FillFieldInfoFromRow( int nRow, _CFieldInfo* pfi )
{
	if( !pfi ) 
		return false;

	if( !(nRow > 0 && nRow < GetRowCount()+1 ) ) 
		return false;

	CString strYes;
	strYes.LoadString( IDS_YES );


	CString strVTObjectType;
	FieldType ft = ftNotSupported;

	ConvertFromStringToFieldType( GetItemText( nRow, COL_FIELD_TYPE), ft, strVTObjectType );

	pfi->m_strFieldName			= GetItemText( nRow, COL_FIELD );
	pfi->m_strFieldName.MakeLower();

	pfi->m_strUserName			= GetItemText( nRow, COL_USER );
	pfi->m_FieldType			= ft;
	pfi->m_strPrimaryTable		= GetItemText( nRow, COL_REF_TABLE );
	pfi->m_strPrimaryFieldName	= GetItemText( nRow, COL_REF_TABLE_FIELD );
	pfi->m_bRequiredValue		= ( GetItemText( nRow, COL_IS_REQ ) == strYes );
	pfi->m_bDefaultValue		= ( GetItemText( nRow, COL_IS_DEF ) == strYes );
	pfi->m_strDefaultValue		= GetItemText( nRow, COL_DEF_VALUE );
	pfi->m_strVTObjectType		= strVTObjectType;
	pfi->m_strFormat			= GetItemText( nRow, COL_FIELD_FORMAT );

	pfi->m_bCanEditStructure	= ( !( GetItemState( nRow, COL_FIELD ) & GVIS_READONLY) );

	if( pfi->m_strUserName.IsEmpty() )
		pfi->m_strUserName = pfi->m_strFieldName;

	return true;
}



/////////////////////////////////////////////////////////////////////////////
bool CDBStructGrid::Init( )
{

	SetDefCellHeight( ::GetSystemMetrics(SM_CYHSCROLL) + 2 * ::GetSystemMetrics(SM_CYBORDER) );

	SetRowCount( 1 );
	SetColumnCount( 1 );

	SetColumnWidth( 0, 20 );

	CString str;

	str.LoadString( IDS_CAPTION_COL_FIELD );
	InsertColumn( str );
	SetColumnWidth( COL_FIELD, ::GetValueInt( ::GetAppUnknown(), "\\DBStructDlg", "FieldWidth", 60 ) );
	
	str.LoadString( IDS_CAPTION_COL_USER );
	InsertColumn( str );
	SetColumnWidth( COL_USER, ::GetValueInt( ::GetAppUnknown(), "\\DBStructDlg", "UserWidth", 60 ) );

	str.LoadString( IDS_CAPTION_COL_FIELD_TYPE );
	InsertColumn( str );
	SetColumnWidth( COL_FIELD_TYPE, ::GetValueInt( ::GetAppUnknown(), "\\DBStructDlg", "FieldTypeWidth", 60 ) );

	str.LoadString( IDS_CAPTION_COL_IS_REQ );
	InsertColumn( str );
	SetColumnWidth( COL_IS_REQ, ::GetValueInt( ::GetAppUnknown(), "\\DBStructDlg", "IsReqWidth", 60 ) );

	str.LoadString( IDS_CAPTION_COL_IS_DEF );
	InsertColumn( str );
	SetColumnWidth( COL_IS_DEF, ::GetValueInt( ::GetAppUnknown(), "\\DBStructDlg", "IsDefWidth", 60 ) );

	str.LoadString( IDS_CAPTION_COL_DEF_VALUE );
	InsertColumn( str );
	SetColumnWidth( COL_DEF_VALUE, ::GetValueInt( ::GetAppUnknown(), "\\DBStructDlg", "DefValueWidth", 60 ) );

	str.LoadString( IDS_CAPTION_COL_FIELD_FORMAT );
	InsertColumn( str );
	SetColumnWidth( COL_FIELD_FORMAT, ::GetValueInt( ::GetAppUnknown(), "\\DBStructDlg", "FieldFormatWidth", 60 ) );

	str.LoadString( IDS_CAPTION_COL_REF_TABLE );
	InsertColumn( str );
	SetColumnWidth( COL_REF_TABLE, ::GetValueInt( ::GetAppUnknown(), "DBStructDlg", "RefTableWidth", 60 ) );

	str.LoadString( IDS_CAPTION_COL_REF_TABLE_FIELD );
	InsertColumn( str );
	SetColumnWidth( COL_REF_TABLE_FIELD, ::GetValueInt( ::GetAppUnknown(), "DBStructDlg", "RefTableFieldWidth", 60 ) );

	SetFixedColumnCount(1);
	SetFixedRowCount(1);	

	EnableSelection( TRUE );

	SetAdditionRow( );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBStructGrid::SetColumnWidth(int col, int width)
{
	int nnew_width = width;
	

	if( col == COL_FIELD_FORMAT )
	{
		if( 0L == ::GetValueInt( ::GetAppUnknown(), "\\DBStructDlg", "ShowFormatColumn", 0 ) )
			nnew_width = 0;

	}
	else if( col == COL_IS_REQ )
	{
		if( 0L == ::GetValueInt( ::GetAppUnknown(), "\\DBStructDlg", "ShowIsReq", 0 ) )
			nnew_width = 0;

	}	
	else if( col == COL_REF_TABLE || col == COL_REF_TABLE_FIELD )
	{
		if( 0L == ::GetValueInt( ::GetAppUnknown(), "\\DBStructDlg", "ShowReferenceTable", 0 ) )
			nnew_width = 0;
	}

	return CGridCtrl::SetColumnWidth( col, nnew_width );
}

/////////////////////////////////////////////////////////////////////////////
bool CDBStructGrid::IsValidRow( int nRow )
{
	int nRowCount = GetRowCount();
	if( !( nRow > 0 && nRow < nRowCount ) )
		return false;

	CString strField, strFieldType;

	strField	 = GetItemText( nRow, COL_FIELD );
	strFieldType = GetItemText( nRow, COL_FIELD_TYPE );

	if( strField.IsEmpty() )
		return false;

	if( strFieldType.IsEmpty() )
		return false;

	CStringArray arTypes, arUserNames;
	GetAvailableTypes( arTypes, arUserNames, FIELD_TYPE_DBASE | FIELD_TYPE_VT );

	bool bFoundType = false;
	for( int i=0;i<arUserNames.GetSize();i++ )
		if( arUserNames[i] == strFieldType )
			bFoundType = true;


	return bFoundType;
}


/////////////////////////////////////////////////////////////////////////////
CString CDBStructGrid::GetFieldTypeAsString( _CFieldInfo* pfi )
{
	if( !pfi )
		return "";

	CString strResult;

	if( pfi->m_FieldType == ftDigit )
	{
		strResult.LoadString( IDS_TYPE_DIGIT );
	}
	else
	if( pfi->m_FieldType == ftString )
	{
		strResult.LoadString( IDS_TYPE_STRING );
	}
	else
	if( pfi->m_FieldType == ftDateTime )
	{
		strResult.LoadString( IDS_TYPE_DATE_TIME );
	}
	else
	if( pfi->m_FieldType == ftVTObject )
	{
		CStringArray arTypes, arUserNames;
		GetAvailableTypes( arTypes, arUserNames, FIELD_TYPE_DBASE | FIELD_TYPE_VT );

		for( int i=0;i<arTypes.GetSize();i++ )
		{
			if( pfi->m_strVTObjectType == arTypes[i] )
			{
				strResult = arUserNames[i];
				break;
			}
		}
	}
	else
	if( pfi->m_FieldType == ftPrimaryKey )
	{
		strResult.LoadString( IDS_TYPE_PRIMARY_KEY );
	}
	else
	if( pfi->m_FieldType == ftForeignKey )
	{
		strResult.LoadString( IDS_TYPE_FOREIGN_KEY );
	}
	else
	if( pfi->m_FieldType == ftNotSupported )
	{
		strResult.LoadString( IDS_TYPE_NOT_SUPPORTED );
	}

	return strResult;
}

////////////////////////////////////////////////////////////////////////////////////////////
void CDBStructGrid::GetAvailableTypes( CStringArray& arrTypes, CStringArray& arrUserNames, DWORD dwFlags )
{
	if( dwFlags & FIELD_TYPE_DBASE )
	{

		CString str;
		str.LoadString( IDS_TYPE_DIGIT );			arrTypes.Add( str );	arrUserNames.Add( str );
		str.LoadString( IDS_TYPE_STRING );			arrTypes.Add( str );	arrUserNames.Add( str );
		str.LoadString( IDS_TYPE_DATE_TIME );		arrTypes.Add( str );	arrUserNames.Add( str );
		str.LoadString( IDS_TYPE_PRIMARY_KEY );		arrTypes.Add( str );	arrUserNames.Add( str );
		//str.LoadString( IDS_TYPE_FOREIGN_KEY );		arrTypes.Add( str );	arrUserNames.Add( str );
		//str.LoadString( IDS_TYPE_NOT_SUPPORTED );	arrTypes.Add( str );	arrUserNames.Add( str );
	}

	CString strDef = szTypeImage;
	strDef += ";";
	strDef += szTypeObjectList;
	strDef += ";";
	strDef += szTypeOleItem;

	CString strAvailableType = 
		::GetValueString( ::GetAppUnknown(), SHELL_DATA_DB_SECTION, SHELL_DATA_AVAILABLE_TYPE, strDef );
	

	if( dwFlags & FIELD_TYPE_VT ) 
	{
		sptrIApplication spApp( ::GetAppUnknown() );
		if( spApp )
		{
			sptrIDataTypeInfoManager	sptrT( spApp );
			if( sptrT )
			{
				long	nTypesCount = 0;
				sptrT->GetTypesCount( &nTypesCount );
				for( long nType = 0; nType < nTypesCount; nType++ )
				{				
					INamedDataInfo* pInfo;
					sptrT->GetTypeInfo( nType, &pInfo );
					if( !pInfo )
						continue;
					
					INamedObject2Ptr	ptrNamed( pInfo );

					BSTR	bstrC = 0;
					pInfo->GetContainerType( &bstrC );
					CString	strCType = bstrC;
					bool	bEmpty = strCType.GetLength()==0;

					if( bstrC )
						::SysFreeString( bstrC );

					pInfo->Release();	pInfo = 0;

					if( !bEmpty )continue;

					if( ptrNamed == 0 )
						continue;

					BSTR bstr = 0;
					ptrNamed->GetName( &bstr );

					CString	strType = bstr;

					if( bstr )
						::SysFreeString( bstr );	bstr = 0;


					CString strUserName;
					ptrNamed->GetUserName( &bstr );
					strUserName = bstr;

					if( bstr )
						::SysFreeString( bstr );	bstr = 0;


					if( dwFlags & FIELD_TYPE_FILTER_VT )
					{
						 if( strAvailableType.Find( strType ) == -1 )
							 continue;
					}

					arrTypes.Add( strType );				
					arrUserNames.Add( strUserName );
				}
			}
		}
	}		
}

/////////////////////////////////////////////////////////////////////////////
bool CDBStructGrid::ConvertFromStringToFieldType( CString strType, FieldType& ft, CString& strObjectType )
{

	ft = ftNotSupported;

	if( strType == LoadString( IDS_TYPE_DIGIT ) )
	{
		ft = ftDigit;
	}
	else
	if( strType == LoadString( IDS_TYPE_STRING ) )
	{
		ft = ftString;
	}
	else
	if( strType == LoadString( IDS_TYPE_DATE_TIME ) )
	{
		ft = ftDateTime;
	}
	else
	if( strType == LoadString( IDS_TYPE_PRIMARY_KEY ) )
	{
		ft = ftPrimaryKey;
	}
	else
	if( strType == LoadString( IDS_TYPE_FOREIGN_KEY ) )
	{
		ft = ftForeignKey;
	}
	else
	if( strType == LoadString( IDS_TYPE_NOT_SUPPORTED ) )
	{
		ft = ftNotSupported;
	}

	if( ft != ftNotSupported )
		return true;

	CStringArray arType, arUserNames;
	GetAvailableTypes( arType, arUserNames, FIELD_TYPE_VT );
	for( int i=0;i<arUserNames.GetSize();i++ )
	{
		if( arUserNames[i] == strType )
		{
			strObjectType = arType[i];
			ft = ftVTObject;
			return true;
			
		}
	}

	return false;
	
}

/////////////////////////////////////////////////////////////////////////////
CString CDBStructGrid::LoadString( UINT uiRes )
{
	CString str;
	str.LoadString( uiRes );

	return str;
}



/////////////////////////////////////////////////////////////////////////////
bool CDBStructGrid::VerifyStruct( bool bTestForPrimaryKey, CString& strError )
{
	_CFieldInfo* pfi = 0;
	int nSize = 0;
	GetTableStruct( &pfi, &nSize );
	if( pfi == 0 || nSize < 1 )
	{		
		strError.LoadString( IDS_WARNING_NO_FIELDS );
		return false;
	}

	//is there primary key?
	if( bTestForPrimaryKey )
	{
	
		bool bFoundPrimaryKey = false;
		for( int i=0;i<nSize;i++ )
		{
			if( bFoundPrimaryKey )
				continue;

			bFoundPrimaryKey = ( pfi[i].m_FieldType == ftPrimaryKey );		
		}

		if( !bFoundPrimaryKey )
		{
			strError.LoadString( IDS_WARNING_TABLE_NEED_PRIMARY_KEY );		
			delete[] pfi;
			return false;
		}
	}

	//wrong sintacsis
	for( int i=0;i<nSize;i++ )
	{
		CString s1 = pfi[i].m_strFieldName;

		if( !::IsValidWord( s1 ) )
		{
			strError.Format( IDS_WARNING_BAD_FIELD_NAME, s1, i+1 );				
			delete[] pfi;
			return false;
		}
	}

	//bad field type
	for( i=0;i<nSize;i++ )
	{
		if( !pfi[i].m_bCanEditStructure )//was enter befor
			continue;

		CString s1 = pfi[i].m_strFieldName;

		if( pfi[i].m_FieldType == ftNotSupported )
		{
			strError.Format( IDS_WARNING_BAD_FIELD_TYPE, s1, i+1 );				
			delete[] pfi;
			return false;
		}
	}	


	//test for equal fields
	for( i=0;i<nSize-1;i++ )
	{
		CString s1 = pfi[i].m_strFieldName;
		s1.MakeLower();
		for( int j=i+1;j<nSize;j++ )
		{
			CString s2 = pfi[j].m_strFieldName;
			s2.MakeLower();

			if( s1 == s2 )
			{
				strError.Format( IDS_WARNING_FIELD_EXIST, pfi[i].m_strFieldName, i+1, j+1 );				
				delete[] pfi;
				return false;
			}
		}
	}

	delete[] pfi;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CDBStructDlg::OnDestroy() 
{
	if( m_grid.GetSafeHwnd() )
	{
		int nColCount = m_grid.GetColumnCount();
		
		if( nColCount > COL_FIELD )
			::SetValue( ::GetAppUnknown(), "DBStructDlg", "FieldWidth", (long)m_grid.GetColumnWidth( COL_FIELD ) );
		
		if( nColCount > COL_USER )
			::SetValue( ::GetAppUnknown(), "DBStructDlg", "UserWidth", (long)m_grid.GetColumnWidth( COL_USER ) );

		if( nColCount > COL_FIELD_TYPE )
			::SetValue( ::GetAppUnknown(), "DBStructDlg", "FieldTypeWidth", (long)m_grid.GetColumnWidth( COL_FIELD_TYPE ) );

		if( nColCount > COL_IS_REQ )
			::SetValue( ::GetAppUnknown(), "DBStructDlg", "IsReqWidth", (long)m_grid.GetColumnWidth( COL_IS_REQ ) );

		if( nColCount > COL_IS_DEF )
			::SetValue( ::GetAppUnknown(), "DBStructDlg", "IsDefWidth", (long)m_grid.GetColumnWidth( COL_IS_DEF ) );

		if( nColCount > COL_DEF_VALUE )
			::SetValue( ::GetAppUnknown(), "DBStructDlg", "DefValueWidth", (long)m_grid.GetColumnWidth( COL_DEF_VALUE ) );

		if( nColCount > COL_FIELD_FORMAT )
			::SetValue( ::GetAppUnknown(), "DBStructDlg", "FieldFormatWidth", (long)m_grid.GetColumnWidth( COL_FIELD_FORMAT ) );

		if( nColCount > COL_REF_TABLE )
			::SetValue( ::GetAppUnknown(), "DBStructDlg", "RefTableWidth", (long)m_grid.GetColumnWidth( COL_REF_TABLE ) );

		if( nColCount > COL_REF_TABLE_FIELD )
			::SetValue( ::GetAppUnknown(), "DBStructDlg", "RefTableFieldWidth", (long)m_grid.GetColumnWidth( COL_REF_TABLE_FIELD ) );

	}
	CRect rcWnd;
	GetWindowRect( &rcWnd );
	

	SetValue( GetAppUnknown(), "DBStructDlg", "Dlg placement x", 
		(long)rcWnd.left );
	SetValue( GetAppUnknown(), "DBStructDlg", "Dlg placement y", 
		(long)rcWnd.top );

	SetValue( GetAppUnknown(), "DBStructDlg", "Dlg placement width", 
		(long)rcWnd.Width() );

	SetValue( GetAppUnknown(), "DBStructDlg", "Dlg placement height", 
		(long)rcWnd.Height() );

	CDialog::OnDestroy();	
	
}


/////////////////////////////////////////////////////////////////////////////
// CStructEditConfirmationDlg dialog


CStructEditConfirmationDlg::CStructEditConfirmationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStructEditConfirmationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStructEditConfirmationDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CStructEditConfirmationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStructEditConfirmationDlg)
	DDX_Control(pDX, IDC_REMOVE, m_ctrlRemove);
	DDX_Control(pDX, IDC_ADD, m_ctrlAdd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStructEditConfirmationDlg, CDialog)
	//{{AFX_MSG_MAP(CStructEditConfirmationDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStructEditConfirmationDlg message handlers

/////////////////////////////////////////////////////////////////////////////
void CStructEditConfirmationDlg::AddField2Add( CString str )
{
	m_arrAddFields.Add( str );
}

/////////////////////////////////////////////////////////////////////////////
void CStructEditConfirmationDlg::AddField2Remove( CString str )
{
	m_arrRemoveFields.Add( str );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CStructEditConfirmationDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	for( int i=0;i<m_arrAddFields.GetSize();i++ )	
	{
		m_ctrlAdd.AddString( m_arrAddFields[i] );	
	}

	for( i=0;i<m_arrRemoveFields.GetSize();i++ )	
	{
		m_ctrlRemove.AddString( m_arrRemoveFields[i] );	
	}		

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
	

