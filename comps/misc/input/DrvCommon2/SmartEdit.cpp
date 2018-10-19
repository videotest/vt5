#include "StdAfx.h"
#include "SmartEdit.h"
#include "Ctrls.h"
#include "StdProfile.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool g_bLock;

#define LOCK() if (g_bLock) return; CLock lock(&g_bLock);
#define LOCK1(b) if (g_bLock) return b; CLock lock(&g_bLock);

CDrvControl::CDrvControl()
{
	m_pSite = NULL;
}

CDrvControl::~CDrvControl()
{
	if (m_pSite)
		m_pSite->RemoveControl(this);
}

void CDrvControl::Register(IDrvControlSite *pSite)
{
	if (!m_pSite)
	{
		m_pSite = pSite;
		m_pSite->AddControl(this);
	}
}


void CSmartIntEdit::_Init(int nIdEdit, int nIdSpin, int nIdSlider, LPCTSTR lpstrSec, LPCTSTR lpstrEntry,
	int nMin, int nMax, int nDef, int nCur, IEditSite *pSite, UINT nFlags)
{
	m_nIdEdit = nIdEdit;
	m_nIdSpin = nIdSpin;
	m_nIdSlider = nIdSlider;
	m_nMin = nMin;
	m_nMax = nMax;
	m_nDef = nDef;
	m_sSec = lpstrSec;
	m_sEntry = lpstrEntry;
	m_nFlags = nFlags;
	m_bOnChange = false;
	m_pSite = pSite;
	m_nCur = nCur;
}


CSmartIntEdit::CSmartIntEdit(int nIdEdit, int nIdSpin, int nIdSlider, LPCTSTR lpstrSec,
	LPCTSTR lpstrEntry, int nMin, int nMax, int nDef, UINT nFlags)
{
	_Init(nIdEdit,nIdSpin,nIdSlider,lpstrSec,lpstrEntry,nMin,nMax,nDef,-1,0,nFlags);
}

CSmartIntEdit::CSmartIntEdit(int nIdEdit, int nIdSpin, int nIdSlider, LPCTSTR lpstrSec, LPCTSTR lpstrEntry,
	IEditSite *pSite, UINT nFlags)
{
	_Init(nIdEdit,nIdSpin,nIdSlider,lpstrSec,lpstrEntry,-1,-1,-1,-1,pSite,nFlags);
}

CSmartIntEdit::CSmartIntEdit(int nIdEdit, int nIdSpin, int nIdSlider, int nMin, int nMax, int nDef, 
	int nCur, IEditSite *pSite, UINT nFlags)
{
	_Init(nIdEdit,nIdSpin,nIdSlider,0,0,nMin,nMax,nDef,nCur,pSite,nFlags);
}

bool CSmartIntEdit::InitControls(CWnd *pDlg, IDriver5 *pDrv)
{
	m_pDlg = pDlg;
	m_pSlider = m_nIdSlider<=0?0:(CSliderCtrl *)pDlg->GetDlgItem(m_nIdSlider);
	m_pSpin = m_nIdSpin<=0?0:(CSpinButtonCtrl *)pDlg->GetDlgItem(m_nIdSpin);
	if (m_pSlider == NULL && pDlg->GetDlgItem(m_nIdEdit) == NULL)
		return false;
	m_sptrDrv = pDrv;
	return true;
}

void CSmartIntEdit::EnableControls(bool bEnable)
{
	if (!bEnable)
	{
		if (m_pSlider) m_pSlider->EnableWindow(FALSE);
		if (m_pSpin) m_pSpin->EnableWindow(FALSE);
		::EnableWindow(::GetDlgItem(m_pDlg->GetSafeHwnd(), m_nIdEdit), FALSE);
	}
}

bool CSmartIntEdit::InitDefaults(int &nCurrent)
{
	VALUEINFOEX vie;
	if (SUCCEEDED(m_sptrDrv->GetValueInfoByName(0, m_sSec, m_sEntry, &vie)))
	{
		m_nMin = vie.Info.nMin;
		m_nMax = vie.Info.nMax;
		m_nDef = vie.Info. nDefault;
		nCurrent = vie.Info.nCurrent;
		return true;
	}
	else
		return false;
}

bool CSmartIntEdit::Init(CWnd *pDlg, IDriver5 *pDrv)
{
	if (!InitControls(pDlg, pDrv))
		return false;
	int nCurrent = m_nCur;
	bool bEnable = true;
	if (m_nDef == -1 && m_sptrDrv != 0)
		bEnable = InitDefaults(nCurrent);
	if (bEnable)
	{
		if (nCurrent == -1 && m_sptrDrv != 0)
		{
			nCurrent = m_nDef;
			m_sptrDrv->GetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), &nCurrent, 1);
		}
		SetRange(m_nMin, m_nMax);
		SetPos(nCurrent);
	}
	else
		EnableControls(false);
	return true;
}

