// Line1PropPage.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "FloodFillPropPage.h"
#include "PropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CFloodFillPropPage, CDialog)

// {4D426962-EB05-446b-B410-A89712DE4AEF}
GUARD_IMPLEMENT_OLECREATE(CFloodFillPropPage, szEditorFloodFillPropPageProgID, 
0x4d426962, 0xeb05, 0x446b, 0xb4, 0x10, 0xa8, 0x97, 0x12, 0xde, 0x4a, 0xef);


/////////////////////////////////////////////////////////////////////////////
// CFloodFillPropPage dialog


CFloodFillPropPage::CFloodFillPropPage(CWnd* pParent /*=NULL*/)
	: CEditorPropBase(CFloodFillPropPage::IDD)
{
	//{{AFX_DATA_INIT(CLine1PropPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sName = c_szCFloodFillPropPage;
	m_sUserName.LoadString( IDS_PROPPAGE_FLOODFILL );
}


BEGIN_INTERFACE_MAP(CFloodFillPropPage, CPropertyPageBase)	
	INTERFACE_PART(CFloodFillPropPage, IID_IColorPickerPropPage, FloodFillPropPage)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CFloodFillPropPage, FloodFillPropPage);


HRESULT CFloodFillPropPage::XFloodFillPropPage::ChangeColor( DWORD dwRGB, color H, color S, color L )
{
	_try_nested(CFloodFillPropPage, FloodFillPropPage, ChangeColor)
	{	
		if( !pThis->GetSafeHwnd() )
			return S_FALSE;
		pThis->m_wndColor.SetColor( dwRGB );
		return S_OK;
	}
	_catch_nested;
}



void CFloodFillPropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLine1PropPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
		DDX_Control(pDX, IDC_VTCOLORPICKERCTRL1, m_clrpickFore);
		DDX_Control(pDX, IDC_VTCOLORPICKERCTRL2, m_clrpickBack);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFloodFillPropPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CFloodFillPropPage)
		// NOTE: the ClassWizard will add message map macros here
		ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLine1PropPage message handlers
BEGIN_EVENTSINK_MAP(CFloodFillPropPage, CDialog)
    //{{AFX_EVENTSINK_MAP(CLine1PropPage)
	ON_EVENT(CFloodFillPropPage, IDC_VTCOLORPICKERCTRL1, 1 /* OnChange */, OnOnChangeVtcolorpickerctrl1, VTS_NONE)
	ON_EVENT(CFloodFillPropPage, IDC_VTCOLORPICKERCTRL2, 1 /* OnChange */, OnOnChangeVtcolorpickerctrl2, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

BOOL CFloodFillPropPage::OnInitDialog() 
{
	CDialog::OnInitDialog();
//	_Init("FloodFill", false);
	InitColors();
	CWnd* pWnd = GetDlgItem( IDC_COLOR_WND );
	if( pWnd )
	{
		CRect rc;
		pWnd->GetWindowRect( &rc );
		ScreenToClient( &rc);
		m_wndColor.Create( NULL, NULL, WS_CHILD | WS_VISIBLE, rc, this, 10023, NULL );
	}
	return TRUE;  // return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}

void CFloodFillPropPage::InitColors()
{
	COLORREF Palette[256];
	int nColorMode = GetColorMode(Palette);
	m_bColor = nColorMode == 1;
	m_bPalette = nColorMode == 2;
	if(m_bColor)
	{
		m_clrpickFore.SetBlackAndWhite(FALSE);
		m_clrpickBack.SetBlackAndWhite(FALSE);
		m_clrpickFore.SetColor(::GetValueColor(GetAppUnknown(), "Editor", "Fore", RGB(255,255,255)));
		m_clrpickBack.SetColor(::GetValueColor(GetAppUnknown(), "Editor", "Back", RGB(0,0,0)));
	}
	else
	{
		if (m_bPalette)
		{
			for (short i = 0; i < 256; i++)
			{
				m_clrpickFore.SetPaletteEntry(i, Palette[i]);
				m_clrpickBack.SetPaletteEntry(i, Palette[i]);
			}
		}
		m_clrpickFore.SetBlackAndWhite(TRUE);
		m_clrpickBack.SetBlackAndWhite(TRUE);
		m_clrpickFore.SetColor(::GetValueColor(GetAppUnknown(), "Editor", "Fore gray", RGB(255,255,255)));
		m_clrpickBack.SetColor(::GetValueColor(GetAppUnknown(), "Editor", "Back gray", RGB(0,0,0)));
	}
}

void CFloodFillPropPage::OnDestroy() 
{
//	_DeInit();

	CDialog::OnDestroy();
}

void CFloodFillPropPage::OnOnChangeVtcolorpickerctrl1() 
{
	SetColor(m_clrpickFore.GetColor(), true);
}

void CFloodFillPropPage::OnOnChangeVtcolorpickerctrl2() 
{
  SetColor(m_clrpickBack.GetColor(), false);
}

void CFloodFillPropPage::DoLoadSettings()
{
	if(!m_hWnd)
		return;
	InitColors();
}

