#include "StdAfx.h"
#include "MeasManual.h"
#include "ActionBase.h"
#include "OleEventProvider.h"
#include "Resource.h"

#include "NameConsts.h"
#include "Measure5.h"
#include "Utils.h"
#include "ObListWrp.h"
#include "statusutils.h"
#include "\vt5\common2\misc_calibr.h"
#include "\vt5\common2\class_utils.h"

/////////////////////////////////////////////////////////////////////////////////////////
//CMMActionObjectBase
/*


Краткое описание:
Перед началом, т.е. перед тем, как производить измерения необходимо:
1. Получить активный списов объектов.
1.1. если его нет - создать и внести в Undo/Redo
2. Получить активный объект из списка.
2.1. если нет активного, создать его, сделать активным и внести в Undo/Redo
3. Получить активный параметр из списка
3.1. если его нет, создать его и внести в Undo/Redo соотв. запись о передыдущем состоянии списка объектов
4. Найти соответствующий параметру измерительный объект
4.1. сохранить его, если он существует, выкинуть его из документа и поместить запись об этом в Undo/Redo
4.2. создать его, прописать в документ и поместить запись об этом в Undo/Redo

  далее идут измерения
5. создать новый измерительный объект
6. произвести операцию
7. если операция прошла удачно, новый измерительный объект прописать в документ и поместить запись об этом в Undo/Redo
 
   завершение операции
8. сдвинуть в соответствие с флагом активный параметр в списке объектов.


Сдвиг активного параметра:
прочитать флаг сдвига NextParam 1 or 0
{ т.е. сдвигать на след параметр или на следующий объект }
если параметр один, то сдвигать на след объект
если параметр последний, то сдвигать на следующий объект и на первый параметр.

*/
//bool CMMActionObjectBase::s_bNeedNewObject = false;
//TCHAR * CMMActionObjectBase::s_szNotData = _T("It's not enough data");


CMMActionObjectBase::CMMActionObjectBase()
{
	m_hcurActive = 0;

	m_fCurrentValue = 0.;

	m_bNewParam  = false;
	m_clDefColor = RGB(64, 128, 255);
	
	m_nCrossOffset = CROSS_OFFSET;
	m_nArcSize = ARC_SIZE;
	m_fZoom = 1;
}

CMMActionObjectBase::~CMMActionObjectBase()
{

}


bool CMMActionObjectBase::IsCompatibleTarget()
{
	if( CheckInterface( m_punkTarget, IID_IMeasureView ) )
		return true;
	DWORD dwFlags = 0;
	IImageViewPtr sptrIV(m_punkTarget);
	if(sptrIV != 0)sptrIV->GetImageShowFlags(&dwFlags);
	return ((dwFlags & isfFilledImageExceptObjects) == isfFilledImageExceptObjects) ||
		   ((dwFlags & isfFilledOnlyObjects) == isfFilledOnlyObjects) ||
           ((dwFlags & isfContours) == isfContours) ||
           ((dwFlags & isfPseudoImage) == isfPseudoImage);
}



//return the active object list
IUnknownPtr CMMActionObjectBase::_GetActiveObjectList()
{
	/*
	if (CheckInterface(m_punkTarget, IID_IDocument))
		return ::GetActiveObjectFromDocument(m_punkTarget, szTypeObjectList);
	else
	*/
	IUnknown* punk = 0;
	punk = ::GetActiveObjectFromContext( _GetActiveView(), szTypeObjectList);
	if( !punk )
		return 0;

	IUnknownPtr ptr = punk;
	punk->Release();
	
	return ptr;
}

//return the active image
IUnknownPtr CMMActionObjectBase::_GetActiveImage()
{

	sptrINamedDataObject2 sptrN;

	IUnknown * punkImage = 0;

	/*
	if (CheckInterface(m_punkTarget, IID_IDocument))
		punkImage = ::GetActiveObjectFromDocument(m_punkTarget, szTypeImage);
	else
	*/
	punkImage = ::GetActiveObjectFromContext( _GetActiveView(), szTypeImage);

	IUnknownPtr ptrImage;

	if (!punkImage)
		return 0;

	sptrN = punkImage;
	IUnknown * punkParent = 0;
	sptrN->GetParent(&punkParent);

	if (punkParent)
	{
		punkImage->Release();
		ptrImage = punkParent;
		punkParent->Release();

		return ptrImage;
	}

	ptrImage = punkImage;
	punkImage->Release();

	return ptrImage;
}

IUnknownPtr	CMMActionObjectBase::_GetActiveDocument()
{

	if( CheckInterface( m_punkTarget, IID_IView ) )
	{
		IViewPtr ptrV( m_punkTarget );
		if( ptrV == 0 )
			return 0;

		IUnknown* punkDoc = 0;

		ptrV->GetDocument( &punkDoc );

		if( !punkDoc )
			return 0;

		IUnknownPtr ptrDoc = punkDoc;
		punkDoc->Release();
		return ptrDoc;
	}
	else
	if( CheckInterface( m_punkTarget, IID_IDocument ) )
	{		
		if( m_punkTarget )
		{
			IUnknownPtr ptrDoc( m_punkTarget );			
			return ptrDoc;
		}
	}		 

	return 0;
}


