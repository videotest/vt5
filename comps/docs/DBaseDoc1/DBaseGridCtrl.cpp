// DBaseGrid.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "DBaseGridCtrl.h"
#include "constant.h"
#include "dbasegridview.h"
#include "querybase.h"

#include "MenuRegistrator.h"

#include "PopupList.h"



#define VERIFY_IS_WINDOW(pWnd, returnValue)		\
	if( pWnd == NULL )							\
		return returnValue;						\
												\
	if( !::IsWindow( pWnd->GetSafeHwnd() ) )	\
		return returnValue;						\

#define VERIFY_IS_WINDOW_NULL(pWnd)					\
	if( pWnd == NULL )							\
		return;									\
												\
	if( !::IsWindow( pWnd->GetSafeHwnd() ) )	\
		return;									\


/////////////////////////////////////////////////////////////////////////////
// CDBaseGridCtrl
CDBaseGridCtrl::CDBaseGridCtrl()
{
	m_bUseFilterRow	= FALSE;
	m_bUseSortRow	= FALSE;

	m_nSecondaryRowCount = 1;

	m_bCanProcessOnChangeRecordScroll = true;	

	m_nRecordDelete = -1;

	//Timer
	m_lCurrentTimerRecord = -1;
	m_nTimerID = -1;
	m_lCurrentTimerRecord = -1;
	m_bTimerActivate = false;	

	EnableTitleTips( FALSE );
	EnableDragAndDrop( FALSE );
	
	EnableSelection( TRUE );

}

/////////////////////////////////////////////////////////////////////////////
CDBaseGridCtrl::~CDBaseGridCtrl()
{
	DestroyColumnInfoArr();
}

void CDBaseGridCtrl::DestroyColumnInfoArr()
{
	for( int i=0;i<m_arColumnInfo.GetSize();i++ )
		delete m_arColumnInfo[i];

	m_arColumnInfo.RemoveAll();
}


