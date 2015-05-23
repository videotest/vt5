// ChoiceActiveQuery.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "ChoiceActiveQuery.h"
#include "constant.h"


/////////////////////////////////////////////////////////////////////////////
// CChoiceActiveQuery dialog


CChoiceActiveQuery::CChoiceActiveQuery(CWnd* pParent /*=NULL*/)
	: CDialog(CChoiceActiveQuery::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChoiceActiveQuery)
	m_strActiveQuery = _T("");
	//}}AFX_DATA_INIT
}


void CChoiceActiveQuery::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChoiceActiveQuery)
	DDX_LBString(pDX, IDC_QUERY_LIST, m_strActiveQuery);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChoiceActiveQuery, CDialog)
	//{{AFX_MSG_MAP(CChoiceActiveQuery)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CChoiceActiveQuery::SetDBaseDocument( IUnknown* pUnkDoc )
{
	m_spDBaseDoc = pUnkDoc;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CChoiceActiveQuery::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_QUERY_LIST);
	if( pListBox == NULL || !::IsWindow(pListBox->GetSafeHwnd() ) )
		return TRUE;

	if( m_spDBaseDoc == NULL )
		return TRUE;

	IUnknown* punkQ = 0;
	
	CString strActiveQuery;
	m_spDBaseDoc->GetActiveQuery( &punkQ );

	if( punkQ )
	{
		strActiveQuery = ::GetObjectName( punkQ );
		punkQ->Release();	punkQ = 0;
	}

	IDataContext2Ptr ptrDC( m_spDBaseDoc );
	if( ptrDC == 0 )
	{
		ASSERT( false );
		return FALSE;
	}

	long lPos = 0;
	ptrDC->GetFirstObjectPos( _bstr_t(szTypeQueryObject), &lPos );
	while( lPos )
	{
		IUnknown* punk = 0;
		ptrDC->GetNextObject( _bstr_t(szTypeQueryObject), &lPos, &punk );
		if( punk )
		{
			CString strName = ::GetObjectName( punk );
			pListBox->AddString( strName );
			punk->Release();	punk = 0;
		}
	}


	pListBox->SelectString( 0, strActiveQuery );

	return TRUE;
}
