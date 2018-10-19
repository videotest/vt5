#include "StdAfx.h"
#include "method.h"
#include "Utils.h"
#define SIMPLE_TYPES_DEFINED
#include "\vt5\awin\misc_list.h"
#include "MethodDataUtil.h"
#include "\vt5\crypto\crypto.h"

////////////////////////////////////////////
BEGIN_INTERFACE_MAP(CMethod, CCmdTargetEx)
	INTERFACE_PART(CMethod, IID_IMethod, Method)
	INTERFACE_PART(CMethod, IID_IMethodData, MethodData)
	INTERFACE_PART(CMethod, IID_IMethodChanges, MethodChanges)
	INTERFACE_AGGREGATE(CMethod, m_punkNamedData)
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CMethod, Method);
IMPLEMENT_UNKNOWN(CMethod, MethodData);
IMPLEMENT_UNKNOWN(CMethod, MethodChanges);

IMPLEMENT_DYNCREATE(CMethod, CCmdTargetEx);

GUARD_IMPLEMENT_OLECREATE(CMethod, "Metodics.Method",
0xef2b7cbb, 0x4e67, 0x46eb, 0x9b, 0x2b, 0xff, 0x64, 0x95, 0xb6, 0x2c, 0xa6);

CMethod::CMethod(void)
{
	m_bstrName = "";
	m_lActiveStepPos = 0;
	m_bModifiedFlag = 0;
	m_bLockUpdateShellData = 0;
	m_bNeedUpdateShellData = 0;

	m_Steps.clear();
	m_StepsPosMap.clear();

	CMethodStep stepBeginLoop;
	stepBeginLoop.m_bstrActionName = szBeginMethodLoop;
	stepBeginLoop.m_dwFlags |= msfUndead | msfStateless;
	m_xMethodData.AddStep(&stepBeginLoop,0);

	CMethodStep stepEndLoop;
	stepEndLoop.m_bstrActionName = szEndMethodLoop;
	stepEndLoop.m_dwFlags |= msfUndead | msfStateless;
	m_xMethodData.AddStep(&stepEndLoop,0);

	m_punkNamedData = 0;

	//Create NamedData
	CLSID clsid={0};
	HRESULT hr1 = ::CLSIDFromProgID( _bstr_t(szNamedDataProgID), &clsid );
	ASSERT( S_OK == hr1 );

	HRESULT hr = CoCreateInstance(	clsid, GetControllingUnknown(), 
	CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&m_punkNamedData );	
	if(hr != S_OK)
	{
		m_punkNamedData = NULL;
		ASSERT( false );
	}

	TPOS lPos = 0;
	m_xMethodData.GetNextStep(&lPos,0);
	m_xMethodData.SetActiveStepPos(lPos, true);
}

CMethod::~CMethod(void)
{
	if (m_punkNamedData != 0)
	{
		m_punkNamedData->Release();
		m_punkNamedData = 0;
	}
}

//----------------------------------------
// IMethod interface

void CMethod::StoreToNamedData(IUnknown* punkND)
{
	::DeleteEntry( punkND, "\\_method" );

	{	// сохраним PropBag в [_method\Settings]
		long lPos = 0;
		GetFirstPropertyPos( &lPos );
		while( lPos != 0 )
		{
			_bstr_t varName;
			_variant_t varValue;
			GetNextProperty( varName.GetAddress(), &varValue, &lPos );
			::SetValue(punkND,"\\_method\\Settings",varName, varValue);
		}
	}

	::SetValue(punkND,"\\_method","StepCount", long(m_Steps.count()));
	long n; TPOS lpos;
	for(lpos=m_Steps.head(), n=0;
		lpos;
		lpos=m_Steps.next(lpos), n++)
	{
		if(lpos==m_lActiveStepPos)
			::SetValue(punkND,"\\_method","ActiveStep",long(n));
		CString strPath;
		strPath.Format("\\_method\\Step%u",n);
		CMethodStep* pStep = &(m_Steps.get(lpos));
		pStep->StoreToNamedData(punkND, strPath);
	}
}

