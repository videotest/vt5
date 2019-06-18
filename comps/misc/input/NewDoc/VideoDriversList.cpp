// VideDriversList.cpp : implementation file
//

#include "stdafx.h"
#include "NewDoc.h"
#include "VideoDriversList.h"
#include "Common.h"
#include "Input.h"
#include "NewDocStdProfile.h"


/////////////////////////////////////////////////////////////////////////////
// CVideDriversList dialog


CVideDriversList::CVideDriversList(CWnd* pParent /*=NULL*/)
	: CDialog(CVideDriversList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVideDriversList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CVideDriversList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideDriversList)
	DDX_Control(pDX, IDC_LIST_DRIVERS, m_ListBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVideDriversList, CDialog)
	//{{AFX_MSG_MAP(CVideDriversList)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVideDriversList message handlers

BOOL CVideDriversList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString sDrvName = GetValueString(GetAppUnknown(false), "Input", "VideoDevice", NULL);
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		IDriverManagerPtr sptrDM(punkCM);
		ICompManagerPtr sptrCM(punkCM);
		punkCM->Release();
		int nDevices;
		int iCur = 0;
		HRESULT hr1 = sptrDM->GetDevicesCount(&nDevices);
		int nDrivers;
		HRESULT hr2 = sptrCM->GetCount(&nDrivers);
		if (FAILED(hr1) || FAILED(hr2))
			return FALSE;
		for (int i = 0; i < nDevices; i++)
		{
			BSTR bstrShort;
			BSTR bstrLong;
			BSTR bstrCat;
			int  nDriver;
			int  nDevInDriver;
			CString sShort;
			CString sLong;
			CString sCat;
			hr1 = sptrDM->GetDeviceInfo(i, &bstrShort, &bstrLong, &bstrCat, &nDriver, &nDevInDriver);
			sShort = bstrShort;
			sLong = bstrLong;
			sCat = bstrCat;
			::SysFreeString(bstrShort);
			::SysFreeString(bstrLong);
			::SysFreeString(bstrCat);
			IUnknown *punkDriver = NULL;
			sptrCM->GetComponentUnknownByIdx(nDriver, &punkDriver);
			if (punkDriver)
			{
				sptrIDriver sptrD(punkDriver);
				punkDriver->Release();
				DWORD dwFlags;
				sptrD->GetFlags(&dwFlags);
				if (dwFlags & FG_SUPPORTSVIDEO)
				{
					if (sDrvName == sShort)
						iCur = m_saShortNames.GetSize();
					m_saShortNames.Add(sShort);
					m_saLongNames.Add(sLong);
				}
			}
		}
		if (m_saLongNames.GetSize() == 0)
		{
			AfxMessageBox(IDS_NO_DRIVER_SUPPORTS_AVI, MB_OK|MB_ICONEXCLAMATION);
			EndDialog(IDCANCEL);
			return FALSE;
		}
		if (m_saLongNames.GetSize() == 1)
		{
			CString s = m_saShortNames.GetAt(0);
			SetValue(GetAppUnknown(false), "Input", "VideoDevice", s);
			EndDialog(IDOK);
			return FALSE;
		}
		for (i = 0; i < m_saLongNames.GetSize(); i++)
		{
			m_ListBox.AddString(m_saLongNames.GetAt(i));
		}
		m_ListBox.SetCurSel(iCur);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVideDriversList::OnOK() 
{
	int iCur = m_ListBox.GetCurSel();
	if (iCur != -1)
	{
		CString s = m_saShortNames.GetAt(iCur);
		SetValue(GetAppUnknown(false), "Input", "VideoDevice", s);
	}
	
	CDialog::OnOK();
}

void CVideDriversList::OnHelp() 
{
	HelpDisplay( "VideDriversList" );
}
