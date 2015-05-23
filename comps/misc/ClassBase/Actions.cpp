#include "stdafx.h"
#include "Actions.h"
#include "object5.h"

#include "class_utils.h"
#include "ClassSubMenu.h"

#include <atlstr.h>
#include "ScriptNotifyInt.h"
#include "classify_int.h"
#include "helper.h"

//DWORD check_addref(IUnknown* unk)
//{
//	if(unk==0) return 0;
//	unk->AddRef();
//	return unk->Release();
//}

_ainfo_base::arg CSwitchClassFileInfo::s_pargs[] =
{
	{"ClassFile",szArgumentTypeString, "", true, false },
	{"ObjectList",	szArgumentTypeObjectList, 0, true, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg CObjectSetClassInfo::s_pargs[] =
{
	{"Class",szArgumentTypeInt, "-1", true, false },
	{"ClassFile",szArgumentTypeString, "", true, false },
	{"Deselect",szArgumentTypeInt, "0", true, false },
	{0, 0, 0, false, false },
};

// vanek - 31.10.2003
_ainfo_base::arg CActionGetObjectDescriptionInfo::s_pargs[] =
{
	{"ObjectListName",szArgumentTypeString, "", true, false },
	{"Class",szArgumentTypeInt, "0", true, false },
	{0, 0, 0, false, false },
};

HRESULT CObjectSetClassInfo::GetUIUnknown( IUnknown **ppunkinterface )
{
	if(ppunkinterface==0) return E_INVALIDARG;
	CClassSubMenu *pMenu = new CClassSubMenu;
	*ppunkinterface = pMenu->Unknown();
	return S_OK;
};

CObjectSetClass::CObjectSetClass()
{
	m_ptrList=0;
	ICalcObjectPtr m_ptrObject=0;
	m_bstrClassFile="";
	m_bPhased = false;
}

CObjectSetClass::~CObjectSetClass()
{
	DeInit();
}

IUnknown* CObjectSetClass::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	return CAction::DoGetInterface( iid );
}

HRESULT CObjectSetClass::GetActionState( DWORD *pdwState )
{
	if( !pdwState )	return E_POINTER;
	*pdwState = 0;
	IUnknownPtr punkOL(::GetActiveObjectFromDocument(m_ptrTarget, szTypeObjectList), false);
	if (punkOL == 0) return S_OK;
	INamedDataObject2Ptr ptrList(punkOL);
	if (ptrList == 0) return S_OK;
	TPOS lPos = 0;
	ptrList->GetActiveChild(&lPos);
	*pdwState = lPos==0?0:afEnabled;
	return S_OK;
}

HRESULT CObjectSetClass::DoInvoke()
{

	m_lClass=GetArgLong("Class");
	m_bstrClassFile=GetArgString("ClassFile");
	bool bDeselect = GetArgLong("Deselect")!=0;
	m_manual_flag = m_lClass!=-1;

	IUnknown* punkObj = 0;
	punkObj = ::GetActiveObjectFromDocument(m_ptrTarget, szTypeObjectList);
	m_ptrList = punkObj;
	if(punkObj) punkObj->Release();
	if(m_ptrList==0) return E_FAIL;

	TPOS lPos=0;
	m_ptrList->GetActiveChild(&lPos);
	if(0==lPos) return E_FAIL;

	IUnknown* unk;
	m_ptrList->GetNextChild(&lPos, &unk);
	m_ptrObject = unk;
	if(unk) unk->Release();
	if(m_ptrObject==0) return E_FAIL;

	
	IUnknown* object =0;
	if(is_phased(m_ptrList))
	{

		CString s((char*)m_bstrClassFile);
		PtrList affected;
		object = SetPhaseClass(m_ptrList, m_ptrObject, s, m_lClass, affected);

		if(object!=0)
		{
			m_ptrObject = object;
			object->Release();
			/*DWORD ref = check_addref(object);*/
			PrepareTarget(true);

			POS pos = affected.GetHeadPos();
			while(pos)
			{
				IUnknown* obj = (IUnknown*)affected.GetNextPtr(pos);
				INamedDataObject2Ptr ndo(obj);
				RemoveFromDocData(m_ptrTarget, obj);
				
			}
			m_bPhased = true;
			/*ref = check_addref(object);*/
			SetToDocData(m_ptrTarget, m_ptrObject);
			/*IDataObjectListPtr(m_ptrList)->UpdateObject(m_ptrObject);*/
			INotifyController2Ptr nc(m_ptrTarget);
			if(nc)
			{
				long code = cncChange;
				SetModified( true );

	
	
			FireEvent( m_ptrTarget, szEventChangeObjectList, m_ptrList, 0, &code, sizeof( code ));
				
			}
			
			POSITION p;
			INamedDataObject2Ptr(m_ptrObject)->GetObjectPosInParent(&p);
			m_ptrList->SetActiveChild(p);
			
			PrepareTarget(false);
			/*ref = check_addref(object);*/
			
//			__asm nop
		}

	}
	if(object==0)
	{
		m_bPhased = false;
		m_undo_lClass=get_object_class(m_ptrObject, m_bstrClassFile);
		m_undo_manual_flag=is_object_class_manual(m_ptrObject, m_bstrClassFile);
		if(bDeselect) m_ptrList->SetActiveChild(0);// если надо задеселектить - сделаем это до реального выполнения
	}

	 
	
	set_object_class(m_ptrObject, m_lClass, m_bstrClassFile);
	set_object_class_manual(m_ptrObject, m_manual_flag, m_bstrClassFile);

	m_ptrList->SetModifiedFlag( true );
	SetModified( true );
	NotifyParent();	//просигналим, чтоб обновить вьюху
	
	return S_OK;
}

HRESULT CObjectSetClass::Undo()
{	
	if(m_ptrList==0) return E_FAIL;
	

	if(!m_bPhased)
	{

		if(m_ptrObject==0) return E_FAIL;

		set_object_class(m_ptrObject, m_undo_lClass, m_bstrClassFile);
		set_object_class_manual(m_ptrObject, m_undo_manual_flag, m_bstrClassFile);
	}
	else
	{
		PrepareTarget(true );
		CUndoneableActionImpl::DoUndo();
		PrepareTarget(false );
	}

	m_ptrList->SetModifiedFlag( true );
	SetModified( true );
	NotifyParent();	//просигналим, чтоб обновить вьюху

	return S_OK;
}

HRESULT CObjectSetClass::Redo()
{
	if(m_ptrList==0) return E_FAIL;
	

	if(!m_bPhased)
	{
		if(m_ptrObject==0) return E_FAIL;
		set_object_class(m_ptrObject, m_lClass, m_bstrClassFile);
		set_object_class_manual(m_ptrObject, m_manual_flag, m_bstrClassFile);
	}
	else
	{
		PrepareTarget(true );
		CUndoneableActionImpl::DoRedo();
		PrepareTarget(false );
	}

	m_ptrList->SetModifiedFlag( true );
	SetModified( true );
	NotifyParent();	//просигналим, чтоб обновить вьюху

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////
// CActionGetObjectDescr implementation
HRESULT CActionGetObjectDescription::DoInvoke()
{
	_bstr_t bstrObjListName = GetArgString("ObjectListName");
	long lClass = GetArgLong( "Class" );

	if( lClass == -1 )
		return S_OK; // не работаем для (-1)-ого класса  

	// get classifier
	_bstr_t bstrClassFile = ::GetValueString( ::GetAppUnknown(), "\\Classes", "ClassFile", "" );
	CString strProgID = _get_value_string( (char*)(bstrClassFile), ID_SECT_GENERAL, ID_ENTR_CLASSIFY_PROGID );

	if( strProgID.IsEmpty() )
		return S_FALSE;

	IUnknownPtr sptrUnk( strProgID, 0, CLSCTX_INPROC_SERVER );
	if( sptrUnk == 0 )
		return S_FALSE;
	
	IClassifyProxyPtr sptrProxy = sptrUnk;
	if( sptrProxy == 0 )
		return S_FALSE;

	IClassifyInfoProxyPtr sptrInfoProxy = sptrProxy;
	if( sptrInfoProxy == 0 )
		return S_FALSE;
    
	if( sptrProxy->Load( bstrClassFile ) != S_OK )
		return S_FALSE;

	IClassifyParamsPtr sptrClassifyParams = sptrInfoProxy;
	if( sptrClassifyParams == 0 )
		return S_FALSE;

    long lClassCount = 0;
	if( S_OK != sptrClassifyParams->GetClasses( 0, &lClassCount ) )
		return S_FALSE;
    
	BSTR bstrDescription = 0;
	BOOL bNeedNotify = FALSE;

	if( lClass < lClassCount )
	{

		// get objectlist
		IUnknown *punkObjList = 0;         
		if( bstrObjListName.length() )
			punkObjList = ::GetObjectByName( m_ptrTarget, bstrObjListName, _bstr_t( szTypeObjectList ) );
		else
			punkObjList = GetActiveObjectFromDocument( m_ptrTarget, szTypeObjectList );
		
		if( !punkObjList )
			return S_FALSE;

		INamedDataObject2Ptr ptrList = punkObjList;

		if( punkObjList )
			punkObjList->Release(); punkObjList = 0;

		if( ptrList == 0 )
			return S_FALSE;


		// active object from objectlist
		IUnknownPtr sptrObject;
		POSITION lPosObj = 0;
		ptrList->GetActiveChild( &lPosObj );
		if( !lPosObj )
			return S_FALSE;

		if( S_OK != ptrList->GetNextChild( &lPosObj, &sptrObject ) )
			return S_FALSE;

		if( sptrObject == 0 )
			return S_FALSE;

		if( S_OK != sptrInfoProxy->PrepareObjectDescrByClass( sptrObject, lClass ) )
			return S_FALSE;

		sptrProxy->Start();
		
		if( sptrProxy->Process( sptrObject ) != S_OK )
			return S_FALSE;   

		sptrProxy->Finish();

		long lObjClass = -1;
		sptrProxy->GetObjectClass( sptrObject, &lObjClass );
	    
		if( lClass != lObjClass )
			bNeedNotify = (S_OK == sptrInfoProxy->GetObjectDescrByClass( &bstrDescription ));
		else  // если класс запроса и класс объекта совпадают - возвращаем пустую строку
		{
			bstrDescription = _bstr_t( _T("") ).Detach();
			bNeedNotify = TRUE;
		}
	}
	else
	{
		bstrDescription = _bstr_t( _T("error:0") ).Detach(); // текущий классификатор не содержит заданный класс
		bNeedNotify = TRUE;
	}

	if( bNeedNotify )
	{
		IScriptSitePtr	sptrScriptSite = ::GetAppUnknown();
	    if( sptrScriptSite != 0 )
		{
			_variant_t vartArg( bstrDescription );
			sptrScriptSite->Invoke( _bstr_t( "GetObjectDescr_OnComplete" ), &vartArg, 1, 0, fwFormScript | fwAppScript );
		}
	}
    
	if( bstrDescription )
	{
		::SysFreeString( bstrDescription );
		bstrDescription = 0;
	}
	
    return S_OK;
}

HRESULT CActionGetObjectDescription::GetActionState( DWORD *pdwState )
{
	if( !pdwState )	return E_POINTER;
	*pdwState = 0;

	_bstr_t bstrObjListName = GetArgString("ObjectListName");
	long lClass = GetArgLong( "Class" );
	if( lClass == -1 )
		return S_OK; // не работаем для (-1)-ого класса  
	// get objectlist
	IUnknown *punkObjList = 0;         
	if( bstrObjListName.length() )
		punkObjList = ::GetObjectByName( m_ptrTarget, bstrObjListName, _bstr_t( szTypeObjectList ) );
	else
		punkObjList = GetActiveObjectFromDocument( m_ptrTarget, szTypeObjectList );
	if( !punkObjList )
		return S_OK;
	INamedDataObject2Ptr ptrList = punkObjList;
	if( punkObjList )
		punkObjList->Release(); punkObjList = 0;
	if( ptrList == 0 )
		return S_OK;
	// active object from objectlist
	TPOS lPosObj = 0;
	ptrList->GetActiveChild( &lPosObj );
	*pdwState = lPosObj==0?0:afEnabled;
	return S_OK;
}


static GuidKey GetObjectKey( IUnknown* pUnk )
{
	GuidKey key;
	if(pUnk==0) return key;
	INumeredObjectPtr no(pUnk);
	
	if(no)
	{
		no->GetKey(&key);
	}
	return key;
}

IUnknown* SetPhaseClass(IUnknown* olist, IUnknown* object, CString& classifier, int nClass, PtrList& affected)
{

	IUnknown* newObject =0;

	INamedDataObject2Ptr ndo(olist);
	if(ndo==0) return 0;

	IMeasureObject2Ptr mo(object);
	if(mo==0) return 0;

	IUnknown* pImage;
	mo->GetImage(&pImage);
	if(pImage==0) return 0;

	IImage4Ptr image =pImage;
	pImage->Release();


	GuidKey key = GetObjectKey( object );

	if(nClass==-1) return 0;

	POSITION pos =0;
	ndo->GetFirstChildPosition(&pos);
	IUnknown* pUnk =0;
	while(pos)
	{
		ndo->GetNextChild(&pos, &pUnk);
		if( GetObjectKey(pUnk)== key) 
		{
			pUnk->Release();
			continue;
		}
		if(get_object_class(ICalcObject2Ptr(pUnk), classifier) == nClass)
		{
			IMeasureObject2Ptr moOther(pUnk);
			if(moOther ==0) 
				continue;

			moOther->GetImage(&pImage);
			if(pImage==0) 
				continue;
			IImage4Ptr imageOther = pImage;
			pImage->Release();
			RECT rectOther, rect, rectNew;

			IRectObjectPtr ro;

			ro =imageOther; 
			ro->GetRect(&rectOther);

			ro = image;
			ro->GetRect(&rect);

			::UnionRect(&rectNew, &rectOther, &rect);

			pImage = CreateTypedObject(bstr_t( szTypeImage));
			IImage4Ptr newImage(pImage);
			pImage->Release();

			INamedDataObject2Ptr ndo(image);
			IUnknown *parentUnk =0;
			ndo->GetParent(&parentUnk);
			ndo = newImage;
			ndo->SetParent(parentUnk, 0);
			if(parentUnk) parentUnk->Release();

			newImage->CreateVirtual(rectNew);
			newImage->InitRowMasks();
			int CX = rectNew.right - rectNew.left, 
				CY = rectNew.bottom - rectNew.top;

			byte marker = 0x80 + nClass;
			byte* pRow, *pRowOther, *pRowNew;
			byte* dummyRow = new byte[CX];
			ZeroMemory(dummyRow, CX*sizeof(byte));
			for(int y = rectNew.top; y<rectNew.bottom;y++)
			{
				(y>=rect.bottom || y<rect.top)? *(pRow = dummyRow) : (byte)image->GetRowMask(y, &pRow);
				(y>=rectOther.bottom || y<rectOther.top)? *(pRowOther = dummyRow) : (byte)imageOther->GetRowMask(y, &pRowOther);
				newImage->GetRowMask(y, &pRowNew);

				byte bt, btOther;
				for(int x = rectNew.left; x<rectNew.right; x++, pRow++, pRowOther++, pRowNew++)
				{
					bt = (x<rect.left || x>=rect.right) ? 0: *pRow;
					btOther = (x<rectOther.left || x>=rectOther.right) ? 0: *pRowOther;
					*pRowNew = (bt>=128 || btOther>=128)? marker:0;
				}
			}
			delete[] dummyRow;
			int count =0;
			BOOL bContoured = image->GetContoursCount( &count );
			newObject = CreateTypedObject(bstr_t(szTypeObject));
			IMeasureObject2Ptr newMO = newObject;
			newMO->SetImage(newImage);
			CalculatePhase(olist, newObject, bContoured);
			
			affected.AddToTail(pUnk);
		}

		pUnk->Release();

	}
	affected.AddToTail(object);

	return newObject;
};

BOOL CalculatePhase(IUnknown* unkList, IUnknown* pUnkPhase, BOOL bContoured)
{
	IUnknown* unk = _GetOtherComponent( GetAppUnknown(), IID_IMeasureManager );
	ICompManagerPtr ptrManager= unk;
	if(unk) unk->Release();
	
	if(ptrManager==0) return 0;
	IUnknown* img =0;
	IMeasureObject2Ptr mo(pUnkPhase);
	if(mo==0) return 0;

	mo->GetImage(&img);
	IImage4Ptr image(img);
	if(img) img->Release();
	else return 0;

	image->InitContours();

	int grpCount;
	ptrManager->GetCount(&grpCount);
	IUnknown* unkGroup;
	IMeasParamGroupPtr mg;
	for(int n=0;n<grpCount;n++)
	{
		
		ptrManager->GetComponentUnknownByIdx( n, &unkGroup );
		if(unkGroup==0) continue;
		mg = unkGroup;
		unkGroup->Release();
		if(mg==0) continue;
		

		mg->InitializeCalculation(unkList);
		mg->CalcValues(pUnkPhase, image);
		mg->FinalizeCalculation();
	}	

	if(!bContoured)
	{
		image->FreeContours();
	}
	return 1;
}











