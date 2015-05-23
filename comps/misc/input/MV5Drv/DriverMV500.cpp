// DriverMV500.cpp : implementation file
//

#include "stdafx.h"
#include "VT5Profile.h"
#include "MV5Drv.h"
#include "MV500VT5Grab.h"
#include "DriverMV500.h"
#include "PreviewDialog.h"
#include "SourceDlg.h"
#include "FormatDlg.h"
#include "Accumulate.h"
#include "Image5.h"
#include "ImageUtil.h"
#include <vfw.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DRAW_REAL

class CDibDraw
{
public:
	HDRAWDIB	hdd;

	CDibDraw()
	{ hdd = ::DrawDibOpen();}
	~CDibDraw()
	{ DrawDibClose( hdd );}
};

CDibDraw	hdd;

static BSTR __AllocBstr(int id)
{
	CString s;
	s.LoadString(id);
	return s.AllocSysString();
}

static BSTR __AllocBstr(LPCTSTR lpsz)
{
	CString s(lpsz);
	return s.AllocSysString();
}

CDriverMV500::CDriverMV500()
{
	m_nGrabCount = 0;
	CMV500Grab::s_pGrab->Preinit();
	m_nPreviewMode = Preview_Fast;
	m_pBufDraw = NULL;
	m_dwDrawBufferSize = 0;
	m_bUseDrawBuffer = true;
	CMV500Grab::s_pGrab->m_pGrabSite = this;
}

CDriverMV500::~CDriverMV500()
{
	if (m_pBufDraw)
		delete m_pBufDraw;
	CMV500Grab::s_pGrab->m_pGrabSite = NULL;
}


IUnknown *CDriverMV500::DoGetInterface( const IID &iid )
{
	if (iid == IID_IDriver || iid == IID_IDriver2 || iid == IID_IDriver3 || iid == IID_IDriver4 ||
		iid == IID_IDriver5 || iid == IID_IDriver6)
		return (IDriver6 *)this;
	else if (iid == IID_IInputPreview || iid == IID_IInputPreview2)
		return (IInputPreview2 *)this;
	else
		return ComObjectBase::DoGetInterface(iid);
}

void CDriverMV500::DoInputNative(IUnknown *pUnkImg, int nDevice, BOOL bFromSettings)
{
	BOOL bAccum = ((CVT5ProfileManager *)CStdProfileManager::m_pMgr)->GetProfileInt(_T("Settings"), _T("Accumulate"), FALSE);
	if (bAccum)
	{
		int nImagesNum = ((CVT5ProfileManager *)CStdProfileManager::m_pMgr)->GetProfileInt(_T("Settings"), _T("AccumulateImagesNum"), FALSE);
		CAccumulate Accum;
		LPBITMAPINFOHEADER lpbi;
		for (int i = 0; i < nImagesNum; i++)
		{
			lpbi = CMV500Grab::s_pGrab->Grab(nDevice,i>0);
			Accum.Add(lpbi);
			free(lpbi);
		}
		unsigned nSize = Accum.CalcSize();
		lpbi = (LPBITMAPINFOHEADER)malloc(nSize);
		Accum.GetResult(lpbi, true);
		IImage3Ptr img(pUnkImg);
		LPBYTE pData = (LPBYTE)(((RGBQUAD *)(lpbi+1))+lpbi->biClrUsed);
		AttachDIBBits(img, lpbi, pData);
		free(lpbi);
	}
	else
		((CMV500VT5Grab *)CMV500Grab::s_pGrab)->Grab(nDevice,pUnkImg,false);
}

