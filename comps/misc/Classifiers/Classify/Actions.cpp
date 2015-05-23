#include "stdafx.h"
#include "resource.h"
#include "actions.h"
#include "nameconsts.h"
#include "image5.h"
#include "data5.h"
#include "misc_utils.h"
#include "misc_templ.h"
#include "core5.h"
#include "Classes5.h"
#include "docview5.h"
#include "calibrint.h"
#include "class_utils.h"
#include "stdio.h"
#include "fcntl.h"
#include "sys/stat.h"
#include "LearningInfo.h"
#include "StatFile.h"
#include "MessageException.h"
#include "../../Chromo/ChromosEx/consts.h"
#include "CarioInt.h"
//#include "TimeTest.h"

static bool IsActionEnabledOnDoc(IUnknown *punkDoc)
{
	bool bAnyObjects = false;
	IUnknownPtr punkOL(::GetActiveObjectFromDocument(punkDoc, szTypeObjectList), false);
	if (punkOL != 0)
	{
		sptrINamedDataObject2 sptrNDO2ObjList(punkOL);
		long lObjects;
		if (SUCCEEDED(sptrNDO2ObjList->GetChildsCount(&lObjects)) && lObjects > 0)
			bAnyObjects = true;
	}
	IUnknownPtr punkView;
	IDocumentSitePtr	ptrDocSite(punkDoc);
	ptrDocSite->GetActiveView(&punkView);
	IImageViewPtr sptrImageView(punkView);
	if (sptrImageView != 0)
	{
		DWORD dwFlags;
		HRESULT hr = sptrImageView->GetImageShowFlags(&dwFlags);
		if (SUCCEEDED(hr) && (dwFlags & ~isfSourceImage))
			return bAnyObjects;
	}
	else if (::CheckInterface(punkView, IID_ICarioView) || 
		CheckInterface(punkView, IID_IMeasureView))
			return bAnyObjects;
	return false;
};

_bstr_t _GetArgString(CAction *pact, const char *pszArgName)
{
	_bstr_t bstr = pact->GetArgString(pszArgName);
	if (bstr.length() <= 0)
	{
		char szBuffer[200];
		sprintf(szBuffer, "No argument %s in action", pszArgName);
		throw new CMessageException(szBuffer);
	}
	else
		return bstr;
}

IUnknownPtr _CreateTypedObject(_bstr_t bstrName)
{
	IUnknown *punk = CreateTypedObject(bstrName);
	IUnknownPtr sptr(punk);
	if (punk) punk->Release();
	return sptr;
}

IUnknownPtr _GetObjectByName( IUnknown *punkFrom, const BSTR bstrObject, const BSTR bstrType, bool bThrowException = true)
{
	_bstr_t	bstrObjectName = bstrObject;
	_bstr_t	bstrTypeName = bstrType;
	sptrIDataTypeManager	ptrDataType( punkFrom );

	long	nTypesCounter = 0;
	ptrDataType->GetTypesCount( &nTypesCounter );

	for( long nType = 0; nType < nTypesCounter; nType ++ )
	{
		BSTR	bstrCurTypeName = 0;
		ptrDataType->GetType( nType, &bstrCurTypeName );

		_bstr_t	str( bstrCurTypeName, false );

		if( bstrType && str != bstrTypeName )
			continue;

		IUnknown	*punkObj = 0;
		LONG_PTR	dwPos = 0;

		ptrDataType->GetObjectFirstPosition( nType, &dwPos );

		while( dwPos )
		{
			punkObj = 0;
			ptrDataType->GetNextObject( nType, &dwPos, &punkObj );

			if (bstrObjectName.length() == 0 || ::GetName( punkObj ) == bstrObjectName)
			{
				IUnknownPtr sptr(punkObj);
				if (punkObj) punkObj->Release();
				return sptr;
			}

			INamedDataObject2Ptr	ptrN( punkObj );
			punkObj->Release();

			POSITION lpos = 0;
			ptrN->GetFirstChildPosition( &lpos );

			while( lpos )
			{
				IUnknown	*punkObject = 0;
				ptrN->GetNextChild( &lpos, &punkObject );

				if (bstrObjectName.length() == 0 || ::GetName( punkObject ) == bstrObjectName)
				{
					IUnknownPtr sptrO(punkObject);
					punkObject->Release();
					return sptrO;
				}

				punkObject->Release();


			}

			
		}
	}
	// Object not found. Throw exception if needed.
	if (bThrowException)
	{
		char szBuffer[500];
		_bstr_t bstrDocName = ::GetName(punkFrom);
		if (bstrDocName.length() <= 0)
			bstrDocName = "document";
		if (bstrTypeName.length() > 0)
			sprintf(szBuffer, "Object %s (%s) not found in %s", (const char *)bstrObjectName, (const char *)bstrTypeName,
				(const char *)bstrDocName);
		else
			sprintf(szBuffer, "Object %s not found in %s", (const char *)bstrObjectName, (const char *)bstrDocName);
		throw new CMessageException(szBuffer);
	}
	return 0;
}

