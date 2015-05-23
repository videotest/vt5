// DBaseInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "DBaseInfoDlg.h"
#include "UserInfo.h"


/////////////////////////////////////////////////////////////////////////////
// CDBaseInfoDlg dialog


CDBaseInfoDlg::CDBaseInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDBaseInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDBaseInfoDlg)
	//}}AFX_DATA_INIT
}


void CDBaseInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDBaseInfoDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDBaseInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CDBaseInfoDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_COMMAND(IDC_USER_INFO,OnUserInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBaseInfoDlg message handlers
/////////////////////////////////////////////////////////////////////////////
void CDBaseInfoDlg::SetDBaseDocument( IUnknown* pUnkDoc )
{
	m_spDBaseDoc = pUnkDoc;
}

void CDBaseInfoDlg::OnUserInfo()
{
	CUserInfoDlg dlg;
	dlg.DoModal();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDBaseInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString strPath, strFileSize;
	CString strQueryName, strQueryState, strRecordCount, strCurRecord;
	strQueryState = "Not available";
	strRecordCount = "0";
	strCurRecord = "0";
	
	if( m_spDBaseDoc != NULL )
	{
		BSTR bstr = 0;
		IDBConnectionPtr ptrDBC( m_spDBaseDoc );
		if( ptrDBC == 0 )
			return FALSE;

		ptrDBC->GetMDBFileName( &bstr );
		strPath = bstr;

		if( bstr )
			::SysFreeString( bstr );

		CFileFind ff;
		if( ff.FindFile( strPath ) )
		{
			ff.FindNextFile();
			DWORD dwLength = ff.GetLength();
			strFileSize.Format( "%d KB", dwLength / 1024 );
		}

		IUnknown* pUnkQuery = NULL;
		m_spDBaseDoc->GetActiveQuery( &pUnkQuery );
		if( pUnkQuery )
		{			
			sptrISelectQuery spQuery(pUnkQuery);
			pUnkQuery->Release();
			if( spQuery )
			{
				BOOL bOpen = FALSE;
				spQuery->IsOpen( &bOpen );
				if( bOpen )				
					strQueryState.LoadString( IDS_OPEN_STATUS );
				else
					strQueryState.LoadString( IDS_CLOSE_STATUS );

				if( bOpen )
				{
					long nRecordCount = 0;
					long nCurRecord = 0;
					spQuery->GetRecordCount( &nRecordCount );
					spQuery->GetCurrentRecord( &nCurRecord );

					strRecordCount.Format( "%d", nRecordCount );
					strCurRecord.Format( "%d", nCurRecord );
				}

				
				sptrIQueryObject spSelQ( spQuery );
				if( spSelQ )
				{
					BSTR bstrSQL = NULL;
					spSelQ->GetSQL( &bstrSQL );
					CString strSQL = bstrSQL;

					if( bstrSQL )
						::SysFreeString( bstrSQL );

					GetDlgItem(IDC_EDIT_SQL)->SetWindowText( strSQL );					
				}

				{
					INamedObject2Ptr ptrNO2( spQuery );
					if( ptrNO2 )
					{
						BSTR bstrName = 0;
						ptrNO2->GetName( &bstrName );
						strQueryName = bstrName;

						if( bstrName )
							::SysFreeString( bstrName );	bstrName = 0;
					}

				}
				


			}
		}		
	}		


	GetDlgItem(IDC_PATH)->SetWindowText( strPath );
	GetDlgItem(IDC_FILE_SIZE)->SetWindowText( strFileSize );
	GetDlgItem(IDC_QUERY_NAME)->SetWindowText( strQueryName );
	GetDlgItem(IDC_QUERY_STATE)->SetWindowText( strQueryState );
	GetDlgItem(IDC_QUERY_RECORD_COUNT)->SetWindowText( strRecordCount );
	GetDlgItem(IDC_QUERY_CURRENT_RECORD)->SetWindowText( strCurRecord );
	
	return TRUE;	            
}