void CMethod::LoadFromNamedData(IUnknown* punkND)
{
	{	// скопируем секцию [Metodic\Settings] в PropBag
		long count = ::GetEntriesCount(punkND,"\\_method\\Settings");
		for(long n=0; n<count; n++)
		{
			CString strName = ::GetEntryName(punkND,"\\_method\\Settings",n);
			_variant_t var;
			_variant_t varValue = ::GetValue(punkND,"\\_method\\Settings",strName, var);
			SetProperty( _bstr_t(strName), varValue );
		}
	}

	long count = ::GetValueInt(punkND,"\\_method","StepCount");
	long nActiveStep = ::GetValueInt(punkND,"\\_method","ActiveStep",-1);
	m_Steps.clear();
	m_StepsPosMap.clear();
	m_lActiveStepPos = 0;
	for(int n=0; n<count; n++)
	{
		CString strPath;
		strPath.Format("\\_method\\Step%u",n);
		CMethodStep step;
		step.LoadFromNamedData(punkND, strPath);
		TPOS lNewPos = m_Steps.add_tail(step);
		m_StepsPosMap.set(lNewPos,lNewPos);
		if(n==nActiveStep) 	m_lActiveStepPos = lNewPos;
	}
}

HRESULT CMethod::XMethod::CreateCompatible(IUnknown **ppunkObject)
{
	METHOD_PROLOGUE_EX(CMethod, Method);
	_try(CMethod::XMethod, CreateCompatible)
	{
		if(ppunkObject==0) return E_INVALIDARG;
		IMethodPtr sptr;
		sptr.CreateInstance( "Metodics.Method" );
		if( sptr == 0 ) return E_FAIL;

		sptrIFileDataObject	sptrF;
		sptrF.CreateInstance( szNamedDataProgID );
		if( sptrF == 0 ) return E_FAIL;

		pThis->StoreToNamedData(sptrF);
		sptr->LoadFromNamedData(sptrF);

		IMethodDataPtr sptrMD(sptr);
		if(sptrMD) sptrMD->SetModifiedFlag(TRUE);

		sptr->AddRef();
		*ppunkObject = sptr;
	}
	_catch return E_FAIL;

	return S_OK;
}

HRESULT CMethod::XMethod::StoreToNamedData(IUnknown* punkND)
{
	METHOD_PROLOGUE_EX(CMethod, Method);
	_try(CMethod::XMethod, StoreToNamedData)
	{
		pThis->StoreToNamedData(punkND);
	}
	_catch return E_FAIL;

	return S_OK;
}

HRESULT CMethod::XMethod::LoadFromNamedData(IUnknown* punkND)
{
	METHOD_PROLOGUE_EX(CMethod, Method);
	_try(CMethod::XMethod, LoadFromNamedData)
	{
		pThis->LoadFromNamedData(punkND);
	}
	_catch return E_FAIL;

	return S_OK;
}

HRESULT CMethod::XMethod::StoreFile(BSTR bstrFile)
{
	METHOD_PROLOGUE_EX(CMethod, Method);
	_try(CMethod::XMethod, StoreFile)
	{
		BOOL bLockOld = pThis->m_bLockUpdateShellData;
		pThis->m_bLockUpdateShellData = FALSE;
		pThis->m_xMethodData.UpdateMethod(); // обновим shell.data
		pThis->m_bLockUpdateShellData = bLockOld;

		sptrIFileDataObject3 sptrF(pThis->m_punkNamedData);
		if( sptrF == 0 ) return E_FAIL;

		sptrINamedData sptrND(pThis->m_punkNamedData);
		if( sptrND == 0 ) return E_FAIL;

		// здесь - сохраниться в собственную NamedData
		pThis->StoreToNamedData(sptrF);

		int nStealthIndex = ::GetValueInt(sptrND, "\\General", "StealthIndex",-1);
		if( nStealthIndex>=0 )
		{	// Задан индекс птички в ключе - значит, методика защищенная,
			// и сохранять ее как защищенную
			if( sptrF->SaveProtectedFile( bstrFile ) != S_OK )
				return E_FAIL;
			CString	strPathDef = ::MakeAppPathName( "Methods" )+"\\";
			CString strPath = ::GetValueString( GetAppUnknown(), "\\Paths", "Methods", 
				(const char*)strPathDef );
			CString strFileTemplate = ::GetValueString( GetAppUnknown(), "\\Methodics", "MethodTemplate",
				strPath + "vt5.method_t" );
			CString strFile = bstrFile;
			if( !PathFileExists( strFile ) )
			{ // если незащищенной части нет - создать из темплейта
				if( ! CopyFile( strFileTemplate, strFile, TRUE) )
					return E_FAIL;
			}
		}
		else
		{
			if( sptrF->SaveFile( bstrFile ) != S_OK )
				return E_FAIL;
		}
	}
	_catch return E_FAIL;

	pThis->m_xMethodData.SetModifiedFlag(FALSE);
	return S_OK;
}

