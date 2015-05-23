// DriverFromAVI.cpp: implementation of the CDriverFromAVI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "avidocman.h"
#include "DriverFromAVI.h"
#include "VT5AVI.h"
#include "VT5Profile.h"
#include "InputUtils.h"
#include "VideDriversList.h"



IMPLEMENT_DYNCREATE(CDriverFromAVI, CDriverBase)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDriverFromAVI::CDriverFromAVI()
{
	EnableAutomation();
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	_OleLockApp( this );
	m_strDriverName = "FromVideo";
	m_strDriverFullName = "From video file";
	m_bFirst = true;
}

CDriverFromAVI::~CDriverFromAVI()
{
	_OleUnlockApp( this );
}


BEGIN_MESSAGE_MAP(CDriverFromAVI, CDriverBase)
	//{{AFX_MSG_MAP(CDriverEmpty)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDriverFromAVI, CDriverBase)
	//{{AFX_DISPATCH_MAP(CDriverEmpty)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IDriverEmpty to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {502C0CE7-F984-4890-976E-094256A08C16}
static const IID IID_IDriverFromAVI = 
{ 0x502c0ce7, 0xf984, 0x4890, { 0x97, 0x6e, 0x9, 0x42, 0x56, 0xa0, 0x8c, 0x16 } };

BEGIN_INTERFACE_MAP(CDriverFromAVI, CDriverBase)
	INTERFACE_PART(CDriverFromAVI, IID_IDriverFromAVI, Dispatch)
//	INTERFACE_PART(CDriverFromAVI, IID_IInputPreview, InpPrv)
END_INTERFACE_MAP()

// {462CA19F-41F4-4D7E-8172-AB43E51A2789}
GUARD_IMPLEMENT_OLECREATE(CDriverFromAVI, "IDriver.DriverFromVideo", 
0x462ca19f, 0x41f4, 0x4d7e, 0x81, 0x72, 0xab, 0x43, 0xe5, 0x1a, 0x27, 0x89);

static IUnknown *_FindVideoWindow(bool bByKey, GuidKey lActiveDocKey, bool bByVideoKey, GuidKey lVideoDocKey)
{
	sptrIApplication sptrA(GetAppUnknown());
	IUnknown *punkVideoDoc = NULL;
	CString s1,s2;
	s1.LoadString(IDS_TEMPLATE);
	AfxExtractSubString(s2, s1, CDocTemplate::docName);
	long lPosTemplate,lPosTemplFnd = 0;
	HRESULT hr = sptrA->GetFirstDocTemplPosition(&lPosTemplate);
	while (SUCCEEDED(hr) && lPosTemplate)
	{
		IUnknown *punkTemplate;
		BSTR bstrName;
		long lPrevTempl = lPosTemplate;
		hr = sptrA->GetNextDocTempl(&lPosTemplate, &bstrName, &punkTemplate);
		CString s(bstrName);
		::SysFreeString(bstrName);
		if (punkTemplate)
			punkTemplate->Release();
		if (s2 == s)
		{
			lPosTemplFnd = lPrevTempl;
			break;
		}
	}
	if (lPosTemplFnd)
	{
		long lPosDoc;
		hr = sptrA->GetFirstDocPosition(lPosTemplFnd, &lPosDoc);
		while (SUCCEEDED(hr) && lPosDoc)
		{
			IUnknown *punkDoc;
			hr = sptrA->GetNextDoc(lPosTemplFnd, &lPosDoc, &punkDoc);
			if (SUCCEEDED(hr) && punkDoc)
			{
				sptrIDocumentSite sptrDS(punkDoc);
				INumeredObjectPtr sptrNO(punkDoc);
				punkDoc->Release();
				if (bByVideoKey)
				{
					GuidKey	lKey;
					sptrNO->GetKey(&lKey);
					if (lKey != lVideoDocKey)
						continue;
				}
				IUnknown *punkView = NULL;
				hr = sptrDS->GetActiveView(&punkView);
				if (SUCCEEDED(hr) && punkView)
				{
					if (!bByKey)
						return punkView;
					sptrIVideoView sptrVV(punkView);
					BOOL bDocKeyInited = FALSE;
					GuidKey lSavedDocKey;
					sptrVV->GetImageDocKey(&lSavedDocKey, &bDocKeyInited);
					if (bDocKeyInited && lSavedDocKey == lActiveDocKey)
						return punkView;
					else
						punkView->Release();
				}

			}
		}
	}
	return NULL;
}

static bool __FindVideoFile(GuidKey lActiveDocKey)
{
	IUnknown *punk = _FindVideoWindow(true, lActiveDocKey, false, INVALID_KEY );
	if (punk)
	{
		punk->Release();
		return true;
	}
	else
		return false;
}

IDriver *__FindDriverForAVIInput(int nMode, int &nDevice)
{
	if (nMode != 0)
	{
		CVideDriversList dlg;
		if (dlg.DoModal() != IDOK)
			return FALSE;
	}
	CString sDrvName = CStdProfileManager::m_pMgr->_GetProfileString(NULL, _T("VideoDrvName"), NULL);
	CString sDrv1stShort;
	sptrIDriver sptrD1st;
	int nDevInDrv1st;
	sptrIDriver sptrDCur;
	int nDevInDrvCur;
	// Find driver, supporting input of video files.
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		IDriverManagerPtr sptrDM(punkCM);
		ICompManagerPtr sptrCM(punkCM);
		punkCM->Release();
		int nDevices;
		HRESULT hr1 = sptrDM->GetDevicesCount(&nDevices);
		int nDrivers;
		HRESULT hr2 = sptrCM->GetCount(&nDrivers);
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
//			if (sCat != _T("Input")) continue;
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
					if (!bool(sptrD1st))
					{
						sDrv1stShort = sShort;
						sptrD1st = sptrD;
						nDevInDrv1st = nDevInDriver;
					}
					if (!sDrvName.IsEmpty() && sShort == sDrvName)
					{
						sptrDCur = sptrD;
						nDevInDrvCur = nDevInDriver;
					}
				}
			}
		}
	}
	// If current driver uninitialized or absent, then use first found driver.
	if (!bool(sptrDCur))
	{
		if (!bool(sptrD1st))
		{
			AfxMessageBox(IDS_NO_DRIVER_SUPPORTS_AVI, MB_OK|MB_ICONEXCLAMATION);
			return NULL;
		}
		else
		{
			CStdProfileManager::m_pMgr->WriteProfileString(NULL, _T("VideoDrvName"), sDrv1stShort);
			nDevice = nDevInDrv1st;
			return sptrD1st.Detach();
		}
	}
	else
	{
		nDevice = nDevInDrvCur;
		return sptrDCur.Detach();
	}
	return NULL;
}

