// ObjectsDisp.cpp : implementation file
//

#include "stdafx.h"
#include "data.h"
#include "ObjectsDisp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectsDisp

IMPLEMENT_DYNCREATE(CObjectsDisp, CCmdTarget)

CObjectsDisp::CObjectsDisp()
{
	EnableAutomation();

	SetName( _T("Objects") );
	
	_OleLockApp( this );
}

CObjectsDisp::~CObjectsDisp()
{
	_OleUnlockApp( this );
}


void CObjectsDisp::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CObjectsDisp, CCmdTarget)
	//{{AFX_MSG_MAP(CObjectsDisp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CObjectsDisp, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CObjectsDisp)
	DISP_PROPERTY_EX(CObjectsDisp, "ActiveImage", GetActiveImage, SetActiveImage, VT_DISPATCH)
	DISP_PROPERTY_EX(CObjectsDisp, "ActiveObjectList", GetActiveObjectList, SetActiveObjectList, VT_DISPATCH)
	DISP_PROPERTY_EX(CObjectsDisp, "ActiveClasses", GetActiveClasses, SetActiveClasses, VT_DISPATCH)
	DISP_FUNCTION(CObjectsDisp, "GetActiveObject", GetActiveObject, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION(CObjectsDisp, "GetObjectCount", GetObjectCount, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CObjectsDisp, "GetTypesCount", GetTypesCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CObjectsDisp, "GetType", GetType, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CObjectsDisp, "AddObject", AddObject, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION(CObjectsDisp, "RemoveObject", RemoveObject, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION(CObjectsDisp, "SetObjectActive", SetObjectActive, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION(CObjectsDisp, "CreateObject", CreateNewObject, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION(CObjectsDisp, "GetObjectByName", GetObjectByName, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION(CObjectsDisp, "GetFirstObjectPos", GetFirstObjectPos, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CObjectsDisp, "GetNextObjectPos", GetNextObjectPos, VT_I4, VTS_BSTR VTS_I4)
	DISP_FUNCTION(CObjectsDisp, "GetLastObjectPos", GetLastObjectPos, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CObjectsDisp, "GetPrevObjectPos", GetPrevObjectPos, VT_I4, VTS_BSTR VTS_I4)
	DISP_FUNCTION(CObjectsDisp, "GetObject", GetObject, VT_DISPATCH, VTS_BSTR VTS_I4)
	DISP_FUNCTION(CObjectsDisp, "GetFirstSelectedObjectPos", GetFirstSelectedObjectPos, VT_EMPTY, VTS_BSTR VTS_PVARIANT)
	DISP_FUNCTION(CObjectsDisp, "GetLastSelectedObjectPos", GetLastSelectedObjectPos, VT_EMPTY, VTS_BSTR  VTS_PVARIANT)
	DISP_FUNCTION(CObjectsDisp, "GetNextSelectedObject", GetNextSelectedObject, VT_DISPATCH, VTS_BSTR VTS_PVARIANT)
	DISP_FUNCTION(CObjectsDisp, "GetPrevSelectedObject", GetPrevSelectedObject, VT_DISPATCH, VTS_BSTR VTS_PVARIANT)
	DISP_FUNCTION(CObjectsDisp, "GetSelectedObjectsCount", GetSelectedObjectsCount, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CObjectsDisp, "SelectObject", SelectObject, VT_EMPTY, VTS_DISPATCH VTS_BOOL)
	DISP_FUNCTION(CObjectsDisp, "GetSelectedObject", GetSelectedObject, VT_DISPATCH, VTS_BSTR VTS_I4)
	DISP_FUNCTION(CObjectsDisp, "GetSelectedByIndex", GetSelectedByIndex, VT_DISPATCH, VTS_BSTR VTS_I4)
	DISP_FUNCTION(CObjectsDisp, "SetToActiveDocument", SetToActiveDocument, VT_I4, VTS_DISPATCH)
	DISP_FUNCTION(CObjectsDisp, "CloneObject", CloneObject, VT_DISPATCH, VTS_DISPATCH)
	DISP_FUNCTION(CObjectsDisp, "GenerateNewKey", GenerateNewKey, VT_BSTR, VTS_NONE )
	DISP_FUNCTION(CObjectsDisp, "GetBaseObjectKey", GetBaseObjectKey, VT_BSTR, VTS_BSTR VTS_I4)
	DISP_FUNCTION(CObjectsDisp, "SetBaseObjectKey", SetBaseObjectKey, VT_BOOL, VTS_BSTR VTS_I4 VTS_BSTR )
	DISP_FUNCTION(CObjectsDisp, "IsBaseObject", IsBaseObject, VT_BOOL, VTS_BSTR VTS_I4 )
	DISP_FUNCTION(CObjectsDisp, "GetObjectKey", GetObjectKey, VT_BSTR, VTS_BSTR VTS_I4)
	DISP_FUNCTION(CObjectsDisp, "GetLastActiveObject", GetLastActiveObject, VT_DISPATCH, VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IObjectsDisp to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {0185A230-5FF4-4f68-B96C-A44D4CEE8906}
static const IID IID_IObjectsDisp = 
{ 0x185a230, 0x5ff4, 0x4f68, { 0xb9, 0x6c, 0xa4, 0x4d, 0x4c, 0xee, 0x89, 0x6 } };

BEGIN_INTERFACE_MAP(CObjectsDisp, CCmdTarget)
	INTERFACE_PART(CObjectsDisp, IID_IObjectsDisp, Dispatch)
	INTERFACE_PART(CObjectsDisp, IID_INamedObject2, Name)
END_INTERFACE_MAP()

// {27DDB9F9-6F44-424b-AE43-1D65D1F7901B}
GUARD_IMPLEMENT_OLECREATE(CObjectsDisp, "Data.Objects",
0x27ddb9f9, 0x6f44, 0x424b, 0xae, 0x43, 0x1d, 0x65, 0xd1, 0xf7, 0x90, 0x1b);

/////////////////////////////////////////////////////////////////////////////
// CObjectsDisp private funcs implement

IUnknown* CObjectsDisp::_GetActiveContext()
{
	IApplicationPtr sptrApp(GetAppUnknown());
	if(sptrApp == 0)
		return 0;

	IUnknown* punkDoc = 0;
	sptrApp->GetActiveDocument(&punkDoc);

	INamedDataPtr sptrND(punkDoc);
	if(punkDoc)
		punkDoc->Release();

	if(sptrND == 0)
	{
		//ErrorMsg(IDS_DOC_NOTFOUND);
		return 0;
	}

	IUnknown* punkContext = 0;
	sptrND->GetActiveContext(&punkContext);

	IUnknownPtr	sptrU(punkContext);

	if(punkContext)
		punkContext->Release();

	return sptrU;
}


LPDISPATCH CObjectsDisp::_GetActiveObject(CString strType)
{
	/*IDataContextPtr	sptrContext(_GetActiveContext());
	
	if(sptrContext == 0)
		return 0;
	

	IUnknown* punkObj = 0;
	sptrContext->GetActiveObject(_bstr_t(strType), &punkObj);
	*/

	IApplicationPtr sptrApp(GetAppUnknown());
	if(sptrApp == 0)
		return 0;
	IUnknown* punkDoc = 0;
	sptrApp->GetActiveDocument(&punkDoc);

	IUnknown* punkObj = ::GetActiveObjectFromDocument(punkDoc, strType);

	if(punkDoc)
		punkDoc->Release();
	
	IDispatch* pdisp = 0;

	if(punkObj)
	{
		punkObj->QueryInterface(IID_IDispatch, (void**)&pdisp);
		punkObj->Release();
	}

	return pdisp;
}

void CObjectsDisp::_SetActiveObject(IUnknown* punkObj) 
{
	INamedDataObject2Ptr	sptrNDO(punkObj);

	if(sptrNDO == 0)
		return;

	IUnknown* punkNamedData = 0;
	sptrNDO->GetData(&punkNamedData);

	IDocumentSitePtr	sptrDoc(punkNamedData);
	INamedDataPtr		sptrND(punkNamedData);

	if(punkNamedData)
		punkNamedData->Release();
	
	if(sptrDoc == 0)
	{
		ErrorMsg(IDS_DOC_NOTFOUND);
		return;
	}

	IUnknown* punkView = 0;
	sptrDoc->GetActiveView(&punkView);

	IViewSitePtr sptrV(punkView);

	if(punkView)
		punkView->Release();

	if(sptrV == 0)
	{
		ErrorMsg(IDS_DOC_NOTFOUND);
		return;
	}

	IUnknown* punkFrame = 0;
	sptrV->GetFrameWindow(&punkFrame);

	IWindowPtr sptrWndChild(punkFrame);

	if(punkFrame)
		punkFrame->Release();

	HWND hwndChild = 0;
	sptrWndChild->GetHandle((HANDLE*)&hwndChild);


	IApplicationPtr	sptrApp(GetAppUnknown());
	HWND hwndMain = 0;
	if(sptrApp != 0)
	{
		sptrApp->GetMainWindowHandle(&hwndMain);
	}
	
	if(hwndMain && hwndChild)
	{
		SendMessage(hwndMain, WM_MDIACTIVATE, (WPARAM)hwndChild, (LPARAM)0);
	}

	IUnknown* punkContext = 0;
	sptrND->GetActiveContext(&punkContext); 

	IDataContextPtr	sptrContext(punkContext);
	if(punkContext)
		punkContext->Release();

	if(sptrContext != 0)
	{
		sptrContext->SetActiveObject(0, sptrNDO, aofActivateDepended);
	}
}

void CObjectsDisp::ErrorMsg(UINT nErrorID)
{
	if(GetValueInt(GetAppUnknown(), "\\General", "ObjectsShowErrorMsgs", 1) != 1)
		return;

	CString strCaption;
	strCaption.LoadString(IDS_ERROR_CAPTION);

	CString strErr;
	strErr.LoadString(nErrorID);

	HWND	hwnd = 0;
	IApplicationPtr	ptrA( GetAppUnknown() );
	ptrA->GetMainWindowHandle( &hwnd );

	MessageBox( hwnd, strErr, strCaption, MB_OK|MB_ICONERROR|MB_APPLMODAL );
}

bool CObjectsDisp::CheckType(CString strType)
{

	sptrIDataTypeInfoManager	sptrT(GetAppUnknown());

	if(sptrT == 0)
		return false;

	long	nTypesCount = 0;
	sptrT->GetTypesCount( &nTypesCount );

	
	for( long nType = 0; nType < nTypesCount; nType++ )
	{
		BSTR	bstr = 0;
		INamedDataInfo	*pTypeInfo = 0;
		sptrT->GetTypeInfo( nType, &pTypeInfo );

		CString	str = ::GetObjectName( pTypeInfo );
		pTypeInfo->Release();

		::SysFreeString( bstr );

		if(strType == str)
			return true;
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// CObjectsDisp dispatch implement
LPDISPATCH CObjectsDisp::GetActiveImage() 
{
	// TODO: Add your property handler here
	return _GetActiveObject(szTypeImage);
}

void CObjectsDisp::SetActiveImage(LPDISPATCH newValue) 
{
	// TODO: Add your property handler here

	if(::GetObjectKind(newValue) != CString(szTypeImage))
	{
		ErrorMsg(IDS_INCORRECT_OBJECT);
		return;
	}

	_SetActiveObject(newValue);
}

LPDISPATCH CObjectsDisp::GetActiveObjectList() 
{
	// TODO: Add your property handler here
	return _GetActiveObject(szTypeObjectList);
}

void CObjectsDisp::SetActiveObjectList(LPDISPATCH newValue) 
{
	// TODO: Add your property handler here
	if(::GetObjectKind(newValue) != CString(szTypeObjectList))
	{
		ErrorMsg(IDS_INCORRECT_OBJECT);
		return;
	}

	_SetActiveObject(newValue);
}

LPDISPATCH CObjectsDisp::GetActiveClasses() 
{
	// TODO: Add your property handler here
	return _GetActiveObject(szTypeClassList);
}

void CObjectsDisp::SetActiveClasses(LPDISPATCH newValue) 
{
	// TODO: Add your property handler here
	if(::GetObjectKind(newValue) != CString(szTypeClassList))
	{
		ErrorMsg(IDS_INCORRECT_OBJECT);
		return;
	}

	_SetActiveObject(newValue);
}

LPDISPATCH CObjectsDisp::GetActiveObject(LPCTSTR szType) 
{
	// TODO: Add your dispatch handler code here
	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return 0;
	}

	return _GetActiveObject(szType);
}


long CObjectsDisp::GetObjectCount(LPCTSTR szType) 
{
	// TODO: Add your dispatch handler code here
	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return 0;
	}

	IDataContext2Ptr	sptrContext(_GetActiveContext());
	
	if(sptrContext == 0)
		return 0;

	long	nObjCount = 0;

	sptrContext->GetChildrenCount(_bstr_t(szType), 0, &nObjCount);

	return nObjCount;
}


long CObjectsDisp::GetTypesCount() 
{
	// TODO: Add your dispatch handler code here
	long	nTypesCount = 0;
	sptrIDataTypeInfoManager	sptrT(GetAppUnknown());
	if(sptrT != 0)
	{
		sptrT->GetTypesCount( &nTypesCount );
	}
	return nTypesCount;
}

BSTR CObjectsDisp::GetType(long nTypeNo) 
{
	CString strResult;
	// TODO: Add your dispatch handler code here
	long nTypesCount = GetTypesCount();
	if(!(nTypeNo < nTypesCount && nTypeNo >= 0))
	{
		ErrorMsg(IDS_INCORRECT_POS);
		return 0;
	}
	
	sptrIDataTypeInfoManager	sptrT(GetAppUnknown());
	if(sptrT == 0)
		return 0;

	BSTR	bstr = 0;
	sptrT->GetType( nTypeNo, &bstr );

	strResult = CString(bstr);

	::SysFreeString( bstr );

	return strResult.AllocSysString();
}

void CObjectsDisp::AddObject(LPDISPATCH pdispObject) 
{
	// TODO: Add your dispatch handler code here
	INamedDataObject2Ptr	sptrNDO(pdispObject);
	if(sptrNDO == 0)
		return;
	IUnknown* punkNamedData = 0;

	sptrNDO->GetData(&punkNamedData);

	INamedDataPtr		sptrObjND(punkNamedData);

	if(punkNamedData)
		punkNamedData->Release();
	
	/*if(sptrObjND == 0)
	{
		ErrorMsg(IDS_DOC_NOTFOUND);
		return;
	}
	*/

	IApplicationPtr sptrApp(GetAppUnknown());
	if(sptrApp == 0)
		return;
	IUnknown* punkDoc = 0;
	sptrApp->GetActiveDocument(&punkDoc);

	INamedDataPtr		sptrActiveND(punkDoc);

	if(punkDoc)
		punkDoc->Release();

	if(sptrActiveND == 0)
	{
		ErrorMsg(IDS_DOC_NOTFOUND);
		return;
	}


	if(::GetObjectKey(sptrObjND) == ::GetObjectKey(sptrActiveND))
		return;


	CObjectListUndoRecord changes;

	changes.SetToDocData(sptrActiveND, pdispObject);
	if(sptrObjND != 0)
		changes.RemoveFromDocData(sptrObjND, pdispObject, false);
}


void CObjectsDisp::RemoveObject(LPDISPATCH pdispObject) 
{
	// TODO: Add your dispatch handler code here
	INamedDataObject2Ptr	sptrNDO(pdispObject);
	if(sptrNDO == 0)
		return;
	IUnknown* punkNamedData = 0;

	sptrNDO->GetData(&punkNamedData);

	INamedDataPtr		sptrObjND(punkNamedData);

	if(punkNamedData)
		punkNamedData->Release();
	
	if(sptrObjND == 0)
	{
		ErrorMsg(IDS_NOTFOUND_IN_ACTIVEDOC);
		return;
	}
	

	IApplicationPtr sptrApp(GetAppUnknown());
	if(sptrApp == 0)
		return;
	IUnknown* punkDoc = 0;
	sptrApp->GetActiveDocument(&punkDoc);

	INamedDataPtr		sptrActiveND(punkDoc);

	if(punkDoc)
		punkDoc->Release();

	if(sptrActiveND == 0)
	{
		ErrorMsg(IDS_NOTFOUND_IN_ACTIVEDOC);
		return;
	}	

	if(::GetObjectKey(sptrObjND) != ::GetObjectKey(sptrActiveND))
	{
		ErrorMsg(IDS_NOTFOUND_IN_ACTIVEDOC);
		return;
	}

	CObjectListUndoRecord changes;
	changes.RemoveFromDocData(sptrObjND, pdispObject, false);
}


void CObjectsDisp::SetObjectActive(LPDISPATCH pdispObject) 
{
	// TODO: Add your dispatch handler code here
	_SetActiveObject(pdispObject);	
}


LPDISPATCH CObjectsDisp::CreateNewObject(LPCTSTR szType) 
{
	// TODO: Add your dispatch handler code here
	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return 0;
	}

	IUnknown* punkCreatedObject = 0;

	IApplicationPtr sptrApp(GetAppUnknown());
	if(sptrApp == 0)
		return 0;
	IUnknown* punkDoc = 0;
	sptrApp->GetActiveDocument(&punkDoc);

	INamedDataPtr		sptrD(punkDoc);

	if(punkDoc)
		punkDoc->Release();

//get types manager
	IUnknown	*punkApp = GetAppUnknown();
	sptrIDataTypeInfoManager	sptrT( punkApp );
//get type info
	INamedDataInfo *punkTypeInfo;
	sptrT->GetTypeInfo( _bstr_t(szType), &punkTypeInfo );
	if( !punkTypeInfo )
		return 0;	//wrong type specified

	sptrINamedDataInfo	sptrI( punkTypeInfo );
	punkTypeInfo->Release();

//construct new object
	sptrI->CreateObject( &punkCreatedObject );

	if (!punkCreatedObject)
		return 0;

	// create name by default
/*	BSTR bstrName = 0;
	sptrD->CreateUniqueName(_bstr_t(szType), &bstrName);
	INamedObject2Ptr	sptrN(punkCreatedObject);
	sptrN->SetName(bstrName);
	::SysFreeString(bstrName);*/
	

	IDispatch* pdisp = 0;
	punkCreatedObject->QueryInterface(IID_IDispatch, (void**)&pdisp);
	punkCreatedObject->Release();
	
	return pdisp;
}

LPDISPATCH CObjectsDisp::GetObjectByName(LPCTSTR szName) 
{
	// TODO: Add your dispatch handler code here
	IUnknown* punkObj = 0;
	IApplicationPtr sptrApp(GetAppUnknown());
	if(sptrApp == 0)
		return 0;
	IUnknown* punkDoc = 0;

	//first check active doc
	sptrApp->GetActiveDocument(&punkDoc);
	if(punkDoc)
	{
		punkObj = ::GetObjectByName(punkDoc, szName);
		punkDoc->Release();
	}
	

	if(!punkObj)
	{
		if (GetValueInt(GetAppUnknown(), "ObjectDisp", "GetObjectByNameWorksInAllDocs", 1))
		{
		//check all docs in app
		LONG_PTR lPosTemplate = 0;
		sptrApp->GetFirstDocTemplPosition(&lPosTemplate);
		while(lPosTemplate)
		{
			LONG_PTR lPosDoc = 0;
			sptrApp->GetFirstDocPosition(lPosTemplate, &lPosDoc);
			while(lPosDoc)
			{
				sptrApp->GetNextDoc(lPosTemplate, &lPosDoc, &punkDoc);
				if(punkDoc)
				{
					punkObj = ::GetObjectByName(punkDoc, szName);
					punkDoc->Release();
					if(punkObj)
					{
						IDispatch* pdisp = 0;
						punkObj->QueryInterface(IID_IDispatch, (void**)&pdisp);
						punkObj->Release();
						return pdisp;
					}
				}
			}

			sptrApp->GetNextDocTempl(&lPosTemplate, 0, 0);

			}
		}
	}
	else
	{
		IDispatch* pdisp = 0;
		punkObj->QueryInterface(IID_IDispatch, (void**)&pdisp);
		punkObj->Release();
		return pdisp;
	}

	//ErrorMsg(IDS_INCORRECT_OBJNAME);
	return NULL;
}

LONG_PTR CObjectsDisp::GetFirstObjectPos(LPCTSTR szType) 
{
	// TODO: Add your dispatch handler code here
	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return 0;
	}
	IDataContext2Ptr	sptrContext(_GetActiveContext());
	
	if (sptrContext == 0)
		return 0;
	
	TPOS lPos = 0;
	sptrContext->GetFirstChildPos(_bstr_t(szType), 0, &lPos);

	return (LONG_PTR)lPos;
}

LPOS CObjectsDisp::GetNextObjectPos(LPCTSTR szType, LPOS lPos) 
{
	// TODO: Add your dispatch handler code here
	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return 0;
	}
	if (!lPos)
	{
		ErrorMsg(IDS_INCORRECT_POS);
		return 0;
	}

	IDataContext2Ptr	sptrContext(_GetActiveContext());
	
	if (sptrContext == 0)
		return 0;
	
	IUnknownPtr sptrObj;
	sptrContext->GetNextChild(_bstr_t(szType), 0, (TPOS*)&lPos, &sptrObj);

	return lPos;
}

LPOS CObjectsDisp::GetLastObjectPos(LPCTSTR szType) 
{
	// TODO: Add your dispatch handler code here
	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return 0;
	}
	IDataContext2Ptr	sptrContext(_GetActiveContext());
	
	if (sptrContext == 0)
		return 0;
	
	LPOS lPos = 0;
	IUnknownPtr sptrObj;
	sptrContext->GetLastChildPos(_bstr_t(szType), 0, (TPOS*)&lPos);

	return lPos;
}

LPOS CObjectsDisp::GetPrevObjectPos(LPCTSTR szType, LPOS lPos) 
{
	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return 0;
	}
	if (!lPos)
	{
		ErrorMsg(IDS_INCORRECT_POS);
		return 0;
	}

	IDataContext2Ptr	sptrContext(_GetActiveContext());
	
	if (sptrContext == 0)
		return 0;

	IUnknownPtr sptrObj;
	sptrContext->GetPrevChild(_bstr_t(szType), 0, (TPOS*)&lPos, &sptrObj);

	return lPos;
}

LPDISPATCH CObjectsDisp::GetObject(LPCTSTR szType, LPOS lPos) 
{
	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return 0;
	}
	if (!lPos)
	{
		ErrorMsg(IDS_INCORRECT_POS);
		return 0;
	}

	IDataContext2Ptr	sptrContext(_GetActiveContext());
	
	if (sptrContext == 0)
		return 0;
	
	IUnknownPtr sptrObj;
	sptrContext->GetNextChild(_bstr_t(szType), 0, (TPOS*)&lPos, &sptrObj);

	IDispatch* pdisp = 0;

	if(sptrObj)
		sptrObj->QueryInterface(IID_IDispatch, (void**)&pdisp);

	return pdisp;
}


/////////////////////////////////////////////////////////////////////////////
void CObjectsDisp::GetFirstSelectedObjectPos(LPCTSTR szType, VARIANT FAR* pvarPos) 
{
	if( pvarPos )
		*pvarPos = _variant_t( (long)0 );	

	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return;
	}

	if ( !pvarPos )
	{
		ErrorMsg(IDS_INCORRECT_POS);
		return;
	}	

	IDataContext2Ptr	sptrContext(_GetActiveContext());
	
	if (sptrContext == 0)
		return;
		
	LONG_PTR nPos = 0;
	sptrContext->GetFirstSelectedPos(_bstr_t(szType), &nPos );	

	*pvarPos = _variant_t( (long)nPos );	
}

