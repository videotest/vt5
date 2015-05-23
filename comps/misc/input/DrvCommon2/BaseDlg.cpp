// BaseDialog.cpp: implementation of the CBaseDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "BaseDlg.h"
#include "StdProfile.h"
#include "Ctrls.h"
#include "SmartEdit.h"
#include "BinResource.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CBaseDlg, CDialog);

static bool s_bChanging;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int CBaseDlg::m_idDefault = -1;
bool CBaseDlg::s_bIgnoreThumbTrack = false;

CBaseDlg::CBaseDlg(int idd, CWnd *pParent, CBaseDlg *pBase, IUnknown *punk) : CDialog(idd, pParent)
{
	m_bDelayedInitCtrllSite = false;
	m_pBase = pBase;
	m_bDisableAllCtrls = true;
	m_bInited = false;
	m_sptrDDS = punk;
	m_bVideo = false;
	m_idd = idd;
	m_bSubdlgControls = false;
}

CBaseDlg::~CBaseDlg()
{
	DestroyControls();
}


BEGIN_MESSAGE_MAP(CBaseDlg, CDialog)
	//{{AFX_MSG_MAP(CBaseDlg)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
END_MESSAGE_MAP()



class _CNewResource
{
public:
	LPVOID lpData;
	_CNewResource(HGLOBAL hTemplate, DWORD dwSize)
	{
		LPVOID lpSrc = LockResource(hTemplate);
		LPVOID lpDst = malloc(dwSize);
		memcpy(lpDst, lpSrc, dwSize);
		UnlockResource(hTemplate);
		lpData = lpDst;
		_DLGTEMPLATEEX *lpDlgTemplEx = (_DLGTEMPLATEEX*)lpDst;
		if (lpDlgTemplEx->signature != 0xFFFF)
		{
			LPDLGTEMPLATE lpDlgTempl = (LPDLGTEMPLATE)lpDlgTemplEx;
			lpDlgTempl->style &= ~WS_BORDER;
			lpDlgTempl->dwExtendedStyle &= ~WS_EX_CLIENTEDGE;
		}
		else
		{
			lpDlgTemplEx->style &= ~WS_BORDER;
			lpDlgTemplEx->exStyle &= ~WS_EX_CLIENTEDGE;
		}
	};
	~_CNewResource()
	{
		free(lpData);
	};
};

BOOL CBaseDlg::CreatePlain(int idd, CWnd *pParent)
{
	m_idd = idd;
	LPCTSTR lpszTemplateName = MAKEINTRESOURCE(idd);
	HINSTANCE hInst = AfxFindResourceHandle(lpszTemplateName, RT_DIALOG);
	HRSRC hResource = ::FindResource(hInst, lpszTemplateName, RT_DIALOG);
	DWORD dwSize = SizeofResource(hInst, hResource);
	HGLOBAL hTemplate = LoadResource(hInst, hResource);
	_CNewResource NewRes(hTemplate, dwSize);
	FreeResource(hTemplate);
	BOOL bResult = CreateIndirect((LPCDLGTEMPLATE)NewRes.lpData, pParent, 0, hInst);
	return bResult;
}


void CBaseDlg::InitControl(CDrvControl *pCtrl, IDrvControlSite *pSite)
{
	if (pCtrl->Init(this, m_sptrDrv))
	{
		m_arrCtrls.Add(pCtrl);
		if (pSite != NULL)
			pCtrl->Register(pSite);
	}
	else
		delete pCtrl;
}


void CBaseDlg::DestroyControls()
{
	for (int i = 0; i < m_arrCtrls.GetSize(); i++)
		delete m_arrCtrls[i];
	m_arrCtrls.RemoveAll();
}

CDrvControl *CBaseDlg::FindControl(int id)
{
	for (int i = 0; i < m_arrCtrls.GetSize(); i++)
	{
		CDrvControl *p = m_arrCtrls[i];
		if (p->IsHandler(id))
			return p;
	}
	return NULL;
}

