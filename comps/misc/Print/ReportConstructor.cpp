// ReportConstructor.cpp: implementation of the CReportConstructor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "obj_types.h"
#include "print.h"
#include "ReportConstructor.h"
#include "afxpriv2.h"
#include "PrinterSetup.h"
#include "thumbnail.h"
#include "PrintView.h"
#include "..\\..\\..\\common2\\misc_calibr.h"

#include "misc_utils.h"

extern CReportConstructor g_ReportConstructor;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


BOOL GenerateAutoReport(int autoMode)
{
	IUnknown* pUnk =GetAppUnknown();
	if(!pUnk) return 0;
	
	IApplication* pApp;
	IUnknown* docUnk;

	HRESULT hr =  pUnk->QueryInterface(&pApp);

	if(FAILED(hr)) return 0;

	hr = pApp->GetActiveDocument(&docUnk);
	pApp->Release();pApp=0;
	if(!docUnk) return 0;
	CReportConstructor rc;
	rc.SetAutoTemplateAlghoritm((AutoTemplateAlghoritm)autoMode);
	

	IUnknown* unkRf = GetActiveObjectFromDocument(docUnk, szTypeReportForm);
	//INamedDataObject2* nd2; /*nd2->SetParent*/
	if(!unkRf) {docUnk->Release(); return 0;}
	INamedDataObject2Ptr nd = unkRf;
	

	
	long childs;
	nd->GetChildsCount(&childs);
	sptrIFileDataObject2	sptrF( docUnk );


	if(childs>1) 
	{
		docUnk->Release();
		unkRf->Release();

		return 1;
	}

	
	

	IDocumentSite* doc;
	docUnk->QueryInterface(&doc);
	docUnk->Release();docUnk=0;
	if(!doc) return 0;

	IUnknown* viewUnk;
	
	doc->GetActiveView(&viewUnk);
    IReportView2* rv; viewUnk->QueryInterface(&rv);;
	doc->Release();doc=0;
	
	sptrIReportForm form(unkRf);
	if(unkRf) {unkRf->Release();unkRf=0;}
	else return 0;

	/*INamedDataObject2Ptr nd(form);
	IViewCtrlPtr viewCtrl(form);
	long lCount;
	nd->GetChildsCount( &lCount );
	if(lCount>1)
	{
		long pos;
		nd->GetFirstChildPosition( &pos );
		while(pos)
		{
			IUnknown* pUnk;
			nd->GetNextChild( &pos, &pUnk );

			INamedDataObject2Ptr ndChild(pUnk);
			if(pUnk) pUnk->Release();
			ndChild->SetParent(0,0);

		}
	}*/



	bool bResult=false;
		if( autoMode == ataActiveView )
		{
			bResult = rc.FillOut_ActiveView( form );
		}
		else
		if( autoMode == ataOneObjectPerPage )
		{
			bResult = rc.FillOut_OneObjectPerPage( form );
		}
		else
		if( autoMode == ataVerticalAsIs )
		{
			bResult = rc.FillOut_VerticalAsIs( form );
		}
		else
		if( autoMode == ataGallery )
		{
			bResult = rc.FillOut_Gallery( form );
		}
		else
		if( autoMode == ataFromSplitter )
		{
			bResult = rc.FillOut_FromSplitter( form );
		}

		//if(childs)
		//{
		//	long pos;
		//	nd->GetFirstChildPosition(&pos);
		//	IUnknown* chUnk;
		//	while(pos)
		//	{
		//		nd->GetNextChild(&pos, &chUnk);
		//		INamedDataObject2Ptr ndO = chUnk;	
		//		chUnk->Release();
		//		ndO->SetModifiedFlag(FALSE);
		//	}
		//}
		
	if(rv)
	{
		rv->EnterPreviewMode(1);
		rv->Release();
		rv=0;
	}
			viewUnk->Release();viewUnk=0;

	return bResult;
}
EXTERN_C __declspec (dllexport) 
IUnknown *CreateReport( IUnknown *punkDoc )
{
	CReportConstructor	generator( punkDoc );
	return generator.GenerateReport();
}

//////////////////////////////////////////////////////////////////////
// class CViewObjectList
//////////////////////////////////////////////////////////////////////
CViewObjectList::CViewObjectList()
{
	m_strType.Empty();
	m_strObjectName.Empty();
	m_bUse		= false;
	m_nNum		= -1;	
	m_bNewGroup	= false;
}

//////////////////////////////////////////////////////////////////////
void CViewObjectList::CopyFrom( CViewObjectList* pvo )
{
	m_strType		= pvo->m_strType;
	m_strObjectName	= pvo->m_strObjectName;
	m_bUse			= pvo->m_bUse;
	m_nNum			= pvo->m_nNum;
	m_bNewGroup		= pvo->m_bNewGroup;
}

//////////////////////////////////////////////////////////////////////
// class CViewDescription
//////////////////////////////////////////////////////////////////////
CViewDescription::CViewDescription()
{
	m_strViewProgID.Empty();
	m_sizeView		= CSize( 0, 0 );
	m_sizeClient	= CSize( 0, 0 );
	m_fZoom			= 0.0;
	m_nCol			= -1;
	m_nRow			= -1;
	m_nDPI			= 0;
}

//////////////////////////////////////////////////////////////////////
CViewDescription::~CViewDescription()
{
	DeInit();	
}

//////////////////////////////////////////////////////////////////////
bool CViewDescription::Init( IUnknown* punkView, IUnknown* punkDocumentSite )
{
	DeInit();

	sptrIDataContext sptrDC( punkView );
	if( sptrDC == NULL )
		return false;

	CArray<CString, CString> m_arObjectTypes;
	if( !CReportConstructor::GetAllObjectTypes( m_arObjectTypes ) )	
		return false;

	IViewPtr sptr_view( punkView );
	if( sptr_view == 0 )
		return false;

	TRACE( "\nNew Active Object List creating..." );
	for( int i=0;i<m_arObjectTypes.GetSize();i++ )
	{
		// [vanek] SBT:1261 если вьюха может отображать данные со всех заселекченных объектов - добавляем их все,
		// в противном случае - только активный - 21.12.2004
		DWORD dwobjflags = 0;
        sptr_view->GetObjectFlags( _bstr_t((LPCSTR)m_arObjectTypes[i]), &dwobjflags );
		IDataContext2Ptr sptrdc2 = sptrDC;
		if( sptrdc2 != 0 && (dwobjflags & ofAllSelected) )
		{
			long lpos = 0;
            sptrdc2->GetFirstSelectedPos( _bstr_t((LPCSTR)m_arObjectTypes[i]), &lpos );
			while( lpos )
			{
				IUnknown* punkObject = NULL;
                sptrdc2->GetNextSelected( _bstr_t((LPCSTR)m_arObjectTypes[i]), &lpos, &punkObject );
                if( punkObject == NULL )
					continue;

				CString strName = ::GetObjectName( punkObject );
				punkObject->Release();

				if( strName.IsEmpty() )
					continue;		

				CViewObjectList* pInfo = new CViewObjectList;
				pInfo->m_strObjectName	= strName;
				pInfo->m_strType		= m_arObjectTypes[i];
				m_arViewObject.Add( pInfo );

				CString strDebug;
				strDebug.Format( "\n%s->%s", pInfo->m_strObjectName, pInfo->m_strType );
				TRACE( strDebug );
			}
		}            
		else
		{
			IUnknown* punkObject = NULL;
			sptrDC->GetActiveObject( _bstr_t((LPCSTR)m_arObjectTypes[i]), &punkObject );
			if( punkObject == NULL )
				continue;

			CString strName = ::GetObjectName( punkObject );
			punkObject->Release();

			if( strName.IsEmpty() )
				continue;		

			CViewObjectList* pInfo = new CViewObjectList;
			pInfo->m_strObjectName	= strName;
			pInfo->m_strType		= m_arObjectTypes[i];
			m_arViewObject.Add( pInfo );

			CString strDebug;
			strDebug.Format( "\n%s->%s", pInfo->m_strObjectName, pInfo->m_strType );
			TRACE( strDebug );
		}
	}

	m_arObjectTypes.RemoveAll();

	IPersistPtr ptrPersist( punkView );
	if( ptrPersist )
	{
		CLSID ClassID;
		ISOK( ptrPersist->GetClassID(&ClassID) );
		LPWSTR psz;
		if( ::ProgIDFromCLSID( ClassID, &psz ) == S_OK )
		{
			m_strViewProgID = psz;
			::CoTaskMemFree(psz);
		}
	}

	CRect rcView = NORECT;
	CWnd* pWnd = NULL;
	pWnd = ::GetWindowFromUnknown( punkView );
	if( pWnd )
	{
		pWnd->GetWindowRect( &rcView );
		m_sizeView = CSize( rcView.Width(), rcView.Height() );
	}

	sptrIScrollZoomSite sptrSZS( punkView );
	if( sptrSZS )
	{
		sptrSZS->GetZoom( &m_fZoom );	
		sptrSZS->GetClientSize( &m_sizeClient );		
	}
	


	IUnknown* punkSplitter = NULL;
	punkSplitter = ::GetSplitterFromDocumentSite( punkDocumentSite );
	sptrISplitterWindow sptrSW( punkSplitter );
	if( punkSplitter )
		punkSplitter->Release();

	sptrIDocumentSite sptrDS( punkDocumentSite );


	if( sptrSW != NULL && sptrDS != NULL )
	{		

		int nSplitterRowCount, nSplitterColCount;
		nSplitterRowCount = nSplitterColCount = -1;

		sptrSW->GetRowColCount( &nSplitterRowCount, &nSplitterColCount );

		for( int iRow=0;iRow<nSplitterRowCount;iRow++ )
		{
			for( int iCol=0;iCol<nSplitterColCount;iCol++ )
			{
				IUnknown* punkSplitterView = NULL;				
				sptrSW->GetViewRowCol( iRow, iCol, &punkSplitterView );

				if( punkSplitterView == NULL )
					continue;

				{	
					sptrIView sptrV1 = punkView;
					sptrIView sptrV2 = punkSplitterView;

					if( sptrV1 == sptrV2 )
					{
						m_nRow = iRow;
						m_nCol = iCol;
					}
				}

				punkSplitterView->Release();
			}
		}
	}

	ASSERT( m_nCol >= 0 && m_nRow >= 0 );

	return true;

}