void RedrawObjectList(IUnknown *punkObjList)
{
	sptrINamedDataObject2 sptrNDO2Obj(punkObjList);
	// Now all OK. Redraw document.
	IUnknown *punk = NULL;
	HRESULT hr = sptrNDO2Obj->GetData(&punk);
	if (SUCCEEDED(hr) && punk)
	{
		long l = cncReset;
		INotifyControllerPtr sptr(punk);
		sptr->FireEvent(_bstr_t(szEventChangeObjectList), NULL, sptrNDO2Obj, &l, 0);
		punk->Release();
	}
}

void RedrawDocument(IUnknown *punkDoc)
{
	IUnknown *punk = ::GetActiveObjectFromDocument(punkDoc, szTypeObjectList);
	RedrawObjectList(punk);
	if (punk) punk->Release();
}

void RedrawContext(IUnknown *punkView)
{
	IUnknown *punk = ::GetActiveObjectFromContext(punkView, szTypeObjectList);
	RedrawObjectList(punk);
	if (punk) punk->Release();
}


IUnknown *CObjIntActionBase::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CInteractiveAction::DoGetInterface( iid );
}

HRESULT CObjIntActionBase::DoInvoke()
{
	//get the target document
	IUnknownPtr ptrDoc;
	IViewPtr sptrV(m_ptrTarget);
	sptrV->GetDocument(&ptrDoc);
	sptrIDocument	sptrD(ptrDoc);
	//change the target - now it is document
	m_guidTarget = ::GetKey(sptrD);
	SetModified(true);
	return S_OK;
}

IUnknownPtr CObjIntActionBase::GetFoundObject()
{
	IUnknown *punk = ::GetActiveObjectFromContext(m_ptrTarget, szTypeObjectList);
	sptrINamedDataObject2 sptrNDO2Obj(punk);
	if (!punk) return 0;
	punk->Release();
	POSITION pos = 0;
	int nObjNo = 0;
	sptrNDO2Obj->GetFirstChildPosition(&pos);
	while (pos)
	{
		IUnknownPtr sptr;
		sptrNDO2Obj->GetNextChild(&pos, &sptr);
		if (nObjNo++ == m_nFoundObjNo)
			return sptr;
	}
	return 0;
}

static int FindObjNo(INamedDataObject2 *pNDO2, TPOS posFind)
{
	POSITION pos = 0;
	int nObjNo = 0;
	pNDO2->GetFirstChildPosition(&pos);
	while (pos)
	{
		if (pos == posFind)
			return nObjNo;
		IUnknownPtr sptr;
		pNDO2->GetNextChild(&pos, &sptr);
	}
	return -1;
}

