#include "stdafx.h"
#include "chrom_aam_utils.h"
#include "misc_utils.h"
#include <math.h>
#include <limits.h>
#include "measure5.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "ImagePane.h"
#include "alloc.h"
#include "aam_utils.h"

#include "docview5.h"
#include "Chromosome.h"

IUnknown * FindComponentByName(IUnknown *punkThis, const IID iid, LPCTSTR szName)
{
	_bstr_t strName(szName);
	_bstr_t strCompName;

	if (CheckInterface(punkThis, iid))
	{
		strCompName = ::GetName(punkThis);
		if (strCompName == strName)
		{
			punkThis->AddRef();
			return punkThis;
		}
	}

	IUnknown *punk = 0;
	if (CheckInterface(punkThis, IID_ICompManager))
	{
		ICompManagerPtr	sptr(punkThis);
		int nCount = 0;
		sptr->GetCount(&nCount);
		for (int i = 0; i < nCount; i++)
		{
			sptr->GetComponentUnknownByIdx(i, &punk);

			if (punk)
			{
				if (CheckInterface(punk, iid))
				{
					strCompName = ::GetName(punk);
					if (strCompName == strName)
						return punk;
				}
				punk->Release();
				punk = 0;
			}
		}
	}

	if (CheckInterface(punkThis, IID_IRootedObject))
	{
		IRootedObjectPtr	sptrR(punkThis);
		IUnknown	*punkParent = 0;

		sptrR->GetParent(&punkParent);

		punk = FindComponentByName(punkParent, iid, strName);

		if (punkParent)
			punkParent->Release();
	}

	return punk;
}

void CalcObject(IUnknown* punkCalc, IUnknown* punkImage, bool bSetParams, ICompManager *pGroupManager )
{
	if( !pGroupManager )
	{
		IUnknown *punk = FindComponentByName(GetAppUnknown(), IID_IMeasureManager, szMeasurement);
		if( !punk )return;
		ICompManagerPtr	ptrManager( punk );
		punk->Release();
		pGroupManager = ptrManager;
	}

	IDataObjectListPtr ptrObjectList;

	if( bSetParams )
	{
		INamedDataObject2Ptr ptrObject(punkCalc);
		IUnknown* punkParent = 0;
		ptrObject->GetParent(&punkParent);
		ptrObjectList = punkParent;
		if( punkParent )punkParent->Release();
		if( ptrObjectList == 0 )return;
	}

	IUnknownPtr sptr2;
	if(punkImage==0)
	{
		IMeasureObjectPtr ptrObject(punkCalc);
		if(ptrObject!=0)
		{
			ptrObject->GetImage(&sptr2);
			punkImage=sptr2;
		}
	}

	int	nCount;
	pGroupManager->GetCount( &nCount );
	for( int idx = 0; idx < nCount; idx++ )
	{
		IUnknown	*punk = 0;
		pGroupManager->GetComponentUnknownByIdx( idx, &punk );
		IMeasParamGroupPtr	ptrG( punk );
		if( punk )punk->Release();
		if( ptrG == 0 )continue;

		if( bSetParams )
			ptrG->InitializeCalculation( ptrObjectList );
		ptrG->CalcValues( punkCalc, punkImage );
		if( bSetParams )
			ptrG->FinalizeCalculation();
	}

	if( bSetParams )
		ptrObjectList->UpdateObject( punkCalc );
}

void ConditionalCalcObject(IUnknown* punkCalc, IUnknown* punkImage, bool bSetParams, ICompManager *pGroupManager )
{
	bool bRecalc = ::GetValueInt(GetAppUnknown(), "\\measurement", "DynamicRecalc", 1) == 1;
	if(bRecalc) CalcObject(punkCalc, punkImage, bSetParams, pGroupManager );
}


void FireEvent( IUnknown *punkCtrl, const char *pszEvent, IUnknown *punkFrom, IUnknown *punkData, void *pdata, long cbSize )
{
	INotifyControllerPtr	sptr( punkCtrl );

	if( sptr== 0 )
		return;

	_bstr_t	bstrEvent( pszEvent );
	sptr->FireEvent( bstrEvent, punkData, punkFrom, pdata, cbSize );
}

void DoExecute(LPCTSTR pActionName, LPCTSTR pParams)
{
        IUnknown *punkAM = _GetOtherComponent( GetAppUnknown(), IID_IActionManager );
        sptrIActionManager      pActionMan(punkAM);
        punkAM->Release();
        _bstr_t bstrActionName(pActionName);
        _bstr_t bstrParams(pParams);
        pActionMan->ExecuteAction(bstrActionName, bstrParams, 0);
};
