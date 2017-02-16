#include "StdAfx.h"
#include "CompareImagesDlg.h"
#include "resource.h"
#include "\vt5\awin\misc_list.h"

SIZE CDialogViewFrame::m_size = {0};
POINT CDialogViewFrame::m_ptScroll = {0};
double CDialogViewFrame::m_fZoom = 1;
RECT CDialogViewFrame::m_rcBound = {0};
bool CDialogViewFrame::m_bAutoScrollMode = true;

HANDLE CDialogViewFrame::m_hScrollVert = 0;
HANDLE CDialogViewFrame::m_hScrollHorz = 0;

CCompareImagesDlg::CCompareImagesDlg() : dlg_impl( IDD_COMPAREIMAGES )
{
	m_strLang = ::GetValueString( ::GetAppUnknown(), "\\General", "Language", "ru" );

	m_strClassFile = ::GetValueString( ::GetAppUnknown(), "\\Classes", "ClassFile", "" );
	m_strClassUnk = ::GetValueString( ::GetAppUnknown(), "\\Classes", "UnkClassColor", "(255, 0, 0)" );

	m_lEnableSelection = ::GetValueInt( ::GetAppUnknown(), "\\ImageView", "EnableSelection", 1 );

	m_lLastHLPerc    = ::GetValueInt( ::GetAppUnknown(), "\\ImageView", "HilightPercent", 30 );
	m_lLastHLEnable = ::GetValueInt( ::GetAppUnknown(), "\\ImageView", "HilightEnabled", 1 );

	::SetValue( ::GetAppUnknown(), "\\Classes", "ClassFile", "" );
	::SetValue( ::GetAppUnknown(), "\\Classes", "UnkClassColor", "(255, 0, 0)" );
	::SetValue( ::GetAppUnknown(), "\\ImageView", "EnableSelection", 0L );

	::SetValue( ::GetAppUnknown(), "\\ImageView", "HilightPercent", (long)30 );
	::SetValue( ::GetAppUnknown(), "\\ImageView", "HilightEnabled", (long)1 );

	m_lMaxImage = m_lCurrentImage = 0;
	m_pbShowStates = 0;
	m_pframe = new CDialogViewFrame;
	m_pframe2 = new CDialogViewFrame;
}

CCompareImagesDlg::~CCompareImagesDlg()
{
	::SetValue( ::GetAppUnknown(), "\\ImageView", "HilightPercent", m_lLastHLPerc );
	::SetValue( ::GetAppUnknown(), "\\ImageView", "HilightEnabled", m_lLastHLEnable );

	::SetValue( ::GetAppUnknown(), "\\Classes", "ClassFile", m_strClassFile );
	::SetValue( ::GetAppUnknown(), "\\Classes", "UnkClassColor", m_strClassUnk );
	::SetValue( ::GetAppUnknown(), "\\ImageView", "EnableSelection", m_lEnableSelection );

	if( m_pbShowStates )
		delete []m_pbShowStates; m_pbShowStates = 0;

	if( m_pframe )
		delete []m_pframe; m_pframe = 0;

	if( m_pframe2 )
		delete []m_pframe2; m_pframe2 = 0;
}