void CBaseDlg::RemoveControl(int id)
{
	for (int i = 0; i < m_arrCtrls.GetSize(); i++)
	{
		CDrvControl *p = m_arrCtrls[i];
		if (p->IsHandler(id))
		{
			m_arrCtrls.RemoveAt(i);
			delete p;
			return;
		}
	}
}

class CWindowsGroup : public CArray<HWND,HWND&>
{
public:
	CWindowsGroup(HWND hwnd)
	{
		Init(hwnd);
	}
	void Init(HWND hwnd)
	{
		do
		{
			Add(hwnd);
			hwnd = ::GetWindow(hwnd, GW_HWNDNEXT);
			if (hwnd == NULL)
				break;
			DWORD dwStyle = ::GetWindowLong(hwnd, GWL_STYLE);
			if ((dwStyle&WS_GROUP) != 0)
				break;
		}
		while (1);
	}
	HWND FindWindow(LPCTSTR lpstrClassName, int nFirst = 0, int *pnRet = NULL)
	{
		for (int i = nFirst; i < GetSize(); i++)
		{
			HWND hwnd = GetAt(i);
			TCHAR szBuff[256];
			if (::GetClassName(hwnd, szBuff, 256) == 0)
				continue;
			if (_tcsicmp(szBuff, lpstrClassName) == 0)
			{
				if (pnRet) *pnRet = i;
				return GetAt(i);
			}
		}
		if (pnRet) *pnRet = -1;
		return NULL;
	}
	void Disable()
	{
		for (int i = 0; i < GetSize(); i++)
		{
			HWND hwnd = GetAt(i);
			::EnableWindow(hwnd, FALSE);
		}
	}
};

bool CBaseDlg::TryInitSmartEdit(CWindowsGroup& Group, VALUEINFOEX2 *pvie)
{
	int nSpin1, nSlider1;
	HWND hwndEdit = Group.FindWindow(_T("Edit"));
	HWND hwndSpin = Group.FindWindow(_T("msctls_updown32"), 0, &nSpin1);
	HWND hwndSlider = Group.FindWindow(_T("msctls_trackbar32"), 0, &nSlider1);
	HWND hwndSpin2 = Group.FindWindow(_T("msctls_updown32"), nSpin1+1);
	HWND hwndSlider2 = Group.FindWindow(_T("msctls_trackbar32"), nSlider1+1);
	if (hwndEdit != NULL)
	{
		if (pvie->pInfo->nType == ValueType_Int)
		{
			CReprType ReprType = pvie->pReprData->Type;
			VALUEINFO *pvi = (VALUEINFO *)pvie->pInfo;
			if (ReprType == IRepr_SmartSpanEdit)
			{
				if (pvie->pHdr->pszEntry != 0 || pvie->pHdr->pszSection != 0)
					InitControl(new CSmartSpanEdit(::GetDlgCtrlID(hwndEdit),
						::GetDlgCtrlID(hwndSpin), ::GetDlgCtrlID(hwndSlider),
						pvie->pHdr->pszEntry, pvie->pHdr->pszSection), pvie->pDrvCtrlSite);
				else
					InitControl(new CSmartSpanEdit(::GetDlgCtrlID(hwndEdit),
						::GetDlgCtrlID(hwndSpin), ::GetDlgCtrlID(hwndSlider),
						pvi->nMin, pvi->nMax, 
						pvi->nDefault, pvi->nCurrent,
						this), pvie->pDrvCtrlSite);
			}
			else if (ReprType == IRepr_SmartIntEdit2 && (hwndSpin2 != NULL || hwndSlider2 != NULL))
			{
				SMARTINTREPRDATA2 *pSIRepr2 = (SMARTINTREPRDATA2 *)pvie->pReprData;
				if (pvie->pHdr->pszEntry != 0 || pvie->pHdr->pszSection != 0)
					InitControl(new CSmartIntEdit2(::GetDlgCtrlID(hwndEdit),
						::GetDlgCtrlID(hwndSpin), ::GetDlgCtrlID(hwndSlider),
						::GetDlgCtrlID(hwndSpin2), ::GetDlgCtrlID(hwndSlider2),
						pvie->pHdr->pszSection, pvie->pHdr->pszEntry, pSIRepr2->nStep1,
						pSIRepr2->nStep2), pvie->pDrvCtrlSite);
				else
					InitControl(new CSmartIntEdit2(::GetDlgCtrlID(hwndEdit),
						::GetDlgCtrlID(hwndSpin), ::GetDlgCtrlID(hwndSlider),
						::GetDlgCtrlID(hwndSpin2), ::GetDlgCtrlID(hwndSlider2),
						pvi->nMin, pvi->nMax, 
						pvi->nDefault, pvi->nCurrent, pSIRepr2->nStep1,
						pSIRepr2->nStep2, this), pvie->pDrvCtrlSite);
			}
			else if (ReprType == IRepr_SmartIntEdit || ReprType == IRepr_SmartIntEdit2)
			{
				int nFlags = s_bIgnoreThumbTrack?CSmartIntEdit::ThumbPosition:0;
				if (pvie->pHdr->pszEntry != 0 || pvie->pHdr->pszSection != 0)
					InitControl(new CSmartIntEdit(::GetDlgCtrlID(hwndEdit),
						::GetDlgCtrlID(hwndSpin), ::GetDlgCtrlID(hwndSlider),
						pvie->pHdr->pszSection, pvie->pHdr->pszEntry, 0, nFlags),
						pvie->pDrvCtrlSite);
				else
					InitControl(new CSmartIntEdit(::GetDlgCtrlID(hwndEdit),
						::GetDlgCtrlID(hwndSpin), ::GetDlgCtrlID(hwndSlider),
						pvi->nMin, pvi->nMax, 
						pvi->nDefault, pvi->nCurrent,
						this, nFlags), pvie->pDrvCtrlSite);
			}
			return true;
		}
		else if (pvie->pInfo->nType == ValueType_Double)
		{
			CReprType ReprType = pvie->pReprData->Type;
			VALUEINFODOUBLE *pvi = (VALUEINFODOUBLE *)pvie->pInfo;
			if (ReprType == IRepr_LogDoubleEdit)
			{
					InitControl(new CSmartLogDoubleEdit(::GetDlgCtrlID(hwndEdit),
						::GetDlgCtrlID(hwndSpin), ::GetDlgCtrlID(hwndSlider),
						pvie->pHdr->pszSection, pvie->pHdr->pszEntry, pvi->dMin, pvi->dMax,
						pvi->dDefault), pvie->pDrvCtrlSite);
			}
		}
	}
	return false;
}

