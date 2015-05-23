// ThumbnailDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ThumbMan.h"
#include "ThumbnailDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CThumbnailDlg dialog


CThumbnailDlg::CThumbnailDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CThumbnailDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CThumbnailDlg)
	m_nFlags = 0;
	m_nHeight = 100;
	m_lParam = 0;
	m_nWidth = 100;
	m_nX = 100;
	m_nY = 100;
	m_nGenerateWidth = 100;
	m_nGenerateHeight = 100;
	m_strObjectName = _T("");
	//}}AFX_DATA_INIT
}


void CThumbnailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CThumbnailDlg)
	DDX_Control(pDX, IDC_OBJECT_NAME, m_ctrlObjectName);
	DDX_Text(pDX, IDC_FLAGS, m_nFlags);
	DDX_Text(pDX, IDC_HEIGHT, m_nHeight);
	DDX_Text(pDX, IDC_LPARAM, m_lParam);
	DDX_Text(pDX, IDC_WIDTH, m_nWidth);
	DDX_Text(pDX, IDC_X, m_nX);
	DDX_Text(pDX, IDC_Y, m_nY);
	DDX_Text(pDX, IDC_GENERATE_WIDTH, m_nGenerateWidth);
	DDX_Text(pDX, IDC_GENERATE_HEIGHT, m_nGenerateHeight);
	DDX_CBString(pDX, IDC_OBJECT_NAME, m_strObjectName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CThumbnailDlg, CDialog)
	//{{AFX_MSG_MAP(CThumbnailDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CThumbnailDlg::SetNamedData( IUnknown* punkND )
{
	m_spNamedData = punkND;
}


/////////////////////////////////////////////////////////////////////////////
// CThumbnailDlg message handlers


BOOL CThumbnailDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if( m_spNamedData == NULL )
		return TRUE;


	sptrIDataTypeManager	sptrM( m_spNamedData );



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
					
					m_ctrlObjectName.AddString( CString(bstrObjectName));
				
					::SysFreeString( bstrObjectName );					
				}
			}			
			
			punkParent->Release();
		}
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
