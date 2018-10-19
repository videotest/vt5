#include "stdafx.h"
#include "print.h"
#include "afxpriv2.h"

#include "PrintActions.h"
#include "PrinterSetup.h"
#include "PrintView.h"
#include "TemplateSetup.h"
#include "constant.h"


CString FindObjectTypeByViewProgID( CString strViewProgID /*, 
								   CString *arrObjectType, int nTypeCount */);
//////////////////////////////////////////////////////////////////////
//Write active object name & view type
/*
void WriteActiveObject()
{
	IUnknown* punkApp = NULL;

	sptrIApplication sptrA( GetAppUnknown( ) );
	if( sptrA == NULL )
		return;

	IUnknown* punk = NULL;

	ISOK( sptrA->GetActiveDocument( &punk ) );

	if( punk == NULL )
		return;


	sptrIDocumentSite sptrDS( punk );
	punk->Release();

	if( sptrDS == NULL )
		return;


	ISOK( sptrDS->GetActiveView( &punk ) );

	if( punk == NULL )
		return;

	sptrIView sptrV( punk );
	punk->Release();

	if( sptrV == NULL )
		return;

	CString strCurType;					  
	_bstr_t bstrTypeImage( szTypeImage );	
	_bstr_t bstrTypeScript( szArgumentTypeScript );

	DWORD dwSupport;
	
	strCurType = szTypeImage;
	ISOK( sptrV->GetMatchType( bstrTypeImage, &dwSupport ) );
	if( dwSupport != (MatchView)mvFull )
	{	
		strCurType = szArgumentTypeScript;
		ISOK( sptrV->GetMatchType( bstrTypeScript, &dwSupport ) );
		if( dwSupport != (MatchView)mvFull )
		{
			return;
		}
	}
	
	punk = NULL;
	punk = ::GetActiveObjectFromDocument( sptrDS, strCurType );		

	if( punk == NULL )
		return;
	

	CString strName = GetObjectName( punk );
	punk->Release();

	
	CString strProgID;

	IPersistPtr ptrPersist( sptrV );
	if( ptrPersist )
	{
		CLSID ClassID;
		ISOK( ptrPersist->GetClassID(&ClassID) );
		LPWSTR psz;
		if( ::ProgIDFromCLSID( ClassID, &psz ) == S_OK )
		{
			strProgID = psz;
			::CoTaskMemFree(psz);
		}

	}

	::SetValue( GetAppUnknown(), "Print", SD_ACTIVE_OBJECT, strName );
	::SetValue( GetAppUnknown(), "Print", SD_ACTIVE_VIEW_TYPE, strProgID );

	
}
*/

//////////////////////////////////////////////////////////////////////
//Generate report if no exist doc & *.rpt source template

