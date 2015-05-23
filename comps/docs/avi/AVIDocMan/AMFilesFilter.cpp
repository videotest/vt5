#include "stdafx.h"
#include "resource.h"
#include "AMFilesFilter.h"
#include "VT5AVI.h"




CAMFilesFilter::CAMFilesFilter()
{
	m_strDocumentTemplate = _T("Video");
	m_strFilterName.LoadString( IDS_FILTERNAME );
	m_strFilterExt = _T(".avi");
//	AddDataObjectType("Video");
}

bool CAMFilesFilter::ReadFile( const char *pszFileName )
{
	// Create object
	CString s(szVideoObject);
	IUnknown *punkObject = CreateNamedObject(s);
	if (punkObject)
	{
		INamedObject2Ptr sptrNO(punkObject);
		_bstr_t bstr(pszFileName);
		sptrNO->SetUserName(bstr);
		sptrNO->SetName(bstr);
		punkObject->Release();
	}
	return true;
}

bool CAMFilesFilter::WriteFile( const char *pszFileName )
{
	_bstr_t bstrType(szVideoObject);
	IUnknown* punk = NULL;
	if (!(m_bObjectExists && m_sptrAttachedObject != NULL) && m_sptrIDataContext != NULL)
		m_sptrIDataContext->GetActiveObject( bstrType, &punk );
	else
	{
		punk = (IUnknown*)m_sptrAttachedObject.GetInterfacePtr();
		punk->AddRef();
	}
	if( !punk )
		return false;
	INamedObject2Ptr sptrNO(punk);
	punk->Release();
	BSTR bstr = NULL;
	sptrNO->GetUserName(&bstr);
	CString sSrcFileName = CString(bstr);
	::SysFreeString(bstr);
	BOOL bCopy = CopyFile(sSrcFileName,pszFileName,FALSE);
	return bCopy?true:false;
}

int  CAMFilesFilter::GetPropertyCount()
{
	return 0;
}

void CAMFilesFilter::GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx)
{
}

IMPLEMENT_DYNCREATE(CAVIFilesFilter, CCmdTargetEx);

// {6CC4C2C4-42B8-4277-A71B-6852859FB6F0}
GUARD_IMPLEMENT_OLECREATE(CAVIFilesFilter, "VideoDoc.AVIFilesFilter",
0x6cc4c2c4, 0x42b8, 0x4277, 0xa7, 0x1b, 0x68, 0x52, 0x85, 0x9f, 0xb6, 0xf0);

CAVIFilesFilter::CAVIFilesFilter()
{
	m_strFilterName.LoadString(IDS_FILTERNAME);
	m_strFilterExt = _T(".avi");
}

IMPLEMENT_DYNCREATE(CMOVFilesFilter, CCmdTargetEx);

// {1E5947C2-E319-4128-92C4-C0A536303977}
GUARD_IMPLEMENT_OLECREATE(CMOVFilesFilter, "VideoDoc.MOVFilesFilter", 
0x1e5947c2, 0xe319, 0x4128, 0x92, 0xc4, 0xc0, 0xa5, 0x36, 0x30, 0x39, 0x77);

CMOVFilesFilter::CMOVFilesFilter()
{
	m_strFilterName.LoadString(IDS_FILTERNAME_MOV);
	m_strFilterExt = _T(".mov");
}

IMPLEMENT_DYNCREATE(CMPGFilesFilter, CCmdTargetEx);

// {72A67F7F-6486-4E44-8E11-8EFB14EC4CEC}
GUARD_IMPLEMENT_OLECREATE(CMPGFilesFilter, "VideoDoc.MPGFilesFilter", 
0x72a67f7f, 0x6486, 0x4e44, 0x8e, 0x11, 0x8e, 0xfb, 0x14, 0xec, 0x4c, 0xec);

CMPGFilesFilter::CMPGFilesFilter()
{
	m_strFilterName.LoadString(IDS_FILTERNAME_MPG);
	m_strFilterExt = _T(".mpg");
}