bool CObjIntActionBase::_DoLButtonDownOnObject( _point point, TPOS &posObj,
	INamedDataObject2 *pNDO2, bool bSetActive, int iObjNo)
{
	TPOS pos1 = posObj;
	IUnknownPtr sptr;
	pNDO2->GetNextChild((POSITION*)&posObj, &sptr);
	IRectObjectPtr sptrROObj(sptr);
	bool b = sptrROObj;
	if (b)
	{
		long lHitTest;
		sptrROObj->HitTest(point, &lHitTest);
		if (lHitTest != 0)
		{
			if (bSetActive) pNDO2->SetActiveChild(pos1);
			if (DoLButtonDownOnObject(point, sptr, pNDO2))
			{
				if (iObjNo == -1)
					m_nFoundObjNo =  FindObjNo(pNDO2, pos1);
				else
					m_nFoundObjNo = iObjNo;
				// Now all OK. Redraw document.
				RedrawObjectList(pNDO2);
				SetModified(true);
				Finalize();
				pNDO2->SetActiveChild(pos1);
				return true;
			}
		}
	}
	return false;
}


bool CObjIntActionBase::DoLButtonDown( _point point )
{
	// If karyogram active, spectial treatment.
	ICarioViewPtr sptrCario(m_ptrTarget);
	if (sptrCario != 0)
	{
		int nLine,nCell,nChromoObj;
		sptrCario->GetCellByPoint(point, &nLine, &nCell, &nChromoObj, FALSE );
		if (nLine == -1 || nCell == -1 || nChromoObj == -1)
			return false;
		IUnknownPtr punkChr,punkObj,punkList;
		sptrCario->GetChromoObject(nLine, nCell, nChromoObj, &punkChr);
		INamedDataObject2Ptr sptrNDO2Chr(punkChr);
		sptrNDO2Chr->GetParent(&punkObj);
		INamedDataObject2Ptr sptrNDO2Obj(punkObj);
		sptrNDO2Obj->GetParent(&punkList);
		INamedDataObject2Ptr sptrNDO2List(punkList);
		if (DoLButtonDownOnObject(point, punkObj, sptrNDO2List))
		{
			sptrCario->SetActiveChromo(nLine, nCell, nChromoObj);
			IWindow2Ptr wnd(sptrCario);
			HWND hwnd;
			wnd->GetHandle((HANDLE*)&hwnd);
			::InvalidateRect(hwnd, NULL, TRUE);
			SetModified(true);
			Finalize();
			return true;
		}
		return false;
	}
	// Find active objects list
	IUnknown *punk = ::GetActiveObjectFromContext(m_ptrTarget, szTypeObjectList);
	if (!punk) return 0;
	sptrINamedDataObject2 sptrNDO2Obj(punk);
	if (punk) punk->Release();
	TPOS posActive;
	sptrNDO2Obj->GetActiveChild(&posActive);
	if (posActive != 0)
	{
		if (_DoLButtonDownOnObject(point, posActive, sptrNDO2Obj, false, -1))
			return true;
	}
	// Now iterate list
	TPOS pos = 0;
	int nObjNo = 0;
	sptrNDO2Obj->GetFirstChildPosition(&pos);
	while (pos)
	{
		if (_DoLButtonDownOnObject(point, pos, sptrNDO2Obj, false, nObjNo))
			break;
		nObjNo++;
	}
	return true;
}