IUnknownPtr CMMActionObjectBase::_GetActiveView()
{

	if( CheckInterface( m_punkTarget, IID_IView ) )
	{
		IUnknownPtr ptrView = m_punkTarget;
		return ptrView;
	}
	else
	if( CheckInterface( m_punkTarget, IID_IDocument ) )
	{
		IDocumentSitePtr ptrDS( m_punkTarget );
		if( ptrDS == 0 )
			return 0;

		IUnknown* punkV = 0;
		ptrDS->GetActiveView( &punkV );

		if( !punkV )
			return 0;


		IViewPtr ptrV( punkV );
		punkV->Release();	punkV = 0;

		return ptrV;
	}		 

	return 0;
}


IUnknownPtr CMMActionObjectBase::_GetActiveMMObject()
{
	if( m_ObjListWrp == 0 )
		return 0;

	POSITION pos = m_ObjListWrp.GetCurPosition();
	if( pos == 0 )
		return 0;


	IUnknown* punk = 0;
	punk = m_ObjListWrp.GetNextObject( pos );
	if( !punk )
		return 0;

	IUnknownPtr ptrMM = punk;
	punk->Release();

	return ptrMM;
}



bool CMMActionObjectBase::CreateObjectList()
{
	//find || create object list
	m_ObjListWrp.Attach( _GetActiveObjectList() );
	
	m_undoInfo.m_bWasCreatedObjectList = false;

	if( m_ObjListWrp == 0 )
	{		
		IUnknown	*punkListObj = ::CreateTypedObject( szTypeObjectList );
		if( !punkListObj )
			return false;

		m_ObjListWrp.Attach( punkListObj );		
		punkListObj->Release();				

		IUnknownPtr ptrImage( _GetActiveImage() );
		INamedDataObject2Ptr ptrNDO2 = ptrImage;
		
		BOOL bBaseFlag = false;
		if( ptrNDO2 != 0 )
			ptrNDO2->IsBaseObject( &bBaseFlag );
		if( bBaseFlag ) 
			SetBaseObject( m_ObjListWrp, ptrNDO2, _GetActiveDocument( ) );


		m_undoInfo.m_bWasCreatedObjectList = true;
	}

	if( m_ObjListWrp == 0 )
		return false;


	return true;
}

bool CMMActionObjectBase::CreateMeasObject()
{
	if( m_ObjListWrp == 0 )
		return false;
	
	POSITION posActive = m_ObjListWrp.GetCurPosition();

	m_undoInfo.m_bWasCreatedMeasObject = false;

	{
		if( posActive )
		{
			POSITION p = posActive;
			IUnknown* punk = m_ObjListWrp.GetNextObject( p );
			if( punk )
			{
				m_undoInfo.m_guidPrevActiveObject = ::GetObjectKey( punk );
				punk->Release();	punk = 0;
			}
		}
	}

	//need create meas object
	if( !posActive )
	{
		IUnknown * punkObj = ::CreateTypedObject( szTypeObject );
		if( punkObj == 0 )
			return false;		

		INamedDataObject2Ptr ptrNDO2 = punkObj;
		punkObj->Release();	punkObj = 0;

		ICalcObjectPtr ptrCalc = ptrNDO2;
		

		long ClassKey;
		//::CoCreateGuid( &ClassKey );
		{
			ClassKey = get_object_class( ptrCalc );
		}

		if( ptrCalc )
		{
			set_object_class( ptrCalc, ClassKey );
		}			

		DWORD dwFlags = 0;
		ptrNDO2->GetObjectFlags(&dwFlags);
		ptrNDO2->SetParent( m_ObjListWrp, dwFlags);
		
		POSITION posObj = m_ObjListWrp.Find( ptrNDO2 );
		if( posObj == 0 )
			return false;
		
		m_ObjListWrp.SetCurPosition( posObj );
		posActive = posObj;		

		m_undoInfo.m_bWasCreatedMeasObject = true;
	}


	//add manual meas object to meas object
	IUnknown *punkObject = m_ObjListWrp.GetNextObject( posActive );
	m_ptrMeasObject = punkObject;
	punkObject->Release();	punkObject = 0;

	return true;
}