// There are 3 variants:
// 1. No grab in process
// 2. There are grab in process, but it is in draw mode and incompatible with image capture.
// 3. There are grab in process and it is compatible with image capture.
HRESULT CDriverMV500::InputNative(IUnknown *pUnkImg, IUnknown *punkTarget, int nDevice, BOOL bFromSettings)
{
	if (m_nGrabCount == 0)
	{
//		Sleep(100);
		CMV500Grab::s_pGrab->InitGrab(CMV500Grab::GM_ForGrab, false);
		Sleep(100);
		CMV500Grab::s_pGrab->StartGrab(false, nDevice, false);
//		Sleep(100);
		DoInputNative(pUnkImg, nDevice, bFromSettings);
		CMV500Grab::s_pGrab->StopGrab();
//		Sleep(100);
		CMV500Grab::s_pGrab->DeinitGrab();
	}
	else if (!CMV500Grab::s_pGrab->CanCapture())
	{
		CMV500Grab::s_pGrab->StopGrab();
		Sleep(100);
		CMV500Grab::s_pGrab->InitGrab(CMV500Grab::GM_ForGrab, false);
		CMV500Grab::s_pGrab->StartGrab(false, nDevice, false);
		DoInputNative(pUnkImg, nDevice, bFromSettings);
		CMV500Grab::s_pGrab->StopGrab();
		CMV500Grab::s_pGrab->InitGrab(CMV500Grab::GM_ForDraw, false);
		CMV500Grab::s_pGrab->StartGrab(IsFieldsMode());
	}
	else
		DoInputNative(pUnkImg, nDevice, bFromSettings);
	return S_OK;
}

HRESULT CDriverMV500::InputDIB(IDIBProvider *pDIBPrv, IUnknown *punkTarget, int nDevice, BOOL bFromSettings)
{
	int nTimeout = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("GrabTimeout"), 1000, true, true);
	if (m_nGrabCount == 0)
	{
		CMV500Grab::s_pGrab->InitGrab(CMV500Grab::GM_ForGrab, false);
		Sleep(100);
		CMV500Grab::s_pGrab->StartGrab(false, nDevice, false);
		Sleep(nTimeout);
		CMV500Grab::s_pGrab->StopGrab();
		((CMV500VT5Grab*)CMV500Grab::s_pGrab)->CopyGrabbedImage(pDIBPrv, nDevice);
		CMV500Grab::s_pGrab->DeinitGrab();
	}
	else if (!CMV500Grab::s_pGrab->CanCapture())
	{
		CMV500Grab::s_pGrab->StopGrab();
		Sleep(100);
		CMV500Grab::s_pGrab->InitGrab(CMV500Grab::GM_ForGrab, false);
		CMV500Grab::s_pGrab->StartGrab(false, nDevice, false);
		Sleep(nTimeout);
		CMV500Grab::s_pGrab->StopGrab();
		((CMV500VT5Grab*)CMV500Grab::s_pGrab)->CopyGrabbedImage(pDIBPrv, nDevice);
		CMV500Grab::s_pGrab->InitGrab(CMV500Grab::GM_ForDraw, false);
		CMV500Grab::s_pGrab->StartGrab(IsFieldsMode());
	}
	else
		((CMV500VT5Grab*)CMV500Grab::s_pGrab)->CopyGrabbedImage(pDIBPrv, nDevice);
	return S_OK;
}


HRESULT CDriverMV500::ExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (nMode == 0) return S_OK;
	CPreviewDialog dlg(nDevice, CWnd::FromHandle(hwndParent));
	if (nMode == 0 || nMode == 1)
		dlg.m_WindowType = CPreviewDialog::ForImageCapture;
	else
		dlg.m_WindowType = CPreviewDialog::ForSettings;
	int r = dlg.DoModal();
	if (m_nGrabCount > 0)
	{
		CMV500Grab::s_pGrab->InitGrab(CMV500Grab::GM_ForDraw, false);
		CMV500Grab::s_pGrab->StartGrab(IsFieldsMode());
	}
	return r == IDOK ? S_OK : S_FALSE;
}

HRESULT CDriverMV500::InputVideoFile(BSTR *pbstrVideoFile, int nDevice, int nMode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (*pbstrVideoFile != 0)
	{
		CString s(*pbstrVideoFile);
		if (!s.IsEmpty())
			CStdProfileManager::m_pMgr->WriteProfileString(_T("AVI"), _T("FileName"), s);
	}

	CPreviewDialog dlg(0);
	dlg.m_WindowType = CPreviewDialog::ForAVICapture;
	int r = dlg.DoModal();
	if (r == IDOK)
		*pbstrVideoFile = dlg.m_strAVI.AllocSysString();
	return r == IDOK?S_OK:E_FAIL;
}


