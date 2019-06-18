// TemplateSetup.cpp : implementation file
//

#include "stdafx.h"
#include "print.h"
#include "TemplateSetup.h"
#include "afxpriv2.h"
#include "PrinterSetup.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTemplateSetup dialog


CTemplateSetup::CTemplateSetup(CWnd* pParent )
	: CDialog(CTemplateSetup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTemplateSetup)
	m_nSourceTemplate = 0; //0 - this doc template, 1 - Use file exist template, 2 - use new
	m_bStore = FALSE;		//Save new template or not
	m_nStoreTarget = 2;	//0-To active doc, 1-to spec. doc,2-to file	
	m_nPrintContext = 1; //0-All Data Object 1 - Active View
	m_strNewTemplateName = _T("");
	//}}AFX_DATA_INIT
}


void CTemplateSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateSetup)
	DDX_Control(pDX, IDC_EXIST_TEMPLATE, m_ExistCtrl);
	DDX_Control(pDX, IDC_EXIST_DOC_TEMPLATE, m_ExistDocCtrl);
	DDX_Control(pDX, IDC_BASED_ON_TEMPLATE, m_BasedOnCtrl);
	DDX_Radio(pDX, IDC_SOURCE_TEMPLATE, m_nSourceTemplate);	
	DDX_Check(pDX, IDC_STORE, m_bStore);
	DDX_Radio(pDX, IDC_STORE_TARGET, m_nStoreTarget);
	DDX_Radio(pDX, IDC_ALL_DATA_OBJECT, m_nPrintContext);	
	DDX_Text(pDX, IDC_NEW_TEMPL_NAME, m_strNewTemplateName);
	DDV_MaxChars(pDX, m_strNewTemplateName, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateSetup, CDialog)
	//{{AFX_MSG_MAP(CTemplateSetup)
	ON_BN_CLICKED(IDC_SOURCE_TEMPLATE, OnSourceTemplate)
	ON_BN_CLICKED(IDC_SOURCE_TEMPLATE1, OnSourceTemplate1)
	ON_BN_CLICKED(IDC_SOURCE_TEMPLATE2, OnSourceTemplate2)
	ON_BN_CLICKED(IDC_ALL_DATA_OBJECT, OnAllDataObject)
	ON_BN_CLICKED(IDC_ALL_DATA_OBJECT2, OnAllDataObject2)
	ON_BN_CLICKED(IDC_STORE, OnStore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateSetup message handlers

//Helper
//Get String
CString _GetValueString( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszDefault )
{
	try
	{
		_variant_t	var( pszDefault );
		var = ::GetValue( punkDoc, pszSection, pszEntry, var );

		
		_VarChangeType( var, VT_BSTR );

		return CString( var.bstrVal );
	}
	catch( ... )
	{
		return CString( pszDefault );
	}
}

long _GetValueInt( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lDefault )
{
	try
	{
		_variant_t	var( lDefault );
		var = ::GetValue( punkDoc, pszSection, pszEntry, var );

		_VarChangeType( var, VT_I4 );

		return var.lVal;
	}
	catch( ... )
	{
		return lDefault;
	}
}



BOOL CTemplateSetup::ReadData()
{
	if( m_pDocSite == NULL )
		return FALSE;


	//_GetValueString( m_pDocSite, REPORT_TEMPLATE_SECTION, "", );
	m_nSourceTemplate	= _GetValueInt( m_pDocSite, REPORT_TEMPLATE_SECTION, 
							"nSourceTemplate", (int) tsFile );

	m_bStore			= _GetValueInt( m_pDocSite, REPORT_TEMPLATE_SECTION, 
							"bStore",  FALSE );	

	m_nStoreTarget		= _GetValueInt( m_pDocSite, REPORT_TEMPLATE_SECTION, 
							"nStoreTarget",  (int)stFile );	

	m_nPrintContext		= _GetValueInt( m_pDocSite, REPORT_TEMPLATE_SECTION, 
							"nPrintContext",  (int)pcActiveView );	

	m_strTemplateName	= _GetValueString( m_pDocSite, REPORT_TEMPLATE_SECTION, 
							"TemplateName", "default.rpt" );
	return TRUE;
}

BOOL CTemplateSetup::WriteData()
{
	if( m_pDocSite == NULL )
		return FALSE;

	::SetValue( m_pDocSite, REPORT_TEMPLATE_SECTION, "nSourceTemplate",  (long)m_nSourceTemplate );	
	::SetValue( m_pDocSite, REPORT_TEMPLATE_SECTION, "bStore",  (long)m_bStore );	
	::SetValue( m_pDocSite, REPORT_TEMPLATE_SECTION, "nStoreTarget",  (long)m_nStoreTarget );	
	::SetValue( m_pDocSite, REPORT_TEMPLATE_SECTION, "nPrintContext",  (long)m_nPrintContext );	
	::SetValue( m_pDocSite, REPORT_TEMPLATE_SECTION, "TemplateName", m_strTemplateName );

	return TRUE;
}



BOOL CTemplateSetup::OnInitDialog() 
{
	CDialog::OnInitDialog();	

	ReadData();

	UpdateData( FALSE );

	FillComboBoxes();	
	

	if( m_nSourceTemplate == tsDocument )
		m_ExistDocCtrl.SelectString( 0, m_strTemplateName );

	if( m_nSourceTemplate == tsFile ) 
		m_ExistCtrl.SelectString( 0, m_strTemplateName );
	
	if( m_nSourceTemplate == tsNew ) 
		m_BasedOnCtrl.SelectString( 0, m_strTemplateName );

	UpdateControls();

	return TRUE;	            
}

void CTemplateSetup::OnOK() 
{	
	UpdateData( TRUE );		

	

	if( m_nSourceTemplate == tsDocument )
		m_ExistDocCtrl.GetWindowText( m_strTemplateName );

	if( m_nSourceTemplate == tsFile )
		m_ExistCtrl.GetWindowText( m_strTemplateName );
	
	if( m_nSourceTemplate == tsNew )
	{	
		m_BasedOnCtrl.GetWindowText( m_strTemplateName );

		CString strBaseTempl    = m_strTemplateName;
		CString strNewTemplName = m_strNewTemplateName;

		if( m_strTemplateName.IsEmpty() || strNewTemplName.IsEmpty() )
		{
			AfxMessageBox("Please input 'New template based on' and 'New template name'.");
			return;
		}

		if( !m_bStore )
			m_strNewTemplateName = "temp";
		
		
		if( !GenerateNewTemplate( m_strTemplateName, m_strNewTemplateName,
					m_bStore, (PrintContext)m_nPrintContext, (StoreTarget)m_nStoreTarget) )
		{
			AfxMessageBox("Can't create new template.");
			return;
		}
		

		if( m_nStoreTarget == stActiveDocument )
			m_nSourceTemplate = tsDocument;

		if( m_nStoreTarget == stFile )
			m_nSourceTemplate = tsFile;
		
		m_strTemplateName = m_strNewTemplateName;			


		m_bStore = FALSE;
		
	}

	WriteData();	
	CDialog::OnOK();
}

void CTemplateSetup::FillComboBoxes()
{
	m_ExistDocCtrl.ResetContent();
	m_ExistCtrl.ResetContent();
	m_BasedOnCtrl.ResetContent();


	if( m_pDocSite == NULL )
		return;




	sptrIApplication sptrA( GetAppUnknown( ) );
	if( sptrA != NULL )
	{			

		long	lPosTemplate = 0;
		
		sptrA->GetFirstDocTemplPosition( &lPosTemplate );
		
		while( lPosTemplate )
		{
			long	lPosDoc = 0;

			sptrA->GetFirstDocPosition( lPosTemplate, &lPosDoc );

			while( lPosDoc )
			{
				IUnknown *punkDoc = 0;
				sptrA->GetNextDoc( lPosTemplate, &lPosDoc, &punkDoc );

				ASSERT( punkDoc );						
				

				sptrIDocumentSite sptrDocSite( punkDoc );

				if( sptrDocSite != NULL )
				{



					//Get template from this documet
					IUnknown	*punkV = 0;
					sptrDocSite->GetActiveView( &punkV );
					sptrIView	sptrV( punkV );
					sptrIDataContext2	sptrC( punkV );

					if( punkV )
						punkV->Release();
					else
						return;

					if( sptrV == NULL || sptrC == NULL )
						return;

					long nCount = -1;	

					_bstr_t bstrType(szTypeReportForm);
					sptrC->GetChildrenCount( bstrType, NULL, &nCount );

					if (nCount)
					{
						long lPos = 0;
						sptrC->GetFirstChildPos(bstrType, 0, &lPos);
						while (lPos)
						{
							IUnknownPtr sptrObj;
							if (FAILED(sptrC->GetNextChild(bstrType, 0, &lPos, &sptrObj)) || sptrObj == 0)
								continue;

							CString str = ::GetObjectName(sptrObj);
							m_ExistDocCtrl.AddString( str );
						}
					}

				}				

				punkDoc->Release();
			}

			sptrA->GetNextDocTempl( &lPosTemplate, 0, 0 );
		} 
	
	
	}









	//Fill file based report
	CString strDir = GetReportDir();
	CString strMask = strDir + "*.rpt";

	CFileFind ff;

	BOOL bFound = ff.FindFile( strMask );

	while( bFound )
	{
		bFound = ff.FindNextFile();				
		m_ExistCtrl.AddString( ff.GetFileTitle() );	
		m_BasedOnCtrl.AddString( ff.GetFileTitle() );		
	}
}



BOOL  CTemplateSetup::GenerateNewTemplate( CString strBaseTempl, 
							CString strNewTemplName,
							BOOL bStore,
							PrintContext pc,
							StoreTarget st
							)
{
	if( m_pDocSite == NULL )
		return FALSE;


	CString strDir = GetReportDir();
	CString strFileName = strDir +  strBaseTempl + ".rpt";

	INamedDataPtr	ptrNamedData( m_pDocSite );

	
	IUnknown *punk = ::CreateTypedObject( szTypeReportForm );
		
	// get object name from path and check it's exists
	CString	strObjName = strNewTemplName;//::GetObjectNameFromPath(strFileName);
	_bstr_t bstrName(strObjName);
	long	NameExists = 0;
	
	if(!CheckInterface(punk, IID_ISerializableObject))
		return FALSE;
	
	CFile	file( strFileName, CFile::modeRead );
		
	sptrISerializableObject	sptrO( punk );			
	if( sptrO == NULL )
		return FALSE;
			
	CArchive		ar( &file, CArchive::load );
	CArchiveStream	ars( &ar );				

	SerializeParams	params;
	ZeroMemory( &params, sizeof( params ) );

	if( sptrO->Load( &ars, &params ) != S_OK)
	{
		CString strError;
		strError.Format( "Can't load report template. File name:%s", strFileName );
		AfxMessageBox( strError );
		return FALSE;
	}



	IUnknown* punkNewReport = NULL;
	punkNewReport = ::CloneObject( punk );

	if( punkNewReport == NULL )
		return FALSE;

	punk->Release();
	punk = 0;


	/////////////////////////////////
	//
	//	Work here with data 
	//
	//
	/////////////////////////////////
	
	CRect rcPrnMargins = g_prnSetup.GetPrinterMargins( punkNewReport );

	rcPrnMargins.left   = FROM_DOUBLE_TO_VTPIXEL( rcPrnMargins.left );
	rcPrnMargins.top    = FROM_DOUBLE_TO_VTPIXEL( rcPrnMargins.top );
	rcPrnMargins.right  = FROM_DOUBLE_TO_VTPIXEL( rcPrnMargins.right );
	rcPrnMargins.bottom = FROM_DOUBLE_TO_VTPIXEL( rcPrnMargins.bottom );
	

	sptrIReportForm sptrForm( punkNewReport ); 
	if( sptrForm )
	{
		CSize paperSize;
		CRect paperField;
		CRect paperAlign;
		

		ISOK( sptrForm->GetPaperSize( &paperSize ) );
		ISOK( sptrForm->GetPaperAlign( &paperAlign ) );
		ISOK( sptrForm->GetPaperField( &paperField ) );
		
		CRect rcField = paperField;
		CRect rcAlign = paperAlign;

		rcField.right  = paperSize.cx - rcField.right;
		rcField.bottom = paperSize.cy - rcField.bottom;

		rcAlign.right  = paperSize.cx - rcAlign.right;
		rcAlign.bottom = paperSize.cy - rcAlign.bottom;

		CObjectListWrp	list( sptrForm );

		POSITION	pos = list.GetFirstObjectPosition();

		while( pos )
		{			
			IUnknown *punk = list.GetNextObject( pos );
			sptrIReportCtrl sptrRepCtrl( punk );
			if( sptrRepCtrl )
			{
				ISOK( sptrRepCtrl->SetDesignColRow(0,0) );
				ISOK( sptrRepCtrl->SetDesignOwnerPage( 0 ) );				

				CRect rcDesign;
				ISOK( sptrRepCtrl->GetDesignPosition( &rcDesign) );
				CPoint ptDesign( rcDesign.TopLeft() );
				if( rcField.PtInRect( ptDesign )  )
				{					
					sptrINamedDataObject2 sptrNDO2Ctrl(sptrRepCtrl);
					if( sptrNDO2Ctrl )
						ISOK( sptrNDO2Ctrl->SetParent( 0, 0 ) );
				}

			}

			punk->Release();
		}

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
				sptrVAX = punkVAX;				
			
			if( punkVAX != NULL )  
				punkVAX->Release();

		}
		else
		{
			if( punkNewReport )
				punkNewReport->Release();

			return FALSE;
		}
		


		ISOK( sptrForm->SetPaperAlign( rcPrnMargins ) );

		int nCurRow		= 0;
		int nCurX		= rcField.left + FROM_DOUBLE_TO_VTPIXEL(5);
		int nCurY		= rcField.top + FROM_DOUBLE_TO_VTPIXEL(5);
		int nMaxY		= rcField.bottom;
		CSize sizeObj	= CSize( rcField.Width() - FROM_DOUBLE_TO_VTPIXEL(10), 
							FROM_DOUBLE_TO_VTPIXEL(10) );
		int nDeltaY		= FROM_DOUBLE_TO_VTPIXEL(15);

		//now create objects
		if( pc == pcActiveView )
		{
			IUnknown* punkCtrl = NULL;
			punkCtrl = ::CreateTypedObject( szTypeReportCtrl );
			if( punkCtrl )
			{
				sptrIActiveXCtrl	sptrC( punkCtrl );
				if( sptrC )
					sptrC->SetProgID( bstrViewAXProgID );

				sptrIReportCtrl sptrRC( punkCtrl );
				if( sptrRC )
				{

					CRect rcPos;
					rcPos.left = nCurX;
					rcPos.top  = nCurY;
					rcPos.right  = nCurX + sizeObj.cx;
					rcPos.bottom = nCurY + sizeObj.cy;										

					sptrRC->SetDesignPosition( rcPos );

					sptrRC->SetDesignColRow(0,nCurRow);
					sptrRC->SetDesignOwnerPage( 0 );

				}

				//Save
				::StoreContainerToDataObject( punkCtrl, sptrVAX ); 

				sptrINamedDataObject2 sptrNDO2(punkCtrl);
				if( sptrNDO2 )
					sptrNDO2->SetParent( sptrForm, 0 );
				

				punkCtrl->Release();
			}
			
		}
		else
		{

			IUnknown	*punkV = 0;
			m_pDocSite->GetActiveView( &punkV );
			sptrIView	sptrV( punkV );
			sptrIDataContext2	sptrC( punkV );

			if( punkV )
				punkV->Release();

			if( sptrV != NULL && sptrC != NULL )
			{
				long nTypesCount = 0;
				ISOK( sptrC->GetObjectTypeCount( &nTypesCount ) );
				for( long nType = 0; nType < nTypesCount; nType++ )
				{
					BSTR	bstrType = 0;		

					long nObjCountOfType = 0;		

					ISOK( sptrC->GetObjectTypeName( nType, &bstrType ) );
					ISOK( sptrC->GetChildrenCount( bstrType, NULL, &nObjCountOfType ) );

					if (nObjCountOfType)
					{
						long lPos = 0;
						sptrC->GetFirstChildPos(bstrType, 0, &lPos);
						while (lPos)
						{
							IUnknownPtr sptrObj;
							if (FAILED(sptrC->GetNextChild(bstrType, 0, &lPos, &sptrObj)) || sptrObj == 0)
								continue;

							CString strObjName = ::GetObjectName(sptrObj);
							

							if( sptrVAX )
							{




								//6.10.200 Cos need build
								/*
								_bstr_t bstrObjName( strObjName );
								ISOK( sptrVAX->SetObject( bstrObjName ) );
								ISOK( sptrVAX->SetObjectDefinition( (int)odFixedObject ) );

								IUnknown* punkCtrl = NULL;
								punkCtrl = ::CreateTypedObject( szTypeReportCtrl );
								if( punkCtrl )
								{
									sptrIActiveXCtrl	sptrC( punkCtrl );
									if( sptrC )
										sptrC->SetProgID( bstrViewAXProgID );

									sptrIReportCtrl sptrRC( punkCtrl );
									if( sptrRC )
									{

										CRect rcPos;
										rcPos.left = nCurX;
										rcPos.top  = nCurY;
										rcPos.right  = nCurX + sizeObj.cx;
										rcPos.bottom = nCurY + sizeObj.cy;										

										sptrRC->SetDesignPosition( rcPos );

										sptrRC->SetDesignColRow(0,nCurRow);
										sptrRC->SetDesignOwnerPage( 0 );

										nCurY += nDeltaY;
										if(  nCurY >= nMaxY  )
										{
											nCurY = rcField.top;
											nCurRow++;
										}

									}

									//Save
									::StoreContainerToDataObject( punkCtrl, sptrVAX ); 

									sptrINamedDataObject2 sptrNDO2(punkCtrl);
									if( sptrNDO2 )
										sptrNDO2->SetParent( sptrForm, 0 );									
									

									punkCtrl->Release();
								}
								*/

							}
						
						}									
					}

					::SysFreeString( bstrType );
				}


			}

		}


	}




	/////////////////////////////////

	if( st == stActiveDocument )
	{
		{	//Delete old report
			IUnknown* punkOldReport;
			punkOldReport = ::GetObjectByName( m_pDocSite, strNewTemplName, szTypeReportForm );
			if( punkOldReport != NULL )
			{
				::DeleteObject( m_pDocSite, punkOldReport );
				punkOldReport->Release();
			}
		}

		_bstr_t bstrName( strNewTemplName );
		INamedObject2Ptr ptrNO( punkNewReport );
		if( ptrNO )
		{
			ptrNO->SetName( bstrName );
			ptrNO->SetUserName( bstrName );
		}							

		//Cos Context change 28.08.2000
		//INumeredObjectPtr	ptrN( punkNewReport );
		//ptrN->GenerateNewKey();

		::SetValue( ptrNamedData, 0, 0, _variant_t( punkNewReport ) );

	}
	
	if( st == stFile || !bStore )
	{
		CString strFileName = GetReportDir() + strNewTemplName + ".rpt";				

		try
		{
			
			_bstr_t bstrName( strNewTemplName );
			INamedObject2Ptr ptrNO( punkNewReport );
			if( ptrNO )
			{
				ptrNO->SetName( bstrName );
				ptrNO->SetUserName( bstrName );
			}								


			CFile	file( strFileName, CFile::modeCreate|CFile::modeWrite );
			sptrISerializableObject	sptrO( punkNewReport );
			{
				SerializeParams	params;
				ZeroMemory( &params, sizeof( params ) );

				CArchive		ar( &file, CArchive::store );
				CArchiveStream	ars( &ar );
				sptrO->Store( &ars, &params );
			}
		}
		catch( CFileException* ex)
		{
			ex->ReportError();
			ex->Delete();							
		}
	}

	punkNewReport->Release();

	return TRUE;
}