HRESULT CMethod::XMethod::LoadFile(BSTR bstrFile)
{
	METHOD_PROLOGUE_EX(CMethod, Method);
	_try(CMethod::XMethod, LoadFile)
	{
		sptrIFileDataObject3 sptrF(pThis->m_punkNamedData);
		if( sptrF == 0 ) return E_FAIL;

		sptrINamedData sptrND(pThis->m_punkNamedData);
		if( sptrND == 0 ) return E_FAIL;

		int nStealthIndex = ::GetValueInt(sptrND, "\\General", "StealthIndex",-1);
		if( sptrF->LoadProtectedFile( bstrFile ) != S_OK )
			return E_FAIL;

//TODO: убрать подстановку имен из остальных мест?
		CSubstMap substMap;
		InitSubstMap( &substMap, sptrND, INamedDataPtr(::GetAppUnknown()) );
		FilterNamedData( sptrND, &substMap );

		nStealthIndex = ::GetValueInt(sptrND, "\\General", "StealthIndex",-1);
		// здесь - прочитать все из собственной NamedData
		pThis->LoadFromNamedData(sptrF);
		nStealthIndex = ::GetValueInt(sptrND, "\\General", "StealthIndex",-1);
		nStealthIndex += 0;
	}
	_catch return E_FAIL;

	pThis->m_xMethodData.SetModifiedFlag(FALSE);
	return S_OK;
}

HRESULT CMethod::XMethod::SetName(BSTR bstrName)
{
	METHOD_PROLOGUE_EX(CMethod, Method);
	pThis->m_bstrName = bstrName;

	pThis->m_xMethodData.SetModifiedFlag(TRUE);

	return S_OK;
}

HRESULT CMethod::XMethod::GetName(BSTR *pbstrName)
{
	METHOD_PROLOGUE_EX(CMethod, Method);
	if(!pbstrName) return E_INVALIDARG;
	*pbstrName = pThis->m_bstrName.copy();
	return S_OK;
}

HRESULT CMethod::XMethod::GetFullScript(BSTR *pbstrScript)
{
	METHOD_PROLOGUE_EX(CMethod, Method);
	_try(CMethod::XMethod, GetFullScript)
	{
		_bstr_t bstr("");
		for (TPOS lpos = pThis->m_Steps.head();
			lpos;
			lpos=pThis->m_Steps.next(lpos))
		{
			CMethodStep* pStep = &(pThis->m_Steps.get(lpos));
			bstr += pStep->GetScript();
		}
		*pbstrScript = bstr.copy();
	}
	_catch return E_FAIL;

	return S_OK;
}

//----------------------------------------
// IMethodData interface

HRESULT CMethod::XMethodData::GetFirstStepPos(TPOS *plPos)
{
	METHOD_PROLOGUE_EX(CMethod, MethodData);
	if(!plPos) return E_INVALIDARG;
	*plPos = pThis->m_Steps.head();
	return S_OK;
}

HRESULT CMethod::XMethodData::GetNextStep(TPOS *plPos, CMethodStep *pStep)
{
	METHOD_PROLOGUE_EX(CMethod, MethodData);
	if(!plPos) return E_INVALIDARG;

	if(*plPos==0)
	{
		if(pStep!=0)
		{
			CMethodStep step;
			step.m_bstrActionName = "";
			step.m_bstrUserName = "";
			step.m_bstrForcedName = "";
			*pStep = step;
		}
		return GetFirstStepPos(plPos);
	}

	if(!pThis->m_StepsPosMap.find(*plPos))
	{
		*plPos=0;
		CMethodStep step;
		step.m_bstrActionName = "";
		step.m_bstrUserName = "";
		step.m_bstrForcedName = "";
		if(pStep) pStep -> CopyFrom( step );
		return E_INVALIDARG;
	}

	if(pStep!=0 && *plPos!=0)
	{
		pStep -> CopyFrom( pThis->m_Steps.get(*plPos) );
	}
	*plPos = pThis->m_Steps.next(*plPos);

	return S_OK;
}