IUnknown* GenerateDefaultReport( IUnknown* punkDoc )
{
	

	CString strActiveView;

	
	IUnknown	*punkActiveObject = 
		::GetActiveObjectFromDocument( punkDoc, szArgumentTypeImage );	
	if( punkActiveObject )
	{		
		strActiveView = GetObjectName( punkActiveObject );		
		punkActiveObject->Release();		
	}
	

	IUnknown* punkReport = NULL;
	punkReport = ::CreateTypedObject( szTypeReportForm );	

	if( punkReport == NULL ) 
		return NULL;
	
	CRect rcFields;
	

	CRect rcPrnMargins = g_prnSetup.GetPrinterMargins( 0 );
	sptrIReportForm	sptrForm( punkReport );
	if( sptrForm == NULL )
		return NULL;
	
	CRect paperAlignNew = CRect( 
				FROM_DOUBLE_TO_VTPIXEL(rcPrnMargins.left),
				FROM_DOUBLE_TO_VTPIXEL(rcPrnMargins.top),
				FROM_DOUBLE_TO_VTPIXEL(rcPrnMargins.right),
				FROM_DOUBLE_TO_VTPIXEL(rcPrnMargins.bottom)
				);
	sptrForm->SetPaperAlign( paperAlignNew );	

	sptrForm->GetPaperField( &rcFields );


	CSize pageSize;
	sptrForm->GetPaperSize( &pageSize );

	CRect rcPos;

	rcPos.left = rcFields.left + FROM_DOUBLE_TO_VTPIXEL(5);
	rcPos.top  = rcFields.top + FROM_DOUBLE_TO_VTPIXEL(5);

	rcPos.right   = pageSize.cx - rcFields.right - FROM_DOUBLE_TO_VTPIXEL(5);
	rcPos.bottom  = pageSize.cy - rcFields.bottom - FROM_DOUBLE_TO_VTPIXEL(5);

	IUnknown* punkCtrl = NULL;
	punkCtrl = ::CreateTypedObject( szTypeReportCtrl );

	if( punkCtrl )
	{


		_bstr_t bstr( szViewAXProgID );
		

		sptrIActiveXCtrl	sptrC( punkCtrl );
		if( sptrC )
			sptrC->SetProgID( bstr );

		sptrIReportCtrl sptrRC( punkCtrl );
		if( sptrRC )
		{
			sptrRC->SetDesignColRow(0,0);
			sptrRC->SetDesignOwnerPage( 0 );
			sptrRC->SetDesignPosition( rcPos );
			
			
		}

		sptrINamedDataObject2 sptrNDO2(punkCtrl);
		if( sptrNDO2 )
			sptrNDO2->SetParent( punkReport, 0 );			


		
		CLSID	clsid;		
		if( ::CLSIDFromProgID( bstr, &clsid ) == S_OK )
		{
			IUnknown* punkVAX = NULL;
			HRESULT hr = CoCreateInstance(clsid,
				NULL, CLSCTX_INPROC_SERVER,
				IID_IUnknown, (LPVOID*)&punkVAX);
			if(hr == S_OK)
			{
				sptrIViewCtrl sptrVAX( punkVAX );
				if( sptrVAX )
				{
					//6.10.200 Cos need build
					//ISOK( sptrVAX->SetObjectDefinition( (int)odActiveView ) );					
				}
				punkVAX->Release();

				::StoreContainerToDataObject( punkCtrl, sptrVAX ); 
			}

		}
		
			   

		punkCtrl->Release();
	}


	sptrINamedData	ptrNamedData( punkDoc );
	if( ptrNamedData == NULL )
		return NULL;


	CString	strObjName = g_szPrintPreviewObject;
	_bstr_t bstrName(strObjName);

	// set this name to object
	INamedObject2Ptr sptrObj(punkReport);
	ISOK(sptrObj->SetName(bstrName));


	//Cos Context change 28.08.2000
	//INumeredObjectPtr	ptrN( punkReport );
	//ptrN->GenerateNewKey();

	::SetValue( ptrNamedData, 0, 0, _variant_t(punkReport ) );

	
	return punkReport;

}