bool CMMActionObjectBase::ProcessMeasManualObject()
{

	if( m_ObjListWrp == 0 || m_ptrMeasObject == 0 || m_ptrManualMeasObject == 0 )
		return false;

	//Decide if need create new parameter

	ICalcObjectContainerPtr ptrCont( m_ObjListWrp );
	if( ptrCont == 0 )
		return false;


	POSITION lParPosCur = 0;
	ptrCont->GetCurentPosition( (LONG_PTR*)&lParPosCur, 0 );

	if( lParPosCur > 0 )
	{
		POSITION l = lParPosCur;
		ParameterContainer* pp = 0;
		ptrCont->GetNextParameter((LONG_PTR*)&l, &pp);
		if( pp && pp->pDescr )
			m_undoInfo.m_lPrevActiveParam = pp->pDescr->lKey;
	}

	long lParamKey = -1;
	LONG_PTR lParPos = 0;
	
	ptrCont->GetFirstParameterPos( &lParPos );	

	//need create new object
	if ((LONG_PTR)lParPosCur < 1)
		lParPos = 0;
	else		
		lParPos = (LONG_PTR)lParPosCur;

	while( lParPos )
	{
		ParameterContainer* pp = 0;
		ptrCont->GetNextParameter( &lParPos, &pp );
		if( !pp )
			continue;

		if( pp->type == (ParamType)GetParamType() && pp->pDescr )
		{
			lParamKey = pp->pDescr->lKey;
			break;
		}
	}

	//need remove object
	if( lParamKey != -1 )
	{
		INamedDataObject2Ptr ptrNDO2( m_ptrMeasObject );
		TPOS lChildPos = 0;

		IManualMeasureObjectPtr ptrMM2Delete;

		ptrNDO2->GetFirstChildPosition( &lChildPos );
		while( lChildPos )
		{
			IUnknown* punkDelete = 0;
			ptrNDO2->GetNextChild( &lChildPos, &punkDelete );

			ptrMM2Delete = punkDelete;
			punkDelete->Release();	punkDelete = 0;

			if( ptrMM2Delete == 0 )
				continue;

			long	lKey = 0;
			DWORD	dwType = 0;
			ptrMM2Delete->GetParamInfo( &lKey, &dwType );

			if( lParamKey == lKey )
				break;			
			
			ptrMM2Delete = 0;
		}
		
		if( ptrMM2Delete )
		{
			m_undoInfo.m_ptrPrevManualMeasObject = ptrMM2Delete;
			//m_changes.RemoveFromDocData( _GetActiveDocument(), ptrMM2Delete );
		}		
	}


	
	IUnknown* punkGroup = 0;
	punkGroup = GetMeasManualGroup();

	IMeasParamGroupPtr prtGroup( punkGroup );

	if( punkGroup )
		punkGroup->Release();


	LONG_PTR lParamPos = (LONG_PTR)lParPosCur;

	if( lParamKey == -1 )
	{		
		long lUniqKey = MANUAL_PARAMS_FIRST_KEY;
		LONG_PTR lPos=0;
		
		for(ptrCont->GetFirstParameterPos(&lPos); lPos; )
		{
			ParameterContainer* pp = 0;
			ptrCont->GetNextParameter( &lPos, &pp );
			_ASSERTE(pp);
			if(MANUAL_PARAMS_FIRST_KEY<=pp->pDescr->lKey
				&& pp->pDescr->lKey<=MANUAL_PARAMS_LAST_KEY)
			{
				lUniqKey = max( pp->pDescr->lKey+1, lUniqKey );			
			}
		}
		if(MANUAL_PARAMS_LAST_KEY < lUniqKey)
			return false;
		ptrCont->DefineParameter( lUniqKey, (ParamType)GetParamType(), prtGroup, &lParamPos );	
		
		m_undoInfo.m_bWasCreateNewParam = true;
		if( lParamPos )
		{
			ParameterContainer* pp = 0;
			LONG_PTR lPos = lParamPos;
			ptrCont->GetNextParameter( &lPos, &pp );
			if( pp && pp->pDescr )
				lParamKey = pp->pDescr->lKey;
		}
	}

	if( lParamPos < 1 )
		return false;

	DWORD dwParamType	= GetParamType();
	DWORD dwObjectType	= GetMMObjectType();

	m_undoInfo.m_lParamKey = lParamKey;

	m_ptrManualMeasObject->SetParamInfo( &lParamKey, &dwParamType );
	
	ptrCont->SetCurentPosition( lParamPos );


	//bool bReturn = DoCalcParam();	


	//set parent 
	{
		INamedDataObject2Ptr ptrChild( m_ptrManualMeasObject );
		if( ptrChild == 0 )
			return false;
		
		DWORD dwFlags = 0;
		ptrChild->GetObjectFlags( &dwFlags );

		ptrChild->SetParent( m_ptrMeasObject, dwFlags);
	}


	//Set action info

	ParameterContainer* pp = 0;
	ptrCont->GetNextParameter( &lParamPos, &pp );
	if( pp )
	{
		if( pp->bstrAction )
		{
			m_undoInfo.m_strPrevActionName = pp->bstrAction;

			::SysFreeString( pp->bstrAction );			
		}		

		pp->bstrAction = GetName().AllocSysString();
	}
	else
	{
		ASSERT( false );
	}	


	return true;
}

void CMMActionObjectBase::DoActivateObjects( IUnknown *punkObject )
{
//	if( GetObjectKey( punkObject ) == GetObjectKey( m_ObjListWrp ) )
//		return;
	Terminate();
}

bool CMMActionObjectBase::DoTerminate()
{
	m_ObjListWrp = 0;
	m_ptrMeasObject = 0;
	m_ptrManualMeasObject = 0;
	m_undoInfo.m_ptrPrevManualMeasObject = 0;

	return CInteractiveActionBase::DoTerminate();
}

bool CMMActionObjectBase::CalcObject()
{	
	
	if( m_ptrMeasObject == 0 || m_ptrManualMeasObject == 0 )
		return false;


	long lParamKey = 0;
	m_ptrManualMeasObject->GetParamInfo( &lParamKey, 0 );
	
	if( SUCCEEDED( m_ptrManualMeasObject->CalcValue( &m_fCurrentValue ) ) )
	{
		// and set this value to parent
		ICalcObjectPtr sptrCalc = m_ptrMeasObject;
		if (sptrCalc)
			sptrCalc->SetValue( lParamKey, m_fCurrentValue );
	}


	CString strParamName;
	ICalcObjectContainerPtr ptrCont( m_ObjListWrp );
	if( ptrCont )
	{
		ParameterContainer* pp = 0;
		ptrCont->ParamDefByKey( m_undoInfo.m_lParamKey, &pp );
		if( pp && pp->pDescr && pp->pDescr->pos > 0 )
		{
			strParamName = pp->pDescr->bstrName;

			//store current action name
			if( pp->bstrAction )
			{
				m_undoInfo.m_strPrevActionName = pp->bstrAction;
				::SysFreeString( pp->bstrAction );
			}
			m_pActionInfo->GetCommandInfo( &pp->bstrAction, 0, 0, 0 );
		}
	}

	// create event after measure
	IUnknown* punk = ::_GetOtherComponent( ::GetAppUnknown(false), IID_IMeasureEventProvider );
	if( punk )
	{
		IMeasureEventProviderPtr ptrEv( punk );

		_bstr_t bstrType = ::GetParamNameByParamType( GetParamType() );
		if( ptrEv )
			ptrEv->FireEventAfterMeasure( bstrType, m_fCurrentValue );

		punk->Release();
	}


	return true;	
}