/////////////////////////////////////////////////////////////////////////////
void CObjectsDisp::GetLastSelectedObjectPos(LPCTSTR szType, VARIANT FAR* pvarPos) 
{
	if( pvarPos )
		*pvarPos = _variant_t( (long)0 );	

	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return;
	}

	if ( !pvarPos )
	{
		ErrorMsg(IDS_INCORRECT_POS);
		return;
	}	

	IDataContext2Ptr	sptrContext(_GetActiveContext());
	
	if (sptrContext == 0)
		return;
	
	LONG_PTR lPos = 0;	
	sptrContext->GetLastSelectedPos(_bstr_t(szType), &lPos );	

	*pvarPos = _variant_t( lPos );	
	
}

/////////////////////////////////////////////////////////////////////////////
LPDISPATCH CObjectsDisp::GetNextSelectedObject(LPCTSTR szType, VARIANT FAR* pvarPos) 
{
	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return 0;
	}

	if (!pvarPos || pvarPos->vt != VT_I4 )
	{
		ErrorMsg(IDS_INCORRECT_POS);
		return 0;
	}	

	IDataContext2Ptr	sptrContext(_GetActiveContext());
	
	if (sptrContext == 0)
		return 0;
	
	LONG_PTR nPos = pvarPos->lVal;
	IUnknown* punk = NULL;	
	sptrContext->GetNextSelected( _bstr_t(szType), &nPos, &punk );

	IDispatch* pdisp = NULL;
	if( punk )
	{
		punk->QueryInterface(IID_IDispatch, (void**)&pdisp);		
		punk->Release();
	}		

	*pvarPos = _variant_t( (long)nPos );	

	return pdisp;
}