bool CBaseDlg::TryInitCombobox(CWindowsGroup& Group, VALUEINFOEX2 *pvie)
{
	HWND hwndCombo = Group.FindWindow("combobox");
	if (hwndCombo != 0)
	{
		InitControl(new CComboboxHandler(::GetDlgCtrlID(hwndCombo), pvie->pHdr->pszSection,
			pvie->pHdr->pszEntry, ((VALUEINFO *)pvie->pInfo)->nCurrent,
			pvie->pReprData->Type==IRepr_Combobox?(COMBOBOXREPRDATA*)pvie->pReprData:0),
			pvie->pDrvCtrlSite);
		return true;
	}
	return false;
}

bool CBaseDlg::TryInitListbox(CWindowsGroup& Group, VALUEINFOEX2 *pvie)
{
	HWND hwnd = Group.FindWindow("ListBox");
	if (hwnd != 0)
	{
		InitControl(new CListboxHandler(::GetDlgCtrlID(hwnd), pvie->pHdr->pszSection,
			pvie->pHdr->pszEntry, ((VALUEINFO *)pvie->pInfo)->nCurrent,
			pvie->pReprData->Type==IRepr_Combobox?(COMBOBOXREPRDATA*)pvie->pReprData:0),
			pvie->pDrvCtrlSite);
		return true;
	}
	return false;
}