//////////////////////////////////////////////////////////////////////
//get report from doc || *.rpt
IUnknown* GenerateExistReport( IUnknown* punkDoc, BOOL bForceDelete, CString& strTemplate,
							  IUnknown** punkReportDocFind )
{			
	
	

	//sptrIDocumentSite	sptrDocSite( punkDoc );
	sptrINamedData	ptrNamedData( punkDoc );
	if( ptrNamedData == NULL )
		return NULL;


	//Object to add
	IUnknown* punkReport;

	//Delete prev created
	{
		IUnknown* punkOldPreview;
		punkOldPreview = ::GetObjectByName( ptrNamedData, g_szPrintPreviewObject, szTypeReportForm );
		if( punkOldPreview != NULL )
		{
			if( bForceDelete )
			{
				::DeleteObject( ptrNamedData, punkOldPreview );
				punkOldPreview->Release();
			}
			else
			{
				return punkOldPreview;
			}
		}
	}


	
	//Set data to view
	CString strTemplateName	= _GetValueString( ptrNamedData, REPORT_TEMPLATE_SECTION, 
							"TemplateName", "default" );

	strTemplate = strTemplateName;

	TemplateSource1 ts;
	ts = (TemplateSource1)_GetValueInt( ptrNamedData, REPORT_TEMPLATE_SECTION, 
							"nSourceTemplate",  (int)tsFile );	
	
	//Create new object in document
	{
		//Load from archive
		if( ts == tsFile )
		{
			CString strDir = GetReportDir();
			CString strFileName = strDir + strTemplateName + ".rpt";		
			punkReport = ::CreateTypedObject( szTypeReportForm/*szTypeAXForm*/ );

			if( punkReport == NULL )
				return NULL;
			
			
			// get object name from path and check it's exists
			CString	strObjName = g_szPrintPreviewObject;
			_bstr_t bstrName(strObjName);
			
			long	NameExists = 0;
			
			if(!CheckInterface(punkReport, IID_ISerializableObject))
			{
				punkReport->Release();
				return NULL;
			}
													
			CFileFind ff;
			if( ff.FindFile( strFileName ) )
			{



				CFile	file;
				
				try
				{
					file.Open( strFileName, CFile::modeRead );
					
				}
				catch(CFileException* ex)
				{
					ex->ReportError();
					ex->Delete();
					punkReport->Release();
					return NULL;
				}
					
				sptrISerializableObject	sptrO( punkReport );
				if( sptrO == NULL )
				{	punkReport->Release();
					return NULL;
				}
				
				CArchive		ar( &file, CArchive::load );
				CArchiveStream	ars( &ar );				

				SerializeParams	params;
				ZeroMemory( &params, sizeof( params ) );

				
				if( sptrO->Load( &ars, &params ) != S_OK )		
				{
					CString strError;
					strError.Format( "Can't load report template. File name:%s", strFileName );
					AfxMessageBox( strError );
					punkReport->Release();
					return NULL;
				}
			}
			else
			{
				punkReport->Release();
				return  NULL;

			}			


			// set this name to object
			INamedObject2Ptr sptrObj(punkReport);
			ISOK(sptrObj->SetName(bstrName));


			//Cos Context change 28.08.2000
			//INumeredObjectPtr	ptrN( punkReport );
			//ptrN->GenerateNewKey();

			::SetValue( ptrNamedData, 0, 0, _variant_t(punkReport ) );			
			
			
		}//There is in document
		else
		{	
			

			IUnknown* punkBasedOnReport = NULL;

			
			sptrIApplication sptrA( GetAppUnknown( ) );
			if( sptrA != NULL )
			{			

				LONG_PTR	lPosTemplate = 0;
				
				sptrA->GetFirstDocTemplPosition( &lPosTemplate );
				
				while( lPosTemplate )
				{
					LONG_PTR	lPosDoc = 0;

					sptrA->GetFirstDocPosition( lPosTemplate, &lPosDoc );

					while( lPosDoc )
					{
						IUnknown *punkDocT = 0;
						sptrA->GetNextDoc( lPosTemplate, &lPosDoc, &punkDocT );

						ASSERT( punkDocT );						
						
						if( punkBasedOnReport == NULL )
						{
							punkBasedOnReport = ::GetObjectByName( punkDocT, strTemplateName, szTypeReportForm );
							if( punkBasedOnReport != NULL )
								*punkReportDocFind = punkDocT;
							
						}
						  
						punkDocT->Release();
					}

					sptrA->GetNextDocTempl( &lPosTemplate, 0, 0 );
				} 
			
			
			}

			  

			if( punkBasedOnReport == NULL )
			{
				return NULL;
			}

			punkReport = CloneObject( punkBasedOnReport );
			
			punkBasedOnReport->Release();
			
			if( punkReport == NULL )			
				return NULL;
			

			INumeredObjectPtr	ptrN( punkReport );
			if( ptrN == NULL )
			{
				punkReport->Release();
				return NULL;
			}

			//Cos Context change 28.08.2000
			//ptrN->GenerateNewKey();

			INamedObject2Ptr ptrNO( punkReport );
			if( ptrNO == NULL )
			{
				punkReport->Release();
				return NULL;
			}

			_bstr_t bstrName( g_szPrintPreviewObject );
			
			ptrNO->SetName( bstrName );
			ptrNO->SetUserName( bstrName );

			::SetValue( ptrNamedData, 0, 0, _variant_t( punkReport ) );	
			
		}

	}

	return punkReport;
}


//////////////////////////////////////////////////////////////////////
//Create printPreview Object

IUnknown* CreatePrintPreviewData( IUnknown* punkDoc, BOOL bForceDelete, CString& strTemplate,
								 IUnknown** punkReportDocFind)
{
	IUnknown* punkReport;	

	punkReport = GenerateExistReport( punkDoc, bForceDelete, strTemplate, punkReportDocFind );

	if( punkReport == NULL )		
	{
		strTemplate	= "Default";
		punkReport = GenerateDefaultReport( punkDoc );
	}

	if( punkReport ) 
		ProcessReport( &punkReport, punkDoc );

	return punkReport;
	
}