/////////////////////////////////////////////////////////////////////////////
LPDISPATCH CObjectsDisp::GetPrevSelectedObject(LPCTSTR szType, VARIANT FAR* pvarPos) 
{
	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return 0;
	}

	if (!pvarPos || pvarPos->vt != VT_I4 )
	{
		ErrorMsg(IDS_INCORRECT_POS);
		return 0;
	}	

	IDataContext2Ptr	sptrContext(_GetActiveContext());
	
	if (sptrContext == 0)
		return 0;	

	LONG_PTR lPos = pvarPos->lVal;
	
	IUnknown* punk = NULL;	
	sptrContext->GetPrevSelected( _bstr_t(szType), &lPos, &punk );

	IDispatch* pdisp = NULL;
	if( punk )
	{
		punk->QueryInterface(IID_IDispatch, (void**)&pdisp);		
		punk->Release();
	}		

	*pvarPos = _variant_t( lPos );	

	return pdisp;
}

/////////////////////////////////////////////////////////////////////////////
long CObjectsDisp::GetSelectedObjectsCount(LPCTSTR szType) 
{
	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return 0;
	}

	IDataContext2Ptr	sptrContext(_GetActiveContext());
	
	if (sptrContext == 0)
		return 0;
	
	long nCount = -1;		
	sptrContext->GetSelectedCount(_bstr_t(szType), &nCount);		

	return nCount;
}

