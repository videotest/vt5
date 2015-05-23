// Ctrls.cpp : implementation file
//

#include "stdafx.h"
#include "Ctrls.h"
#include "debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct ControlInfo
{
	int nId;
	CRuntimeClass *pRuntimeClass;
};

ControlInfo aControls[] = 
{
	{0, 0}
};

CWnd *CreateControlForDialog(HWND hwndDlg, int nId, HWND hwndCtrl)
{
	for (int i = 0; aControls[i].nId; i++)
	{
		if (aControls[i].nId == nId)
		{
			CWnd *pCtrl = (CWnd *)aControls[i].pRuntimeClass->CreateObject();
			if (pCtrl->SubclassWindow(hwndCtrl))
			{
				pCtrl->SendMessage(WM_APP);
				return pCtrl;
			}
			dprintf("Error sublassing %d\n", nId);
			delete pCtrl;
			return 0;
		}
	}
	return NULL;
}

void InitSlider(HWND hWnd, int id, int nMin, int nMax, int nPos, int nTicFreq, BOOL bEnable)
{
	CWnd *pWnd = CWnd::FromHandle(hWnd);
	CSliderCtrl *pSlider = (CSliderCtrl *)pWnd->GetDlgItem(id);
	if (pSlider)
	{
		pSlider->SetRange(nMin, nMax);
		pSlider->SetPos(nPos);
		pSlider->SetTicFreq(nTicFreq);
		pSlider->EnableWindow(bEnable);
	}
}

void InitSpin(HWND hWnd, int id, int nMin, int nMax, int nPos, BOOL bEnable)
{
	CWnd *pWnd = CWnd::FromHandle(hWnd);
	CSpinButtonCtrl *pSpin = (CSpinButtonCtrl *)pWnd->GetDlgItem(id);
	if (pSpin)
	{
		pSpin->SetRange32(nMin, nMax);
		pSpin->SetPos(nPos);
		pSpin->EnableWindow(bEnable);
	}
}

void SetSliderPos(HWND hWnd, int id, int nPos)
{
	CWnd *pWnd = CWnd::FromHandle(hWnd);
	CSliderCtrl *pSlider = (CSliderCtrl *)pWnd->GetDlgItem(id);
	if (pSlider)
		pSlider->SetPos(nPos);
}

int  GetSliderPos(HWND hWnd, int id)
{
	CWnd *pWnd = CWnd::FromHandle(hWnd);
	CSliderCtrl *pSlider = (CSliderCtrl *)pWnd->GetDlgItem(id);
	if (pSlider)
		return pSlider->GetPos();
	else
		return 0;
}

void SetSpinPos(HWND hWnd, int id, int nPos)
{
	CWnd *pWnd = CWnd::FromHandle(hWnd);
	CSpinButtonCtrl *pSpin = (CSpinButtonCtrl *)pWnd->GetDlgItem(id);
	if (pSpin)
		pSpin->SetPos(nPos);
}

void InitSmartEdit(HWND hWnd, int idSlider, int idSpin, int idEdit, int nMin, int nMax, int nPos, int nTicFreq, BOOL bEnable)
{
	InitSlider(hWnd, idSlider, nMin, nMax, nPos, nTicFreq, bEnable);
	InitSpin(hWnd, idSpin, nMin, nMax, nPos, bEnable);
	SetDlgItemInt(hWnd, idEdit, nPos, TRUE);
}

bool IsSmartEdit(HWND hWnd, int idSlider, int idEdit)
{
	if (idSlider != -1 && ::GetDlgItem(hWnd, idSlider) != 0)
		return true;
	if (idEdit != -1 && ::GetDlgItem(hWnd, idEdit) != 0)
		return true;
	return false;
}