struct ViewAXData{
	ObjectDefinition ObjDef;
	CString strViewProgID;
	CString strObjType;
	CString strObjName;
	CPoint ptStart;
	int nCol;
	int nRow;	
	GuidKey nKey;
};
//////////////////////////////////////////////////////////////////////
// Process report 
BOOL ProcessReport( IUnknown** ppunkReport, IUnknown* punkDoc )
{

	CArray <DocumentObject*, DocumentObject*> docObjList;
	
	CArray <ViewAXData*, ViewAXData*> viewAXList;

	if( *ppunkReport == NULL || punkDoc == NULL )
		return FALSE;

	BuildMode buildMode;

	sptrIReportForm sptrForm( *ppunkReport ); 
	if( sptrForm == NULL )
		return FALSE;

	int nTemp = 0;
	ISOK( sptrForm->GetBuildMode(&nTemp) );
	buildMode = (BuildMode)nTemp;

	sptrIViewCtrl sptrViewCtrl = 0;					

	
	{
		_bstr_t bstrViewAXProgID( szViewAXProgID );

		CLSID	clsid;		
		sptrIViewCtrl sptrVAX = 0;
		if( ::CLSIDFromProgID( bstrViewAXProgID, &clsid ) == S_OK )
		{
			IUnknown* punkVAX = NULL;
			HRESULT hr = CoCreateInstance(clsid,
				NULL, CLSCTX_INPROC_SERVER,
				IID_IUnknown, (LPVOID*)&punkVAX);
			
			if(hr == S_OK )			
				sptrViewCtrl = punkVAX;				
			
			if( punkVAX != NULL )  
				punkVAX->Release();

			if( sptrViewCtrl == NULL )
				return FALSE;

		}
		else
		{
			return FALSE;
		}
	}



	//Get objects from document

	sptrIDocumentSite sptrDS( punkDoc );
	if( sptrDS == NULL )
		return FALSE;

	IUnknown	*punkV = 0;
	sptrDS->GetActiveView( &punkV );
	sptrIView	sptrV( punkV );
	sptrIDataContext2	sptrC( punkV );

	if( punkV )
		punkV->Release();
	else
		return FALSE;

	if( sptrV == NULL || sptrC == NULL )
		return FALSE;
		  

	long nTypesCount = 0;
	ISOK( sptrC->GetObjectTypeCount( &nTypesCount ) );

	
	//CString* arrObjectTypes = new CString[nTypesCount];
	

	for( long nType = 0; nType < nTypesCount; nType++ )
	{
		BSTR	bstrType = 0;		

		long nObjCountOfType = 0;		

		ISOK( sptrC->GetObjectTypeName( nType, &bstrType ) );
		ISOK( sptrC->GetChildrenCount( bstrType, 0, &nObjCountOfType ) );

		if (nObjCountOfType)
		{
			TPOS lPos = 0;
			sptrC->GetFirstChildPos(bstrType, 0, &lPos);
			while (lPos)
			{
				IUnknownPtr sptrObj;
				if (FAILED(sptrC->GetNextChild(bstrType, 0, &lPos, &sptrObj)) || sptrObj == 0)
					continue;

				DocumentObject * pDocbj = new DocumentObject;
				pDocbj->strType = bstrType;
				pDocbj->strObjName = ::GetObjectName(sptrObj);			
				pDocbj->bUse = FALSE;
				docObjList.Add(pDocbj);				

			}									
		}

		::SysFreeString( bstrType );
	}





	///////////////////////////////////////////////////////////
	//Active object	build mode
	if( buildMode == bmActiveObject )
	{
		CObjectListWrp	list( sptrForm );
		POSITION	pos = list.GetFirstObjectPosition();
		while( pos )
		{			
			IUnknown *punk = list.GetNextObject( pos );
			sptrIReportCtrl sptrRepCtrl( punk );
			if( sptrRepCtrl )
			{
				
				sptrIActiveXCtrl sptrAXCtrl( punk );
				if( sptrAXCtrl )
				{
					BSTR bstrProgID;
					ISOK( sptrAXCtrl->GetProgID(&bstrProgID) );
					CString strProgID = bstrProgID;
					::SysFreeString(bstrProgID);

					if( strProgID == szViewAXProgID )
					{					 

						::RestoreContainerFromDataObject( sptrRepCtrl, sptrViewCtrl );						

						if( sptrViewCtrl != NULL )
						{
							CString strActiveObject = 
								::GetValueString( GetAppUnknown(), "Print", SD_ACTIVE_OBJECT,"" );

							CString strViewProgID = 
								::GetValueString( GetAppUnknown(), "Print", SD_ACTIVE_VIEW_TYPE,"" );

							_bstr_t bstrActiveObject( strActiveObject );
							_bstr_t bstrViewProgID( strViewProgID );

							
							//6.10.200 Cos need build
							
							/*
							int nTemp;
							ISOK( sptrViewCtrl->GetObjectDefinition(&nTemp) );
							ObjectDefinition od = (ObjectDefinition)nTemp;							
							

							if( od == odActiveView )
							{
								ISOK(sptrViewCtrl->SetViewProgID( bstrViewProgID ));
								ISOK(sptrViewCtrl->SetObject( bstrActiveObject ));

								::StoreContainerToDataObject( sptrRepCtrl, sptrViewCtrl ); 
							}

							if( od == odActiveObject )
							{								
								ISOK(sptrViewCtrl->SetObject( bstrActiveObject ));

								::StoreContainerToDataObject( sptrRepCtrl, sptrViewCtrl ); 
							}
							*/

						}
					}
				}
			}
			punk->Release();
		}
	}

	///////////////////////////////////////////////////////////
	//Multi selection build mode
	if( buildMode == bmMultiSelection )
	{
		CObjectListWrp	list( sptrForm );
		POSITION	pos = list.GetFirstObjectPosition();
		while( pos )
		{			
			IUnknown *punk = list.GetNextObject( pos );
			sptrIReportCtrl sptrRepCtrl( punk );
			if( sptrRepCtrl )
			{
				
				sptrIActiveXCtrl sptrAXCtrl( punk );
				if( sptrAXCtrl )
				{
					BSTR bstrProgID;
					ISOK( sptrAXCtrl->GetProgID(&bstrProgID) );
					CString strProgID = bstrProgID;
					::SysFreeString(bstrProgID);

					if( strProgID == szViewAXProgID )
					{					 

						::RestoreContainerFromDataObject( sptrRepCtrl, sptrViewCtrl );
						
						if( sptrViewCtrl != NULL )
						{	
							ViewAXData* pViewAX = new ViewAXData;
							INumeredObjectPtr ptrNumObj( sptrRepCtrl );
							if( ptrNumObj )
								ISOK( ptrNumObj->GetKey(&pViewAX->nKey) );
							
							
							
							
							//6.10.200 Cos need build
							/*
							int nTemp;
							ISOK( sptrViewCtrl->GetObjectDefinition(&nTemp) );
							pViewAX->ObjDef = (ObjectDefinition)nTemp;
							
							BSTR bstr;
							ISOK( sptrViewCtrl->GetViewProgID( &bstr ) );
							pViewAX->strViewProgID = bstr;
							::SysFreeString( bstr );

							pViewAX->strObjType = "";

							ISOK( sptrViewCtrl->GetObject( &bstr ) );
							pViewAX->strObjName = bstr;
							::SysFreeString( bstr );
							*/


							CRect rcDesign;
							ISOK( sptrRepCtrl->GetDesignPosition(&rcDesign) );
							pViewAX->ptStart = rcDesign.TopLeft();
							ISOK( sptrRepCtrl->GetDesignColRow( &pViewAX->nCol, &pViewAX->nRow ) );
							
							viewAXList.Add( pViewAX );
						}
						
					}
				}
			}
			punk->Release();
		}
		//Find strObjType by View ProgID

		for( int i=0;i<viewAXList.GetSize();i++ )
		{
			ViewAXData* pViewAX = viewAXList[i]; 
			pViewAX->strObjType = FindObjectTypeByViewProgID( pViewAX->strViewProgID/*,
									arrObjectTypes, nTypesCount*/ );
		}

		//Create viewAXList DONE. Now set object name
		for( i=0;i<viewAXList.GetSize();i++ )
		{
			ViewAXData* pViewAX = viewAXList[i]; 
			if(pViewAX->ObjDef == odActiveObject)
			{
				BOOL bFind = FALSE;
				for( int j=0;j<docObjList.GetSize();j++)
				{	
					if( !bFind )
					{
						DocumentObject* pDocObj = docObjList[j];
						if( !pDocObj->bUse && pDocObj->strType == pViewAX->strObjType )
						{
							pDocObj->bUse = TRUE;
							pViewAX->strObjName = pDocObj->strObjName;
							bFind = TRUE;
						}
					}					
				}
			}
		}


		//now set value to ctrls data


		
		pos = list.GetFirstObjectPosition();
		while( pos )
		{			
			IUnknown *punk = list.GetNextObject( pos );
			sptrIReportCtrl sptrRepCtrl( punk );
			if( sptrRepCtrl )
			{				
				sptrIActiveXCtrl sptrAXCtrl( punk );
				if( sptrAXCtrl )
				{
					BSTR bstrProgID;
					ISOK( sptrAXCtrl->GetProgID(&bstrProgID) );
					CString strProgID = bstrProgID;
					::SysFreeString(bstrProgID);

					if( strProgID == szViewAXProgID )
					{					 

						::RestoreContainerFromDataObject( sptrRepCtrl, sptrViewCtrl );						

						if( sptrViewCtrl != NULL )
						{

							CString strActiveObject = 
								::GetValueString( GetAppUnknown(), "Print", SD_ACTIVE_OBJECT,"" );

							CString strViewProgID = 
								::GetValueString( GetAppUnknown(), "Print", SD_ACTIVE_VIEW_TYPE,"" );

							_bstr_t bstrActiveObject( strActiveObject );
							_bstr_t bstrViewProgID( strViewProgID );

							
							//6.10.200 Cos need build
							/*
							int nTemp;
							ISOK( sptrViewCtrl->GetObjectDefinition(&nTemp) );
							ObjectDefinition od = (ObjectDefinition)nTemp;							
							

							if( od == odActiveView )
							{
								ISOK(sptrViewCtrl->SetViewProgID( bstrViewProgID ));
								ISOK(sptrViewCtrl->SetObject( bstrActiveObject ));

								::StoreContainerToDataObject( sptrRepCtrl, sptrViewCtrl ); 
							}
							else
							{
								for( i=0;i<viewAXList.GetSize();i++ )
								{
									ViewAXData* pViewAX = viewAXList[i]; 

									INumeredObjectPtr ptrNumObj( sptrRepCtrl );
									GUID nKey;
									if( ptrNumObj )
										ISOK( ptrNumObj->GetKey(&nKey) );

									if( nKey == pViewAX->nKey )
									{
										_bstr_t bstrActiveVew( pViewAX->strObjName );
										ISOK(sptrViewCtrl->SetObject( bstrActiveVew ));
										::StoreContainerToDataObject( sptrRepCtrl, sptrViewCtrl ); 
									}
								}
							}
							*/

						}
					}
				}
			}
			punk->Release();
		}






	}



	for( int i=0;i<docObjList.GetSize();i++ )
		delete docObjList[i];		
	
	docObjList.RemoveAll();

	for( i=0;i<viewAXList.GetSize();i++ )
		delete viewAXList[i];		
	
	viewAXList.RemoveAll();


	//delete []arrObjectTypes;
	

	return TRUE;

}