HRESULT CMethod::XMethodData::GetNextStepPtr(TPOS *plPos, CMethodStep **ppStep)
{
	METHOD_PROLOGUE_EX(CMethod, MethodData);
	if(!plPos) return E_INVALIDARG;

	if(*plPos==0)
	{
		if(ppStep!=0) *ppStep=0;
		return GetFirstStepPos(plPos); // сразу проапдейтим *plPos
	}

	if(!pThis->m_StepsPosMap.find(*plPos))
	{
		*plPos=0;
		if(ppStep!=0) *ppStep=0;
		return E_INVALIDARG;
	}

	if(ppStep!=0 && *plPos!=0)
		*ppStep = &( pThis->m_Steps.get(*plPos) );

	*plPos = pThis->m_Steps.next(*plPos);

	return S_OK;
}

HRESULT CMethod::XMethodData::GetPrevStep(TPOS *plPos, CMethodStep *pStep)
{
	METHOD_PROLOGUE_EX(CMethod, MethodData);
	if(!plPos) return E_INVALIDARG;

	if(*plPos==0)
	{
		if(pStep!=0)
		{
			CMethodStep step;
			step.m_bstrActionName = "";
			step.m_bstrUserName = "";
			step.m_bstrForcedName = "";
			*pStep = step;
		}
		*plPos = pThis->m_Steps.tail(); // сразу проапдейтим *plPos
		return S_OK;
	}

	if(!pThis->m_StepsPosMap.find(*plPos))
	{
		*plPos=0;
		CMethodStep step;
		step.m_bstrActionName = "";
		step.m_bstrUserName = "";
		step.m_bstrForcedName = "";
		if(pStep) pStep -> CopyFrom( step );
		return E_INVALIDARG;
	}

	if(pStep!=0) pStep -> CopyFrom( pThis->m_Steps.get(*plPos) );
	*plPos = pThis->m_Steps.prev(*plPos);

	return S_OK;
}

HRESULT CMethod::XMethodData::SetStep(TPOS *plPos, CMethodStep *pStep, bool bDontClearCache)
{
	METHOD_PROLOGUE_EX(CMethod, MethodData);
	if(!plPos) return E_INVALIDARG;

	if(*plPos==0) return E_INVALIDARG;

	if(!pThis->m_StepsPosMap.find(*plPos))
	{
		*plPos=0;
		return E_INVALIDARG;
	}

	if( !bDontClearCache )
	{ // удалить из кэша строки, начиная с текущей
		IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false); 
		IMethodManPtr sptrM(ptr_mtd_man);
		if(sptrM)
		{
			for (TPOS lPos2 = *plPos; lPos2 != 0; GetNextStep(&lPos2, 0))
				sptrM->DropCache(pThis->GetControllingUnknown(), lPos2);
		}
	}

	if(pStep!=0) pThis->m_Steps.get(*plPos).CopyFrom( *pStep );
	{
		IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false); 
		tagMethodStepNotification msn = {cncChange, *plPos};
		::FireEvent( ptr_mtd_man, szEventChangeMethodStep, pThis->GetControllingUnknown(), 0, &msn, sizeof(msn) );
	}

	*plPos = pThis->m_Steps.next(*plPos);

	IMethodDataPtr sptrMD(pThis->GetControllingUnknown());
	if(sptrMD) sptrMD->SetModifiedFlag(TRUE);

	return S_OK;
}

HRESULT CMethod::XMethodData::GetStepCount( long *plCount )
{
	METHOD_PROLOGUE_EX(CMethod, MethodData);
	if(!plCount) return E_INVALIDARG;

	*plCount = pThis->m_Steps.count();

	return S_OK;
}

HRESULT CMethod::XMethodData::AddStep(CMethodStep *pStep, TPOS lInsertBefore, TPOS *plNewPos)
{
	METHOD_PROLOGUE_EX(CMethod, MethodData);
	if(!pStep) return E_INVALIDARG;

	if(lInsertBefore && !pThis->m_StepsPosMap.find(lInsertBefore))
	{
		if(plNewPos) *plNewPos=0;
		return E_INVALIDARG;
	}

	TPOS lPos = pThis->m_Steps.insert_before(*pStep, lInsertBefore);
	pThis->m_StepsPosMap.set(lPos,lPos);

	if(plNewPos) *plNewPos = lPos;

	{ // удалить из кэша строки, начиная с текущей
		IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false); 
		IMethodManPtr sptrM(ptr_mtd_man);
		if(sptrM)
		{
			for (TPOS lPos2 = lPos; lPos2 != 0; GetNextStep(&lPos2, 0))
				sptrM->DropCache(pThis->GetControllingUnknown(), lPos2);
		}
	}

	{
		IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false); 
		tagMethodStepNotification msn = {cncAdd, lPos};
		::FireEvent( ptr_mtd_man, szEventChangeMethodStep, pThis->GetControllingUnknown(), 0, &msn, sizeof(msn) );
	}

	IMethodDataPtr sptrMD(pThis->GetControllingUnknown());
	if(sptrMD) sptrMD->SetModifiedFlag(TRUE);

	return S_OK;
}

