// LineBinPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "LineBinPropPage.h"
#include "PropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CLineBinPropPage, CDialog)

// {E1E7371B-FF65-468f-AF73-4A77791932C0}
GUARD_IMPLEMENT_OLECREATE(CLineBinPropPage, szEditorBinLinePropPageProgID, 
0xe1e7371b, 0xff65, 0x468f, 0xaf, 0x73, 0x4a, 0x77, 0x79, 0x19, 0x32, 0xc0);

/////////////////////////////////////////////////////////////////////////////
// CLineBinPropPage dialog


CLineBinPropPage::CLineBinPropPage(CWnd* pParent /*=NULL*/)
	: CEditorPropBase(CLineBinPropPage::IDD)
{
	//{{AFX_DATA_INIT(CLineBinPropPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sName = c_szCLineBinPropPage;
	m_sUserName.LoadString( IDS_PROPPAGE_LINE );
	m_nActiveWidth = 0;
	m_nActiveWidth2 = 0;
}


void CLineBinPropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLineBinPropPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
		DDX_Control(pDX, IDC_VTWIDTHCTRL1, m_LineWidth1);
		DDX_Control(pDX, IDC_VTWIDTHCTRL2, m_LineWidth2);
		DDX_Control(pDX, IDC_VTWIDTHCTRL3, m_EraseWidth1);
		DDX_Control(pDX, IDC_VTWIDTHCTRL4, m_EraseWidth2);
		DDX_Control(pDX, IDC_VTCOLORPICKERCTRL1, m_clrpickFore);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLineBinPropPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CLineBinPropPage)
		// NOTE: the ClassWizard will add message map macros here
		ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLine1PropPage message handlers
BEGIN_EVENTSINK_MAP(CLineBinPropPage, CDialog)
    //{{AFX_EVENTSINK_MAP(CLineBinPropPage)
	ON_EVENT(CLineBinPropPage, IDC_VTWIDTHCTRL1, 1 /* OnChange */, OnChangeVtwidthctrl1, VTS_NONE)
	ON_EVENT(CLineBinPropPage, IDC_VTWIDTHCTRL2, 1 /* OnChange */, OnChangeVtwidthctrl2, VTS_NONE)
	ON_EVENT(CLineBinPropPage, IDC_VTWIDTHCTRL3, 1 /* OnChange */, OnChangeVtwidthctrl3, VTS_NONE)
	ON_EVENT(CLineBinPropPage, IDC_VTWIDTHCTRL4, 1 /* OnChange */, OnChangeVtwidthctrl4, VTS_NONE)
	ON_EVENT(CLineBinPropPage, IDC_VTWIDTHCTRL1, -600 /* OnClick */, OnOnClickVtwidthctrl1, VTS_NONE)
	ON_EVENT(CLineBinPropPage, IDC_VTWIDTHCTRL2, -600 /* OnClick */, OnOnClickVtwidthctrl2, VTS_NONE)
	ON_EVENT(CLineBinPropPage, IDC_VTWIDTHCTRL3, -600 /* OnClick */, OnOnClickVtwidthctrl3, VTS_NONE)
	ON_EVENT(CLineBinPropPage, IDC_VTWIDTHCTRL4, -600 /* OnClick */, OnOnClickVtwidthctrl4, VTS_NONE)
	ON_EVENT(CLineBinPropPage, IDC_VTCOLORPICKERCTRL1, 1 /* OnChange */, OnOnChangeVtcolorpickerctrl1, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CLineBinPropPage::ChangeLineWidth(int nWidth, const char *pszEntryName)
{
	::SetValue(GetAppUnknown(), "Editor", pszEntryName, _variant_t(nWidth));
	UpdateAction();
}

void CLineBinPropPage::OnChangeVtwidthctrl1() 
{
	if (m_nActiveWidth == 0) ChangeLineWidth(m_LineWidth1.GetWidth(), "LineWidth");
}

void CLineBinPropPage::OnChangeVtwidthctrl2() 
{
	if (m_nActiveWidth == 1) ChangeLineWidth(m_LineWidth2.GetWidth(), "LineWidth");
}

void CLineBinPropPage::OnChangeVtwidthctrl3() 
{
	if (m_nActiveWidth2 == 0) ChangeLineWidth(m_EraseWidth1.GetWidth(), "LineWidth2");
}

void CLineBinPropPage::OnChangeVtwidthctrl4() 
{
	if (m_nActiveWidth2 == 1) ChangeLineWidth(m_EraseWidth2.GetWidth(), "LineWidth2");
}

void CLineBinPropPage::OnOnClickVtwidthctrl1() 
{
	m_nActiveWidth = 0;
	m_LineWidth1.SetChecked(TRUE);
	m_LineWidth2.SetChecked(FALSE);
	::SetValue(GetAppUnknown(), "Editor_pages", "LineBinWidthActiveBtn", 0L);
	ChangeLineWidth(m_LineWidth1.GetWidth(), "LineWidth");
	::SetValue(GetAppUnknown(), "Editor_pages", "LineBinWidth0", _variant_t(m_LineWidth2.GetWidth()));
}

void CLineBinPropPage::OnOnClickVtwidthctrl2() 
{
	m_nActiveWidth = 1;
	m_LineWidth1.SetChecked(FALSE);
	m_LineWidth2.SetChecked(TRUE);
	::SetValue(GetAppUnknown(), "Editor_pages", "LineBinWidthActiveBtn", 1L);
	ChangeLineWidth(m_LineWidth2.GetWidth(), "LineWidth");
	::SetValue(GetAppUnknown(), "Editor_pages", "LineBinWidth0", _variant_t(m_LineWidth1.GetWidth()));
}

void CLineBinPropPage::OnOnClickVtwidthctrl3() 
{
	m_nActiveWidth2 = 0;
	m_EraseWidth1.SetChecked(TRUE);
	m_EraseWidth2.SetChecked(FALSE);
	::SetValue(GetAppUnknown(), "Editor_pages", "LineBinWidthActiveBtn2", 0L);
	ChangeLineWidth(m_EraseWidth1.GetWidth(), "LineWidth2");
	::SetValue(GetAppUnknown(), "Editor_pages", "LineBinWidth1", _variant_t(m_EraseWidth2.GetWidth()));
}

void CLineBinPropPage::OnOnClickVtwidthctrl4() 
{
	m_nActiveWidth2 = 1;
	m_EraseWidth1.SetChecked(FALSE);
	m_EraseWidth2.SetChecked(TRUE);
	::SetValue(GetAppUnknown(), "Editor_pages", "LineBinWidthActiveBtn2", 1L);
	ChangeLineWidth(m_EraseWidth2.GetWidth(), "LineWidth2");
	::SetValue(GetAppUnknown(), "Editor_pages", "LineBinWidth1", _variant_t(m_EraseWidth1.GetWidth()));
}

BOOL CLineBinPropPage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	InitButtons();
	m_clrpickFore.SetBinary(true);
  
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
		m_clrpickFore.SetActiveBinaryIndex((short)max(0,::GetValueInt( GetAppUnknown(), "Binary", "FillColor", 1)-128));
	}
	else
	{
		m_clrpickFore.ShowWindow(SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_COLOR), SW_HIDE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}