long CCompareImagesDlg::on_initdialog()
{
	long lRes = __super::on_initdialog();

	m_pframe->SetScroll( get_dlg_item( IDC_SCROLLBAR1 ), SB_VERT );
	m_pframe->SetScroll( get_dlg_item( IDC_SCROLLBAR2 ), SB_HORZ );
	

	CLSID	clsidView = {0}, clsidSplitter = {0},  clsidContext = {0}, clsidData = {0};

	if( ::CLSIDFromProgID( _bstr_t( szImageViewProgID ), &clsidView ) != S_OK  )
		return 0;

	if( ::CLSIDFromProgID( _bstr_t( szContextProgID ), &clsidContext ) != S_OK  )
		return 0;

	if( ::CLSIDFromProgID( _bstr_t( "Data.NamedData" ), &clsidData ) != S_OK  )
		return 0;

	IUnknown *punk = 0;


	if( ::CoCreateInstance( clsidData, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk ) != S_OK )
		return 0;

	m_pframe->SetData( punk );
	m_pframe2->SetData( punk );

	IUnknownPtr sptrDoc = punk;
	if( punk )
		punk->Release(); punk = 0;

	if( sptrDoc == 0 )
		return 0;

	if( ::CoCreateInstance( clsidData, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk ) != S_OK )
		return 0;

	IUnknownPtr sptrDoc2 = punk;
	if( punk )
		punk->Release(); punk = 0;

	if( sptrDoc2 == 0 )
		return 0;
	IFileDataObjectPtr	sptrFF = sptrDoc;

	if( sptrFF == 0 )
		return 0;


	CString strNum, strPath = (char *)_bstr_t( m_pErrDescr->bstrPath );

	if( (int)m_pErrDescr->nTryIndex != 0 )
		strNum.Format( ".%x", (int)m_pErrDescr->nTryIndex );

 	sptrFF->LoadFile( _bstr_t( strPath + "\\TDC_" + m_strLang + strNum + ".image" ) );

	IFileDataObjectPtr	sptrFF2 = sptrDoc2;

	if( sptrFF2 == 0 )
		return 0;

	sptrFF2->LoadFile( _bstr_t( strPath + "\\TDS_" + m_strLang + strNum + ".image" ) );

	if( ::CoCreateInstance( clsidView, m_pframe->unknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk ) != S_OK )
		return 0;

	m_pframe->SetView( punk );

	if( punk )
		punk->Release(); punk = 0;

	if( ::CoCreateInstance( clsidView, m_pframe2->unknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk ) != S_OK )
		return 0;

	m_pframe2->SetView( punk );

	if( punk )
		punk->Release(); punk = 0;

	IViewPtr sptrView = m_pframe->GetView();

	if( sptrView == 0 )
		return 0;

	if( sptrView->Init( sptrDoc, 0 ) != S_OK )
		return 0;

	IViewPtr sptrView2 = m_pframe2->GetView();

 	if( sptrView2 == 0 )
		return 0;

	if( sptrView2->Init( sptrDoc, 0 ) != S_OK )
		return 0;

	if( ::CoCreateInstance( clsidContext, m_pframe->unknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk ) != S_OK )
		return 0;

	m_pframe->SetContext( punk );

	if( punk )
		punk->Release(); punk = 0;

	if( ::CoCreateInstance( clsidContext, m_pframe2->unknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk ) != S_OK )
		return 0;

	m_pframe2->SetContext( punk );

	if( punk )
		punk->Release(); punk = 0;

	if( ::CoCreateInstance( clsidContext, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk ) != S_OK )
		return 0;

	IDataContext2Ptr sptrContextTmp = punk;

	if( punk )
		punk->Release(); punk = 0;

	if( sptrContextTmp == 0 )
		return 0;

	IDataContext2Ptr sptrContext = m_pframe->GetContext();

	if( sptrContext == 0 )
		return 0;

	IDataContext2Ptr sptrContext2 = m_pframe2->GetContext();

	if( sptrContext2 == 0 )
		return 0;


	sptrContext->AttachData( sptrDoc );
	sptrContext2->AttachData( sptrDoc );

	sptrContextTmp->AttachData( sptrDoc2 );

	_bstr_t bstrImage = szArgumentTypeImage;

	IWindowPtr sptrWin = sptrView;

 	if( sptrWin == 0 )
		return 0;

	IWindowPtr sptrWin2 = sptrView2;

	if( sptrWin2 == 0 )
		return 0;


	RECT rcView = {0};
	::GetWindowRect( get_dlg_item( IDC_VIEW_HERE ), &rcView ) ;

	RECT rcView2 = {0};
	::GetWindowRect( get_dlg_item( IDC_VIEW_HERE2 ), &rcView2 ) ;

	::ScreenToClient( handle(), (LPPOINT)&rcView );
	::ScreenToClient( handle(), (LPPOINT)&rcView + 1 );

	::ScreenToClient( handle(), (LPPOINT)&rcView2 );
	::ScreenToClient( handle(), (LPPOINT)&rcView2 + 1 );

	::DestroyWindow( get_dlg_item( IDC_VIEW_HERE ) );
	::DestroyWindow( get_dlg_item( IDC_VIEW_HERE2 ) );

	if( sptrWin->CreateWnd( handle(), rcView, WS_VISIBLE | WS_CHILD | WS_VISIBLE | WS_TABSTOP, IDC_VIEW_HERE ) != S_OK )
		return 0;

	if( sptrWin2->CreateWnd( handle(), rcView2, WS_VISIBLE | WS_CHILD | WS_VISIBLE | WS_TABSTOP, IDC_VIEW_HERE2 ) != S_OK )
		return 0;

	TPOS lPos = 0;
	sptrContextTmp->GetFirstObjectPos( bstrImage, &lPos );

	_list_t< IUnknownPtr > m_list;
	while( lPos )
	{
		IUnknown *punkImage = 0;
		sptrContextTmp->GetNextObject(bstrImage, (LPOS*)&lPos, &punkImage);

		_bstr_t bstrName = ::GetObjectName( punkImage );

		CString strName = (char *)bstrName;

		if( strName.Find( "WndPhoto" ) == -1 )
		{
			if( punkImage )
				punkImage->Release();
			continue;
		}

		IUnknown *punkCloneImage = ::CreateTypedObject( szTypeImage );
		ICompatibleObject3Ptr sptrClone = punkCloneImage;

		if( punkCloneImage )
			punkCloneImage->Release();

		if( sptrClone )
		{
			sptrClone->CreateCompatibleObject( punkImage, 0, 0 );
			::SetObjectName( sptrClone, CString( "__" ) + (char *)bstrName );
			m_list.add_tail( sptrClone );
		}

		if( punkImage )
			punkImage->Release();
	}

	m_lMaxImage = m_pErrDescr->lszImages;

	::EnableWindow( get_dlg_item( IDC_BUTTON1 ) , m_lCurrentImage != 0 );
	::EnableWindow( get_dlg_item( IDC_BUTTON2 ) , m_lMaxImage > 1 && m_lCurrentImage < m_lMaxImage - 1 );

	
	m_pbShowStates = new bool[m_lMaxImage];
	::ZeroMemory( m_pbShowStates, sizeof( bool ) * m_lMaxImage );

	for( lPos = m_list.head(); lPos; lPos = m_list.next( lPos ) )
	{
		IUnknownPtr sptrO = m_list.get( lPos );
		::SetValue( sptrDoc, 0, 0, _variant_t( ( IUnknown *)sptrO ) );
	}

	m_list.clear();
	
	_redraw_images();

	sptrDoc2 = 0;
	sptrContextTmp->AttachData( 0 );
	sptrContextTmp = 0;

	init_resize();

	return lRes;
}