HRESULT CMethod::XMethodData::DeleteStep(TPOS lPos)
{
	METHOD_PROLOGUE_EX(CMethod, MethodData);
	if(!lPos) return E_INVALIDARG;

	if(!pThis->m_StepsPosMap.find(lPos)) return E_INVALIDARG;

	{ // удалить из кэша строки, начиная с текущей
		IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false); 
		IMethodManPtr sptrM(ptr_mtd_man);
		if(sptrM)
		{
			for (TPOS lPos2 = lPos; lPos2 != 0; GetNextStep(&lPos2, 0))
				sptrM->DropCache(pThis->GetControllingUnknown(), lPos2);
		}
	}

	// если удаляем активную строку - сместим ее указатель
	if(pThis->m_lActiveStepPos == lPos)
		//SetActiveStepPos( pThis->m_Steps.next(pThis->m_lActiveStepPos) );
		pThis->m_lActiveStepPos = pThis->m_Steps.next(pThis->m_lActiveStepPos);
	{
		IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false); 
		tagMethodStepNotification msn = {cncRemove, lPos};
		::FireEvent( ptr_mtd_man, szEventChangeMethodStep, pThis->GetControllingUnknown(), 0, &msn, sizeof(msn) );
	}

	pThis->m_Steps.remove(lPos);
	pThis->m_StepsPosMap.remove_key(lPos);


	IMethodDataPtr sptrMD(pThis->GetControllingUnknown());
	if(sptrMD) sptrMD->SetModifiedFlag(TRUE);

	return S_OK;
}

HRESULT CMethod::XMethodData::SetActiveStepPos(TPOS lPos, BOOL bLoadCache)
{
	METHOD_PROLOGUE_EX(CMethod, MethodData);
	
	if(lPos!=0)
		if(!pThis->m_StepsPosMap.find(lPos)) return E_INVALIDARG;

	bool bActiveMethod=false;
	IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false); 
	IMethodManPtr sptrM(ptr_mtd_man);
	if(sptrM==0) return E_FAIL;

	TPOS lMethodPos = 0;
	sptrM->GetActiveMethodPos(&lMethodPos);
	IUnknownPtr ptrActiveMethod;
	if(lMethodPos) sptrM->GetNextMethod(&lMethodPos,&ptrActiveMethod);
	bActiveMethod = (ptrActiveMethod == pThis->GetControllingUnknown());

	// если методика активна - обновим состояние сплиттера в активном шаге, прежде чем переключаться
	// при выполнении методики не будем апдейтить состояние вьюх - только при записи на AfterStep и когда стоим
	BOOL bRunning=false, bRecording=false;
	sptrM->IsRunning(&bRunning);
	sptrM->IsRecording(&bRecording);

	//if(bActiveMethod && (!bRunning) && (!bRecording))
	//	UpdateActiveStep();  // 06.10.2005 build 91 - убрано, теперь апдейтим только при завершении записи

	pThis->m_lActiveStepPos = lPos;

	{
		IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false); 
		if(bLoadCache)
		{
			if(bActiveMethod)
			{ // если методика активна - прочитаем кэш выбранного шага
				BOOL bCached;
				sptrM->IsCached(ptrActiveMethod, lPos, &bCached);
				if(bCached) sptrM->LoadCache(pThis->GetControllingUnknown(), lPos);
				//sptrM->LoadCache(pThis->GetControllingUnknown(), lPos);
			}
		}
		tagMethodStepNotification msn = {cncActivate, lPos};
		::FireEvent( ptr_mtd_man, szEventChangeMethodStep, pThis->GetControllingUnknown(), 0, &msn, sizeof(msn) );
	}

	return S_OK;
}

HRESULT CMethod::XMethodData::GetActiveStepPos(TPOS *plPos)
{
	METHOD_PROLOGUE_EX(CMethod, MethodData);
	if(!plPos) return E_INVALIDARG;
	*plPos = pThis->m_lActiveStepPos;
	return S_OK;
}

