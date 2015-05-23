// Circle1PropPage.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "Circle1PropPage.h"
#include "PropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



IMPLEMENT_DYNCREATE(CCircle1PropPage, CDialog)

// {AAD1BA62-A278-11d3-A53A-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CCircle1PropPage, szEditorCirclePropPageProgID, 
0xaad1ba62, 0xa278, 0x11d3, 0xa5, 0x3a, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);

/////////////////////////////////////////////////////////////////////////////
// CCircle1PropPage dialog


CCircle1PropPage::CCircle1PropPage(UINT nIDTemplate) : CEditorPropBase(nIDTemplate)
{
	m_nCurButtonType = 0;
}

CCircle1PropPage::CCircle1PropPage(CWnd* pParent /*=NULL*/)
	: CEditorPropBase(CCircle1PropPage::IDD)
{
	//{{AFX_DATA_INIT(CCircle1PropPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nCurButtonType = 0;

	m_strFigure = "Circle";
	m_sName = c_szCCircle1PropPage;
	m_sUserName.LoadString( IDS_PROPPAGE_CIRCLE );
}


void CCircle1PropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCircle1PropPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_VTWIDTHCTRL1, m_pbWidth[0]);
	DDX_Control(pDX, IDC_VTWIDTHCTRL2, m_pbWidth[1]);
	DDX_Control(pDX, IDC_VTWIDTHCTRL3, m_pbWidth[2]);
	DDX_Control(pDX, IDC_VTWIDTHCTRL4, m_pbWidth[3]);
	DDX_Control(pDX, IDC_PUSHBUTTONCTRL_TYPE1, m_pbType[0]);
	DDX_Control(pDX, IDC_PUSHBUTTONCTRL_TYPE2, m_pbType[1]);
	DDX_Control(pDX, IDC_PUSHBUTTONCTRL_TYPE3, m_pbType[2]);
	DDX_Control(pDX, IDC_VTCOLORPICKERCTRL1, m_clrpickFore);
	DDX_Control(pDX, IDC_VTCOLORPICKERCTRL2, m_clrpickBack);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCircle1PropPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CCircle1PropPage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCircle1PropPage message handlers