void CCompareImagesDlg::_execute_script( IUnknown *punk, BSTR bstrScript )
{
	IApplicationPtr sptrApp = ::GetAppUnknown();

	if( sptrApp )
	{
		IUnknown *punkTarget = 0;

		sptrApp->GetTargetManager( &punkTarget );
		
		IActionTargetManagerPtr sptrMan = punkTarget;
		punkTarget->Release(); punkTarget = 0;

		_bstr_t	bstrTarget = szTargetAnydoc;

		IUnknown *punkPrevTarget = 0;

		sptrMan->GetTarget( bstrTarget, &punkPrevTarget );
		
		if( punkPrevTarget )
			sptrMan->UnRegisterTarget( punkPrevTarget );

		sptrMan->RegisterTarget( punk, bstrTarget );
		sptrMan->ActivateTarget( punk );

		::ExecuteScript( bstrScript );

		sptrMan->UnRegisterTarget( punk );

		if( punkPrevTarget )
			sptrMan->RegisterTarget( punkPrevTarget, bstrTarget );
		if( punkPrevTarget )
			sptrMan->ActivateTarget( punkPrevTarget );

		if( punkPrevTarget )
			punkPrevTarget->Release();
	}
}

void CCompareImagesDlg::_redraw_images()
{

	::EnableWindow( get_dlg_item( IDC_BUTTON1 ) , 0 );
	::EnableWindow( get_dlg_item( IDC_BUTTON2 ) , 0 );

	_bstr_t bstrImage = szArgumentTypeImage;
	_bstr_t bstrList = szArgumentTypeObjectList;

	if( !m_pbShowStates[m_lCurrentImage] )
	{
		long lPos = 0;

		IDataContext2Ptr sptrContext = m_pframe->GetContext();
 		sptrContext->GetFirstObjectPos( bstrImage, &lPos );

		bool bFound = false;
		while( lPos )
		{
			IUnknown *punkImage = 0;
			sptrContext->GetNextObject( bstrImage, &lPos, &punkImage );

			_bstr_t bstrName = ::GetObjectName( punkImage );

			if( bstrName == _bstr_t( m_pErrDescr->pbstrImageNames[m_lCurrentImage] ) )
				bFound = true;

			if( bFound )
				sptrContext->SetActiveObject( bstrImage, punkImage, 0 );
			
			if( punkImage )
				punkImage->Release();

			if( bFound )
				break;
		}

		if( !bFound )
			sptrContext->SetActiveObject( bstrImage, 0, 0 );

		bool bImageA = bFound;

		IDataContext2Ptr sptrContext2 = m_pframe2->GetContext();
		
		lPos = 0;
		sptrContext2->GetFirstObjectPos( bstrImage, &lPos );

		bFound = false;

		while( lPos )
		{
			IUnknown *punkImage = 0;
			sptrContext2->GetNextObject( bstrImage, &lPos, &punkImage );
													
			_bstr_t bstrName = ::GetObjectName( punkImage );

			if( bstrName == ( _bstr_t( "__" ) + m_pErrDescr->pbstrImageNames[m_lCurrentImage] ) )
				bFound = true;

			if( bFound )
				sptrContext2->SetActiveObject( bstrImage, punkImage, 0 );
			
			if( punkImage )
				punkImage->Release();

			if( bFound )
				break;
		}
	 
		if( !bFound )
			sptrContext2->SetActiveObject( bstrImage, 0, 0 );

		bool bImageB = bFound;

 		long lDelete = ::GetValueInt( ::GetAppUnknown(), "\\General", "DeleteFilterArgs", 0 );
		::SetValue( ::GetAppUnknown(), "\\General", "DeleteFilterArgs", 0L );


		CString strScript;

		// making image substraction for both context
 		strScript = "ImageSubEx \"";
		strScript += (char*)_bstr_t( m_pErrDescr->pbstrImageNames[m_lCurrentImage] );
		strScript += "\", \"";
		strScript += "__";
		strScript += (char*)_bstr_t( m_pErrDescr->pbstrImageNames[m_lCurrentImage] );
		strScript += "\", \"res_";
		strScript += (char*)_bstr_t( m_pErrDescr->pbstrImageNames[m_lCurrentImage] );
		strScript += "\"";

		if( bImageA && bImageB )
	 		_execute_script( m_pframe->GetData(), _bstr_t( strScript ) );

		::SetValue( ::GetAppUnknown(), "\\General", "DeleteFilterArgs", _variant_t( (int)lDelete ) );

		// activate last image in context1
		lPos = 0;
 		sptrContext->GetLastObjectPos( bstrImage, &lPos );
		if( bImageA && lPos )
		{
			IUnknown *punkImage = 0;
			sptrContext->GetNextObject( bstrImage, &lPos, &punkImage );

			_bstr_t bstrName = ::GetObjectName( punkImage );
			if( punkImage )
			{
				sptrContext->SetActiveObject( bstrImage, punkImage, 0 );
				punkImage->Release();
			}
		}

		// activate last image in context2
		lPos = 0;
 		sptrContext2->GetLastObjectPos( bstrImage, &lPos );
		if( bImageB && lPos )
		{
			IUnknown *punkImage = 0;
			sptrContext2->GetNextObject( bstrImage, &lPos, &punkImage );

			_bstr_t bstrName = ::GetObjectName( punkImage );
			if( punkImage )
			{
				sptrContext2->SetActiveObject( bstrImage, punkImage, 0 );
				punkImage->Release();
			}
		}

		// making object for both context
		strScript = "GrayThresholding 1,256, \"\", \"obl_";
		strScript += (char*)_bstr_t( m_pErrDescr->pbstrImageNames[m_lCurrentImage] );
		strScript += "\"";

		if( bImageA && bImageB )
   			_execute_script( m_pframe->GetData(), _bstr_t( strScript ) );

		strScript = "GrayThresholding 1,256, \"\", \"obl2_";
		strScript += (char*)_bstr_t( m_pErrDescr->pbstrImageNames[m_lCurrentImage] );
		strScript += "\"";

		if( bImageA && bImageB )
	   		_execute_script( m_pframe->GetData(), _bstr_t( strScript ) );
		
		// activate good photo in context1 again
		lPos = 0;
		sptrContext->GetFirstObjectPos( bstrImage, &lPos );

 		while( bImageA && lPos )
		{
			IUnknown *punkImage = 0;
			sptrContext->GetNextObject( bstrImage, &lPos, &punkImage );

			bool bFound = false;
			_bstr_t bstrName = ::GetObjectName( punkImage );

			if( bstrName == _bstr_t( m_pErrDescr->pbstrImageNames[m_lCurrentImage] ) )
				bFound = true;

			if( bFound )
				sptrContext->SetActiveObject( bstrImage, punkImage, 0 );
			
			if( punkImage )
				punkImage->Release();

			if( bFound )
				break;
		}
		// activate last object list in context1
		// activate good photo in context1 again
		lPos = 0;
		sptrContext->GetFirstObjectPos( bstrList, &lPos );

		while( bImageB && lPos )
		{
			IUnknown *punkList = 0;
			sptrContext->GetNextObject( bstrList, &lPos, &punkList );
													
			bool bFound = false;
			_bstr_t bstrName = ::GetObjectName( punkList );

			if( bstrName == ( _bstr_t( "obl_" ) + m_pErrDescr->pbstrImageNames[m_lCurrentImage] ) )
				bFound = true;

			if( bFound )
				sptrContext->SetActiveObject( bstrList, punkList, 0 );
			
			if( punkList )
				punkList->Release();

			if( bFound )
				break;
		}

		strScript = "MakeObjectsVirtual\n";
		strScript += "FillHoles -1";

		if( bImageA && bImageB )
	  		_execute_script( m_pframe->GetData(), _bstr_t( strScript ) );

		// activate good photo in context1 again
		lPos = 0;
		sptrContext->GetFirstObjectPos( bstrImage, &lPos );

		while( bImageA && lPos )
		{
			IUnknown *punkImage = 0;
			sptrContext->GetNextObject( bstrImage, &lPos, &punkImage );
													
			bool bFound = false;
			_bstr_t bstrName = ::GetObjectName( punkImage );

			if( bstrName == ( _bstr_t( "__" ) + m_pErrDescr->pbstrImageNames[m_lCurrentImage] ) )
				bFound = true;

			if( bFound )
				sptrContext->SetActiveObject( bstrImage, punkImage, 0 );
			
			if( punkImage )
				punkImage->Release();

			if( bFound )
				break;
		}

		// activate good photo in context1 again
		lPos = 0;
		sptrContext->GetFirstObjectPos( bstrList, &lPos );

		while( bImageB && lPos )
		{
			IUnknown *punkList = 0;
			sptrContext->GetNextObject( bstrList, &lPos, &punkList );
													
			bool bFound = false;
			_bstr_t bstrName = ::GetObjectName( punkList );

			if( bstrName == ( _bstr_t( "obl2_" ) + m_pErrDescr->pbstrImageNames[m_lCurrentImage] ) )
				bFound = true;

			if( bFound )
				sptrContext->SetActiveObject( bstrList, punkList, 0 );
			
			if( punkList )
				punkList->Release();

			if( bFound )
				break;
		}

		if( bImageA && bImageB )
	  		_execute_script( m_pframe->GetData(), _bstr_t( strScript ) );

		lPos = 0;
 		sptrContext->GetFirstObjectPos( bstrImage, &lPos );

		bFound = false;
		while( bImageA && lPos )
		{
			IUnknown *punkImage = 0;
			sptrContext->GetNextObject( bstrImage, &lPos, &punkImage );

			_bstr_t bstrName = ::GetObjectName( punkImage );

			if( bstrName == _bstr_t( m_pErrDescr->pbstrImageNames[m_lCurrentImage] ) )
				bFound = true;

			if( bFound )
				sptrContext->SetActiveObject( bstrImage, punkImage, 0 );
			
			if( punkImage )
				punkImage->Release();

			if( bFound )
				break;
		}

		bFound = false;
		sptrContext2->GetFirstObjectPos( bstrImage, &lPos );

		while( bImageB && lPos )
		{
			IUnknown *punkImage = 0;
			sptrContext2->GetNextObject( bstrImage, &lPos, &punkImage );
													
			_bstr_t bstrName = ::GetObjectName( punkImage );

			if( bstrName == ( _bstr_t( "__" ) + m_pErrDescr->pbstrImageNames[m_lCurrentImage] ) )
				bFound = true;

			if( bFound )
				sptrContext2->SetActiveObject( bstrImage, punkImage, 0 );
			
			if( punkImage )
				punkImage->Release();

			if( bFound )
				break;
		}

		lPos = 0;
 		sptrContext->GetFirstObjectPos( bstrList, &lPos );

		while( bImageA && lPos )
		{
			IUnknown *punkList = 0;
			sptrContext->GetNextObject( bstrList, &lPos, &punkList );

			bool bFound = false;
			_bstr_t bstrName = ::GetObjectName( punkList );

			if( bstrName == _bstr_t( "obl_" ) + _bstr_t( m_pErrDescr->pbstrImageNames[m_lCurrentImage] ) )
				bFound = true;

			if( bFound )
				sptrContext->SetActiveObject( bstrList, punkList, 0 );
			
			if( punkList )
				punkList->Release();

			if( bFound )
				break;
		}

		lPos = 0;
		sptrContext2->GetFirstObjectPos( bstrList, &lPos );

		while( bImageB && lPos )
		{
			IUnknown *punkList = 0;
			sptrContext2->GetNextObject( bstrList, &lPos, &punkList );
													
			bool bFound = false;
			_bstr_t bstrName = ::GetObjectName( punkList );

			if( bstrName == ( _bstr_t( "obl2_" ) + m_pErrDescr->pbstrImageNames[m_lCurrentImage] ) )
				bFound = true;

			if( bFound )
				sptrContext2->SetActiveObject( bstrList, punkList, 0 );
			
			if( punkList )
				punkList->Release();

			if( bFound )
				break;
		}
		m_pbShowStates[m_lCurrentImage] = true;
	}
	else
	{
		IDataContext2Ptr sptrContext = m_pframe->GetContext();
		IDataContext2Ptr sptrContext2 = m_pframe2->GetContext();

		long lPos = 0;
 		sptrContext->GetFirstObjectPos( bstrImage, &lPos );

		bool bFound = false;
		while( lPos )
		{
			IUnknown *punkImage = 0;
			sptrContext->GetNextObject( bstrImage, &lPos, &punkImage );

			_bstr_t bstrName = ::GetObjectName( punkImage );

			if( bstrName == _bstr_t( m_pErrDescr->pbstrImageNames[m_lCurrentImage] ) )
				bFound = true;

			if( bFound )
				sptrContext->SetActiveObject( bstrImage, punkImage, 0 );
			
			if( punkImage )
				punkImage->Release();

			if( bFound )
				break;
		}

		if( !bFound )
			sptrContext->SetActiveObject( bstrImage, 0, 0 );

		bool bImageA = bFound;

		lPos = 0;
		sptrContext2->GetFirstObjectPos( bstrImage, &lPos );

		bFound = false;
		while( lPos )
		{
			IUnknown *punkImage = 0;
			sptrContext2->GetNextObject( bstrImage, &lPos, &punkImage );
													
			_bstr_t bstrName = ::GetObjectName( punkImage );

			if( bstrName == ( _bstr_t( "__" ) + m_pErrDescr->pbstrImageNames[m_lCurrentImage] ) )
				bFound = true;

			if( bFound )
				sptrContext2->SetActiveObject( bstrImage, punkImage, 0 );
			
			if( punkImage )
				punkImage->Release();

			if( bFound )
				break;
		}

		if( !bFound )
			sptrContext2->SetActiveObject( bstrImage, 0, 0 );

		bool bImageB = bFound;

		lPos = 0;
 		sptrContext->GetFirstObjectPos( bstrList, &lPos );

		while( bImageA && lPos )
		{
			IUnknown *punkList = 0;
			sptrContext->GetNextObject( bstrList, &lPos, &punkList );

			bool bFound = false;
			_bstr_t bstrName = ::GetObjectName( punkList );

			if( bstrName == _bstr_t( "obl_" ) + _bstr_t( m_pErrDescr->pbstrImageNames[m_lCurrentImage] ) )
				bFound = true;

			if( bFound )
				sptrContext->SetActiveObject( bstrList, punkList, 0 );
			
			if( punkList )
				punkList->Release();

			if( bFound )
				break;
		}

		lPos = 0;
		sptrContext2->GetFirstObjectPos( bstrList, &lPos );

		while( bImageB && lPos )
		{
			IUnknown *punkList = 0;
			sptrContext2->GetNextObject( bstrList, &lPos, &punkList );
													
			bool bFound = false;
			_bstr_t bstrName = ::GetObjectName( punkList );

			if( bstrName == ( _bstr_t( "obl2_" ) + m_pErrDescr->pbstrImageNames[m_lCurrentImage] ) )
				bFound = true;

			if( bFound )
				sptrContext2->SetActiveObject( bstrList, punkList, 0 );
			
			if( punkList )
				punkList->Release();

			if( bFound )
				break;
		}
	}

	::EnableWindow( get_dlg_item( IDC_BUTTON1 ) , m_lCurrentImage != 0 );
	::EnableWindow( get_dlg_item( IDC_BUTTON2 ) , m_lMaxImage > 1 && m_lCurrentImage < m_lMaxImage - 1 );

	
	// [vanek] - 20.08.2004
	_check_sizes( );
}