HRESULT CMethod::XMethodData::GetModifiedFlag( BOOL *pbFlag )
{
	METHOD_PROLOGUE_EX(CMethod, MethodData);
	if(!pbFlag) return E_INVALIDARG;
	*pbFlag = pThis->m_bModifiedFlag;
	return S_OK;
}

HRESULT CMethod::XMethodData::SetModifiedFlag( BOOL bFlag )
{
	METHOD_PROLOGUE_EX(CMethod, MethodData);
	pThis->m_bModifiedFlag = bFlag;
	return S_OK;
}

HRESULT CMethod::XMethodData::GetStepIndexByPos(TPOS lPos, long *plIndex)
{
    METHOD_PROLOGUE_EX(CMethod, MethodData);

	// получить по pos'у шага его номер в методике
	if( !lPos || !plIndex ) 
		return E_INVALIDARG;

	TPOS lStepPos = 0;
	lStepPos = pThis->m_Steps.head();
	*plIndex = 0;
	while(lStepPos)
	{
		if(lStepPos == lPos) 
			return S_OK;

		lStepPos = pThis->m_Steps.next( lStepPos );
		(*plIndex) ++;
	}

	// не нашли
	*plIndex = -1;
	return S_FALSE;		
}

HRESULT CMethod::XMethodData::GetStepPosByIndex(long lIndex, TPOS *plPos)
{
	METHOD_PROLOGUE_EX(CMethod, MethodData);

	// получить по номеру шага его pos в методике
	*plPos = 0;
	if( !plPos || (lIndex < 0) || (lIndex >= pThis->m_Steps.count()) )
	   return E_INVALIDARG;
    
	*plPos = pThis->m_Steps.head();
	for(long l = 0; l < lIndex; l ++)
		*plPos = pThis->m_Steps.next( *plPos );

	return S_OK;
}

HRESULT CMethod::XMethodData::UpdateMethod()
{	// обновляет shell.data методики
	METHOD_PROLOGUE_EX(CMethod, MethodData);
	if(pThis->m_bLockUpdateShellData)
	{
		pThis->m_bNeedUpdateShellData = TRUE;
	    return S_OK;
	}
	else
	{
		//HWND	hwnd_main = 0;
		//IApplicationPtr	ptrA(GetAppUnknown());
		//ptrA->GetMainWindowHandle(&hwnd_main);
		//if( IDYES == ::MessageBox(hwnd_main, "Update method shell.data?", pThis->m_bstrName, MB_YESNO) )
		::CopyNamedData(pThis->GetControllingUnknown(), ::GetAppUnknown());

		pThis->m_bNeedUpdateShellData = FALSE;
		return S_OK;
	}
}

HRESULT CMethod::XMethodData::UpdateActiveStep()
{	// обновляет состояние вьюх (\\StateAfterAction) активного шага
	METHOD_PROLOGUE_EX(CMethod, MethodData);

	if(!pThis->m_lActiveStepPos) return E_FAIL;

	BOOL bCached = FALSE;

	IUnknownPtr ptr_mtd_man( _GetOtherComponent(GetAppUnknown(), IID_IMethodMan), false); 
	IMethodManPtr sptrM(ptr_mtd_man);
	if(sptrM==0) return E_FAIL; // вообще охренели... методман-то всяко должен быть

	TPOS pos1 = pThis->m_lActiveStepPos;
	sptrM->IsCached(pThis->GetControllingUnknown(), pos1, &bCached);
	CMethodStep* pStep;
	GetNextStepPtr(&pos1, &pStep);
	ASSERT(pStep);

	if(pStep && bCached && pStep->m_ptrNamedData!=0)
	{	// обновляем, только если шаг закэширован (это гарантирует наличие документа)
		//{ // !!! debug // если разрешить - будет SBT 1385
		//	long nIndex=-1;
		//	GetStepIndexByPos(pThis->m_lActiveStepPos, &nIndex);
		//	LPOS lPos1 = pThis->m_lActiveStepPos;
		//	CMethodStep* pStep=0;
		//	GetNextStepPtr( &lPos1, &pStep );
		//	CString s;
		//	s.Format("Updating step %i", nIndex);
		//	if(pStep)
		//	{
		//		s += " (";
		//		s += (char*)(pStep->m_bstrUserName);
		//		s += ")";
		//	}
		//	HWND	hwnd_main = 0;
		//	IApplicationPtr	ptrA(GetAppUnknown());
		//	ptrA->GetMainWindowHandle(&hwnd_main);
		//	::MessageBox(hwnd_main, s, pThis->m_bstrName, MB_OK);
		//}
		{
			::DeleteEntry( pStep->m_ptrNamedData, "\\StateAfterAction" ); // грохнем секцию - чтоб мусора не было
			bool bSimpleStoreSplitter = ::GetValueInt( ::GetAppUnknown(), "\\Methodics", "SimpleStoreSplitter", 1  ) != 0;
			StoreSplitter("\\StateAfterAction", pStep->m_ptrNamedData, bSimpleStoreSplitter); // Запомним состояние сплиттера
			SetModifiedFlag(TRUE);
		}
	}

    return S_OK;
}


