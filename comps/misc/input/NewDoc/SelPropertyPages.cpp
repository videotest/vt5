// SelPropertyPages.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SelPropertySheet.h"
#include "Common.h"
#include "Input.h"
#include "misc_utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAX_ACCUMULATE  10

bool FindDriverAndDeviceByShortName(CString sShortF, IUnknown **ppunkDrv, int *pnDev )
{
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		sptrIDriverManager sptrDM(punkCM);
		ICompManagerPtr sptrCM(punkCM);
		punkCM->Release();
		int nCount = 0;
		sptrDM->GetDevicesCount(&nCount);
		for (int i = 0; i < nCount; i++)
		{
			BSTR bstrShort;
			int nDrv;
			int nDev;
			sptrDM->GetDeviceInfo(i, &bstrShort, NULL, NULL, &nDrv, &nDev);
			CString sShort(bstrShort);
			::SysFreeString(bstrShort);
			if (sShort == sShortF)
			{
				sptrCM->GetComponentUnknownByIdx(nDrv, ppunkDrv);
				*pnDev = nDev;
				return true;
			}
		}
	}	
	return false;
}


IMPLEMENT_DYNCREATE(CCommonPropertyPage, CPropertyPage)
IMPLEMENT_DYNCREATE(CInputPropertyPage, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// CCommonPropertyPage property page

CCommonPropertyPage::CCommonPropertyPage() : CPropertyPage(CCommonPropertyPage::IDD)
{
	//{{AFX_DATA_INIT(CCommonPropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CCommonPropertyPage::~CCommonPropertyPage()
{
}

void CCommonPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommonPropertyPage)
	DDX_Control(pDX, IDC_CREATORS_LIST, m_CreatorsList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCommonPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCommonPropertyPage)
	ON_LBN_SELCHANGE(IDC_CREATORS_LIST, OnSelchangeCreatorsList)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()


void CCommonPropertyPage::OnHelp()
{
	HelpDisplayTopic("NewDoc",NULL,"CommonPropertyPage","$GLOBAL_main");
}

/////////////////////////////////////////////////////////////////////////////
// CInputPropertyPage property page

CInputPropertyPage::CInputPropertyPage() : CPropertyPage(CInputPropertyPage::IDD)
{
	//{{AFX_DATA_INIT(CInputPropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bDisableBack = false;
}

CInputPropertyPage::~CInputPropertyPage()
{
}

void CInputPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputPropertyPage)
	DDX_Control(pDX, IDC_USE_FRAME, m_UseFrame);
	DDX_Control(pDX, IDC_EDIT_INPUT_FRAME, m_EditFrame);
	DDX_Control(pDX, IDC_E_IMAGES_NUM, m_EImagesNum);
	DDX_Control(pDX, IDC_ACCUMULATION, m_Accumulate);
	DDX_Control(pDX, IDC_S_IMAGES_NUM, m_SImagesNum);
	DDX_Control(pDX, IDC_DRIVERS_LIST, m_DriverList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CInputPropertyPage)
	ON_LBN_SELCHANGE(IDC_DRIVERS_LIST, OnSelchangeDriversList)
	ON_BN_CLICKED(IDC_ACCUMULATION, OnAccumulation)
	ON_EN_CHANGE(IDC_E_IMAGES_NUM, OnChangeEImagesNum)
	ON_BN_CLICKED(IDC_EDIT_INPUT_FRAME, OnEditInputFrame)
	ON_BN_CLICKED(IDC_USE_FRAME, OnUseFrame)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()


void CInputPropertyPage::OnHelp()
{
	HelpDisplayTopic("NewDoc",NULL,"InputPropertyPage","$GLOBAL_main");
}