void CObjectsDisp::SelectObject(LPDISPATCH pObject, BOOL bSelect) 
{
	INamedDataObject2Ptr sptrNDO = pObject;
	IDataContext2Ptr sptrContext(_GetActiveContext());

	if (sptrNDO == 0 || sptrContext == 0)
		return;
/*
	BOOL bOldSelect = false;
	sptrContext->GetObjectSelect(sptrNDO, &bOldSelect);
	if (bOldSelect && bSelect)
	{
		_bstr_t bstrType = ::GetObjectKind(sptrNDO);
		IUnknownPtr sptrActive;
		sptrContext->GetActiveObject(bstrType, &sptrActive);
		if (sptrActive == 0 || GetObjectKey(sptrActive) != GetObjectKey(sptrNDO))
			sptrContext->SetObjectSelect(sptrNDO, false);
	}
*/	
	sptrContext->SetObjectSelect(sptrNDO, bSelect);
}

LPDISPATCH CObjectsDisp::GetSelectedObject(LPCTSTR szType, LONG_PTR lPos) 
{
	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return 0;
	}

	IDataContext2Ptr	sptrContext(_GetActiveContext());

	if (sptrContext == 0)
		return 0;	

	IUnknownPtr sptrUnk;	
	if (FAILED(sptrContext->GetNextSelected(_bstr_t(szType), &lPos, &sptrUnk)) || sptrUnk == 0)
		return 0;

	IDispatchPtr disp = sptrUnk;
	disp.AddRef();
	return disp;
}