bool CMMActionObjectBase::ShiftParamater()
{
	long	ShiftDir =::GetValueInt(::GetAppUnknown(false), szManualMeasure, szShiftMode, (long)0 );
	bool	bNeedShiftVertical = ShiftDir == 1;

	if( m_ObjListWrp == 0 )
		return false;


	ICalcObjectContainerPtr ptrCont( m_ObjListWrp );
	if( ptrCont == 0 )
		return false;


	IUnknownPtr ptrActiveMeasObj = m_ptrMeasObject;
	//m_undoInfo.m_guidNewActiveObject = GetObjectKey( m_ptrMeasObject );

	//horizontal
	if( ShiftDir == 0 )
	{
		//shift horizontal
		POSITION lParPosCur = 0;

		ptrCont->GetCurentPosition((LONG_PTR*)&lParPosCur, 0);

		long lKeyToRemove = -1;
		LONG_PTR lParPos = 0;
		
		ptrCont->GetFirstParameterPos( &lParPos );	

		//need create new object
		if( lParPosCur > 0 )
			lParPos = (LONG_PTR)lParPosCur;


		bool bFirstMatch = false;
		long lSavePos = 0;
		bool bFindType = false;

		/*
		if( bNeedShiftVertical )
			bFirstMatch = true;
			*/

		while( lParPos )
		{
			lSavePos = lParPos;

			ParameterContainer* pp = 0;
			ptrCont->GetNextParameter( &lParPos, &pp );
			if( !pp || !pp->pDescr )
				continue;


			if( pp->type != (ParamType)GetParamType() )
				continue;

			if( bFirstMatch )
			{
				ptrCont->SetCurentPosition( lSavePos );
				//m_undoInfo.m_lNewActiveParam = pp->pDescr->lKey;
				bFindType = true;
				break;
			}

			if( !bFirstMatch )
			{
				bFirstMatch = true;
			}		
		}

		if( !bFindType  )
		{
			//m_undoInfo.m_lNewActiveParam = 0;
			ptrCont->SetCurentPosition( 0 );
		}
	}
	else  if( ShiftDir == 1 )//vertiacal
	{			
		//Composite support
		ICompositeObjectPtr co(m_ObjListWrp);
		if(co)
		{
			long bC;
			co->IsComposite(&bC);
			if(bC)//shift throw main objects only
			{	
				IUnknown* unk;
				co->GetMainLevelObjects(&unk);
				if(unk)
				{
					INamedDataObject2Ptr ndo(unk);
					if(ndo)
					{
						TPOS pos;
						ndo->GetActiveChild( &pos );
						ndo->GetNextChild(&pos, &unk);
						if(unk) unk->Release();
						/*if(pos ==0) ndo->GetFirstChildPosition( &pos );*/
						ndo->SetActiveChild( pos );
					}
				}
				return true;
			}
		}
		//end composite specific

		POSITION pos = m_ObjListWrp.GetCurPosition();

		if( pos )
		{
			IUnknown* punk = m_ObjListWrp.GetNextObject( pos );
		//m_undoInfo.m_guidNewActiveObject = GetObjectKey( punk );
			if( punk )	punk->Release();
		}

		if( pos )//activate next
		{
			m_ObjListWrp.SetCurPosition( pos );
		}
		else//activate 0
		{
			m_ObjListWrp.SetCurPosition( 0 );
			return true;
		}

		return true;
	}
	else //diagonal
	{
		//shift horizontal
		LONG_PTR lParPos = 0;
		LONG_PTR lSavePos = 0;
		bool	bNewObject = 0;
		
		ptrCont->GetCurentPosition( &lParPos, 0 );
		ASSERT(lParPos);
		ptrCont->GetNextParameter( &lParPos, 0 );
		if( lParPos == 0 )
		{
			ptrCont->GetFirstParameterPos( &lParPos );
			bNewObject = true;
		}
		
		LONG_PTR	ltemp = lParPos;

		ParameterContainer* pp = 0;
		ptrCont->GetNextParameter( &ltemp, &pp );

		//set the current position
		ptrCont->SetCurentPosition( lParPos );
		m_undoInfo.m_lNewActiveParam = pp->pDescr->lKey;

		ASSERT( pp );

		//execute new action
		_bstr_t	bstrNewAction;
		if( pp->type != etUndefined )
		{
			if( pp->bstrAction )
				bstrNewAction = pp->bstrAction;
			else
			{
				if( pp->type == etLinear )
					bstrNewAction = "MeasManualLine";
				else if( pp->type == etCounter )
					bstrNewAction = "MeasManualCount";
				else if( pp->type == etAngle )
					bstrNewAction = "MeasManualAngle3Point";

			}
		}

		//shift vertical
		if( bNewObject )
		{
			POSITION pos = m_ObjListWrp.GetCurPosition();
			if( pos ) 
			{
				IUnknown* punk = m_ObjListWrp.GetNextObject( pos );
				m_undoInfo.m_guidNewActiveObject = GetObjectKey( punk );
				if( punk )	punk->Release();				
			}
			m_ObjListWrp.SetCurPosition( pos );
		}

		

		//execute new action
		if( bstrNewAction.length() == 0 )
		{
			LeaveMode();
		}
		else
		{
			IUnknown	*punkAM;
			IApplication	*papp = GetAppUnknown();
			papp->GetActionManager( &punkAM );
			IActionManagerPtr	ptrAM(punkAM);
			punkAM->Release();

			IUnknown	*punkI = 0;
			ptrAM->GetActionInfo( bstrNewAction, &punkI );
			if( punkI )
			{
				ptrAM->ChangeInteractiveAction( punkI );
				punkI->Release();
			}
		}


/*		while( lParPos )
		{
			lSavePos = lParPos;

			ParameterContainer* pp = 0;
			ptrCont->GetNextParameter( &lParPos, &pp );
			if( !pp || !pp->pDescr )continue;

			if( pp->type == (ParamType)GetParamType() )
			{
				//return if compatible parameter found in current line
				ptrCont->SetCurentPosition( lSavePos );
				//m_undoInfo.m_lNewActiveParam = pp->pDescr->lKey;
				return true;
			}
		}

		//find compatible parameter on next line
		bool	bFindType = false;
		ptrCont->GetFirstParameterPos( &lParPos );	

		while( lParPos )
		{
			lSavePos = lParPos;
			ParameterContainer* pp = 0;
			ptrCont->GetNextParameter( &lParPos, &pp );
			if( !pp || !pp->pDescr )continue;

			if( pp->type == (ParamType)GetParamType() )
			{
				bFindType = true;
				ptrCont->SetCurentPosition( lSavePos );
				//m_undoInfo.m_lNewActiveParam = pp->pDescr->lKey;
				break;
			}
		}
		if( !bFindType )
		{
			ptrCont->SetCurentPosition( 0 );
			//m_undoInfo.m_lNewActiveParam = 0;
		}*/
		

		

	}


	return true;
}



