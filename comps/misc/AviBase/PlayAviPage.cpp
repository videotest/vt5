// PlayAviPage.cpp: implementation of the CPlayAviPage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <atlstr.h>
#include "PlayAviPage.h"
#include "AviBase.h"
#include "core5.h"
#include "docview5.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayAviPage::CPlayAviPage()
{
	m_bstrCaption = "AVI";
}

//////////////////////////////////////////////////////////////////////
CPlayAviPage::~CPlayAviPage()
{
	UnAdviseAll();
}

//////////////////////////////////////////////////////////////////////
CWinImpl* CPlayAviPage::GetTarget()
{
	return this;
}

//////////////////////////////////////////////////////////////////////
IUnknown* CPlayAviPage::DoGetInterface( const IID &iid )
{
	IUnknown	*punk = DoGetDlgInterface( iid );
	if( punk )return punk;
	else if( iid == IID_IPersist )return (IPersist*)this;
	else if( iid == IID_IHelpInfo )return (IHelpInfo*)this;
	else if( iid == IID_IAviPlayPropPage )return (IAviPlayPropPage*)this;
	else return ComObjectBase::DoGetInterface( iid );
}

//IAviPlayPropPage
//////////////////////////////////////////////////////////////////////
HRESULT CPlayAviPage::AttachAction( IUnknown* punk )
{
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CPlayAviPage::UpdatePosition( long lFrame, long lFrameCount, double fFramesPerSecond )
{
	if( m_ptrSlider )
	{
		m_ptrSlider->PosLo = lFrame;

		HWND hWndFrame	= GetDlgItem( m_hwnd, IDC_FRAME );
		HWND hWndTime	= GetDlgItem( m_hwnd, IDC_TIME );
		if( hWndFrame && hWndTime )
		{
			static char szFrame[1024];
			static char szTime[1024];
			wsprintf( szFrame, "%d/%d", lFrame+1, lFrameCount );
			int nCurTime = 0;
			int nAllTime = 0;

			if( fFramesPerSecond > 0 )
			{
				nCurTime = int( double(lFrame) / fFramesPerSecond );
				nAllTime = int( double(lFrameCount) / fFramesPerSecond );
			}
			
			char szSec[20];
			::LoadString(app::handle(), IDS_SEC, szSec, 20);

			wsprintf( szTime, "%d/%d%s", nCurTime, nAllTime, szSec );

			::SetWindowText( hWndFrame, szFrame );
			::SetWindowText( hWndTime, szTime );
		}
		
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CPlayAviPage::GetClassID( CLSID *pclsid )
{
	*pclsid = clsidAviPlayPage;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
bool CPlayAviPage::DoInitDialog()
{
	//cleanup if it is not first call
	UnAdviseAll();

	Advise( IDC_PLAY, __uuidof( _DPushButtonEvents ), DISPID_CLICK, (FuncEventHandler)&CPlayAviPage::OnPlay);
	Advise( IDC_PAUSE, __uuidof( _DPushButtonEvents ), DISPID_CLICK, (FuncEventHandler)&CPlayAviPage::OnPause);
	Advise( IDC_STOP, __uuidof( _DPushButtonEvents ), DISPID_CLICK, (FuncEventHandler)&CPlayAviPage::OnStop);
	Advise( IDC_PREV, __uuidof( _DPushButtonEvents ), DISPID_CLICK, (FuncEventHandler)&CPlayAviPage::OnPrev);
	Advise( IDC_NEXT, __uuidof( _DPushButtonEvents ), DISPID_CLICK, (FuncEventHandler)&CPlayAviPage::OnNext);
	Advise( IDC_FIRST, __uuidof( _DPushButtonEvents ), DISPID_CLICK, (FuncEventHandler)&CPlayAviPage::OnFirst);
	Advise( IDC_LAST, __uuidof( _DPushButtonEvents ), DISPID_CLICK, (FuncEventHandler)&CPlayAviPage::OnLast);
	Advise( IDC_SET_ACTIVE, __uuidof( _DPushButtonEvents ), DISPID_CLICK, (FuncEventHandler)&CPlayAviPage::OnSetActiveFrame);	
	Advise( IDC_SLIDER, __uuidof( _DVTSliderEvents ), 1, (FuncEventHandler)&CPlayAviPage::OnSliderChange);

	m_ptrPlayBtn		= (IUnknown*)::SendMessage( ::GetDlgItem( m_hwnd, IDC_PLAY ), WM_GETCONTROL, 0, 0 );
	m_ptrPauseBtn		= (IUnknown*)::SendMessage( ::GetDlgItem( m_hwnd, IDC_PAUSE ), WM_GETCONTROL, 0, 0 );
	m_ptrStopBtn		= (IUnknown*)::SendMessage( ::GetDlgItem( m_hwnd, IDC_STOP ), WM_GETCONTROL, 0, 0 );
	m_ptrPrevBtn		= (IUnknown*)::SendMessage( ::GetDlgItem( m_hwnd, IDC_PREV ), WM_GETCONTROL, 0, 0 );
	m_ptrNextBtn		= (IUnknown*)::SendMessage( ::GetDlgItem( m_hwnd, IDC_NEXT ), WM_GETCONTROL, 0, 0 );
	m_ptrFirstBtn		= (IUnknown*)::SendMessage( ::GetDlgItem( m_hwnd, IDC_FIRST ), WM_GETCONTROL, 0, 0 );
	m_ptrLastBtn		= (IUnknown*)::SendMessage( ::GetDlgItem( m_hwnd, IDC_LAST ), WM_GETCONTROL, 0, 0 );
	m_ptrSlider			= (IUnknown*)::SendMessage( ::GetDlgItem( m_hwnd, IDC_SLIDER ), WM_GETCONTROL, 0, 0 );
	m_ptrSetActiveBtn	= (IUnknown*)::SendMessage( ::GetDlgItem( m_hwnd, IDC_SET_ACTIVE ), WM_GETCONTROL, 0, 0 );

	UpdateControls();

	UpdateCtrls();

	return true;
}




//////////////////////////////////////////////////////////////////////
HRESULT CPlayAviPage::UpdateCtrls()
{	
	bool bDisable = false;
	/*if( m_ptrAction == 0 )
	{
		bDisable = true;
	}
	else*/
	{
		IAviImagePtr sptrAvi = GetActiveAviImage();

		IUnknown *punkAction = 0;
		punkAction = GetCurrentInteractiveAction( );
		IInteractiveAviPlayActionPtr	sptrAction = punkAction;
		if( punkAction )
			punkAction->Release(); punkAction = 0;

        //IUnknown* punkAvi = 0;
		//m_ptrAction->GetAviImage( &punkAvi );
		

		//long lActionCurFrame = -1;
		//m_ptrAction->GetCurrentFrame( &lActionCurFrame );
		
		long bPlay = FALSE;
		if( sptrAction != 0 )
			sptrAction->IsPlay( &bPlay );

		if( sptrAvi == 0 )
		{
			bDisable = true;
		}		
		else		
		{
			//IAviImagePtr ptrAvi( punkAvi );
			//punkAvi->Release();	punkAvi = 0;
			//if( ptrAvi )

			_variant_t varCurrFrame( (long)(-1) );
			sptrAvi->get_CurFrame( &varCurrFrame );
			long lCurrFrame = (long)(varCurrFrame);

			{
				_variant_t var_count;
				sptrAvi->get_TotalFrames( &var_count );
				long lFrameCount = (long)var_count;


				double fFramesPerSecond = 0;
				variant_t var_fps;
				sptrAvi->get_FramePerSecond( &var_fps );
				fFramesPerSecond = (double)var_fps;
				
				if( m_ptrSlider )
				{
					m_ptrSlider->Disabled = 0;
					m_ptrSlider->Min = 0;
					m_ptrSlider->Max = lFrameCount;

					//long lFrame = -1;
					//m_ptrAction->GetCurrentFrame( &lFrame );

					UpdatePosition( lCurrFrame, lFrameCount, fFramesPerSecond );
				}

				if( bPlay )
				{
					if( m_ptrPlayBtn )		m_ptrPlayBtn->Disabled		= -1;						
					if( m_ptrPauseBtn )		m_ptrPauseBtn->Disabled		= 0;
					if( m_ptrStopBtn )		m_ptrStopBtn->Disabled		= 0;
					if( m_ptrSetActiveBtn )	m_ptrSetActiveBtn->Disabled	= -1;
				}
				else
				{
					if( m_ptrPlayBtn )		m_ptrPlayBtn->Disabled		= 0;
					if( m_ptrPauseBtn )		m_ptrPauseBtn->Disabled		= -1;
					if( m_ptrStopBtn )		m_ptrStopBtn->Disabled		= -1;
					if( m_ptrSetActiveBtn )	m_ptrSetActiveBtn->Disabled	= 0;
				}

				if( (lCurrFrame >= lFrameCount-1) || bPlay )
				{
					if( m_ptrNextBtn )		m_ptrNextBtn->Disabled		= -1;
					if( m_ptrLastBtn )		m_ptrLastBtn->Disabled		= -1;
				}
				else
				{
					if( m_ptrNextBtn )		m_ptrNextBtn->Disabled		= 0;
					if( m_ptrLastBtn )		m_ptrLastBtn->Disabled		= 0;
				}

				if( (lCurrFrame == 0) || bPlay )
				{
					if( m_ptrFirstBtn )		m_ptrFirstBtn->Disabled		= -1;
					if( m_ptrPrevBtn )		m_ptrPrevBtn->Disabled		= -1;
				}
				else
				{
					if( m_ptrFirstBtn )		m_ptrFirstBtn->Disabled		= 0;
					if( m_ptrPrevBtn )		m_ptrPrevBtn->Disabled		= 0;
				}
			}			
		}

	}
	
	if( bDisable )
	{
		if( m_ptrPlayBtn )
			m_ptrPlayBtn->Disabled = -1;

		if( m_ptrStopBtn )
			m_ptrStopBtn->Disabled = -1;

		if( m_ptrPauseBtn )
			m_ptrPauseBtn->Disabled = -1;

		if( m_ptrPrevBtn )
			m_ptrPrevBtn->Disabled = -1;

		if( m_ptrNextBtn )
			m_ptrNextBtn->Disabled = -1;

		if( m_ptrFirstBtn )
			m_ptrFirstBtn->Disabled = -1;

		if( m_ptrLastBtn )
			m_ptrLastBtn->Disabled = -1;

		if( m_ptrSetActiveBtn )
			m_ptrSetActiveBtn->Disabled = -1;


		if( m_ptrSlider )
			m_ptrSlider->Disabled = -1;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
void CPlayAviPage::OnPlay()
{
	// [vanek] :проверим на всякий случай, если акция воспроизведения запущена - Play, если нет, то запускаем ее - 14.09.2004
	IUnknownPtr sptr_unk( GetCurrentInteractiveAction( ), false );
	IInteractiveAviPlayActionPtr	sptrAction = sptr_unk;
	if( sptrAction != 0 )
	    sptrAction->Play( );		
	else
		::ExecuteAction( "AviPlay", 0, 0 );

	IApplicationPtr sptrApp(GetAppUnknown());
	if (sptrApp)
		sptrApp->IdleUpdate();
		
	/*if( m_ptrAction )
	{
		m_ptrAction->Play();
	}  */
    
	//UpdateCtrls();
}

//////////////////////////////////////////////////////////////////////
void CPlayAviPage::OnStop()
{
	// [vanek] - 14.09.2004
	::ExecuteAction( "AviStop", 0, 0 );
	
	//if( m_ptrAction )
	//{
	//	m_ptrAction->Stop();
	//}

	//UpdateCtrls();

}

//////////////////////////////////////////////////////////////////////
void CPlayAviPage::OnPause()
{
	// [vanek] : приостанавливаем воспроизведение - 14.09.2004
	IUnknownPtr sptr_unk( GetCurrentInteractiveAction( ), false );
	IInteractiveAviPlayActionPtr	sptrAction = sptr_unk;
	if( sptrAction == 0 )
		return;

	sptrAction->Pause( );

	// инициируем обновление состояний акций
	IApplicationPtr sptrApp(GetAppUnknown());
	if (sptrApp)
		sptrApp->IdleUpdate();

	//if( m_ptrAction )
	//{
	//	m_ptrAction->Pause();
	//}

	UpdateCtrls();

}

//////////////////////////////////////////////////////////////////////
void CPlayAviPage::OnPrev()
{
	// [vanek] - 14.09.2004
	::ExecuteAction( "AviMovePrev", 0, 0 );
    
	//if( m_ptrAction )
	//{
	//	long lFrame = -1;
	//	m_ptrAction->GetCurrentFrame( &lFrame );
	//	m_ptrAction->SetCurrentFrame( lFrame - 1 );
	//}

	//UpdateCtrls();
}

//////////////////////////////////////////////////////////////////////
void CPlayAviPage::OnNext()
{
	// [vanek] - 14.09.2004
	::ExecuteAction( "AviMoveNext", 0, 0 );

	//if( m_ptrAction )
	//{
	//	long lFrame = -1;
	//	m_ptrAction->GetCurrentFrame( &lFrame );
	//	m_ptrAction->SetCurrentFrame( lFrame + 1 );
	//}

	//UpdateCtrls();
}

//////////////////////////////////////////////////////////////////////
void CPlayAviPage::OnFirst()
{
	// [vanek] - 14.09.2004
	::ExecuteAction( "AviMoveFirst", 0, 0 );

	//if( m_ptrAction )
	//{				
	//	m_ptrAction->SetCurrentFrame( 0 );
	//}

	//UpdateCtrls();

}

//////////////////////////////////////////////////////////////////////
void CPlayAviPage::OnLast()
{
	// [vanek] - 14.09.2004
	::ExecuteAction( "AviMoveLast", 0, 0 );

	//if( m_ptrAction )
	//{				
	//	IUnknown* punkAvi = 0;
	//	m_ptrAction->GetAviImage( &punkAvi );

	//	if( punkAvi )
	//	{
	//		IAviImagePtr ptrAvi( punkAvi );
	//		punkAvi->Release();	punkAvi = 0;

	//		_variant_t var_count;
	//		ptrAvi->get_TotalFrames( &var_count );
	//		long lFrame = (long)var_count;

	//		m_ptrAction->SetCurrentFrame( lFrame - 1 );
	//	}		
	//}

	//UpdateCtrls();

}

//////////////////////////////////////////////////////////////////////
void CPlayAviPage::OnSetActiveFrame()
{
	//if( m_ptrAction )
	//{				
	//	IUnknown* punkAvi = 0;
	//	m_ptrAction->GetAviImage( &punkAvi );

	//	if( punkAvi )
	//	{
	//		long lActionFrame = -1;
	//		m_ptrAction->GetCurrentFrame( &lActionFrame );
	//		IAviImagePtr ptrAvi( punkAvi );
	//		punkAvi->Release();	punkAvi = 0;
	//		if( ptrAvi )
	//		{
	//			ptrAvi->MoveTo( lActionFrame, 0 );
	//		}
	//	}		
	//}  

    
}

bool bSliderChange = false;
//////////////////////////////////////////////////////////////////////
void CPlayAviPage::OnSliderChange()
{
	if( bSliderChange )
		return;

	// [vanek] - 14.09.2004
	if( m_ptrSlider != 0 )
	{				
		bSliderChange = true;

		IUnknownPtr sptr_unk( GetCurrentInteractiveAction( ), false );
		IInteractiveAviPlayActionPtr	sptrAction = sptr_unk;		
		if( sptrAction != 0 )
		{	// акция воспроизведения запущена
			long bPlay = FALSE;
			sptrAction->IsPlay( &bPlay );
			if( bPlay )
				sptrAction->Pause();
		
			sptrAction->SetCurrentFrame( m_ptrSlider->PosLo );

			if( bPlay )
				sptrAction->Play();
		}
		else
		{   // акциЯ воспроизведения не запущена -> просто переходим на нужный кадр
			CString str_param( _T("") );
			str_param.Format( "%d", m_ptrSlider->PosLo );
			::ExecuteAction( "AviMoveTo", str_param , 0 );
		}

		bSliderChange = false;
	}
}

//////////////////////////////////////////////////////////////////////
IUnknown* CPlayAviPage::GetActiveView()
{
    IApplicationPtr sptrApp( ::GetAppUnknown() );
	if( sptrApp == NULL )
		return NULL;

	IUnknown* punkDoc = NULL;
	sptrApp->GetActiveDocument( &punkDoc );
	
	if( punkDoc == NULL )
		return NULL;
	

	IDocumentSitePtr sptrDS( punkDoc );
	punkDoc->Release();
	if( sptrDS == NULL )
		return NULL;

	IUnknown* punkView = NULL;
	sptrDS->GetActiveView( &punkView );

	return punkView;
}

//////////////////////////////////////////////////////////////////////
IAviImagePtr CPlayAviPage::GetActiveAviImage()
{
    IUnknownPtr sptrView( GetActiveView(), false );
	if( sptrView == 0 )
		return 0;

	return ::GetActiveAviFromContext( sptrView );
}