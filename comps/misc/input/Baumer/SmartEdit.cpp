#include "StdAfx.h"
#include "SmartEdit.h"
#include "BaumUtils.h"
#include "Baumer.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool g_bLock;

#define LOCK() if (g_bLock) return; CLock lock(&g_bLock);

CSmartIntEdit::CSmartIntEdit(int nIdEdit, int nIdSpin, int nIdSlider)
{
	m_nIdEdit = nIdEdit;
	m_nIdSpin = nIdSpin;
	m_nIdSlider = nIdSlider;
	m_bOnChange = false;
}

void CSmartIntEdit::Init(CWnd *pDlg, CWnd *pParentSheet, IEditSite *pSite)
{
	m_pDlg = pDlg;
	m_pParent = pParentSheet;
	m_pSlider = (CSliderCtrl *)pDlg->GetDlgItem(m_nIdSlider);
	ASSERT_KINDOF(CSliderCtrl, m_pSlider);
	m_pSpin = (CSpinButtonCtrl *)pDlg->GetDlgItem(m_nIdSpin);
	ASSERT_KINDOF(CSpinButtonCtrl, m_pSpin);
	m_pSite = pSite;
}

void CSmartIntEdit::SetRange(int nMin, int nMax)
{
	m_pSlider->SetRange(nMin, nMax);
	m_pSpin->SetRange(nMin, nMax);
}

void CSmartIntEdit::SetPos(int nPos)
{
	m_pSlider->SetPos(nPos);
	m_pSpin->SetPos(nPos);
	m_pDlg->SetDlgItemInt(m_nIdEdit, nPos);
}

void CSmartIntEdit::OnEditChange()
{
	LOCK();
	int nPos = m_pDlg->GetDlgItemInt(m_nIdEdit);
	m_pSlider->SetPos(nPos);
	m_pSpin->SetPos(nPos);
	if (m_pSite) m_pSite->OnChangeValue(this,nPos);
}

void CSmartIntEdit::OnHScroll()
{
	LOCK();
	int nPos = m_pSlider->GetPos();
	m_pDlg->SetDlgItemInt(m_nIdEdit, nPos);
	m_pSpin->SetPos(nPos);
	if (m_pSite) m_pSite->OnChangeValue(this,nPos);
}

void CSmartIntEdit::OnSpinDelta(int nDelta)
{
	LOCK();
	int nPos = m_pSpin->GetPos();
	m_pDlg->SetDlgItemInt(m_nIdEdit, nPos);
	m_pSlider->SetPos(nPos);
	if (m_pSite) m_pSite->OnChangeValue(this,nPos);
}

void CSmartLogDoubleEdit::SetRange(int nRange)
{
	m_nRange = nRange;
	m_pSlider->SetRange(0, 2*m_nRange);
	m_pSpin->SetRange(0, 2*m_nRange);
}

void CSmartLogDoubleEdit::SetPos(double dVal)
{
	int nPos = PosByDValue(dVal);
	m_pSlider->SetPos(nPos);
	m_pSpin->SetPos(nPos);
	char szBuff[50];
	sprintf(szBuff, "%f", dVal);
	m_pDlg->SetDlgItemText(m_nIdEdit, szBuff);
}

void CSmartLogDoubleEdit::OnEditChange()
{
	LOCK();
	CString s;
	m_pDlg->GetDlgItemText(m_nIdEdit, s);
	double dVal = atof(s);
	int nPos = PosByDValue(dVal);
	m_pSlider->SetPos(nPos);
	m_pSpin->SetPos(nPos);
	if (m_pSite) m_pSite->OnChangeValue(this,nPos);
}

void CSmartLogDoubleEdit::OnHScroll()
{
	LOCK();
	int nPos = m_pSlider->GetPos();
	m_pSpin->SetPos(nPos);
	double dVal = DValueByPos(nPos);
	char szBuff[50];
	sprintf(szBuff, "%f", dVal);
	m_pDlg->SetDlgItemText(m_nIdEdit, szBuff);
	if (m_pSite) m_pSite->OnChangeValue(this,nPos);
}

void CSmartLogDoubleEdit::OnSpinDelta(int nDelta)
{
	LOCK();
	int nPos = m_pSlider->GetPos();
	nPos += nDelta;
	m_pSlider->SetPos(nPos);
	double dVal = DValueByPos(nPos);
	char szBuff[50];
	sprintf(szBuff, "%f", dVal);
	m_pDlg->SetDlgItemText(m_nIdEdit, szBuff);
	if (m_pSite) m_pSite->OnChangeValue(this,nPos);
}

int CSmartLogDoubleEdit::PosByDValue(double dValue)
{
	int n = int(10.*log(dValue));
	return Range(n, -m_nRange, m_nRange)+m_nRange;
}

double CSmartLogDoubleEdit::DValueByPos(int nPos)
{
	double d = (nPos-m_nRange)/10.;
	return pow(10., d);
}