bool CSmartIntEdit::Reset()
{
	int nVal = m_nDef;
	m_sptrDrv->GetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), &nVal, 1);
	SetPos(nVal);
	return true;
}

void CSmartIntEdit::Default()
{
	int nVal = m_nDef;
	int nPos = PosByValue(nVal);
	if (m_pSlider) m_pSlider->SetPos(nPos);
	if (m_pSpin) m_pSpin->SetPos(nPos);
	m_pDlg->SetDlgItemInt(m_nIdEdit, nVal);
	if (m_sptrDrv != 0) m_sptrDrv->SetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), nVal, 1);
	if (m_pSite) m_pSite->OnChangeValue(this,nVal);
}

void CSmartIntEdit::SetRange(int nMin, int nMax)
{
	if (m_pSlider) m_pSlider->SetRange(nMin, nMax);
	if (m_pSlider) m_pSlider->SetTicFreq(max((nMax-nMin)/20,1));
	if (m_pSpin) m_pSpin->SetRange(nMin, nMax);
}

void CSmartIntEdit::SetPos(int nPos)
{
	if (m_pSlider) m_pSlider->SetPos(nPos);
	if (m_pSpin) m_pSpin->SetPos(nPos);
	m_pDlg->SetDlgItemInt(m_nIdEdit, nPos);
}

bool CSmartIntEdit::OnEditChange(int idEdit)
{
	if (idEdit != m_nIdEdit) return false;
	LOCK1(true);
	int nVal = m_pDlg->GetDlgItemInt(m_nIdEdit);
	int nPos = PosByValue(nVal);
	if (m_pSlider) m_pSlider->SetPos(nPos);
	if (m_pSpin) m_pSpin->SetPos(nPos);
	if (m_sptrDrv != 0) m_sptrDrv->SetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), nVal, 1);
	if (m_pSite) m_pSite->OnChangeValue(this,nVal);
	return true;
}

bool CSmartIntEdit::OnHScroll(int idScroll, UINT nSBCode)
{
	if (idScroll != m_nIdSlider)
		return false;
//	if ((m_nFlags&ThumbPosition) && nSBCode == SB_THUMBTRACK)
//		return true;
	LOCK1(true);
	int nPos = m_pSlider->GetPos();
	int nVal = ValueByPos(nPos);
	m_pDlg->SetDlgItemInt(m_nIdEdit, nVal);
	if (m_pSpin) m_pSpin->SetPos(nPos);
	if (m_sptrDrv != 0 && !m_sSec.IsEmpty() && !m_sEntry.IsEmpty() &&
		!((m_nFlags&ThumbPosition) && nSBCode == SB_THUMBTRACK))
		m_sptrDrv->SetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), nVal, 1);
	if (m_pSite) m_pSite->OnChangeValue(this,nVal);
	return true;
}

bool CSmartIntEdit::OnSpinDelta(int idSpin, int nDelta)
{
	if (idSpin != m_nIdSpin)
		return false;
	LOCK1(true);
	int nPos;
	if (m_pSlider)
		nPos = m_pSlider->GetPos();
	else
		nPos = GetDlgItemInt(m_pDlg->GetSafeHwnd(), m_nIdEdit, NULL, FALSE);
	nPos = ValidatePos(nPos+nDelta);
	int nVal = ValueByPos(nPos);
	m_pDlg->SetDlgItemInt(m_nIdEdit, nVal);
	if (m_pSlider) m_pSlider->SetPos(nPos);
	if (m_sptrDrv != 0) m_sptrDrv->SetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), nVal, 1);
	if (m_pSite) m_pSite->OnChangeValue(this,nVal);
	return true;
}

CSmartSpanEdit::CSmartSpanEdit(int nIdEdit, int nIdSpin, int nIdSlider, LPCTSTR lpstrSec, LPCTSTR lpstrEntry,
	int nMin, int nMax, int nDef) :	CSmartIntEdit(nIdEdit, nIdSpin, nIdSlider, lpstrSec,
	lpstrEntry, nMin, nMax, nDef)
{
}

CSmartSpanEdit::CSmartSpanEdit(int nIdEdit, int nIdSpin, int nIdSlider, LPCTSTR lpstrSec, LPCTSTR lpstrEntry) :
	CSmartIntEdit(nIdEdit, nIdSpin, nIdSlider, lpstrSec, lpstrEntry)
{
}

CSmartSpanEdit::CSmartSpanEdit(int nIdEdit, int nIdSpin, int nIdSlider, int nMin, int nMax, int nDef,
	int nCur, IEditSite *pSite):
	CSmartIntEdit(nIdEdit, nIdSpin, nIdSlider, nMin, nMax, nDef, nCur, pSite)
{
}