HRESULT CObjIntActionBase::GetActionState(DWORD *pdwState)
{
	*pdwState = 0;
	bool bAnyObjects = false;
	IUnknownPtr punk(::GetActiveObjectFromContext(m_ptrTarget, szTypeObjectList), false);
	sptrINamedDataObject2 sptrNDO2ObjList(punk);
	if (sptrNDO2ObjList != 0)
	{
		long lObjects;
		if (SUCCEEDED(sptrNDO2ObjList->GetChildsCount(&lObjects)) && lObjects > 0)
			bAnyObjects = true;
	}
	IImageViewPtr sptrImageView(m_ptrTarget);
	if (sptrImageView != 0)
	{
		DWORD dwFlags;
		HRESULT hr = sptrImageView->GetImageShowFlags(&dwFlags);
		if (SUCCEEDED(hr) && (dwFlags & ~isfSourceImage))
		{
			if (bAnyObjects) *pdwState = afEnabled;
		}
	}
	else if (::CheckInterface(m_ptrTarget, IID_ICarioView))
	{
		if (bAnyObjects) *pdwState = afEnabled;
	}


/*	IUnknownPtr punk(::GetActiveObjectFromContext(m_ptrTarget, szTypeObjectList), false);
	if (punk != 0)
	{
		sptrINamedDataObject2 sptrNDO2ObjList(punk);
		long lObjects;
		if (SUCCEEDED(sptrNDO2ObjList->GetChildsCount(&lObjects)) && lObjects > 0)
		{
			IMeasureViewPtr sptrMeasView(m_ptrTarget);
			if (sptrMeasView == 0)
				*pdwState = afEnabled;
		}
	}*/
	return S_OK;
}

LRESULT	CObjIntActionBase::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	if (nMsg == WM_SETCURSOR)
		if (DoSetCursor())
			return 1L;
	return CInteractiveAction::DoMessage(nMsg,wParam,lParam);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionClassify
////////////////////////////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CActionClassifyInfo::s_pargs[] = 
{
	{0, 0, 0, false, false },
};


CActionClassify::CActionClassify()
{
}

HRESULT CActionClassify::GetActionState(DWORD *pdwState)
{
	*pdwState = 0;
	IUnknownPtr punk(::GetActiveObjectFromDocument(m_ptrTarget, szTypeObjectList), false);
	if (punk != 0)
	{
		sptrINamedDataObject2 sptrNDO2ObjList(punk);
		long lObjects;
		if (SUCCEEDED(sptrNDO2ObjList->GetChildsCount(&lObjects)) && lObjects > 0)
			*pdwState = afEnabled;
	}
	return S_OK;
}

HRESULT CActionClassify::DoInvoke()
{
	try
	{
		// Find object list
		m_punkObjList = IUnknownPtr(::GetActiveObjectFromDocument(m_ptrTarget, szTypeObjectList), false);
		// Process classification itself.
		BOOL bRearrange = ::GetValueInt(::GetAppUnknown(), "Classification", "Rearrange", TRUE);
		DoClassify(m_punkObjList, m_ptrTarget, m_arrClasses, bRearrange?true:false);
		// Now all OK. Redraw document.
		INamedDataObject2Ptr NDO(m_punkObjList);
		IUnknown *punk = NULL;
		HRESULT hr = NDO->GetData(&punk);
		if (SUCCEEDED(hr) && punk)
		{
			long l = cncReset;
			INotifyControllerPtr sptr(punk);
			sptr->FireEvent(_bstr_t(szEventChangeObjectList), NULL, NDO, &l, 0);
			punk->Release();
		}
		SetModified(true);
	}
	catch(CMessageException *e)
	{
		return HandleExc(e, E_UNEXPECTED);
	}
	return S_OK;
}

IUnknown *CActionClassify::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CAction::DoGetInterface( iid );
}


HRESULT CActionClassify::DoUndo()
{
	SetClasses(m_punkObjList, m_arrClasses);
	SetModified(false);
	return S_OK;
}

HRESULT CActionClassify::DoRedo()
{
	SetClasses(m_punkObjList, m_arrClasses);
	SetModified(true);
	return S_OK;
}





////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionTeach
////////////////////////////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CActionTeachInfo::s_pargs[] = 
{
	{"ShowMessage", szArgumentTypeInt, 0, true, true },	
	{0, 0, 0, false, false },
};


CActionTeach::CActionTeach()
{
}