//////////////////////////////////////////////////////////////////////
void CViewDescription::DeInit()
{
	m_strViewProgID.Empty();
	for( int i=0;i<m_arViewObject.GetSize();i++ )
		delete m_arViewObject[i];

	m_arViewObject.RemoveAll();
}


//////////////////////////////////////////////////////////////////////
// class CReportConstructor
//////////////////////////////////////////////////////////////////////

CReportConstructor::CReportConstructor()
{
	Init();
}

//////////////////////////////////////////////////////////////////////
bool CReportConstructor::_CreateAllView()
{
	_DestroyAllView();

	CArray<CString, CString> arViewProgID;
	if( !GetAvailableViewProgID( arViewProgID ) )
		return false;

	for( int i=0;i<arViewProgID.GetSize();i++ )
	{
		if( arViewProgID[i].IsEmpty() )
			continue;

		BSTR bstrProgID = arViewProgID[i].AllocSysString();
		IViewPtr sptrV;
		
		try{
			sptrV.CreateInstance(bstrProgID);
		}
		catch( ... ){
		}
		::SysFreeString(bstrProgID);
		
		if( sptrV == NULL )
			continue;

		CViewInfo* pvi = new CViewInfo;
		
		pvi->m_strProgID	= arViewProgID[i];
		pvi->ptrView		= sptrV;
		
		m_arAllView.Add( pvi );
	}

	return true;


}

//////////////////////////////////////////////////////////////////////
bool CReportConstructor::_DestroyAllView()
{
	for( int i=0;i<m_arAllView.GetSize();i++ )
		delete m_arAllView[i];

	m_arAllView.RemoveAll();
	
	return true;
}

//////////////////////////////////////////////////////////////////////
IUnknown* ::CReportConstructor::GetViewByProgID( CString strProgID )
{
	for( int i=0;i<m_arAllView.GetSize();i++ )
	{
		if( m_arAllView[i]->m_strProgID == strProgID )
		{
			m_arAllView[i]->ptrView->AddRef();
			return m_arAllView[i]->ptrView;
		}
	}

	return NULL;	
}

//////////////////////////////////////////////////////////////////////
void CReportConstructor::GetViewProgIDSupportByType( CArray<CString,CString>& arViewProgID, CString strType )
{
	if( m_arAllView.GetSize() < 1 )
		return;

	for( int i=0;i<m_arAllView.GetSize();i++ )
	{
		DWORD dwMatch = mvNone;
		m_arAllView[i]->ptrView->GetMatchType( _bstr_t( (LPCSTR)strType ), &dwMatch );
		if( dwMatch >= mvPartial )
		{
			arViewProgID.Add( m_arAllView[i]->m_strProgID );			
		}		
	}
}


//////////////////////////////////////////////////////////////////////
CReportConstructor::CReportConstructor( IUnknown* punkDoc, bool bGetSettingsFromDocument )
{
	Init();

	m_sptrDocSite = punkDoc;
	if( bGetSettingsFromDocument )
		GetSettingsFromDocument();
}

//////////////////////////////////////////////////////////////////////
CReportConstructor::~CReportConstructor()
{
	DeInit();	
	_DestroyAllView();
}

//////////////////////////////////////////////////////////////////////
void CReportConstructor::Init()
{
	DeInit();
	m_templateSource = tsAutoTemplate;
	m_strTemplateName.Empty();
	m_autoTemplateAlghoritm = ataActiveView;
	m_templateGenerateTarget = tgtNoGenerate;
	m_strNewTemplateObjectName.Empty();
	m_strNewTemplateFileName.Empty();	
}

//////////////////////////////////////////////////////////////////////
void CReportConstructor::DeInit()
{
	DeInitActiveViewDescription();
	DeInitSplitterViewDescription();
	DeInitActiveObjectList();
}


//////////////////////////////////////////////////////////////////////
void CReportConstructor::AttachDocument( IUnknown* punkDoc )
{
	m_sptrDocSite = punkDoc;	
	
	DeInit();
	GetSettingsFromDocument();
	
	CreateActiveViewDescription();	
	CreateSplitterViewDescription( );
	CreateActiveObjectList();

	m_sptrDocSite = NULL;
}
 
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//	AutoTemplateAlghoritm == ataActiveView
//	Active view description
//////////////////////////////////////////////////////////////////////
bool CReportConstructor::CreateActiveViewDescription()
{
	DeInitActiveViewDescription();	

	if( m_sptrDocSite == NULL )
		return false;


	IUnknown* punk = NULL;
	ISOK( m_sptrDocSite->GetActiveView( &punk ) );

	if( punk == NULL )
		return false;

	sptrIView sptrV( punk );
	punk->Release();
	punk = NULL;

	if( sptrV == NULL )
		return false;
	
	return m_ActiveViewDescription.Init( sptrV, m_sptrDocSite );
}