static void _InitComboBoxByReprData(CComboBox *pCombo, COMBOBOXREPRDATA *pReprData)
{
	pCombo->ResetContent();
	if (pReprData->ComboType == CBox_Integer)
	{
		for (int i = 0; i < pReprData->nValues; i++)
		{
			if (pReprData->pnValues[i].lpstrValue)
				pCombo->AddString(pReprData->pnValues[i].lpstrValue);
			else if (pReprData->pnValues[i].idString > 0)
			{
				CString s;
				s.LoadString(pReprData->pnValues[i].idString);
				pCombo->AddString(s);
			}
		}
	}
}

void InitComboBox(HWND hWnd, int id, int nCurSel, COMBOBOXREPRDATA *pReprData)
{
	CWnd *pWnd = CWnd::FromHandle(hWnd);
	CComboBox *pCombo = (CComboBox *)pWnd->GetDlgItem(id);
	if (pCombo)
	{
		if (pReprData)
			_InitComboBoxByReprData(pCombo, pReprData);
		pCombo->SetCurSel(nCurSel);
	}
}

static void _InitListBoxByReprData(CListBox *pLB, COMBOBOXREPRDATA *pReprData)
{
	pLB->ResetContent();
	if (pReprData->ComboType == CBox_Integer)
	{
		for (int i = 0; i < pReprData->nValues; i++)
		{
			if (pReprData->pnValues[i].lpstrValue)
				pLB->AddString(pReprData->pnValues[i].lpstrValue);
			else if (pReprData->pnValues[i].idString > 0)
			{
				CString s;
				s.LoadString(pReprData->pnValues[i].idString);
				pLB->AddString(s);
			}
		}
	}
}

void InitListBox(HWND hWnd, int id, int nCurSel, COMBOBOXREPRDATA *pReprData)
{
	CWnd *pWnd = CWnd::FromHandle(hWnd);
	CListBox *pLB = (CListBox *)pWnd->GetDlgItem(id);
	if (pLB)
	{
		if (pReprData)
			_InitListBoxByReprData(pLB, pReprData);
		pLB->SetCurSel(nCurSel);
	}
}

void InitComboBoxValue(HWND hWnd, int id, int nValue)
{
	CWnd *pWnd = CWnd::FromHandle(hWnd);
	CComboBox *pCombo = (CComboBox *)pWnd->GetDlgItem(id);
	if (!pCombo) return;
	TCHAR szBuff[20];
	_itot(nValue, szBuff, 10);
	int nCurSel = pCombo->FindStringExact(-1, szBuff);
	pCombo->SetCurSel(nCurSel);
}

int GetListBoxValue(HWND hWnd, int id)
{
	CWnd *pWnd = CWnd::FromHandle(hWnd);
	CListBox *pLB = (CListBox *)pWnd->GetDlgItem(id);
	if (pLB)
		return pLB->GetCurSel();
	else
		return 0;
}

int GetComboBoxValue(HWND hWnd, int id)
{
	CWnd *pWnd = CWnd::FromHandle(hWnd);
	CComboBox *pCombo = (CComboBox *)pWnd->GetDlgItem(id);
	if (pCombo)
		return pCombo->GetCurSel();
	else
		return -1;
}

void ResetComboBox(HWND hWnd, int id)
{
	CWnd *pWnd = CWnd::FromHandle(hWnd);
	CComboBox *pCombo = (CComboBox *)pWnd->GetDlgItem(id);
	if (pCombo)
		pCombo->ResetContent();
}

void AddComboBoxString(HWND hWnd, int id, LPCTSTR lpsz)
{
	CWnd *pWnd = CWnd::FromHandle(hWnd);
	CComboBox *pCombo = (CComboBox *)pWnd->GetDlgItem(id);
	if (pCombo)
		pCombo->AddString(lpsz);
}

void AddComboBoxString(HWND hwnd, int idCtrl, int nIdStr)
{
	CString s;
	s.LoadString(nIdStr);
	AddComboBoxString(hwnd, idCtrl, s);
}