static bool __OpenOrCreateVideoFile(HWND hwndParent, int nMode, GuidKey &lDocKey)
{
	BOOL bCreateNew = FALSE;
	if (nMode == 0)
		bCreateNew = CStdProfileManager::m_pMgr->GetProfileInt(NULL, _T("CreateNew"), FALSE, TRUE);
	else
	{
		CString sTitle,sCaption;
		sTitle.LoadString(IDS_CREATE_NEW);
		sCaption.LoadString(IDS_CAPTION_CREATE_NEW);
		int r = ::MessageBox(hwndParent, sTitle, sCaption, MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION);
		bCreateNew = r==IDYES;
		CStdProfileManager::m_pMgr->WriteProfileInt(NULL, _T("CreateNew"), bCreateNew);
	}
	if (bCreateNew)
	{
		int nDevice;
		IDriver *pDrv = __FindDriverForAVIInput(nMode, nDevice);
		if (pDrv)
		{
			BSTR bstrVideoFileName = NULL;
			HRESULT hr = pDrv->InputVideoFile(&bstrVideoFileName, nDevice, nMode);
			CString sVideoFileName(bstrVideoFileName);
			::SysFreeString(bstrVideoFileName);
			pDrv->Release();
			if (hr == S_OK && !sVideoFileName.IsEmpty())
			{
				IUnknown *punkAM = _GetOtherComponent( GetAppUnknown(), IID_IActionManager );
				sptrIActionManager	sptrAM(punkAM);
				punkAM->Release();
				CString sParams = CString(_T("\""))+sVideoFileName+_T("\"");
				DoExecute(sptrAM, _T("FileOpen"), sParams);
				IUnknown *punkDoc = __FindDocByName(sVideoFileName, _T("Video"));
				if (punkDoc)
				{
					INumeredObjectPtr sptrNO(punkDoc);
					punkDoc->Release();
					sptrNO->GetKey(&lDocKey);
					return true;
				}
			}
		}
		return false;
	}
	else
	{
		CArray< GUID, GUID&>	uiaFoundDocs;
		__EnlistAllDocs(uiaFoundDocs,_T("Video"));
		IUnknown *punkAM = _GetOtherComponent( GetAppUnknown(), IID_IActionManager );
		sptrIActionManager	sptrAM(punkAM);
		punkAM->Release();
		DoExecute(sptrAM, _T("FileOpen"), _T("\"\", \"Video\""));
		IUnknown *punkDoc = __FindNewDocs(uiaFoundDocs,lDocKey,_T("Video"));
		if (punkDoc)
		{
			INumeredObjectPtr sptrNO(punkDoc);
			punkDoc->Release();
			sptrNO->GetKey(&lDocKey);
		}
	}
	return true;
}


void CDriverFromAVI::OnInputImage(IUnknown *punkImage, IUnknown *punkTarget, int nDevice, BOOL bDialog)
{
	GuidKey lActiveDocKey;
	INumeredObjectPtr sptrNO(punkTarget);
	sptrNO->GetKey(&lActiveDocKey);
	IUnknown *punkVideoVw = _FindVideoWindow(!m_bFirst, lActiveDocKey, m_bFirst, m_lDocKey);
	if (punkVideoVw)
	{
		sptrIVideoView sptrVV(punkVideoVw);
		punkVideoVw->Release();
		sptrVV->Capture(punkImage, false);
		sptrVV->NextFrame();
		if (m_bFirst)
			sptrVV->SetImageDocKey(lActiveDocKey);
	}
}

void CDriverFromAVI::OnGetSize(short &cx, short &cy)
{
	cx = cy = 0;
}


void CDriverFromAVI::OnDrawRect(HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst)
{
	HBRUSH hbr = ::CreateSolidBrush(RGB(0,64,0));
	::FillRect(hDC, lpRectDst, hbr);
	::DeleteObject(hbr);
}

void CDriverFromAVI::OnSetValue(int nDev, LPCTSTR lpName, VARIANT var)
{
	CDriverBase::OnSetValue(nDev, lpName, var);
}

bool CDriverFromAVI::OnExecuteSettings(HWND hwndParent, IUnknown *punkTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput)
{
	m_bFirst = true;
	sptrIDocumentSite sptrDS(punkTarget);
	if (bool(sptrDS) && !bFirst)
	{
		INumeredObjectPtr sptrNO(punkTarget);
		GuidKey lDocKey;
		sptrNO->GetKey(&lDocKey);
		if (__FindVideoFile(lDocKey))
		{
			m_bFirst = false;
			return true;
		}
	}
	return __OpenOrCreateVideoFile(hwndParent, nMode, m_lDocKey);
}


/////////////////////////////////////////////////////////////////////////////
// CDriverEmpty message handlers





