// AverClassMeasGroup.cpp : implementation file
//

#include "stdafx.h"
#include "Classes.h"
#include "AverClassMeasGroup.h"
#include "Params.h"
#include "Comdef.h"
#include "NameConsts.h"
#include "ObListWrp.h"
#include "Classes5.h"

#include "Utils.h"
#include <Math.h>

#include "\vt5\common2\class_utils.h"

/////////////////////////////////////////////////////////////////////////////
// CAverClassMeasGroup

IMPLEMENT_DYNCREATE(CAverClassMeasGroup, CMeasParamGroupBase)

// {2E28FEC6-CA53-48e5-80C8-94244A5D8EA7}
static const GUID clsidAverClassMeasGroup = 
{ 0x2e28fec6, 0xca53, 0x48e5, { 0x80, 0xc8, 0x94, 0x24, 0x4a, 0x5d, 0x8e, 0xa7 } };
// {2EADBF14-4DFC-40c4-BE05-CC681EC89962}
GUARD_IMPLEMENT_OLECREATE_PROGID(CAverClassMeasGroup, "Classes.AverClassMeasGroup", 
0x2eadbf14, 0x4dfc, 0x40c4, 0xbe, 0x5, 0xcc, 0x68, 0x1e, 0xc8, 0x99, 0x62);

CAverClassMeasGroup::CAverClassMeasGroup()
{
}

CAverClassMeasGroup::~CAverClassMeasGroup()
{
}


/////////////////////////////////////////////////////////////////////////////
// CAverClassMeasGroup message handlers
bool CAverClassMeasGroup::GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fCoeff )
{
	fCoeff = 1;
	if( lKey == KEY_NUM )bstrType = "Number";
	else if( lKey == KEY_NUM )bstrType = "Units*Units";
	else if( lKey == KEY_MEAN )bstrType = "Units";
	else return false;

	return true;
}
bool CAverClassMeasGroup::LoadCreateParam()
{
	DefineParameter( KEY_NUM, "Count", "%0.0f" );
	DefineParameter( KEY_DEV, "Deviation", "%0.3f" );
	DefineParameter( KEY_MEAN, "Average", "%0.3f" );

	return true;
}

bool CAverClassMeasGroup::CalcValues( IUnknown *punkCalcObject, IUnknown *punkOther )
{
	if (!::CheckInterface(punkCalcObject, IID_ICalcObject))
		return false;

	// punk must be CMeasureObject
	if (!::CheckInterface(punkCalcObject, IID_IClassObject))
		return false;

	ICalcObjectPtr		sptrCalc = punkCalcObject;

	// get class key
	long ClassKey = get_object_class( sptrCalc );

	IUnknown *punkParent = 0;
	INamedDataObject2Ptr	ptrN( sptrCalc );
	ptrN->GetParent( &punkParent );
	if( !punkParent )return false;
	ptrN = punkParent;
	punkParent->Release();
	IUnknown	*punkData = 0;
	ptrN->GetData( &punkData );
	if( !punkData )return false;
	IDataTypeManagerPtr sptrTypeMan = punkData;
	punkData->Release();
	if (sptrTypeMan == 0)
		return false;

	// find index of object_list_type
	long lCount = 0;
	if (FAILED(sptrTypeMan->GetTypesCount(&lCount)))
		return false;

	CString strObjListType = szTypeObjectList;
	long lType = -1;
	for (long i = 0; i < lCount; i++)
	{
		BSTR bstrType = 0;
		if (FAILED(sptrTypeMan->GetType(i, &bstrType)))
			return false;

		CString strType = bstrType;
		::SysFreeString(bstrType);
		if (strType == strObjListType)
		{
			lType = i;
			break;
		}
	}
	if (lType == -1)
		return false;

	//  for all object_lists in document
	double fAver = 0;
	int nNumber = 0;
	double fDev = 0;


	long lpos = 0;
	if (FAILED(sptrTypeMan->GetObjectFirstPosition(lType, &lpos)))
		return false;

	while (lpos)
	{
		IUnknown *punkList = 0;
		if (FAILED(sptrTypeMan->GetNextObject(lType, &lpos, &punkList)))
			return false;

		if (!punkList)
			return false;
	
		CObjectListWrp	listObjects(punkList, false);
		if (!listObjects.GetCount())
			continue;

		// object_list lock update
		CListLockUpdate lock(listObjects);

		// for all objects in list 
		POSITION pos = listObjects.GetFirstObjectPosition();
		while (pos)
		{
			IUnknown *punk = listObjects.GetNextObject(pos);
			ICalcObjectPtr sptrObj = punk;

			if (sptrObj == 0)
				continue;
			punk->Release();

			long ObjKey = get_object_class( sptrObj );
			// if  object's class key  == lClassKey
			if (ObjKey == ClassKey)
			{
				// get params of object 
				double fExtPer = 0, 
					   fInnerPer = 0, 
					   fExtArea = 0,
					   fInnerArea = 0;
				bool bAdd = true;

						
				if (FAILED(sptrObj->GetValue(KEY_AREA, &fExtArea)))
					bAdd = false;

				if (FAILED(sptrObj->GetValue(KEY_INT_AREA, &fInnerArea)))
					bAdd = false;

				if (FAILED(sptrObj->GetValue(KEY_PERIMETER, &fExtPer)))
					bAdd = false;

				if (FAILED(sptrObj->GetValue(KEY_INT_PERIMETER, &fInnerPer)))
					bAdd = false;
				
				if (bAdd) // we are get all parameters
				{
					nNumber++;
					fAver += fExtArea;
					fDev += (fExtArea * fExtArea);
				}
			}// it's our object
		}// for all objects
	} // for all object lists

	// and calculate corresponding params
	double fNumber = (double)nNumber;
	if (nNumber)
		fAver /= fNumber;

	fDev -= fNumber * (fAver * fAver);
	if (nNumber - 1)
		fDev /= (fNumber - 1.0);

	fDev = sqrt(fDev);

	// set values to class object
	sptrCalc->SetValue(KEY_NUM, fNumber);
	sptrCalc->SetValue(KEY_MEAN, fAver);
	sptrCalc->SetValue(KEY_DEV, fDev);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//IPersist
void CAverClassMeasGroup::GetClassID( CLSID* pClassID )
{
	memcpy( pClassID, &guid, sizeof(CLSID) );
}