BOOL CCommonPropertyPage::OnSetActive() 
{
	m_saCategories.RemoveAll();
	m_saShortNames.RemoveAll();
	m_saLongNames.RemoveAll();
	m_CreatorsList.ResetContent();
	int nCurSel = 0;
	bool bCurDriver = false;
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		sptrIDriverManager sptrDM(punkCM);
		punkCM->Release();
		BSTR bstrCur;
		sptrDM->GetCurrentDeviceName(&bstrCur);
		CString sCur(bstrCur);
		::SysFreeString(bstrCur);
		int nCount = 0;
		sptrDM->GetDevicesCount(&nCount);
		for (int i = 0; i < nCount; i++)
		{
			BSTR bstrShort,bstrLong,bstrCat;
			sptrDM->GetDeviceInfo(i, &bstrShort, &bstrLong, &bstrCat, NULL, NULL);
			CString sShort(bstrShort);
			CString sLong(bstrLong);
			CString sCat(bstrCat);
			::SysFreeString(bstrShort);
			::SysFreeString(bstrLong);
			::SysFreeString(bstrCat);
			if (sCat == _T("CreationMethod"))
			{
				m_saLongNames.Add(sLong);
				m_saShortNames.Add(sShort);
				if (sShort == sCur)
				{
					bCurDriver = true;
					nCurSel = (int)m_saLongNames.GetSize()-1;
				}
			}
			else
			{
				bool bFound = false;
				for (int j = 0; j < m_saCategories.GetSize(); j++)
				{
					if (m_saCategories.GetAt(j) == sCat)
					{
						bFound = true;
						break;
					}
				}
				if (!bFound)
					m_saCategories.Add(sCat);
				if (sShort == sCur)
					nCurSel = (int)m_saCategories.GetSize() - 1;
			}
		}
	}	
	
	for (int i = 0; i < m_saCategories.GetSize(); i++)
		m_CreatorsList.AddString(m_saCategories.GetAt(i));
	for (i = 0; i < m_saLongNames.GetSize(); i++)
		m_CreatorsList.AddString(m_saLongNames.GetAt(i));
	if (bCurDriver)
		m_CreatorsList.SetCurSel(m_saCategories.GetSize()+nCurSel);
	else
		m_CreatorsList.SetCurSel(nCurSel);
	int n = (int)bCurDriver ? m_saCategories.GetSize() + nCurSel : nCurSel;
	if (n < m_saCategories.GetSize())
		((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_NEXT);
	else
		((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_FINISH);

	return CPropertyPage::OnSetActive();
}

LRESULT CCommonPropertyPage::OnWizardNext() 
{
	int nCurSel = m_CreatorsList.GetCurSel();
	ASSERT(nCurSel < m_saCategories.GetSize());
	CString sCreator;
	m_CreatorsList.GetText(nCurSel, sCreator);
	((CSelPropertySheet *)GetParent())->m_sCreator = sCreator;
	return CPropertyPage::OnWizardNext();
}

BOOL CCommonPropertyPage::OnWizardFinish() 
{
	int nCurSel = m_CreatorsList.GetCurSel();
	ASSERT(nCurSel >= m_saCategories.GetSize());
	CString sShort(m_saShortNames.GetAt(nCurSel-m_saCategories.GetSize()));
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		sptrIDriverManager sptrDM(punkCM);
		punkCM->Release();
		sptrDM->SetCurrentDeviceName(_bstr_t(sShort));
	}	
	
	return CPropertyPage::OnWizardFinish();
}

