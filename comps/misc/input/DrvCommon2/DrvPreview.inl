#pragma once

#include "DrvPreview.h"

inline bool MakeDummyDIB(IDIBProvider *pDIBPrv, int cx, int cy, int bpp)
{
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)pDIBPrv->AllocMem(::DIBSize(cx, cy, bpp, true));
	if (lpbi)
		::InitDIB(lpbi, cx, cy, bpp);
	return lpbi!=NULL;
}

class CMaintainImage
{
public:
	IImage3 **m_ppimg;
	CMaintainImage(IImage3 *&pimg, IUnknown *punk)
	{
		m_ppimg = &pimg;
		IImage3 *pimg1 = NULL;
		punk->QueryInterface(IID_IImage3, (void **)&pimg1);
		*m_ppimg = pimg;
	}
	~CMaintainImage()
	{
		if (*m_ppimg)
			(*m_ppimg)->Release();
		(*m_ppimg) = NULL;
	}
};

class CMaintainLPBI
{
public:
	LPBITMAPINFOHEADER *m_ppbi;
	CMaintainLPBI(LPBITMAPINFOHEADER &pbi, LPBITMAPINFOHEADER pbiSet)
	{
		m_ppbi = &pbi;
		*m_ppbi = pbiSet;
	}
	~CMaintainLPBI()
	{
		*m_ppbi = NULL;
	}
};

inline void CDriverPreview::ReadSettings()
{
	m_bVertMirror  = CStdProfileManager::m_pMgr->GetProfileBool(_T("Image"), _T("VertMirror"), false, true, true);
	m_bHorzMirror  = CStdProfileManager::m_pMgr->GetProfileBool(_T("Image"), _T("HorzMirror"), false, true, true);
	m_bGray  = CStdProfileManager::m_pMgr->GetProfileBool(_T("Image"), _T("Gray"), false, true, true);
}

inline void CDriverPreview::PlanesReady16(LPWORD lpRed, LPWORD lpGreen, LPWORD lpBlue)
{
	bool bHorzMirror = m_bHorzMirror&&(GetExFlags()&EFG_HMIRROR)==0;
	bool bVertMirror = m_bVertMirror&&(GetExFlags()&EFG_VMIRROR)==0;
	if (m_pimgCap != 0)
	{
		if (m_biCapFmt.biBitCount == 24)
			AttachPlanes16ToRGB(m_pimgCap, m_biCapFmt.biWidth, m_biCapFmt.biHeight, lpRed,
				lpGreen, lpBlue, bHorzMirror, bVertMirror);
		else if (m_biCapFmt.biBitCount == 8)
		{
			if (m_nGrayPlane == 0)
				AttachPlane16ToGray(m_pimgCap, m_biCapFmt.biWidth, m_biCapFmt.biHeight, lpRed,
					bHorzMirror, bVertMirror);
			else if (m_nGrayPlane == 1)
				AttachPlane16ToGray(m_pimgCap, m_biCapFmt.biWidth, m_biCapFmt.biHeight, lpGreen,
					bHorzMirror, bVertMirror);
			else if (m_nGrayPlane == 2)
				AttachPlane16ToGray(m_pimgCap, m_biCapFmt.biWidth, m_biCapFmt.biHeight, lpBlue,
					bHorzMirror, bVertMirror);
			else
				AttachPlanes16ToGray(m_pimgCap, m_biCapFmt.biWidth, m_biCapFmt.biHeight, lpRed,
					lpGreen, lpBlue, bHorzMirror, bVertMirror);
		}
	}
	else
	{
		LPBITMAPINFOHEADER lpbi = NULL;
		if (m_lpbiCap != 0)
			lpbi = m_lpbiCap;
		else if (m_nGrabCount > 0)
			lpbi = m_Dib.m_lpbi;
		if (lpbi != NULL)
		{
			if (lpbi->biBitCount == 24)
				ConvertPlanes16ToDIB24(lpbi, lpRed, lpGreen, lpBlue, bHorzMirror, bVertMirror);
			else if (lpbi->biBitCount == 8)
			{
				if (m_nGrayPlane == 0)
					ConvertPlane16ToDIB8(lpbi, lpRed, bHorzMirror, bVertMirror);
				else if (m_nGrayPlane == 1)
					ConvertPlane16ToDIB8(lpbi, lpGreen, bHorzMirror, bVertMirror);
				else if (m_nGrayPlane == 2)
					ConvertPlane16ToDIB8(lpbi, lpBlue, bHorzMirror, bVertMirror);
				else
					ConvertPlanes16ToDIB8(lpbi, lpRed, lpGreen, lpBlue, bHorzMirror, bVertMirror);
			}
		}
	}
}

inline HRESULT CDriverPreview::InputNative(IUnknown *pUnkImage, IUnknown *punkTarget, int nDevice, BOOL bFromSettings)
{
	XStopPreview StopGrab(this, m_CamState==Succeeded&&m_nGrabCount>0&&!CanCaptureDuringPreview());
	if (!DoInitCamera())
	{
		MakeDummyImage(pUnkImage, 1280, 1024);
		return S_OK;
	}
	memset(&m_biCapFmt, 0, sizeof(m_biCapFmt));
	if (!GetFormat(true, &m_biCapFmt))
	{
		MakeDummyImage(pUnkImage, 1280, 1024);
		return S_OK;
	}
	CMaintainImage Maintain(m_pimgCap, pUnkImage);
	ProcessCapture();
	return S_OK;
}

