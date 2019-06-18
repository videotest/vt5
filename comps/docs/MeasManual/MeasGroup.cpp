// MeasGroup.cpp : implementation file
//

#include "stdafx.h"
#include "measmanual.h"
#include "MeasGroup.h"
#include "units_const.h"

/////////////////////////////////////////////////////////////////////////////
// CMeasGroup

IMPLEMENT_DYNCREATE(CManualMeasGroup, CCmdTarget)

CManualMeasGroup::CManualMeasGroup()
{
	AfxOleLockApp();
	m_sName = "Manual";
}

CManualMeasGroup::~CManualMeasGroup()
{
	AfxOleUnlockApp( );
}

// {E3985D92-5C77-4644-8EA5-9707360F133D}
IMPLEMENT_GROUP_OLECREATE(CManualMeasGroup, "MeasManual.MeasManualGroup", 
0xe3985d92, 0x5c77, 0x4644, 0x8e, 0xa5, 0x97, 0x7, 0x36, 0xf, 0x13, 0x3d)

/////////////////////////////////////////////////////////////////////////////
// CMeasGroup message handlers

bool CManualMeasGroup::CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource )
{
	
	ICalcObjectPtr ptrCalc = punkCalcObject;
	if( ptrCalc == 0 )
		return false;

	INamedDataObject2Ptr ptrParent( ptrCalc );
	if( ptrParent == 0 )
		return false;

	long lPos = 0;

	ptrParent->GetFirstChildPosition( &lPos );
	while( lPos )
	{
		IUnknown* punk = 0;
		ptrParent->GetNextChild( &lPos, &punk );
		if( !punk )
			continue;

		IManualMeasureObjectPtr ptrMM( punk );
		punk->Release();	punk = 0;

		if( ptrMM == 0 )
			continue;

		ptrMM->CalcValue(NULL);
/*		long lParamKey = 0;
		ptrMM->GetParamInfo( &lParamKey, 0 );
		double fVal = 0;
		if( SUCCEEDED( ptrMM->CalcValue( &fVal ) ) )
		{
			// and set this value to parent
			ptrCalc->SetValue( lParamKey, fVal );
		}*/
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CManualMeasGroup::GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fMeterPerUnit )
{
	if( lType == etLinear )
	{
		bstrType = GetValueString( GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );
		return true;
	}
	else if( lType == etCounter )
	{
		bstrType = GetValueString( GetAppUnknown(), szCountUnits, szUnitName, szDefCountUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szCountUnits, szUnitCoeff, 1 );
		return true;
	}
	else if( lType == etAngle )
	{
		bstrType = GetValueString( GetAppUnknown(), szAngleUnits, szUnitName, szDefAngleUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szAngleUnits, szUnitCoeff, c_dblDefAngle );
		return true;
	}
	else
	{
		bstrType = GetValueString( GetAppUnknown(), szUnknownUnits, szUnitName, szDefUnknownUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szUnknownUnits, szUnitCoeff, 1 );
		return true;
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CManualMeasGroup::OnInitCalculation()
{
	return CMeasParamGroupBase::OnInitCalculation();
}

/////////////////////////////////////////////////////////////////////////////
bool CManualMeasGroup::OnFinalizeCalculation()
{
	return CMeasParamGroupBase::OnFinalizeCalculation();
}

/////////////////////////////////////////////////////////////////////////////
bool CManualMeasGroup::ReloadState()
{
	IMeasParamGroupPtr	ptrGroup( GetControllingUnknown() );

	IApplicationPtr	ptrApp( ::GetAppUnknown() );
	if( ptrApp == 0 || ptrGroup == 0 )
		return false;
	
	_bstr_t			bstrObjectList( szTypeObjectList );


	long	lPosTemplate = 0;		
	ptrApp->GetFirstDocTemplPosition( &lPosTemplate );
	
	while( lPosTemplate )
	{
		long	lPosDoc = 0;

		ptrApp->GetFirstDocPosition( lPosTemplate, &lPosDoc );

		while( lPosDoc )
		{
			IUnknown *punkDoc = 0;
			ptrApp->GetNextDoc( lPosTemplate, &lPosDoc, &punkDoc );

			if( !punkDoc )		continue;


			IDataContext2Ptr	ptrDC( punkDoc );
			punkDoc->Release();	punkDoc = 0;

			if( ptrDC == 0 )	continue;

			long lObjListPos = 0;
			ptrDC->GetFirstObjectPos( bstrObjectList, &lObjListPos );
			while( lObjListPos )
			{
				IUnknown* punkObjList = 0;
				ptrDC->GetNextObject( bstrObjectList, &lObjListPos, &punkObjList );
				if( !punkObjList )		continue;


				ICalcObjectContainerPtr ptrContainer( punkObjList );
				punkObjList->Release();	punkObjList = 0;

				if( ptrContainer == 0 )	continue;

				
				long lParamPos = 0;
				ptrContainer->GetFirstParameterPos( &lParamPos );
				while( lParamPos )
				{
					ParameterContainer	*pParCont = 0;
					ptrContainer->GetNextParameter( &lParamPos, &pParCont );
					if( pParCont && pParCont->pDescr && !pParCont->pDescr->pos )
					{
						if( pParCont->pDescr->bstrUnit )
						{
							::SysFreeString( pParCont->pDescr->bstrUnit );
							pParCont->pDescr->bstrUnit = 0;
						}
						ptrGroup->GetUnit( pParCont->type, &pParCont->pDescr->bstrUnit, &pParCont->pDescr->fCoeffToUnits );						
					}					
				}
			}
		}

		ptrApp->GetNextDocTempl( &lPosTemplate, 0, 0 );
	} 

	/*
	POSITION	pos = m_listParamDescr.GetHeadPosition();
	while( pos )
	{
		_bstr_t	bstrUnit;

		ParameterDescriptor	*pdescr = (ParameterDescriptor	*)m_listParamDescr.GetNext( pos );
		GetUnit( pdescr->lKey, etUndefined, bstrUnit, pdescr->fCoeffToUnits );
		pdescr->bstrUnit = bstrUnit.copy();
	}
	*/

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//IPersist
void CManualMeasGroup::GetClassID( CLSID* pClassID )
{
	memcpy( pClassID, &guid, sizeof(CLSID) );
}