void SetDlgItemDouble(HWND hWnd, int nID, double dVal, int nDigits)
{
	CString s;
	if ((double)(int)dVal == dVal)
		s.Format("%.0f", dVal);
	else
	{
		CString sFmt;
		sFmt.Format(_T("%%.%df"), nDigits);
		s.Format(sFmt, dVal);
	}
	::SetDlgItemText(hWnd, nID, s);
}

double GetDlgItemDouble(HWND hWnd, int nID)
{
	TCHAR szBuff[50];
	::GetDlgItemText(hWnd, nID, szBuff, 50);
	return _tcstod(szBuff, NULL);
}



#if 0

/////////////////////////////////////////////////////////////////////////////
// CTimeAdjustEdit

IMPLEMENT_DYNCREATE(CTimeAdjustEdit, CEdit);

CTimeAdjustEdit::CTimeAdjustEdit()
{
}

CTimeAdjustEdit::~CTimeAdjustEdit()
{
}


BEGIN_MESSAGE_MAP(CTimeAdjustEdit, CEdit)
	//{{AFX_MSG_MAP(CTimeAdjustEdit)
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_APP, OnInit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimeAdjustEdit message handlers

void CTimeAdjustEdit::OnChange() 
{
	CString s;
	GetWindowText(s);
	AutoExposureModeEx Mode = CAM_AE_GetMode();
	if (Mode == kManualExposure)
	{
		if (!s.IsEmpty())
		{
			int n = 0;
			while(_istdigit(s[n++]));
			if (n > 0)
			{
				CString sVal = s.Left(n);
				CString sUnit = s.Mid(n);
				CString sMs,sMks;
				sMs.LoadString(IDS_MS);
				sMks.LoadString(IDS_MKS);
				int nVal = _ttoi(sVal);
				if (!s.CompareNoCase(sMs))
					nVal *= 1000;
				CAM_EXP_SetSpeed(double(nVal)/100.);
			}
		}
	}
}

static LPCTSTR aszNames[13] =
{_T("-2"), _T("-1 2/3"), _T("-1 1/3"), _T("-1"), _T("-2/3"), _T("-1/3"), _T("0"),
_T("1/3"), _T("2/3"), _T("1"), _T("1 1/3"), _T("1 2/3"), _T("2")};

LRESULT CTimeAdjustEdit::OnInit(WPARAM wParam, LPARAM lParam)
{
	if (wParam == (WPARAM)m_hWnd) return 0L;
	AutoExposureModeEx Mode = CAM_AE_GetMode();
	if (Mode == kManualExposure)
	{
		EnableWindow();
		double d;
		CAM_EXP_GetSpeed(&d);
		d *= 100.;
		CString s;
		CString sMs,sMks;
		sMs.LoadString(IDS_MS);
		sMks.LoadString(IDS_MKS);
		if (d > 1000.)
			s.Format(_T("%d%s"), int(d/1000.), (LPCTSTR)sMs);
		else
			s.Format(_T("%d%s"), int(d), (LPCTSTR)sMks);
		SetWindowText(s);
	}
	else
	{
		EnableWindow(FALSE);
		int n = CAM_AE_GetAdjust();
		if (n >= -6 && n <= 6)
			SetWindowText(aszNames[n+6]);
	}
	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
// CAutoExposeButton

CAutoExposeButton::CAutoExposeButton()
{
}

CAutoExposeButton::~CAutoExposeButton()
{
}


BEGIN_MESSAGE_MAP(CAutoExposeButton, CButton)
	//{{AFX_MSG_MAP(CAutoExposeButton)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_APP, OnInit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoExposeButton message handlers

void CAutoExposeButton::OnClicked() 
{
	if (GetCheck()==1)
		CAM_AE_Start();
	else
		CAM_AE_Stop();
}

LRESULT CAutoExposeButton::OnInit(WPARAM wParam, LPARAM lParam)
{
	if (wParam == (WPARAM)m_hWnd) return 0L;
	AutoExposureModeEx Mode = CAM_AE_GetMode();
	SetCheck(Mode==kManualExposure?0:1);
	return 0L;
}
#endif