bool CBaseDlg::TryInitButton(CWindowsGroup& Group, VALUEINFOEX2 *pvie)
{
	HWND hwnd = Group.FindWindow("button");
	DWORD dwStyle = ::GetWindowLong(hwnd, GWL_STYLE);
	if (hwnd != 0)
	{
		if ((dwStyle&BS_CHECKBOX) != 0)
		{
			InitControl(new CCheckboxHandler(::GetDlgCtrlID(hwnd), pvie->pHdr->pszSection,
				pvie->pHdr->pszEntry, FALSE), pvie->pDrvCtrlSite);
			return true;
		}
		if ((dwStyle&BS_RADIOBUTTON) != 0 || (dwStyle&BS_AUTORADIOBUTTON) != 0)
		{
			CArray<int,int> arrIds;
			for (int i = 0; i < Group.GetSize(); i++)
			{
				int nID = ::GetWindowLong(Group.GetAt(i), GWL_ID);
				arrIds.Add(nID);
			}
			InitControl(new CRadioHandler(arrIds.GetData(), arrIds.GetSize(), pvie->pHdr->pszSection,
				pvie->pHdr->pszEntry, 0), pvie->pDrvCtrlSite);
			return true;
		}
	}
	return false;
}

bool CBaseDlg::TryInitStatic(CWindowsGroup& Group, VALUEINFOEX2 *pvie)
{
	HWND hwnd = Group.FindWindow(_T("Static"));
	if (Group.GetSize() == 1 && hwnd != NULL)
	{
		if (pvie->pInfo->nType == ValueType_Int)
			InitControl(new CStaticHandler(::GetDlgCtrlID(hwnd), pvie->pHdr->pszSection,
				pvie->pHdr->pszEntry, ((VALUEINFO *)pvie->pInfo)->nCurrent), pvie->pDrvCtrlSite);
		else if (pvie->pInfo->nType == ValueType_Text)
			InitControl(new CStaticHandler(::GetDlgCtrlID(hwnd), pvie->pHdr->pszSection,
				pvie->pHdr->pszEntry, ((VALUEINFOTEXT *)pvie->pInfo)->pszBuffer),
				pvie->pDrvCtrlSite);
		return true;
	}
	return false;
}


void CBaseDlg::InitControlsState()
{
	if (m_sptrDDS != 0)
	{
		for (HWND hwnd = ::GetWindow(m_hWnd, GW_CHILD); hwnd != NULL; hwnd = ::GetWindow(hwnd, GW_HWNDNEXT))
		{
			DWORD dwStyle = ::GetWindowLong(hwnd, GWL_STYLE);
			if ((dwStyle&WS_GROUP) == 0) continue;
			int id = ::GetDlgCtrlID(hwnd);
			if (id <= 0 || IsOwnControl(hwnd, id) || m_pBase!=NULL && 
				m_pBase->IsOwnControl(hwnd,id)) continue;
			CWindowsGroup Group(hwnd);
			VALUEINFOEX2 vie2;
			HRESULT hr = m_sptrDDS->GetControlInfo(id, &vie2);
			if (hr == S_OK)
			{
				// 1. First try to initialize smart edit
				bool b = TryInitSmartEdit(Group, &vie2);
				if (!b)	b = TryInitCombobox(Group, &vie2);
				if (!b) b = TryInitListbox(Group, &vie2);
				if (!b)	b = TryInitButton(Group, &vie2);
				if (!b)	b = TryInitStatic(Group, &vie2);
			}
			else if (hr != S_FALSE)
				Group.Disable();
		}
	}
	m_bInited = true;
}

BOOL CBaseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (m_sptrDDS != 0)
		m_sptrDDS->OnInitDialog(this);
	if (!m_bDelayedInitCtrllSite) InitControlsState();
	if (m_pBase)
		m_pBase->OnInitChild(this);
	UpdateCmdUI();
	return TRUE;
}

void CBaseDlg::OnHScroll( UINT nSBCode, UINT nPos1, CScrollBar* pScrollBar)
{
	if (s_bIgnoreThumbTrack && nSBCode == SB_THUMBTRACK)
		return;
	if (s_bChanging) return;
	CLock lock(&s_bChanging);
	int nID = pScrollBar->GetDlgCtrlID();
	int nPos = ((CSliderCtrl*)pScrollBar)->GetPos();
	for (int i = 0; i < m_arrCtrls.GetSize(); i++)
		if (m_arrCtrls[i]->OnHScroll(pScrollBar->GetDlgCtrlID(), nSBCode))
			break;
}