LPDISPATCH CObjectsDisp::GetSelectedByIndex(LPCTSTR szType, long nIndex) 
{
	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return 0;
	}

	IDataContext2Ptr sptrContext(_GetActiveContext());
	if (sptrContext == 0)
		return 0;	

	long nCount = 0;
	sptrContext->GetSelectedCount(_bstr_t(szType), &nCount);
	if (nIndex >= nCount)
		return 0;

	LONG_PTR nPos = 0;
	IDispatchPtr disp;
	sptrContext->GetFirstSelectedPos(_bstr_t(szType), &nPos);
	while (nPos)
	{
		IUnknownPtr sptrUnk;	
		if (FAILED(sptrContext->GetNextSelected(_bstr_t(szType), &nPos, &sptrUnk)))
			continue;

		if (nIndex-- == 0)
		{
			disp = sptrUnk;
			break;
		}
	}

	disp.AddRef();

	return disp;
}

long CObjectsDisp::SetToActiveDocument(LPDISPATCH pobj) 
{
	if( !CheckInterface( pobj, IID_INamedDataObject2 ) )
	{
		AfxMessageBox( "Only native objects can be placed to document" );
		return 0;
	}

	IApplication	*papp = GetAppUnknown();
	IUnknown	*punkDoc = 0;
	papp->GetActiveDocument( &punkDoc );
	if( punkDoc == 0 )
	{
		AfxMessageBox( "No Active Document found" );
		return 0;
	}
	CObjectListUndoRecord changes;
	changes.SetToDocData(punkDoc, pobj);
	punkDoc->Release();

	return 1;
}

