#include "stdafx.h"
#include "resource.h"
#include "selectquery.h"

#define CTRL_MARGINS_PIXEL			5
#define CTRL_WIDTH_IMAGE_PIXEL		200
#define CTRL_HEIGHT_IMAGE_PIXEL		200

#define CTRL_WIDTH_TEXT_PIXEL		200
#define CTRL_HEIGHT_TEXT_PIXEL		30

#define FORM_WIDTH_MAX				640

////////////////////////////////////////////////////////////////////////////////////////////
bool CQueryObject::AnalizeQueryProperty()
{
	if( !IsOpen() ) 
		return false;

	bool	bNeedFindTumbnailImage		= true;
	bool	bNeedFindTumbnailCaption	= true;


	for( int i=0;i<m_query.m_arrFields.GetSize();i++ )
	{
		CQueryField* pf = m_query.m_arrFields[i];

		if( bNeedFindTumbnailImage )
		{
			if( pf->m_strTable == m_strTumbnailTable && 
				pf->m_strField == m_strTumbnailField && 
				pf->GetFieldType() == ftVTObject
				)
			{
				bNeedFindTumbnailImage = false;
			}
		}

		if( bNeedFindTumbnailCaption )
		{
			if( pf->m_strTable == m_strTumbnailCaptionTable && 
				pf->m_strField == m_strTumbnailCaptionField && 
				pf->GetFieldType() == ftString
				)
			{
				bNeedFindTumbnailCaption = false;
			}
		}

	}

	ISelectQueryPtr ptrThis( GetControllingUnknown() );
	if( ptrThis == 0 )
		return false;

	if( bNeedFindTumbnailImage || bNeedFindTumbnailCaption )
	{
		for( int i=0;i<m_query.m_arrFields.GetSize();i++ )
		{
			CQueryField* pf = m_query.m_arrFields[i];

			if( bNeedFindTumbnailImage )
			{
				if( pf->GetFieldType() == ftVTObject )
				{
					bNeedFindTumbnailImage = false;
					ptrThis->SetTumbnailLocation( 
								_bstr_t( (LPCSTR)pf->m_strTable ), _bstr_t( (LPCSTR)pf->m_strField ) );					
				}
			}

			if( bNeedFindTumbnailCaption )
			{
				if( pf->GetFieldType() == ftString )
				{
					bNeedFindTumbnailCaption = false;

					ptrThis->SetTumbnailCaptionLocation( 
								_bstr_t( (LPCSTR)pf->m_strTable ), _bstr_t( (LPCSTR)pf->m_strField ) );					
				}
			}
		}
	}

	return true;

}