void CSmartSpanEdit::InitValues(int nMin, int nMax)
{
	m_arrVals.RemoveAll();
	for (int nStep = nMin; nStep < nMax; nStep *= 10)
	{
		for (int i = 1; i < 5 ; i++)
		{
			for (int j = 0; j < 10; j++)
				m_arrVals.Add((UINT)nStep*i+nStep*j/10);
		}
		for (; i < 10 ; i++)
			m_arrVals.Add((UINT)nStep*i);
	}
	m_arrVals.Add((UINT)nMax);
}

int CSmartSpanEdit::PosByValue(int nValue)
{
	for(int i = 1; i < m_arrVals.GetSize(); i++)
	{
		int n = (int)m_arrVals[i];
		if (nValue < n)
			return i-1;
	}
	return (int)m_arrVals.GetSize()-1;
}

bool CSmartSpanEdit::Init(CWnd *pDlg, IDriver5 *pDrv)
{
	if (!InitControls(pDlg, pDrv))
		return false;
	int nCurrent = m_nCur;
	bool bEnable = true;
	if (m_nDef == -1 && m_sptrDrv != 0)
		bEnable = InitDefaults(nCurrent);
	if (nCurrent == -1 && m_sptrDrv != 0)
	{
		nCurrent = m_nDef;
		m_sptrDrv->GetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), &nCurrent, 1);
	}
	if (bEnable)
	{
		InitValues(m_nMin, m_nMax);
		SetRange(0, (int)m_arrVals.GetSize()-1);
		int nPos = PosByValue(nCurrent);
		if (m_pSlider) m_pSlider->SetPos(nPos);
		if (m_pSpin) m_pSpin->SetPos(nPos);
		m_pDlg->SetDlgItemInt(m_nIdEdit, nCurrent);
	}
	else
		EnableControls(false);
	return true;
}

CSmartIntEdit2::CSmartIntEdit2(int nIdEdit, int nIdSpin, int nIdSlider, int nIdSpin2, int nIdSlider2,
	LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nMin, int nMax, int nDef, int nStep1,
	int nStep2, UINT nFlags) : CSmartIntEdit(nIdEdit, nIdSpin, nIdSlider, lpstrSec,
	lpstrEntry, nMin, nMax, nDef)
{
	m_nIdSpin2 = nIdSpin2;
	m_nIdSlider2 = nIdSlider2;
	m_nStep1 = nStep1;
	m_nStep2 = nStep2;
}

CSmartIntEdit2::CSmartIntEdit2(int nIdEdit, int nIdSpin, int nIdSlider, int nIdSpin2, int nIdSlider2,
	LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nStep1, int nStep2, IEditSite *pSite, UINT nFlags) :
	CSmartIntEdit(nIdEdit, nIdSpin, nIdSlider, lpstrSec, lpstrEntry)
{
	m_nIdSpin2 = nIdSpin2;
	m_nIdSlider2 = nIdSlider2;
	m_nStep1 = nStep1;
	m_nStep2 = nStep2;
}

CSmartIntEdit2::CSmartIntEdit2(int nIdEdit, int nIdSpin, int nIdSlider, int nIdSpin2, int nIdSlider2,
	int nMin, int nMax, int nDef, int nCur, int nStep1, int nStep2, IEditSite *pSite, UINT nFlags) :
	CSmartIntEdit(nIdEdit, nIdSpin, nIdSlider, nMin, nMax, nDef, nCur, pSite)
{
	m_nIdSpin2 = nIdSpin2;
	m_nIdSlider2 = nIdSlider2;
	m_nStep1 = nStep1;
	m_nStep2 = nStep2;
}

bool CSmartIntEdit2::InitControls(CWnd *pDlg, IDriver5 *pSite)
{
	if (!__super::InitControls(pDlg,pSite))
		return false;
	m_pSlider2 = m_nIdSlider<=0?0:(CSliderCtrl *)pDlg->GetDlgItem(m_nIdSlider2);
	m_pSpin2 = m_nIdSpin<=0?0:(CSpinButtonCtrl *)pDlg->GetDlgItem(m_nIdSpin2);
	return true;
}

bool CSmartIntEdit2::Init(CWnd *pDlg, IDriver5 *pDrv)
{
	if (!InitControls(pDlg, pDrv))
		return false;
	int nCurrent = m_nCur;
	bool bEnable = true;
	if (m_nDef == -1 && m_sptrDrv != 0)
		bEnable = InitDefaults(nCurrent);
	if (bEnable)
	{
		if (nCurrent == -1 && m_sptrDrv != 0)
		{
			nCurrent = m_nDef;
			m_sptrDrv->GetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), &nCurrent, 1);
		}
		SetRange(m_nMin, m_nMax);
		SetPos(nCurrent);
	}
	else
		EnableControls(false);
	return true;
}