LPDISPATCH CObjectsDisp::CloneObject(LPDISPATCH obj) 
{
	IUnknown* punkClone = 0;
	punkClone = ::CloneObject( obj );
	if( punkClone == 0 )
		return 0;

	INumeredObjectPtr ptrNumObj( punkClone );
	if( ptrNumObj )
		ptrNumObj->GenerateNewKey( 0 );
		
	IDispatch*	pdisp = 0;
	punkClone->QueryInterface( IID_IDispatch, (void **)&pdisp );
	punkClone->Release();

	return pdisp;
}

BSTR CObjectsDisp::GenerateNewKey()
{
	GUID guid;
	::CoCreateGuid( &guid );

	if( guid == INVALID_KEY )
		return 0;

	return _guid_to_bstr( &guid );
}

BSTR CObjectsDisp::GetBaseObjectKey( LPCTSTR szType, LPOS lPos )
{
	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return 0;
	}
	if (!lPos)
	{
		ErrorMsg(IDS_INCORRECT_POS);
		return 0;
	}

	IDataContext2Ptr	sptrContext = _GetActiveContext();
	
	if (sptrContext == 0)
		return 0;
	
	IUnknownPtr sptrObj;
	sptrContext->GetNextChild(_bstr_t(szType), 0, (TPOS*)&lPos, &sptrObj);

	if( sptrObj == 0 )
		return 0;
	
	INamedDataObject2Ptr sptrData = sptrObj;

	if( sptrData == 0)
		return 0;

	GUID guid;
	sptrData->GetBaseKey( &guid );

	if( guid == INVALID_KEY )
		return 0;

	return _guid_to_bstr( &guid );
}