void CCompareImagesDlg::_check_sizes( )
{
	int cx = 0, cy = 0, cx2 = 0, cy2 = 0;

	IImageViewPtr	sptrIV = m_pframe->GetView();
	if( sptrIV )
	{
		IUnknown *punkImage = 0;
		sptrIV->GetActiveImage( &punkImage, 0 );
		IImage3Ptr sptrImage = punkImage;			
		if( punkImage )
			punkImage->Release();	punkImage = 0;

		if( sptrImage )
		{
			RECT rcImage = {0};
			sptrImage->GetSize( &cx, &cy );
		}
		sptrImage = 0;
	}
	sptrIV = 0;

	sptrIV = m_pframe2->GetView();
	if( sptrIV )
	{
		IUnknown *punkImage = 0;
		sptrIV->GetActiveImage( &punkImage, 0 );
		IImage3Ptr sptrImage = punkImage;			
		if( punkImage )
			punkImage->Release();	punkImage = 0;

		if( sptrImage )
		{
			RECT rcImage = {0};
			sptrImage->GetSize( &cx2, &cy2 );
		}
		sptrImage = 0;
	}
	sptrIV = 0;

	double fcurr_zoom = 0.;
	m_pframe->GetZoom( &fcurr_zoom );
	double fzoom = 1.;
	bool bdiff = false;
	bdiff = (cx != cx2 || cy != cy2) && cx && cy && cx2 && cy2;
	if( bdiff )
	{
		RECT rc_client = {0},  rc_client2 = {0};
		rc_client = m_pframe->GetClientRect( );
		rc_client2 = m_pframe2->GetClientRect( );
		if( !::IsRectEmpty( &rc_client ) || !::IsRectEmpty( &rc_client2 ) )
		{
			fzoom = min( min( (double)(rc_client.right-rc_client.left)/(double)(cx),
				(double)(rc_client.bottom-rc_client.top)/(double)(cy) ),
				min( (double)(rc_client2.right-rc_client2.left)/(double)(cx2),
				(double)(rc_client2.bottom-rc_client2.top)/(double)(cy2) ) );
		}	
	}    

	::EnableWindow( ::GetDlgItem( handle(), IDC_BUTTON3 ), !bdiff );
	::EnableWindow( ::GetDlgItem( handle(), IDC_BUTTON4 ), !bdiff );

	if( bdiff && fcurr_zoom != fzoom )
	{
		m_pframe->SetZoom( fzoom );
		m_pframe2->SetZoom( fzoom );
	}	
}

long CCompareImagesDlg::on_destroy()
{
	m_pframe->de_init();
	m_pframe2->de_init();

	return 0L;
}