HRESULT CDriverMV500::GetFlags(DWORD *pdwFlags)
{
	if (pdwFlags) *pdwFlags = CMV500Grab::s_pGrab->GetCamerasNum()>1?FG_ACCUMULATION|FG_INPUTFRAME:
		FG_SUPPORTSVIDEO|FG_ACCUMULATION|FG_INPUTFRAME;
	return S_OK;
}

HRESULT CDriverMV500::GetShortName(BSTR *pbstrShortName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (pbstrShortName) *pbstrShortName = __AllocBstr(_T("MV500"));
	return S_OK;
}

HRESULT CDriverMV500::GetLongName(BSTR *pbstrLongName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (pbstrLongName) *pbstrLongName = __AllocBstr(_T("Mutech MV-500/510"));
	return S_OK;
}

static void _SectionAndEntry(LPCTSTR lpName, CString &sSec, CString &sEntry)
{
	CString s(lpName);
	int n = s.Find("\\/");
	if (n == -1)
	{
		sSec = "Settings";
		sEntry = s;
	}
	else
	{
		sSec = s.Left(n);
		sEntry = s.Mid(n+1);
	}
}

HRESULT CDriverMV500::GetValue(int nCurDev, BSTR bstrName, VARIANT *pValue)
{
	CString sName(bstrName);
	CString sSec,sEntry;
	if (sName == _T("IsBrightness") || sName == _T("IsContrast") || sName == _T("IsSource") ||
		sName == _T("IsFormat"))
	{
		pValue->vt = VT_I4;
		pValue->lVal = 1;
		return S_OK;
	}
	else if (sName == _T("IsHue") || sName == _T("IsSaturation"))
	{
		pValue->vt = VT_I4;
		pValue->lVal = ((CMV500VT5Grab *)CMV500Grab::s_pGrab)->GetBoardType()==CMV500Grab::Board_MV500?1:0;
		return S_OK;
	}
	else if (strncmp(sName,"Is",2) == 0)
	{
		pValue->vt = VT_I4;
		pValue->lVal = 0;
		return S_OK;
	}
	else if (sName == _T("BrightnessMin") || sName == _T("ContrastMin") || sName == _T("SaturationMin"))
	{
		pValue->vt = VT_I4;
		pValue->lVal = 0;
		return S_OK;
	}
	else if (sName == _T("BrightnessMax") || sName == _T("ContrastMax") || sName == _T("SaturationMax"))
	{
		pValue->vt = VT_I4;
		pValue->lVal = 255;
		return S_OK;
	}
	else if (sName == _T("HueMin"))
	{
		pValue->vt = VT_I4;
		pValue->lVal = -128;
		return S_OK;
	}
	else if (sName == _T("HueMax"))
	{
		pValue->vt = VT_I4;
		pValue->lVal = 127;
		return S_OK;
	}
	else if (sName == _T("Brightness") || sName == _T("Contrast") || sName == _T("Saturation") ||
		sName == _T("Hue"))
	{
		sEntry = sName;
		sSec = _T("Source");
		*pValue = ((CVT5ProfileManager *)CStdProfileManager::m_pMgr)->GetProfileValue(sSec, sEntry);
		if (pValue->vt == VT_EMPTY)
		{
			pValue->vt = VT_I4;
			pValue->lVal = sName==_T("Hue")?0:128;
		}
	}
	else
		_SectionAndEntry(sName, sSec, sEntry);
	return S_OK;
}

