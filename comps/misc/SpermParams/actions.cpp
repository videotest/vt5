#include "stdafx.h"
#include "actions.h"
#include "resource.h"
#include "math.h"
#include "NameConsts.h"
#include "float.h"
#include "\vt5\common2\misc_calibr.h"
#include "\vt5\common2\class_utils.h"
#include "units_const.h"
#include "alloc.h"

void CopyParams( ICalcObjectContainerPtr in, ICalcObjectContainerPtr out )
{
	if( in == 0 || out == 0)
		return;

	LONG_PTR lParamPos = 0;
	in->GetFirstParameterPos( &lParamPos );
	while( lParamPos )
	{
		ParameterContainer	*pc;
		in->GetNextParameter( &lParamPos, &pc );
		out->DefineParameterFull( pc, 0 );
	}
}

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionCalcPartResult,	CCmdTargetEx);

// olecreate

// {B8EE9667-7A44-4542-A711-F4F7B4FF2C40}
GUARD_IMPLEMENT_OLECREATE(CActionCalcPartResult, "SpermParams.CalcPartResult",
0xb8ee9667, 0x7a44, 0x4542, 0xa7, 0x11, 0xf4, 0xf7, 0xb4, 0xff, 0x2c, 0x40);

// Action info

// {F6828EE0-BB5C-489a-B826-7EF5C5184598}
static const GUID guidCalcPartResult =
{ 0xf6828ee0, 0xbb5c, 0x489a, { 0xb8, 0x26, 0x7e, 0xf5, 0xc5, 0x18, 0x45, 0x98 } };



//[ag]6. action info
ACTION_INFO_FULL(CActionCalcPartResult, IDS_ACTION_CALCPARTRESULT, -1, -1, guidCalcPartResult);

//[ag]7. targets


ACTION_TARGET(CActionCalcPartResult,	szTargetAnydoc);

//[ag]8. arguments

ACTION_ARG_LIST(CActionCalcPartResult)
	ARG_STRING(_T("ObjectList"),"")
	ARG_INT(_T("BaseParamKey"), KEY_PARTICLES_NFRAMES)
	ARG_INT(_T("ExcludeKeys"), 1)
END_ACTION_ARG_LIST()

//[ag]9. implementation

//measurement
//////////////////////////////////////////////////////////////////////
//CActionCalcPartResult implementation
CActionCalcPartResult::CActionCalcPartResult()
{
}

CActionCalcPartResult::~CActionCalcPartResult()
{
//	m_sptrMan = 0;
	m_manGroup.FreeComponents();
}

IUnknown *CActionCalcPartResult::GetContextObject(_bstr_t cName, _bstr_t cType)
{ //¬озвращает с AddRef

	IDataContext3Ptr ptrC = m_punkTarget;
	_bstr_t	bstrType = cType;

	if( cName==_bstr_t("") )
	{
		IUnknown *punkImage = 0;
		ptrC->GetActiveObject( bstrType, &punkImage );
		//if( punkImage )
			//punkImage->Release();

		return punkImage;
	}

	LONG_PTR lPos = 0;
	ptrC->GetFirstObjectPos( bstrType, &lPos );
	
	while( lPos )
	{
		IUnknown *punkImage = 0;
		ptrC->GetNextObject( bstrType, &lPos, &punkImage );

		if( punkImage )
		{
			if( _bstr_t(GetObjectName(punkImage)) == cName )
			{
				//if( punkImage )
					//punkImage->Release();
				return punkImage;
			}

			punkImage->Release();
		}
	}
	return 0;
}


