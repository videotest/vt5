#include "stdafx.h"
#include "NewDoc.h"
#include "input.h"
#include "imgdoccreator.h"
#include "InputUtils.h"
#include "NotifySpec.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageDocCreator

IMPLEMENT_DYNCREATE(CImageDocCreator, CCmdTargetEx)


CImageDocCreator::CImageDocCreator(void)
{
	_OleLockApp( this );
	m_bSelectDriver = 1;
	m_nDialogMode = 1;
	m_bFirst = TRUE;
}

CImageDocCreator::~CImageDocCreator(void)
{
	_OleUnlockApp( this );
}

BEGIN_INTERFACE_MAP(CImageDocCreator, CCmdTargetEx)
	INTERFACE_PART(CImageDocCreator, IID_IDocCreater, Creator)
END_INTERFACE_MAP()


// {2135984C-086F-4c99-9765-60A8581CEA38}
GUARD_IMPLEMENT_OLECREATE(CImageDocCreator, "DocCreators.ImageDocCreator", 
0x2135984c, 0x86f, 0x4c99, 0x97, 0x65, 0x60, 0xa8, 0x58, 0x1c, 0xea, 0x38);

IMPLEMENT_UNKNOWN(CImageDocCreator, Creator)

HRESULT CImageDocCreator::XCreator::ExecuteSettings(HWND hwndParent, BSTR bstrDocType, int nDialogMode)
{
	_try_nested(CImageDocCreator, Creator, ExecuteSettings)
	{
		if (_bstr_t(bstrDocType) != _bstr_t("Image"))
			return E_INVALIDARG;
		pThis->m_bSelectDriver = nDialogMode/100;;
		pThis->m_nDialogMode = nDialogMode%100;
		pThis->m_bFirst = ::GetValueInt(GetAppUnknown(), "Input", "First", 1);
		pThis->m_sName = ::GetValueString(GetAppUnknown(), "Input", "ImageName", "");
		pThis->m_sConf = ::GetValueString(GetAppUnknown(), "Input", "Configuration", "");
		IUnknownPtr punkCM(_GetOtherComponent(GetAppUnknown(), IID_IDriverManager), false);
		if (punkCM != 0)
		{
			sptrIDriverManager2 sptrDM(punkCM);
			BOOL bForInput = pThis->m_nDialogMode<4;
			if (pThis->m_nDialogMode>=4)
				pThis->m_nDialogMode -= 4;
			if (sptrDM->ExecuteSettings2(hwndParent, GetAppUnknown(),
				pThis->m_bSelectDriver, pThis->m_nDialogMode, pThis->m_bFirst, bForInput,
				_bstr_t(pThis->m_sConf)) == S_OK)
				return S_OK;
		}	
		return E_UNEXPECTED;
	}
	_catch_nested;
}

HRESULT CImageDocCreator::XCreator::InitDocument(IUnknown *punkDoc)
{
	_try_nested(CImageDocCreator, Creator, InitDocument)
	{
		IUnknownPtr punkCM(_GetOtherComponent(GetAppUnknown(), IID_IDriverManager), false);
		if (punkCM != 0)
		{
			sptrIDriverManager sptrDM(punkCM);
			IUnknownPtr punkD;
			int nDev;
			sptrDM->GetCurrentDeviceAndDriver(NULL, &punkD, &nDev);
			if (punkD != 0)
			{
				sptrIDriver sptrD(punkD);
				_CUseState state(sptrD, pThis->m_sConf);
				INotifyPlacePtr ptrNPDoc(punkDoc);
				IUnknownPtr punkRes = IUnknownPtr(::CreateTypedObject(_T("Image")), false);
				sptrD->InputNative(punkRes, punkDoc, nDev, TRUE);
				if (!pThis->m_sName.IsEmpty())
				{
					INamedObject2Ptr sptrNO2Img(punkRes);
					sptrNO2Img->SetName(_bstr_t(pThis->m_sName));
				}
				if (ptrNPDoc != 0)
				{
					HRESULT hRes = ptrNPDoc->NotifyPutToData(punkRes);
					if (FAILED(hRes))
						return E_UNEXPECTED;
				}
				else
				{
					_variant_t var((IUnknown*)punkRes);
					::SetValue(punkDoc, 0, 0, var);
					::FireEvent( GetAppUnknown(), szEventAfterInputImage, punkRes, pThis->GetControllingUnknown(), 0);	
/*					IUnknown punkViewPtr;
					IDocumentSitePtr ptrDocSite = punkDoc;
					ptrDocSite->GetActiveView(&punkView);
					if (punkView != 0)
					{
						sptrIDataContext sptrDC(punkView);
						sptrDC->SetActiveObject(_bstr_t(szTypeImage), punkRes, 0);
					}*/
				}
			}
		}
		return S_OK;
	}
	_catch_nested;
}