HRESULT CDriverMV500::SetValue(int nCurDev, BSTR bstrName, VARIANT var)
{
	CString sName(bstrName);
	CString sSec,sEntry;
	if (sName == _T("Brightness") || sName == _T("Contrast") || sName == _T("Saturation"))
	{
		sEntry = sName;
		sSec = _T("Source");
	}
	else
		_SectionAndEntry(sName, sSec, sEntry);
	((CVT5ProfileManager *)CStdProfileManager::m_pMgr)->WriteProfileValue(sSec, sEntry, var);
	CArray<IInputPreviewSite *,IInputPreviewSite *&> arrSites;
	if (var.vt == VT_BSTR)
	{
		CString s(var.bstrVal);
		((CMV500VT5Grab *)CMV500Grab::s_pGrab)->OnSetValueString(sSec, sEntry, s, arrSites);
	}
	else if (var.vt == VT_I2)
	{
		int nValue = var.iVal;
		((CMV500VT5Grab *)CMV500Grab::s_pGrab)->OnSetValueInt(sSec, sEntry, nValue, arrSites);
	}
	else if (var.vt == VT_I4)
	{
		int nValue = var.lVal;
		((CMV500VT5Grab *)CMV500Grab::s_pGrab)->OnSetValueInt(sSec, sEntry, nValue, arrSites);
	}
	return S_OK;
}

HRESULT CDriverMV500::ExecuteDriverDialog(int nDev, LPCTSTR lpstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_nGrabCount <= 0) return S_FALSE;
	if (_tcsicmp(lpstrName, _T("Format")) == 0)
	{
		CFormatDlg dlg;
		if (dlg.DoModal() == IDOK)
			Reinit(1);
	}
	else
	{
		int nWide = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("WideDialog"), 1, true);
		int idd;
		if (((CMV500VT5Grab *)CMV500Grab::s_pGrab)->GetCamerasNum() == 2)
			idd = nWide?IDD_SOURCE2:IDD_SOURCE;
		else
			idd = nWide?IDD_SOURCE1:IDD_SOURCE;
		CSourceDlg dlg(nDev,NULL,idd);
		dlg.SetSite(this);
		dlg.DoModal();
	}
	return S_OK;
}

HRESULT CDriverMV500::GetDevicesCount(int *pnCount)
{
	if (pnCount) *pnCount = ((CMV500VT5Grab *)CMV500Grab::s_pGrab)->GetCamerasNum();
	return S_OK;
}

HRESULT CDriverMV500::GetDeviceNames(int nDev, BSTR *pbstrShortName, BSTR *pbstrLongName, BSTR *pbstrCategory)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (pbstrCategory) *pbstrCategory = __AllocBstr(IDS_INPUT);
	if (((CMV500VT5Grab *)CMV500Grab::s_pGrab)->GetCamerasNum() == 1)
	{
		if (pbstrShortName) *pbstrShortName = __AllocBstr(_T("MV500"));
		if (pbstrLongName) *pbstrLongName = __AllocBstr(_T("Mutech MV-500/510"));
	}
	else
	{
		CString s;
		if (pbstrShortName)
		{
			s.Format(_T("MV500 %d"), nDev);
			*pbstrShortName = s.AllocSysString();
		}
		if (pbstrLongName)
		{
			s.Format(_T("Mutech MV-500/510 Camera %d"), nDev+1);
			*pbstrLongName = s.AllocSysString();
		}
	}
	return S_OK;
}

HRESULT CDriverMV500::GetImage(int nCurDev, LPVOID *ppData, DWORD *pdwDataSize)
{
	if (pdwDataSize)
		*pdwDataSize = CMV500Grab::s_pGrab->CalcBufferSize(false);
	if (m_nGrabCount > 0)
	{
		if (ppData)
			*ppData = CMV500Grab::s_pGrab->GetRealData(nCurDev);
	}
	return S_OK;
}

HRESULT CDriverMV500::ConvertToNative(int nCurDev, LPVOID lpData, DWORD dwDataSize, IUnknown *punk)
{
	((CMV500VT5Grab *)CMV500Grab::s_pGrab)->CopyGrabbedImage(punk, (LPBYTE)lpData, dwDataSize);
	return S_OK;
}

HRESULT CDriverMV500::GetTriggerMode(int nCurDev, BOOL *pgTriggerMode)
{
	*pgTriggerMode = CMV500Grab::s_pGrab->m_bTrigger?true:false;
	return S_OK;
}