inline HRESULT CDriverPreview::InputDIB(IDIBProvider *pDIBPrv, IUnknown *punkTarget, int nDevice, BOOL bFromSettings)
{
	XStopPreview StopGrab(this, m_CamState==Succeeded&&m_nGrabCount>0&&!CanCaptureDuringPreview());
	if (!DoInitCamera())
		return MakeDummyDIB(pDIBPrv, 1280, 1024, 24)?S_OK:E_FAIL;
	memset(&m_biCapFmt, 0, sizeof(m_biCapFmt));
	if (!GetFormat(true, &m_biCapFmt))
		return MakeDummyDIB(pDIBPrv, 1280, 1024, 24)?S_OK:E_FAIL;
	if (m_biCapFmt.biBitCount == 16)
		m_biCapFmt.biBitCount = 8;
	else if (m_biCapFmt.biBitCount > 24)
		m_biCapFmt.biBitCount = 24;
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)pDIBPrv->AllocMem(::DIBSize(m_biCapFmt.biWidth,
		m_biCapFmt.biHeight, m_biCapFmt.biBitCount, true));
	if (lpbi == NULL)
		return E_FAIL;
	::InitDIB(lpbi, m_biCapFmt.biWidth, m_biCapFmt.biHeight, m_biCapFmt.biBitCount);
	CMaintainLPBI Maintain(m_lpbiCap, lpbi);
	ProcessCapture();
	return S_OK;
}

inline HRESULT CDriverPreview::GetShortName(BSTR *pbstrShortName)
{
	CString strDriverName(OnGetShortName());
	*pbstrShortName = strDriverName.AllocSysString();
	return S_OK;
}

inline HRESULT CDriverPreview::GetLongName(BSTR *pbstrLongName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strDriverFullName;
	strDriverFullName.LoadString(OnGetLongNameId());
	*pbstrLongName = strDriverFullName.AllocSysString();
	return S_OK;
}

inline HRESULT CDriverPreview::GetValue(int nCurDev, BSTR bstrName, VARIANT *pValue)
{
	CString s(bstrName);
	*pValue = ((CVT5ProfileManager*)CStdProfileManager::m_pMgr)->GetProfileValue(NULL, s);
	return S_OK;
}

inline HRESULT CDriverPreview::SetValue(int nCurDev, BSTR bstrName, VARIANT Value)
{
	CString s(bstrName);
	((CVT5ProfileManager*)CStdProfileManager::m_pMgr)->WriteProfileValue(NULL, s, Value);
	return S_OK;
}

inline HRESULT CDriverPreview::GetDevicesCount(int *pnCount)
{
	*pnCount = 1;
	return S_OK;
}

inline HRESULT CDriverPreview::GetDeviceNames(int nDev, BSTR *pbstrShortName, BSTR *pbstrLongName, BSTR *pbstrCategory)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (nDev != 0) return E_INVALIDARG;
	CString strDriverName(OnGetShortName());
	CString strDriverFullName,strCategory;
	strDriverFullName.LoadString(OnGetLongNameId());
	strCategory.LoadString(OnGetInputId());
	*pbstrShortName = strDriverName.AllocSysString();
	*pbstrLongName = strDriverFullName.AllocSysString();
	*pbstrCategory = strCategory.AllocSysString();
	return S_OK;
}

inline bool CDriverPreview::DoInitCamera()
{
	if (m_CamState == NotInited)
	{
		ReadSettings();
		m_CamState = InitCamera()?Succeeded:Failed;
	}
	return m_CamState==Succeeded?true:false;
}


inline HRESULT CDriverPreview::StartImageAquisition(int, BOOL bStart)
{
	if (bStart)
	{
		m_nGrabCount++;
		if (m_nGrabCount==1)
		{
			DoInitCamera();
			if (m_CamState == Succeeded)
			{
				BITMAPINFOHEADER bi;
				GetFormat(false, &bi);
				m_Dib.InitBitmap(bi.biWidth, bi.biHeight, m_bGray?8:24);
				OnStartGrab();
			}
			else
				return E_FAIL;
		}
	}
	else
	{
		--m_nGrabCount;
		if (m_nGrabCount == 0 && m_CamState == Succeeded)
		{
			OnStopGrab();
		}
	}
	return S_OK;
}

inline HRESULT CDriverPreview::IsImageAquisitionStarted(int, BOOL *pbAquisition)
{
	*pbAquisition = m_nGrabCount > 0;
	return S_OK;
}

inline HRESULT CDriverPreview::AddDriverSite(int, IDriverSite *pSite)
{
	EnterCriticalSection(&m_csSites);
	bool bFound = false;
	for (int i = 0; i < m_arrSites.GetSize(); i++)
	{
		if (m_arrSites[i] == pSite)
		{
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		pSite->AddRef();
		m_arrSites.Add(pSite);
	}
	LeaveCriticalSection(&m_csSites);
	return S_OK;
}

inline HRESULT CDriverPreview::RemoveDriverSite(int, IDriverSite *pSite)
{
	EnterCriticalSection(&m_csSites);
	for (int i = 0; i < m_arrSites.GetSize(); i++)
	{
		if (m_arrSites[i] == pSite)
		{
			m_arrSites.RemoveAt(i);
			pSite->Release();
		}
	}
	LeaveCriticalSection(&m_csSites);
	return S_OK;
}

inline HRESULT CDriverPreview::DrawRect(int nDevice, HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst)
{
	if (m_CamState == Succeeded)
	{
	}
	else
		::FillRect(hDC, lpRectDst, (HBRUSH)::GetStockObject(BLACK_BRUSH));
	return S_OK;
}