void CSmartIntEdit2::EnableControls(bool bEnable)
{
	__super::EnableControls(bEnable);
	if (!bEnable)
	{
		if (m_pSlider2) m_pSlider2->EnableWindow(FALSE);
		if (m_pSpin2) m_pSpin2->EnableWindow(FALSE);
	}
}

void CSmartIntEdit2::SetRange(int nMin, int nMax)
{
	if (m_pSlider) m_pSlider->SetRange(nMin/m_nStep1, nMax/m_nStep1);
	if (m_pSlider) m_pSlider->SetTicFreq(max((nMax/m_nStep1-nMin/m_nStep1)/20,1));
	if (m_pSpin) m_pSpin->SetRange(nMin/m_nStep1, nMax/m_nStep1);
	if (m_pSlider2) m_pSlider2->SetRange(0, m_nStep1-1);
	if (m_pSlider2) m_pSlider2->SetTicFreq(max(m_nStep1/20,1));
	if (m_pSpin2) m_pSpin2->SetRange(0, m_nStep1-1);
}

void CSmartIntEdit2::_SetPos(int nPos, DWORD dwFlags)
{
	int nPos1 = nPos/m_nStep1;
	if (m_pSlider && (dwFlags&8)==0) m_pSlider->SetPos(nPos1);
	if (m_pSpin && (dwFlags&2)==0) m_pSpin->SetPos(nPos1);
	int nPos2 = nPos-nPos1*m_nStep1;
	if (m_pSlider2 && (dwFlags&16)==0) m_pSlider2->SetPos(nPos2);
	if (m_pSpin2 && (dwFlags&4)==0) m_pSpin2->SetPos(nPos2);
	if ((dwFlags&1)==0) m_pDlg->SetDlgItemInt(m_nIdEdit, nPos);
}

void CSmartIntEdit2::SetPos(int nPos)
{
	_SetPos(nPos, 0);
}

bool CSmartIntEdit2::IsHandler(int id)
{
	if (__super::IsHandler(id))
		return true;
	return id==m_nIdSpin2||id==m_nIdSlider2;
}

bool CSmartIntEdit2::OnEditChange(int idEdit)
{
	if (idEdit != m_nIdEdit) return false;
	LOCK1(true);
	int nVal = m_pDlg->GetDlgItemInt(m_nIdEdit);
	int nPos = PosByValue(nVal);
	_SetPos(nPos, 1);
	if (m_sptrDrv != 0) m_sptrDrv->SetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), nVal, 1);
	if (m_pSite) m_pSite->OnChangeValue(this,nVal);
	return true;
}

bool CSmartIntEdit2::OnHScroll(int idScroll, UINT nSBCode)
{
	if (idScroll != m_nIdSlider && idScroll != m_nIdSlider2)
		return false;
	if ((m_nFlags&ThumbPosition) && nSBCode == SB_THUMBTRACK)
		return true;
	LOCK1(true);
	int nPos1 = m_pSlider->GetPos();
	int nPos2 = m_pSlider2->GetPos();
	int nPos = nPos1*m_nStep1+nPos2;
	nPos = min(max(nPos,m_nMin),m_nMax);
	int nVal = ValueByPos(nPos);
	m_pDlg->SetDlgItemInt(m_nIdEdit, nVal);
	_SetPos(nPos, 1+8+16);
	if (m_sptrDrv != 0 && !m_sSec.IsEmpty() && !m_sEntry.IsEmpty())
		m_sptrDrv->SetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), nVal, 1);
	if (m_pSite) m_pSite->OnChangeValue(this,nVal);
	return true;
}

bool CSmartIntEdit2::OnSpinDelta(int idSpin, int nDelta)
{
	if (idSpin != m_nIdSpin && idSpin != m_nIdSpin2)
		return false;
	LOCK1(true);
	int nPos;
	if (m_pSlider)
	{
		int nPos1 = m_pSlider->GetPos();
		int nPos2 = m_pSlider2->GetPos();
		nPos = nPos1*m_nStep1+nPos2;
		nPos = min(max(nPos,m_nMin),m_nMax);
	}
	else
		nPos = GetDlgItemInt(m_pDlg->GetSafeHwnd(), m_nIdEdit, NULL, FALSE);
	if (idSpin == m_nIdSpin)
		nPos = ValidatePos(nPos+nDelta*m_nStep1);
	else
		nPos = ValidatePos(nPos+nDelta);
	int nVal = ValueByPos(nPos);
	_SetPos(nPos, 1+2+4);
	m_pDlg->SetDlgItemInt(m_nIdEdit, nVal);
//	if (m_pSlider) m_pSlider->SetPos(nPos);
	if (m_sptrDrv != 0) m_sptrDrv->SetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), nVal, 1);
	if (m_pSite) m_pSite->OnChangeValue(this,nVal);
	return true;
}

