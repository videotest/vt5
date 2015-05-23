// VTPushButtonPpg.cpp : Implementation of the CVTKeyButtonPropPage property page class.

#include "stdafx.h"
#include "VTControls.h"
#include "VTKeyButtonPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTKeyButtonPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTKeyButtonPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTKeyButtonPropPage)
	ON_BN_CLICKED(IDC_BTN_PICK, OnBtnPick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTKeyButtonPropPage, "VTCONTROLS.VTKeyButtonPropPage.1",
	0xa8ad8a85, 0x7567, 0x4b4b, 0xaf, 0xce, 0x74, 0x59, 0xe4, 0x22, 0xb9, 0x18)


/////////////////////////////////////////////////////////////////////////////
// CVTKeyButtonPropPage::CVTKeyButtonPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTKeyButtonPropPage

BOOL CVTKeyButtonPropPage::CVTKeyButtonPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTKEYBUTTON_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTKeyButtonPropPage::CVTKeyButtonPropPage - Constructor

CVTKeyButtonPropPage::CVTKeyButtonPropPage() :
	COlePropertyPage(IDD, IDS_VTKEYBUTTON_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CVTKeyButtonPropPage)
	m_lVirtKey = 0;
	m_nAlignHorz = -1;
	m_nAlignVert = -1;
	m_ItemDeltaX = 0;
	m_ItemDeltaY = 0;
	//}}AFX_DATA_INIT
	m_bProcessPick = false;
}


/////////////////////////////////////////////////////////////////////////////
// CVTKeyButtonPropPage::DoDataExchange - Moves data between page and properties

void CVTKeyButtonPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CVTKeyButtonPropPage)
	DDP_Text(pDX, IDC_VIRT_KEY, m_lVirtKey, _T("VirtKey") );
	DDX_Text(pDX, IDC_VIRT_KEY, m_lVirtKey);
	DDP_CBIndex(pDX, IDC_ALIGN_HORZ, m_nAlignHorz, _T("ImageAlignHorz") );
	DDX_CBIndex(pDX, IDC_ALIGN_HORZ, m_nAlignHorz);
	DDP_CBIndex(pDX, IDC_ALIGN_VERT, m_nAlignVert, _T("ImageAlignVert") );
	DDX_CBIndex(pDX, IDC_ALIGN_VERT, m_nAlignVert);
	DDP_Text(pDX, IDC_ITEM_DELTA_X, m_ItemDeltaX, _T("ImageDeltaX") );
	DDX_Text(pDX, IDC_ITEM_DELTA_X, m_ItemDeltaX);
	DDP_Text(pDX, IDC_ITEM_DELTA_Y, m_ItemDeltaY, _T("ImageDeltaY") );
	DDX_Text(pDX, IDC_ITEM_DELTA_Y, m_ItemDeltaY);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CVTKeyButtonPropPage message handlers
void CVTKeyButtonPropPage::OnBtnPick() 
{	
	m_bProcessPick = !m_bProcessPick;

	CString strText;

	if( m_bProcessPick )
		strText.LoadString( IDS_KEY_PICK_PROCESS );
	else
		strText.LoadString( IDS_WAITING );

	GetDlgItem(IDC_STATE)->SetWindowText( strText );

}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTKeyButtonPropPage::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();	
	
	CString str;
	str.LoadString( IDS_WAITING );
	GetDlgItem(IDC_STATE)->SetWindowText( str );	

	return TRUE;  	              
}

/////////////////////////////////////////////////////////////////////////////
long CVTKeyButtonPropPage::GetCurrentPressedKey()
{
	for( long nKey=0x01;nKey<0xFF;nKey++ )
	{
		if( (::GetAsyncKeyState( nKey ) & 0x8000) == 0x8000 )
			return nKey;
	}
			
	return -1;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CVTKeyButtonPropPage::PreTranslateMessage(MSG* pMsg)
{
	
	//if( IsDialogMessage( pMsg ) )
	if( pMsg->message == WM_KEYDOWN )
	{		
		if( m_bProcessPick )
		{
			long nVirtKey = GetCurrentPressedKey();
			if( nVirtKey != -1 )
			{
				m_bProcessPick = !m_bProcessPick;

				CString strFormat;
				strFormat.LoadString( IDS_KEY_MESSAGE );
				CString strText;
				strText.Format( strFormat, nVirtKey );				
				GetDlgItem(IDC_STATE)->SetWindowText( strText );

				CString strCode;
				strCode.Format( "%d", nVirtKey );

				GetDlgItem(IDC_VIRT_KEY)->SetWindowText( strCode );

				return TRUE;
			}			
		}
	}
	
	
	return COlePropertyPage::PreTranslateMessage( pMsg );
}
