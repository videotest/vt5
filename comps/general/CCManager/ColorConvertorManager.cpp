// ColorConvertorManager.cpp : implementation file
//

#include "stdafx.h"
#include "CCManager.h"
#include "ColorConvertorManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorConvertorManager

IMPLEMENT_DYNCREATE(CColorConvertorManager, CCmdTargetEx)

CColorConvertorManager::CColorConvertorManager()
{
	EnableAutomation();
	
	_OleLockApp( this );

	SetName( _T("ColorConvertorManager") );
	AttachComponentGroup( szPluginColorCnv );
	CCompManager::Init();
}

CColorConvertorManager::~CColorConvertorManager()
{
	_OleUnlockApp( this );
}


void CColorConvertorManager::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CColorConvertorManager, CCmdTargetEx)
	//{{AFX_MSG_MAP(CColorConvertorManager)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CColorConvertorManager, CCmdTargetEx)
	//{{AFX_DISPATCH_MAP(CColorConvertorManager)
	DISP_FUNCTION(CColorConvertorManager, "GetCnvCount", GetCnvCount, VT_I2, VTS_NONE)
	DISP_FUNCTION(CColorConvertorManager, "GetCnvPos", GetCnvPos, VT_I2, VTS_BSTR)
	DISP_FUNCTION(CColorConvertorManager, "GetCnvName", GetCnvName, VT_BSTR, VTS_I2)
	DISP_FUNCTION(CColorConvertorManager, "GetCnvPanesCount", GetCnvPanesCount, VT_I2, VTS_I2)
	DISP_FUNCTION(CColorConvertorManager, "GetCnvPaneName", GetCnvPaneName, VT_BSTR, VTS_I2 VTS_I2)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IColorConvertorManager to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {6A1FC3C9-6749-11D3-A4EB-0000B493A187}
 //static const IID IID_IColorConvertorManager =
//{ 0x6a1fc3c9, 0x6749, 0x11d3, { 0xa4, 0xeb, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };

BEGIN_INTERFACE_MAP(CColorConvertorManager, CCmdTargetEx)
	INTERFACE_PART(CColorConvertorManager, IID_INamedObject2, Name)
	INTERFACE_PART(CColorConvertorManager, IID_ICompManager, CompMan)
	INTERFACE_PART(CColorConvertorManager, IID_IRootedObject, Rooted)
	INTERFACE_PART(CColorConvertorManager, IID_IColorConvertorManager, Dispatch)
END_INTERFACE_MAP()


// {6A1FC3CB-6749-11D3-A4EB-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CColorConvertorManager, "CCManager.ColorConvertorManager", 
0x6a1fc3cb, 0x6749, 0x11d3, 0xa4, 0xeb, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87)

/////////////////////////////////////////////////////////////////////////////
// CColorConvertorManager message handlers

short CColorConvertorManager::GetCnvCount() 
{
	return GetComponentCount();
}

short CColorConvertorManager::GetCnvPos(LPCTSTR szName) 
{
	IUnknown* punk;
	sptrIColorConvertor2 sptrCC;
	int numCvtrs = GetComponentCount();
	BSTR bstr;
	int idx = -1;
	for(int i = 0; i < numCvtrs; i++)
	{
		punk = GetComponent(i, false);
		sptrCC = punk;
		punk->Release();
		sptrCC->GetCnvName(&bstr);
		CString strName(bstr);
		::SysFreeString(bstr);
		if (strName == szName)
		{
			idx = i;
			break;
		}
	}
	return idx;
}

BSTR CColorConvertorManager::GetCnvName(short nPos) 
{
	IUnknown* punk = GetComponent(nPos, false);
	sptrIColorConvertor2 sptrCC(punk);
	punk->Release();
	if (sptrCC == 0)
		return 0;
	BSTR bstr;
	sptrCC->GetCnvName(&bstr);
	CString strRes(bstr);
	::SysFreeString(bstr);
	return strRes.AllocSysString();
}

short CColorConvertorManager::GetCnvPanesCount(short nPos) 
{
	// TODO: Add your dispatch handler code here
	IUnknown* punk = GetComponent(nPos, false);
	sptrIColorConvertor2 sptrCC(punk);
	punk->Release();
	if (sptrCC == 0)
		return 0;
	int numPanes;
	sptrCC->GetColorPanesDefCount(&numPanes);
	return numPanes;
}

BSTR CColorConvertorManager::GetCnvPaneName(short nPosCnv, short nPosPane) 
{
	// TODO: Add your dispatch handler code here
	IUnknown* punk = GetComponent(nPosCnv, false);
	sptrIColorConvertor2 sptrCC(punk);
	punk->Release();
	if (sptrCC == 0)
		return 0;
	BSTR bstr;
	sptrCC->GetPaneName(nPosPane, &bstr);
	CString strRes(bstr);
	::SysFreeString(bstr);
	return strRes.AllocSysString();
}