HRESULT CDriverMV500::SetTriggerMode(int nCurDev, BOOL bSet)
{
	TRACE("CDriverMV500::SetTriggerMode, %d\n", bSet);
	bool bNewSetting = bSet!=FALSE;
	bool bChange = true;//CMV500Grab::s_pGrab->m_bTrigger!=bNewSetting;
	if (bChange && m_nGrabCount)
	{
		CMV500Grab::s_pGrab->StopGrab();
		CMV500Grab::s_pGrab->DeinitGrab();
	}
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Trigger"), _T("Enable"), bNewSetting, true);
	if (bChange && m_nGrabCount)
	{
		CMV500Grab::s_pGrab->InitGrab(CMV500Grab::GM_ForDraw, false);
		TRACE("CDriverMV500::SetTriggerMode, calling StartGrab, m_bTrigger =%d\n",
			CMV500Grab::s_pGrab->m_bTrigger);
		CMV500Grab::s_pGrab->StartGrab(IsFieldsMode());
	}
	return S_OK;
}

HRESULT CDriverMV500::GetTriggerNotifyMode(int nCurDev, DWORD *plTriggerNotifyMode)
{
	*plTriggerNotifyMode = TrigNtfMode_SeqNum;
	return S_OK;
}

HRESULT CDriverMV500::GetImageSequenceNumber(int nCurDev, long *plSeqNum)
{
	*plSeqNum = CMV500Grab::s_lImageNum;
	return S_OK;
}

HRESULT CDriverMV500::ResetTrigger(int nCurDev)
{
//	CMV500Grab::s_pGrab->ResetTrigger(false);
	CMV500Grab::s_bResetTrigger = true;
	return S_OK;
}

HRESULT CDriverMV500::StartImageAquisition(int nDev, BOOL bStart)
{
	if (bStart)
	{
		if (m_nGrabCount++ == 0)
		{
			CMV500Grab::s_pGrab->InitGrab(CMV500Grab::GM_ForDraw, false);
			CMV500Grab::s_pGrab->StartGrab(IsFieldsMode());
			if (IsFieldsMode())
				m_bUseDrawBuffer = CStdProfileManager::m_pMgr->GetProfileInt(_T("Multy"), _T("DrawNaturally"), 0, true, true)?true:false;
		}
	}
	else
	{
		if (--m_nGrabCount <= 0)
		{
			CMV500Grab::s_pGrab->StopGrab();
			CMV500Grab::s_pGrab->DeinitGrab();
		}
	}
	return S_OK;
}

HRESULT CDriverMV500::IsImageAquisitionStarted(int nCurDev, BOOL *pbAquisition)
{
	return S_OK;
}

int CDriverMV500::FindSite(IDriverSite *pSite)
{
	for (int i = 0; i < m_arrSites.GetSize(); i++)
		if (m_arrSites.GetAt(i) == pSite)
			return i;
	return -1;
}

HRESULT CDriverMV500::AddDriverSite(int nCurDev, IDriverSite *pSite)
{
	if (FindSite(pSite) == -1)
		m_arrSites.Add(pSite);
	return S_OK;
}

HRESULT CDriverMV500::RemoveDriverSite(int nCurDev, IDriverSite *pSite)
{
	int n = FindSite(pSite);
	if (n != -1)
		m_arrSites.RemoveAt(n);
	return S_OK;
}


HRESULT CDriverMV500::BeginPreview(int nDev, IInputPreviewSite *pSite)
{
	StartImageAquisition(nDev, TRUE);
	AddDriverSite(nDev, pSite);
	return S_OK;
}

HRESULT CDriverMV500::EndPreview(int nDev, IInputPreviewSite *pSite)
{
	RemoveDriverSite(nDev, pSite);
	StartImageAquisition(nDev, FALSE);
	return S_OK;
}