bool CActionCalcPartResult::Invoke()
{
	if (!m_punkTarget)
		return false;

	// get meas_param_group_manager
	IUnknown *punk = FindComponentByName(GetAppUnknown(false), IID_IMeasureManager, szMeasurement);
	ICompManagerPtr	ptrManager( punk );
	if( punk )punk->Release();

	if( ptrManager == 0 )return false;

	// check manager contains groups
	int	nGroup, nGroupCount;
	ptrManager->GetCount( &nGroupCount );

	_bstr_t bstrList = GetArgumentString( "ObjectList" );

	// get active object list
	//m_listObjects.Attach(GetActiveList(szTypeObjectList), false);
	m_listObjects.Attach(GetContextObject(bstrList, szTypeObjectList), false);
	if(m_listObjects==0) return false;
	// check list contains objects
	if (!m_listObjects.GetCount())
		return false;

	long lBaseParamKey = GetArgumentInt("BaseParamKey");
	long lExcludeKeys = GetArgumentInt("ExcludeKeys");

	CPtrArray	objects;
	CPtrArray	images;

	POSITION pos = m_listObjects.GetFirstObjectPosition();		//get the first object position
	while( pos )
	{
		IUnknown *punk = m_listObjects.GetNextObject( pos );//get the object at the specified position and move the POS
		if( !punk )continue;
		IMeasureObjectPtr	ptrO( punk );
		punk->Release();
		if( ptrO == 0 )continue;
		ptrO->GetImage( &punk );
		objects.Add( ptrO.Detach() );
		images.Add( punk );
	}

	//remove everything from objects

	IDataObjectListPtr	ptrList( m_listObjects );
	BOOL	bLock = false;
	ptrList->GetObjectUpdateLock( &bLock );
	ptrList->LockObjectUpdate( true );

	int	nCount = objects.GetSize();

	for( int i = 0; i < nCount; i++ )
	{
		ICalcObjectPtr	ptrCalc( (IUnknown*)objects[i] );
		// ptrCalc->ClearValues(); //AAM - убрано, чтобы не обнул€ло
	}


	if(0)
	{
		//remove from container
		LONG_PTR	lposParam = 0;
		ICalcObjectContainerPtr	ptrContainer( m_listObjects );
		ptrContainer->GetFirstParameterPos( &lposParam );
		ptrContainer->SetCurentPosition( 0 );

		while( lposParam )
		{
			ParameterContainer *pContainer = 0;
			long	lPosRemove = lposParam;
			ptrContainer->GetNextParameter( &lposParam, &pContainer );

			if( pContainer->pDescr->pos == 0 )
				continue;
			ptrContainer->RemoveParameterAt( pContainer->lpos );
		}
	}

	for( nGroup = 0; nGroup < nGroupCount; nGroup++ )
	{
		IUnknownPtr	ptrG = 0;
		ptrManager->GetComponentUnknownByIdx( nGroup, &ptrG );
		IMeasParamGroupPtr	ptrGroup( ptrG );
		if( ptrGroup == 0 )continue;

		LONG_PTR lParamPos;
		ptrGroup->GetPosByKey ( lBaseParamKey, &lParamPos );
		if( lExcludeKeys && lParamPos ) continue;
		if( (!lExcludeKeys) && (!lParamPos) ) continue;

		ptrGroup->InitializeCalculation( m_listObjects );

		for( i = 0; i < nCount; i++ )
		{
			IUnknown	*punkO = (IUnknown*)objects[i];
			IUnknown	*punkI = (IUnknown*)images[i];
			ptrGroup->CalcValues( punkO, punkI );
		}
		
		ptrGroup->FinalizeCalculation();
	}

	for( i = 0; i < nCount; i++ )
	{
		IUnknown	*punkO = (IUnknown*)objects[i];
		IUnknown	*punkI = (IUnknown*)images[i];
		if( punkO )punkO->Release();
		if( punkI )punkI->Release();
	}

	ptrList->LockObjectUpdate( bLock );
	ptrList->UpdateObject( 0 );
	return true;
}

//extended UI
bool CActionCalcPartResult::IsAvaible()
{
	if (!m_punkTarget)
		return false;

	CObjectListWrp list;
	list.Attach(GetActiveList(szTypeObjectList), false);

	return list.GetCount() != 0;
}

IUnknown* CActionCalcPartResult::GetActiveList(LPCTSTR szObjectType)
{
	if (CheckInterface(m_punkTarget, IID_IDocument))
		return ::GetActiveObjectFromDocument(m_punkTarget, szObjectType);
	else
		return ::GetActiveObjectFromContext(m_punkTarget, szObjectType);
}

