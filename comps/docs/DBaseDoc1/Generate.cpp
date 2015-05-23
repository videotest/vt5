#include "stdafx.h"
#include "resource.h"
#include "dbasedoc.h"
#include "docman.h"
#include "Generate.h"
#include "GenerateDlg.h"
#include "afxpriv.h"


#define szViewAXProgID  "VIEWAX.ViewAXCtrl.1"


#define CTRL_MARGINS_PIXEL			5
#define CTRL_MARGINS_MM				FROM_DOUBLE_TO_VTPIXEL(5)

#define CTRL_WIDTH_IMAGE_PIXEL		200
#define CTRL_HEIGHT_IMAGE_PIXEL		200

#define CTRL_WIDTH_IMAGE_MM			FROM_DOUBLE_TO_VTPIXEL(100)
#define CTRL_HEIGHT_IMAGE_MM		FROM_DOUBLE_TO_VTPIXEL(100)


#define CTRL_WIDTH_TEXT_PIXEL		200
#define CTRL_HEIGHT_TEXT_PIXEL		30

#define CTRL_WIDTH_TEXT_MM			FROM_DOUBLE_TO_VTPIXEL(70)
#define CTRL_HEIGHT_TEXT_MM			FROM_DOUBLE_TO_VTPIXEL(5)

#define FORM_WIDTH_MAX				640