bool CMMActionObjectBase::PutToData()
{
	if( m_ObjListWrp == 0 || m_ptrMeasObject == 0 || m_ptrManualMeasObject == 0 )
		return false;

	IUnknownPtr ptrDoc = _GetActiveDocument( );
	if( ptrDoc == 0 )
		return false;

	if( m_undoInfo.m_ptrPrevManualMeasObject )
		m_changes.RemoveFromDocData( ptrDoc, m_undoInfo.m_ptrPrevManualMeasObject );


	if( m_undoInfo.m_bWasCreatedObjectList )
		m_changes.SetToDocData( ptrDoc, m_ObjListWrp );


	//activate object list
	IDataContextPtr	ptrC = _GetActiveView();
	if( ptrC )
	{
		IUnknown	*punk = 0;
		ptrC->GetActiveObject( _bstr_t( szTypeObjectList ), &punk );
		GuidKey	keyCurrent = GetObjectKey( punk );
		if( punk )punk->Release();

		if( keyCurrent != GetObjectKey( m_ObjListWrp ) )
			ptrC->SetActiveObject( 0, m_ObjListWrp, aofActivateDepended );
	}



	//set base object
	if( m_undoInfo.m_bWasCreatedMeasObject )
		m_changes.SetToDocData( ptrDoc, m_ptrMeasObject );


	m_changes.SetToDocData( ptrDoc, m_ptrManualMeasObject );


/*	if( m_undoInfo.m_ptrPrevManualMeasObject )
		m_changes.RemoveFromDocData( ptrDoc, m_undoInfo.m_ptrPrevManualMeasObject );
*/
	ICalcObjectPtr sptrCalc = m_ptrMeasObject;
	if (sptrCalc)sptrCalc->SetValue( m_undoInfo.m_lParamKey, m_fCurrentValue );


	::UpdateDataObject( ptrDoc, m_ObjListWrp );
	

	INamedDataObjectPtr ptr_ndo( m_ptrMeasObject );
	if( ptr_ndo )
		ptr_ndo->SetModifiedFlag( true );


	return true;
}

bool CMMActionObjectBase::UpdatePropPage( bool bonly_calibr_name )
{
	CString strParamName, strParamNameFmt = "%g";
	ICalcObjectContainerPtr ptrCont( m_ObjListWrp );
	if( ptrCont != 0 )
	{
		ParameterContainer* pp = 0;
		ptrCont->ParamDefByKey( m_undoInfo.m_lParamKey, &pp );
		if( pp && pp->pDescr )
		{
			strParamName = pp->pDescr->bstrName;
			strParamNameFmt = pp->pDescr->bstrDefFormat;
		}
	}


	GUID	guid;
	char	szCalibrName[100];
	::GetCalibration( _GetActiveImage(), 0, &guid );
	::GetCalibrationFromTable( guid, 0, szCalibrName, 0 );


	if( !bonly_calibr_name )
	{
		CString	s;
		s.Format( strParamNameFmt, m_fCurrentValue*m_fUnitCoeff );
		::SetValue(GetAppUnknown(false), szManualMeasure, szCurrentValue, s );
		::SetValue(GetAppUnknown(false), szManualMeasure, szCurrentParam, strParamName );
		::SetValue(GetAppUnknown(false), szManualMeasure, szCurrentUnitName, m_strUnitName );
	}

	::SetValue(GetAppUnknown(false), szManualMeasure, szCurrentCalibrName, szCalibrName );

	{

		long lDummy;
		CString  strPageName;
		strPageName = c_szCMeasPage;
		IUnknownPtr sptrUnkPage(::GetPropertyPageByTitle(strPageName, &lDummy), false);
		IOptionsPagePtr sptrPage = sptrUnkPage;
		if (sptrPage)
			sptrPage->LoadSettings();
	}

	long lDummy;
	CString  strPageName = c_szCMeasPage;
	IUnknownPtr sptrUnkPage(::GetPropertyPageByTitle(strPageName, &lDummy), false);
	IOptionsPagePtr sptrPage = sptrUnkPage;
	if (sptrPage)
		sptrPage->LoadSettings();

	return true;
}