HRESULT CDriverMV500::GetPreviewFreq(int nDev, DWORD *pdwFreq)
{
	if (pdwFreq)
	{
		if (((CMV500VT5Grab *)CMV500Grab::s_pGrab)->GetCamerasNum() > 1)
			*pdwFreq = CStdProfileManager::m_pMgr->GetProfileInt(_T("Multy"), _T("RedrawFrequency"), 6, true);
		else
			*pdwFreq = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("RedrawFrequency"), 30, true);
	}
	return S_OK;
}

HRESULT CDriverMV500::GetSize(int nDev, short *pdx, short *pdy)
{
	CSize sz = CMV500Grab::s_pGrab->GetSize();
	if (CMV500Grab::s_pGrab->m_bFields && !m_bUseDrawBuffer)
		sz.cy /= 2;
	if (pdx) *pdx = (short)sz.cx;
	if (pdy) *pdy = (short)sz.cy;
	return S_OK;
}

LPBYTE m_pBufDraw = NULL;

HRESULT CDriverMV500::DrawRect(int nDev, HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst)
{
#if 1
	BYTE *pData = CMV500Grab::s_pGrab->GetRealData(nDev);
	if (pData && CMV500Grab::s_pGrab->CanDraw())
	{
		CSize sz = CMV500Grab::s_pGrab->GetSize();
		CSize szOrig = sz;
		if (CMV500Grab::s_pGrab->m_bFields && !m_bUseDrawBuffer)
			sz.cy /= 2;
		CRect rcSrc(*lpRectSrc);
		CRect rcDst(*lpRectDst);
		bool bStretch = rcSrc.Width() != rcDst.Width() || rcSrc.Height() != rcDst.Height();
		if (rcSrc.Width() > sz.cx)
		{
			rcDst.right = rcDst.left + rcDst.Width()*sz.cx/rcSrc.Width();
			rcSrc.right = rcSrc.left + sz.cx;
		}
		if (rcSrc.Height() > sz.cx)
		{
			rcDst.bottom = rcDst.top + rcDst.Height()*sz.cy/rcSrc.Height();
			rcSrc.bottom = sz.cy;
		}
		BOOL bHorzMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("HorizontalMirror"), TRUE);
		bool bIndirect = bHorzMirror&&CMV500Grab::s_pGrab->GetBoardType()==CMV500Grab::Board_MV510;
		BITMAPINFO256 bi;
		memset(&bi, 0, sizeof(BITMAPINFO256));
		bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bi.bmiHeader.biWidth = sz.cx;
		BOOL bVertMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("VerticalMirror"), TRUE, true);
		if (bVertMirror&&!bIndirect)
			bi.bmiHeader.biHeight = -sz.cy;
		else
			bi.bmiHeader.biHeight = sz.cy;
		bi.bmiHeader.biBitCount = CMV500Grab::s_pGrab->GetBitsPerSample();
		bi.bmiHeader.biPlanes = 1;
		bi.bmiHeader.biCompression = BI_RGB;
		bi.bmiHeader.biClrImportant = bi.bmiHeader.biClrUsed = bi.bmiHeader.biBitCount==8?256:0;
		if (bi.bmiHeader.biClrImportant)
		{
			for (int i = 0; i < 256; i++)
				bi.bmiColors[i].rgbBlue = bi.bmiColors[i].rgbGreen = bi.bmiColors[i].rgbRed = i;
		}
		if (bStretch && bVertMirror && !bIndirect)
		{
			 CRect rc(rcSrc.left, sz.cy-rcSrc.bottom, rcSrc.right, sz.cy-rcSrc.top);
			 rcSrc = rc;
		}
		_ptr_t2<byte> bufIndir;
		if (bIndirect)
		{
			bufIndir.alloc((szOrig.cx*bi.bmiHeader.biBitCount/8+3)/4*4*szOrig.cy);
			CMV500Grab::s_pGrab->CopyGrabbedImage(bufIndir,(LPWORD)pData,false);
			pData = (LPBYTE)bufIndir;
		}
		if (CMV500Grab::s_pGrab->m_bFields && m_bUseDrawBuffer)
		{
			LPBYTE pDataOld = pData;
			DWORD dwRow = (((sz.cx*bi.bmiHeader.biBitCount/8+3)>>2)<<2);
			if (m_pBufDraw == NULL)
			{
				m_pBufDraw = new BYTE[dwRow*sz.cy];
				m_dwDrawBufferSize = dwRow*sz.cy;
			}
			else if (m_dwDrawBufferSize != dwRow*sz.cy)
			{
				delete m_pBufDraw;
				m_pBufDraw = new BYTE[dwRow*sz.cy];
				m_dwDrawBufferSize = dwRow*sz.cy;
			}
			pData = m_pBufDraw;
			LPBYTE p1 = pDataOld;
			LPBYTE p2 = pData;
			for (int i = 0; i < sz.cy/2; i++)
			{
				memcpy(p2,p1,dwRow);
				p2 += dwRow;
				memcpy(p2,p1,dwRow);
				p2 += dwRow;
				p1 += dwRow;
			}
		}
		if (bStretch)
			if (bVertMirror && !bIndirect)
				StretchDIBits(hDC, lpRectDst->left, lpRectDst->top, rcDst.Width(), rcDst.Height(), rcSrc.left, rcSrc.top,
					rcSrc.Width(), rcSrc.Height(), pData, (BITMAPINFO *)&bi, DIB_RGB_COLORS, SRCCOPY);
			else
				DrawDibDraw(hdd.hdd, hDC, lpRectDst->left, lpRectDst->top, rcDst.Width(), rcDst.Height(), &bi.bmiHeader,
					pData, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(), 0);
		else
			SetDIBitsToDevice(hDC, lpRectDst->left-rcSrc.left, lpRectDst->top-rcSrc.top, sz.cx, sz.cy, 0,
				0, 0, sz.cy, pData, (BITMAPINFO *)&bi, DIB_RGB_COLORS);
	}
	else