void CCommonPropertyPage::OnSelchangeCreatorsList() 
{
	int nCurSel = m_CreatorsList.GetCurSel();
	if (nCurSel < m_saCategories.GetSize())
		((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_NEXT);
	else
		((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_FINISH);
	
}


BOOL CInputPropertyPage::OnSetActive() 
{
	m_saShortNames.RemoveAll();
	m_saLongNames.RemoveAll();
	m_DriverList.ResetContent();
	CString sCreator = ((CSelPropertySheet *)GetParent())->m_sCreator;
	int nCurSel = 0;
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		sptrIDriverManager sptrDM(punkCM);
		punkCM->Release();
		BSTR bstrCur;
		sptrDM->GetCurrentDeviceName(&bstrCur);
		CString sCur(bstrCur);
		::SysFreeString(bstrCur);
		int nCount = 0;
		sptrDM->GetDevicesCount(&nCount);
		for (int i = 0; i < nCount; i++)
		{
			BSTR bstrShort,bstrLong,bstrCat;
			sptrDM->GetDeviceInfo(i, &bstrShort, &bstrLong, &bstrCat, NULL, NULL);
			CString sShort(bstrShort);
			CString sLong(bstrLong);
			CString sCat(bstrCat);
			::SysFreeString(bstrShort);
			::SysFreeString(bstrLong);
			::SysFreeString(bstrCat);
			if (sCat == sCreator)
			{
				m_saLongNames.Add(sLong);
				m_saShortNames.Add(sShort);
				if (sShort == sCur)
					nCurSel = (int)m_saShortNames.GetSize() - 1;
			}
		}
	}	
	
	for (int i = 0; i < m_saLongNames.GetSize(); i++)
		m_DriverList.AddString(m_saLongNames.GetAt(i));
	m_DriverList.SetCurSel(nCurSel);
	((CPropertySheet *)GetParent())->SetTitle(sCreator);
	DWORD dwBackFlags = m_bDisableBack?0:PSWIZB_BACK;
	if (m_saLongNames.GetSize() <= 0) // Disable all controls and FINISH button if no drivers installed
	{
		m_SImagesNum.EnableWindow(FALSE);
		m_EImagesNum.EnableWindow(FALSE);
		m_Accumulate.EnableWindow(FALSE);
		m_UseFrame.EnableWindow(FALSE);
		m_EditFrame.EnableWindow(FALSE);
		m_DriverList.EnableWindow(FALSE);
		((CPropertySheet *)GetParent())->SetWizardButtons(dwBackFlags|PSWIZB_DISABLEDFINISH);
	}
	else
		((CPropertySheet *)GetParent())->SetWizardButtons(dwBackFlags|PSWIZB_FINISH);
	UpdateControls();	
	
	return CPropertyPage::OnSetActive();
}

BOOL CInputPropertyPage::OnWizardFinish() 
{
	int nCurSel = m_DriverList.GetCurSel();
	if (nCurSel == -1) return FALSE;
	CString sShort(m_saShortNames.GetAt(nCurSel));
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		sptrIDriverManager sptrDM(punkCM);
		punkCM->Release();
		sptrDM->SetCurrentDeviceName(_bstr_t(sShort));
	}
	
	return CPropertyPage::OnWizardFinish();
}

void CInputPropertyPage::OnSelchangeDriversList() 
{
	int nCurSel = m_DriverList.GetCurSel();
	if (nCurSel == -1) return;
	CString sShort(m_saShortNames.GetAt(nCurSel));
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		sptrIDriverManager sptrDM(punkCM);
		punkCM->Release();
		sptrDM->SetCurrentDeviceName(_bstr_t(sShort));
	}	
	UpdateControls();	
}

DWORD GetDriverFlags(LPCTSTR lpShortName)
{
	DWORD dwFlags = 0;
	IUnknown *punkD;
	int nDev;
	bool b = FindDriverAndDeviceByShortName(lpShortName, &punkD, &nDev);
	if (b)
	{
		sptrIDriver sptrD(punkD);
		punkD->Release();
		sptrD->GetFlags(&dwFlags);
	}
	return dwFlags;
}

int GetDriverInt(LPCTSTR lpShortName, LPCTSTR lpEntry, int nDef, bool bSaveDefault = false)
{
	int nValue = nDef;
	IUnknown *punkD;
	int nDev;
	bool b = FindDriverAndDeviceByShortName(lpShortName, &punkD, &nDev);
	if (b)
	{
		sptrIDriver sptrD(punkD);
		punkD->Release();
		VARIANT var;
		::VariantInit(&var);
		_bstr_t bstrEntry(lpEntry);
		sptrD->GetValue(nDev, bstrEntry, &var);
		if (var.vt == VT_I4)
			nValue = var.lVal;
		else if (var.vt == VT_I2)
			nValue = var.iVal;
		else if (bSaveDefault)
		{
			VARIANT varS;
			varS.vt = VT_I4;
			varS.lVal = nDef;
			sptrD->SetValue(nDev, bstrEntry, varS);
		}
		::VariantClear(&var);
	}
	return nValue;
}

void WriteDriverInt(LPCTSTR lpShortName, LPCTSTR lpEntry, int nValue)
{
	IUnknown *punkD;
	int nDev;
	bool b = FindDriverAndDeviceByShortName(lpShortName, &punkD, &nDev);
	if (b)
	{
		sptrIDriver sptrD(punkD);
		punkD->Release();
		VARIANT var;
		var.vt = VT_I4;
		var.lVal = nValue;
		_bstr_t bstrEntry(lpEntry);
		sptrD->SetValue(nDev, bstrEntry, var);
	}
}