/*
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XDBaseDocument::Generate( BSTR bstrEvent, BSTR bstrUIName )
{
	_try_nested(CDBaseDocument, DBaseDocument, Generate)
	{
		pThis->Generate( bstrEvent, bstrUIName );
		return S_OK;
	}
	_catch_nested;
}
*/
/*
////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::Generate( CString strEvent, CString strUIName )
{
	if( strEvent.IsEmpty() || 
			(	(strEvent == szEventOnAddTable ||
				 strEvent == szEventOnEditTable ||
				 strEvent == szEventOnDropTable ||

				 szEventGenerateAXFormByQuery ||
				 szEventGenerateReportByQuery ||
				 szEventGenerateReportByAXForm ||

				 strEvent == szEventAnalizeAXForm ||
				 strEvent == szEventAnalizeReport ||
				 strEvent == szEventAnalizeQuery
				)
				&& strUIName.IsEmpty()
			)
		)
	{
		CGenerateDlg dlg( ::GetMainFrameWnd() );
		dlg.SetDBaseDocument( GetControllingUnknown() );
		dlg.m_strEvent = strEvent;
		dlg.m_strUIName = strUIName;
		dlg.DoModal();

		strEvent	= dlg.m_strEvent;
		strUIName	= dlg.m_strUIName;
	}


	if( strEvent == szEventOnAddTable )	
	{
		OnEventAddTable( strUIName );
	}
	else
	if( strEvent == szEventOnEditTable )	
	{
		OnEventEditTable( strUIName );
	}
	else
	if( strEvent == szEventOnDropTable )	
	{
		OnEventDropTable( strUIName );
	}
	else
	if( strEvent == szEventGenerateAXFormByQuery )	
	{
		OnEventGenerateAXFormByQuery( strUIName );
	}
	else
	if( strEvent == szEventGenerateReportByQuery )
	{
		OnEventGenerateReportByQuery( strUIName );
	}
	else
	if( strEvent == szEventGenerateReportByAXForm )	
	{
		OnEventGenerateReportByAXForm( strUIName );
	}
	else
	if( strEvent == szEventAnalizeAXForm )	
	{
		OnEventAnalizeAXForm( strUIName );
	}
	else
	if( strEvent == szEventAnalizeReport )	
	{
		OnEventAnalizeReport( strUIName );
	}
	if( strEvent == szEventAnalizeQuery )	
	{
		OnEventAnalizeQuery( strUIName );
	}
	if( strEvent == szEventAnalizeAllAXForms )	
	{
		OnEventAnalizeAllAXForms( );
	}
	if( strEvent == szEventAnalizeAllReports )	
	{
		OnEventAnalizeAllReports( );
	}
	if( strEvent == szEventAnalizeAllQueries )	
	{
		OnEventAnalizeAllQueries( );
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////
//
//		DDL event support
//
////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::OnEventAddTable( CString strTable )
{
	// need add new Query and then add new form && report.
	if( !m_dbEngine.IsSessionOK() )
		return false;

	CArray<_CTableInfo*, _CTableInfo*>* pArrTableInfo = NULL;
	if( !m_dbEngine.GetTablesInfo( &pArrTableInfo, false ) )
		return false;

	if( pArrTableInfo == NULL )
		return false;

	int nTableIndex = -1;

	for( int i=0;i<pArrTableInfo->GetSize();i++ )
	{
		if( (*pArrTableInfo)[i]->m_strTableName == strTable )
			nTableIndex = i;
	}
	
	if( nTableIndex < 0 )
		return false;


	IUnknown* punkQuery = NULL;
	punkQuery = ::CreateTypedObject( szTypeQueryObject );	
	if( punkQuery == NULL )
		return false;

	sptrIQueryObject spQuery( punkQuery );
	punkQuery->Release();
	if( spQuery == NULL )
		return false;


	spQuery->SetMainTable( _bstr_t( (LPCTSTR)strTable ) );
	spQuery->SetUseNativeSQL( FALSE );	

	bool bFoundCaption  = false;
	bool bFoundTumbnail = false;



	sptrISelectQuery spSelectQuery( spQuery );

	for( i=0;i<(*pArrTableInfo)[nTableIndex]->m_arrFieldInfo.GetSize();i++ )
	{
		_CFieldInfo* pFI = (*pArrTableInfo)[nTableIndex]->m_arrFieldInfo[i];
		spQuery->AddField( _bstr_t( (LPCTSTR)strTable ), _bstr_t( (LPCTSTR)pFI->m_strFieldName) );		

		if( !bFoundCaption )
		{
			if( ::GetFieldType( GetControllingUnknown(), strTable, pFI->m_strFieldName ) 
				== ftString )
			{
				spSelectQuery->SetTumbnailCaptionLocation( 
							_bstr_t( (LPCTSTR)strTable ), 
							_bstr_t( (LPCTSTR)pFI->m_strFieldName) );
				bFoundCaption = true;
			}
		}

		if( !bFoundTumbnail )
		{
			if( ::GetFieldType( GetControllingUnknown(), strTable, pFI->m_strFieldName ) 
				== ftVTObject )
			{
				spSelectQuery->SetTumbnailLocation( 
							_bstr_t( (LPCTSTR)strTable ), 
							_bstr_t( (LPCTSTR)pFI->m_strFieldName) );
				bFoundTumbnail = true;
			}
		}

	}


	INamedObject2Ptr spReportNO( spQuery );
	_bstr_t bstrQueryName;
	
	if( spReportNO  )			
	{
		BSTR bstrName;
		spReportNO->GenerateUniqueName( _bstr_t( (LPCTSTR)"Query_" + strTable ) );
		spReportNO->GetName( &bstrName );
		bstrQueryName = bstrName;
		::SysFreeString( bstrName );
	}
	
	INumeredObjectPtr	ptrNum( spQuery );
	if( ptrNum )
		ptrNum->GenerateNewKey( 0 );

	::_SetValue( GetControllingUnknown(), _bstr_t((LPCTSTR)""), bstrQueryName, 
					_variant_t((IUnknown*)spQuery) );

	::_SetValue( GetControllingUnknown(), 
		SECTION_DBASE, SECTION_ACTIVE_QUERY, (LPCTSTR)bstrQueryName );	

	
	OnEventGenerateAXFormByQuery( (LPCTSTR)bstrQueryName );
	OnEventGenerateReportByQuery( (LPCTSTR)bstrQueryName );

	
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::OnEventEditTable( CString strTable )
{
	//bool bPrevState = CloseActiveQuery( );

	OnEventAnalizeAllQueries( );
	OnEventAnalizeAllAXForms( );
	OnEventAnalizeAllReports( );
	
	//OpenActiveQuery( bPrevState );
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::OnEventDropTable( CString strTable )
{
	//bool bPrevState = CloseActiveQuery( );

	OnEventAnalizeAllQueries( );
	OnEventAnalizeAllAXForms( );
	OnEventAnalizeAllReports( );	
	
	//OpenActiveQuery( bPrevState );

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//		Generation support
//
////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::OnEventGenerateAXFormByQuery( CString strQuery )
{	
	sptrIDBaseDocument spDBaseDoc( GetControllingUnknown() );
	if( spDBaseDoc == NULL )
		return false;

	//Get query unknown
	IUnknown* pUnkQuery = NULL;
	//spDBaseDoc->GetQueryUnknownByName( 0, &pUnkQuery );
	if( pUnkQuery == NULL )
		return false;

	sptrIQueryObject spQuery( pUnkQuery );
	pUnkQuery->Release();

	if( spQuery == NULL )
		return false;

	int nFieldCount = 0;
	spQuery->GetFieldsCount( &nFieldCount );
	if( nFieldCount < 1 )
		return false;


	//Create AXForm
	IUnknown* punkAXForm = NULL;
	punkAXForm = ::CreateTypedObject( szTypeBlankForm );	
	if( punkAXForm == NULL )
		return false;

	sptrIActiveXForm spAXForm( punkAXForm );
	punkAXForm->Release();

	if( spAXForm == NULL ) 
		return false;

	_bstr_t bstrAXFormName;
	INamedObject2Ptr spAXFormNO( spAXForm );
	if( spAXFormNO  )			
	{
		BSTR bstrName;
		spAXFormNO->GenerateUniqueName( _bstr_t( (LPCTSTR)"Blank_" + strQuery ) );
		spAXFormNO->GetName( &bstrName );
		bstrAXFormName = bstrName;
		::SysFreeString( bstrName );

	}

	INumeredObjectPtr	ptrNumAXForm( spAXForm );
	if( ptrNumAXForm )
		ptrNumAXForm->GenerateNewKey( 0 );

	::_SetValue( spDBaseDoc, _bstr_t((LPCTSTR)""), bstrAXFormName, _variant_t((IUnknown*)spAXForm) );

	int nCurPosX = CTRL_MARGINS_PIXEL;
	int nCurPosY = CTRL_MARGINS_PIXEL;		
	
	CRect rcCtrl = CRect( CTRL_MARGINS_PIXEL, CTRL_MARGINS_PIXEL, 0, 0);

	//Add VTObject
	bool bFirstEnter = true;
	for( int i=0;i<nFieldCount;i++)
	{
		BSTR bstrTable, bstrField;
		spQuery->GetField( i, &bstrTable, &bstrField );
		CString strTable, strField;
		strTable = bstrTable;
		strField = bstrField;
		::SysFreeString( bstrTable );
		::SysFreeString( bstrField );

		FieldType fieldType = ::GetFieldType( spDBaseDoc, strTable, strField );		
		if( fieldType == ftVTObject )
		{
			AddFieldToAXForm( strTable, strField, rcCtrl, spAXForm, bFirstEnter );
			bFirstEnter = false;
		}
	}

	//Add Number
	bFirstEnter = true;
	for( i=0;i<nFieldCount;i++)
	{
		BSTR bstrTable, bstrField;
		spQuery->GetField( i, &bstrTable, &bstrField );
		CString strTable, strField;
		strTable = bstrTable;
		strField = bstrField;
		::SysFreeString( bstrTable );
		::SysFreeString( bstrField );

		FieldType fieldType = ::GetFieldType( spDBaseDoc, strTable, strField );
		
		if( fieldType == ftDigit )
		{
			AddFieldToAXForm( strTable, strField, rcCtrl, spAXForm, bFirstEnter );
			bFirstEnter = false;
		}
	}

	//Add Text
	bFirstEnter = true;
	for( i=0;i<nFieldCount;i++)
	{
		BSTR bstrTable, bstrField;
		spQuery->GetField( i, &bstrTable, &bstrField );
		CString strTable, strField;
		strTable = bstrTable;
		strField = bstrField;
		::SysFreeString( bstrTable );
		::SysFreeString( bstrField );

		FieldType fieldType = ::GetFieldType( spDBaseDoc, strTable, strField );
		
		if( fieldType == ftString )
		{
			AddFieldToAXForm( strTable, strField, rcCtrl, spAXForm, bFirstEnter );
			bFirstEnter = false;
		}
	}

	//Add DateTime
	bFirstEnter = true;
	for( i=0;i<nFieldCount;i++)
	{
		BSTR bstrTable, bstrField;
		spQuery->GetField( i, &bstrTable, &bstrField );
		CString strTable, strField;
		strTable = bstrTable;
		strField = bstrField;
		::SysFreeString( bstrTable );
		::SysFreeString( bstrField );

		FieldType fieldType = ::GetFieldType( spDBaseDoc, strTable, strField );
		
		if( fieldType == ftDateTime )
		{
			AddFieldToAXForm( strTable, strField, rcCtrl, spAXForm, bFirstEnter );
			bFirstEnter = false;
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::OnEventGenerateReportByQuery( CString strQuery )
{
	sptrIDBaseDocument spDBaseDoc( GetControllingUnknown() );
	if( spDBaseDoc == NULL )
		return false;

	//Get query unknown
	IUnknown* pUnkQuery = NULL;
	//spDBaseDoc->GetQueryUnknownByName( _bstr_t((LPCTSTR)strQuery), &pUnkQuery );
	if( pUnkQuery == NULL )
		return false;

	sptrIQueryObject spQuery( pUnkQuery );
	pUnkQuery->Release();

	if( spQuery == NULL )
		return false;

	int nFieldCount = 0;
	spQuery->GetFieldsCount( &nFieldCount );
	if( nFieldCount < 1 )
		return false;


	//Create Report
	IUnknown* punkReport = NULL;
	punkReport = ::CreateTypedObject( szTypeReportForm );	
	if( punkReport == NULL )
		return false;

	sptrIReportForm spReport( punkReport );
	punkReport->Release();

	if( spReport == NULL ) 
		return false;

	_bstr_t bstrReportName;
	INamedObject2Ptr spReportNO( spReport );
	if( spReportNO  )			
	{
		BSTR bstrName;
		spReportNO->GenerateUniqueName( _bstr_t( (LPCTSTR)"Report_" + strQuery ) );
		spReportNO->GetName( &bstrName );
		bstrReportName = bstrName;
		::SysFreeString( bstrName );

	}

	INumeredObjectPtr	ptrNumReport( spReport );
	if( ptrNumReport )
		ptrNumReport->GenerateNewKey( 0 );

	::_SetValue( spDBaseDoc, _bstr_t((LPCTSTR)""), bstrReportName, _variant_t((IUnknown*)spReport) );


	CRect rcFields;
	spReport->GetPaperField( &rcFields );

	CRect rcCtrl = CRect( rcFields.left + CTRL_MARGINS_MM, rcFields.top + CTRL_MARGINS_MM,
						rcFields.left + CTRL_MARGINS_MM, rcFields.top + CTRL_MARGINS_MM );

	int nCurRow = 0;
	//Add VTObject
	bool bFirstEnter = true;
	for( int i=0;i<nFieldCount;i++)
	{
		BSTR bstrTable, bstrField;
		spQuery->GetField( i, &bstrTable, &bstrField );
		CString strTable, strField;
		strTable = bstrTable;
		strField = bstrField;
		::SysFreeString( bstrTable );
		::SysFreeString( bstrField );

		FieldType fieldType = ::GetFieldType( spDBaseDoc, strTable, strField );		
		if( fieldType == ftVTObject )
		{
			AddFieldToReport(	strTable, strField, rcCtrl,
								spReport, bFirstEnter, nCurRow );
			bFirstEnter = false;
		}
	}

	//Add Number
	bFirstEnter = true;
	for(i=0;i<nFieldCount;i++)
	{
		BSTR bstrTable, bstrField;
		spQuery->GetField( i, &bstrTable, &bstrField );
		CString strTable, strField;
		strTable = bstrTable;
		strField = bstrField;
		::SysFreeString( bstrTable );
		::SysFreeString( bstrField );

		FieldType fieldType = ::GetFieldType( spDBaseDoc, strTable, strField );		
		if( fieldType == ftDigit )
		{
			AddFieldToReport(	strTable, strField, rcCtrl,
								spReport, bFirstEnter, nCurRow );
			bFirstEnter = false;
		}
	}

	//Add Text
	bFirstEnter = true;
	for( i=0;i<nFieldCount;i++)
	{
		BSTR bstrTable, bstrField;
		spQuery->GetField( i, &bstrTable, &bstrField );
		CString strTable, strField;
		strTable = bstrTable;
		strField = bstrField;
		::SysFreeString( bstrTable );
		::SysFreeString( bstrField );

		FieldType fieldType = ::GetFieldType( spDBaseDoc, strTable, strField );
		
		if( fieldType == ftString )
		{
			AddFieldToReport(	strTable, strField, rcCtrl,
								spReport, bFirstEnter, nCurRow );
			bFirstEnter = false;
		}
	}

	//Add DateTime
	bFirstEnter = true;
	for( i=0;i<nFieldCount;i++)
	{
		BSTR bstrTable, bstrField;
		spQuery->GetField( i, &bstrTable, &bstrField );
		CString strTable, strField;
		strTable = bstrTable;
		strField = bstrField;
		::SysFreeString( bstrTable );
		::SysFreeString( bstrField );

		FieldType fieldType = ::GetFieldType( spDBaseDoc, strTable, strField );
		
		if( fieldType == ftDateTime )
		{
			AddFieldToReport(	strTable, strField, rcCtrl,
								spReport, bFirstEnter, nCurRow );
			bFirstEnter = false;
		}
	}
	

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::OnEventGenerateReportByAXForm( CString strAXForm )
{

	sptrIDBaseDocument spDBaseDoc( GetControllingUnknown() );
	if( spDBaseDoc == NULL )
		return false;

	IUnknown* pUnkForm = NULL;
	pUnkForm = ::GetObjectByName( GetControllingUnknown(), strAXForm, szTypeBlankForm );
	if( pUnkForm == NULL )
		return false;

	sptrIActiveXForm spAXForm( pUnkForm );
	pUnkForm->Release();

	if( spAXForm == NULL )
		return false;
	

	//Create Report
	IUnknown* punkReport = NULL;
	punkReport = ::CreateTypedObject( szTypeReportForm );	
	if( punkReport == NULL )
		return false;

	sptrIReportForm spReport( punkReport );
	punkReport->Release();

	if( spReport == NULL ) 
		return false;

	_bstr_t bstrReportName;
	INamedObject2Ptr spReportNO( spReport );
	if( spReportNO  )			
	{
		BSTR bstrName;
		spReportNO->GenerateUniqueName( _bstr_t( (LPCTSTR)"ReportBy" + strAXForm ) );
		spReportNO->GetName( &bstrName );
		bstrReportName = bstrName;
		::SysFreeString( bstrName );

	}

	INumeredObjectPtr	ptrNumReport( spReport );
	if( ptrNumReport )
		ptrNumReport->GenerateNewKey( 0 );

	::_SetValue( spDBaseDoc, _bstr_t((LPCTSTR)""), bstrReportName, _variant_t((IUnknown*)spReport) );


	
	//Wanna place all controls to one page. Calc zoom factor.	

	//Get report work area
	CRect rcFields;
	spReport->GetPaperField( &rcFields );
	CSize pageSize;
	spReport->GetPaperSize( &pageSize );	
	
	
	CSize sizePrnArea( pageSize.cx - rcFields.Width(), pageSize.cy - rcFields.Height() );
	
	//Get AXForm work area
	CSize sizeAxFormArea;
	spAXForm->AutoUpdateSize( );
	spAXForm->GetSize( &sizeAxFormArea );	

	if( sizeAxFormArea.cx < 1 || sizeAxFormArea.cy < 1 || 
		sizePrnArea.cx < 1 || sizePrnArea.cy < 1
		)
		return false;
	

	double fReportWidth		= (double)sizePrnArea.cx;
	double fReportHeight	= (double)sizePrnArea.cy;
	
	double fReportZoom	= fReportWidth / fReportHeight;

	double fFormWidth		= (double)sizeAxFormArea.cx;
	double fFormHeight		= (double)sizeAxFormArea.cy;

	double fFormZoom		= fFormWidth / fFormHeight;		

	
	CSize sizeOutput( 0, 0 );

	if( fFormZoom > 1.0 ) //Form width > Form height
	{
		sizeOutput	= CSize ( fReportWidth, fReportWidth / fFormZoom );		
	}
	else//Form width < Form height
	{
		sizeOutput	= CSize ( fReportHeight * fFormZoom, fReportHeight );		
	}

	double fZoom = (double)sizeOutput.cx / fFormWidth;



	//Now put controls
	CObjectListWrp	list( spAXForm );
	POSITION	pos = list.GetFirstObjectPosition();	


	while( pos )
	{			
		IUnknown *punk = list.GetNextObject( pos );
		sptrIActiveXCtrl sptrAXCtrl( punk );
		if( sptrAXCtrl )
		{
			CRect rcAXCtrl;
			sptrAXCtrl->GetRect( &rcAXCtrl );
			
			rcAXCtrl.left	*= fZoom;
			rcAXCtrl.top	*= fZoom;
			rcAXCtrl.right	*= fZoom;
			rcAXCtrl.bottom	*= fZoom;

			
			rcAXCtrl.left	+= rcFields.left;
			rcAXCtrl.top	+= rcFields.top;
			rcAXCtrl.right	+= rcFields.left;
			rcAXCtrl.bottom	+= rcFields.top;


			IUnknown* punkReportCtrl = NULL;
			punkReportCtrl = ::CreateTypedObject( szTypeReportCtrl );
			if( punkReportCtrl == NULL )
				continue;

			sptrIReportCtrl	spReportCtrl( punkReportCtrl );
			punkReportCtrl->Release();
			if( spReportCtrl == NULL )
				continue;


			sptrINamedDataObject2 sptrNDO2( spReportCtrl );
			if( sptrNDO2 == NULL )
				continue;
			
			sptrNDO2->SetParent( spReport, 0 );

			{
					CMemFile fileMem;
					//loading
					{
						CArchive arSourse( &fileMem, CArchive::store );
						CArchiveStream	streamSource( &arSourse );

						sptrISerializableObject	sptrSource( sptrAXCtrl );
						sptrSource->Store( &streamSource );
						arSourse.Flush();
						fileMem.Flush();

					}
					fileMem.SeekToBegin();
					//storing
					{
						CArchive arTarget( &fileMem, CArchive::load );
						CArchiveStream	streamTarget(&arTarget);

						sptrISerializableObject	sptrTarget( spReportCtrl );								
						sptrTarget->Load( &streamTarget );
					}
				
			}

			spReportCtrl->SetDesignOwnerPage( 0 );
			spReportCtrl->SetDesignColRow( 0, 0 );
			spReportCtrl->SetDesignPosition( rcAXCtrl );

		}
		punk->Release();
	}
	

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//		Analize Form, Reports & Queries for available fields & add new if need
//
////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::OnEventAnalizeAXForm( CString strAXForm )
{
	CArray<CFieldBase, CFieldBase> arrDBaseFields;
	CArray<CFieldBase, CFieldBase> arrAXFormFields;
	
	CArray<CFieldBase, CFieldBase> arrAddFields;
	CArray<CFieldBase, CFieldBase> arrRemoveFields;

	IUnknown* pUnkForm = NULL;
	pUnkForm = ::GetObjectByName( GetControllingUnknown(), strAXForm, szTypeBlankForm );
	if( pUnkForm == NULL )
		return false;

	IUnknownPtr ptrForm = pUnkForm;
	pUnkForm->Release();


	GetDBaseFields( arrDBaseFields );
	GetAXFormFields( ptrForm, arrAXFormFields );


	GetAddFieldsFrom2Array( arrDBaseFields, arrAXFormFields, arrAddFields );
	GetRemoveFieldsFrom2Array( arrDBaseFields, arrAXFormFields, arrRemoveFields );


	for( int i=0;i<arrRemoveFields.GetSize();i++ )
	{
		RemoveFieldFromAXForm( arrRemoveFields[i].m_strTable, 
							   arrRemoveFields[i].m_strField,
							   ptrForm );
	}


	CRect rcCtrl = CRect( CTRL_MARGINS_PIXEL, CTRL_MARGINS_PIXEL, 0, 0);
	GetLastRectFromAXForm( ptrForm, rcCtrl );

	for( i=0;i<arrAddFields.GetSize();i++ )
	{
		AddFieldToAXForm( arrAddFields[i].m_strTable, 
						  arrAddFields[i].m_strField,
						  rcCtrl, ptrForm, true	);
	}

	arrDBaseFields.RemoveAll();
	arrAXFormFields.RemoveAll();
	arrAddFields.RemoveAll();
	arrRemoveFields.RemoveAll();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::OnEventAnalizeReport( CString strReport )
{
	CArray<CFieldBase, CFieldBase> arrDBaseFields;
	CArray<CFieldBase, CFieldBase> arrReportFields;
	
	CArray<CFieldBase, CFieldBase> arrAddFields;
	CArray<CFieldBase, CFieldBase> arrRemoveFields;

	IUnknown* pUnkReport = NULL;
	pUnkReport = ::GetObjectByName( GetControllingUnknown(), strReport, szTypeReportForm );
	if( pUnkReport == NULL )
		return false;

	IUnknownPtr ptrReport = pUnkReport;
	pUnkReport->Release();


	GetDBaseFields( arrDBaseFields );
	GetReportFields( ptrReport, arrReportFields );


	GetAddFieldsFrom2Array( arrDBaseFields, arrReportFields, arrAddFields );
	GetRemoveFieldsFrom2Array( arrDBaseFields, arrReportFields, arrRemoveFields );


	for( int i=0;i<arrRemoveFields.GetSize();i++ )
	{
		RemoveFieldFromReport( arrRemoveFields[i].m_strTable, 
							   arrRemoveFields[i].m_strField,
							   ptrReport );
	}


	CRect rcCtrl = CRect( CTRL_MARGINS_MM, CTRL_MARGINS_MM, 0, 0);	

	int nRow, nCol, nMainPage;

	GetLastRectFromReport( ptrReport, rcCtrl, nRow, nCol, nMainPage );

	for( i=0;i<arrAddFields.GetSize();i++ )
	{
		AddFieldToReport( arrAddFields[i].m_strTable, 
						  arrAddFields[i].m_strField,
						  rcCtrl, ptrReport, true,
						  nRow, nCol, nMainPage
						  );
	}

	arrDBaseFields.RemoveAll();
	arrReportFields.RemoveAll();
	arrAddFields.RemoveAll();
	arrRemoveFields.RemoveAll();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::OnEventAnalizeQuery( CString strQuery )
{
	CArray<CFieldBase, CFieldBase> arrDBaseFields;
	CArray<CFieldBase, CFieldBase> arrQueryFields;

	CArray<CFieldBase, CFieldBase> arrAddFields;
	CArray<CFieldBase, CFieldBase> arrRemoveFields;

	IUnknown* pUnkQuery = NULL;
	pUnkQuery = ::GetObjectByName( GetControllingUnknown(), strQuery, szTypeQueryObject );
	if( pUnkQuery == NULL )
		return false;

	sptrIQueryObject spQuery( pUnkQuery );
	pUnkQuery->Release();
	if( spQuery == NULL )
		return false;

	bool bPrevState = CloseActiveQuery( );

	CString strMainTable;
	{
		BSTR bstrMainTable;
		spQuery->GetMainTable( &bstrMainTable );
		strMainTable = bstrMainTable;
		::SysFreeString( bstrMainTable );
	}
	
	int nFieldCount = 0;
	spQuery->GetFieldsCount( &nFieldCount );	
	for( int i=0;i<nFieldCount;i++)
	{
		BSTR bstrTable, bstrField;
		spQuery->GetField( i, &bstrTable, &bstrField );
		CString strTable, strField;
		strTable = bstrTable;
		strField = bstrField;
		::SysFreeString( bstrTable );
		::SysFreeString( bstrField );
		
		CFieldBase fieldBase;
		fieldBase.m_strTable = strTable;
		fieldBase.m_strField = strField;
		
		arrQueryFields.Add( fieldBase );
	}


	GetDBaseFields( arrDBaseFields );
	for( i=arrDBaseFields.GetSize()-1;i>=0;i-- )
	{
		if( arrDBaseFields[i].m_strTable != strMainTable )		
		{
			arrDBaseFields.RemoveAt(i);					
		}
	}


	GetAddFieldsFrom2Array( arrDBaseFields, arrQueryFields, arrAddFields );
	GetRemoveFieldsFrom2Array( arrDBaseFields, arrQueryFields, arrRemoveFields );
	
	//Remove field
	for( i=0;i<arrRemoveFields.GetSize();i++)
	{
		for( int j=nFieldCount-1;j>=0;j--)
		{
			BSTR bstrTable, bstrField;
			spQuery->GetField( j, &bstrTable, &bstrField );
			CString strTable, strField;
			strTable = bstrTable;
			strField = bstrField;
			::SysFreeString( bstrTable );
			::SysFreeString( bstrField );			
			if( strTable == arrRemoveFields[i].m_strTable &&
				strField == arrRemoveFields[i].m_strTable
				)
			{
				spQuery->DeleteField( i );
			}
		}		
	}
	
	//Remove sort fields
	int nSortCount = -1;
	spQuery->GetSortFieldCount( &nSortCount );
	for( i=nSortCount-1;i>=0;i-- )
	{
		BSTR bstrTable, bstrField;
		BOOL bASC;
		bstrTable = bstrField = NULL;
		if( S_OK != spQuery->GetSortField( i, &bstrTable, &bstrField, &bASC ) )
			continue;

		CString strTable, strField;
		
		strTable = bstrTable;
		strField = bstrField;

		::SysFreeString( bstrTable );
		::SysFreeString( bstrField );
		
		for( int j=0;j<arrRemoveFields.GetSize();j++ )	
		{
			if( strTable == arrRemoveFields[j].m_strTable && 
				strField == arrRemoveFields[j].m_strField )
			{
				spQuery->DeleteSortField( i );
			}
		}
	}
	
	//Remove grid fields
	int nGridCount = -1;
	spQuery->GetGridFieldCount( &nGridCount );
	for( i=nGridCount-1;i>=0;i-- )
	{
		BSTR bstrTable, bstrField, bstrColumnCaption;
		int nColumnWidth;
		bstrTable = bstrField = bstrColumnCaption = NULL;
		if( S_OK != spQuery->GetGridField( i, &bstrTable, &bstrField, &bstrColumnCaption, &nColumnWidth ) )
			continue;

		CString strTable, strField;
		
		strTable = bstrTable;
		strField = bstrField;

		::SysFreeString( bstrTable );
		::SysFreeString( bstrField );
		::SysFreeString( bstrColumnCaption );
		
		
		for( int j=0;j<arrRemoveFields.GetSize();j++ )	
		{
			if( strTable == arrRemoveFields[j].m_strTable && 
				strField == arrRemoveFields[j].m_strField )
			{
				spQuery->DeleteGridField( i );
			}
		}
	}

	//Remove filter condition
	int nFilterCount = 0;
	spQuery->GetFiltersCount( &nFilterCount );
	for( int nFilterIndex=nFilterCount-1;nFilterIndex>=0;nFilterIndex-- )
	{
		BSTR bstrFilterName = NULL;
		if( S_OK != spQuery->GetFilter( nFilterIndex, &bstrFilterName ) )
			continue;

		_bstr_t bstrFilter( bstrFilterName );
		::SysFreeString( bstrFilterName );

		int nFilterCondCount = -1;
		spQuery->GetFilterConditionCount( bstrFilter, &nFilterCondCount );
		for( int nCondIndex=nFilterCondCount-1;nCondIndex>=0;nCondIndex-- )
		{
			BSTR bstrTable, bstrField;
			BSTR bstrCondition, bstrValue, bstrValueTo;
			BOOL bANDunion;
			int nLeftBracketCount, nRightBracketCount;

			bstrTable = bstrField = bstrCondition = bstrValue = bstrValueTo = NULL;

			if( S_OK != spQuery->GetFilterCondition( bstrFilter, nCondIndex, 
											&bstrTable, &bstrField,
											&bstrCondition, &bstrValue, &bstrValueTo, 
											&bANDunion, 
											&nLeftBracketCount, &nRightBracketCount	
											) )
			{
				continue;
			}

			CString strTable, strField;
			
			strTable = bstrTable;
			strField = bstrField;

			::SysFreeString( bstrTable );
			::SysFreeString( bstrField );
			::SysFreeString( bstrCondition );
			::SysFreeString( bstrValue );
			::SysFreeString( bstrValueTo );						
			
			for( int j=0;j<arrRemoveFields.GetSize();j++ )	
			{
				if( strTable == arrRemoveFields[j].m_strTable && 
					strField == arrRemoveFields[j].m_strField )
				{
					spQuery->DeleteFilterCondition( bstrFilter, nCondIndex );
				}
			}


		}
	}




	//Add fields to query
	for( i=0;i<arrAddFields.GetSize();i++)
	{
		spQuery->AddField(  _bstr_t((LPCTSTR)arrAddFields[i].m_strTable),
							_bstr_t((LPCTSTR)arrAddFields[i].m_strField)
							);
	}


	OpenActiveQuery( bPrevState );



	arrDBaseFields.RemoveAll();
	arrQueryFields.RemoveAll();
	arrAddFields.RemoveAll();
	arrRemoveFields.RemoveAll();

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::OnEventAnalizeAllAXForms( )
{
	CArray <CString, CString> arForm;
	if( !GetAXForms( arForm ) )
		return false;	

	for( int i=0;i<arForm.GetSize();i++ )
	{
		OnEventAnalizeAXForm( arForm[i] );
	}	

	arForm.RemoveAll();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::OnEventAnalizeAllReports( )
{
	CArray <CString, CString> arReport;
	if( !GetReports( arReport ) )
		return false;	

	for( int i=0;i<arReport.GetSize();i++ )
	{
		OnEventAnalizeReport( arReport[i] );
	}	

	arReport.RemoveAll();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::OnEventAnalizeAllQueries( )
{
	CArray <CString, CString> arQuery;
	if( !GetQueries( arQuery ) )
		return false;

	bool bPrevState = CloseActiveQuery( );

	for( int i=0;i<arQuery.GetSize();i++ )
	{
		OnEventAnalizeQuery( arQuery[i] );
	}

	OpenActiveQuery( bPrevState );

	arQuery.RemoveAll();

	return true;
}




////////////////////////////////////////////////////////////////////////////////////////////
//
//		Helper functions
//
////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::CloseActiveQuery( ) //return state( open, close )
{	
	sptrIDBaseDocument spDoc( GetControllingUnknown() );
	if( spDoc == NULL )
		return false;

	IUnknown* pUnkQuery = NULL;
	//spDoc->GetActiveQueryUnknown( &pUnkQuery );
	if( pUnkQuery == NULL )
		return false;

	sptrISelectQuery spQuery( pUnkQuery );
	pUnkQuery->Release();

	if( spQuery == NULL )
		return false;

	BOOL bOpen = FALSE;
	spQuery->IsOpen( &bOpen );	

	if( bOpen )
		spQuery->Close( );

	return (bOpen == TRUE);
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::OpenActiveQuery( bool bPrevSate )
{
	if( !bPrevSate )
		return true;

	sptrIDBaseDocument spDoc( GetControllingUnknown() );
	if( spDoc == NULL )
		return false;

	IUnknown* pUnkQuery = NULL;
	//spDoc->GetActiveQueryUnknown( &pUnkQuery );
	if( pUnkQuery == NULL )
		return false;

	sptrISelectQuery spQuery( pUnkQuery );
	pUnkQuery->Release();

	if( spQuery == NULL )
		return false;

	spQuery->Open();

	BOOL bOpen = FALSE;
	spQuery->IsOpen( &bOpen );	

	return (bOpen == TRUE);
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::GetAXForms( CArray<CString, CString>& arAXFrom )
{
	sptrIDataContext2 spDC( GetControllingUnknown() );
	if( spDC == NULL )
		return false;

	long nAXFormCount = 0;
	_bstr_t bstrAXFormType( szTypeBlankForm );
	
	spDC->GetObjectCount( bstrAXFormType, &nAXFormCount );
	for( int i=0;i<nAXFormCount;i++ )
	{
		IUnknown* pUnkAXForm = NULL;

		pUnkAXForm = ::GetObjectFromContextByPos( spDC, bstrAXFormType, i );

		//spDC->GetObject( bstrAXFormType, i, NULL, &pUnkAXForm );
		if( pUnkAXForm != NULL )
		{
			CString strAXFormName = ::GetObjectName( pUnkAXForm );
			pUnkAXForm->Release();

			arAXFrom.Add( strAXFormName );
		}
	}	

	return (arAXFrom.GetSize() < 1 );
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::GetReports( CArray<CString, CString>& arReport )
{
	sptrIDataContext2 spDC( GetControllingUnknown() );
	if( spDC == NULL )
		return false;

	long nReportCount = 0;
	_bstr_t bstrReportType( szTypeReportForm );
	
	spDC->GetObjectCount( bstrReportType, &nReportCount );
	for( int i=0;i<nReportCount;i++ )
	{
		IUnknown* pUnkReport = NULL;
		
		pUnkReport = ::GetObjectFromContextByPos( spDC, bstrReportType, i );
		
		//spDC->GetObject( bstrReportType, i, NULL, &pUnkReport );
		if( pUnkReport != NULL )
		{
			CString strReportName = ::GetObjectName( pUnkReport );
			pUnkReport->Release();

			arReport.Add( strReportName );
		}
	}	

	return (arReport.GetSize() < 1 );
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::GetQueries( CArray<CString, CString>& arQuery )
{
	sptrIDataContext2 spDC( GetControllingUnknown() );
	if( spDC == NULL )
		return false;

	long nQueryCount = 0;
	_bstr_t bstrQueryType( szTypeQueryObject );
	
	
	spDC->GetObjectCount( bstrQueryType, &nQueryCount );
	for( int i=0;i<nQueryCount;i++ )
	{
		IUnknown* pUnkQuery = NULL;
		
		pUnkQuery = ::GetObjectFromContextByPos( spDC, bstrQueryType, i );

		//spDC->GetObject( bstrQueryType, i, NULL, &pUnkQuery );
		if( pUnkQuery != NULL )
		{
			CString strQueryName = ::GetObjectName( pUnkQuery );
			pUnkQuery->Release();

			arQuery.Add( strQueryName );
		}
	}	

	return (arQuery.GetSize() < 1 );
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::AddFieldToAXForm( CString strTable, CString strField, 
									  CRect& rcCtrl, IUnknown* pUnkForm,
									  bool bNewType )
{
	sptrIActiveXForm spAXFrom( pUnkForm );
	if( spAXFrom == NULL )
		return false;

	sptrIDBaseDocument spDBDoc( GetControllingUnknown() );
	if( spDBDoc == NULL )
		return false;

	FieldType fieldType = ::GetFieldType( spDBDoc, strTable, strField );

	if( !(fieldType == ftDigit ||
		fieldType == ftDateTime ||
		fieldType == ftString || 
		fieldType == ftVTObject)
		)
		return false;

	IUnknown* punkCtrl = NULL;
	punkCtrl = ::CreateTypedObject( szTypeBlankCtrl );
	if( punkCtrl == NULL )
		return false;

	sptrIActiveXCtrl	spCtrl( punkCtrl );
	punkCtrl->Release();
	if( spCtrl == NULL )
		return false;

	CSize ctrlSize = CSize( CTRL_WIDTH_TEXT_PIXEL, CTRL_HEIGHT_TEXT_PIXEL );
	_bstr_t bstrProgID;
	if( fieldType == ftDigit )
	{
		bstrProgID = DBNumberProgID;
	}
	else
	if( fieldType == ftDateTime )
	{
		bstrProgID = DBDateTimeProgID;
	}
	else
	if( fieldType == ftString )
	{
		bstrProgID = DBTextProgID;
	}
	else
	if( fieldType == ftVTObject )
	{
		bstrProgID = DBVTObjectProgID;
		ctrlSize = CSize( CTRL_WIDTH_IMAGE_PIXEL, CTRL_HEIGHT_IMAGE_PIXEL );
	}
	else
		return false;

	spCtrl->SetProgID( bstrProgID );
	

	BSTR newName;
	spCtrl->GenerateUniqueName( _bstr_t((LPCTSTR)strField), &newName );
	spCtrl->SetName( newName );
	::SysFreeString( newName );

	
	CRect rcPrev = rcCtrl;

	if( bNewType )
	{
		rcCtrl.left		= CTRL_MARGINS_PIXEL;
		rcCtrl.right	= rcCtrl.left + ctrlSize.cx;		
		rcCtrl.top		= rcPrev.bottom + CTRL_MARGINS_PIXEL;		
	}
	else
	{
		if( rcCtrl.left + ctrlSize.cx > FORM_WIDTH_MAX )
		{
			rcCtrl.left = CTRL_MARGINS_PIXEL;
			rcCtrl.right	= rcCtrl.left + ctrlSize.cx;
			rcCtrl.top		= rcPrev.bottom + CTRL_MARGINS_PIXEL;			
		}
	}		
	
	rcCtrl.bottom	= rcCtrl.top + ctrlSize.cy + CTRL_MARGINS_PIXEL;

	//rcCtrl = CRect( ptPos.x, ptPos.y, ptPos.x + ctrlSize.cx, ptPos.y + ctrlSize.cy );
	spCtrl->SetRect( rcCtrl );

	sptrINamedDataObject2 sptrNDO2( spCtrl );
	if( sptrNDO2 == NULL )
		return false;
	
	sptrNDO2->SetParent( spAXFrom, 0 );			


	CLSID	clsid;		
	if( ::CLSIDFromProgID( bstrProgID, &clsid ) == S_OK )
	{
		IUnknown* punkDBControl = NULL;
		HRESULT hr = CoCreateInstance(clsid,
			NULL, CLSCTX_INPROC_SERVER,
			IID_IUnknown, (LPVOID*)&punkDBControl);
		if( hr == S_OK)
		{
			sptrIDBControl spDBCtrl( punkDBControl );
			if( spDBCtrl )
			{	
				spDBCtrl->SetTableName( _bstr_t( (LPCTSTR)strTable ) );
				spDBCtrl->SetFieldName( _bstr_t( (LPCTSTR)strField ) );

				if( !strcmp( (LPCTSTR)bstrProgID, DBVTObjectProgID) )
				{
					spDBCtrl->SetAutoLabel( FALSE );
				}
				

				::StoreContainerToDataObject( spCtrl, spDBCtrl ); 
			}
			punkDBControl->Release();			
		}
	}	



	CRect rcTemp = rcCtrl;

	rcCtrl.left = rcTemp.right + CTRL_MARGINS_PIXEL;
	rcCtrl.right = rcCtrl.left + rcTemp.Width();	


	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::RemoveFieldFromAXForm( CString strTable, CString strField, 
										   IUnknown* pUnkForm )
{
	sptrIActiveXForm spAXFrom( pUnkForm );
	if( spAXFrom == NULL )
		return false;

	sptrIDBaseDocument spDBDoc( GetControllingUnknown() );
	if( spDBDoc == NULL )
		return false;

	CObjectListWrp	list( spAXFrom );
	POSITION	pos = list.GetFirstObjectPosition();	

	while( pos )
	{			
		bool bNeedDeleteCtrl = false;

		IUnknown *punk = list.GetNextObject( pos );
		sptrIActiveXCtrl sptrAXCtrl( punk );
		if( sptrAXCtrl )
		{
			BSTR bstrProgID;
			sptrAXCtrl->GetProgID(&bstrProgID);
			CString strProgID = bstrProgID;
			::SysFreeString(bstrProgID);

			if( strProgID == DBDateTimeProgID ||
				strProgID == DBNumberProgID ||
				strProgID == DBVTObjectProgID ||
				strProgID == DBTextProgID
				)
			{
				CLSID	clsid;		
				if( ::CLSIDFromProgID( _bstr_t((LPCTSTR)strProgID), &clsid ) == S_OK )
				{
					IUnknown* punkDBControl = NULL;
					HRESULT hr = CoCreateInstance(clsid,
						NULL, CLSCTX_INPROC_SERVER,
						IID_IUnknown, (LPVOID*)&punkDBControl);
					if( hr == S_OK)
					{
						sptrIDBControl spDBCtrl( punkDBControl );
						if( spDBCtrl )
						{	
							::RestoreContainerFromDataObject( sptrAXCtrl, spDBCtrl ); 
							BSTR bstrTable, bstrField;
							spDBCtrl->GetTableName( &bstrTable );
							spDBCtrl->GetFieldName( &bstrField );
							CString strTableCtrl = bstrTable;
							CString strFieldCtrl = bstrField;
							::SysFreeString( bstrTable );
							::SysFreeString( bstrField );							
							if( strTableCtrl == strTable &&
								strFieldCtrl == strField
								)
								bNeedDeleteCtrl = true;

						}
						punkDBControl->Release();			
					}
				}
			}
		}
		if( bNeedDeleteCtrl )
		{
			sptrINamedDataObject2	sptrN( punk );
			if( sptrN )
			{
				sptrINamedDataObject2	sptrN( punk );
				sptrN->SetParent( 0, 0 );
			}
			else
				ASSERT( FALSE );
		}
		punk->Release();
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::AddFieldToReport( CString strTable, CString strField, 
									  CRect& rcCtrl, IUnknown* pUnkReport, 
									  bool bNewType, int& nCurRow, int nCurCol, int nMainPage )
{
	sptrIReportForm spReport( pUnkReport );
	if( spReport == NULL )
		return false;


	sptrIDBaseDocument spDBDoc( GetControllingUnknown() );
	if( spDBDoc == NULL )
		return false;

	FieldType fieldType = ::GetFieldType( spDBDoc, strTable, strField );

	if( !(fieldType == ftDigit ||
		fieldType == ftDateTime ||
		fieldType == ftString || 
		fieldType == ftVTObject)
		)
		return false;

	IUnknown* punkCtrl = NULL;
	punkCtrl = ::CreateTypedObject( szTypeReportCtrl );
	if( punkCtrl == NULL )
		return false;

	sptrIReportCtrl	spCtrl( punkCtrl );
	punkCtrl->Release();
	if( spCtrl == NULL )
		return false;

	CSize ctrlSize = CSize( CTRL_WIDTH_TEXT_MM, CTRL_HEIGHT_TEXT_MM );
	_bstr_t bstrProgID;
	if( fieldType == ftDigit )
	{
		bstrProgID = DBNumberProgID;
	}
	else
	if( fieldType == ftDateTime )
	{
		bstrProgID = DBDateTimeProgID;
	}
	else
	if( fieldType == ftString )
	{
		bstrProgID = DBTextProgID;
	}
	else
	if( fieldType == ftVTObject )
	{

		bstrProgID = DBVTObjectProgID;//szViewAXProgID
		ctrlSize = CSize( CTRL_WIDTH_IMAGE_MM, CTRL_HEIGHT_IMAGE_MM );
	}
	else
		return false;


	sptrIActiveXCtrl spAXCtrl( spCtrl );
	if( spAXCtrl != NULL )
	{
		spAXCtrl->SetProgID( bstrProgID );
		BSTR newName;
		spAXCtrl->GenerateUniqueName( _bstr_t((LPCTSTR)strField), &newName );
		spAXCtrl->SetName( newName );
		::SysFreeString( newName );		
	}


	CRect rcFields;
	spReport->GetPaperField( &rcFields );

	CSize pageSize;
	spReport->GetPaperSize( &pageSize );

	
	CRect rcPrev = rcCtrl;

	if( bNewType )
	{
		rcCtrl.left		= rcFields.left + CTRL_MARGINS_MM;
		rcCtrl.right	= rcCtrl.left + ctrlSize.cx;		
		rcCtrl.top		= rcPrev.bottom + CTRL_MARGINS_MM;		
	}
	else
	{
		if( rcCtrl.left + ctrlSize.cx > pageSize.cx - rcFields.right )
		{
			rcCtrl.left		= rcFields.left + CTRL_MARGINS_MM;
			rcCtrl.right	= rcCtrl.left + ctrlSize.cx;
			rcCtrl.top		= rcPrev.bottom + CTRL_MARGINS_MM;			
		}
	}

	rcCtrl.bottom = rcCtrl.top + ctrlSize.cy;

	if( rcCtrl.bottom > pageSize.cy - rcFields.bottom )
	{
		nCurRow++;
		rcCtrl.left		= rcFields.left + CTRL_MARGINS_MM;
		rcCtrl.right	= rcCtrl.left + ctrlSize.cx;		
		rcCtrl.top		= rcFields.top + CTRL_MARGINS_MM;		
	}	

	rcCtrl.bottom = rcCtrl.top + ctrlSize.cy;


	spCtrl->SetDesignColRow( nCurCol, nCurRow );
	spCtrl->SetDesignOwnerPage( nMainPage );
	spCtrl->SetDesignPosition( rcCtrl );


	sptrINamedDataObject2 sptrNDO2( spCtrl );
	if( sptrNDO2 == NULL )
		return false;
	
	sptrNDO2->SetParent( spReport, 0 );			


	CLSID	clsid;		
	if( ::CLSIDFromProgID( bstrProgID, &clsid ) == S_OK )
	{
		IUnknown* punkDBControl = NULL;
		HRESULT hr = CoCreateInstance(clsid,
			NULL, CLSCTX_INPROC_SERVER,
			IID_IUnknown, (LPVOID*)&punkDBControl);
		if( hr == S_OK)
		{
			sptrIDBControl spDBCtrl( punkDBControl );
			if( spDBCtrl )
			{	
				spDBCtrl->SetTableName( _bstr_t( (LPCTSTR)strTable ) );
				spDBCtrl->SetFieldName( _bstr_t( (LPCTSTR)strField ) );

				if( !strcmp(((LPCTSTR)bstrProgID), DBVTObjectProgID) )
				{
					spDBCtrl->SetAutoLabel( FALSE );
				}

				::StoreContainerToDataObject( spCtrl, spDBCtrl ); 
			}
			punkDBControl->Release();			
		}
	}

	CRect rcTemp = rcCtrl;

	rcCtrl.left = rcTemp.right + CTRL_MARGINS_MM;
	rcCtrl.right = rcCtrl.left + rcTemp.Width();	


	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::RemoveFieldFromReport( CString strTable, CString strField, IUnknown* pUnkReport )
{
	return RemoveFieldFromAXForm( strTable, strField, pUnkReport );	
}


////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::GetLastRectFromAXForm( IUnknown* pUnkForm, CRect& rectCtrl )
{
	sptrIActiveXForm spAXFrom( pUnkForm );
	if( spAXFrom == NULL )
		return false;
	CObjectListWrp	list( spAXFrom );
	POSITION	pos = list.GetFirstObjectPosition();	

	//rectCtrl = CRect( 0, 0, 0, 0 );

	while( pos )
	{			
		bool bNeedDeleteCtrl = false;

		IUnknown *punk = list.GetNextObject( pos );
		sptrIActiveXCtrl sptrAXCtrl( punk );
		if( sptrAXCtrl )
		{
			CRect rcAXCtrl;
			sptrAXCtrl->GetRect( rcAXCtrl );
			if( rcAXCtrl.bottom > rectCtrl.bottom )
				rectCtrl = rcAXCtrl;
		}
		punk->Release();
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::GetLastRectFromReport( IUnknown* pUnkReport, CRect& rectCtrl, 
										   int& nRow, int& nCol, int& nMainPage )
{
	sptrIReportForm spReport( pUnkReport );
	if( spReport == NULL )
		return false;

	CObjectListWrp	list( spReport );
	POSITION	pos = list.GetFirstObjectPosition();	

	//rectCtrl = CRect( 0, 0, 0, 0 );

	while( pos )
	{			
		bool bNeedDeleteCtrl = false;

		IUnknown *punk = list.GetNextObject( pos );
		sptrIReportCtrl sptrReportCtrl( punk );
		if( sptrReportCtrl )
		{
			int nRowNew, nColNew, nMainPageNew;
			CRect rcNew;
			sptrReportCtrl->GetDesignOwnerPage( &nMainPageNew );
			sptrReportCtrl->GetDesignColRow( &nColNew, &nRowNew );
			sptrReportCtrl->GetDesignPosition( &rcNew );
			if( nMainPageNew >= nMainPage )
			{
				if( nRowNew >= nRowNew )
				{
					if( rcNew.bottom > rectCtrl.bottom )
						rectCtrl = rcNew;
				}
			}
		}
		punk->Release();
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::GetDBaseFields( CArray<CFieldBase, CFieldBase>& arrFields )
{
	arrFields.RemoveAll();

	// need add new Query and then add new form && report.
	if( !m_dbEngine.IsSessionOK() )
		return false;

	CArray<_CTableInfo*, _CTableInfo*>* pArrTableInfo = NULL;
	if( !m_dbEngine.GetTablesInfo( &pArrTableInfo, false ) )
		return false;

	for( int i=0;i<pArrTableInfo->GetSize();i++ )
	{
		_CTableInfo* pTI = (*pArrTableInfo)[i];
		for( int j=0;j<pTI->m_arrFieldInfo.GetSize();j++ )
		{
			_CFieldInfo* pFI = pTI->m_arrFieldInfo[j];
			CFieldBase fieldBase;
			fieldBase.m_strTable = pTI->m_strTableName;
			fieldBase.m_strField = pFI->m_strFieldName;
			arrFields.Add( fieldBase );
		}			
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::GetAXFormFields( IUnknown* pUnkForm, 
									 CArray<CFieldBase, CFieldBase>& arrFields )
{
	arrFields.RemoveAll();
	sptrIActiveXForm spAXFrom( pUnkForm );
	if( spAXFrom == NULL )
		return false;

	sptrIDBaseDocument spDBDoc( GetControllingUnknown() );
	if( spDBDoc == NULL )
		return false;

	CObjectListWrp	list( spAXFrom );
	POSITION	pos = list.GetFirstObjectPosition();	

	while( pos )
	{		

		IUnknown *punk = list.GetNextObject( pos );
		sptrIActiveXCtrl sptrAXCtrl( punk );
		if( sptrAXCtrl )
		{
			BSTR bstrProgID;
			sptrAXCtrl->GetProgID(&bstrProgID);
			CString strProgID = bstrProgID;
			::SysFreeString(bstrProgID);

			if( strProgID == DBDateTimeProgID ||
				strProgID == DBNumberProgID ||
				strProgID == DBVTObjectProgID ||
				strProgID == DBTextProgID
				)
			{
				CLSID	clsid;		
				if( ::CLSIDFromProgID( _bstr_t((LPCTSTR)strProgID), &clsid ) == S_OK )
				{
					IUnknown* punkDBControl = NULL;
					HRESULT hr = CoCreateInstance(clsid,
						NULL, CLSCTX_INPROC_SERVER,
						IID_IUnknown, (LPVOID*)&punkDBControl);
					if( hr == S_OK)
					{
						sptrIDBControl spDBCtrl( punkDBControl );
						if( spDBCtrl )
						{	
							::RestoreContainerFromDataObject( sptrAXCtrl, spDBCtrl ); 
							BSTR bstrTable, bstrField;
							spDBCtrl->GetTableName( &bstrTable );
							spDBCtrl->GetFieldName( &bstrField );
							CString strTableCtrl = bstrTable;
							CString strFieldCtrl = bstrField;
							::SysFreeString( bstrTable );
							::SysFreeString( bstrField );							

							CFieldBase fieldBase;
							fieldBase.m_strTable = strTableCtrl;
							fieldBase.m_strField = strFieldCtrl;
							arrFields.Add( fieldBase );
						}
						punkDBControl->Release();			
					}
				}
			}
		}
		punk->Release();
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::GetReportFields( IUnknown* pUnkRepot, 
									 CArray<CFieldBase, CFieldBase>& arrFields )
{	
	return GetAXFormFields( pUnkRepot, arrFields );
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::GetQueryFields( IUnknown* pUnkQuery, 
									CArray<CFieldBase, CFieldBase>& arrFields )
{
	arrFields.RemoveAll();

	sptrIQueryObject spQuery( pUnkQuery );	

	if( spQuery == NULL )
		return false;

	int nFieldCount = 0;
	spQuery->GetFieldsCount( &nFieldCount );


	for( int i=0;i<nFieldCount;i++)
	{
		BSTR bstrTable, bstrField;
		spQuery->GetField( i, &bstrTable, &bstrField );
		CString strTable, strField;
		strTable = bstrTable;
		strField = bstrField;
		::SysFreeString( bstrTable );
		::SysFreeString( bstrField );
		
		CFieldBase fieldBase;
		fieldBase.m_strTable = strTable;
		fieldBase.m_strField = strField;
		arrFields.Add( fieldBase );
	}
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::GetAddFieldsFrom2Array( CArray<CFieldBase, CFieldBase>& arrFieldsBase,
								 CArray<CFieldBase, CFieldBase>& arrFieldsCompare,
								 CArray<CFieldBase, CFieldBase>& arrFieldsResult
								 )
{
	//Determine fields to add
	for( int i=0;i<arrFieldsBase.GetSize();i++ )			
	{			
		bool bFound = false;

		for( int j=0;j<arrFieldsCompare.GetSize();j++ )
		{			
			if( arrFieldsBase[i].m_strTable == arrFieldsCompare[j].m_strTable &&
				arrFieldsBase[i].m_strField == arrFieldsCompare[j].m_strField
				)
				bFound = true;
		}

		if( !bFound )
		{
			CFieldBase fieldBase;
			fieldBase.m_strTable = arrFieldsBase[i].m_strTable;
			fieldBase.m_strField = arrFieldsBase[i].m_strField;
			arrFieldsResult.Add( fieldBase );
		}			
	}


	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::GetRemoveFieldsFrom2Array( CArray<CFieldBase, CFieldBase>& arrFieldsBase,
								 CArray<CFieldBase, CFieldBase>& arrFieldsCompare,
								 CArray<CFieldBase, CFieldBase>& arrFieldsResult
								 )
{
	//Determine fields to remove
	for( int i=0;i<arrFieldsCompare.GetSize();i++ )
	{			
		bool bFound = false;

		for( int j=0;j<arrFieldsBase.GetSize();j++ )		
		{			
			if( arrFieldsBase[j].m_strTable == arrFieldsCompare[i].m_strTable &&
				arrFieldsBase[j].m_strField == arrFieldsCompare[i].m_strField
				)
				bFound = true;
		}

		if( !bFound )
		{
			CFieldBase fieldBase;
			fieldBase.m_strTable = arrFieldsCompare[i].m_strTable;
			fieldBase.m_strField = arrFieldsCompare[i].m_strField;
			arrFieldsResult.Add( fieldBase );
		}			
	}

	return true;
}
*/