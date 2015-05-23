// TypeInfoMgr.cpp : implementation file
//

#include "stdafx.h"
#include "Data.h"
#include "TypeInfoMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTypeInfoManager

IMPLEMENT_DYNCREATE(CTypeInfoManager, CCmdTargetEx)

CTypeInfoManager::CTypeInfoManager()
{
//	EnableAutomation();
	EnableAggregation();
	
	_OleLockApp( this );
	// 
	AttachComponentGroup(szPluginObject);
	// set parent
//!!!andy
	//SetRootUnknown(GetControllingUnknown());
	// initalize (load components)
	Init();
}

CTypeInfoManager::~CTypeInfoManager()
{
	// clean up
	DeInit();

	_OleUnlockApp( this );
	}


void CTypeInfoManager::OnFinalRelease()
{
	CCmdTargetEx::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CTypeInfoManager, CCmdTargetEx)
	//{{AFX_MSG_MAP(CTypeInfoManager)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// {C49F7851-39FA-11D3-87C6-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CTypeInfoManager, "Data.TypeInfoManager", 
0xc49f7851, 0x39fa, 0x11d3, 0x87, 0xc6, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b)

/*
BEGIN_DISPATCH_MAP(CTypeInfoManager, CCmdTargetEx)
	//{{AFX_DISPATCH_MAP(CTypeInfoManager)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()
*/
// Note: we add support for IID_ITypeInfoManagerDisp to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {C49F7850-39FA-11D3-87C6-0000B493FF1B}
static const IID IID_ITypeInfoManagerDisp =
{ 0xc49f7850, 0x39fa, 0x11d3, { 0x87, 0xc6, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };

BEGIN_INTERFACE_MAP(CTypeInfoManager, CCmdTargetEx)
//	INTERFACE_PART(CTypeInfoManager, IID_ITypeInfoManagerDisp, Dispatch)
	INTERFACE_PART(CTypeInfoManager, IID_IDataTypeInfoManager, TypeInfo)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CTypeInfoManager, TypeInfo);


/////////////////////////////////////////////////////////////////////////////
// CTypeInfoManager message handlers

// implements IDataTypeInfoManager interface

// Create Named Data Info Object for object with 'Type' type  and return its IUnknown
// we can use this Named Data Info Object to create object of 'Type' type
HRESULT CTypeInfoManager::XTypeInfo::GetTypeInfo(BSTR bstrType, INamedDataInfo **ppunkObj)
{
	_try_nested(CTypeInfoManager, TypeInfo, GetTypeInfo)
	{
		IUnknown* punk = NULL;
		INamedDataInfo* pndinfo = NULL;
		BSTR pbstr = NULL;
		_bstr_t requestType(bstrType, true);

		// for all types
		for (int i = 0; i < pThis->GetComponentCount(); i++)
		{	
			// get component
			punk = pThis->GetComponent(i, false);
			
			// get component's INamedDataInfo
			if (!CheckInterface(punk, IID_INamedDataInfo))
				return E_FAIL;

			INamedObject2Ptr	ptrN(punk);

			// get type
			ptrN->GetName(&pbstr);
			_bstr_t existType(pbstr, false);
			
			// compare getted type and requested type
			if (existType == requestType)
			{	
				// if we are here it's success
				// copy INamedDataInfo (we dont' call Release !!!)
				INamedDataInfoPtr	ptrObjectInfo( punk );
				*ppunkObj = ptrObjectInfo.Detach();
				return S_OK;
			}
			// else continue
		}
		// if we are here it's mean that request type is invalid
		return E_INVALIDARG;
	}
	_catch_nested;
}

// enumerate all types and return count of it 
// simple call CCompManager::GetComponentCount
HRESULT CTypeInfoManager::XTypeInfo::GetTypesCount(long *pnCount)
{
	_try_nested(CTypeInfoManager, TypeInfo, GetTypesCount)
	{
		// simple call CCompManager::GetComponentCount
		*pnCount = pThis->GetComponentCount();
		return S_OK;
	}
	_catch_nested;
}

// return type of object by index (this function work with GetTypesCount...)
// that described below
HRESULT CTypeInfoManager::XTypeInfo::GetType(long index, BSTR *pbstrType)
{
	_try_nested(CTypeInfoManager, TypeInfo, GetType)
	{
		// if index out of bound we return E_INVALIDARG
		int size = pThis->GetComponentCount();
		// check index do not exceed the bounds
		if (index < 0 || index > size - 1)
			return E_INVALIDARG;

		// get component's IUnknown
		IUnknown* punk = pThis->GetComponent(index, false);

		// get component's INamedDataInfo
		if (!CheckInterface(punk, IID_INamedDataInfo))
			return E_FAIL;

		INamedObject2Ptr	ptrNamed(punk);
		ptrNamed->GetName(pbstrType);
	
		return S_OK;
	}
	_catch_nested;
}


HRESULT CTypeInfoManager::XTypeInfo::GetTypeInfo( long index, INamedDataInfo **ppunkObj )
{
	_try_nested(CTypeInfoManager, TypeInfo, GetType)
	{
		// if index out of bound we return E_INVALIDARG
		int size = pThis->GetComponentCount();
		// check index do not exceed the bounds
		if (index < 0 || index > size - 1)
			return E_INVALIDARG;

		// get component's IUnknown
		IUnknown* punk = pThis->GetComponent(index, false);

		INamedDataInfoPtr	ptrTypeInfo( punk );
		*ppunkObj =ptrTypeInfo.Detach();

		return S_OK;
	}
	_catch_nested;
}