BSTR CObjectsDisp::_guid_to_bstr( LPGUID lpGuid )
{
	if( !lpGuid )
		return 0;

	BSTR	bstr = 0;
	::StringFromCLSID( *lpGuid, &bstr );

	_bstr_t bstrGuid = bstr;	

	::CoTaskMemFree( bstr );	bstr = 0;

	return bstrGuid.copy();
}

BOOL CObjectsDisp::SetBaseObjectKey( LPCTSTR szType, LPOS lPos, LPCTSTR lpctVal )
{
	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return false;
	}
	if (!lPos)
	{
		ErrorMsg(IDS_INCORRECT_POS);
		return false;
	}

	IDataContext2Ptr	sptrContext = _GetActiveContext();
	
	if (sptrContext == 0)
		return false;
	
	IUnknownPtr sptrObj;
	sptrContext->GetNextChild(_bstr_t(szType), 0, (TPOS*)&lPos, &sptrObj);

	if( sptrObj == 0 )
		return false;
	
	INamedDataObject2Ptr sptrData = sptrObj;

	if( sptrData == 0)
		return false;

	GUID guid;

	_bstr_to_guid( lpctVal, &guid );

	if( guid == INVALID_KEY )
		::CoCreateGuid( &guid );

	if( sptrData->SetBaseKey( &guid ) != S_OK )
		return false;

	return true;

}

