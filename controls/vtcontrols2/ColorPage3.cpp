// ColorPage3.cpp : implementation file
//

#include "stdafx.h"
#include "vtcontrols2.h"
#include "ColorPage3.h"
#include "BCGColorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorPage3 property page

IMPLEMENT_DYNCREATE(CColorPage3, CPropertyPage)

CColorPage3::CColorPage3() : CPropertyPage(CColorPage3::IDD)
{
	//{{AFX_DATA_INIT(CColorPage3)
	m_L = 0;
	m_B = 0;
	m_G = 0;
	m_R = 0;
	//}}AFX_DATA_INIT
	m_IsReady=false;
}

CColorPage3::~CColorPage3()
{
}

void CColorPage3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorPage3)
	DDX_Control(pDX, IDC_BCGBARRES_GREYLUMPLACEHOLDER, m_luminance);
	DDX_Control(pDX, IDC_BCGBARRES_GREYHEXPLACEHOLDER, m_hexgreyscale);
	DDX_Text(pDX, IDC_EDIT_LUMINANCE, m_L);
	DDV_MinMaxUInt(pDX, m_L, 0, 255);
	DDX_Text(pDX, IDC_BAW_B, m_B);
	DDV_MinMaxUInt(pDX, m_B, 0, 255);
	DDX_Text(pDX, IDC_BAW_G, m_G);
	DDV_MinMaxUInt(pDX, m_G, 0, 255);
	DDX_Text(pDX, IDC_BAW_R, m_R);
	DDV_MinMaxUInt(pDX, m_R, 0, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorPage3, CPropertyPage)
	//{{AFX_MSG_MAP(CColorPage3)
	ON_BN_DOUBLECLICKED(IDC_BCGBARRES_GREYHEXPLACEHOLDER, OnDoubleClickedColor)
	ON_BN_CLICKED(IDC_BCGBARRES_GREYHEXPLACEHOLDER, OnHexColor)
	ON_BN_CLICKED(IDC_BCGBARRES_GREYLUMPLACEHOLDER, OnLumColor)
	ON_EN_CHANGE(IDC_EDIT_LUMINANCE, OnLumChanged)
	ON_BN_DOUBLECLICKED(IDC_BCGBARRES_GREYLUMPLACEHOLDER, OnDoubleClickedColor)
	ON_EN_CHANGE(IDC_BAW_B, OnChangeB)
	ON_EN_CHANGE(IDC_BAW_G, OnChangeG)
	ON_EN_CHANGE(IDC_BAW_R, OnChangeR)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPage3 message handlers

void CColorPage3::OnDoubleClickedColor() 
{
	m_pDialog->EndDialog (IDOK);
}

BOOL CColorPage3::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_hexgreyscale.SetPalette (m_pDialog->GetPalette ());
	m_hexgreyscale.SetType(CBCGColorPickerCtrl::HEX_GREYSCALE);
	if (m_pDialog->IsDrawPalette())
		m_hexgreyscale.SetDrawPalette(m_pDialog->GetDrawPalette());

	m_luminance.SetPalette (m_pDialog->GetPalette ());
	m_luminance.SetType(CBCGColorPickerCtrl::LUMINANCE);
	if (m_pDialog->IsDrawPalette())
		m_luminance.SetDrawPalette(m_pDialog->GetDrawPalette());

	m_IsReady=TRUE;

	CSpinButtonCtrl* pWnd;
	pWnd = (CSpinButtonCtrl*) GetDlgItem (IDC_BAW_SPIN1);
	pWnd->SetRange (0, 255);
	pWnd = (CSpinButtonCtrl*) GetDlgItem (IDC_BAW_SPIN2);
	pWnd->SetRange (0, 255);
	pWnd = (CSpinButtonCtrl*) GetDlgItem (IDC_BAW_SPIN3);
	pWnd->SetRange (0, 255);
	pWnd = (CSpinButtonCtrl*) GetDlgItem (IDC_BAW_SPIN4);
	pWnd->SetRange (0, 255);

	if (m_pDialog->IsDrawPalette())
	{
		::EnableWindow(::GetDlgItem(m_hWnd,IDC_BAW_B),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,IDC_BAW_SPIN2),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,IDC_BAW_G),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,IDC_BAW_SPIN3),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,IDC_BAW_R),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,IDC_BAW_SPIN4),FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CColorPage3::OnHexColor() 
{
	double H,L,S;
	m_hexgreyscale.GetHLS(&H,&L,&S);

	COLORREF color = HLStoRGB_TWO(H, L, S);

	m_pDialog->SetNewColor (color);

	BYTE R = GetRValue (color);
	BYTE G = GetGValue (color);
	BYTE B = GetBValue (color);

	m_pDialog->SetPageThree (R, G, B);
	m_luminance.SetHLS(H,L,S, TRUE);
	m_luminance.Invalidate ();
	m_L=(UINT)G;
	m_R=(UINT)G;
	m_G=(UINT)G;
	m_B=(UINT)G;
	UpdateData(FALSE);
}

void CColorPage3::OnLumColor() 
{
	double H,L,S;
	m_luminance.GetHLS(&H,&L,&S);
	
	COLORREF color = HLStoRGB_TWO(H, L, S);

	m_pDialog->SetNewColor (color);

	BYTE R = GetRValue (color);
	BYTE G = GetGValue (color);
	BYTE B = GetBValue (color);

	m_pDialog->SetPageThree (R, G, B);

	m_hexgreyscale.SelectCellHexagon (R, G, B);
	m_hexgreyscale.Invalidate ();
	m_L=(UINT)G;
	m_R=(UINT)G;
	m_G=(UINT)G;
	m_B=(UINT)G;

	UpdateData(FALSE);
}

void CColorPage3::OnLumChanged() 
{
	if( !m_IsReady)
		return;
	UpdateData();
	m_R=(UINT)m_L;
	m_G=(UINT)m_L;
	m_B=(UINT)m_L;
	UpdateRGBL();
}

void CColorPage3::UpdateRGBL()
{
	COLORREF color = RGB (m_L, m_L, m_L);

	m_pDialog->SetNewColor (color);
	m_pDialog->SetPageThree ((BYTE) m_L, (BYTE) m_L, (BYTE) m_L);

	double hue;
	double luminance;
	double saturation;
	RGBtoHSL (color, &hue, &saturation, &luminance);
	m_hexgreyscale.SelectCellHexagon(m_L,m_L,m_L);
	m_luminance.SetHLS(hue, luminance, saturation, TRUE);
	UpdateData(FALSE);

}

void CColorPage3::OnChangeB() 
{
	if( !m_IsReady)
		return;
	UpdateData();
	m_R=(UINT)m_B;
	m_G=(UINT)m_B;
	m_L=(UINT)m_B;
	UpdateRGBL();
}

void CColorPage3::OnChangeG() 
{
	if( !m_IsReady)
		return;
	UpdateData();
	m_R=(UINT)m_G;
	m_L=(UINT)m_G;
	m_B=(UINT)m_G;
	UpdateRGBL();
}

void CColorPage3::OnChangeR() 
{
	if( !m_IsReady)
		return;
	UpdateData();
	m_L=(UINT)m_R;
	m_G=(UINT)m_R;
	m_B=(UINT)m_R;
	UpdateRGBL();
}