#endif
	{
		HBRUSH br = ::CreateSolidBrush(RGB(0,0,0));
		FillRect(hDC, lpRectDst, br);
		DeleteObject(br);
	}
	return S_OK;
}

HRESULT CDriverMV500::GetPeriod(int nDevice, int nMode, DWORD *pdwPeriod)
{
	if (pdwPeriod)
	{
		if (CMV500Grab::s_pGrab->GetCamerasNum() > 1)
			*pdwPeriod = CMV500Grab::s_pGrab->GetPeriod();
		else 
			*pdwPeriod = 0;
	}
	return S_OK;
}

HRESULT CDriverMV500::GetPreviewMode(int nDevice, int *pnMode)
{
	if (pnMode) *pnMode = m_nPreviewMode;
	return S_OK;
}

HRESULT CDriverMV500::SetPreviewMode(int nDevice, int nMode)
{
	if (nMode != m_nPreviewMode)
	{
		m_nPreviewMode = nMode;
		if (m_nGrabCount > 0)
		{
			CMV500Grab::s_pGrab->StopGrab();
			CMV500Grab::s_pGrab->StartGrab(IsFieldsMode(), nDevice, true);
		}
		for (int i = 0; i < m_arrSites.GetSize(); i++)
			m_arrSites[i]->OnChangeSize();
	}
	return S_OK;
}

void CDriverMV500::Reinit(int nMode)
{
	if (m_nGrabCount == 0) return;
	if (nMode == 0 || nMode == 1)
	{
		CMV500Grab::s_pGrab->DeinitGrab();
		unsigned nModeGm = nMode==0?CMV500Grab::GM_ForDraw|CMV500Grab::GM_Reinit:CMV500Grab::GM_ForDraw;
		CMV500Grab::s_pGrab->InitGrab(nModeGm,false);
		CMV500Grab::s_pGrab->StartGrab(IsFieldsMode());
		for (int i = 0; i < m_arrSites.GetSize(); i++)
			m_arrSites[i]->OnChangeSize();
	}
	else if (nMode == 2)
	{
		CMV500Grab::s_pGrab->ReinitBrightContrast();
	}
}

void CDriverMV500::OnTrigger()
{
	for (int i = 0; i < m_arrSites.GetSize(); i++)
		m_arrSites[i]->Invalidate();
}

