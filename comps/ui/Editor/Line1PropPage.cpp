// Line1PropPage.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "Line1PropPage.h"
#include "PropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CLine1PropPage, CDialog)

// {8291DE52-A1B4-11d3-A539-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CLine1PropPage, szEditorLinePropPageProgID, 
0x8291de52, 0xa1b4, 0x11d3, 0xa5, 0x39, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);

/////////////////////////////////////////////////////////////////////////////
// CLine1PropPage dialog


CLine1PropPage::CLine1PropPage(CWnd* pParent /*=NULL*/)
	: CEditorPropBase(CLine1PropPage::IDD)
{
	//{{AFX_DATA_INIT(CLine1PropPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sName = c_szCLine1PropPage;
	m_sUserName.LoadString( IDS_PROPPAGE_LINE );

  m_bBinaryMode = false;
}


void CLine1PropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLine1PropPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
		DDX_Control(pDX, IDC_VTWIDTHCTRL1, m_pbWidth[0]);
		DDX_Control(pDX, IDC_VTWIDTHCTRL2, m_pbWidth[1]);
		DDX_Control(pDX, IDC_VTWIDTHCTRL3, m_pbWidth[2]);
		DDX_Control(pDX, IDC_VTWIDTHCTRL4, m_pbWidth[3]);
		DDX_Control(pDX, IDC_VTWIDTHCTRL5, m_pbWidth[4]);
		DDX_Control(pDX, IDC_VTCOLORPICKERCTRL1, m_clrpickFore);
		DDX_Control(pDX, IDC_VTCOLORPICKERCTRL2, m_clrpickBack);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLine1PropPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CLine1PropPage)
		// NOTE: the ClassWizard will add message map macros here
		ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLine1PropPage message handlers