CString FindObjectTypeByViewProgID( CString strViewProgID/*, 
								   CString *arrObjectType, int nTypeCount */ )
{
	CString strObjectType;

	CLSID	clsid;
	BSTR bstr = strViewProgID.AllocSysString();
	if( ::CLSIDFromProgID( bstr, &clsid ) )
		return "";

	IUnknown* punkView = NULL;

	::SysFreeString( bstr );
	HRESULT hr = CoCreateInstance(clsid,
       NULL, CLSCTX_INPROC_SERVER,
       IID_IUnknown, (LPVOID*)&punkView);

	if( hr != S_OK)
	{
		punkView = NULL;
		return "";		
	}
	

	IUnknown	*punkApp = GetAppUnknown();

	sptrIDataTypeInfoManager	sptrT( punkApp );

	long	nTypesCount = 0;
	sptrT->GetTypesCount( &nTypesCount );

	ASSERT(nTypesCount>0);
	CString* arrObjectTypes = new CString[nTypesCount];


	for( long nType = 0; nType < nTypesCount; nType++ )
	{
		BSTR	bstr = 0;
		sptrT->GetType( nType, &bstr );
		arrObjectTypes[nType] = bstr;
		::SysFreeString( bstr );
	}

		 
	

	sptrIView sptrView( punkView );
	if( sptrView )
	{
		BOOL bFind = FALSE;
		for( int i=0;i<nTypesCount;i++)
		{
			if( !bFind )
			{
				BSTR bsrtType = arrObjectTypes[i].AllocSysString();
				DWORD dwMatch = 0;
				ISOK( sptrView->GetMatchType( bsrtType, &dwMatch));
				if( dwMatch & mvFull )
				{
					bFind = TRUE;
					strObjectType = arrObjectTypes[i];
				}
				::SysFreeString( bsrtType );
			}
		}

	}

	punkView->Release();

	delete []arrObjectTypes;

	return strObjectType;
}