////////////////////////////////////////////////////////////////////////////////////////////
void CQueryObject::AnalizeBlank()
{
	RemoveNotExistControls();
	AddNewControls();
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CQueryObject::RemoveNotExistControls()
{
	CArray<CCtrlFormData*, CCtrlFormData*> arrCtrls;

	if( !GetFormCtrls( arrCtrls ) )
		return false;

	CArray< IUnknown*, IUnknown* > m_arDelete;

	for( int i=0;i<arrCtrls.GetSize();i++ )
	{
		CCtrlFormData* pData = arrCtrls[i];

		if( !pData->bDBaseCtrl )
			continue;

		bool bNeedRemove = false;
		
		bool bFound = false;
		for( int iField=0;iField<m_query.m_arrFields.GetSize();iField++ )
		{
			CQueryField* pField = m_query.m_arrFields[iField];
			if( pField->m_strTable == pData->strTable && pField->m_strField == pData->strField )
			{
				bFound = true;
				break;
			}
		}

		bNeedRemove = !bFound;			

		if( !bNeedRemove )
		{				
			bNeedRemove = !IsSupportedCtrlType( pData->ft );
		}

		if( !bNeedRemove )
		{
			bNeedRemove = 
							( pData->ft == ftDigit && pData->bstrProgID != _bstr_t(DBNumberProgID) ) ||
							( pData->ft == ftString && pData->bstrProgID != _bstr_t(DBTextProgID) ) ||
							( pData->ft == ftDateTime && pData->bstrProgID != _bstr_t(DBDateTimeProgID) ) ||
							( pData->ft == ftVTObject && pData->bstrProgID != _bstr_t(DBVTObjectProgID) );
		}

		if( bNeedRemove )
		{			
			m_arDelete.Add( pData->ptrCtrlData );
		}
	}

	for( i=0;i<m_arDelete.GetSize();i++ )
	{
		IUnknown* punk = m_arDelete[i];
		INamedDataObject2Ptr ptrND2( punk );
		if( ptrND2 == 0 )
			continue;

		ptrND2->SetParent( 0, 0 );		
	}

	m_arDelete.RemoveAll();

	for(i=0;i<arrCtrls.GetSize();i++ )
		delete arrCtrls[i];

	arrCtrls.RemoveAll();
	

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CQueryObject::AddNewControls()
{
	CArray<CCtrlFormData*, CCtrlFormData*> arrCtrls;

	if( !GetFormCtrls( arrCtrls ) )
		return false;

	CArray<CQueryField*, CQueryField*> arAdd;



	//add controls
	for( int iField=0;iField<m_query.m_arrFields.GetSize();iField++ )
	{
		CQueryField* pField = m_query.m_arrFields[iField];		
		
		bool bFound = false;

		for( int i=0;i<arrCtrls.GetSize();i++ )
		{
			CCtrlFormData* pData = arrCtrls[i];

			if( !pData->bDBaseCtrl )
				continue;

			if( pData->strTable == pField->m_strTable && pData->strField == pField->m_strField )
				bFound = true;

		}

		if( !bFound )
		{
			CQueryField* pAddField = new CQueryField;
			pAddField->InitFrom( pField );
			arAdd.Add( pAddField );
		}
	}


	//add VTObject
	for( int i=0;i<arAdd.GetSize();i++ )
	{
		CQueryField* pf = arAdd[i];
		if( pf->GetFieldType() != ftVTObject )
			continue;

		AddCtrl( pf );		
	}
	//add text
	for( i=0;i<arAdd.GetSize();i++ )
	{
		CQueryField* pf = arAdd[i];
		if( pf->GetFieldType() != ftString )
			continue;

		AddCtrl( pf );		
	}

	//add numbers
	for( i=0;i<arAdd.GetSize();i++ )
	{
		CQueryField* pf = arAdd[i];
		if( pf->GetFieldType() != ftDigit )
			continue;

		AddCtrl( pf );		
	}

	//add data
	for( i=0;i<arAdd.GetSize();i++ )
	{
		CQueryField* pf = arAdd[i];
		if( pf->GetFieldType() != ftDateTime )
			continue;

		AddCtrl( pf );		
	}

	
	for( i=0;i<arAdd.GetSize();i++ )	
		delete arAdd[i];	

	arAdd.RemoveAll();


	for( i=0;i<arrCtrls.GetSize();i++ )
		delete arrCtrls[i];

	arrCtrls.RemoveAll();


	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CQueryObject::AddCtrl( CQueryField* pqf )
{
	if( !pqf )
		return false;

	FieldType ft = pqf->GetFieldType();

	_bstr_t bstrProgID( "" );
	if( ft == ftDigit )
		bstrProgID = DBNumberProgID;
	else if( ft == ftString )
		bstrProgID = DBTextProgID;
	else if( ft == ftDateTime )
		bstrProgID = DBDateTimeProgID;
	else if( ft == ftVTObject )
		bstrProgID = DBVTObjectProgID;

	if( bstrProgID == _bstr_t( "" ) )
		return false;


	CLSID	clsid;		
	if( S_OK != ::CLSIDFromProgID( bstrProgID, &clsid ) )
		return false;
	
	IUnknown* punkAX = 0;
	if( S_OK != CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punkAX ) )
		return false;

	IDBControlPtr prtDBCtrl( punkAX );
	punkAX->Release();	punkAX = 0;

	if( prtDBCtrl == 0 )
		return false;


	prtDBCtrl->SetTableName( _bstr_t(pqf->m_strTable) );
	prtDBCtrl->SetFieldName( _bstr_t(pqf->m_strField) );



	IUnknown* punkCtrlData = NULL;
	punkCtrlData = ::CreateTypedObject( szTypeBlankCtrl );
	if( punkCtrlData == NULL )
		return false;


	IActiveXCtrlPtr ptrAXData( punkCtrlData );
	punkCtrlData->Release();	punkCtrlData = 0;

	if( ptrAXData == 0 )
		return false;

	CRect rc = GetPreferedCtrlArea( pqf );
	ptrAXData->SetProgID( bstrProgID );
	ptrAXData->SetRect( rc );


	sptrINamedDataObject2 sptrNDO2( ptrAXData );
	if( sptrNDO2 == NULL )
		return false;
	
	::StoreContainerToDataObject( ptrAXData, prtDBCtrl );

	sptrNDO2->SetParent( GetControllingUnknown(), 0 );
	

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
CRect CQueryObject::GetPreferedCtrlArea( CQueryField* pqf )
{

	CRect rc( 0, 0, 20, 20 );

	FieldType ft = pqf->GetFieldType();

	if( !IsSupportedCtrlType( ft ) )
		return rc;

	CArray<CCtrlFormData*, CCtrlFormData*> arrCtrls;

	if( !GetFormCtrls( arrCtrls ) )
		return rc;
	

	int nWidth, nHeight;

	nWidth	= CTRL_WIDTH_TEXT_PIXEL;
	nHeight	= CTRL_HEIGHT_TEXT_PIXEL;

	if( ft == ftVTObject )
	{
		nWidth	= CTRL_WIDTH_IMAGE_PIXEL;
		nHeight	= CTRL_HEIGHT_IMAGE_PIXEL;
	}

	
	int nBottomPos = CTRL_MARGINS_PIXEL;
	for( int i=0;i<arrCtrls.GetSize();i++ )
	{
		CCtrlFormData* pData = arrCtrls[i];
		if( pData->rcPos.bottom > nBottomPos )
			nBottomPos = pData->rcPos.bottom;
	}

	//////////////////////

	CRect rcNewPos( 0, 0, 0, 0);	
	
	if( !FindCtrlRect( rcNewPos, ft, nWidth, nHeight, true, &arrCtrls )	)	//try right 
	{
		if( !FindCtrlRect( rcNewPos, ft, nWidth, nHeight, false, &arrCtrls ) )	//try bottom 
		{
			//set bottom of the form
			rcNewPos.left	= CTRL_MARGINS_PIXEL;
			rcNewPos.top	= nBottomPos;

			rcNewPos.right	= rcNewPos.left	+ nWidth;
			rcNewPos.bottom	= rcNewPos.top	+ nHeight;
		}
	}
	
	rc = rcNewPos;

	for( i=0;i<arrCtrls.GetSize();i++ )
		delete arrCtrls[i];

	arrCtrls.RemoveAll();

	return rc;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CQueryObject::GetFormCtrls( CArray<CCtrlFormData*, CCtrlFormData*>& arrCtrls)
{
	
	if( !IsOpen() ) 
		return false;

	IDBaseStructPtr ptrDBS( m_sptrParent );
	if( ptrDBS == 0 )
		return false;

	CObjectListWrp list( GetControllingUnknown() );

	if( list == 0 )
		return false;

	POSITION pos = list.GetFirstObjectPosition();
	while( pos )
	{
		IUnknown* punk = list.GetNextObject( pos );
		if( !punk )
			continue;

		IActiveXCtrlPtr ptrAXData( punk );
		punk->Release();
		if( ptrAXData == 0 )
			continue;

		BSTR bstrProgID = 0;
		ptrAXData->GetProgID( &bstrProgID );
				
		_bstr_t _bstrCtrlProgID( bstrProgID );

		if( bstrProgID )
			::SysFreeString( bstrProgID );	bstrProgID = 0;

		CLSID	clsid;		
		if( S_OK != ::CLSIDFromProgID( _bstrCtrlProgID, &clsid ) )
			continue;
		
		IUnknown* punkAX = 0;
		if( S_OK != CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punkAX ) )
			continue;


		IUnknownPtr ptrCtrlAX = punkAX;
		punkAX->Release();	punkAX = 0;

		CCtrlFormData* pData = new CCtrlFormData;
		arrCtrls.Add( pData );

		pData->ptrCtrlData		= ptrAXData;
		pData->bstrProgID		= _bstrCtrlProgID;
		ptrAXData->GetRect( &pData->rcPos );
		pData->ptrCtrlAX		= ptrCtrlAX;

		::RestoreContainerFromDataObject( ptrAXData, ptrCtrlAX );

		IDBControlPtr prtDBCtrl( ptrCtrlAX );
		if ( prtDBCtrl == 0 )
			continue;

		pData->bDBaseCtrl = true;

		BSTR bstrTable = 0;
		BSTR bstrField = 0;
		prtDBCtrl->GetTableName( &bstrTable );
		prtDBCtrl->GetFieldName( &bstrField );

		pData->strTable = bstrTable;
		pData->strField = bstrField;

		short nFieldType = (short)ftNotSupported;
		
		HRESULT hrFind = ptrDBS->GetFieldInfo( bstrTable, bstrField, 0, &nFieldType, 0, 0, 0, 0, 0, 0 );
		
		FieldType ft = (FieldType)nFieldType;

		if( bstrTable )
			::SysFreeString( bstrTable );	bstrTable = 0;

		if( bstrField )
			::SysFreeString( bstrField );	bstrField = 0;		

		pData->ft = ft;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CQueryObject::CanSetCtrl( CRect rcNewCtrl, CArray<CCtrlFormData*, CCtrlFormData*>* parrCtrls )
{
	if( parrCtrls == 0 )	
		return false;

	if( rcNewCtrl.left > FORM_WIDTH_MAX )
		return false;

	for( int i=0;i<parrCtrls->GetSize();i++ )
	{
		CCtrlFormData* pData = (*parrCtrls)[i];

		CRect rc;
		if( ::IntersectRect( &rc, &pData->rcPos, &rcNewCtrl ) )
			return false;
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////
bool CQueryObject::FindCtrlRect(	CRect& rcNewPos, FieldType ft,
									int nWidth, int nHeight, bool bTryRight/*or bottom*/, 
									CArray<CCtrlFormData*, CCtrlFormData*>* parrCtrls )
{
	if( parrCtrls == 0 )
		return false;

	for( int i=0;i<parrCtrls->GetSize();i++ )
	{
		CCtrlFormData* pData = (*parrCtrls)[i];
		if( !pData->bDBaseCtrl )
			continue;

		if( pData->ft != ft )
			continue;

		CRect rcTest;
		if( bTryRight )
		{
			rcTest.left		= pData->rcPos.right + CTRL_MARGINS_PIXEL;
			rcTest.top		= pData->rcPos.top;
		}
		else//try botom
		{
			rcTest.left		= pData->rcPos.left;
			rcTest.top		= pData->rcPos.bottom + CTRL_MARGINS_PIXEL;
		}

		rcTest.right	= rcTest.left	+ nWidth;
		rcTest.bottom	= rcTest.top	+ nHeight;

		if( CanSetCtrl( rcTest, parrCtrls ) )
		{
			rcNewPos = rcTest;
			return true;
		}
	}

	return false;

}

////////////////////////////////////////////////////////////////////////////////////////////
bool CQueryObject::IsSupportedCtrlType( FieldType ft )
{
	if ( ft == ftDigit || ft == ftString || ft == ftDateTime || ft == ftVTObject )
		return true;

	return false;
}