BEGIN_EVENTSINK_MAP(CLine1PropPage, CDialog)
    //{{AFX_EVENTSINK_MAP(CLine1PropPage)
	ON_EVENT(CLine1PropPage, IDC_VTWIDTHCTRL1, 1 /* OnChange */, OnChangeVtwidthctrl1, VTS_NONE)
	ON_EVENT(CLine1PropPage, IDC_VTWIDTHCTRL2, 1 /* OnChange */, OnChangeVtwidthctrl2, VTS_NONE)
	ON_EVENT(CLine1PropPage, IDC_VTWIDTHCTRL3, 1 /* OnChange */, OnChangeVtwidthctrl3, VTS_NONE)
	ON_EVENT(CLine1PropPage, IDC_VTWIDTHCTRL4, 1 /* OnChange */, OnChangeVtwidthctrl4, VTS_NONE)
	ON_EVENT(CLine1PropPage, IDC_VTWIDTHCTRL5, 1 /* OnChange */, OnChangeVtwidthctrl5, VTS_NONE)
	ON_EVENT(CLine1PropPage, IDC_VTWIDTHCTRL1, -600 /* OnClick */, OnOnClickVtwidthctrl1, VTS_NONE)
	ON_EVENT(CLine1PropPage, IDC_VTWIDTHCTRL2, -600 /* OnClick */, OnOnClickVtwidthctrl2, VTS_NONE)
	ON_EVENT(CLine1PropPage, IDC_VTWIDTHCTRL3, -600 /* OnClick */, OnOnClickVtwidthctrl3, VTS_NONE)
	ON_EVENT(CLine1PropPage, IDC_VTWIDTHCTRL4, -600 /* OnClick */, OnOnClickVtwidthctrl4, VTS_NONE)
	ON_EVENT(CLine1PropPage, IDC_VTWIDTHCTRL5, -600 /* OnClick */, OnOnClickVtwidthctrl5, VTS_NONE)
	ON_EVENT(CLine1PropPage, IDC_VTCOLORPICKERCTRL1, 1 /* OnChange */, OnOnChangeVtcolorpickerctrl1, VTS_NONE)
	ON_EVENT(CLine1PropPage, IDC_VTCOLORPICKERCTRL2, 1 /* OnChange */, OnOnChangeVtcolorpickerctrl2, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CLine1PropPage::OnChangeVtwidthctrl1() 
{
	// TODO: Add your control notification handler code here
	OnWidth(0);
}

void CLine1PropPage::OnChangeVtwidthctrl2() 
{
	// TODO: Add your control notification handler code here
	OnWidth(1);
}

void CLine1PropPage::OnChangeVtwidthctrl3() 
{
	// TODO: Add your control notification handler code here
	OnWidth(2);
}

void CLine1PropPage::OnChangeVtwidthctrl4() 
{
	// TODO: Add your control notification handler code here
	OnWidth(3);
}

void CLine1PropPage::OnChangeVtwidthctrl5() 
{
	// TODO: Add your control notification handler code here
	OnWidth(4);
}


void CLine1PropPage::OnOnClickVtwidthctrl1() 
{
	// TODO: Add your control notification handler code here
	m_nActiveWidth = 0;
	_ManageWidthButtons();
}

void CLine1PropPage::OnOnClickVtwidthctrl2() 
{
	// TODO: Add your control notification handler code here
	m_nActiveWidth = 1;
	_ManageWidthButtons();
}

void CLine1PropPage::OnOnClickVtwidthctrl3() 
{
	// TODO: Add your control notification handler code here
	m_nActiveWidth = 2;
	_ManageWidthButtons();
}

void CLine1PropPage::OnOnClickVtwidthctrl4() 
{
	// TODO: Add your control notification handler code here
	m_nActiveWidth = 3;
	_ManageWidthButtons();
}

void CLine1PropPage::OnOnClickVtwidthctrl5() 
{
	// TODO: Add your control notification handler code here
	m_nActiveWidth = 4;
	_ManageWidthButtons();
}



BOOL CLine1PropPage::OnInitDialog() 
{
	MakeButtons(5);

	CDialog::OnInitDialog();

	_Init("Line", false);


	m_bBinaryMode = ::GetValueInt(GetAppUnknown(), "Editor", "BinaryMode", 0) == 1;
	m_clrpickFore.SetBinary(m_bBinaryMode);
  
	if(m_bBinaryMode)
	{
		m_clrpickBack.ShowWindow(SW_HIDE);
		if (GetValueInt(GetAppUnknown(), "\\Binary", "Multyphase", 1))
		{
			DWORD dwDefColor = RGB(255,255,0);
			long nCounter = m_clrpickFore.GetBinaryColorsCount();
			//AAM: делаю так, чтобы здесь не пыталось уменьшить число цветов в shell.data
			long nCounter1 = GetValueInt(GetAppUnknown(), "\\Binary", "IndexedColorsCount", 1);
			nCounter=max(nCounter, nCounter1);
			SetValue(GetAppUnknown(), "\\Binary", "IndexedColorsCount", _variant_t(nCounter));
			for(long i = 0; i < nCounter; i++)
			{
				CString str;
				str.Format("Fore_%d", i);
				DWORD dwColor = GetValueColor(GetAppUnknown(), "\\Binary", str, dwDefColor);
				if(dwColor == dwDefColor && i > 0)
				SetValueColor(GetAppUnknown(), "\\Binary", str, m_clrpickFore.GetBinaryColor((short)i));
				else
				m_clrpickFore.SetBinaryColor((short)i, dwColor);
			}
			// vk begin
			m_clrpickFore.SetActiveBinaryIndex((short)max(0,::GetValueInt( GetAppUnknown(), "Binary", "FillColor", 1)-128));
			// vk end
		}
		else
		{
			m_clrpickFore.ShowWindow(SW_HIDE);
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_COLOR), SW_HIDE);
		}
	}
	else
		InitColors();
	return TRUE;  // return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}

void CLine1PropPage::InitColors()
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

void CLine1PropPage::OnDestroy() 
{
	_DeInit();

	CDialog::OnDestroy();
}