bool SetClassifyResult(double dResult);
HRESULT CActionTeach::DoInvoke()
{
	long nShowMessage = GetArgLong("ShowMessage");
	try
	{
		_bstr_t bstrClassFile = GetValueString(GetAppUnknown(),"Classes","ClassFile","Classes\\Classes.ini");
		CLearningInfo LearningInfo(NULL);
		LearningInfo.ReadData();
		// If learning database is empty, teaching will be invalid.
		if (LearningInfo.m_nObjectsCount == 0 || LearningInfo.m_FeaturesArray.GetSize() == 0 ||
			LearningInfo.m_FeaturesArray.nFeatures == 0)
			throw new CMessageException(IDS_LEARNDB_EMPTY);
		// If all objects not classified or same class, teaching will be invalid.
		int nClass = LearningInfo.m_FeaturesArray[0].nClass;
		bool bDiffClasses = false;
		for (int i = 1; i < LearningInfo.m_FeaturesArray.GetSize(); i++)
			if (LearningInfo.m_FeaturesArray[i].nClass != nClass)
			{
				bDiffClasses = true;
				break;
			}
		if (!bDiffClasses && nShowMessage)
			throw new CMessageException(IDS_LEARNDB_SAME_CLASS);
		VcvParamsHld Params;
		Params.nClasses = LearningInfo.m_nClassesCount;
		Params.nDim = LearningInfo.m_FeaturesArray.nFeatures;
		_StatFile Stat("train.txt", Params.nStat);
		resultdata r;
		TeachClassifier((VcvParams*)&Params, LearningInfo.m_FeaturesArray.GetSize(), LearningInfo.m_FeaturesArray.GetData(),
			LearningInfo.m_FeaturesArray.plFeatKeys, &r, Stat.f);
		if (::GetValueInt(::GetAppUnknown(), "Classification", "ShowResult", 0))
			SetClassifyResult(r.AvrV);
		SetValue(GetAppUnknown(), "Classification", "LastResult", r.AvrV);
	}
	catch(CMessageException *e)
	{
		return HandleExc(e, E_UNEXPECTED);
	}
	return S_OK;
}



_ainfo_base::arg	CActionEmptyClassifierInfo::s_pargs[] = 
{
	{0, 0, 0, false, false },
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionEmptyClassifier
////////////////////////////////////////////////////////////////////////////////////////////////////
CActionEmptyClassifier::CActionEmptyClassifier() : m_LearningInfoState(false)
{
}

HRESULT CActionEmptyClassifier::DoInvoke()
{
//	BOOL bMessages = ::GetValueInt(::GetAppUnknown(), "Classification", "Messages", 0);
	try
	{
		_bstr_t bstrClassFile = GetValueString(GetAppUnknown(),"Classes","ClassFile","Classes\\Classes.ini");
		CLearningInfo LearningInfo(bstrClassFile);
		LearningInfo.Empty(m_LearningInfoState);
		//get the target document
		IUnknownPtr ptrDoc;
		IApplicationPtr sptrApp(m_ptrTarget);
		sptrApp->GetActiveDocument(&ptrDoc);
		sptrIDocument sptrD(ptrDoc);
		//change the target - now it is document
		m_guidTarget = ::GetKey(sptrD);
	}
	catch(CMessageException *e)
	{
		return HandleExc(e, E_UNEXPECTED);
	}
	return S_OK;
}

IUnknown *CActionEmptyClassifier::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CAction::DoGetInterface( iid );
}


HRESULT CActionEmptyClassifier::DoUndo()
{
	m_LearningInfoState.DoUndoRedo(true);
	return S_OK;
}