bool CMMActionObjectBase::InvalidateView(  )
{
	IViewPtr ptrView = _GetActiveView();
	if( ptrView == 0 )
		return false;
	
	CRect rect = ::ConvertToWindow( ptrView, GetInvalidateRect());
	CWnd * pwnd = GetWindowFromUnknown( ptrView );
	if( !pwnd )
		return false;
	
	pwnd->InvalidateRect( rect );
	
	return true;
}

bool CMMActionObjectBase::Invoke()
{
	//[AY] - part of code move to Initialize

	

	if( !CreateObjectList() )
		return false;

	if( !CreateMeasObject() )
		return false;

	if( !ProcessMeasManualObject() )
		return false;

	if( !CalcObject() )
		return false;

	if( !PutToData() )
		return false;

	if( !ShiftParamater() )
		return false;


	ICalcObjectContainerPtr ptrCont( m_ObjListWrp );
	if( ptrCont )
	{
		POSITION lParPos = 0;
		ptrCont->GetCurentPosition((LONG_PTR*)&lParPos, 0);

		if( lParPos )
		{
			ParameterContainer* pp = 0;
			ptrCont->GetNextParameter((LONG_PTR*)&lParPos, &pp);
			if( pp && pp->pDescr )
			{
				m_undoInfo.m_lNewActiveParam = pp->pDescr->lKey;
			}
		}
	}


	POSITION posActive = m_ObjListWrp.GetCurPosition();	

	if( posActive )
	{
		POSITION p = posActive;
		IUnknown* punk = m_ObjListWrp.GetNextObject( p );
		if( punk )
		{
			m_undoInfo.m_guidNewActiveObject = ::GetObjectKey( punk );
			punk->Release();	punk = 0;
		}
	}
	

	//update property page
	UpdatePropPage();


	//sync table
	SyncViewData sd;
	::ZeroMemory( &sd, sizeof(SyncViewData) );

	sd.dwStructSize				= sizeof(SyncViewData);
	sd.punkObject2Activate		= (IUnknown*)m_ObjListWrp;
	sd.bCanChangeViewType		= false;
	sd.bSyncAllView				= false;
	sd.bOnlyViewInterfaceMatch	= true;
	memcpy( &(sd.iidViewMatch), &IID_IMeasureView, sizeof( IID ) );

	::SyncSplitterView( m_punkTarget, &sd );

	InvalidateView( );

	if( m_ptrManualMeasObject )
	{
		m_ptrManualMeasObject->UpdateParent();
	}


	m_lTargetKey = ::GetObjectKey( _GetActiveDocument() );

	
	return true;
}


bool CMMActionObjectBase::DoRedo()
{
	INamedDataObjectPtr ptr_ndo( m_ptrMeasObject );
	if( ptr_ndo )
		ptr_ndo->SetModifiedFlag( true );



	IDocumentPtr ptrDoc = m_punkTarget;//_GetActiveDocument( );
	ICalcObjectContainerPtr ptrCont( m_ObjListWrp );

	m_changes.DoRedo( ptrDoc );

	if( m_undoInfo.m_bWasCreateNewParam )
	{

		IUnknown* punkGroup = 0;
		punkGroup = GetMeasManualGroup();

		IMeasParamGroupPtr prtGroup( punkGroup );

		if( punkGroup )
			punkGroup->Release();
		
		ptrCont->DefineParameter( m_undoInfo.m_lParamKey, (ParamType)GetParamType(), prtGroup, 0 );	
	}

	ParameterContainer* pp = 0;
	ptrCont->ParamDefByKey( m_undoInfo.m_lParamKey, &pp );
	if( pp  )
	{
		if( pp->bstrAction )
		{
			::SysFreeString( pp->bstrAction );
			pp->bstrAction = 0;
			pp->bstrAction = GetName().AllocSysString();
		}
	}



	if( m_undoInfo.m_lNewActiveParam > 0 )
	{
		ParameterContainer* pp = 0;
		ptrCont->ParamDefByKey( m_undoInfo.m_lNewActiveParam, &pp );
		if( pp && pp->pDescr && pp->pDescr->pos > 0 )
		{
			ptrCont->SetCurentPosition( pp->pDescr->pos );
		}
	}	
	else	
	{
		ptrCont->SetCurentPosition( 0 );
	}
	
	if( m_undoInfo.m_guidNewActiveObject != INVALID_KEY )
	{
		POSITION pos = m_ObjListWrp.GetFirstObjectPosition();
		while( pos )
		{
			POSITION lPosSave = pos;
			IUnknown* punk = m_ObjListWrp.GetNextObject( pos );
			if( punk )
			{				
				IUnknownPtr ptrObj = punk;
				punk->Release();
				punk = 0;

				if( ::GetObjectKey( ptrObj ) == m_undoInfo.m_guidNewActiveObject )
				{
					m_ObjListWrp.SetCurPosition( lPosSave );
					break;
				}
			}
		}

	}
	else
	{
		m_ObjListWrp.SetCurPosition( 0 );
	}

	InvalidateView( );


	return true;	
}