/*void CLineBinPropPage::InitColors()
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
}*/

void CLineBinPropPage::OnDestroy() 
{
	CDialog::OnDestroy();
}

void CLineBinPropPage::OnOnChangeVtcolorpickerctrl1() 
{
	::SetValue( GetAppUnknown(), "Binary", "FillColor", _variant_t((long)(m_clrpickFore.GetActiveBinaryIndex()+128)));
}

void CLineBinPropPage::DoLoadSettings()
{
	if(!m_hWnd)
		return;

	m_clrpickFore.SetBinary(true);
	m_clrpickFore.SetActiveBinaryIndex((short)max(0,::GetValueInt( GetAppUnknown(), "Binary", "FillColor", 1)-128));
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
}

void CLineBinPropPage::InitButtons()
{
	int nLineWidth = ::GetValueInt(GetAppUnknown(), "Editor", "LineWidth", 1);
	int nLineWidth2 = ::GetValueInt(GetAppUnknown(), "Editor", "LineWidth2", 1);
	m_nActiveWidth = ::GetValueInt(GetAppUnknown(), "Editor_pages", "LineBinWidthActiveBtn", 0);
	m_nActiveWidth = min(max(m_nActiveWidth,0),1);
	int nWidth1 = ::GetValueInt(GetAppUnknown(), "Editor_pages", "LineBinWidth0", 1);
	int nWidth2 = ::GetValueInt(GetAppUnknown(), "Editor_pages", "LineBinWidth1", 4);
	if (m_nActiveWidth== 0)
		m_LineWidth1.SetChecked(TRUE);
	else
		m_LineWidth2.SetChecked(TRUE);
	m_LineWidth1.SetWidth(m_nActiveWidth==0?nLineWidth:nWidth1);
	m_LineWidth2.SetWidth(m_nActiveWidth!=0?nLineWidth:nWidth1);
	m_nActiveWidth2 = ::GetValueInt(GetAppUnknown(), "Editor_pages", "LineBinWidthActiveBtn2", 0);
	m_nActiveWidth2 = min(max(m_nActiveWidth2,0),1);
	if (m_nActiveWidth2 == 0)
		m_EraseWidth1.SetChecked(TRUE);
	else
		m_EraseWidth2.SetChecked(TRUE);
	m_EraseWidth1.SetWidth(m_nActiveWidth2==0?nLineWidth2:nWidth2);
	m_EraseWidth2.SetWidth(m_nActiveWidth2!=0?nLineWidth2:nWidth2);
}

void CLineBinPropPage::SaveButtons()
{
}