CSmartLogDoubleEdit::CSmartLogDoubleEdit(int nIdEdit, int nIdSpin, int nIdSlider, LPCTSTR lpstrSec,
	LPCTSTR lpstrEntry, double dMin, double dMax, double dDef, UINT nFlags) :
	CSmartIntEdit(nIdEdit, nIdSpin, nIdSlider, lpstrSec, lpstrEntry, 0, 0, 0, nFlags)
{
	double d = dMin;
	while (d <= dMax)
	{
		for (double c = 1.; c < 4.95 && d*c<=dMax; c += .1)
			m_arrValues.Add(d*c);
		for (int i = 5; i < 10 && d*i<=dMax; i++)
			m_arrValues.Add(d*i);
		d *= 10.;
	}
	m_nMax = (int)m_arrValues.GetSize()-1;
	m_nDef = PosByDValue(dDef);
}

int CSmartLogDoubleEdit::PosByDValue(double d)
{
	for (int i = 0; i < m_arrValues.GetSize()-1; i++)
		if (m_arrValues[i] >= d)
			break;
	return i;
}

bool CSmartLogDoubleEdit::Init(CWnd *pDlg, IDriver5 *pDrv)
{
	if (!InitControls(pDlg, pDrv))
		return false;
	double dCur = DValueByPos(m_nDef);
	if (m_sptrDrv != 0)
		m_sptrDrv->GetValueDouble(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), &dCur, 1);
	m_nCur = PosByDValue(dCur);
	SetRange(m_nMin, m_nMax);
	SetPos(dCur);
	return true;
}

/*void CSmartLogDoubleEdit::SetRange(int nRange)
{
	m_nRange = nRange;
	m_pSlider->SetRange(0, 2*m_nRange);
	m_pSpin->SetRange(0, 2*m_nRange);
}*/

void CSmartLogDoubleEdit::SetPos(double dVal)
{
	int nPos = PosByDValue(dVal);
	m_pSlider->SetPos(nPos);
	m_pSpin->SetPos(nPos);
	char szBuff[50];
	sprintf(szBuff, "%.2f", dVal);
	m_pDlg->SetDlgItemText(m_nIdEdit, szBuff);
}

bool CSmartLogDoubleEdit::OnEditChange(int idEdit)
{
	if (idEdit != m_nIdEdit) return false;
	LOCK1(true);
	CString s;
	m_pDlg->GetDlgItemText(m_nIdEdit, s);
	double dVal = atof(s);
	int nPos = PosByDValue(dVal);
	m_pSlider->SetPos(nPos);
	m_pSpin->SetPos(nPos);
	if (m_sptrDrv != 0) m_sptrDrv->SetValueDouble(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), dVal, 1);
	if (m_pSite) m_pSite->OnChangeValue(this,nPos);
	return true;
}

bool CSmartLogDoubleEdit::Reset()
{
	double dCur = DValueByPos(m_nDef);
	if (m_sptrDrv != 0)
		m_sptrDrv->GetValueDouble(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), &dCur, 1);
	SetPos(dCur);
	return true;
}


bool CSmartLogDoubleEdit::OnHScroll(int idScroll, UINT nSBCode)
{
	if (idScroll != m_nIdSlider)
		return false;
	if ((m_nFlags&ThumbPosition) && nSBCode == SB_THUMBTRACK)
		return true;
	LOCK1(true);
	int nPos = m_pSlider->GetPos();
	m_pSpin->SetPos(nPos);
	double dVal = DValueByPos(nPos);
	char szBuff[50];
	sprintf(szBuff, "%.2f", dVal);
	m_pDlg->SetDlgItemText(m_nIdEdit, szBuff);
	if (m_sptrDrv != 0) m_sptrDrv->SetValueDouble(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), dVal, 1);
	if (m_pSite) m_pSite->OnChangeValue(this,nPos);
	return true;
}

bool CSmartLogDoubleEdit::OnSpinDelta(int idSpin, int nDelta)
{
	if (idSpin != m_nIdSpin)
		return false;
	LOCK1(true);
	int nPos = m_pSlider->GetPos();
	nPos += nDelta;
	nPos = Range(nPos, m_nMin, m_nMax);
	m_pSlider->SetPos(nPos);
	double dVal = DValueByPos(nPos);
	char szBuff[50];
	sprintf(szBuff, "%.2f", dVal);
	m_pDlg->SetDlgItemText(m_nIdEdit, szBuff);
	if (m_sptrDrv != 0) m_sptrDrv->SetValueDouble(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), dVal, 1);
	if (m_pSite) m_pSite->OnChangeValue(this,nPos);
	return true;
}

/*int CSmartLogDoubleEdit::PosByDValue(double dValue)
{
	int n = int(10.*log(dValue));
	return Range(n, -m_nRange, m_nRange)+m_nRange;
}

double CSmartLogDoubleEdit::DValueByPos(int nPos)
{
	double d = (nPos-m_nRange)/10.;
	return pow(10., d);
}*/