BEGIN_MESSAGE_MAP(CDBaseGridCtrl, CGridCtrl)
	//{{AFX_MSG_MAP(CDBaseGridCtrl)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDS_MENU_ADD_FILTER, OnAddFilter)
	ON_COMMAND(IDS_MENU_EDIT_FILTER, OnEditFilter)
	ON_COMMAND(IDS_MENU_DELETE_FILTER, OnDeleteFilter)
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::OnSize(UINT nType, int cx, int cy) 
{	
	CGridCtrl::OnSize(nType, cx, cy);	
	AnalizeNewValueSet();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBaseGridCtrl::MoveColumn(int nCol, int nColAfter)
{	
	BOOL bResult = CGridCtrl::MoveColumn( nCol, nColAfter );
	if( bResult )
	{		
		CArray<CGridColumnInfo*, CGridColumnInfo*> arColumnInfo;
		for( int i=0;i<m_arColumnInfo.GetSize();i++ )
		{		
			CGridColumnInfo* pInfo = (CGridColumnInfo*)GetItemData( 0, i+1 );
			arColumnInfo.Add( pInfo );
		}

		m_arColumnInfo.RemoveAll();

		for( i=0;i<arColumnInfo.GetSize();i++ )
		{		
			CGridColumnInfo* pInfo = arColumnInfo[i];
			m_arColumnInfo.Add( pInfo );
		}

		arColumnInfo.RemoveAll();		

	}	


	SaveGridToQuery();

	SetModifiedFlagToDoc( GetDBaseDocument() );

	return bResult;
	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBaseGridCtrl::SetColumnWidth(int col, int width)
{
	BOOL bRes = CGridCtrl::SetColumnWidth(col, width);

	SaveGridToQuery();

	return bRes;
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::TestForGridColumns()
{
	return;

	/*
	sptrIQueryObject spActiveQuery = GetActiveQuery();
	if( spActiveQuery == NULL )
		return;	

	sptrIDBaseDocument spDBDoc = GetDBaseDocument();
	if( spDBDoc == NULL )
		return;

	int nFieldCount = 0;
	spActiveQuery->GetFieldsCount( &nFieldCount );
	for( int i=0;i<nFieldCount;i++)
	{
		BSTR bstrTable, bstrField;
		spActiveQuery->GetField( i, 0, &bstrTable, &bstrField );
		
		CString strTable = bstrTable;
		CString strField = bstrField;

		FieldType fieldType = ::GetFieldType( spDBDoc, strTable, strField );
		CString strCaption  = ::GetFieldCaption( spDBDoc, strTable, strField );

		if( fieldType == ftDigit ||
			fieldType == ftString ||
			fieldType == ftDateTime 
			)
		{
			//spActiveQuery->AddGridField( bstrTable, bstrField, bstrField, 50 );			
		}


		::SysFreeString( bstrTable );
		::SysFreeString( bstrField );
	}
	*/

}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::BuildAppearance( )
{
	DestroyColumnInfoArr();

	SetColumnCount( 0 );
	SetRowCount( 0 );

	VERIFY_IS_WINDOW_NULL(this);

	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
	{
		SetGridAsEmpty();
		return;
	}

	IQueryMiscPtr ptrQMisc( spQuery );
	if( ptrQMisc == 0 )
		return;

	m_bUseFilterRow = m_bUseSortRow = 0;
	/*
	spQuery->GetUseFilterRowInGrid( &m_bUseFilterRow );
	spQuery->GetUseSortRowInGrid( &m_bUseSortRow );
	*/

	m_nSecondaryRowCount = 1;

	TestForGridColumns();

	int nCount = 0;
	spQuery->GetFieldsCount( &nCount );


	InsertColumn( "" );
	SetFixedColumnCount(1);

	SetFixedRowCount(1);		

	sptrIDBaseDocument spDBDoc = GetDBaseDocument();


	CArray<CGridColumnInfo*, CGridColumnInfo*> arQuery;
	CArray<CGridField*, CGridField*> arGrid;

	int nFieldCount = -1;
	spQuery->GetFieldsCount( &nFieldCount );
	for( int i=0;i<nFieldCount;i++)
	{
		int nColumnWidth = 200;
		//spQuery->GetGridField( i, &bstrTable, &bstrField, &bstrColumnCaption, &nColumnWidth );


		BSTR bstrTable = 0;
		BSTR bstrField = 0;
		if( S_OK != spQuery->GetField( i, 0, &bstrTable, &bstrField ) )
			continue;
		
		CString strTable = bstrTable;
		CString strField = bstrField;		

		FieldType fieldType = ::GetFieldType( spDBDoc, strTable, strField );
		CString strCaption  = ::GetFieldCaption( spDBDoc, strTable, strField );

		if( bstrTable )
			::SysFreeString( bstrTable );	bstrTable = 0;

		if( bstrField )
			::SysFreeString( bstrField );	bstrField = 0;

		if( !( fieldType == ftDigit ||
			fieldType == ftString ||
			fieldType == ftDateTime )
			)
		{
			continue;
		}	
	
		CGridColumnInfo* pColumnInfo = new CGridColumnInfo;
		pColumnInfo->m_strTable			= strTable;
		pColumnInfo->m_strField			= strField;
		pColumnInfo->m_strColumnCaption	= strCaption;
		pColumnInfo->m_nColumnWidth			= nColumnWidth;

		arQuery.Add( pColumnInfo );

	}

	for( i=0;i<arQuery.GetSize();i++ )
	{
		int nWidth, nPos;
		if( ptrQMisc->GetGridFieldInfo( 
					_bstr_t( (LPCSTR)arQuery[i]->m_strTable ),
					_bstr_t( (LPCSTR)arQuery[i]->m_strField ),
					&nWidth, &nPos
			) != S_OK )
			continue;		

		CGridField* p = new CGridField;
		arGrid.Add( p );

		p->m_strTable	= arQuery[i]->m_strTable;
		p->m_strField	= arQuery[i]->m_strField;
		p->m_nPos		= nPos;
		p->m_nWidth		= nWidth;
	}


	//sort by pos
	for( i=0;i<arGrid.GetSize()-1;i++ )
	{
		CGridField* p1 = arGrid[i];
		for( int j=i+1;j<arGrid.GetSize();j++ )
		{
			CGridField* p2 = arGrid[j];
			if( p2->m_nPos < p1->m_nPos )
			{
				CGridField* pTemp = arGrid[i];				
				arGrid[i] = arGrid[j];
				arGrid[j] = pTemp;

				p1 = arGrid[i];
				p2 = arGrid[j];
			}
		}

	}


	//add fields from grid setting first
	for( i=0;i<arGrid.GetSize();i++ )
	{
		CGridField* p = arGrid[i];
		CString strCaption  = ::GetFieldCaption( spDBDoc, p->m_strTable, p->m_strField );

		CGridColumnInfo* pci = new CGridColumnInfo;
		m_arColumnInfo.Add( pci );

		pci->m_strTable		= p->m_strTable;
		pci->m_strField		= p->m_strField;
		pci->m_nColumnWidth = p->m_nWidth;
		pci->m_strColumnCaption = ::GetFieldCaption( spDBDoc, p->m_strTable, p->m_strField );
	}

	//not exist in query settings
	for( i=0;i<arQuery.GetSize();i++ )
	{
		CGridColumnInfo* pci = arQuery[i];
		bool bFound = false;
		for( int j=0;j<arGrid.GetSize();j++ )
		{
			CGridField* p = arGrid[j];
			if( p->m_strTable == pci->m_strTable && p->m_strField == pci->m_strField )
			{
				bFound = true;
				break;
			}
		}

		if( !bFound )
		{
			CGridColumnInfo* pciNew = new CGridColumnInfo;
			m_arColumnInfo.Add( pciNew );

			pciNew->m_strTable		= pci->m_strTable;
			pciNew->m_strField		= pci->m_strField;
			pciNew->m_nColumnWidth	= 100;
			pciNew->m_strColumnCaption = pci->m_strColumnCaption;
		}

	}

	for( i=0;i<arQuery.GetSize();i++ )
		delete arQuery[i];

	arQuery.RemoveAll();

	for( i=0;i<arGrid.GetSize();i++ )
		delete arGrid[i];

	arGrid.RemoveAll();

	SetColumnCount( m_arColumnInfo.GetSize() + 1 );

	for( i=0;i<m_arColumnInfo.GetSize();i++ )
	{			
		CGridColumnInfo* pColumnInfo = m_arColumnInfo[i];

		//int nColumn = InsertColumn( pColumnInfo->m_strColumnCaption, DT_LEFT|DT_VCENTER );		
		int nColumn = i + 1;

		SetItemText( 0, nColumn, pColumnInfo->m_strColumnCaption );

		VERIFY( SetItemData( 0, nColumn, (LPARAM)pColumnInfo ) );							
		VERIFY( SetItemState( 0, nColumn, GetItemState( 0, nColumn ) | GVIS_READONLY ) );		

		SetColumnWidth( nColumn, pColumnInfo->m_nColumnWidth );		
	}
	
	
	
	//Set color? row count
	FillGridFirstTime();

	/*
	FillFilterRow();	
	FillSortRow();
	*/

	//set data to cell
	AnalizeNewValueSet();


	ResetScrollBars();

	AfterNavigation();

	Invalidate();

}


/////////////////////////////////////////////////////////////////////////////
sptrISelectQuery CDBaseGridCtrl::GetActiveQuery()
{
	sptrIDBaseDocument spDoc = GetDBaseDocument();	
	if( spDoc == NULL )
		return NULL;

	IUnknown* pUnkQuery = NULL;
	spDoc->GetActiveQuery( &pUnkQuery );
	if( pUnkQuery == NULL )
		return NULL;

	sptrISelectQuery spSelectQuery( pUnkQuery );
	pUnkQuery->Release();

	return spSelectQuery;	
}

/////////////////////////////////////////////////////////////////////////////
sptrIDBaseDocument CDBaseGridCtrl::GetDBaseDocument()
{
	if( !GetSafeHwnd() )
		return NULL;

	CDBaseGridView* pGridView = (CDBaseGridView*)GetParent();
	if( pGridView == NULL )
		return NULL;

	IDocument* pDoc = pGridView->GetDocument();
	if( pDoc == NULL )
		return NULL;

	sptrIDBaseDocument spDoc( pDoc );
	pDoc->Release();

	return spDoc;	
}

static void DeleteCRLF(CString &s)
{
	LPTSTR lpstr = s.GetBuffer();
	int nLen = s.GetLength();
	for( int i = 0; i < nLen; i++)
	{
		if (lpstr[i] == _T('\n') || lpstr[i] == _T('\r'))
			lpstr[i] = _T(' ');
	}
	s.ReleaseBuffer();
};


/////////////////////////////////////////////////////////////////////////////
bool CDBaseGridCtrl::GetValueFromData( sptrINamedData spND, int nCol, CString& strValue )
{	
	ASSERT( spND != NULL );
	if( spND == NULL )
		return false;

	if( nCol < 0 || nCol >= m_arColumnInfo.GetSize() )
	{
		ASSERT( FALSE );
		return false;
	}

	CString strKey;	
	strKey.Format( "%s\\%s.%s",	(LPCTSTR)SECTION_DBASEFIELDS,
								(LPCTSTR)m_arColumnInfo[nCol]->m_strTable,
								(LPCTSTR)m_arColumnInfo[nCol]->m_strField
								);

	_variant_t var;	

	//spND->SetupSection( _bstr_t( SECTION_DBASEFIELDS ) );
	spND->GetValue( _bstr_t( (LPCTSTR)strKey ), &var );
	try{
		if( !(var.vt == VT_EMPTY || var.vt == VT_NULL) )
		{
			var.ChangeType( VT_BSTR );		
			strValue = var.bstrVal;
		}
		else
		{
			strValue = "";
		}
		
	}
	catch(...)
	{

	}
	
	DeleteCRLF(strValue); // A.M. fix. BT 4567
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::AfterInsertRecord()
{	
	sptrINamedData spND = GetDBaseDocument();
	if( spND == NULL )
		return;

	long nRecordCount = GetRecordsetRowCount();
	
	//workaround: для DESC сортировки запись вставляется в начало
	sptrISelectQuery spQuery = GetActiveQuery();
	if( spQuery )
	{
		long nCurRecord = 0;
		spQuery->GetCurrentRecord( &nCurRecord );
		if( nCurRecord != nRecordCount )
		{
			for( int iRow=1;iRow<=nRecordCount;iRow++ )
			{
				for( int iCol=1;iCol<GetColumnCount();iCol++ )
				{
					if( iCol < m_arColumnInfo.GetSize() + 1 )
						SetItemData( iRow, iCol, ID_CELL_WAS_NOT_READ );		
				}
			}				
		}
	}

	int nRowCount = nRecordCount + m_nSecondaryRowCount;
	SetRowCount( nRowCount );	
	for( int i=0;i<GetColumnCount();i++ )
	{
		SetItemData( GetRowCount() - 1 , i, ID_CELL_WAS_NOT_READ );
	}

	AnalizeNewValueSet();
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::AfterDeleteRecord( int nRecordNum )
{
	m_nRecordDelete = -1;
	int nRow = GetRowFormRecordNum( nRecordNum );
	if( nRow == -1 )
	{
		ASSERT(FALSE);
		return;
	}

	DeleteRow( nRow );

	Invalidate( );
	
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::AfterFieldChange( CString strTable, CString strField, _variant_t var )
{
	bool bFound = false;
	int nColumn = -1;
	for( int i=0;i<m_arColumnInfo.GetSize();i++ )
	{
		if( nColumn != -1 )
			continue;

		if( m_arColumnInfo[i]->m_strTable == strTable && 
			m_arColumnInfo[i]->m_strField == strField
			)
			nColumn = i;
	}

	if( nColumn == -1 )
		return;

	try{
		CString strValue;
		_variant_t _var = var;
		if( !(_var.vt == VT_NULL || _var.vt == VT_EMPTY) )
		{
			var.ChangeType( VT_BSTR );
			strValue = var.bstrVal;
		}		

		sptrISelectQuery spQuery = GetActiveQuery();
		if( spQuery == NULL )
			return;


		long nCurRecord;
		spQuery->GetCurrentRecord( &nCurRecord );
		int nRow = nCurRecord + m_nSecondaryRowCount - 1;		
		if( nRow < m_nSecondaryRowCount || nRow >= GetRowCount() )
			return;

		SetItemText( nRow, nColumn + 1, strValue );
		InvalidateCellRect( nRow, nColumn + 1 );
	}
	catch(...)
	{

	}
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::AfterNavigation()
{
	sptrISelectQuery spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return;	

	long nRecord = 0;		
	spQuery->GetCurrentRecord( &nRecord );
	
	SetCurrentRecord( nRecord );
	
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::AnalizeNewValueSet()
{
	sptrISelectQuery spSelectQuery = GetActiveQuery();

	if( spSelectQuery == NULL )
		return;

	BOOL bOpen = FALSE;
	spSelectQuery->IsOpen( &bOpen );
	if( !bOpen)
		return;

	
	long lRecordCount = -1;
	spSelectQuery->GetRecordCount( &lRecordCount );

	if( lRecordCount < 1 )
		return;

	
	

	VERIFY_IS_WINDOW_NULL(this)

	CRect rcClient;
	GetClientRect( &rcClient );

	CCellRange cellRange;
	cellRange = GetVisibleNonFixedCellRange( rcClient );


	sptrINamedData spND( GetDBaseDocument() );
	if( spND == NULL )
		return;

	int nStartRow	= cellRange.Top();
	int nEndRow		= cellRange.Bottom();

	if( nStartRow < m_nSecondaryRowCount )
		nStartRow = m_nSecondaryRowCount;

	spSelectQuery->EnterPrivateMode();

	for( int iRow=nStartRow;iRow<=nEndRow;iRow++ )
	{
		bool bRowNeedFill = false;
		for( int iCol=1;iCol<GetColumnCount();iCol++ )
		{
			if( iCol < m_arColumnInfo.GetSize() + 1 )
			{
				long nData = GetItemData( iRow, iCol );
				if( nData != ID_CELL_WAS_READ )
				{
					bRowNeedFill = true;
					break;
				}
			}
		}

		if( bRowNeedFill )
		{
			int nRecordNum = GetRecordNumFormRow( iRow );
			long nRealRecord;
			spSelectQuery->SetCurrentRecord( nRecordNum, &nRealRecord );

			/*{
				char sz_buf[100];
				sprintf( sz_buf, "%d", nRecordNum );
				SetItemText( iRow, 0, sz_buf );
			}*/

			for( int i=0;i<m_arColumnInfo.GetSize();i++ )
			{				
				CString strValue;
				GetValueFromData( spND, i, strValue );
				SetItemText( iRow, i + 1, strValue );			
				SetItemData( iRow, i + 1, ID_CELL_WAS_READ );
			}
		}
	}

	spSelectQuery->LeavePrivateMode( TRUE, FALSE );	
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var )
{
	if( !strcmp( pszEvent, szDBaseEventAfterQueryClose ) )
	{
		SaveGridToQuery();
	}

	if( !strcmp( pszEvent, szDBaseEventGridOptionsChange ) ||
		!strcmp( pszEvent, szDBaseEventAfterQueryOpen) ||
		!strcmp( pszEvent, szDBaseEventAfterQueryClose) ||
		!strcmp( pszEvent, szDBaseEventQueryOpenFailed) ||
		!strcmp( pszEvent, szDBaseEventConnectionClose)
		)
	{
		BuildAppearance();				
	}
	else	
	if( !strcmp( pszEvent, szDBaseEventAfterInsertRecord ) )
	{
		AfterInsertRecord();
	}
	else	
	if( !strcmp( pszEvent, szDBaseEventBeforDeleteRecord) )	
	{
		sptrISelectQuery spSelectQuery = GetActiveQuery();
		if( spSelectQuery != NULL )
		{			
			if( S_OK != spSelectQuery->GetCurrentRecord( &m_nRecordDelete ) )
				m_nRecordDelete = -1;
			
		}		
	}
	if( !strcmp( pszEvent, szDBaseEventAfterDeleteRecord) )	
	{
		AfterDeleteRecord( m_nRecordDelete );
	}
	else
	if( !strcmp( pszEvent, szDBaseEventAfterNavigation) )
	{
		AfterNavigation();		
	}
	if( !strcmp( pszEvent, szDBaseEventBeforNavigation) || !strcmp( pszEvent, szDBaseEventBeforInsertRecord) )
	{
		EndEditing();
	}		
	else
	if( !strcmp( pszEvent, szDBaseEventFieldChange ) )
	{
		AfterFieldChange( bstrTableName, bstrFieldName, var );
	}
	else
	if( !strcmp( pszEvent, szDBaseEventActiveFieldSet ) )
	{
		ActivateField( bstrTableName, bstrFieldName );
	}
	
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::OnEditCell(int nRow, int nCol, CPoint point, UINT nChar)
{	
	ForceGoToRecord( );	

	CGridCtrl::OnEditCell( nRow, nCol, point, nChar);
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::OnEndEditCell(int nRow, int nCol, CString str)
{			

	CGridCtrl::OnEndEditCell( nRow, nCol, str );		

	if( nRow < m_nSecondaryRowCount || nRow >= GetRowCount() )
		return;

	if( nCol -1 < 0 || nCol - 1 >= m_arColumnInfo.GetSize() )
		return;

	sptrIDBaseDocument spDoc = GetDBaseDocument();
	if( spDoc == NULL )
		return;

	sptrINamedData spND( spDoc );
	if( spND == NULL )
		return;


	CString strTable = m_arColumnInfo[nCol-1]->m_strTable;
	CString strField = m_arColumnInfo[nCol-1]->m_strField;	

	CString srtKey;
	srtKey.Format( "%s\\%s.%s", (LPCTSTR)SECTION_DBASEFIELDS, (LPCTSTR)strTable, (LPCTSTR)strField );

	FieldType fieldType = ::GetFieldType( spDoc, strTable, strField );
	if( !(
		fieldType == ftString ||
		fieldType == ftDigit ||
		fieldType == ftDateTime
		))
		return;

	_variant_t var;
	try{
		if( str.IsEmpty() )
		{
			var.ChangeType( VT_NULL );
		}
		else
		{
			var = str;

			if( fieldType == ftDigit )
			{
				var.ChangeType( VT_R8 );
			}
			else		
			if( fieldType == ftDateTime )
			{
				var.ChangeType( VT_DATE );
			}		
		}

		//spND->SetupSection( _bstr_t( SECTION_DBASEFIELDS ) );
		spND->SetValue( _bstr_t( (LPCTSTR)srtKey ), var );

	}
	catch(...)
	{
		try{
			tagVARIANT oldVar;
			VariantInit(&oldVar);
			
			//spND->SetupSection( _bstr_t( SECTION_DBASEFIELDS ) );
			spND->GetValue( _bstr_t( (LPCTSTR)srtKey ), &oldVar );
			_variant_t _oldVar( oldVar );
		
			CString strValue;
			if( !(_oldVar.vt == VT_EMPTY || _oldVar.vt == VT_NULL) )			
			{
				_oldVar.ChangeType( VT_BSTR );
				strValue = _oldVar.bstrVal;
			}
			else
			{
				strValue = "";
			}		
			
			SetItemText( nRow, nCol, strValue );
			InvalidateCellRect( nRow, nCol );
		}
		catch(...)
		{

		}

	}
}	




/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::StartTimer()
{
	m_nTimerID = 1;
	m_nTimerID = SetTimer( m_nTimerID, 100, NULL );

}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::StopTimer()
{
	if( m_nTimerID != -1)
		KillTimer( m_nTimerID );

	m_nTimerID = -1;

}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::OnTimer(UINT_PTR nIDEvent) 
{
	if( nIDEvent == m_nTimerID && m_bTimerActivate )
	{
		long nCurTickCount = ::GetTickCount();

		long lNavigatePeriod = ::_GetValueInt( ::GetAppUnknown(), 
								SHELL_DATA_DB_SECTION, SHELL_DATA_DB_NAVIGATE_PERIOD, 500 );

		if( nCurTickCount - m_lRecordStartTickCount >= lNavigatePeriod )
		{			
			ForceGoToRecord( );
			m_bTimerActivate = false;
			m_lCurrentTimerRecord = -1;
		}

	}

	CGridCtrl::OnTimer(nIDEvent);
}


/////////////////////////////////////////////////////////////////////////////
bool CDBaseGridCtrl::ForceGoToRecord( )
{
	if( !m_bTimerActivate )
		return false;

	sptrISelectQuery spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return false;

	long lRecordCount, lCurrentRecord;
	lRecordCount = lCurrentRecord = 0;

	spQuery->GetCurrentRecord( &lCurrentRecord );
	spQuery->GetRecordCount( &lRecordCount );

	if( lRecordCount >= 1 &&
		m_lCurrentTimerRecord >= 1 && m_lCurrentTimerRecord <= lRecordCount &&
		m_lCurrentTimerRecord != lCurrentRecord
		)
	{
		m_bTimerActivate = false;
		spQuery->GoToRecord( m_lCurrentTimerRecord );
		return true;
	}

	return false;

}


/////////////////////////////////////////////////////////////////////////////
bool CDBaseGridCtrl::GotoRecordSlow( long lNewCurrentRecord )
{
	//at first update
	sptrISelectQuery spQuery = GetActiveQuery();
	if( spQuery )
		spQuery->UpdateInteractive( TRUE );

	m_lCurrentTimerRecord = lNewCurrentRecord;
	m_lRecordStartTickCount = ::GetTickCount();
	m_bTimerActivate = true;

	return true;		
}


					
/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::OnParentGetSelChangeMessage( )
{
	if( !m_bCanProcessOnChangeRecordScroll ) 
		return;
	
	//support toolbar update in ShellFrame
	::ForceAppIdleUpdate();

	int nCurRecord = GetCurrentRecord();
	if( nCurRecord <= 0 ) 
		return;

	if( GetFocusCell().row == m_nPrevRow ) 
		return;

	GotoRecordSlow( nCurRecord );	
}

/////////////////////////////////////////////////////////////////////////////
int CDBaseGridCtrl::GetRecordNumFormRow( int nRow )
{	
	int nRowCount	= GetRowCount();

	if( nRow < 1 || nRow >= nRowCount )
		return GRID_NONE_RECORD;

	if( nRow == 0 )
		return GRID_HEADER_RECORD;

	if( nRow == 1 )
	{
		if( m_bUseFilterRow )
		{
			return GRID_FILTER_RECORD;
		}
		else
		{
			if( m_bUseSortRow )
				return GRID_SORT_RECORD;
		}		
	}

	if( nRow == 2 ) 
	{
		if( m_bUseFilterRow && m_bUseSortRow )
			return GRID_SORT_RECORD;
	}

	if( nRow >= m_nSecondaryRowCount && nRow < nRowCount )
		return nRow - m_nSecondaryRowCount + 1;

	return GRID_NONE_RECORD;
}

/////////////////////////////////////////////////////////////////////////////
int CDBaseGridCtrl::GetRowFormRecordNum( int nRecordNum )
{
	int nRowCount	= GetRowCount();
	int nRow = m_nSecondaryRowCount + nRecordNum - 1;
	if( nRow < m_nSecondaryRowCount || nRow >= GetRowCount() )
		return -1;

	return nRow;
}

/////////////////////////////////////////////////////////////////////////////
int CDBaseGridCtrl::GetCurrentRecord()
{	
	return GetRecordNumFormRow( GetFocusCell().row );
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::SetCurrentRecord( int nRecordNum )
{
	int nRowCount = GetRowCount();
	int nNewRow = nRecordNum + m_nSecondaryRowCount - 1;
	if( nNewRow <= 0 || nNewRow >= nRowCount )
		return;

	CCellID curCell = GetFocusCell();
	curCell.row = nNewRow;
	if( !(curCell.col > 0 && curCell.col < GetColumnCount() ))
	{
		curCell.col = 1;
	}	

	m_bCanProcessOnChangeRecordScroll = false;
	
	SetFocusCell( curCell );	
	//SetSelectedRange( curCell.row, curCell.col, curCell.row, curCell.col );
	EnsureVisible( curCell );
	Invalidate();
	AnalizeNewValueSet();
	
	m_bCanProcessOnChangeRecordScroll = true;	
}



/////////////////////////////////////////////////////////////////////////////
bool CDBaseGridCtrl::SetValueToData( sptrINamedData spND, int nCol, CString strValue )
{
	ASSERT( spND != NULL );
	if( spND == NULL )
		return false;

	return true;
}



/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::SetGridAsEmpty()
{
	DestroyColumnInfoArr();

	SetRowCount( 0 );
	SetColumnCount( 0 );
}

/////////////////////////////////////////////////////////////////////////////
CScrollBar* CDBaseGridCtrl::GetScrollBarCtrl(int nBar) const
{
	CWnd* pParent = GetParent();
	if( pParent == NULL )
		return NULL;

	if( !::IsWindow( pParent->GetSafeHwnd() ) )
		return NULL;	

	return pParent->GetScrollBarCtrl( nBar );
}


/////////////////////////////////////////////////////////////////////////////
//Close, Open, After Navi, Insert, Delete, OnSize
void CDBaseGridCtrl::FillGridFirstTime()
{
		

	VERIFY_IS_WINDOW_NULL(this)	  
		
	//Record colors
	COLORREF clrRecordBack = /*::GetValueColor( ::GetAppUnknown(), 
					SHELL_DATA_DB_GRID, SHELL_DATA_RECORD_BACK_COLOR,*/
					::GetSysColor(COLOR_WINDOW);

	COLORREF clrRecordText = /*::GetValueColor( ::GetAppUnknown(), 
					SHELL_DATA_DB_GRID, SHELL_DATA_RECORD_TEXT_COLOR,*/
					::GetSysColor(COLOR_WINDOWTEXT);

	//Headers colors
	COLORREF clrHeaderBack = /*::GetValueColor( ::GetAppUnknown(), 
					SHELL_DATA_DB_GRID, SHELL_DATA_HEADER_BACK_COLOR,*/
					::GetSysColor(COLOR_BTNFACE);

	COLORREF clrHeaderText = /*::GetValueColor( ::GetAppUnknown(), 
					SHELL_DATA_DB_GRID, SHELL_DATA_HEADER_TEXT_COLOR,*/
					::GetSysColor(COLOR_WINDOWTEXT);


	//Info column colors
	COLORREF clrInfoColumnBack = /*::GetValueColor( ::GetAppUnknown(), 
					SHELL_DATA_DB_GRID, SHELL_DATA_INFOCOLUMN_BACK_COLOR,*/
					::GetSysColor(COLOR_BTNFACE);

	COLORREF clrInfoColumnText = /*::GetValueColor( ::GetAppUnknown(), 
					SHELL_DATA_DB_GRID, SHELL_DATA_INFOCOLUMN_TEXT_COLOR,*/
					::GetSysColor(COLOR_HIGHLIGHT);

	//Filter colors
	COLORREF clrFilterBack = /*::GetValueColor( ::GetAppUnknown(), 
					SHELL_DATA_DB_GRID, SHELL_DATA_FILTER_BACK_COLOR,*/
					::GetSysColor(COLOR_WINDOW);

	COLORREF clrFilterText = /*::GetValueColor( ::GetAppUnknown(), 
					SHELL_DATA_DB_GRID, SHELL_DATA_FILTER_TEXT_COLOR,*/
					::GetSysColor(COLOR_WINDOWTEXT);

	//Sort colors
	COLORREF clrSortBack = /*::GetValueColor( ::GetAppUnknown(), 
					SHELL_DATA_DB_GRID, SHELL_DATA_SORT_BACK_COLOR,*/
					::GetSysColor(COLOR_WINDOW);

	COLORREF clrSortText = /*::GetValueColor( ::GetAppUnknown(), 
					SHELL_DATA_DB_GRID, SHELL_DATA_SORT_TEXT_COLOR,*/
					::GetSysColor(COLOR_WINDOWTEXT);
	
	

	int nColumnCount = m_arColumnInfo.GetSize();

	if( nColumnCount < 1 )
	{
		SetGridAsEmpty();
		return;
	}

	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
	{
		SetGridAsEmpty();
		return;
	}
	
	int nOldRowCount = GetRowCount();
	int nRecordCount = GetRecordsetRowCount();


	if( nRecordCount <= 0 )
	{
		//SetGridAsEmpty();
		//return;
	}

	//if( nRecordCount != nOldRowCount  )
	SetRowCount( nRecordCount + m_nSecondaryRowCount );

	//Set item flag, that they was read
	for( int iRow=m_nSecondaryRowCount;iRow<GetRowCount();iRow++ )
	{
		for( int iCol=1;iCol<GetColumnCount();iCol++ )
		{
			SetItemData( iRow, iCol, ID_CELL_WAS_NOT_READ );
		}
	}


	sptrISelectQuery spSelectQuery = GetActiveQuery();

	if( spSelectQuery == NULL )
	{
		SetGridAsEmpty();
		return;
	}

	BOOL bOpen = FALSE;
	spSelectQuery->IsOpen( &bOpen );
	if( !bOpen)
	{
		//SetGridAsEmpty();
		//return;
	}

	VERIFY_IS_WINDOW_NULL(this)

	CRect rcClient;
	GetClientRect( &rcClient );

	CCellRange cellRange;
	cellRange = GetVisibleNonFixedCellRange( rcClient );


	sptrINamedData spND( GetDBaseDocument() );
	if( spND == NULL )
	{
		SetGridAsEmpty();
		return;
	}	

	int nFilterRow, nSortRow;
	nFilterRow = GetFilterRow();
	nSortRow = GetSortRow();


	////////////set filter row
	if( nFilterRow != -1 )
	{
		for( int i=0;i<nColumnCount;i++ )
		{						
			SetItemState( nFilterRow, i+1, GVIS_READONLY );
		}
		SetItemText( nFilterRow, 0, "Filters:" );			
		SetItemState( nFilterRow, 0, GVIS_READONLY );
	}
	
	 
	////////////set sort row
	if( nSortRow != -1 )
	{
		for( int i=0;i<nColumnCount;i++ )
		{
			SetItemState( nSortRow, i+1, GVIS_READONLY );
		}
		SetItemText( nSortRow, 0, "Sorting:" );
	}



	//Set record color
	SetTextColor( clrRecordText );
	SetTextBkColor( clrRecordBack );


	//set header color	
	for( int i=0;i<nColumnCount;i++ )
	{
		SetItemBkColour( 0, i, clrHeaderBack );
		SetItemFgColour( 0, i, clrHeaderText);
	}

	//set infocolumn color	
	for( i=1;i<nRecordCount+m_nSecondaryRowCount;i++ )
	{
		SetItemBkColour( i, 0, clrInfoColumnBack );
		SetItemFgColour( i, 0, clrInfoColumnText);
	}
	
	
	//set filter color
	if( nFilterRow != -1 )
	{
		for( int i=1;i<nColumnCount;i++ )
		{
			SetItemBkColour( nFilterRow, i, clrFilterBack );
			SetItemFgColour( nFilterRow, i, clrFilterText);
		}
	}

	//set sort color
	if( nSortRow != -1 )
	{
		for( int i=1;i<nColumnCount;i++ )
		{
			SetItemBkColour( nSortRow, i, clrSortBack );
			SetItemFgColour( nSortRow, i, clrSortText );
		}
	}

	ResetScrollBars();

}


/////////////////////////////////////////////////////////////////////////////
int CDBaseGridCtrl::GetFilterRow()
{
	if( m_bUseFilterRow )
		return 1;

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
int CDBaseGridCtrl::GetSortRow()
{
	if( !m_bUseSortRow )
		return -1;

	if( m_bUseFilterRow )
		return 2;
	
	return 1;
}


/////////////////////////////////////////////////////////////////////////////
long CDBaseGridCtrl::GetRecordsetRowCount()
{
	sptrISelectQuery spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return 0;

	BOOL bOpen = FALSE;
	spQuery->IsOpen( &bOpen );
	if( !bOpen )
		return 0;

	
	long nRecordCount = 0;
	if( S_OK != spQuery->GetRecordCount( &nRecordCount ) )
		return 0;

	return nRecordCount;
}


void CDBaseGridCtrl::Init()
{
	StartTimer();
	BuildAppearance( );	
}

/////////////////////////////////////////////////////////////////////////////
int CDBaseGridCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CGridCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{	
	CGridCtrl::OnHScroll(nSBCode, nPos, pScrollBar);	
}



/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{	
	CGridCtrl::OnVScroll(nSBCode, nPos, pScrollBar);	
	AnalizeNewValueSet();		
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::OnDestroy() 
{	  
	StopTimer();
	SaveGridToQuery();

	CGridCtrl::OnDestroy();		
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::SaveGridToQuery()
{
	IQueryMiscPtr ptrQ = GetActiveQuery();	

	if( ptrQ == NULL )
		return;

	ptrQ->EmptyGridFieldInfo();

	for( int iCol=0;iCol<m_arColumnInfo.GetSize();iCol++ )
	{
		CGridColumnInfo* pi = m_arColumnInfo[iCol];

		ptrQ->SetGridFieldInfo( _bstr_t( (LPCSTR)pi->m_strTable ), 
								_bstr_t( (LPCSTR)pi->m_strField ), 
								GetColumnWidth( iCol + 1 ), iCol );
		
	}		


	
}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseGridCtrl::SaveGridSettingsToQuery()
{
	return true;
	/*
	sptrIQueryObject spActiveQuery = GetActiveQuery();	

	if( spActiveQuery == NULL )
		return false;

	int nCount;
	spActiveQuery->GetGridFieldCount( &nCount );

	//At first delete
	for( int i=nCount-1;i>=0;i--)
	{
		spActiveQuery->DeleteGridField( i );
	}
	

	//int nColumnCount = GetColumnCount();
	//Now fill
	for( int iCol=0;iCol<m_arColumnInfo.GetSize();iCol++ )
	{
		CGridColumnInfo* pColumnInfo = m_arColumnInfo[iCol];
		if( pColumnInfo )
		{
			CString strCaption = GetItemText( 0, iCol + 1 );
			int nColumnWidth = GetColumnWidth( iCol + 1 );

			spActiveQuery->AddGridField(
						_bstr_t( (LPCTSTR)pColumnInfo->m_strTable ),
						_bstr_t( (LPCTSTR)pColumnInfo->m_strField ),
						_bstr_t( (LPCTSTR)strCaption ),
						nColumnWidth
						);
		}
	}


	return true;
	*/
}


/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::FillFilterRow()
{
	int nFilterRow = GetFilterRow();
	if( nFilterRow == -1 )
		return;

	for( int i=0;i<m_arColumnInfo.GetSize();i++ )
	{
		SetCurrentFilter( i, 0 );
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::SetCurrentFilter( int nColumnInfoIndex, int nSubIndex )
{
	int nFilterRow = GetFilterRow();
	if( nFilterRow == -1 )
		return;

	int nFilterCol = -1;

	if( nColumnInfoIndex < 0 || nColumnInfoIndex >= m_arColumnInfo.GetSize() )
		return;

	nFilterCol = nColumnInfoIndex + 1;

	if( !IsValid( nFilterRow, nFilterCol ) )
		return;

	UINT uOldState = GetItemState( nFilterRow, nFilterCol );
	SetItemState( nFilterRow, nFilterCol, uOldState & ~GVIS_READONLY );
	SetItemData( nFilterRow, nFilterCol, -1 );
	SetItemText( nFilterRow, nFilterCol, "" );
	SetItemState( nFilterRow, nFilterCol, uOldState | GVIS_READONLY );

	

	CString strTable = m_arColumnInfo[nColumnInfoIndex]->m_strTable;
	CString strField = m_arColumnInfo[nColumnInfoIndex]->m_strField;

	CString strFilter = GetFilterString( strTable, strField, nSubIndex );

	if( strFilter.IsEmpty() )
		return;

	uOldState = GetItemState( nFilterRow, nFilterCol );
	SetItemState( nFilterRow, nFilterCol, uOldState & ~GVIS_READONLY );

	SetItemData( nFilterRow, nFilterCol, nSubIndex );
	SetItemText( nFilterRow, nFilterCol, strFilter );

	SetItemState( nFilterRow, nFilterCol, uOldState | GVIS_READONLY );

	InvalidateCellRect( nFilterRow, nFilterCol );
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::FillSortRow()
{

	int nSortRow = GetSortRow();
	if( nSortRow == -1 )
		return;

	for( int i=0;i<m_arColumnInfo.GetSize();i++ )
	{
		int nCol = i+1;
		if( IsValid( nSortRow, nCol ) )
		{
			CString strTable = m_arColumnInfo[i]->m_strTable;
			CString strField = m_arColumnInfo[i]->m_strField;

			UINT uOldState = GetItemState( nSortRow, nCol );
			SetItemState( nSortRow, nCol, uOldState & ~GVIS_READONLY );			
			SetItemText( nSortRow, nCol, GetSortString( strTable, strField ) );
			SetItemState( nSortRow, nCol, uOldState | GVIS_READONLY );
		}
	}

}
/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::SetCurrentSort( int nColumnInfoIndex, int nSubIndex )
{
	return;
	/*
	int nSortRow = GetSortRow();
	if( nSortRow == -1 )
		return;

	int nSortCol = -1;

	if( nColumnInfoIndex < 0 || nColumnInfoIndex >= m_arColumnInfo.GetSize() )
		return;

	nSortCol = nColumnInfoIndex + 1;

	if( !IsValid( nSortRow, nSortCol ) )
		return;


	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return;


	CString strTable = m_arColumnInfo[nColumnInfoIndex]->m_strTable;
	CString strField = m_arColumnInfo[nColumnInfoIndex]->m_strField;

	int nSortIndex = -1;
	int nSortFieldsCount = 0;
	IS_OK( spQuery->GetSortFieldCount( &nSortFieldsCount ) );
	for( int i=0;i<nSortFieldsCount;i++ )
	{
		BSTR bstrTableName, bstrFieldName;
		BOOL bASC;
		if( S_OK == ( spQuery->GetSortField( i, &bstrTableName, &bstrFieldName, &bASC ) ) )
		{	
			CString strTableS = bstrTableName;
			CString strFieldS = bstrFieldName;
			::SysFreeString( bstrTableName );
			::SysFreeString( bstrFieldName );
			if( strTable == strTableS && strFieldS == strField )			
				nSortIndex = i;
		}
	}

	if( nSortIndex != -1 )
	{
		if( nSubIndex == ID_SORT_NONE )
		{
			spQuery->DeleteSortField( nSortIndex );	
		}
		else
		if( nSubIndex == ID_SORT_ASCENDING )
		{
			spQuery->EditSortField( nSortIndex, _bstr_t( (LPCTSTR)strTable ), _bstr_t( (LPCTSTR)strField ), TRUE );
		}
		else
		if( nSubIndex == ID_SORT_DESCENDING )
		{
			spQuery->EditSortField( nSortIndex, _bstr_t( (LPCTSTR)strTable ), _bstr_t( (LPCTSTR)strField ), FALSE );
		}
	}
	else//Its mean that we must add new sort
	{
		if( nSubIndex == ID_SORT_ASCENDING )
		{
			spQuery->AddSortField( _bstr_t( (LPCTSTR)strTable ), _bstr_t( (LPCTSTR)strField ), TRUE );
		}
		else
		if( nSubIndex == ID_SORT_DESCENDING )
		{
			spQuery->AddSortField( _bstr_t( (LPCTSTR)strTable ), _bstr_t( (LPCTSTR)strField ), FALSE );
		}
	}

	CString strSort = GetSortString( strTable, strField );


	UINT uOldState = GetItemState( nSortRow, nSortCol );
	SetItemState( nSortRow, nSortCol, uOldState & ~GVIS_READONLY );	
	SetItemText( nSortRow, nSortCol, strSort );
	SetItemState( nSortRow, nSortCol, uOldState | GVIS_READONLY );

	InvalidateCellRect( nSortRow, nSortCol );

	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	::ExecuteAction( GetActionNameByResID( IDS_REQUERY ) );
	*/

}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::OnFilterPopup( int nCol )
{
	int nFilterRow = GetFilterRow();
	if( nFilterRow == -1 )
		return;

	int nFilterCol = nCol;
	int nColumnInfoIndex = nCol - 1;

	if( nColumnInfoIndex < 0 || nColumnInfoIndex >= m_arColumnInfo.GetSize() )
		return;	

	if( !IsValid( nFilterRow, nFilterCol ) )
		return;


	int nCurSubItem = GetItemData( nFilterRow, nFilterCol );

	CString strTable = m_arColumnInfo[nColumnInfoIndex]->m_strTable;
	CString strField = m_arColumnInfo[nColumnInfoIndex]->m_strField;

	int nCount = GetFilterCount( strTable, strField );
	if( nCount < 1 )
		return;

	CRect rcCell;
	GetCellRect( nFilterRow, nFilterCol, &rcCell);

	ClientToScreen( &rcCell );

	CPopupList* pList = new CPopupList;
	CRect rc = CRect( 0, 0, 0, 0 );	
	pList->Create( LBS_NOTIFY | WS_CHILD | WS_BORDER | WS_VSCROLL, rc, this, 100 );

	for( int i=0;i<nCount;i++ )
	{
		pList->AddString( GetFilterString( strTable, strField, i ) );
	}
		

	rc = pList->GetInitRect( CPoint( rcCell.left, rcCell.bottom), rcCell.Width(), 200 );	

	CPopup* pPopup = (CPopup*)RUNTIME_CLASS(CPopup)->CreateObject();    
    VERIFY(pPopup->Display (pList, this, rc, &pList->m_xPopupCtrlEvent, 
		&m_xPopupCtrlReciever));

}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::OnSortPopup( int nCol )
{
	int nSortRow = GetSortRow();
	if( nSortRow == -1 )
		return;

	int nSortCol = nCol;
	int nColumnInfoIndex = nCol - 1;

	if( nColumnInfoIndex < 0 || nColumnInfoIndex >= m_arColumnInfo.GetSize() )
		return;	

	if( !IsValid( nSortRow, nSortCol ) )
		return;


	int nCurSubItem = GetItemData( nSortRow, nSortCol );

	CString strTable = m_arColumnInfo[nColumnInfoIndex]->m_strTable;
	CString strField = m_arColumnInfo[nColumnInfoIndex]->m_strField;

	CRect rcCell;
	GetCellRect( nSortRow, nSortCol, &rcCell);

	ClientToScreen( &rcCell );

	CPopupList* pList = new CPopupList;
	CRect rc = CRect( 0, 0, 0, 0 );	
	pList->Create( LBS_NOTIFY | WS_CHILD | WS_BORDER | WS_VSCROLL, rc, this, 100 );

	/*
	#define ID_SORT_NONE		0
	#define ID_SORT_ASCENDING	1
	#define ID_SORT_DESCENDING	2
	*/


	CString strAdd;
	strAdd.LoadString(IDS_SORT_NONE);
	pList->AddString( strAdd );	
	strAdd.LoadString(IDS_SORT_ASCENDING);
	pList->AddString( strAdd );
	strAdd.LoadString(IDS_SORT_DESCENDING);
	pList->AddString( strAdd );

	rc = pList->GetInitRect( CPoint( rcCell.left, rcCell.bottom), rcCell.Width(), 200 );	

	CPopup* pPopup = (CPopup*)RUNTIME_CLASS(CPopup)->CreateObject();    
    VERIFY(pPopup->Display (pList, this, rc, &pList->m_xPopupCtrlEvent, 
		&m_xPopupCtrlReciever));
}

/////////////////////////////////////////////////////////////////////////////
CString CDBaseGridCtrl::GetSortString( CString strTable, CString strField )
{
	//Need recover
	/*	
	CString strNone, strAscending, strDescending;

	strNone.LoadString(IDS_SORT_NONE);
	strAscending.LoadString(IDS_SORT_ASCENDING);
	strDescending.LoadString(IDS_SORT_DESCENDING);	
	
	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return strNone;


	int nColInfoIndex = GetInfoColumnFromTableField( strTable, strField );
	if( nColInfoIndex == -1 )
		return strNone;

	strTable	= m_arColumnInfo[nColInfoIndex]->m_strTable;
	strField	= m_arColumnInfo[nColInfoIndex]->m_strField;

	int nSortFieldsCount = 0;
	IS_OK( spQuery->GetSortFieldCount( &nSortFieldsCount ) );
	for( int i=0;i<nSortFieldsCount;i++ )
	{
		BSTR bstrTableName, bstrFieldName;
		BOOL bASC;
		if( S_OK == ( spQuery->GetSortField( i, &bstrTableName, &bstrFieldName, &bASC ) ) )
		{	
			CString strTableS = bstrTableName;
			CString strFieldS = bstrFieldName;
			::SysFreeString( bstrTableName );
			::SysFreeString( bstrFieldName );
			if( strTable == strTableS && strFieldS == strField )
			{
				if( bASC )
					return strAscending;
				else
					return strDescending;
			}
		}
	}

  
	return strNone;
	*/

	return "";

}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	/*
	CCellID cell = GetFocusCell();
	if( cell.IsValid() )
	{
		if( cell.col > 0 )
		{
			if( cell.row == GetFilterRow())
				OnFilterPopup( cell.col );
			else
			if( cell.row == GetSortRow())			
				OnSortPopup( cell.col );
		}
	}
	*/
	CGridCtrl::OnLButtonDblClk(nFlags, point);	
}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseGridCtrl::XPopupCtrlReciever::OnNotify( const char* szEvent,  CString strValue )
{
	METHOD_PROLOGUE_(CDBaseGridCtrl, PopupCtrlReciever);
	return true;

}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseGridCtrl::XPopupCtrlReciever::OnNotify( const char* szEvent,  UINT nValue )
{
	METHOD_PROLOGUE_(CDBaseGridCtrl, PopupCtrlReciever);
	if( !strcmp( szEvent, szChooserEventUINTValueChange ) )
	{
		CCellID cell =pThis->GetFocusCell();
		if( cell.IsValid() )
		{
			if( cell.row == pThis->GetFilterRow() )
				pThis->SetCurrentFilter( cell.col - 1, (int)nValue );

			if( cell.row == pThis->GetSortRow() )
				pThis->SetCurrentSort( cell.col - 1, (int)nValue );

		}
	}
	return true;

}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{	
	CGridCtrl::OnRButtonDown(nFlags, point);

	CCellID cellPick = GetCellFromPt( point );

	CCellID cell = GetFocusCell();

	if( cellPick != cell )
		return;


	if( cell.IsValid() )
	{
		if( cell.col > 0 )
		{
			if( cell.col > 0 && cell.row == GetFilterRow())
			{
				PopupFilterMenu( cell, point );
			}				
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::PopupFilterMenu( CCellID cell, CPoint point )
{
	CString strAddFilter, strEditFilter, strDeleteFilter;

	strAddFilter.LoadString( IDS_MENU_ADD_FILTER );
	strEditFilter.LoadString( IDS_MENU_EDIT_FILTER );
	strDeleteFilter.LoadString( IDS_MENU_DELETE_FILTER );	

	CString strTable, strField;
	int nSubIndex;

	if( !GetTableFieldSubIndexFromCurrentCell( strTable, strField, nSubIndex ) )
		return;

	UINT nState = MF_ENABLED;
	if( nSubIndex < 0 )
		nState = MF_GRAYED;


	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu( MF_ENABLED | MF_STRING, IDS_MENU_ADD_FILTER, strAddFilter );
	menu.AppendMenu( nState | MF_STRING, IDS_MENU_EDIT_FILTER, strEditFilter );
	menu.AppendMenu( nState | MF_STRING, IDS_MENU_DELETE_FILTER, strDeleteFilter );
		
	ClientToScreen( &point );
	menu.TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this );
}



//Filter helper
/////////////////////////////////////////////////////////////////////////////
int CDBaseGridCtrl::GetFilterCount( CString strTable, CString strField )
{
	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return -1;


	int nCount = -1;

	/*

	if( S_OK != spQuery->GetFilterCountByFieldTable(
							_bstr_t( (LPCTSTR)strTable ),
							_bstr_t( (LPCTSTR)strField ),
							&nCount
							))
			return -1;
			*/

	return nCount;
}

/////////////////////////////////////////////////////////////////////////////
CString CDBaseGridCtrl::GetFilterString( CString strTable, CString strField, int nSubIndex )
{
	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return "";


	int nFilterIndex = GetIndexFromSubIndex(strTable, strField, nSubIndex );
	if( nFilterIndex < 0 )
		return "";
	return "";
	/*
	BSTR bstrFilterName;
	if( S_OK != spQuery->GetFilterAsShortString( nFilterIndex, &bstrFilterName ) )
		return "";

	CString strFilter = bstrFilterName;
	::SysFreeString( bstrFilterName );

	return strFilter;
	*/
}

/////////////////////////////////////////////////////////////////////////////
int CDBaseGridCtrl::GetIndexFromSubIndex(CString strTable, CString strField, int nSubIndex )
{
	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return -1;


	/*
	if( nSubIndex >=0 && nSubIndex < GetFilterCount(strTable, strField) )
	{
		int nFilterIndex = -1;
		if( S_OK != spQuery->GetFilterIndexByFieldTable(
								_bstr_t( (LPCTSTR)strTable ),
								_bstr_t( (LPCTSTR)strField ),
								nSubIndex,
								&nFilterIndex
								))
				return -1;

		return nFilterIndex;
	}
	*/

	return -1;

}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseGridCtrl::GetTableFieldSubIndexFromCurrentCell( CString& strTable, CString& strField, int& nSubIndex )
{
	CCellID cell = GetFocusCell();
	if( !cell.IsValid() )
		return false;
	
	int nColInfoIndex = cell.col - 1;
	
	if( nColInfoIndex < 0 || nColInfoIndex >= m_arColumnInfo.GetSize() )
		return false;

	if( cell.row != GetFilterRow() ) 
		return false;

	strTable	= m_arColumnInfo[nColInfoIndex]->m_strTable;
	strField	= m_arColumnInfo[nColInfoIndex]->m_strField;
	nSubIndex	= GetItemData( cell.row, cell.col );	

	return true;

}

int CDBaseGridCtrl::GetInfoColumnFromTableField( CString strTable, CString strField )
{
	int nIndex = -1;
	for( int i=0;i<m_arColumnInfo.GetSize();i++ )
	{
		if( m_arColumnInfo[i]->m_strTable == strTable && 
			m_arColumnInfo[i]->m_strField == strField
			)
			nIndex = i;
	}

	return nIndex;
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::OnAddFilter()
{
	CString strTable, strField;
	int nSubIndex;

	if( !GetTableFieldSubIndexFromCurrentCell( strTable, strField, nSubIndex ) )
		return;

	if( !AddFilter( strTable, strField ) )
		return;
	
	SetCurrentFilter( GetInfoColumnFromTableField(strTable, strField), 
						GetFilterCount(strTable, strField) - 1 );

	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	::ExecuteAction( GetActionNameByResID( IDS_REQUERY ) );

}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::OnEditFilter()
{
	CString strTable, strField;
	int nSubIndex;

	if( !GetTableFieldSubIndexFromCurrentCell( strTable, strField, nSubIndex ) )
		return;

	if( !EditFilter( strTable, strField, nSubIndex ) )
		return;

	SetCurrentFilter( GetInfoColumnFromTableField(strTable, strField), 
						nSubIndex );

	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	::ExecuteAction( GetActionNameByResID( IDS_REQUERY ) );

}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridCtrl::OnDeleteFilter()
{
	CString strTable, strField;
	int nSubIndex;

	if( !GetTableFieldSubIndexFromCurrentCell( strTable, strField, nSubIndex ) )
		return;

	if( !DeleteFilter( strTable, strField, nSubIndex ) )
		return;

	SetCurrentFilter( GetInfoColumnFromTableField(strTable, strField), 
						0 );

	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	::ExecuteAction( GetActionNameByResID( IDS_REQUERY ) );

}


/////////////////////////////////////////////////////////////////////////////
bool CDBaseGridCtrl::EditFilter( CString strTable, CString strField, int nSubIndex )
{
	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return false;


	int nFilterIndex = GetIndexFromSubIndex(strTable, strField, nSubIndex );
	if( nFilterIndex < 0 )
		return false;

	
	return false;//( spQuery->EditFilterVisible( nFilterIndex ) ==S_OK );
}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseGridCtrl::DeleteFilter( CString strTable, CString strField, int nSubIndex )
{
	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return false;

	int nFilterIndex = GetIndexFromSubIndex(strTable, strField, nSubIndex );
	if( nFilterIndex < 0 )
		return false;

	
	return false;//(spQuery->DeleteFilter( nFilterIndex ) == S_OK);
}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseGridCtrl::AddFilter( CString strTable, CString strField )
{
	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return false;


	return false;//( spQuery->AddFilterVisible( _bstr_t( (LPCTSTR)strTable ), _bstr_t( (LPCTSTR)strField ) ) == S_OK);	
}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseGridCtrl::ActivateField( CString strTable, CString strField )
{
	CCellID curCell = GetFocusCell();

	if( IsValid( curCell ) )	
	{
		int nIndex = -1;
		for( int i=0;i<m_arColumnInfo.GetSize();i++ )
		{
			if( m_arColumnInfo[i]->m_strTable == strTable && 
				m_arColumnInfo[i]->m_strField == strField
				)
			{
				nIndex = i;
				break;
			}
		}

		if( nIndex != -1 )
		{
			curCell.col = nIndex + 1;
			if( IsValid( curCell ) )
			{
				m_bCanProcessOnChangeRecordScroll = false;
				SetFocusCell( curCell );	
				EnsureVisible( curCell );
				Invalidate();
				m_bCanProcessOnChangeRecordScroll = true;
				return true;
			}

		}		
	}	

	return false;
}

/////////////////////////////////////////////////////////////////////////////
//
//Filter Support
//
/////////////////////////////////////////////////////////////////////////////
bool CDBaseGridCtrl::GetActiveField( CString& strTable, CString& strField )
{
	CCellID cell = GetFocusCell();
	if( !cell.IsValid() )
		return false;
	
	int nColInfoIndex = cell.col - 1;
	
	if( nColInfoIndex < 0 || nColInfoIndex >= m_arColumnInfo.GetSize() )
		return false;

	sptrISelectQuery spSelectQuery( GetActiveQuery() );

	if( spSelectQuery == NULL )
		return false;

	BOOL bOpen = FALSE;
	spSelectQuery->IsOpen( &bOpen );
	if( !bOpen )
		return false;

	long nRecordCount = -1;
	spSelectQuery->GetRecordCount( &nRecordCount );
	if( nRecordCount < 1 )
		return false;
	
	strTable	= m_arColumnInfo[nColInfoIndex]->m_strTable;
	strField	= m_arColumnInfo[nColInfoIndex]->m_strField;	
	
	if( !IsAvailableFieldInQuery( spSelectQuery, strTable, strField ) )
		return false;

	return true;
}


bool CDBaseGridCtrl::GetActiveFieldValue( CString& strTable, CString& strField, CString& strValue )
{
	if( !GetActiveField( strTable, strField ) )
		return false;

	CString strKey;
	strKey.Format( "%s\\%s.%s",
						(LPCTSTR)SECTION_DBASEFIELDS, 
						(LPCTSTR)strTable, (LPCTSTR)strField );						

	sptrINamedData spND( GetDBaseDocument() );
	if( spND == NULL ) 
		return false;

	_variant_t var;
	spND->GetValue( _bstr_t((LPCSTR)strKey), &var );
	if( !(var.vt == VT_R8 || var.vt == VT_BSTR || var.vt == VT_DATE ) )
		return false;

	var.ChangeType( VT_BSTR );
	strValue = var.bstrVal;

	return true;
}

POSITION CDBaseGridCtrl::GetFisrtVisibleObjectPosition()
{
	if( GetActiveQuery() )
		return (POSITION)1;
	return (POSITION)0;
}

IUnknown * CDBaseGridCtrl::GetNextVisibleObject( POSITION &rPos )
{
	if( (long)rPos == 1 )
	{
		IUnknown	*p = GetActiveQuery();
		if( p == NULL )
		{
			rPos = 0;
			return 0;
		}

		p->AddRef();

		rPos = 0;

		return p;
	}
	return 0;
}

void CDBaseGridCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	ForceEndEditing();
	{
		CPoint _pt = point;
		ScreenToClient( &_pt );
		CCellID cell  = GetCellFromPt( _pt );
		if( IsValid( cell ) )
		{
			GotoRecordSlow( cell.row );
			ForceGoToRecord();
		}
	}

	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );

	if( !punk )
		return;

	sptrICommandManager2 sptrCM = punk;
	punk->Release();

	if( sptrCM == NULL )
		return;
	
	CMenuRegistrator rcm;
	_bstr_t	bstrMenuName = rcm.GetMenu( szDBGridMenu, 0 );
	sptrCM->ExecuteContextMenu2( bstrMenuName, point, 0 );

}

void CDBaseGridCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	CGridCtrl::OnLButtonDown(nFlags, point);
}

class CPrimaryKeyCell : public CGridCell
{
public:
	virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE)
	{
		m_strText.Format( "%d", nRow ); 
		SetTextClr( CLR_DEFAULT );
		return __super::Draw( pDC, nRow, nCol, rect, bEraseBkgnd );
	}
};


CGridCellBase* CDBaseGridCtrl::CreateCell(int nRow, int nCol)
{
	if( nRow > 0 && nCol == 0 )
	{
		CPrimaryKeyCell* pcell = new CPrimaryKeyCell;		
		pcell->SetState( GVIS_FIXED );
		pcell->SetGrid( this );
		return pcell;
	}
	return __super::CreateCell( nRow,  nCol );

}

void CDBaseGridCtrl::ForceEndEditing()
{
	EndEditing();
}