BEGIN_EVENTSINK_MAP(CCircle1PropPage, CDialog)
    //{{AFX_EVENTSINK_MAP(CCircle1PropPage)
	ON_EVENT(CCircle1PropPage, IDC_VTWIDTHCTRL2, 1 /* OnChange */, OnOnChangeVtwidthctrl2, VTS_NONE)
	ON_EVENT(CCircle1PropPage, IDC_VTWIDTHCTRL2, -600 /* OnClick */, OnOnClickVtwidthctrl2, VTS_NONE)
	ON_EVENT(CCircle1PropPage, IDC_VTWIDTHCTRL3, 1 /* OnChange */, OnOnChangeVtwidthctrl3, VTS_NONE)
	ON_EVENT(CCircle1PropPage, IDC_VTWIDTHCTRL3, -600 /* OnClick */, OnOnClickVtwidthctrl3, VTS_NONE)
	ON_EVENT(CCircle1PropPage, IDC_VTWIDTHCTRL4, 1 /* OnChange */, OnOnChangeVtwidthctrl4, VTS_NONE)
	ON_EVENT(CCircle1PropPage, IDC_VTWIDTHCTRL4, -600 /* OnClick */, OnOnClickVtwidthctrl4, VTS_NONE)
	ON_EVENT(CCircle1PropPage, IDC_VTWIDTHCTRL1, 1 /* OnChange */, OnOnChangeVtwidthctrl1, VTS_NONE)
	ON_EVENT(CCircle1PropPage, IDC_VTWIDTHCTRL1, -600 /* OnClick */, OnOnClickVtwidthctrl1, VTS_NONE)
	ON_EVENT(CCircle1PropPage, IDC_PUSHBUTTONCTRL_TYPE1, -600 /* Click */, OnClickPushbuttonctrlType1, VTS_NONE)
	ON_EVENT(CCircle1PropPage, IDC_PUSHBUTTONCTRL_TYPE2, -600 /* Click */, OnClickPushbuttonctrlType2, VTS_NONE)
	ON_EVENT(CCircle1PropPage, IDC_PUSHBUTTONCTRL_TYPE3, -600 /* Click */, OnClickPushbuttonctrlType3, VTS_NONE)
	ON_EVENT(CCircle1PropPage, IDC_VTCOLORPICKERCTRL1, 1 /* OnChange */, OnOnChangeVtcolorpickerctrl1, VTS_NONE)
	ON_EVENT(CCircle1PropPage, IDC_VTCOLORPICKERCTRL2, 1 /* OnChange */, OnOnChangeVtcolorpickerctrl2, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CCircle1PropPage::OnDestroy() 
{
	_DeInit();
	CDialog::OnDestroy();
}

BOOL CCircle1PropPage::OnInitDialog() 
{
	MakeButtons(4, 2);

	CDialog::OnInitDialog();
	
	_Init(m_strFigure, true);

	bool bNeedToFill = ::GetValueInt(GetAppUnknown(), "Editor", m_strFigure+"NeedToFill", 0) == 1;
	bool bFillByDrawColor = ::GetValueInt(GetAppUnknown(), "Editor", m_strFigure+"FillByDrawColor", 0) == 1;

	if(bNeedToFill)
	{
		if(bFillByDrawColor)
		{
			m_nCurButtonType = 1;
			m_pbType[1].SetPressedState();
		}
		else
		{
			m_nCurButtonType = 2;
			m_pbType[2].SetPressedState();
		}
	}
	else
	{
		m_nCurButtonType = 0;
		m_pbType[0].SetPressedState();

	}
	InitColors();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CCircle1PropPage::InitColors()
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

void CCircle1PropPage::OnOnChangeVtwidthctrl2() 
{
	// TODO: Add your control notification handler code here
	OnWidth(1);	
}

void CCircle1PropPage::OnOnClickVtwidthctrl2() 
{
	// TODO: Add your control notification handler code here
	m_nActiveWidth = 1;
	_ManageWidthButtons();	
}

void CCircle1PropPage::OnOnChangeVtwidthctrl3() 
{
	// TODO: Add your control notification handler code here
	OnWidth(2);	
}

void CCircle1PropPage::OnOnClickVtwidthctrl3() 
{
	// TODO: Add your control notification handler code here
	m_nActiveWidth = 2;
	_ManageWidthButtons();
}

void CCircle1PropPage::OnOnChangeVtwidthctrl4() 
{
	// TODO: Add your control notification handler code here
	OnWidth(3);	
}

void CCircle1PropPage::OnOnClickVtwidthctrl4() 
{
	// TODO: Add your control notification handler code here
	m_nActiveWidth = 3;
	_ManageWidthButtons();
}

void CCircle1PropPage::OnOnChangeVtwidthctrl1() 
{
	// TODO: Add your control notification handler code here
	OnWidth(0);		
}

void CCircle1PropPage::OnOnClickVtwidthctrl1() 
{
	// TODO: Add your control notification handler code here
	m_nActiveWidth = 0;
	_ManageWidthButtons();	
}

void CCircle1PropPage::_SetTypeValue(int nButton)
{
	bool bNeedToFill = false;
	bool bFillByDrawColor = false;
	switch(nButton)
	{
	case 1:
		bNeedToFill = true;
		bFillByDrawColor = true;
		break;
	case 2:
		bNeedToFill = true;
		break;
	}
		
	::SetValue(GetAppUnknown(), "Editor", m_strFigure+"FillByDrawColor", _variant_t((long)(bFillByDrawColor?1:0)));
	::SetValue(GetAppUnknown(), "Editor", m_strFigure+"NeedToFill", _variant_t((long)(bNeedToFill?1:0)));
}

void CCircle1PropPage::_ManageTypeButtonState(int nButton)
{
	if(nButton != m_nCurButtonType) 
	{
		m_pbType[m_nCurButtonType].ResetPressedState();
		m_nCurButtonType = nButton;
	}
	m_pbType[m_nCurButtonType].SetPressedState();
	_SetTypeValue(m_nCurButtonType);

}

void CCircle1PropPage::OnClickPushbuttonctrlType1() 
{
	// TODO: Add your control notification handler code here
	_ManageTypeButtonState(0);
}

void CCircle1PropPage::OnClickPushbuttonctrlType2() 
{
	// TODO: Add your control notification handler code here
	_ManageTypeButtonState(1);
}

void CCircle1PropPage::OnClickPushbuttonctrlType3() 
{
	// TODO: Add your control notification handler code here
	_ManageTypeButtonState(2);
}

void CCircle1PropPage::OnOnChangeVtcolorpickerctrl1() 
{
	// TODO: Add your control notification handler code here
	SetColor(m_clrpickFore.GetColor(), true);
}

void CCircle1PropPage::OnOnChangeVtcolorpickerctrl2() 
{
	// TODO: Add your control notification handler code here
	SetColor(m_clrpickBack.GetColor(), false);
}

void CCircle1PropPage::DoLoadSettings()
{
	if(!m_hWnd)
		return;

	InitColors();
}