void CLine1PropPage::OnOnChangeVtcolorpickerctrl1() 
{
	// TODO: Add your control notification handler code here
	if(m_bBinaryMode)
	{
		// vk 
		// Будем заполнять цветами, начиная со 128
		::SetValue( GetAppUnknown(), "Binary", "FillColor", _variant_t((long)(m_clrpickFore.GetActiveBinaryIndex()+128)));
		//if ( m_clrpickFore.GetActiveBinaryIndex() < 128 )
		//	::SetValue( GetAppUnknown(), "Binary", "FillColor", _variant_t((long)(m_clrpickFore.GetActiveBinaryIndex()+1)));
		//else
		//	::SetValue( GetAppUnknown(), "Binary", "FillColor", _variant_t((long)(m_clrpickFore.GetActiveBinaryIndex()-1)));
		//long binInd = (long)(m_clrpickFore.GetActiveBinaryIndex()+1);
		//if ( binInd ) 
		//	binInd |= 128; 
		//::SetValue( GetAppUnknown(), "Binary", "FillColor", _variant_t(binInd));
		//::SetValue( GetAppUnknown(), "Binary", "FillColor", _variant_t((long)(m_clrpickFore.GetActiveBinaryIndex()+1)));
		//::SetValue( GetAppUnknown(), "Binary", "FillColor", _variant_t((long)(m_clrpickFore.GetActiveBinaryIndex()+128)));
		
		//::SetValue( GetAppUnknown(), "Binary", "FillColor", _variant_t((long)(m_clrpickFore.GetActiveBinaryIndex()+1)));
		// end vk

		/*CString str;
		long nIndex = m_clrpickFore.GetActiveBinaryIndex();
		str.Format("Fore_%d", nIndex);
		::SetValueColor(GetAppUnknown(), "\\Binary", str, m_clrpickFore.GetBinaryColor(nIndex));*/
	}
	else
		SetColor(m_clrpickFore.GetColor(), true);
}

void CLine1PropPage::OnOnChangeVtcolorpickerctrl2() 
{
	// TODO: Add your control notification handler code here
  SetColor(m_clrpickBack.GetColor(), false);
}

void CLine1PropPage::DoLoadSettings()
{
	if(!m_hWnd)
		return;

	m_bBinaryMode = ::GetValueInt(GetAppUnknown(), "Editor", "BinaryMode", 0) == 1;
	m_clrpickFore.SetBinary(m_bBinaryMode);
	m_clrpickBack.SetBinary(m_bBinaryMode);
	if(m_bBinaryMode)
	{
		// vk
		m_clrpickFore.SetActiveBinaryIndex((short)max(0,::GetValueInt( GetAppUnknown(), "Binary", "FillColor", 1)-128));
		//if ( ::GetValueInt( GetAppUnknown(), "Binary", "FillColor", 1) < 128 )
		//	m_clrpickFore.SetActiveBinaryIndex((short)max(0,::GetValueInt( GetAppUnknown(), "Binary", "FillColor", 1)-1));
		//else
		//	m_clrpickFore.SetActiveBinaryIndex((short)max(0,::GetValueInt( GetAppUnknown(), "Binary", "FillColor", 1)+1));
		//m_clrpickFore.SetActiveBinaryIndex((short)max(0,::GetValueInt( GetAppUnknown(), "Binary", "FillColor", 1)-128));

		//m_clrpickFore.SetActiveBinaryIndex((short)max(0,::GetValueInt( GetAppUnknown(), "Binary", "FillColor", 1)-1));
		// end vk
		if (GetValueInt(GetAppUnknown(), "\\Binary", "Multyphase", 1))
		{
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_COLOR), SW_SHOW);
			m_clrpickFore.ShowWindow(SW_SHOW);
		}
		else
		{
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_COLOR), SW_HIDE);
			m_clrpickFore.ShowWindow(SW_HIDE);
		}
		m_clrpickBack.ShowWindow(SW_HIDE);
		return;
	}

	::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_COLOR), SW_SHOW);
	m_clrpickFore.ShowWindow(SW_SHOW);
	m_clrpickBack.ShowWindow(SW_SHOW);
	InitColors();
}