CMethod::XMethodChanges::XMethodChanges() // конструктор, однако
{
	m_nUndoDepth = 100;
}

HRESULT CMethod::XMethodChanges::SetUndoDepth(long nDepth)
{
	METHOD_PROLOGUE_EX(CMethod, MethodChanges);
	m_nUndoDepth = max(0,nDepth);
	return S_OK;
};

HRESULT CMethod::XMethodChanges::AddStep(long nStep, CMethodStep *pStep)
{
	CMethodDoerAddStep *pDoer = new CMethodDoerAddStep;
	if(pDoer==0) return E_FAIL;
	pDoer->m_nIndex = nStep;
	pDoer->m_Step = *pStep;
	return Do(pDoer);
};

HRESULT CMethod::XMethodChanges::DeleteStep(long nStep)
{
	CMethodDoerDeleteStep *pDoer = new CMethodDoerDeleteStep;
	if(pDoer==0) return E_FAIL;
	pDoer->m_nIndex = nStep;
	return Do(pDoer);
};

HRESULT CMethod::XMethodChanges::MoveStep(long nStepFrom, long nStepTo, long nMoveCode)
{
	CMethodDoerMoveStep *pDoer = new CMethodDoerMoveStep;
	if(pDoer==0) return E_FAIL;
	pDoer->m_nIndex0 = nStepFrom;
	pDoer->m_nIndex1 = nStepTo;

	long nIndex0 = nStepFrom;
	do
	{ // если должны и можем продолжить имеющийся шаг - то...
		if(nMoveCode==mmscMove) break; // просто движение - отдельная запись
		TPOS lOldHead = m_undo.head();
		if(!lOldHead) break;
		CMethodDoer *pDoer0 = m_undo.get(m_undo.head());
		if(!pDoer0) break;
		if(pDoer0->GetKindName() != pDoer->GetKindName()) break; // убедились, что перед этим был Move
		CMethodDoerMoveStep* pDoerMoveStep0 = (CMethodDoerMoveStep*)pDoer0;
		if(pDoerMoveStep0->m_nIndex1 != pDoer->m_nIndex0) break; // убедились, что продолжаем перемещать тот же шаг
		nIndex0 = pDoerMoveStep0->m_nIndex0; // и дальше работаем со старым индексом
		m_undo.remove(m_undo.head());
	}
	while(0); // а вот так. просто блок - чтобы выходить из него по break

	HRESULT hr = Do(pDoer);
	pDoer->m_nIndex0 = nIndex0; // чтобы undo сработало сразу на оба блока

	if(nMoveCode==mmscFinishMove)
	{// завершаем движение - проверить, а не пустая ли запись
		if(pDoer->m_nIndex0==pDoer->m_nIndex1)
		{ // пустая запись - удалим
			if(m_undo.head()) m_undo.remove(m_undo.head());
		}
	}

	return hr;
};

HRESULT CMethod::XMethodChanges::ChangeStep(long nStep, CMethodStep *pStep, bool bDontClearCache)
{
	CMethodDoerChangeStep *pDoer = new CMethodDoerChangeStep;
	if(pDoer==0) return E_FAIL;
	pDoer->m_nIndex = nStep;
	pDoer->m_Step = *pStep;
	pDoer->m_bDontClearCache = bDontClearCache;
	return Do(pDoer);
};