//////////////////////////////////////////////////////////////////////
void CReportConstructor::DeInitActiveViewDescription()
{
	m_ActiveViewDescription.DeInit();
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//	AutoTemplateAlghoritm == ataFromSplitter
//	Splitter view description
//////////////////////////////////////////////////////////////////////
bool CReportConstructor::CreateSplitterViewDescription( )
{
	DeInitSplitterViewDescription();

	m_sizeSplitterWnd	= CSize( 0, 0 );
	m_nSplitterRowCount = -1;
	m_nSplitterColCount	= -1;
	

	if( m_sptrDocSite == NULL )
		return false;
	

	IUnknown* punkSplitter = ::GetSplitterFromDocumentSite( m_sptrDocSite );
	if( punkSplitter == NULL )
		return false;

	sptrISplitterWindow sptrSW( punkSplitter );
	if( punkSplitter )
		punkSplitter->Release();

	if( sptrSW == NULL )
		return NULL;

	sptrSW->GetRowColCount( &m_nSplitterRowCount, &m_nSplitterColCount );


	CWnd* pWnd = NULL;
	pWnd = GetWindowFromUnknown( sptrSW );
	if( pWnd )
	{
		CRect rcSplitter;
		pWnd->GetWindowRect( &rcSplitter );
		m_sizeSplitterWnd = CSize( rcSplitter.Width(), rcSplitter.Height() );
	}
	

	long lPos = 0;
	m_sptrDocSite->GetFirstViewPosition( &lPos );
	while( lPos )
	{
		IUnknown* punkView = NULL;
		m_sptrDocSite->GetNextView( &punkView, &lPos );
		if( punkView == NULL )
			continue;
		CViewDescription* pvd = new CViewDescription;
		pvd->Init( punkView, m_sptrDocSite );
		m_arSplitterViewDescription.Add( pvd );
		punkView->Release();
	}	
		
	return true;

}

//////////////////////////////////////////////////////////////////////
void CReportConstructor::DeInitSplitterViewDescription()
{
	for( int i=0;i<m_arSplitterViewDescription.GetSize();i++ )
		delete m_arSplitterViewDescription[i];

	m_arSplitterViewDescription.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
int CReportConstructor::GetSplitterViewDescriptionCount()
{
	return m_arSplitterViewDescription.GetSize();
}

//////////////////////////////////////////////////////////////////////
CViewDescription* CReportConstructor::GetSplitterViewDescriptionItem( int nIndex )
{
	if( nIndex < 0 || nIndex >= GetSplitterViewDescriptionCount() )
		return NULL;

	return m_arSplitterViewDescription[nIndex];
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//	AutoTemplateAlghoritm == ataOneObjectPerPage || ataVerticalAsIs || ataGallery
//	Active object
//////////////////////////////////////////////////////////////////////
bool CReportConstructor::CreateActiveObjectList()
{
	DeInitActiveObjectList();

	if( m_sptrDocSite == NULL )
		return false;


	
	sptrIDataContext2 sptrDC2( m_sptrDocSite );
	if( sptrDC2 == NULL )
		return false;


	{
		IUnknown* punkView = NULL;
		m_sptrDocSite->GetActiveView( &punkView );
		if( punkView )
		{
			sptrDC2->SynchronizeWithContext( punkView );
			punkView->Release();
		}
	}
	

	CArray<CString, CString> m_arObjectTypes;
	if( !GetAllObjectTypes( m_arObjectTypes ) )	
		return false;

	for( int i=0;i<m_arObjectTypes.GetSize();i++ )
	{
		_bstr_t bstrType = (LPCSTR)m_arObjectTypes[i];
		
		if( bstrType == _bstr_t( szTypeReportForm ) )
			continue;

		long nPos = 0;
		sptrDC2->GetFirstSelectedPos( bstrType,  &nPos );
		int nObjectNum = 0;
		while( nPos )
		{
			
			IUnknown* punkObject = NULL;
			sptrDC2->GetNextSelected( bstrType,  &nPos, &punkObject );
			if( punkObject )
			{
				CViewObjectList* pvo = new CViewObjectList;

				pvo->m_strObjectName	= ::GetObjectName( punkObject );
				pvo->m_strType			= (LPCSTR)bstrType;
				pvo->m_nNum				= nObjectNum;

				m_arActiveObject.Add( pvo );
				punkObject->Release();
				nObjectNum++;
			}
		}						
	}

	m_arObjectTypes.RemoveAll();

	return true;
}

//////////////////////////////////////////////////////////////////////
void CReportConstructor::DeInitActiveObjectList()
{
	for( int i=0;i<m_arActiveObject.GetSize();i++ )
		delete m_arActiveObject[i];

	m_arActiveObject.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
int CReportConstructor::GetActiveObjectListCount()
{
	return m_arActiveObject.GetSize();
}

//////////////////////////////////////////////////////////////////////
CViewObjectList* CReportConstructor::GetActiveObjectListItem( int nIndex )
{
	if( nIndex < 0 || nIndex >= GetActiveObjectListCount() )
		return NULL;

	return m_arActiveObject[nIndex];
}


class _ViewProgID
{
public:
	CString strViewProgID;
	int nObjectIndex;
};

//////////////////////////////////////////////////////////////////////
bool CReportConstructor::CreateGroupViewObjectList( CArray<CViewObjectList*, CViewObjectList*>& arOL )
{
	int nObjectCount = g_ReportConstructor.GetActiveObjectListCount();
	if( nObjectCount < 1 )
		return false;

	if( nObjectCount == 1 )
	{
		CViewObjectList* pvo = g_ReportConstructor.GetActiveObjectListItem( 0 );
		
		CViewObjectList* pvoNew = new CViewObjectList;
		pvoNew->CopyFrom( pvo );
		pvoNew->m_bNewGroup = true;
		arOL.Add( pvoNew );
		return true;		
	}


	_CreateAllView();


	//Init Object list
	for( int i=0;i<nObjectCount;i++ )
	{
		CViewObjectList* pvo = g_ReportConstructor.GetActiveObjectListItem( i );
		pvo->m_bUse = false;
		pvo->m_bNewGroup = false;
	}

	bool bUseGroup = ( 1L == ::GetValueInt(		::GetAppUnknown(), 
												"\\PrintPreview", 
												"Use object base group", 
												0L ) );
	if( bUseGroup )
		return CreateGroupViewObjectListUseBaseGroup( arOL );

	for( i=0;i<nObjectCount;i++ )
	{
		CViewObjectList* pvo = g_ReportConstructor.GetActiveObjectListItem( i );
		if( pvo->m_bUse )
			continue;

		CArray<CString,CString> arProgID;
		GetViewProgIDSupportByType( arProgID, pvo->m_strType );
		if( arProgID.GetSize() < 1 )
			continue;

		CArray<_ViewProgID, _ViewProgID> arTemp;
		for( int index=0;index<arProgID.GetSize();index++ )
		{
			_ViewProgID vp;
			vp.strViewProgID	= arProgID[index];
			vp.nObjectIndex		= i;
			arTemp.Add( vp );
		}
		
		int nFirstObjectEntryCount = arProgID.GetSize();
		
		arProgID.RemoveAll();		

		if( i != nObjectCount )
		{
			CStringArray arAddTypes;
			arAddTypes.Add( pvo->m_strType );

			for( int j=i+1;j<nObjectCount;j++ )
			{
				CViewObjectList* pvo2 = g_ReportConstructor.GetActiveObjectListItem( j );
				if( pvo->m_strType == pvo2->m_strType || pvo2->m_bUse )
					continue;
				
				bool bFindType = false;
				for( int ind=0;ind<arAddTypes.GetSize();ind++ )
					if( arAddTypes[ind] == pvo2->m_strType )
						bFindType = true;

				if( bFindType )
					continue;

				arAddTypes.Add( pvo2->m_strType );

				GetViewProgIDSupportByType( arProgID, pvo2->m_strType );
				if( arProgID.GetSize() < 1 )
					continue;
				
				for( int index=0;index<arProgID.GetSize();index++ )
				{
					_ViewProgID vp;
					vp.strViewProgID	= arProgID[index];
					vp.nObjectIndex		= j;
					arTemp.Add( vp );
				}
				arProgID.RemoveAll();
			}
		}

		//now have arTemp
		if( arTemp.GetSize() < 1 )
			continue;


		int nSupportedCount = 1;
		CString strMacthProgID = arTemp[0].strViewProgID;;

		for( int j=0;j<nFirstObjectEntryCount;j++ )
		{
			int nCurSupportedCount = 0;
			CString strProgID = arTemp[j].strViewProgID;
			for( int k=nFirstObjectEntryCount;k<arTemp.GetSize();k++ )
			{
				if( strProgID == arTemp[k].strViewProgID )
					nCurSupportedCount++;
			}

			if( nCurSupportedCount > nSupportedCount )
				strMacthProgID = strProgID;
		}

		
		bool bFirst = true;
		for( j=0;j<arTemp.GetSize();j++ )
		{
			if( strMacthProgID == arTemp[j].strViewProgID )
			{
				CViewObjectList* pvoAdd = new CViewObjectList;
				CViewObjectList* pvoCopyFrom = g_ReportConstructor.GetActiveObjectListItem( arTemp[j].nObjectIndex );

				pvoAdd->CopyFrom( pvoCopyFrom );
				pvoCopyFrom->m_bUse = true;
				pvoAdd->m_bUse = true;
				pvoAdd->m_bNewGroup = bFirst;				

				arOL.Add( pvoAdd );
				if( bFirst ) 
					bFirst = false;
			}			
		}
		
		arTemp.RemoveAll();
		
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CReportConstructor::CreateGroupViewObjectListUseBaseGroup( CArray<CViewObjectList*, CViewObjectList*>& arOL )
{
	IDataContext2Ptr ptrContext( m_sptrDocSite );
	if( ptrContext == 0 )
		return false;

	INamedDataPtr ptrData( m_sptrDocSite );
	if( ptrData == 0 )
		return false;

	int nObjectCount = g_ReportConstructor.GetActiveObjectListCount();

	for( int i=0;i<nObjectCount;i++ )
	{
		CViewObjectList* pvo = g_ReportConstructor.GetActiveObjectListItem( i );
		if( pvo->m_bUse )
			continue;

		IUnknown* punk = 0;
		ptrData->GetObject( _bstr_t( pvo->m_strObjectName ), _bstr_t( pvo->m_strType ), &punk );
		if( !punk )
			continue;

		IUnknownPtr ptrBaseObj = punk;
		punk->Release();	punk = 0;


		GuidKey guidGroup = INVALID_KEY;

		long lGroupPos = 0;
		ptrData->GetBaseGroupFirstPos( &lGroupPos );
		while( lGroupPos )
		{
			GuidKey guid = INVALID_KEY;
			ptrData->GetNextBaseGroup( &guid, &lGroupPos );
			IUnknown* punkObject = NULL;
			ptrData->GetBaseGroupBaseObject( &guid, &punkObject );
			if( !punkObject )
				continue;

			if( ::GetObjectKey( ptrBaseObj ) == ::GetObjectKey(punkObject) )
				guidGroup = guid;

			punkObject->Release();
		}

		if( guidGroup == INVALID_KEY )
			continue;

		CViewObjectList* pvol_base_group = new CViewObjectList;
		arOL.Add( pvol_base_group );		
		
		pvol_base_group->CopyFrom( pvo );

		pvo->m_bUse						= true;
		pvol_base_group->m_bNewGroup	= true;
		pvol_base_group->m_bUse			= true;


		long lPos = 0;
		ptrData->GetBaseGroupObjectFirstPos( &guidGroup, &lPos );
		while( lPos )
		{
			IUnknown* punkObj = 0;
			ptrData->GetBaseGroupNextObject( &guidGroup, &lPos, &punkObj );
			if( punkObj == NULL )
				continue;

			CString strName = ::GetObjectName( punkObj );
			CString strType = ::GetObjectKind( punkObj );

			punkObj->Release();

			for( int j=0;j<nObjectCount;j++ )
			{
				CViewObjectList* pvo2 = g_ReportConstructor.GetActiveObjectListItem( j );
				if( pvo2->m_bUse )
					continue;

				if( !( strName == pvo2->m_strObjectName && strType == pvo2->m_strType ) )
					continue;

				CViewObjectList* pvol_in_group = new CViewObjectList;
				arOL.Add( pvol_in_group );
				
				pvol_in_group->CopyFrom( pvo2 );

				pvo2->m_bUse						= true;
				pvol_in_group->m_bNewGroup			= false;
				pvol_in_group->m_bUse				= true;
			}
		}
	}


	for( i=0;i<nObjectCount;i++ )
	{
		CViewObjectList* pvo = g_ReportConstructor.GetActiveObjectListItem( i );
		if( pvo->m_bUse )
			continue;

		CViewObjectList* pvol_single = new CViewObjectList;
		arOL.Add( pvol_single );		
		
		pvol_single->CopyFrom( pvo );

		pvo->m_bUse					= true;
		pvol_single->m_bNewGroup	= true;
		pvol_single->m_bUse			= true;

	}

	return true;
}

//////////////////////////////////////////////////////////////////////
void CReportConstructor::GetSettingsFromDocument()
{
	IUnknown* punk_data = m_sptrDocSite;
	if( punk_data == 0 )	punk_data = ::GetAppUnknown();

	m_templateSource = (TemplateSource)::GetValueInt( punk_data, 
								REPORT_TEMPLATE_SECTION, REPORT_TEMPLATE_SOURCE, tsAutoTemplate );

	
	m_strTemplateName = ::GetValueString( punk_data, 
								REPORT_TEMPLATE_SECTION, REPORT_EXIST_TEMPLATE_NAME, "" );

	m_autoTemplateAlghoritm = (AutoTemplateAlghoritm)::GetValueInt( punk_data, 
								REPORT_TEMPLATE_SECTION, REPORT_AUTO_TEMPLATE_ALGHORITM, ataActiveView );

	m_templateGenerateTarget = (TemplateGenerateTarget)::GetValueInt( punk_data, 
								REPORT_TEMPLATE_SECTION, REPORT_NEW_TEMPLATE_TARGET, tgtNoGenerate );
	m_strNewTemplateObjectName = ::GetValueString( punk_data, 
								REPORT_TEMPLATE_SECTION, REPORT_NEW_TEMPLATE_FILENAME, "" );
	m_strNewTemplateFileName = ::GetValueString( punk_data, 
								REPORT_TEMPLATE_SECTION, REPORT_NEW_TEMPLATE_OBJECTNAME, "" );
	
}

//////////////////////////////////////////////////////////////////////
bool CReportConstructor::GetAllObjectTypes( CArray<CString, CString>& arTypes )
{
	sptrIApplication spApp( ::GetAppUnknown() );
	if( spApp == NULL )
		return false;
	
	
	sptrIDataTypeInfoManager	sptrT( spApp );
	if( sptrT == NULL )
		return false;

	long	nTypesCount = 0;
	sptrT->GetTypesCount( &nTypesCount );
	for( long nType = 0; nType < nTypesCount; nType++ )
	{
		BSTR	bstr = NULL;
		sptrT->GetType( nType, &bstr );

		arTypes.Add( bstr );

		::SysFreeString( bstr );
	}

	return true;

}


//////////////////////////////////////////////////////////////////////
IUnknown* CReportConstructor::GenerateReport()
{
	CWaitCursor wait;

	IUnknown* punkForm = CreateReport();
	if( punkForm == NULL )
		return NULL;

	sptrIReportForm form( punkForm );
	punkForm->Release();


	IUnknownPtr ptrExistReport;

	bool bResult = false;
	
	if( m_templateSource == tsUseExist )
	{
		IUnknown* punkReportDocFind = NULL;
		IUnknown* punkReport = NULL;
		bResult = FillOut_UseExist( &punkReport, &punkReportDocFind );

		if( punkReport )
		{
			ptrExistReport = punkReport;
			punkReport->Release();
		}

		if( punkReportDocFind )
		{
			m_sptrDocSiteCopyFrom = punkReportDocFind;
			punkReportDocFind->Release();
		}
	}
	else	
	if( m_templateSource == tsAutoTemplate )
	{
		if( m_autoTemplateAlghoritm == ataActiveView )
		{
			bResult = FillOut_ActiveView( form );
		}
		else
		if( m_autoTemplateAlghoritm == ataOneObjectPerPage )
		{
			bResult = FillOut_OneObjectPerPage( form );
		}
		else
		if( m_autoTemplateAlghoritm == ataVerticalAsIs )
		{
			bResult = FillOut_VerticalAsIs( form );
		}
		else
		if( m_autoTemplateAlghoritm == ataGallery )
		{
			bResult = FillOut_Gallery( form );
		}
		else
		if( m_autoTemplateAlghoritm == ataFromSplitter )
		{
			bResult = FillOut_FromSplitter( form );
		}
	}

	if( !bResult )
	{
		IUnknown* punkDefForm = CreateReport();
		if( punkDefForm == NULL )
			return NULL;

		sptrIReportForm formDef( punkDefForm );
		punkDefForm->Release();

		FillOut_ActiveView( formDef );

		formDef->AddRef();
		return formDef;
	}


	if( ptrExistReport != NULL )
	{
		ptrExistReport->AddRef();		
		return ptrExistReport;
	}

	form->AddRef();
	return form;	
}

//////////////////////////////////////////////////////////////////////
bool CReportConstructor::FillOut_UseExist( IUnknown** ppunkReport, IUnknown** ppunkReportDocFind )
{
	if( ppunkReport == NULL )
		return false;


	IUnknownPtr ptrSourceReport;
	IUnknownPtr ptrDocFind;

	
	CString strTemplateName = m_strTemplateName;
	strTemplateName.MakeLower();
	


	bool bUseFromFile = (strTemplateName.Find( ".rpt" ) != -1 );

	
	if( bUseFromFile )
	{

		IUnknown* punkSourceReport = NULL;
		punkSourceReport = CreateReport();
		
		if( punkSourceReport == NULL )
			return false;

		ptrSourceReport = punkSourceReport;

		punkSourceReport->Release();

		if( ptrSourceReport == NULL )
			return false;

	
		CString strFileName = ::GetReportDir() + m_strTemplateName;		
		
		//Load source template
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
				return false;
			}
				
			sptrISerializableObject	sptrO( ptrSourceReport );
			if( sptrO == NULL )		
				return false;

			SerializeParams	params;
			ZeroMemory( &params, sizeof( params ) );

			
			CArchive		ar( &file, CArchive::load );
			CArchiveStream	ars( &ar );		
			sptrO->Load( &ars, &params );
		}
	}
	else	
	{

		//first lookup in active document
		IUnknown* punkBasedOnReport = ::GetObjectByName( m_sptrDocSite, m_strTemplateName, szTypeReportForm );
		if( punkBasedOnReport )
		{
			ptrDocFind = m_sptrDocSite;
		}
		else
		{
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
						
						if( punkBasedOnReport == NULL )
						{
							punkBasedOnReport = ::GetObjectByName( punkDoc, m_strTemplateName, szTypeReportForm );						
							
							if( punkBasedOnReport != NULL )
							{
								ptrDocFind = punkDoc;
							}												
						}
						  
						punkDoc->Release();
					}

					sptrA->GetNextDocTempl( &lPosTemplate, 0, 0 );
				} 
			}
		}  

		if( punkBasedOnReport == NULL )			
			return false;			

		
		IUnknown* punk = NULL;
		punk = CloneObject( punkBasedOnReport );

		if( CheckInterface( punkBasedOnReport, IID_INamedData ) )
		{
			INamedDataObject2Ptr ptr_obj_new( punk );
			if( ptr_obj_new )
			{
				ptr_obj_new->InitAttachedData();

				INamedDataPtr ptr_src( punkBasedOnReport );
				INamedDataPtr ptr_target( punk );
				RecursiveWriteEntry( ptr_src, ptr_target, "", true );
			}
		}
		
		if( punk == NULL )
			return false;			

		punkBasedOnReport->Release();
											
		ptrSourceReport = punk;
		punk->Release();		
		
	}


	if( ptrSourceReport == NULL )
		return false;


	{
		INumeredObjectPtr ptrNO( ptrSourceReport );
		if( ptrNO )
			ptrNO->GenerateNewKey( 0 );
	}

	
	
	if( !ProcessExistReport( ptrSourceReport ) )
		return false;		
	

	
	if( ptrDocFind )
	{
		ptrDocFind->AddRef();
		*ppunkReportDocFind = ptrDocFind;
	}
	


	ptrSourceReport->AddRef();
	*ppunkReport = ptrSourceReport;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CReportConstructor::ProcessExistReport( IUnknown* punkReport )
{

	IReportFormPtr form( punkReport );
	if( form == NULL )
		return false;


	INamedDataObject2Ptr ptrNDO2( form );
	if( ptrNDO2 == NULL )
		return false;

	CArray<CViewObjectList*, CViewObjectList*> arOL;
	CreateGroupViewObjectList( arOL );
	if( arOL.GetSize() < 1 )
		return true;

	for( int i=0;i<arOL.GetSize();i++ )
		arOL[i]->m_bUse = false;


	//Process arOL
	{
		long lPos;
		ptrNDO2->GetFirstChildPosition( &lPos );
		while( lPos )
		{
			IUnknown* punkChild = NULL;
			ptrNDO2->GetNextChild( &lPos, &punkChild );
			if( punkChild )
			{
				sptrIReportCtrl sptrRC( punkChild );
				punkChild->Release();

				if( sptrRC )
				{

					IUnknown* punkAXCtrl = NULL;
					punkAXCtrl = GetAXCtrlFromDataCtrl( sptrRC );
					if( punkAXCtrl == NULL )
						continue;

					IViewCtrlPtr ptrAX( punkAXCtrl );
					punkAXCtrl->Release();

					if( ptrAX == NULL )
						continue;

					BOOL bUseActiveView = FALSE;
					ptrAX->GetUseActiveView( &bUseActiveView );
					if( bUseActiveView )
						continue;


					long lVPos = 0;
					ptrAX->GetFirstObjectPosition( &lVPos );
					while( lVPos )
					{
						BSTR bstrObjectName, bstrObjectType;
						BOOL bActiveObject = FALSE;
						bstrObjectName = bstrObjectType = NULL;
						
						ptrAX->GetNextObject( &bstrObjectName, &bActiveObject, 
												&bstrObjectType, &lVPos );

						if( !bActiveObject )
						{
							for( int j=0;j<arOL.GetSize();j++ )
							{
								if( CString(bstrObjectName) == arOL[j]->m_strObjectName &&
									CString(bstrObjectType) == arOL[j]->m_strType)
								{
									arOL[j]->m_bUse = true;	
								}
							}
						}


						::SysFreeString( bstrObjectName );
						::SysFreeString( bstrObjectType );		
					}
				}
			}
		}

	}


	long lPos;
	ptrNDO2->GetFirstChildPosition( &lPos );
	while( lPos )
	{
		IUnknown* punkChild = NULL;
		ptrNDO2->GetNextChild( &lPos, &punkChild );
		if( punkChild )
		{
			sptrIReportCtrl sptrRC( punkChild );
			punkChild->Release();

			if( sptrRC )
			{

				IUnknown* punkAXCtrl = NULL;
				punkAXCtrl = GetAXCtrlFromDataCtrl( sptrRC );
				if( punkAXCtrl == NULL )
					continue;

				IViewCtrlPtr ptrAX( punkAXCtrl );
				punkAXCtrl->Release();

				if( ptrAX == NULL )
					continue;

				BOOL bUseActiveView = FALSE;
				ptrAX->GetUseActiveView( &bUseActiveView );
				if( bUseActiveView )
					continue;


				long lVPos = 0;
				ptrAX->GetFirstObjectPosition( &lVPos );
				while( lVPos )
				{
					BSTR bstrObjectName, bstrObjectType;
					BOOL bActiveObject = FALSE;
					bstrObjectName = bstrObjectType = NULL;
					
					long nPosSave = lVPos;
					ptrAX->GetNextObject( &bstrObjectName, &bActiveObject, 
											&bstrObjectType, &lVPos );

					if( bActiveObject )
					{
						bool bFound = false;
						for( int j=0;j<arOL.GetSize();j++ )
						{
							if( bFound )
								continue;							
							
							if( !arOL[j]->m_bUse && arOL[j]->m_strType == CString(bstrObjectType) )
							{
								ptrAX->EditAt( nPosSave, _bstr_t( (LPCSTR)arOL[j]->m_strObjectName),
														FALSE,
														_bstr_t( (LPCSTR)arOL[j]->m_strType)
														 );									
								arOL[j]->m_bUse = true;
								bFound = true;
							}
						}
					}

					::SysFreeString( bstrObjectName );
					::SysFreeString( bstrObjectType );		
				}

				::StoreContainerToDataObject( sptrRC, ptrAX );				
			}
		}
	}


	for( i=0;i<arOL.GetSize();i++ )
	{
		delete arOL[i];
	}
	arOL.RemoveAll();


	return true;
}


//////////////////////////////////////////////////////////////////////
bool CReportConstructor::FillOut_ActiveView( sptrIReportForm form )
{
	if( form == NULL )
		return false;

	if( UsePresetMargins() )
	{		
		form->SetPaperField( GetGlobalPaperMargins( ACTIVE_VIEW_MARGINS ) );
	}



	IUnknown* punkData = NULL;
	IUnknown* punkCtrl = NULL;

	if( !CreateControl( &punkData, &punkCtrl ) )
	{
		ASSERT( FALSE );
		return false;
	}

	sptrIReportCtrl sptrRC( punkData );
	sptrIViewCtrl sptrVAX( punkCtrl );

	if( punkData ) punkData->Release();
	if( punkCtrl ) punkCtrl->Release();

	if( sptrRC == NULL || sptrVAX == NULL )
		return false;

	CViewDescription* pvd = g_ReportConstructor.GetActiveViewDescription();

	//Set AXView properties
	sptrVAX->SetAutoBuild( FALSE );
	sptrVAX->SetObjectTransformation( (short)otStretch );
	sptrVAX->SetUseActiveView( FALSE );
	sptrVAX->SetViewAutoAssigned( FALSE );
	sptrVAX->SetViewProgID( _bstr_t( pvd->GetViewProgID() ) );

	for( int j=0;j<pvd->m_arViewObject.GetSize();j++ )
	{
		CViewObjectList* pObject = pvd->m_arViewObject[j];
		sptrVAX->InsertAfter( 0, _bstr_t( (LPCSTR)pObject->m_strObjectName ), 
								FALSE, _bstr_t( (LPCSTR)pObject->m_strType ) );
	}	

	//Set Rectangle
	CRect rcWorkArea = GetReportWorkArea( form );

	CRect rcDesign;	
	rcDesign = rcWorkArea;
	rcDesign.DeflateRect(	D2V( REPORT_CTRL_WORK_AREA_ALIGN_MM ),
							D2V( REPORT_CTRL_WORK_AREA_ALIGN_MM ),
							D2V( REPORT_CTRL_WORK_AREA_ALIGN_MM ),
							D2V( REPORT_CTRL_WORK_AREA_ALIGN_MM )
							);

	CSize sizeImage ( rcDesign.Width(), rcDesign.Height() );

	bool bSetImageSize = false;
	double fDPI = 0;
	
	double	fShellDataZoom		= GetShellDataZoom();

	bool	bShellDataTrans		= UsePresetTransformation();
	short	nShellData			= GetPresetTransformation();

	for( j=0;j<pvd->m_arViewObject.GetSize();j++ )
	{
		CViewObjectList* pObject = pvd->m_arViewObject[j];
		if( GetObjectSizeVTPixel( pObject->m_strObjectName, sizeImage, fDPI ) )
			bSetImageSize = true;		
	}

	if( bSetImageSize )
	{
		sptrVAX->SetObjectTransformation( bShellDataTrans ? nShellData : (short)otGrowTwoSide );
		sptrVAX->SetDPI( fDPI );
		sptrVAX->SetZoom( fShellDataZoom );

		sizeImage.cx *= fShellDataZoom;
		sizeImage.cy *= fShellDataZoom;
	}
	

	CSize sizeSet;
	
	if( bShellDataTrans  && nShellData == otStretch /*paul fix 27.04.2002*/ )
		sizeSet = sizeImage;
	else
		sizeSet = CSize( rcDesign.Width(), rcDesign.Height() );

	double fZoom = (double)sizeSet.cx / (double)sizeSet.cy;

	if( sizeSet.cx >= rcWorkArea.Width() )
	{
		sizeSet.cx = rcWorkArea.Width();
		sizeSet.cy = (double)sizeSet.cx / fZoom;
	}

	if( sizeSet.cy >= rcWorkArea.Height() )
	{
		sizeSet.cy = rcWorkArea.Height();
		sizeSet.cx = (double)sizeSet.cy * fZoom;
	}

	rcDesign.right  = rcDesign.left + sizeSet.cx;
	rcDesign.bottom = rcDesign.top  + sizeSet.cy;
	

	sptrRC->SetDesignPosition( rcDesign );		

	sptrRC->SetDesignOwnerPage( 0 );
	sptrRC->SetDesignColRow( 0, 0 );

	::StoreContainerToDataObject( sptrRC, sptrVAX ); 
	StoreCtrlToForm( sptrRC, form );
	
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CReportConstructor::FillOut_OneObjectPerPage( sptrIReportForm form )
{

	if( form == NULL )
		return false;


	if( UsePresetMargins() )
	{		
		form->SetPaperField( GetGlobalPaperMargins( ONE_OBJECT_PER_PAGE_MARGINS ) );
	}


	CArray<CViewObjectList*, CViewObjectList*> arOL;
	CreateGroupViewObjectList( arOL );
	if( arOL.GetSize() < 1 )
		return true;

	CArray<CViewDescription*, CViewDescription*> arVD;

	CViewDescription* pCurVD = NULL;
	for( int i=0;i<arOL.GetSize();i++ )
	{
		if( arOL[i]->m_bNewGroup )
		{
			CViewDescription* pVD = new CViewDescription;
			pCurVD = pVD;
			arVD.Add( pVD );
		}

		if( pCurVD )
		{
			CViewObjectList* pvol = new CViewObjectList;
			pvol->CopyFrom( arOL[i] );
			pCurVD->m_arViewObject.Add( pvol );
		}

	}

	for( i=0;i<arOL.GetSize();i++)
		delete arOL[i];

	arOL.RemoveAll();	

	if( arVD.GetSize() < 1 )
		return false;



	CRect rcWorkArea = GetReportWorkArea( form );

	int nDelta = D2V( 5 ); //5 mm
	rcWorkArea.DeflateRect( nDelta, nDelta, nDelta, nDelta );	

	for( i=0;i<arVD.GetSize();i++ )
	{
		CViewDescription* pvd = arVD[i];

		IUnknown* punkData = NULL;
		IUnknown* punkCtrl = NULL;

		if( !CreateControl( &punkData, &punkCtrl ) )
		{
			ASSERT( FALSE );
			return false;
		}

		sptrIReportCtrl sptrRC( punkData );
		sptrIViewCtrl sptrVAX( punkCtrl );

		if( punkData ) punkData->Release();
		if( punkCtrl ) punkCtrl->Release();

		if( sptrRC == NULL || sptrVAX == NULL )
			return false;

		CRect rcItem = rcWorkArea;
		/*
		rcItem.left	= rcWorkArea.left;
		rcItem.top	= rcWorkArea.top;
		rcItem.right	= rcItem.left + rcWorkArea.Width() / 3;
		rcItem.bottom	= rcItem.top + rcWorkArea.Height() / 3;
		*/


		//Set AXView properties
		sptrVAX->SetAutoBuild( FALSE );
		sptrVAX->SetObjectTransformation( (short)otStretch );
		sptrVAX->SetUseActiveView( FALSE );
		sptrVAX->SetViewAutoAssigned( TRUE );		

		for( int j=0;j<pvd->m_arViewObject.GetSize();j++ )
		{
			CViewObjectList* pObject = pvd->m_arViewObject[j];
			sptrVAX->InsertAfter( 0, _bstr_t( (LPCSTR)pObject->m_strObjectName ), 
									FALSE, _bstr_t( (LPCSTR)pObject->m_strType ) );
		}	


		CSize sizeImage ( rcItem.Width(), rcItem.Height() );

		bool bSetImageSize = false;
		double fDPI = 0;


		double	fShellDataZoom		= GetShellDataZoom();

		bool	bShellDataTrans		= UsePresetTransformation();
		short	nShellData			= GetPresetTransformation();


		for( j=0;j<pvd->m_arViewObject.GetSize();j++ )
		{
			CViewObjectList* pObject = pvd->m_arViewObject[j];
			if( GetObjectSizeVTPixel( pObject->m_strObjectName, sizeImage, fDPI ) )
				bSetImageSize = true;		
		}
			
		if( bSetImageSize )
		{
			sptrVAX->SetObjectTransformation( bShellDataTrans ? nShellData : (short)otGrowTwoSide );		
			sptrVAX->SetDPI( fDPI );
			sptrVAX->SetZoom( fShellDataZoom );

			sizeImage.cx *= fShellDataZoom;
			sizeImage.cy *= fShellDataZoom;
		}

		CSize sizeSet;

		if( bShellDataTrans  && nShellData == otStretch /*paul fix 27.04.2002*/ )
			sizeSet = sizeImage;
		else
			sizeSet = CSize( rcItem.Width(), rcItem.Height() );

		double fZoom = (double)sizeSet.cx / (double)sizeSet.cy;


		if( sizeSet.cx >= rcItem.Width() )
		{
			sizeSet.cx = rcItem.Width();
			sizeSet.cy = (double)sizeSet.cx / fZoom;
		}

		if( sizeSet.cy >= rcItem.Height() )
		{
			sizeSet.cy = rcItem.Height();
			sizeSet.cx = (double)sizeSet.cy * fZoom;
		}

		rcItem.right  = rcItem.left + sizeSet.cx;
		rcItem.bottom = rcItem.top  + sizeSet.cy;


		sptrRC->SetDesignPosition( rcItem );		

		sptrRC->SetDesignOwnerPage( i );
		sptrRC->SetDesignColRow( 0, 0 );

		::StoreContainerToDataObject( sptrRC, sptrVAX ); 
		StoreCtrlToForm( sptrRC, form );
	}


	for( i=0;i<arVD.GetSize();i++ )
		delete arVD[i];

	arVD.RemoveAll();

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CReportConstructor::FillOut_VerticalAsIs( sptrIReportForm form )
{
	if( form == NULL )
		return false;

	if( UsePresetMargins() )
	{		
		form->SetPaperField( GetGlobalPaperMargins( VERTICAL_AS_IS_MARGINS ) );
	}

	CArray<CViewObjectList*, CViewObjectList*> arOL;
	CreateGroupViewObjectList( arOL );
	if( arOL.GetSize() < 1 )
		return true;

	CArray<CViewDescription*, CViewDescription*> arVD;

	CViewDescription* pCurVD = NULL;
	for( int i=0;i<arOL.GetSize();i++ )
	{
		if( arOL[i]->m_bNewGroup )
		{
			CViewDescription* pVD = new CViewDescription;
			pCurVD = pVD;
			arVD.Add( pVD );
		}

		if( pCurVD )
		{
			CViewObjectList* pvol = new CViewObjectList;
			pvol->CopyFrom( arOL[i] );
			pCurVD->m_arViewObject.Add( pvol );
		}

	}

	for( i=0;i<arOL.GetSize();i++)
		delete arOL[i];

	arOL.RemoveAll();

	if( arVD.GetSize() < 1 )
		return false;



	CRect rcWorkArea = GetReportWorkArea( form );

	int nDelta = D2V( 5 ); //5 mm
	rcWorkArea.DeflateRect( nDelta, nDelta, nDelta, nDelta );	

	int nCurY = rcWorkArea.top;
	int nCurRow = 0;
	int nSetHeight = rcWorkArea.Height( ) / 10; //50 mm

	for( i=0;i<arVD.GetSize();i++ )
	{
		CViewDescription* pvd = arVD[i];

		IUnknown* punkData = NULL;
		IUnknown* punkCtrl = NULL;

		if( !CreateControl( &punkData, &punkCtrl ) )
		{
			ASSERT( FALSE );
			return false;
		}

		sptrIReportCtrl sptrRC( punkData );
		sptrIViewCtrl sptrVAX( punkCtrl );

		if( punkData ) punkData->Release();
		if( punkCtrl ) punkCtrl->Release();

		if( sptrRC == NULL || sptrVAX == NULL )
			return false;


		
		CRect rcItem;
		rcItem.left	= rcWorkArea.left;
		rcItem.top	= nCurY;
		rcItem.right	= rcItem.left + rcWorkArea.Width() / 3;
		rcItem.bottom	= rcItem.top + nSetHeight;


		//Set AXView properties
		sptrVAX->SetAutoBuild( FALSE );
		sptrVAX->SetObjectTransformation( (short)otStretch );
		sptrVAX->SetUseActiveView( FALSE );
		sptrVAX->SetViewAutoAssigned( TRUE );		

		for( int j=0;j<pvd->m_arViewObject.GetSize();j++ )
		{
			CViewObjectList* pObject = pvd->m_arViewObject[j];
			sptrVAX->InsertAfter( 0, _bstr_t( (LPCSTR)pObject->m_strObjectName ), 
									FALSE, _bstr_t( (LPCSTR)pObject->m_strType ) );
		}	


		CSize sizeImage ( rcItem.Width(), rcItem.Height() );

		double	fShellDataZoom		= GetShellDataZoom();

		bool	bShellDataTrans		= UsePresetTransformation();
		short	nShellData			= GetPresetTransformation();


		bool bSetImageSize = false;
		double fDPI = 0;

		for( j=0;j<pvd->m_arViewObject.GetSize();j++ )
		{
			CViewObjectList* pObject = pvd->m_arViewObject[j];
			if( GetObjectSizeVTPixel( pObject->m_strObjectName, sizeImage, fDPI ) )
				bSetImageSize = true;		
		}

		if( bSetImageSize )
		{
			sptrVAX->SetObjectTransformation( bShellDataTrans ? nShellData : (short)otGrowTwoSide );		
			sptrVAX->SetDPI( fDPI );
			sptrVAX->SetZoom( fShellDataZoom );

			sizeImage.cx *= fShellDataZoom;
			sizeImage.cy *= fShellDataZoom;
		}
		

		CSize sizeSet;
		
		if( bShellDataTrans  && nShellData == otStretch /*paul fix 27.04.2002*/ )
			sizeSet = sizeImage;
		else
		{
			sizeSet = CSize( rcWorkArea.Width(), rcWorkArea.Width() );
			sizeSet = ::ThumbnailGetRatioSize( sizeSet, sizeImage );
		}

		double fZoom = (double)sizeSet.cx / (double)sizeSet.cy;


		if( sizeSet.cx >= rcWorkArea.Width() )
		{
			sizeSet.cx = rcWorkArea.Width();
			sizeSet.cy = (double)sizeSet.cx / fZoom;
		}

		
		if( sizeSet.cy >= rcWorkArea.Height() )
		{
			sizeSet.cy = rcWorkArea.bottom - nCurY;
			sizeSet.cx = (double)sizeSet.cy * fZoom;
		}

		rcItem.right  = rcItem.left + sizeSet.cx;
		rcItem.bottom = rcItem.top  + sizeSet.cy;



		if( rcItem.bottom >= rcWorkArea.bottom )
		{
			nCurY = rcWorkArea.top;
			nCurRow++;
			rcItem.top = nCurY;
			rcItem.bottom = rcItem.top + sizeSet.cy;
		}

		nCurY += rcItem.Height() + nDelta;



		sptrRC->SetDesignPosition( rcItem );		

		sptrRC->SetDesignOwnerPage( 0 );
		sptrRC->SetDesignColRow( 0, nCurRow );

		::StoreContainerToDataObject( sptrRC, sptrVAX ); 
		StoreCtrlToForm( sptrRC, form );
	}


	for( i=0;i<arVD.GetSize();i++ )
		delete arVD[i];

	arVD.RemoveAll();

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CReportConstructor::FillOut_Gallery( sptrIReportForm form )
{
	if( form == NULL )
		return false;				   

	if( UsePresetMargins() )
	{		
		form->SetPaperField( GetGlobalPaperMargins( GALLERY_MARGINS ) );
	}


	CArray<CViewObjectList*, CViewObjectList*> arOL;
	CreateGroupViewObjectList( arOL );
	if( arOL.GetSize() < 1 )
		return true;

	CArray<CViewDescription*, CViewDescription*> arVD;

	CViewDescription* pCurVD = NULL;
	for( int i=0;i<arOL.GetSize();i++ )
	{
		if( arOL[i]->m_bNewGroup )
		{
			CViewDescription* pVD = new CViewDescription;
			pCurVD = pVD;
			arVD.Add( pVD );
		}

		if( pCurVD )
		{
			CViewObjectList* pvol = new CViewObjectList;
			pvol->CopyFrom( arOL[i] );
			pCurVD->m_arViewObject.Add( pvol );
		}

	}


	for( i=0;i<arOL.GetSize();i++)
		delete arOL[i];

	arOL.RemoveAll();

	if( arVD.GetSize() < 1 )
		return false;




	CRect rcWorkArea = GetReportWorkArea( form );

	int nHorzItemCount	= ::GetValueInt( ::GetAppUnknown(), GENERATE_REPORT_SECTION, GALLERY_HORZ_ITEM_COUNT, 2 );
	if( nHorzItemCount < 1 )
		nHorzItemCount = 2;

	int nVertItemCount	= arVD.GetSize() / nHorzItemCount +
		( ( (arVD.GetSize() ) % nHorzItemCount ) ? 1 : 0 );

	if( nVertItemCount < 1 )
		nVertItemCount = 1;


	int nDelta = D2V( 5 ); //5 mm
	rcWorkArea.DeflateRect( nDelta, nDelta, nDelta, nDelta );	

	int nItemWidth	= rcWorkArea.Width() / nHorzItemCount;
	int nItemHeight = rcWorkArea.Height() / nVertItemCount;


	for( i=0;i<arVD.GetSize();i++ )
	{

		CViewDescription* pvd = arVD[i];

		int iRow, iCol;

		iRow = i / nHorzItemCount;
		iCol = i - iRow * nHorzItemCount;
		

		IUnknown* punkData = NULL;
		IUnknown* punkCtrl = NULL;

		if( !CreateControl( &punkData, &punkCtrl ) )
		{
			ASSERT( FALSE );
			return false;
		}

		sptrIReportCtrl sptrRC( punkData );
		sptrIViewCtrl sptrVAX( punkCtrl );

		if( punkData ) punkData->Release();
		if( punkCtrl ) punkCtrl->Release();

		if( sptrRC == NULL || sptrVAX == NULL )
			return false;

		CRect rcItem;
		rcItem.left	= rcWorkArea.left + iCol * nItemWidth;
		rcItem.top	= rcWorkArea.top  + iRow * nItemHeight;
		rcItem.right	= rcItem.left + nItemWidth  - nDelta;
		rcItem.bottom	= rcItem.top  + nItemHeight - nDelta;


		//Set AXView properties
		sptrVAX->SetAutoBuild( FALSE );
		sptrVAX->SetObjectTransformation( /*UsePresetTransformation() ? GetPresetTransformation(): */(short)otGrowTwoSide );
		sptrVAX->SetUseActiveView( FALSE );
		sptrVAX->SetViewAutoAssigned( TRUE );		

		for( int j=0;j<pvd->m_arViewObject.GetSize();j++ )
		{
			CViewObjectList* pObject = pvd->m_arViewObject[j];
			sptrVAX->InsertAfter( 0, _bstr_t( (LPCSTR)pObject->m_strObjectName ), 
									FALSE, _bstr_t( (LPCSTR)pObject->m_strType ) );
		}	

		sptrRC->SetDesignPosition( rcItem );		

		sptrRC->SetDesignOwnerPage( 0 );
		sptrRC->SetDesignColRow( 0, 0 );

		::StoreContainerToDataObject( sptrRC, sptrVAX ); 
		StoreCtrlToForm( sptrRC, form );
	}



	for( i=0;i<arVD.GetSize();i++ )
		delete arVD[i];

	arVD.RemoveAll();
	

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CReportConstructor::FillOut_FromSplitter( sptrIReportForm form )
{
	if( form == NULL )
		return false;

	if( UsePresetMargins() )
	{		
		form->SetPaperField( GetGlobalPaperMargins( FROM_SPLITTER_MARGINS ) );
	}


	int nViewCount = g_ReportConstructor.GetSplitterViewDescriptionCount();
	
	int nSplitterRowCount = g_ReportConstructor.GetSplitterRowCount();
	int nSplitterColCount = g_ReportConstructor.GetSplitterColCount();	

	CRect rcWorkArea = GetReportWorkArea( form );

	int nItemWidth	= rcWorkArea.Width()  / nSplitterColCount;
	int nItemHeight	= rcWorkArea.Height() / nSplitterRowCount;

	int nDelta = D2V( 5 ); //5 mm
	rcWorkArea.DeflateRect( nDelta, nDelta, nDelta, nDelta );	

	for( int iCol=0;iCol<nSplitterColCount;iCol++ ) 
	{
		for( int iRow=0;iRow<nSplitterRowCount;iRow++ ) 
		{
			for( int i=0;i<nViewCount;i++ )
			{
				CViewDescription* pvd = g_ReportConstructor.GetSplitterViewDescriptionItem( i );
				if( pvd == NULL )
					continue;

				if( !(pvd->GetCol() == iCol && pvd->GetRow() == iRow) )
					continue;

				IUnknown* punkData = NULL;
				IUnknown* punkCtrl = NULL;

				if( !CreateControl( &punkData, &punkCtrl ) )
				{
					ASSERT( FALSE );
					return false;
				}

				sptrIReportCtrl sptrRC( punkData );
				sptrIViewCtrl sptrVAX( punkCtrl );

				if( punkData ) punkData->Release();
				if( punkCtrl ) punkCtrl->Release();

				if( sptrRC == NULL || sptrVAX == NULL )
					return false;

				CRect rcItem;
				rcItem.left	= rcWorkArea.left + iCol * nItemWidth;
				rcItem.top	= rcWorkArea.top  + iRow * nItemHeight;
				rcItem.right	= rcItem.left + nItemWidth  - nDelta;
				rcItem.bottom	= rcItem.top  + nItemHeight - nDelta;


				//Set AXView properties
				sptrVAX->SetAutoBuild( FALSE );
				sptrVAX->SetObjectTransformation( otGrowTwoSide/*UsePresetTransformation() ? GetPresetTransformation(): (short)otGrowTwoSide*/ );
				sptrVAX->SetUseActiveView( FALSE );
				sptrVAX->SetViewAutoAssigned( FALSE );
				sptrVAX->SetViewProgID( _bstr_t( pvd->GetViewProgID() ) );

				for( int j=0;j<pvd->m_arViewObject.GetSize();j++ )
				{
					CViewObjectList* pObject = pvd->m_arViewObject[j];
					sptrVAX->InsertAfter( 0, _bstr_t( (LPCSTR)pObject->m_strObjectName ), 
											FALSE, _bstr_t( (LPCSTR)pObject->m_strType ) );
				}	

				sptrRC->SetDesignPosition( rcItem );		

				sptrRC->SetDesignOwnerPage( 0 );
				sptrRC->SetDesignColRow( 0, 0 );

				::StoreContainerToDataObject( sptrRC, sptrVAX ); 
				StoreCtrlToForm( sptrRC, form );
			}
		}
	}	
	

	return true;
}

//////////////////////////////////////////////////////////////////////
IUnknown* CReportConstructor::CreateReport()
{
	IUnknown* punkForm = NULL;
	punkForm = ::CreateTypedObject( szTypeReportForm );

	CRect rcFields;
	

	CRect rcPrnMargins = g_prnSetup.GetPrinterMargins( punkForm );
	sptrIReportForm	sptrForm( punkForm );
	if( sptrForm != NULL )
	{
			
		
		CRect paperAlignNew = CRect( 
					D2V(rcPrnMargins.left),
					D2V(rcPrnMargins.top),
					D2V(rcPrnMargins.right),
					D2V(rcPrnMargins.bottom)
					);
		sptrForm->SetPaperAlign( paperAlignNew );	
	}

	
	return punkForm;	
}

//////////////////////////////////////////////////////////////////////
IUnknown* CReportConstructor::CreateViewAXCtrl()
{

	CLSID	clsid;
	::ZeroMemory( &clsid, sizeof(clsid) );

	if( ::CLSIDFromProgID( _bstr_t(szViewAXProgID), &clsid ) == S_OK )
	{
		IUnknown* punkVAX = NULL;
		HRESULT hr = CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER,
										IID_IUnknown, (LPVOID*)&punkVAX);
		if(hr == S_OK)
			return punkVAX;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////
bool CReportConstructor::GetMatchViewProgID( CArray<CString, CString>& arrViewProgID, CString strObjectType )
{
	sptrIApplication sptrApp( ::GetAppUnknown() );
	long pos = 0;
	sptrApp->GetFirstDocTemplPosition(&pos);
	while(pos)
	{
		BSTR bstrName = 0;
		sptrApp->GetNextDocTempl(&pos, &bstrName, 0);
		CString strTempl(bstrName);
		::SysFreeString(bstrName);

		//if(punkObj)
		{
			sptrApp->GetFirstDocTemplPosition(&pos);

			while(pos)
			{
				sptrApp->GetNextDocTempl(&pos, &bstrName, 0);
				strTempl = bstrName;
				::SysFreeString(bstrName);

				_bstr_t bstrType( strObjectType );
				
			
				CCompRegistrator	rv( strTempl+"\\"+szPluginView );
				long nComps = rv.GetRegistredComponentCount();
				for(long i = 0; i < nComps; i++)
				{
					CString strProgID = rv.GetComponentName(i);
					if( strProgID.IsEmpty() )
						continue;
					
					BSTR bstrProgID = strProgID.AllocSysString();
					IViewPtr sptrV(bstrProgID);
					::SysFreeString(bstrProgID);

					DWORD dwMatch = mvNone;					
					sptrV->GetMatchType( bstrType, &dwMatch );

					if( dwMatch > mvNone )
					{
						BSTR bstrName = 0;
						INamedObject2Ptr sptrNO(sptrV);
						sptrNO->GetName(&bstrName);						
						arrViewProgID.Add( bstrName );
						::SysFreeString(bstrName);						
					}
				}
			}

			break;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
IUnknown* CReportConstructor::CreateViewAXDataCtrl()
{
	IUnknown* punkCtrl = NULL;
	punkCtrl = ::CreateTypedObject( szTypeReportCtrl );
	sptrIActiveXCtrl	sptrC( punkCtrl );
	if( sptrC )
		sptrC->SetProgID( _bstr_t(szViewAXProgID) );

	return punkCtrl;	
}

//////////////////////////////////////////////////////////////////////
IUnknown* CReportConstructor::GetAXCtrlFromDataCtrl( IUnknown* punkViewAXDataCtrl )
{
	IUnknown* punkViewAxCtrl = CreateViewAXCtrl();
	if( punkViewAxCtrl == NULL )
		return NULL;

	IUnknownPtr ptrViewAxCtrl = punkViewAxCtrl;
	punkViewAxCtrl->Release();

	sptrIActiveXCtrl sptrAXCtrl( punkViewAXDataCtrl );
	if( sptrAXCtrl == NULL )
		return NULL;

	BSTR bstrProgID;
	ISOK( sptrAXCtrl->GetProgID(&bstrProgID) );
	CString strProgID = bstrProgID;
	::SysFreeString(bstrProgID);

	if( strProgID != szViewAXProgID )
		return NULL;

	::RestoreContainerFromDataObject( punkViewAXDataCtrl, ptrViewAxCtrl );

	ptrViewAxCtrl->AddRef();

	IUnknown* punkAX = ptrViewAxCtrl;

	return punkAX;

}

//////////////////////////////////////////////////////////////////////
bool CReportConstructor::SetAXCtrlToDataCtrl( IUnknown* punkViewAXDataCtrl, IUnknown* punkViewAXCtrl )
{
	sptrIActiveXCtrl sptrAXCtrl( punkViewAXDataCtrl );
	if( sptrAXCtrl == NULL )
		return false;

	sptrAXCtrl->SetProgID( _bstr_t(szViewAXProgID) );

	::StoreContainerToDataObject( punkViewAXDataCtrl, punkViewAXCtrl ); 

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CReportConstructor::CreateControl( IUnknown** ppunkData, IUnknown** ppunkCtrl )
{
	if( ppunkData == NULL || ppunkCtrl == NULL )
		return false;

	*ppunkData = NULL;
	*ppunkCtrl = NULL;

	IUnknown* punk  = NULL;
	punk = CreateViewAXDataCtrl();
	if( punk == NULL )
		return false;

	IUnknownPtr ptrData = punk;
	punk->Release();
	punk = NULL;

	punk = GetAXCtrlFromDataCtrl( ptrData ) ;
	if( punk == NULL )
		return false;

	IUnknownPtr ptrCtrl( punk );
	punk->Release();
	punk = NULL;

	ptrData->AddRef();
	*ppunkData = ptrData;

	ptrCtrl->AddRef();
	*ppunkCtrl = ptrCtrl;

	return true;
}

//////////////////////////////////////////////////////////////////////
IUnknown* CReportConstructor::GetDocumentReportCopyFrom()
{
	if( m_sptrDocSiteCopyFrom == NULL )
		return NULL;

	m_sptrDocSiteCopyFrom->AddRef();
	return (IUnknown*)m_sptrDocSiteCopyFrom;
}


//////////////////////////////////////////////////////////////////////
bool CReportConstructor::StoreCtrlToForm( IUnknown* punkCtrl, IUnknown* punkForm )
{
	sptrINamedDataObject2 sptrNDO2(punkCtrl);
	if( sptrNDO2 )
	{
		sptrNDO2->SetParent( punkForm, 0 );
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
CRect CReportConstructor::GetReportWorkArea( IUnknown* punkReport )
{
	sptrIReportForm sptrReport( punkReport );
	if( sptrReport == NULL )
		return NORECT;
		
	CSize sizePaper;
	sptrReport->GetPaperSize( &sizePaper );

	CRect rcFields;
	sptrReport->GetPaperField( &rcFields );

	CRect rc;
	rc			= rcFields;

	rc.right	= sizePaper.cx - rcFields.right;
	rc.bottom	= sizePaper.cy - rcFields.bottom;

	return rc;
}

//////////////////////////////////////////////////////////////////////

IUnknown* GetSplitterFromDocumentSite( IUnknown* punkDS )
{
	sptrIDocumentSite sptrDS( punkDS );
	if( sptrDS == NULL )
		return NULL;

	IUnknown* punk = NULL;
	sptrDS->GetActiveView( &punk );
	if( punk == NULL )
		return NULL;

	sptrIView sptrV( punk );
	punk->Release();
	punk = NULL;

	if( sptrV == NULL )
		return NULL;

	sptrIViewSite	sptrVS( sptrV );
	if( sptrVS == NULL )
		return NULL;

	IUnknown*	punkFrame = NULL;
	sptrVS->GetFrameWindow( &punkFrame );
	sptrIFrameWindow	sptrF( punkFrame ) ;
	if ( punkFrame )
		punkFrame->Release();

	if( sptrF == NULL )
		return NULL;

	IUnknown* punkSplitter = NULL;		
	sptrF->GetSplitter( &punkSplitter );

	return punkSplitter;
}

////////////////////////////////////////////////////////////////////////////
bool CReportConstructor::GetAvailableViewProgID( CArray<CString, CString>& arViewList )
{
	sptrIApplication sptrApp( ::GetAppUnknown() );
	if( sptrApp == NULL )
		return false;

	long pos = 0;
	sptrApp->GetFirstDocTemplPosition(&pos);
	while(pos)
	{
		BSTR bstrName = 0;
		sptrApp->GetNextDocTempl(&pos, &bstrName, 0);
		CString strTempl(bstrName);
		::SysFreeString(bstrName);
		
		//if(punkObj)
		{
			sptrApp->GetFirstDocTemplPosition(&pos);

			while(pos)
			{

				sptrApp->GetNextDocTempl(&pos, &bstrName, 0);
				strTempl = bstrName;
				::SysFreeString(bstrName);				
				
			
				CCompRegistrator	rv( strTempl+"\\"+szPluginView );
				long nComps = rv.GetRegistredComponentCount();
				for(long i = 0; i < nComps; i++)
				{
					CString strProgID = rv.GetComponentName(i);
					if( !strProgID.IsEmpty() )
						arViewList.Add( rv.GetComponentName(i) );					
				}
			}
		}
	}

	return true;	
}

////////////////////////////////////////////////////////////////////////////
bool CReportConstructor::GetObjectSizeVTPixel( CString strObjectName, CSize& size, double& fDPI )
{
	IUnknown* punkObject = ::FindObjectByName( m_sptrDocSite, strObjectName );
	if( punkObject == NULL )
		return false;

	CImageWrp image( punkObject );	
	punkObject->Release();

	if( image == NULL )
		return false;

	CRect rcImage = image.GetRect();

	double	fCalibr = 0;
	GetCalibration( image, &fCalibr, 0 );

	double fDisplayPixPMM = CPrintView::GetDisplayPixelPMM(); //3.0

	if( fDisplayPixPMM <= 0.0 )
		return false;

	if( !::IsCalibrationForPrintEnable() )
	{		
		fDPI = fDisplayPixPMM * MeasureUnitTable::mmPerInch;
		size.cx = D2V( (double)rcImage.Width() / fDisplayPixPMM );
		size.cy = D2V( (double)rcImage.Height() / fDisplayPixPMM );
		return true;		
	}

/*
	double fMeterPerUnit;
	GetCalibrationUnit( &fMeterPerUnit, 0 );
	

	if( fMeterPerUnit <= 0.0 )
	{
		//~96 DPI
		size.cx = D2V( (double)rcImage.Width() / fDisplayPixPMM );
		size.cy = D2V( (double)rcImage.Height() / fDisplayPixPMM );	
		return true;
	}
	*/

	if( fCalibr <= 0.0 )
	{
		size.cx = D2V( (double)rcImage.Width() / fDisplayPixPMM );
		size.cy = D2V( (double)rcImage.Height() / fDisplayPixPMM );	
		return true;
	}

	//Image size in unit
	double fW = ( (double)rcImage.Width() ) * fCalibr;
	double fH = ( (double)rcImage.Height() ) * fCalibr;

	/*
	//Image size in meter
	fW *= fMeterPerUnit;
	fH *= fMeterPerUnit;
	*/

	//Image size in mm
	fW *= 1000.0;
	fH *= 1000.0;


	//calc DPI
	fDPI = ( (double)rcImage.Width() ) / fW * MeasureUnitTable::mmPerInch;

	//Image size in VT_PIXEL( mm * 100 )
	fW = D2V( fW );
	fH = D2V( fH );

	size.cx = (LONG)fW;
	size.cy = (LONG)fH;

	return true;
}

////////////////////////////////////////////////////////////////////////////
double CReportConstructor::GetShellDataZoom()
{
	double fZoom = 1;
	if( UsePresetZoom() )
		fZoom = GetPresetZoom();
	
	if( fZoom <= 0.0 )
		fZoom = 1.0;

	return fZoom;
}