bool CSmartRangeDoubleEdit::Init(CWnd *pDlg, IDriver5 *pDrv)
{
	if (!InitControls(pDlg, pDrv))
		return false;
	SetRange(m_dMin, m_dMax, m_dStep);
	double dVal = m_dDef;
	m_sptrDrv->GetValueDouble(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), &dVal, 1);
	SetPos(dVal);
	return true;
}

bool CSmartRangeDoubleEdit::Reset()
{
	double dVal = m_dDef;
	m_sptrDrv->GetValueDouble(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), &dVal, 1);
	SetPos(dVal);
	return true;
}

void CSmartRangeDoubleEdit::Default()
{
	double dVal = m_dDef;
	int nPos = (int)((dVal-m_dMin)/m_dStep);
	if (m_pSlider) m_pSlider->SetPos(nPos);
	if (m_pSpin) m_pSpin->SetPos(nPos);
	SetDlgItemDouble(m_pDlg->m_hWnd, m_nIdEdit, dVal);
	if (m_sptrDrv != 0) m_sptrDrv->SetValueDouble(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), dVal, 1);
}

void CSmartRangeDoubleEdit::SetRange(double dMin, double dMax, double dStep)
{
	m_dMin = dMin;
	m_dMax = dMax;
	m_dStep = dStep;
	int nRange = (int)((dMax-dMin)/dStep);
	m_nMin = 0;
	m_nMax = nRange;
	if (m_pSlider) m_pSlider->SetRange(0, nRange);
	if (m_pSpin) m_pSpin->SetRange(0, nRange);
}

void CSmartRangeDoubleEdit::SetPos(double dVal)
{
	int nPos = (int)((dVal-m_dMin)/m_dStep);
	nPos = Range(nPos, m_nMin, m_nMax);
	if (m_pSlider) m_pSlider->SetPos(nPos);
	if (m_pSpin) m_pSpin->SetPos(nPos);
	char szBuff[50];
	sprintf(szBuff, "%f", dVal);
	m_pDlg->SetDlgItemText(m_nIdEdit, szBuff);
}

bool CSmartRangeDoubleEdit::OnEditChange(int idEdit)
{
	if (idEdit != m_nIdEdit) return false;
	LOCK1(true);
	CString s;
	m_pDlg->GetDlgItemText(m_nIdEdit, s);
	double dVal = atof(s);
	int nPos = (int)((dVal-m_dMin)/m_dStep);
	nPos = Range(nPos, m_nMin, m_nMax);
	m_pSlider->SetPos(nPos);
	m_pSpin->SetPos(nPos);
	if (m_sptrDrv != 0) m_sptrDrv->SetValueDouble(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), dVal, 1);
	if (m_pSite) m_pSite->OnChangeValue(this,nPos);
	return true;
}

bool CSmartRangeDoubleEdit::OnHScroll(int idScroll, UINT nSBCode)
{
	if (idScroll != m_nIdSlider)
		return false;
	if ((m_nFlags&ThumbPosition) && nSBCode == SB_THUMBTRACK)
		return true;
	LOCK1(true);
	int nPos = m_pSlider->GetPos();
	m_pSpin->SetPos(nPos);
	double dVal = m_dMin+m_dStep*nPos;
	char szBuff[50];
	sprintf(szBuff, "%f", dVal);
	m_pDlg->SetDlgItemText(m_nIdEdit, szBuff);
	if (m_sptrDrv != 0) m_sptrDrv->SetValueDouble(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), dVal, 1);
	if (m_pSite) m_pSite->OnChangeValue(this,nPos);
	CString s;
	m_pDlg->GetDlgItemText(m_nIdEdit, s);
	return true;
}

bool CSmartRangeDoubleEdit::OnSpinDelta(int idSpin, int nDelta)
{
	if (idSpin != m_nIdSpin)
		return false;
	LOCK1(true);
	int nPos = m_pSlider->GetPos();
	nPos += nDelta;
	m_pSlider->SetPos(nPos);
	double dVal = m_dMin+m_dStep*nPos;
	char szBuff[50];
	sprintf(szBuff, "%f", dVal);
	m_pDlg->SetDlgItemText(m_nIdEdit, szBuff);
	if (m_sptrDrv != 0) m_sptrDrv->SetValueDouble(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), dVal, 1);
	if (m_pSite) m_pSite->OnChangeValue(this,nPos);
	return true;
}


CControlHandler::CControlHandler(int id, LPCTSTR lpstrSec, LPCTSTR lpstrEntry)
{
	m_id = id;
	m_sSec = lpstrSec;
	m_sEntry = lpstrEntry;
}

bool CControlHandler::Init(CWnd *pDlg, IDriver5 *pDrv)
{
	HWND hwnd = ::GetDlgItem(pDlg->GetSafeHwnd(), m_id);
	if (hwnd == NULL)
		return false;
	m_pDlg = pDlg;
	m_sptrDrv = pDrv;
	return true;
}