CIntIniValue g_DisableAccumulate(_T("\\Input"), _T("DisableAccumulation"), FALSE);

CIntIniValue g_DisableInputFrame(_T("\\Input"), _T("DisableInputFrame"), FALSE);

void CInputPropertyPage::UpdateControls()
{
	int nDrv = m_DriverList.GetCurSel();
	if (nDrv < 0) return;
	CString sCurDrvSht(m_saShortNames.GetAt(nDrv));
	DWORD dwFlags = GetDriverFlags(sCurDrvSht);
	BOOL bSupportsAccum = int(g_DisableAccumulate)==0&&(dwFlags&FG_ACCUMULATION)!=0;
	BOOL bSupportsFrame = int(g_DisableInputFrame)==0&&(dwFlags&FG_INPUTFRAME)!=0;
	BOOL bAccum = bSupportsAccum&&GetDriverInt(sCurDrvSht, _T("Accumulate"), 0);
	BOOL bUseFrame = bSupportsFrame&&GetDriverInt(sCurDrvSht, _T("UseInputFrame"), 0);
	BOOL bEditFrame = bSupportsFrame&&GetDriverInt(sCurDrvSht, _T("EditInputFrame"), 0);
	int nImagesNum = bAccum?GetDriverInt(sCurDrvSht, _T("AccumulateImagesNum"),2):1;
	if (bAccum && (nImagesNum < 2 || nImagesNum > MAX_ACCUMULATE))
		nImagesNum = 2;
	if (bAccum)
		m_SImagesNum.SetRange(2, MAX_ACCUMULATE);
	else
		m_SImagesNum.SetRange(1, MAX_ACCUMULATE);
	m_Accumulate.EnableWindow(bSupportsAccum);
	m_Accumulate.SetCheck(bAccum);
	m_SImagesNum.SetPos(nImagesNum);
	m_EImagesNum.EnableWindow(bSupportsAccum&&bAccum);
	m_SImagesNum.EnableWindow(bSupportsAccum&&bAccum);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_STATIC_IMAGES), bSupportsAccum&&bAccum);
	m_UseFrame.EnableWindow(bSupportsFrame);
	m_UseFrame.SetCheck(bUseFrame);
	m_EditFrame.EnableWindow(bSupportsFrame&&bUseFrame);
	m_EditFrame.SetCheck(bEditFrame);
}


void CInputPropertyPage::OnAccumulation() 
{
	int nDrv = m_DriverList.GetCurSel();
	if (nDrv < 0) return;
	CString sCurDrvSht(m_saShortNames.GetAt(nDrv));
	BOOL bAccum = m_Accumulate.GetCheck();
	WriteDriverInt(sCurDrvSht, _T("Accumulate"), bAccum);
	UpdateControls();
}

void CInputPropertyPage::OnChangeEImagesNum() 
{
	if (m_EImagesNum.m_hWnd)
	{
		int nDrv = m_DriverList.GetCurSel();
		if (nDrv < 0) return;
		CString sCurDrvSht(m_saShortNames.GetAt(nDrv));
		CString s;
		m_EImagesNum.GetWindowText(s);
		int nImg = _ttoi(s);
		WriteDriverInt(sCurDrvSht, _T("AccumulateImagesNum"), nImg);
	}
}

void CInputPropertyPage::OnEditInputFrame() 
{
	int nDrv = m_DriverList.GetCurSel();
	if (nDrv < 0) return;
	CString sCurDrvSht(m_saShortNames.GetAt(nDrv));
	BOOL bEditFrame = m_EditFrame.GetCheck();
	WriteDriverInt(sCurDrvSht, _T("EditInputFrame"), bEditFrame);
	UpdateControls();
}

void CInputPropertyPage::OnUseFrame() 
{
	int nDrv = m_DriverList.GetCurSel();
	if (nDrv < 0) return;
	CString sCurDrvSht(m_saShortNames.GetAt(nDrv));
	BOOL bUseFrame = m_UseFrame.GetCheck();
	WriteDriverInt(sCurDrvSht, _T("UseInputFrame"), bUseFrame);
	UpdateControls();
}