bool CMMActionObjectBase::DoUndo()
{
	INamedDataObjectPtr ptr_ndo( m_ptrMeasObject );
	if( ptr_ndo )
		ptr_ndo->SetModifiedFlag( true );


	IDocumentPtr ptrDoc = m_punkTarget;//_GetActiveDocument( );
	ICalcObjectContainerPtr ptrCont( m_ObjListWrp );

	if( ptrDoc == 0 || ptrCont == 0 )
		return false;

	m_changes.DoUndo( ptrDoc );


	if( m_undoInfo.m_bWasCreateNewParam )
	{
		ptrCont->RemoveParameter( m_undoInfo.m_lParamKey );
	}
	else
	{
		ParameterContainer* pp = 0;
		ptrCont->ParamDefByKey( m_undoInfo.m_lParamKey, &pp );
		if( pp  )
		{
			if( pp->bstrAction )
			{
				::SysFreeString( pp->bstrAction );
				pp->bstrAction = 0;
				if( !m_undoInfo.m_strPrevActionName.IsEmpty() )
				{
					pp->bstrAction = m_undoInfo.m_strPrevActionName.AllocSysString();
				}
			}
		}
	}


	if( m_undoInfo.m_lPrevActiveParam > 0 )
	{
		ParameterContainer* pp = 0;
		ptrCont->ParamDefByKey( m_undoInfo.m_lPrevActiveParam, &pp );
		if( pp && pp->pDescr && pp->pDescr->pos > 0 )
		{
			ptrCont->SetCurentPosition( pp->pDescr->pos );
		}
	}	
	else	
	{
		ptrCont->SetCurentPosition( 0 );
	}
	
	if( m_undoInfo.m_guidPrevActiveObject != INVALID_KEY )
	{
		POSITION pos = m_ObjListWrp.GetFirstObjectPosition();
		while( pos )
		{
			POSITION lPosSave = pos;
			IUnknown* punk = m_ObjListWrp.GetNextObject( pos );
			if( punk )
			{				
				IUnknownPtr ptrObj = punk;
				punk->Release();
				punk = 0;

				if( ::GetObjectKey( ptrObj ) == m_undoInfo.m_guidPrevActiveObject )
				{
					m_ObjListWrp.SetCurPosition( lPosSave );
					break;
				}
			}
		}

	}
	else
	{
		m_ObjListWrp.SetCurPosition( 0 );
	}


	InvalidateView( );


	return true;
}


bool CMMActionObjectBase::CreateMMObject()
{							 
	IUnknown	*punk = CreateTypedObject(szTypeManualMeasObject);
	ASSERT(punk);
	if (!punk)
		return false;

	m_ptrManualMeasObject = punk;
	punk->Release();

	if (m_ptrManualMeasObject == 0)
		return false;

	long lParamKey		= -1;
	DWORD dwParamType	= GetParamType();
	DWORD dwObjectType	= GetMMObjectType();

	//try to get the parameter information
	m_ptrManualMeasObject->SetParamInfo( &lParamKey, &dwParamType );
	m_ptrManualMeasObject->SetObjectType( dwObjectType );

	//initalize unit and coeff
	IUnknown* punkGroup = GetMeasManualGroup();
	IMeasParamGroupPtr prtGroup( punkGroup );
	if( punkGroup )punkGroup->Release();

	BSTR	bstr;
	prtGroup->GetUnit( dwParamType, &bstr, &m_fUnitCoeff );
	m_strUnitName = bstr;
	::SysFreeString( bstr );

	double	fc;
	GUID	guid;
	::GetCalibration( _GetActiveImage(), &fc, &guid );

	ICalibrPtr	ptrC( m_ptrManualMeasObject );
	ptrC->SetCalibration( fc, &guid );

	return true;
}


bool CMMActionObjectBase::CalcValue()
{
	m_fCurrentValue = 0;
	if( m_ptrManualMeasObject == 0 )
		return false;

	if( !DoCalcParam( ) )
		return false;

	if( FAILED(m_ptrManualMeasObject->CalcValue(&m_fCurrentValue)) )
		return false;


	CString	strParameter;
	strParameter.Format( "%g %s", m_fCurrentValue*m_fUnitCoeff, (const char*)m_strUnitName );
	::StatusSetPaneText( guidPaneAction, strParameter );


	return true;
}

CRect CMMActionObjectBase::GetInvalidateRect()
{
	CRect rc = NORECT;
	IDrawObjectPtr sptrD = m_ptrManualMeasObject;

	IUnknownPtr ptrView = _GetActiveView();

	if (ptrView != 0 && sptrD != 0 )
	{
		sptrD->SetTargetWindow( ptrView );
		sptrD->GetRect( &rc );
		sptrD->SetTargetWindow( 0 );
	}
	return rc;
}


DWORD CMMActionObjectBase::GetMMObjectType()
{
	return (DWORD)emotUndefined;
}

DWORD CMMActionObjectBase::GetParamType()
{
	return (DWORD)etUndefined;
}

void CMMActionObjectBase::DeleteMMObject(IUnknown *punkObject)
{
	//m_changes.RemoveFromDocData(m_punkDocument, punkObject);
}