void CControlHandler::DoReinit(int id, LPCTSTR lpstrSec, LPCTSTR lpstrEntry)
{
	m_id = id;
	m_sSec = lpstrSec;
	m_sEntry = lpstrEntry;
}

CCheckboxHandler::CCheckboxHandler(int idCheckBox, LPCTSTR lpstrSec, LPCTSTR lpstrEntry,
	BOOL bDefault)
{
	m_bDefault = bDefault;
	m_idCheckBox = idCheckBox;
	m_sSec = lpstrSec;
	m_sEntry = lpstrEntry;
}

bool CCheckboxHandler::Init(CWnd *pDlg, IDriver5 *pDrv)
{
	HWND hwnd = ::GetDlgItem(pDlg->GetSafeHwnd(), m_idCheckBox);
	if (hwnd == NULL)
		return false;
	m_pDlg = pDlg;
	m_sptrDrv = pDrv;
	int nVal = m_bDefault;
	m_sptrDrv->GetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), &nVal, 0);
	m_pDlg->CheckDlgButton(m_idCheckBox, nVal);
	return true;
}

void CCheckboxHandler::Default()
{
	int nVal = m_bDefault;
	m_pDlg->CheckDlgButton(m_idCheckBox, nVal);
	if (m_sptrDrv != 0) m_sptrDrv->SetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), nVal, 1);
}

bool CCheckboxHandler::OnCmdMsg(int nCode, int id, HWND hwndCtrl)
{
	if (nCode!=BN_CLICKED||id!=m_idCheckBox)
		return false;
	int nVal = m_pDlg->IsDlgButtonChecked(m_idCheckBox);
	if (m_sptrDrv != 0) m_sptrDrv->SetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), nVal, 1);
	return true;
}

CRadioHandler::CRadioHandler(int *pnArray, int nArraySize, LPCTSTR lpstrSec, LPCTSTR lpstrEntry,
	int nDefault)
{
	for (int i = 0; i < nArraySize; i++)
		m_arrIds.Add(pnArray[i]);
	m_nFirstButton = m_nLastButton = pnArray[0];
	for (i = 1; i < nArraySize; i++)
	{
		if (pnArray[i] < m_nFirstButton)
			m_nFirstButton = pnArray[i];
		if (pnArray[i] > m_nLastButton)
			m_nLastButton = pnArray[i];
	}
	m_sSec = lpstrSec;
	m_sEntry = lpstrEntry;
	m_nCurrent = m_nDefault = nDefault;
}

int CRadioHandler::ValueById(int id)
{
	for (int i = 0; i < m_arrIds.GetSize(); i++)
		if (m_arrIds[i] == id)
			return i;
	return 0;
}

bool CRadioHandler::IsHandler(int id)
{
	for (int i = 0; i < m_arrIds.GetSize(); i++)
		if (m_arrIds[i] == id)
			return true;
	return false;
}

bool CRadioHandler::Init(CWnd *pDlg, IDriver5 *pDrv)
{
	HWND hwnd = ::GetDlgItem(pDlg->GetSafeHwnd(), m_nFirstButton);
	if (hwnd == NULL)
		return false;
	m_pDlg = pDlg;
	m_sptrDrv = pDrv;
	m_sptrDrv->GetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), &m_nCurrent, 0);
	m_pDlg->CheckRadioButton(m_nFirstButton, m_nLastButton, m_arrIds[m_nCurrent]);
	return true;
}

void CRadioHandler::Default()
{
	m_nCurrent = m_nDefault;
	m_pDlg->CheckRadioButton(m_nFirstButton, m_nLastButton, m_arrIds[m_nCurrent]);
	m_sptrDrv->SetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), m_nCurrent, 0);
}

bool CRadioHandler::OnCmdMsg(int nCode, int id, HWND hwndCtrl)
{
	if (nCode!=BN_CLICKED || !IsHandler(id))
		return false;
	int nId = m_pDlg->GetCheckedRadioButton(m_nFirstButton, m_nLastButton);
	m_nCurrent = ValueById(nId);
	if (m_sptrDrv != 0) m_sptrDrv->SetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), m_nCurrent, 1);
	return true;
}

CComboboxHandler::CComboboxHandler(int id, LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nDefault,
	COMBOBOXREPRDATA *pReprData) : CControlHandler(id, lpstrSec, lpstrEntry)
{
	m_nDefault = nDefault;
	m_pReprData = pReprData;
}

bool CComboboxHandler::Init(CWnd *pDlg, IDriver5 *pSite)
{
	if (!CControlHandler::Init(pDlg, pSite))
		return false;
	int nVal = m_nDefault;
	m_sptrDrv->GetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), &nVal, 0);
	InitComboBox(pDlg->GetSafeHwnd(), m_id, nVal, m_pReprData);
	return true;
}

