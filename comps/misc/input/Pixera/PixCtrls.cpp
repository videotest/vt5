// PixCtrls.cpp : implementation file
//

#include "stdafx.h"
#include "pixera.h"
#include "PixCtrls.h"
#include "PixSDK.h"
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
	{IDC_EDIT_EXP_TIME, RUNTIME_CLASS(CTimeAdjustEdit)},
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
