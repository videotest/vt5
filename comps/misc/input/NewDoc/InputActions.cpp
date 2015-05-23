#include "stdafx.h"
#include "resource.h"
#include "input.h"
#include "InputActions.h"
#include "InputUtils.h"
#include "NotifySpec.h"
#include "VideoDriversList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionInputImage, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionDoInputImage, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionInputVideo, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowPreview, CCmdTargetEx);

// olecreate

// {D651D677-C8EA-11d3-99F5-000000000000}
GUARD_IMPLEMENT_OLECREATE(CActionInputImage, "input.InputImage",
0xd651d677, 0xc8ea, 0x11d3, 0x99, 0xf5, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
// {8EB3A303-CCCF-11d3-99F8-000000000000}
GUARD_IMPLEMENT_OLECREATE(CActionDoInputImage, "input.DoInputImage",
0x8eb3a303, 0xcccf, 0x11d3, 0x99, 0xf8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
// {47B9199E-A7EA-441C-9536-895056DAC18D}
GUARD_IMPLEMENT_OLECREATE(CActionInputVideo, "input.InputVideo", 
0x47b9199e, 0xa7ea, 0x441c, 0x95, 0x36, 0x89, 0x50, 0x56, 0xda, 0xc1, 0x8d);
// {90B1E689-6F89-4C5A-82B2-5CC9F16F0B0A}
GUARD_IMPLEMENT_OLECREATE(CActionShowPreview, "ShowPreview.ShowPreview",
0x90b1e689, 0x6f89, 0x4c5a, 0x82, 0xb2, 0x5c, 0xc9, 0xf1, 0x6f, 0xb, 0xa);

// guidinfo 

// {D651D675-C8EA-11d3-99F5-000000000000}
static const GUID guidInputImage =
{ 0xd651d675, 0xc8ea, 0x11d3, { 0x99, 0xf5, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 } };
// {8EB3A301-CCCF-11d3-99F8-000000000000}
static const GUID guidDoInputImage =
{ 0x8eb3a301, 0xcccf, 0x11d3, { 0x99, 0xf8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 } };
// {73E6F8C4-36FF-4A85-B96D-E893C4DADFF5}
static const GUID guidInputVideo = 
{ 0x73e6f8c4, 0x36ff, 0x4a85, { 0xb9, 0x6d, 0xe8, 0x93, 0xc4, 0xda, 0xdf, 0xf5 } };
// {E20A2B8A-B1B6-4EBE-9C62-08ADD09A0BF1}
static const GUID guidShowPreview = 
{ 0xe20a2b8a, 0xb1b6, 0x4ebe, { 0x9c, 0x62, 0x8, 0xad, 0xd0, 0x9a, 0xb, 0xf1 } };



//[ag]6. action info

ACTION_INFO_FULL(CActionInputImage, IDS_ACTION_INPUT_IMAGE, IDS_MENU_INPUT_IMAGE, IDS_TB_MAIN, guidInputImage);
ACTION_INFO_FULL(CActionDoInputImage, IDS_ACTION_INPUT_IMAGE_IN_CUR, -1, -1, guidDoInputImage);
ACTION_INFO_FULL(CActionInputVideo, IDS_ACTION_INPUT_VIDEO, -1, -1, guidInputVideo);
ACTION_INFO_FULL(CActionShowPreview, IDS_ACTION_SHOW_PREVIEW, IDS_MENU_VIEW, -1, guidShowPreview);

//[ag]7. targets
ACTION_TARGET(CActionInputImage, szTargetApplication);
ACTION_TARGET(CActionDoInputImage, szTargetAnydoc);
ACTION_TARGET(CActionInputVideo, szTargetApplication);
ACTION_TARGET(CActionShowPreview, szTargetFrame);

//[ag]8. arguments
ACTION_ARG_LIST(CActionInputImage)
	ARG_INT( _T("SelectDriver"), 1 )
	ARG_INT( _T("DialogMode"), 1 )
	ARG_INT( _T("First"), 1 )
	ARG_STRING( _T("Name"), NULL)
	ARG_STRING( _T("Configuration"), NULL)
END_ACTION_ARG_LIST();
ACTION_ARG_LIST(CActionDoInputImage)
	ARG_INT( _T("SelectDriver"), 1 )
	ARG_INT( _T("DialogMode"), 1 )
	ARG_INT( _T("First"), 1 )
	ARG_STRING( _T("Name"), NULL)
	ARG_STRING( _T("Configuration"), NULL)
END_ACTION_ARG_LIST();
ACTION_ARG_LIST(CActionInputVideo)
	ARG_INT( _T("SelectDriver"), 1 )
	ARG_INT( _T("DialogMode"), 1 )
	ARG_STRING( _T("Name"), NULL)
	ARG_STRING( _T("Configuration"), NULL)
END_ACTION_ARG_LIST();




//[ag]9. implementation


//////////////////////////////////////////////////////////////////////
//CActionDoInputImage implementation
CActionDoInputImage::CActionDoInputImage()
{
}

CActionDoInputImage::~CActionDoInputImage()
{
}

bool CActionDoInputImage::Invoke()
{
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	int nDialogMode = GetArgumentInt("DialogMode");
	CString sName = GetArgumentString("Name");
	CString sConf = GetArgumentString("Configuration");
	BOOL bFromSettings = nDialogMode != 0;
	if (punkCM)
	{
		sptrIDriverManager sptrDM(punkCM);
		punkCM->Release();
		IUnknown *punkD;
		int nDev;
		sptrDM->GetCurrentDeviceAndDriver(NULL, &punkD, &nDev);
		if (punkD)
		{
			sptrIDriver sptrD(punkD);
			punkD->Release();
			_CUseState state(sptrD, sConf);
			INotifyPlacePtr ptrNPDoc( m_punkTarget );
			IUnknownPtr punkRes(::CreateTypedObject(_T("Image")), false);
			sptrD->InputNative(punkRes, m_punkTarget, nDev, bFromSettings);
			if (!sName.IsEmpty())
			{
				INamedObject2Ptr sptrNO2Img(punkRes);
				sptrNO2Img->SetName(_bstr_t(sName));
			}
			if (ptrNPDoc == 0)
			{
				IUnknownPtr punkSLI;
				if (!sName.IsEmpty())
					punkSLI = IUnknownPtr(GetObjectByName(m_punkTarget,
						_bstr_t(sName),_bstr_t(szArgumentTypeSewLI)),false);
				if (punkSLI == 0)
					punkSLI = IUnknownPtr(GetActiveObjectFromDocument(m_punkTarget, szArgumentTypeSewLI), false);
				if (punkSLI != 0)
					ptrNPDoc = punkSLI;

/*				IDocumentSitePtr ptrDS(m_punkTarget);
				if (ptrDS != 0)
				{
					IUnknownPtr punkActiveView;
					ptrDS->GetActiveView(&punkActiveView);
					ptrNPDoc = punkActiveView;
				}*/
			}
			if (ptrNPDoc != 0)
			{
				IUnknownPtr punkUndoObj;
				HRESULT hRes = ptrNPDoc->NotifyPutToDataEx(punkRes, &punkUndoObj);
				if (FAILED(hRes))
					return false;
				m_sptrNty = ptrNPDoc;
				m_punkUndoObj = punkUndoObj;
			}
			else
			{
				m_punkRes = punkRes;
				_variant_t var((IUnknown*)m_punkRes);
				::SetValue(m_punkTarget, 0, 0, var);

				::FireEvent( GetAppUnknown(), szEventAfterInputImage, m_punkRes, GetControllingUnknown(), 0);	
				IUnknown	*punkView = 0;
				IDocumentSitePtr	ptrDocSite = m_punkTarget;
				ptrDocSite->GetActiveView(&punkView);
				if( punkView )
				{
					sptrIDataContext2 sptrDC(punkView);
					if (GetValueInt(GetAppUnknown(), "Input", "DeselectObjListAfterInput", TRUE))
					{
						sptrDC->UnselectAll(_bstr_t(szArgumentTypeObjectList));
						sptrDC->UnselectAll(_bstr_t(szArgumentTypeBinaryImage));
						sptrDC->UnselectAll(_bstr_t(szDrawingObjectList));
					}
					sptrDC->SetActiveObject(_bstr_t(szTypeImage), m_punkRes, 0);
					punkView->Release();
				}
			}
		}
	}
	
	return true;
}

bool CActionDoInputImage::IsAvaible()
{
	return CActionBase::IsAvaible();
}

bool CActionDoInputImage::ExecuteSettings(CWnd *pwndParent)
{
	int nSelectDriver = GetArgumentInt("SelectDriver");
	int nDialogMode = GetArgumentInt("DialogMode");
	BOOL bFirst = GetArgumentInt("First");
	if (nSelectDriver == 0 && nDialogMode == -1) //Action called from "InputImage"
		return true;
	CString sConf = GetArgumentString("Configuration");
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		sptrIDriverManager2 sptrDM(punkCM);
		punkCM->Release();
//		if (sptrDM->ExecuteSettings(pwndParent?pwndParent->m_hWnd:NULL, m_punkTarget, nSelectDriver, nDialogMode, bFirst, TRUE) == S_OK)
		if (sptrDM->ExecuteSettings2(pwndParent?pwndParent->m_hWnd:NULL, m_punkTarget, nSelectDriver,
			nDialogMode, bFirst, TRUE, _bstr_t(sConf)) == S_OK)
			return true;
	}	
	return false;
}

bool CActionDoInputImage::DoUndo()
{
	if (m_sptrNty)
		m_sptrNty->Undo(m_punkUndoObj);
	else
		::DeleteObject(m_punkTarget, m_punkRes);
	return true;
}

bool CActionDoInputImage::DoRedo()
{
	if (m_sptrNty)
		m_sptrNty->Redo(m_punkUndoObj);
	else
	{
		_variant_t var((IUnknown*)m_punkRes);
		::SetValue(m_punkTarget, 0, 0, var);
		IUnknownPtr punkView;
		IDocumentSitePtr ptrDocSite = m_punkTarget;
		ptrDocSite->GetActiveView(&punkView);
		if (punkView != 0)
		{
			sptrIDataContext sptrDC(punkView);
			sptrDC->SetActiveObject(_bstr_t(szTypeImage), m_punkRes, 0);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionInputImage implementation
CActionInputImage::CActionInputImage()
{
}

CActionInputImage::~CActionInputImage()
{
}

bool CActionInputImage::Invoke()
{
	IUnknown *punkAM = _GetOtherComponent( GetAppUnknown(), IID_IActionManager );
	sptrIActionManager	sptrAM(punkAM);
	punkAM->Release();
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	BOOL bFirst = GetArgumentInt("First");
	int nDialogMode = GetArgumentInt("DialogMode");
	CString sName = GetArgumentString("Name");
	CString sConf = GetArgumentString("Configuration");
	if (punkCM && nDialogMode < 4)
	{
		sptrIDriverManager sptrDM(punkCM);
		BOOL bAvail;
		sptrDM->IsInputAvailable(&bAvail);
		if (bAvail)
		{
			DoExecute(sptrAM, _T("FileNew"), _T("\"Image\""));
			CString sArg;
			sArg.Format("0, -1, %d, \"%s\", \"%s\"", bFirst, (LPCTSTR)sName, (LPCTSTR)sConf);
			DoExecute(sptrAM, _T("InputImageInCurrentDoc"), sArg);
		}
	}
	if (punkCM) punkCM->Release();
	return true;
}

bool CActionInputImage::ExecuteSettings(CWnd *pwndParent)
{
	BOOL bSelectDriver = GetArgumentInt("SelectDriver");
	int nDialogMode = GetArgumentInt("DialogMode");
	BOOL bFirst = GetArgumentInt("First");
	CString sConf = GetArgumentString("Configuration");
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		sptrIDriverManager2 sptrDM(punkCM);
		punkCM->Release();
		BOOL bForInput = nDialogMode<4;
		if (nDialogMode>=4)
			nDialogMode -= 4;
		if (sptrDM->ExecuteSettings2(pwndParent?pwndParent->m_hWnd:NULL, m_punkTarget, bSelectDriver,
			nDialogMode, bFirst, bForInput, _bstr_t(sConf)) == S_OK)
			return true;
	}	
	return false;
}

//////////////////////////////////////////////////////////////////////
//CActionInputVideo implementation
CActionInputVideo::CActionInputVideo()
{
}

CActionInputVideo::~CActionInputVideo()
{
}

IDriverPtr __FindDriverForAVIInput(int nMode, int &nDevice)
{
	if (nMode != 0)
	{
		CVideDriversList dlg;
		if (dlg.DoModal() != IDOK)
			return FALSE;
	}
	CString sDrvName = GetValueString(GetAppUnknown(false), "Input", "VideoDevice", NULL);
	CString sDrv1stShort;
	sptrIDriver sptrD1st;
	int nDevInDrv1st;
	sptrIDriver sptrDCur;
	int nDevInDrvCur;
	// Find driver, supporting input of video files.
	IUnknownPtr punkCM(_GetOtherComponent( GetAppUnknown(), IID_IDriverManager ), false);
	if (punkCM)
	{
		IDriverManagerPtr sptrDM(punkCM);
		ICompManagerPtr sptrCM(punkCM);
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
			IUnknownPtr punkDriver;
			sptrCM->GetComponentUnknownByIdx(nDriver, &punkDriver);
			if (punkDriver != 0)
			{
				sptrIDriver sptrD(punkDriver);
				DWORD dwFlags;
				sptrD->GetFlags(&dwFlags);
				if (dwFlags & FG_SUPPORTSVIDEO)
				{
					if (sptrD1st == 0)
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
	if (sptrDCur == 0)
	{
		if (sptrD1st == 0)
		{
			AfxMessageBox(IDS_NO_DRIVER_SUPPORTS_AVI, MB_OK|MB_ICONEXCLAMATION);
			return NULL;
		}
		else
		{
			SetValue(GetAppUnknown(false), "Input", "VideoDevice", sDrv1stShort);
			nDevice = nDevInDrv1st;
			return sptrD1st;
		}
	}
	else
	{
		nDevice = nDevInDrvCur;
		return sptrDCur;
	}
	return NULL;
}


bool CActionInputVideo::Invoke()
{
	IUnknownPtr punkCM(_GetOtherComponent( GetAppUnknown(), IID_IDriverManager ), false);
	int nSelectDriver = GetArgumentInt("SelectDriver");
	int nDialogMode = GetArgumentInt("DialogMode");
	CString sName = GetArgumentString("Name");
	CString sConf = GetArgumentString("Configuration");
	int nDev;
	IDriverPtr sptrD = __FindDriverForAVIInput(nSelectDriver, nDev);
	if (sptrD != 0)
	{
		_CUseState state(sptrD, sConf);
		BSTR bstrName = sName.IsEmpty()?NULL:sName.AllocSysString();
		HRESULT hr = sptrD->InputVideoFile(&bstrName, nDev, nDialogMode);
//		if (bstrName != NULL) ::SysFreeString(bstrName);
		if (FAILED(hr))
		{
			if (bstrName) ::SysFreeString(bstrName);
			::SetValue(::GetAppUnknown(false), "Input", "LastVideoFile", "");
			return false;
		}
		else
		{
			::SetValue(::GetAppUnknown(false), "Input", "LastVideoFile", (LPCTSTR)CString(bstrName));
			if (bstrName) ::SysFreeString(bstrName);
		}
		return true;
	}
	return false;
}

#define szPreviewViewProgID "PreviewDoc.PreviewView"

CString CActionShowPreview::GetViewProgID()
{	return szPreviewViewProgID;}