bool CMMActionObjectBase::DoUpdateSettings()
{
	m_clDefColor = ::GetValueColor(::GetAppUnknown(false), szManualMeasure, szMeasureColor, RGB(64, 128, 255));


	m_nCrossOffset = ::GetValueInt(::GetAppUnknown(false), szManualMeasure, szMMCrossOffset, CROSS_OFFSET);
	m_nArcSize = ::GetValueInt(::GetAppUnknown(false), szManualMeasure, szMMArcSize, ARC_SIZE);

	m_fZoom = 1;
	IScrollZoomSitePtr sptrZ = _GetActiveView();
	if (sptrZ)sptrZ->GetZoom(&m_fZoom);

	return true;
}



bool CMMActionObjectBase::DoLButtonDown(CPoint pt)
{
	if (!CheckInterface(m_punkTarget, IID_IImageView))
		return false;

//	ActivateObject(pt);
	return false;
}

bool CMMActionObjectBase::DoRButtonDown(CPoint pt)
{
	Terminate();
	return true;
}



bool CMMActionObjectBase::Initialize()
{
	
	if( CheckInterface( m_punkTarget, IID_IImageView ) )
	{
		if( !IsCompatibleTarget())
			if( !SetAppropriateView(m_punkTarget, avtObject) || m_state == asTerminate )
				return false;

		{
			CImageWrp	image( _GetActiveImage() );

			if( image == 0 )return false;
			CRect	rect = image.GetRect();
			CMouseImpl::LockMovement( true, rect );
		}

	}

	ICalcObjectContainerPtr ptrCont( _GetActiveObjectList() );

	if( ptrCont != 0 )
	{
		LONG_PTR	lpos = 0;
		ptrCont->GetCurentPosition( &lpos, 0 );
		LONG_PTR	lposC = lpos;

		while( lpos != 0 )
		{
			LONG_PTR	lposSave = lpos;
			ParameterContainer* pp = 0;
			ptrCont->GetNextParameter( &lpos, &pp );

			if( pp->type == GetParamType() )
			{
				lpos = lposSave;
				break;
			}
		}

		long ShiftDir =::GetValueInt(::GetAppUnknown(false), szManualMeasure, szShiftMode, (long)0 );
		if( !lpos && ShiftDir != 0 ) 
		{
			ptrCont->GetFirstParameterPos( &lpos );
			while( lpos != 0 )
			{
				long	lposSave = lpos;
				ParameterContainer* pp = 0;
				ptrCont->GetNextParameter( &lpos, &pp );

				if( pp->type == GetParamType() )
				{
					lpos = lposSave;
					break;
				}
			}
		}
			

		if( (lpos||ShiftDir==0) && lpos != lposC )
			ptrCont->SetCurentPosition( lpos );

	}


	// create new manual measure object 
	if( !CreateMMObject() )
		return false;

	// and freeze it
	if (m_ptrManualMeasObject)
		m_ptrManualMeasObject->StartStopMeasure(true);

	// create event befour measure
	IUnknown* punk = ::_GetOtherComponent( ::GetAppUnknown(false), IID_IMeasureEventProvider );
	if( punk )
	{
		IMeasureEventProviderPtr ptrEv( punk );

		_bstr_t bstrType = ::GetParamNameByParamType( GetParamType() );
		if( ptrEv )
			ptrEv->FireEventBeforeMeasure( bstrType );

		punk->Release();
	}
	


	if( !CInteractiveActionBase::Initialize() )
		return false;

	UpdatePropPage( true );

	return true;
}


void CMMActionObjectBase::Finalize()
{
	if( m_state != asTerminate )
		_Draw();
	else
	{
		RECT rc;

		CWnd *pwnd = GetWindowFromUnknown( m_punkTarget );
		if( pwnd )
		{
			pwnd->GetClientRect( &rc );
			pwnd->InvalidateRect( &rc );
		}
	}
	CInteractiveActionBase::Finalize();
}

bool CMMActionObjectBase::IsAvaible()
{
	if (CheckInterface(m_punkTarget, IID_IImageView))
	{
		IUnknown* punk_img = ::GetActiveObjectFromContext( m_punkTarget, szTypeImage );
		if( punk_img )
		{
			punk_img->Release();
			return true;
		}
		return false;
	}
	/*else if (CheckInterface(m_punkTarget, IID_IMeasureView))
		return true;*/
	else
		return false;

	//paul
	/*
	if (!CheckInterface(punk, IID_IMeasParamGroup2))
		return false;
		

	IMeasParamGroup2Ptr sptrParam = punk;
	punk->Release();
	*/



/*	
	INamedDataObject2Ptr sptrList = sptrParam;

	long lCount = 0;
	if (FAILED(sptrList->GetChildsCount(&lCount)) || !lCount)
		return true;

	long lPos = 0;
	if (FAILED(sptrParam->GetActiveParamPos(&lPos)) || !lPos)
		return true;

	DWORD dwType = 0;
	if (FAILED(sptrParam->GetParamType(lPos, &dwType)))
		return true;

//	if (!SupportType(dwType))
//		return false;
*/
	return true;
}

bool CMMActionObjectBase::SupportType(DWORD dwType)
{
	dwType;
	return true;
}

void CMMActionObjectBase::DrawCross(CDC *pDC, const CPoint & pt)
{
	if (!pDC)
		return;

	pDC->MoveTo(pt.x - m_nCrossOffset, pt.y - m_nCrossOffset);		//  "\ "
	pDC->LineTo(pt.x + m_nCrossOffset, pt.y + m_nCrossOffset);		//  " \"

	pDC->MoveTo(pt.x + m_nCrossOffset, pt.y - m_nCrossOffset);		//  " /"
	pDC->LineTo(pt.x - m_nCrossOffset, pt.y + m_nCrossOffset);		//  "/ "
}