void CTemplateSetup::OnSourceTemplate(){
	UpdateControls();		
}

void CTemplateSetup::OnSourceTemplate1(){
	UpdateControls();		
}

void CTemplateSetup::OnSourceTemplate2(){
	UpdateControls();		
}

void CTemplateSetup::OnAllDataObject(){
	UpdateControls();		
}

void CTemplateSetup::OnAllDataObject2(){
	UpdateControls();		
}

void CTemplateSetup::OnStore(){
	UpdateControls();		
}

void CTemplateSetup::UpdateControls()
{
	UpdateData( TRUE );

	if( (TemplateSource1)m_nSourceTemplate == tsDocument )
	{
			GetDlgItem(IDC_EXIST_DOC_TEMPLATE)->EnableWindow( TRUE );			
			GetDlgItem(IDC_EXIST_TEMPLATE)->EnableWindow( FALSE );
			GetDlgItem(IDC_BASED_ON_TEMPLATE)->EnableWindow( FALSE );

			GetDlgItem(IDC_ALL_DATA_OBJECT)->EnableWindow( FALSE );
			GetDlgItem(IDC_ALL_DATA_OBJECT2)->EnableWindow( FALSE );

			GetDlgItem(IDC_STORE)->EnableWindow( FALSE );

			GetDlgItem(IDC_STORE_TARGET)->EnableWindow( FALSE );
			GetDlgItem(IDC_STORE_TARGET2)->EnableWindow( FALSE );

			GetDlgItem(IDC_NEW_TEMPL_NAME)->EnableWindow( FALSE );			
	}

	if( (TemplateSource1)m_nSourceTemplate == tsFile )
	{
			GetDlgItem(IDC_EXIST_DOC_TEMPLATE)->EnableWindow( FALSE );			
			GetDlgItem(IDC_EXIST_TEMPLATE)->EnableWindow( TRUE );
			GetDlgItem(IDC_BASED_ON_TEMPLATE)->EnableWindow( FALSE );

			GetDlgItem(IDC_ALL_DATA_OBJECT)->EnableWindow( FALSE );
			GetDlgItem(IDC_ALL_DATA_OBJECT2)->EnableWindow( FALSE );

			GetDlgItem(IDC_STORE)->EnableWindow( FALSE );

			GetDlgItem(IDC_STORE_TARGET)->EnableWindow( FALSE );
			GetDlgItem(IDC_STORE_TARGET2)->EnableWindow( FALSE );

			GetDlgItem(IDC_NEW_TEMPL_NAME)->EnableWindow( FALSE );

	}

	if( (TemplateSource1)m_nSourceTemplate == tsNew )
	{
			GetDlgItem(IDC_EXIST_DOC_TEMPLATE)->EnableWindow( FALSE );			
			GetDlgItem(IDC_EXIST_TEMPLATE)->EnableWindow( FALSE );
			GetDlgItem(IDC_BASED_ON_TEMPLATE)->EnableWindow( TRUE );

			GetDlgItem(IDC_ALL_DATA_OBJECT)->EnableWindow( TRUE );
			GetDlgItem(IDC_ALL_DATA_OBJECT2)->EnableWindow( TRUE );

			GetDlgItem(IDC_STORE)->EnableWindow( TRUE );

			if( !m_bStore)
			{
				GetDlgItem(IDC_STORE_TARGET)->EnableWindow( FALSE );
				GetDlgItem(IDC_STORE_TARGET2)->EnableWindow( FALSE );
				GetDlgItem(IDC_NEW_TEMPL_NAME)->EnableWindow( FALSE );			
			}
			else
			{
				GetDlgItem(IDC_STORE_TARGET)->EnableWindow( TRUE );
				GetDlgItem(IDC_STORE_TARGET2)->EnableWindow( TRUE );
				GetDlgItem(IDC_NEW_TEMPL_NAME)->EnableWindow( TRUE );			
			}

	}

}
