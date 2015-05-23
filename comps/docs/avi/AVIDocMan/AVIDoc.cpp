#include "StdAfx.h"
#include "Resource.h"
#include "AVIDoc.h"
#include "Input.h"
#include "VT5AVI.h"
#include "StdProfile.h"


// {FDCCBE48-8C07-4372-909C-483FB585D5DB}
GUARD_IMPLEMENT_OLECREATE(CAVIDocument, "VideoDoc.VideoDocument", 
0xfdccbe48, 0x8c07, 0x4372, 0x90, 0x9c, 0x48, 0x3f, 0xb5, 0x85, 0xd5, 0xdb);

// {7D6C7F1F-5004-466E-9073-866DBCBBE275}
GUARD_IMPLEMENT_OLECREATE(CAVIDocTemplate, "VideoDoc.VideoDocTemplate", 
0x7d6c7f1f, 0x5004, 0x466e, 0x90, 0x73, 0x86, 0x6d, 0xbc, 0xbb, 0xe2, 0x75);

IMPLEMENT_DYNCREATE(CAVIDocument, CCmdTargetEx)
IMPLEMENT_DYNCREATE(CAVIDocTemplate, CCmdTargetEx)

BEGIN_INTERFACE_MAP(CAVIDocument, CDocBase)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////////////////
//CImageDocTemplate
CAVIDocTemplate::CAVIDocTemplate()
{
	EnableAggregation();
}

CString CAVIDocTemplate::GetDocTemplString()
{
	CString	str;
	str.LoadString( IDS_TEMPLATE );
	return str;
}

/////////////////////////////////////////////////////////////////////////////////////////
//CImageDocument
CAVIDocument::CAVIDocument()
{
	EnableAggregation();
}

IDriver *__FindDriverForAVIInput(int nMode, int &nDevice);
bool CAVIDocument::OnNewDocument()
{
	int nMode1 = CStdProfileManager::m_pMgr->GetProfileInt(NULL, _T("SelectDriverDialog"), 1, true);
	int nMode2 = CStdProfileManager::m_pMgr->GetProfileInt(NULL, _T("DriverDialog"), 1, true);
	int nDevice;
	IDriver *pDrv = __FindDriverForAVIInput(nMode1, nDevice);
	if (pDrv)
	{
		BSTR bstrVideoFileName = NULL;
		HRESULT hr = pDrv->InputVideoFile(&bstrVideoFileName, nDevice, nMode2);
		CString sVideoFileName(bstrVideoFileName);
		::SysFreeString(bstrVideoFileName);
		pDrv->Release();
		if (hr == S_OK && !sVideoFileName.IsEmpty())
		{
			sptrINamedData sptrD( GetControllingUnknown() );
			// Create object
			CString s(szVideoObject);
			IUnknown *punkObject = NULL;
			punkObject = ::CreateTypedObject(s);
			if (punkObject)
			{
				INamedObject2Ptr sptrNO(punkObject);
				_bstr_t bstr(sVideoFileName);
				sptrNO->SetUserName(bstr);
				_variant_t var(punkObject);
				sptrD->SetValue(NULL, var);
				punkObject->Release();
			}
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

bool CAVIDocument::OnOpenDocument( const char *psz )
{
	return false;
}