HRESULT CActionEmptyClassifier::DoRedo()
{
	m_LearningInfoState.DoUndoRedo(false);
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionAddObject
////////////////////////////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CActionAddObjectInfo::s_pargs[] = 
{
	{0, 0, 0, false, false },
};


CActionAddObject::CActionAddObject()
{
	m_hcurDefault = LoadCursor(app::handle(), MAKEINTRESOURCE(IDC_ADD_LEARN));
}

HRESULT CActionAddObject::DoInvoke()
{
	//get the target document
	IUnknownPtr ptrDoc;
	IViewPtr sptrV(m_ptrTarget);
	sptrV->GetDocument(&ptrDoc);
	sptrIDocument	sptrD(ptrDoc);
	//change the target - now it is document
	m_guidTarget = ::GetKey(sptrD);
	return S_OK;
}

bool CActionAddObject::DoLButtonDownOnObject( _point point, IUnknown *punkObj, IUnknown *punkObjList )
{
	try
	{
		_bstr_t bstrClassFile = GetValueString(GetAppUnknown(),"Classes","ClassFile","Classes\\Classes.ini");
		CLearningInfo LearnInfo(bstrClassFile,punkObjList,&m_LearningInfoState);
		if (!LearnInfo.AddObjectToLearnInfo(punkObj))
		{
			_MessageBoxCaption MsgBoxCaption;
			VTMessageBox(IDS_OBJECT_UNKNOWN, app::get_instance()->handle(), MsgBoxCaption, MB_OK|MB_ICONEXCLAMATION);
			return false;
		}
		return true;
	}
	catch(CMessageException *e)
	{
		e->ReportError();
		delete e;
		return false;
	}
}

IUnknown *CActionAddObject::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CInteractiveAction::DoGetInterface( iid );
}


HRESULT CActionAddObject::DoUndo()
{
	m_LearningInfoState.DoUndoRedo(true);
	return S_OK;
}

HRESULT CActionAddObject::DoRedo()
{
	m_LearningInfoState.DoUndoRedo(false);
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionAddAllObjects
////////////////////////////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CActionAddAllObjectsInfo::s_pargs[] = 
{
	{0, 0, 0, false, false },
};


CActionAddAllObjects::CActionAddAllObjects()
{
}

IUnknown *CActionAddAllObjects::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CAction::DoGetInterface( iid );
}

HRESULT CActionAddAllObjects::DoInvoke()
{
	_bstr_t bstrClassFile = GetValueString(GetAppUnknown(),"Classes","ClassFile","Classes\\Classes.ini");
	try
	{
		IUnknown *punk = ::GetActiveObjectFromDocument(m_ptrTarget, szTypeObjectList);
		sptrINamedDataObject2 sptrNDO2Obj(punk);
		if (punk) punk->Release();
		CLearningInfo LearnInfo(bstrClassFile,sptrNDO2Obj,&m_LearningInfoState);
		TPOS pos = 0;
		bool bOk = true;
		sptrNDO2Obj->GetFirstChildPosition(&pos);
		while (pos)
		{
			IUnknownPtr sptr;
			sptrNDO2Obj->GetNextChild(&pos, &sptr);
			bOk &= LearnInfo.AddObjectToLearnInfo(sptr);
		}
		if (!bOk && _GetClassifierInt("Classification", "Messages", 0, bstrClassFile))
		{
			_MessageBoxCaption MsgBoxCaption;
			VTMessageBox(IDS_SOME_OBJECTS_UNKNOWN, app::get_instance()->handle(), MsgBoxCaption, MB_OK|MB_ICONEXCLAMATION);
		}
	}
	catch(CMessageException *e)
	{
		return HandleExc(e, E_UNEXPECTED, bstrClassFile);
	}
	return S_OK;
}

HRESULT CActionAddAllObjects::DoUndo()
{
	m_LearningInfoState.DoUndoRedo(true);
	return S_OK;
}

HRESULT CActionAddAllObjects::DoRedo()
{
	m_LearningInfoState.DoUndoRedo(false);
	return S_OK;
}

HRESULT CActionAddAllObjects::GetActionState(DWORD *pdwState)
{
	*pdwState = 0;
	if (IsActionEnabledOnDoc(m_ptrTarget))
		*pdwState = afEnabled;
	return S_OK;
}