BOOL CObjectsDisp::IsBaseObject( LPCTSTR szType, LPOS lPos )
{
	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return false;
	}
	if (!lPos)
	{
		ErrorMsg(IDS_INCORRECT_POS);
		return false;
	}

	IDataContext2Ptr	sptrContext = _GetActiveContext();
	
	if (sptrContext == 0)
		return false;
	
	IUnknownPtr sptrObj;
	sptrContext->GetNextChild(_bstr_t(szType), 0, (TPOS*)&lPos, &sptrObj);

	if( sptrObj == 0 )
		return false;
	
	INamedDataObject2Ptr sptrData = sptrObj;

	if( sptrData == 0)
		return false;

	BOOL bBase = false;
	if( sptrData->IsBaseObject( &bBase ) != S_OK )
		return false;

	return bBase;
}

void CObjectsDisp::_bstr_to_guid( LPCTSTR lpctVal, LPGUID lpGuid )
{
	if( !lpGuid || !lpctVal )
		return;

	::CLSIDFromString( _bstr_t(lpctVal), lpGuid );
}

BSTR CObjectsDisp::GetObjectKey( LPCTSTR szType, LPOS lPos )
{
	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return 0;
	}
	if (!lPos)
	{
		ErrorMsg(IDS_INCORRECT_POS);
		return 0;
	}

	IDataContext2Ptr	sptrContext = _GetActiveContext();
	
	if (sptrContext == 0)
		return 0;
	
	IUnknownPtr sptrObj;
	sptrContext->GetNextChild(_bstr_t(szType), 0, (TPOS*)&lPos, &sptrObj);

	if( sptrObj == 0 )
		return 0;
	
	INamedDataObject2Ptr sptrData = sptrObj;

	if( sptrData == 0)
		return 0;

	GUID guid = ::GetObjectKey( sptrData );

	if( guid == INVALID_KEY )
		return 0;

	return _guid_to_bstr( &guid );
}

LPDISPATCH CObjectsDisp::GetLastActiveObject(LPCTSTR szType)
{
	if(!CheckType(szType))
	{
		ErrorMsg(IDS_INCORRECT_OBJTYPE);
		return 0;
	}
	IDataContext2Ptr	sptrContext(_GetActiveContext());
	
	if (sptrContext == 0)
		return 0;
	
	IUnknown* punk = NULL;	
	sptrContext->GetLastActiveObject( _bstr_t(szType), &punk );
	
	IDispatch* pdisp = NULL;
	if( punk )
	{
		punk->QueryInterface(IID_IDispatch, (void**)&pdisp);		
		punk->Release();
	}		
	return pdisp;
}