void CComboboxHandler::Default()
{
	int nVal = m_nDefault;
	InitComboBox(m_pDlg->GetSafeHwnd(), m_id, nVal);
	if (m_sptrDrv != 0) m_sptrDrv->SetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), nVal, 1);
}

bool CComboboxHandler::OnCmdMsg(int nCode, int id, HWND hwndCtrl)
{
	if (id==m_id&&nCode==LBN_SELCHANGE)
	{
		int nVal = GetComboBoxValue(m_pDlg->GetSafeHwnd(), m_id);
		if (m_sptrDrv != 0) m_sptrDrv->SetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), nVal, 1);
		return true;
	}
	return false;
}


CListboxHandler::CListboxHandler(int id, LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nDefault,
	COMBOBOXREPRDATA *pReprData) : CControlHandler(id, lpstrSec, lpstrEntry)
{
	m_nDefault = nDefault;
	m_pReprData = pReprData;
}

bool CListboxHandler::Init(CWnd *pDlg, IDriver5 *pSite)
{
	if (!CControlHandler::Init(pDlg, pSite))
		return false;
	int nVal = m_nDefault;
	if (m_sptrDrv != 0 && !m_sSec.IsEmpty() && !m_sEntry.IsEmpty())
		m_sptrDrv->GetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), &nVal, 0);
	InitListBox(pDlg->GetSafeHwnd(), m_id, nVal, m_pReprData);
	return true;
}

void CListboxHandler::Default()
{
	int nVal = m_nDefault;
	InitListBox(m_pDlg->GetSafeHwnd(), m_id, nVal);
	if (m_sptrDrv != 0 && !m_sSec.IsEmpty() && !m_sEntry.IsEmpty())
		m_sptrDrv->SetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), nVal, 1);
}

bool CListboxHandler::OnCmdMsg(int nCode, int id, HWND hwndCtrl)
{
	if (id==m_id&&nCode==LBN_SELCHANGE)
	{
		int nVal = GetListBoxValue(m_pDlg->GetSafeHwnd(), m_id);
		if (m_sptrDrv != 0 && !m_sSec.IsEmpty() && !m_sEntry.IsEmpty())
		{
			m_sptrDrv->SetValueInt(0, _bstr_t(m_sSec), _bstr_t(m_sEntry), nVal, 1);
			return true;
		}
	}
	return false;
}

bool CListboxHandler::Reinit()
{
	if (m_pSite)
	{
		VALUEINFOEX2 vie2;
		m_pSite->GetValueInfo(&vie2);
		if (vie2.pInfo->nType == ValueType_Int)
		{
			DoReinit(vie2.pReprData->nDlgCtrlId, vie2.pHdr->pszSection, vie2.pHdr->pszEntry);
			m_nDefault = ((VALUEINFO*)vie2.pInfo)->nDefault;
			m_pReprData = vie2.pReprData->Type==IRepr_Combobox?(COMBOBOXREPRDATA*)vie2.pReprData:NULL;
			InitListBox(m_pDlg->GetSafeHwnd(), m_id, ((VALUEINFO*)vie2.pInfo)->nCurrent, m_pReprData);
		}
		return true;
	}
	return false;
}

CStaticHandler::CStaticHandler(int id, LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nDefault) :
	CControlHandler(id, lpstrSec, lpstrEntry)	
{
	m_nType = 0;
	m_nDefault = nDefault;
}

CStaticHandler::CStaticHandler(int id, LPCTSTR lpstrSec, LPCTSTR lpstrEntry, LPCTSTR lpstrDefault) :
	CControlHandler(id, lpstrSec, lpstrEntry)	
{
	m_nType = 1;
	m_sDefault = lpstrDefault;
}

bool CStaticHandler::Init(CWnd *pDlg, IDriver5 *pSite)
{
	bool b = __super::Init(pDlg, pSite);
	if (b)
	{
		if (m_nType == 0)
			m_pDlg->SetDlgItemInt(m_id, m_nDefault);
		else if (m_nType == 1)
			m_pDlg->SetDlgItemText(m_id, m_sDefault);
	}
	return b;
}

bool CStaticHandler::Reset()
{
	if (m_pSite)
	{
		VALUEINFOEX2 vie2;
		m_pSite->GetValueInfo(&vie2);
		if (vie2.pInfo->nType == ValueType_Int)
		{
			int n = ((VALUEINFO*)vie2.pInfo)->nCurrent;
			if (m_nType == 0)
				m_pDlg->SetDlgItemInt(m_id, n);
		}
		else if (vie2.pInfo->nType == ValueType_Text)
		{
			const char *pText = ((VALUEINFOTEXT *)vie2.pInfo)->pszBuffer;
			if (m_nType == 1)
				m_pDlg->SetDlgItemText(m_id, pText);
		}
	}
	return true;
}