HRESULT CMethod::XMethodChanges::Do(CMethodDoer* pDoer)
{	// выполнить шаг и воткнуть его в список - в head
	METHOD_PROLOGUE_EX(CMethod, MethodChanges);
	pDoer->AttachMethod(pThis);
	pDoer->Do();

	while(m_undo.count() >= m_nUndoDepth)
	{ // почистим undo - чтобы по возможности было меньше m_nUndoDepth записей
		CMethodDoer *pDoer = m_undo.get(m_undo.tail());
		if(pDoer)
		{
			if(!strcmp("Group", pDoer->GetKindName()) )
			{
				CMethodDoerGroup *pDoerGroup = (CMethodDoerGroup*)pDoer;
				if(pDoerGroup->m_bIncomplete)
					break; // Если у нас незавершенная группа - то удалять ее из списка undo нельзя
			}
		}
		m_undo.remove(m_undo.tail());
	}

	m_redo.clear(); // очистить список redo
	m_undo.add_head(pDoer);
	return S_OK;
};

HRESULT CMethod::XMethodChanges::Undo()
{
	if(!m_undo.head()) return E_FAIL;
	CMethodDoer **ppDoer = &( m_undo.get(m_undo.head()) );
	(*ppDoer)->Undo();
	m_redo.add_head(*ppDoer);
	*ppDoer=0; // дабы m_undo его не попыталось освободить
	m_undo.remove(m_undo.head());
	return S_OK;
};

HRESULT CMethod::XMethodChanges::Redo()
{
	if(!m_redo.head()) return E_FAIL;
	CMethodDoer **ppDoer = &( m_redo.get(m_redo.head()) );
	(*ppDoer)->Do();
	m_undo.add_head(*ppDoer);
	*ppDoer=0; // дабы m_undo его не попыталось освободить
	m_redo.remove(m_redo.head());
	return S_OK;
};


HRESULT CMethod::XMethodChanges::ClearUndoRedo(bool bClearUndo, bool bClearRedo)
{
	if(bClearUndo) m_undo.clear();
	if(bClearRedo) m_redo.clear();
	return S_OK;
};

HRESULT CMethod::XMethodChanges::GetUndoAvailable( long *pnCount )
{ // возвращает, сколько шагов Undo доступно
	if(!pnCount) return E_INVALIDARG;
	*pnCount = m_undo.count();
	return S_OK;
};

HRESULT CMethod::XMethodChanges::GetRedoAvailable( long *pnCount )
{ // возвращает, сколько шагов Redo доступно
	if(!pnCount) return E_INVALIDARG;
	*pnCount = m_redo.count();
	return S_OK;
};

HRESULT CMethod::XMethodChanges::BeginGroupUndo()
{ // Начать группу Undo; допускаются вложенные группы
	CMethodDoerGroup *pDoer = new CMethodDoerGroup;
	if(pDoer==0) return E_FAIL;
	pDoer->m_bIncomplete = true; // подготовить новый Doer к приему шагов
	return Do(pDoer);
}

HRESULT CMethod::XMethodChanges::EndGroupUndo()
{ // Завершить группу Undo (при этом все шаги от последнего BeginGroupUndo попадают внутрь единого шага GroupUndo)

	// пробежимся по списку, найдем созданный по BeginGroupUndo DoerGroup
	TPOS lpos = m_undo.head();
	CMethodDoerGroup* pDoerGroup = 0;
	while(lpos)
	{
		CMethodDoer *pDoer = m_undo.get(lpos);
		if(pDoer)
		{ // на всякий случай - вдруг по lpos ничего не отдадут...
			if(!strcmp("Group", pDoer->GetKindName()) )
			{ // нашли группу
				pDoerGroup = (CMethodDoerGroup*)pDoer;
				if(pDoerGroup->m_bIncomplete)
					break; // нашли, выходим из цикла
					// при этом установлены lpos и pDoerGroup
			}
		}
		lpos = m_undo.next(lpos);
		pDoerGroup=0; // просто на всякий пожарный
	}
	// Если нашли - установлены lpos, pDoerGroup
	// если не нашли - lpos==0, pDoerGroup==0

	if(!lpos || !pDoerGroup) return E_FAIL;

	lpos=m_undo.prev(lpos);
	while(lpos)
	{
		CMethodDoer **ppDoer = &( m_undo.get(lpos) );
		pDoerGroup->m_undo.add_head(*ppDoer);
		*ppDoer=0; // дабы m_undo его не попыталось освободить
		TPOS lpos1 = lpos;
		lpos = m_undo.prev(lpos);
		m_undo.remove(lpos1);
	}

	pDoerGroup->m_bIncomplete=false; // теперь шаг "готов к употреблению"

	return S_OK;
}