void CBaseDlg::OnDefault()
{
	for (int i = 0; i < m_arrCtrls.GetSize(); i++)
	{
		CDrvControl *p = m_arrCtrls[i];
		p->Default();
	}
}

void CBaseDlg::OnOK()
{
	CWnd *pParent = GetParent();
	if (!pParent || !pParent->IsKindOf(RUNTIME_CLASS(CBaseDlg)))
		CDialog::OnOK();
}

void CBaseDlg::OnCancel()
{
	CWnd *pParent = GetParent();
	if (!pParent || !pParent->IsKindOf(RUNTIME_CLASS(CBaseDlg)))
		CDialog::OnCancel();
}

BOOL CBaseDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);
	if (nCode==EN_CHANGE)
	{
		for (int i = 0; i < m_arrCtrls.GetSize(); i++)
			if (m_arrCtrls[i]->OnEditChange(nID))
				return TRUE;
	}
	else
	{
		for (int i = 0; i < m_arrCtrls.GetSize(); i++)
			if (m_arrCtrls[i]->OnCmdMsg(nCode,nID,hWndCtrl))
			{
				if (nCode == BN_CLICKED)
					UpdateCmdUI();
				return TRUE;
			}
	}
	if (nID == m_idDefault && nCode == BN_CLICKED)
	{
		OnDefault();
		return TRUE;
	}
	if (m_sptrDDS != 0 && m_sptrDDS->OnCmdMsg(nCode,nID,(long)hWndCtrl)==S_OK)
	{
		UpdateCmdUI();
		return TRUE;
	}
	if (m_pBase && m_pBase->m_bSubdlgControls)
	{
		BOOL b = m_pBase->SendMessage(WM_COMMAND, wParam, lParam);
		if (b)
			return b;
	}
	return CDialog::OnCommand(wParam, lParam);
}

BOOL CBaseDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR* pNMHDR = (NMHDR*)lParam;
	HWND hWndCtrl = pNMHDR->hwndFrom;
	UINT_PTR nID = wParam;
	int nCode = pNMHDR->code;
	if (nCode == UDN_DELTAPOS)
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
		for (int i = 0; i < m_arrCtrls.GetSize(); i++)
			if (m_arrCtrls[i]->OnSpinDelta((int)nID, pNMUpDown->iDelta))
			{
				*pResult = 0;
				return TRUE;
			}
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CBaseDlg::OnResetSettings(LPCTSTR lpstrSec, LPCTSTR lpstrEntry)
{
	for (int i = 0; i < m_arrCtrls.GetSize(); i++)
	{
		CDrvControl *p = m_arrCtrls.GetAt(i);
		if ((lpstrSec == NULL || p->GetSectionName() == lpstrSec) &&
			(lpstrEntry == NULL || p->GetEntryName() == lpstrEntry))
		{
			p->Reset();
			break;
		}
	}
}



void CBaseDlg::OnChangeValue(CSmartIntEdit *pEdit, int nValue)
{
	if (m_sptrDDS != 0)
		m_sptrDDS->OnSetControlInt(pEdit->GetId(), nValue);
}

void CBaseDlg::UpdateCmdUI()
{
	if (m_pBase != NULL)
		m_pBase->UpdateCmdUI();
	if (m_sptrDDS != 0)
		for (HWND hwnd = ::GetWindow(m_hWnd, GW_CHILD); hwnd != NULL; hwnd = ::GetWindow(hwnd, GW_HWNDNEXT))
			m_sptrDDS->OnUpdateCmdUI(hwnd, ::GetWindowLong(hwnd, GWL_ID));
}


void CBaseDlg::OnDestroy()
{
	//sergey 29/09/06
	//SendMessage(WM_CLOSE,0,0);
	//end
	if (m_sptrDDS != 0)
		m_sptrDDS->OnCloseDialog(this);
	__super::OnDestroy();
}
