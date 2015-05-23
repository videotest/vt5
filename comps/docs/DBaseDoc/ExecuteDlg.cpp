// ExecuteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "ExecuteDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CExecuteDlg dialog


CExecuteDlg::CExecuteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExecuteDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExecuteDlg)
	m_strSQL = _T("");
	//}}AFX_DATA_INIT
}


void CExecuteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExecuteDlg)
	DDX_Control(pDX, IDC_RESULT, m_ctrlList);
	DDX_Control(pDX, IDC_BTN_WORK, m_ctrlWorkButton);
	DDX_Text(pDX, IDC_SQL, m_strSQL);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExecuteDlg, CDialog)
	//{{AFX_MSG_MAP(CExecuteDlg)
	ON_BN_CLICKED(IDC_BTN_WORK, OnBtnWork)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExecuteDlg message handlers
void CExecuteDlg::SetDBaseDocument( IUnknown* pUnkDoc )
{
	m_spDBaseDoc = pUnkDoc;
}

static bool bStateEnterSQL = true;

/////////////////////////////////////////////////////////////////////////////
void CExecuteDlg::OnBtnWork() 
{
	if( m_spDBaseDoc == NULL )
		return;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());


	IDBConnectionPtr ptrDBC( m_spDBaseDoc );
	if( ptrDBC == 0 )
		return;
	

	if( bStateEnterSQL )	
	{


		m_ctrlList.DeleteAllItems( );
		while( m_ctrlList.DeleteColumn(0) ){}

				
		CString strCommand;
		GetDlgItem(IDC_SQL)->GetWindowText( strCommand );
		IUnknown* pUnkRecordset = NULL;
		if( S_OK != ptrDBC->ExecuteSQL( _bstr_t( (LPCTSTR)strCommand ), &pUnkRecordset) )
		{
			return;
		}

		if( pUnkRecordset == NULL )
		{
			AfxMessageBox( IDS_WARNING_EXECUTE_SUCCESS );
			return;
		}

		ADO::_RecordsetPtr spRecordset = pUnkRecordset;
		pUnkRecordset->Release( );
		if( spRecordset == NULL )
			return;

		

		try{

			ADO::FieldsPtr spFields = spRecordset->Fields;
			if( spFields == NULL )
				return;

			int nFieldsCount = spFields->Count;
			for( long i=0;i<nFieldsCount;i++ )
			{
				ADO::FieldPtr spField = spFields->Item[_variant_t(i)];
				m_ctrlList.InsertColumn( i, spField->GetName(), LVCFMT_CENTER, 50 );
			}

			long nRecordNum = 0;


			if( VARIANT_FALSE == spRecordset->ADOEOF )
				spRecordset->MoveFirst();
			
			while( VARIANT_FALSE == spRecordset->ADOEOF )
			{  			
				
				ADO::FieldsPtr spFields = spRecordset->Fields;
				if( spFields )
				{
					
					for( long i=0;i<nFieldsCount;i++ )
					{						
						CString strValue;						

						ADO::FieldPtr spField = spFields->Item[_variant_t(i)];

						_variant_t var;
						if( S_OK != spField->get_Value( &var ) )
							continue;

						try
						{
							var.ChangeType( VT_BSTR );
							strValue = var.bstrVal;
						}
						catch(...)
						{
						}

						LVITEM lvitem;		
						ZeroMemory( &lvitem, sizeof(lvitem) );

						lvitem.mask = LVIF_TEXT;
						lvitem.iItem = nRecordNum;						

						//strcpy( lvitem.pszText, strValue );						

						if (i == 0)
						{
							lvitem.iSubItem = 0;
							m_ctrlList.InsertItem(&lvitem);
						}
						else
						{
							lvitem.iSubItem = i;
							m_ctrlList.SetItem(&lvitem);
						}

						m_ctrlList.SetItemText( lvitem.iItem, lvitem.iSubItem, strValue );

					}
				}
				nRecordNum++;
				spRecordset->MoveNext();
			}
		}
		catch (_com_error &e)
		{			
			dump_com_error(e);					
		}

		bStateEnterSQL = false;
		m_ctrlWorkButton.SetWindowText( "Enter" );
		m_ctrlList.ShowWindow( SW_SHOWNA );
		m_ctrlList.SetFocus();
		GetDlgItem(IDC_SQL)->ShowWindow( SW_HIDE );		
	}
	else
	{

		bStateEnterSQL = true;
		m_ctrlWorkButton.SetWindowText( "Execute" );
		m_ctrlList.ShowWindow( SW_HIDE );
		GetDlgItem(IDC_SQL)->ShowWindow( SW_SHOWNA );
		GetDlgItem(IDC_SQL)->SetFocus();

	}
	
}
