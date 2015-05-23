// Actions.cpp: implementation of the CActions class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Actions.h"

#include "nameconsts.h"
#include "misc_utils.h"

#include "core5.h"
#include "data5.h"
#include "docview5.h"

#include "avibase.h"
#include "guardint.h"

#include "\vt5\ifaces\avi_int.h"
#include "alloc.h"
#include "ImageSignature.h"

#include "alloc.h"




//////////////////////////////////////////////////////////////////////
// class CInsertAviObject
//////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CActionInsertAviObjectInfo::s_pargs[] = 
{	
	{"AviFileName",	szArgumentTypeString, "", true, false },
	{"LinkToFile",	szArgumentTypeInt, "1", true, false },
	{0, 0, 0, false, false },
};

//////////////////////////////////////////////////////////////////////
HRESULT CActionInsertAviObject::DoInvoke()
{

	IUnknown* punk = 0;
	punk = CreateAviObject();
	if( !punk )
		return E_FAIL;

	IAviObjectPtr ptrAvi( punk );
	punk->Release();	punk = 0;

	if( ptrAvi == 0 )
		return E_FAIL;


	_bstr_t bstrFileName = GetArgString( "AviFileName" );
	long lLinkToFile = GetArgLong( "LinkToFile" );


	if( lLinkToFile == 1 )
		ptrAvi->CreateLinkToFile( bstrFileName );
	

	SetToDocData( m_ptrTarget, ptrAvi );

	return S_OK;
}

//InsertAviObject "E:\avi_base\FILEMOVE.AVI"
/*
*pdwState = 0;
if( pThis->IsAvaible() )
	(*pdwState) |= afEnabled;
if( pThis->IsChecked() )
	(*pdwState) |= afChecked;
	*/


//////////////////////////////////////////////////////////////////////
// class CActionAviMoveFirst
//////////////////////////////////////////////////////////////////////
HRESULT CActionAviMoveFirst::DoInvoke()
{
	IAviImagePtr ptrAvi = GetActiveAviFromDoc( m_ptrTarget );
	if( ptrAvi == 0 )
		return S_FALSE;

	_variant_t var_count, var_cur;
	ptrAvi->get_TotalFrames( &var_count );
	ptrAvi->get_CurFrame( &var_cur );

	long lCount		= (long)var_count;
	long llCurFrame	= (long)var_cur;

	if( lCount > 0 )
	{
		ptrAvi->MoveTo( 0, 0 );		

		// [vanek] : update avi's properties page - 14.09.2004
		IUnknownPtr sptr_unk;
		GetAviPropPage( &sptr_unk );
		IAviPlayPropPagePtr sptr_avipage = sptr_unk;
		if( sptr_avipage != 0 )
			sptr_avipage->UpdateCtrls( );

	}	

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionAviMoveFirst::GetActionState( DWORD* pdwState )
{	
	*pdwState = 0;
	
	// [vanek] : не доступна, если идет воспроизведение  - 14.09.2004
	long bPlay = FALSE;
	IUnknownPtr sptr_unk( GetCurrentInteractiveAction( ), false );
	IInteractiveAviPlayActionPtr	sptrAction = sptr_unk;
	if( sptrAction )
		sptrAction->IsPlay( &bPlay );

	IAviImagePtr ptrAvi = GetActiveAviFromDoc( m_ptrTarget );

	if( (ptrAvi != 0) && !bPlay )
	{		

		_variant_t var_count, var_cur;
		ptrAvi->get_TotalFrames( &var_count );
		ptrAvi->get_CurFrame( &var_cur );

		long lCount		= (long)var_count;
		long llCurFrame	= (long)var_cur;

		if( lCount > 0 && llCurFrame != 0 )
		{
			(*pdwState) |= afEnabled;
		}	
	}

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
// class CActionAviMoveNext
//////////////////////////////////////////////////////////////////////
HRESULT CActionAviMoveNext::DoInvoke()
{	

	IAviImagePtr ptrAvi = GetActiveAviFromDoc( m_ptrTarget );
	if( ptrAvi == 0 )
		return S_FALSE;

	_variant_t var_count, var_cur;
	ptrAvi->get_TotalFrames( &var_count );
	ptrAvi->get_CurFrame( &var_cur );

	long lCount		= (long)var_count;
	long llCurFrame	= (long)var_cur;

	if( lCount > 0 && llCurFrame + 1 < lCount )
	{
		ptrAvi->MoveTo( llCurFrame + 1, 0 );		

		// [vanek] : update avi's properties page - 14.09.2004
		IUnknownPtr sptr_unk;
		GetAviPropPage( &sptr_unk );
		IAviPlayPropPagePtr sptr_avipage = sptr_unk;
		if( sptr_avipage != 0 )
			sptr_avipage->UpdateCtrls( );

	}	

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionAviMoveNext::GetActionState( DWORD* pdwState )
{	
	*pdwState = 0;
	
	// [vanek] : не доступна, если идет воспроизведение  - 14.09.2004
	long bPlay = FALSE;
	IUnknownPtr sptr_unk( GetCurrentInteractiveAction( ), false );
	IInteractiveAviPlayActionPtr	sptrAction = sptr_unk;
	if( sptrAction )
		sptrAction->IsPlay( &bPlay );

    IAviImagePtr ptrAvi = GetActiveAviFromDoc( m_ptrTarget );

	if( (ptrAvi != 0) && !bPlay )
	{		
		_variant_t var_count, var_cur;
		ptrAvi->get_TotalFrames( &var_count );
		ptrAvi->get_CurFrame( &var_cur );

		long lCount		= (long)var_count;
		long llCurFrame	= (long)var_cur;

		if( lCount > 0 && llCurFrame != lCount - 1 )
		{
			(*pdwState) |= afEnabled;
		}	
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// class CActionAviMoveLast
//////////////////////////////////////////////////////////////////////
HRESULT CActionAviMoveLast::DoInvoke()
{
	IAviImagePtr ptrAvi = GetActiveAviFromDoc( m_ptrTarget );
	if( ptrAvi == 0 )
		return S_FALSE;

	_variant_t var_count, var_cur;
	ptrAvi->get_TotalFrames( &var_count );
	ptrAvi->get_CurFrame( &var_cur );

	long lCount		= (long)var_count;
	long llCurFrame	= (long)var_cur;

	if( lCount > 0 )
	{
		ptrAvi->MoveTo( lCount - 1, 0 );		

		// [vanek] : update avi's properties page - 14.09.2004
		IUnknownPtr sptr_unk;
		GetAviPropPage( &sptr_unk );
		IAviPlayPropPagePtr sptr_avipage = sptr_unk;
		if( sptr_avipage != 0 )
			sptr_avipage->UpdateCtrls( );
	}	

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionAviMoveLast::GetActionState( DWORD* pdwState )
{	
	*pdwState = 0;

	// [vanek] : не доступна, если идет воспроизведение  - 14.09.2004
	long bPlay = FALSE;
	IUnknownPtr sptr_unk( GetCurrentInteractiveAction( ), false );
	IInteractiveAviPlayActionPtr	sptrAction = sptr_unk;
	if( sptrAction )
		sptrAction->IsPlay( &bPlay );
	
	IAviImagePtr ptrAvi = GetActiveAviFromDoc( m_ptrTarget );

	if( (ptrAvi != 0) && !bPlay )
	{		
		_variant_t var_count, var_cur;
		ptrAvi->get_TotalFrames( &var_count );
		ptrAvi->get_CurFrame( &var_cur );

		long lCount		= (long)var_count;
		long llCurFrame	= (long)var_cur;

		if( lCount > 0 && llCurFrame != lCount - 1 )
		{
			(*pdwState) |= afEnabled;
		}	
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// class CActionAviMovePrev
//////////////////////////////////////////////////////////////////////
HRESULT CActionAviMovePrev::DoInvoke()
{
	IAviImagePtr ptrAvi = GetActiveAviFromDoc( m_ptrTarget );
	if( ptrAvi == 0 )
		return S_FALSE;

	_variant_t var_count, var_cur;
	ptrAvi->get_TotalFrames( &var_count );
	ptrAvi->get_CurFrame( &var_cur );

	long lCount		= (long)var_count;
	long llCurFrame	= (long)var_cur;

	if( lCount > 0 && llCurFrame - 1 < lCount && llCurFrame - 1 >= 0 )
	{
		ptrAvi->MoveTo( llCurFrame - 1, 0 );		

		// [vanek] : update avi's properties page - 14.09.2004
		IUnknownPtr sptr_unk;
		GetAviPropPage( &sptr_unk );
		IAviPlayPropPagePtr sptr_avipage = sptr_unk;
		if( sptr_avipage != 0 )
			sptr_avipage->UpdateCtrls( );	
	}	

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionAviMovePrev::GetActionState( DWORD* pdwState )
{	
	*pdwState = 0;

	// [vanek] : не доступна, если идет воспроизведение  - 14.09.2004
	long bPlay = FALSE;
	IUnknownPtr sptr_unk( GetCurrentInteractiveAction( ), false );
	IInteractiveAviPlayActionPtr	sptrAction = sptr_unk;
	if( sptrAction )
		sptrAction->IsPlay( &bPlay );
	
	IAviImagePtr ptrAvi = GetActiveAviFromDoc( m_ptrTarget );

	if( (ptrAvi != 0) && !bPlay )
	{		
		_variant_t var_count, var_cur;
		ptrAvi->get_TotalFrames( &var_count );
		ptrAvi->get_CurFrame( &var_cur );

		long lCount		= (long)var_count;
		long llCurFrame	= (long)var_cur;
		if( lCount > 0 && llCurFrame != 0 )
		{
			(*pdwState) |= afEnabled;
		}	
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// class CActionAviMoveTo
//////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CActionAviMoveToInfo::s_pargs[] = 
{	
	{"FrameNum",	szArgumentTypeInt, "1", true, false },
	{0, 0, 0, false, false },
};

//////////////////////////////////////////////////////////////////////
HRESULT CActionAviMoveTo::DoInvoke()
{

	IAviImagePtr ptrAvi = GetActiveAviFromDoc( m_ptrTarget );
	if( ptrAvi == 0 )
		return S_FALSE;

	long lFrame = GetArgLong( "FrameNum" );

	_variant_t var_count, var_cur;
	ptrAvi->get_TotalFrames( &var_count );
	long lCount		= (long)var_count;

	if( lCount > 0 && lFrame < lCount && lFrame >= 0 )
	{
		ptrAvi->MoveTo( lFrame, 0 );	

		// [vanek] : update avi's properties page - 14.09.2004
		IUnknownPtr sptr_unk;
		GetAviPropPage( &sptr_unk );
		IAviPlayPropPagePtr sptr_avipage = sptr_unk;
		if( sptr_avipage != 0 )
			sptr_avipage->UpdateCtrls( );
	}	

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionAviMoveTo::GetActionState( DWORD* pdwState )
{	
	*pdwState = 0;

	// [vanek] : не доступна, если идет воспроизведение  - 14.09.2004
	long bPlay = FALSE;
	IUnknownPtr sptr_unk( GetCurrentInteractiveAction( ), false );
	IInteractiveAviPlayActionPtr	sptrAction = sptr_unk;
	if( sptrAction )
		sptrAction->IsPlay( &bPlay );
	
	IAviImagePtr ptrAvi = GetActiveAviFromDoc( m_ptrTarget );

	if( (ptrAvi != 0) && !bPlay )
	{		
		_variant_t var_count;
		ptrAvi->get_TotalFrames( &var_count );

		long lCount		= (long)var_count;

		if( lCount > 0 )
		{
			(*pdwState) |= afEnabled;
		}	
	}

	return S_OK;
}



void CALLBACK OnTimerProc(UINT wTimerID, UINT msg, 
    DWORD dwUser, DWORD dw1, DWORD dw2) 
{

	if( dwUser < 1 )
		return;

	CActionAviPlay* paction = (CActionAviPlay*)dwUser;

	long lcur_frame = paction->GetCurFrame();
	paction->OnTimerSetFrame( ++lcur_frame );
	HWND hwnd = paction->GetHwnd();
//	static DWORD dwStart = 0;
//	char szBuff[256];
	bool bExpired = paction->IsExpired(lcur_frame);
	bool bProcessing = paction->IsMessageProcessing();
	long lPeriod = paction->GetTimerPeriod();
//	if (dwStart == 0)
//		dwStart = timeGetTime();
//	sprintf(szBuff, "OnTimer, lcur_frame=%d, bExpired=%d, time=%d\n",
//		lcur_frame, bExpired, timeGetTime()-dwStart);
//	OutputDebugString(szBuff);
	paction = 0;
	if (!bExpired && !bProcessing)
	{
	DWORD_PTR dw = 0;
	LRESULT lr =
			::SendMessageTimeout( hwnd, WM_USER + 778, (WPARAM)0, 0, SMTO_BLOCK, lPeriod/2, &dw );
	}

	/*

	IInteractiveAviPlayActionPtr ptr( (IUnknown*)dwUser );

	if( ptr )
	{
		long lFrame = -1;
		ptr->GetCurrentFrame( &lFrame );
		if( S_OK != ptr->SetCurrentFrame( lFrame + 1 ) )
		{
			ptr->Stop();
			return;
		}
	}
	*/

}


//////////////////////////////////////////////////////////////////////
// class CActionAviPlay
//////////////////////////////////////////////////////////////////////
CActionAviPlay::CActionAviPlay()
{
	m_lCurFrame		= 0;
	m_lPrevKeyFrame = m_lPrevFrame = -1;
	m_lFrameCount	= -1;
	m_pDibBits		= 0;
	m_lBufSize		= 0;
	m_bCanPlay		= false;

	m_sizePrev.cx	= m_sizePrev.cy = 0;
	m_pointPrev.x   = m_pointPrev.y = 0;
	m_fZoomPrev		= 1.0;
	m_lTimerPeriod	= 100;
	m_fFramesPerSecond	= 0.0;

	m_bPlay			= false;

	m_hDrawDib		= 0;
	
	m_bTimerFired	= false;

	m_dwStartTime = 0;
	m_lStartFrame = 0;
	m_bMessageProcessing = false;
}

//////////////////////////////////////////////////////////////////////
CActionAviPlay::~CActionAviPlay()
{	
	if( m_hDrawDib )
		DrawDibClose( m_hDrawDib );
	
	m_hDrawDib = 0;
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionAviPlay::AttachTarget( IUnknown *punkTarget )
{
	return CInteractiveAction::AttachTarget( punkTarget );
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionAviPlay::SetArgument( BSTR bstrArgName, VARIANT *pvarVal )
{
	return CInteractiveAction::SetArgument( bstrArgName, pvarVal );
}

//////////////////////////////////////////////////////////////////////
bool CActionAviPlay::Initialize()
{
	if( !CInteractiveAction::Initialize() )
		return false;

	IScrollZoomSitePtr ptrZ( m_ptrTarget );
	if( ptrZ )
	{
		ptrZ->GetClientSize( &m_sizePrev );
		ptrZ->GetZoom( &m_fZoomPrev );		
		ptrZ->GetScrollPos( &m_pointPrev );		
	}
	
	m_bCanPlay = Init( false );
	ptrZ->SetScrollPos( m_pointPrev );		

	InvalidateView();

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CActionAviPlay::ShowPage( bool bshow )
{
	int	nPage = -1;
	IPropertySheet	*pSheet = ::FindPropertySheet();
	IOptionsPage	*pPage = ::FindPage( pSheet, clsidAviPlayPage, &nPage );

	//show hide
	if( nPage != -1 )
	{
		IAviPlayPropPagePtr	ptr_page = pPage;
		if( bshow )
		{
			pSheet->IncludePage( nPage );

			// [vanek] : страница свойств не связывается с текущей акцией, а живет при наличии активного AVI - 14.09.2004
			//if( ptr_page )
			//	ptr_page->AttachAction( Unknown() );

			m_ptrAviPropPage = ptr_page;
		}
		else
		{
            m_ptrAviPropPage = 0;

			// [vanek] : см. выше
			//if( ptr_page )
			//	ptr_page->AttachAction( 0 );

			pSheet->ExcludePage( nPage );
		}
	}

	if( pPage ) 
		pPage->Release();	pPage = 0;
	if( pSheet )
		pSheet->Release();	pSheet = 0;

	return true;
}


//////////////////////////////////////////////////////////////////////
bool CActionAviPlay::Finalize()
{
	CInteractiveAction::Finalize();

	Deinit( true );
	
	/*{
		IScrollZoomSitePtr ptrZ( m_ptrTarget );
		if( ptrZ )
		{
			ptrZ->SetClientSize( m_sizePrev );
			ptrZ->SetZoom( m_fZoomPrev );
			ptrZ->SetScrollPos( m_pointPrev );		
		}
	}*/
	
	InvalidateView();	
    return true;
}

//////////////////////////////////////////////////////////////////////
bool CActionAviPlay::DoLButtonDown( _point point )
{
	if( !CInteractiveAction::DoLButtonDown( point ) )
		return false;

	return true;

}

//////////////////////////////////////////////////////////////////////
void CActionAviPlay::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp( pszEvent, szEventActivateObject ) )
	{
		IAviImagePtr ptrNewAvi = ::GetActiveAviFromContext( punkFrom );

		if( ptrNewAvi != 0 && ::GetKey( ptrNewAvi ) != ::GetKey( m_avi ) )
		{	
			// [vanek] : останавливаем воспроизведение, согласовано со Струем - 14.09.2004
			if( m_bPlay )
				Pause();

			m_bCanPlay = Init( true );
		}	
	}
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionAviPlay::DoInvoke()
{
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
bool CActionAviPlay::Init( bool bUpdatePropPage )//Cos after sheet->IncludePage not realy create dialog
{
	Deinit( bUpdatePropPage );

	IScrollZoomSitePtr ptrZ( m_ptrTarget );
	if( ptrZ )
	{
		SIZE size;
		size.cx = size.cy = 0;
		ptrZ->SetClientSize( size );
	}

	IAviImagePtr ptrAvi = GetActiveAviFromContext( m_ptrTarget );
	if( ptrAvi == 0 )
		return false;

	_variant_t var_count;
	ptrAvi->get_TotalFrames( &var_count );
	long lCount		= (long)var_count;

	if( lCount < 1 )
	{
		return false;
	}	

	long lTimerPeriod = 1;

	m_lCurFrame		= 0;
	m_lPrevKeyFrame	= m_lPrevFrame = -1;
	m_lFrameCount	= lCount;

	m_avi = ptrAvi;

	_variant_t var_cur;
	if( S_OK == ptrAvi->get_CurFrame( &var_cur ) )
		m_lCurFrame = (long)var_cur;

	variant_t var_fps;
	m_fFramesPerSecond	= 0.0;
	if( S_OK == ptrAvi->get_FramePerSecond( &var_fps ) )
	{
		m_fFramesPerSecond = (double)var_fps;
		m_lTimerPeriod = long( 1000.0 / m_fFramesPerSecond );
	}
	else
	{
		m_lTimerPeriod = 100;
	}


	{
		long lInfoSize = 0;

		if( S_OK != m_avi->GetBitmapInfoHeader( 0, &lInfoSize, 0 ) )
			return false;

		BYTE* pInfo = new BYTE[lInfoSize];
		_ptrKiller_t<BYTE> pk1(pInfo);

		if( S_OK != m_avi->GetBitmapInfoHeader( m_lCurFrame, &lInfoSize, pInfo ) )
			return false;

		BITMAPINFOHEADER* pbi = (BITMAPINFOHEADER*)pInfo;

		SIZE size;
		size.cx = pbi->biWidth;
		size.cy = pbi->biHeight;
		ptrZ->SetClientSize( size );
	}


	ShowPage( true );

	if( m_ptrAviPropPage != 0 && bUpdatePropPage )
		m_ptrAviPropPage->UpdateCtrls();		


	if( m_lCurFrame == m_lFrameCount - 1 )
		SetCurrentFrame( 0 );

	Play();
    return true;
}

//////////////////////////////////////////////////////////////////////
void CActionAviPlay::Deinit( bool bUpdatePropPage )
{
	m_timer.StopTimer();

	m_avi = 0;

	if( m_pDibBits )
	{
		delete m_pDibBits;
		m_pDibBits = 0;
	}

	m_lCurFrame		= 0;
	m_lPrevKeyFrame = m_lPrevFrame = -1;
	m_lFrameCount	= -1;
	m_lBufSize		= 0;
	m_bCanPlay		= false;

	m_lTimerPeriod		= 100;
	m_fFramesPerSecond	= 0.0;

	m_bPlay			= false;

	if( m_ptrAviPropPage != 0 && bUpdatePropPage )
		m_ptrAviPropPage->UpdateCtrls();		
}

//////////////////////////////////////////////////////////////////////
IUnknown* CActionAviPlay::DoGetInterface( const IID &iid )
{
	if( iid == IID_IInteractiveAviPlayAction )return (IInteractiveAviPlayAction*)this;	
	else return CInteractiveAction::DoGetInterface( iid );
}

//////////////////////////////////////////////////////////////////////
//IInteractiveAviPlayAction
//////////////////////////////////////////////////////////////////////
HRESULT CActionAviPlay::Play()
{
	m_timer.StopTimer( );
	m_bPlay = false;
	m_bMessageProcessing = false;

	m_dwStartTime = timeGetTime();
	m_lStartFrame = m_lCurFrame;
	m_timer.StartTimer( m_lTimerPeriod, (DWORD)this, (LPTIMECALLBACK)OnTimerProc );
	m_bPlay = true;

	// [vanek] : update controls - 14.09.2004
	if( m_ptrAviPropPage )
		m_ptrAviPropPage->UpdateCtrls();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionAviPlay::Pause()
{
	m_timer.StopTimer( );
	m_bPlay = false;

	// [vanek] : при приостановке воспроизведения акция завершается
	LeaveMode();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionAviPlay::Stop()
{
	m_timer.StopTimer( );
	// [vanek] : при остановке воспроизведения возвращаемся в начало, согласовано со Струем - 14.09.2004
	SetCurrentFrame( 0 );	
	m_bPlay = false;

	LeaveMode();
    return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionAviPlay::SetCurrentFrame( long lFrame )
{
	if( lFrame < 0 || lFrame >= m_lFrameCount  )	
	{
		m_timer.StopTimer( );
		m_bPlay = false;
		if( m_ptrAviPropPage )
			m_ptrAviPropPage->UpdateCtrls();

		return S_FALSE;
	}


	m_lCurFrame = lFrame;

	IAviImagePtr ptr_avi = m_avi;
	ptr_avi->MoveTo( m_lCurFrame, 0 );

	// [vanek] BT2000:3989 - 15.09.2004
	InvalidateAvi();

	if( m_ptrAviPropPage )
		m_ptrAviPropPage->UpdatePosition( lFrame, m_lFrameCount, m_fFramesPerSecond );

	if( lFrame == 0 || lFrame == m_lFrameCount  )	
	{
		if( m_ptrAviPropPage )
			m_ptrAviPropPage->UpdateCtrls();
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionAviPlay::IsPlay( long* pbPlay )
{
	*pbPlay = ( m_bPlay == true ? TRUE : FALSE );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionAviPlay::GetCurrentFrame( long* plFrame )
{
	IAviImagePtr ptr_avi = m_avi;
	if( ptr_avi == 0 )
		return S_FALSE;

	_variant_t var;
	ptr_avi->get_CurFrame( &var );

	*plFrame = (long)var;
	m_lCurFrame = (long)var;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionAviPlay::GetAviImage( IUnknown** punkAvi )
{
	if( m_avi )
	{
		m_avi->AddRef();
		*punkAvi = m_avi;
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionAviPlay::SetZoom( double fZoom )
{
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
void CActionAviPlay::InvalidateView()
{
	return;

	HWND	hwnd = 0;
	
	IWindowPtr	ptrWindow( m_ptrTarget );
	if( ptrWindow )
		ptrWindow->GetHandle( (HANDLE*)&hwnd );
	

	if( hwnd )
	{
		::InvalidateRect( hwnd, 0, FALSE );
		::UpdateWindow( hwnd );
	}

}

//////////////////////////////////////////////////////////////////////
void CActionAviPlay::InvalidateAvi()
{
	//return;
	if( !m_bCanPlay )
		return;

	if( m_lCurFrame >= m_lFrameCount || m_lCurFrame < 0 )
		return;

	if( m_avi == 0 )
		return;

	HWND	hwnd = 0;
	
	IWindowPtr	ptrWindow( m_ptrTarget );
	if( ptrWindow == 0 )
		return;
	ptrWindow->GetHandle( (HANDLE*)&hwnd );

	// [vanek] BT2000:3989 - 15.09.2004
	::InvalidateRect( hwnd, 0, FALSE );
}


//////////////////////////////////////////////////////////////////////
bool CActionAviPlay::Paint( HDC hdc, RECT rect )
{
	return true;
	if( !m_bCanPlay )
		return false;

	if( m_lCurFrame >= m_lFrameCount || m_lCurFrame < 0 )
		return false;

	if( m_avi == 0 )
		return false;

	long lInfoSize = 0;

	if( S_OK != m_avi->GetBitmapInfoHeader( m_lCurFrame, &lInfoSize, 0 ) )
		return false;

	BYTE* pInfo = new BYTE[lInfoSize];
	_ptrKiller_t<BYTE> pk1(pInfo);

	if( S_OK != m_avi->GetBitmapInfoHeader( m_lCurFrame, &lInfoSize, pInfo ) )
		return false;

	BITMAPINFOHEADER* pbi = (BITMAPINFOHEADER*)pInfo;

	pbi->biCompression	= BI_RGB;
	pbi->biSize			= sizeof(BITMAPINFOHEADER); 

	long lDibBitsSize = 0;
	if( S_OK != m_avi->GetDibBitsSize( (BYTE*)pbi, &lDibBitsSize ) )
		return false;

	if( lDibBitsSize != m_lBufSize )
	{
		m_lBufSize = lDibBitsSize;
		if( m_pDibBits )
		{
			delete m_pDibBits;
			m_pDibBits = 0;
		}

		m_pDibBits = new BYTE[m_lBufSize];
	}

	if( S_OK != m_avi->GetDibBits( m_lCurFrame, m_pDibBits, &m_lPrevKeyFrame, &m_lPrevFrame ) )
		return false;	 	

	

	if( !m_hDrawDib )
		m_hDrawDib = ::DrawDibOpen();

	if( !m_hDrawDib )
		return false;

	double fZoom = 1.0;
	POINT ptScroll;
	ptScroll.x = ptScroll.y = 0;
	IScrollZoomSitePtr ptrZ( m_ptrTarget );
	if( ptrZ )
	{
		ptrZ->GetScrollPos( &ptScroll );
		ptrZ->GetZoom( &fZoom );		
	}

	int xDst, yDst;
	int dxDst, dyDst;

	int xSrc, ySrc;
	int dxSrc, dySrc;

	xSrc	= 0;
	ySrc	= 0;

	dxSrc	= pbi->biWidth;
	dySrc	= pbi->biHeight;


	xDst	= -long( double(ptScroll.x) );
	yDst	= -long( double(ptScroll.y) );

	dxDst	= long( double(pbi->biWidth) * fZoom );
	dyDst	= long( double(pbi->biHeight) * fZoom );


	
	BOOL bRes = ::DrawDibDraw( m_hDrawDib, hdc,
									xDst, yDst, 
									dxDst, dyDst,
									pbi, m_pDibBits, 
									xSrc, ySrc,
									dxSrc, dySrc, 0 );	
	

	/*
	int xDst, yDst;
	int dxDst, dyDst;

	int xSrc, ySrc;
	int dxSrc, dySrc;

	xDst	= rect.left;
	yDst	= rect.top;	

	dxDst	= rect.right - xDst;
	dyDst	= rect.bottom - yDst;

	if( long( double(dxDst) / fZoom ) > pbi->biWidth )
		dxDst = long( double(pbi->biWidth) * fZoom );

	if( long( double(dyDst) / fZoom ) > pbi->biHeight )
		dyDst = long( double(pbi->biHeight) * fZoom );

	xSrc	= long( double(ptScroll.x + rect.left) / fZoom );
	ySrc	= long( double(ptScroll.y + rect.top) / fZoom );
	dxSrc	= long( double(dxDst) / fZoom );
	dySrc	= long( double(dyDst) / fZoom );
	
	dxSrc	= long( double(dxDst) / fZoom );
	dySrc	= long( double(dyDst) / fZoom );

	
	if( xSrc < 0 )
		xSrc = 0;

	if( xSrc >= pbi->biWidth )
		xSrc = pbi->biWidth-1;

	if( ySrc < 0 )
		ySrc = 0;

	if( ySrc >= pbi->biHeight )
		ySrc = pbi->biHeight - 1;

	if( dxSrc < 0 )
		dxSrc = 0;

	if( xSrc + dxSrc >= pbi->biWidth )
		dxSrc = pbi->biWidth - 1 - xSrc;

	if( dySrc < 0 )
		dySrc = 0;

	if( ySrc + dySrc >= pbi->biHeight )
		dySrc = pbi->biHeight - 1 - ySrc;		

	
	BOOL bRes = ::DrawDibDraw( hDrawDib, hdc,
									xDst, yDst, 
									dxDst, dyDst,
									pbi, m_pDibBits, 
									xSrc, ySrc,
									dxSrc, dySrc, 0 );	
	DrawDibClose( hDrawDib );
	*/

	return true;

}

bool CActionAviPlay::IsExpired(long lFrame)
{
	DWORD dwNow = timeGetTime();
	DWORD dwFrame = m_dwStartTime+(lFrame-m_lStartFrame)*m_lTimerPeriod;
	return dwNow > dwFrame+m_lTimerPeriod/2;
}

bool CActionAviPlay::IsMessageProcessing()
{
	return m_bMessageProcessing;
}

//////////////////////////////////////////////////////////////////////

class _CMessageProcessing
{
	bool *m_pbFlag;
public:
	_CMessageProcessing(bool *pbFlag)
	{
		m_pbFlag = pbFlag;
		*m_pbFlag = true;
	}
	~_CMessageProcessing()
	{
		*m_pbFlag = false;
	}

};

LRESULT	CActionAviPlay::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	if( nMsg == WM_USER + 778 )
	{
		_CMessageProcessing MsgProc(&m_bMessageProcessing);
//		DWORD dw1 = timeGetTime();
//		char szBuff[256];
//		sprintf(szBuff, "WM_USER + 778 processing starts, m_lCurFrame = %d,"
//			" time = %d, frame time = %d, expire time = %d\n", m_lCurFrame,
//			dw1-m_dwStartTime, (m_lCurFrame-m_lStartFrame)*m_lTimerPeriod,
//			(m_lCurFrame-m_lStartFrame)*m_lTimerPeriod+m_lTimerPeriod/2);
//		OutputDebugString(szBuff);
		if( S_OK != SetCurrentFrame( m_lCurFrame ) )
		{
			Stop();
			return 1L;
		}
//		sprintf(szBuff, "WM_USER + 778 processing ends, %d\n" , timeGetTime()-m_dwStartTime);
//		OutputDebugString(szBuff);
	}

	long	lResult = CInteractiveAction::DoMessage( nMsg, wParam, lParam );
	if( lResult )
		return lResult;

	/*
	if( nMsg == WM_PAINT )
	{
		HWND	hwnd;
		{
		IWindowPtr	ptrWindow( m_ptrTarget );
		if( ptrWindow == 0 )
			return 0;
		ptrWindow->GetHandle( (HANDLE*)&hwnd );

		}

		PAINTSTRUCT	paint;
		HDC	hdcPaint = ::BeginPaint( hwnd, &paint );

		int	cx = paint.rcPaint.right - paint.rcPaint.left;
		int	cy = paint.rcPaint.bottom - paint.rcPaint.top;

		if( cx <= 0 || cy <= 0 )
		{
			::EndPaint( hwnd, &paint );
			return 1;
		}

		if( !Paint( hdcPaint, paint.rcPaint ) )
		{
			HBRUSH hBrush = ::CreateSolidBrush( ::GetSysColor( COLOR_WINDOW ) );			
			::FillRect( hdcPaint, &paint.rcPaint, hBrush );
			::DeleteObject( hBrush );	hBrush = 0;
		}
		
		::EndPaint( hwnd, &paint );
		
		return 1;
	}

	if( nMsg == WM_ERASEBKGND )
		return 0;
		*/

	return 0;
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionAviPlay::GetActionState( DWORD* pdwState )
{	
	*pdwState = 0;

	// [vanek] SBT:1172 выставляем этот флаг, чтобы акция в конце концов попала в ActionLog - 14.10.2004
	(*pdwState) |= afRequiredInvokeOnTerminate;

	IAviImagePtr ptr_avi = GetActiveAviFromContext( m_ptrTarget );
	if( ptr_avi != 0 )
		(*pdwState) |= afEnabled;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
bool CActionAviPlay::OnTimerSetFrame( long lframe )
{
	if( !m_bPlay )
		return false;

	InterlockedExchange( &m_lCurFrame, lframe );

	return true;
}

//////////////////////////////////////////////////////////////////////
long CActionAviPlay::GetCurFrame()
{
	return m_lCurFrame;
}

//////////////////////////////////////////////////////////////////////
HWND CActionAviPlay::GetHwnd()
{
	return m_hwnd;
}
//////////////////////////////////////////////////////////////////////
//
//
//	class CActionAviStop
//
//
//////////////////////////////////////////////////////////////////////
HRESULT CActionAviStop::DoInvoke()
{
	IUnknown* punk = get_interactive();
	if( !punk )
		return S_FALSE;

	IInteractiveAviPlayActionPtr ptr_action = punk;
	punk->Release();	punk = 0;
	if( ptr_action == 0 )
		return S_FALSE;

	ptr_action->Stop( );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionAviStop::GetActionState( DWORD* pdwState )
{
	if( !pdwState )
		return E_POINTER;

	*pdwState = 0;

	IUnknown* punk = get_interactive();
	if( punk )
	{
		IInteractiveAviPlayActionPtr ptr_action = punk;
		if( ptr_action )
		{
			long bplay = FALSE;
			ptr_action->IsPlay( &bplay );
			if( bplay )
				(*pdwState) |= afEnabled;
		}
		punk->Release();	punk = 0;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
IUnknown* CActionAviStop::get_interactive()
{
	return GetCurrentInteractiveAction( );
}


//////////////////////////////////////////////////////////////////////
//
//
//	class CShowAviGallery
//
//
//////////////////////////////////////////////////////////////////////
CShowAviGallery::CShowAviGallery()
{

}

//////////////////////////////////////////////////////////////////////
CShowAviGallery::~CShowAviGallery()
{

}

//////////////////////////////////////////////////////////////////////
HRESULT CShowAviGallery::DoInvoke()
{

	if( m_ptrTarget == NULL )
		return E_FAIL;

	sptrIFrameWindow	sptrF( m_ptrTarget );
	if( sptrF == NULL )
		return E_FAIL;

	IUnknown	*punkSplitter = 0;

	sptrF->GetSplitter( &punkSplitter );
	if( !punkSplitter )
		return E_FAIL;

	sptrISplitterWindow	sptrS( punkSplitter );
	punkSplitter->Release();

	if( sptrS == NULL )
		return E_FAIL;	
		
	int	nRow, nCol;

	_bstr_t	bstrViewProgID( szAviGalleryViewProgID );
	sptrS->GetActivePane( &nRow, &nCol );
	sptrS->ChangeViewType( nRow, nCol, bstrViewProgID );

	IUnknown	*punkView = 0;
	sptrS->GetViewRowCol( nCol, nRow, &punkView );

	if( !punkView )return E_FAIL;
	punkView->Release();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//
//	class CAviSignature
//
//
//////////////////////////////////////////////////////////////////////
CAviSignature::CAviSignature()
{

}

//////////////////////////////////////////////////////////////////////
CAviSignature::~CAviSignature()
{

}

//////////////////////////////////////////////////////////////////////
HRESULT CAviSignature::DoInvoke()
{
	IAviImagePtr ptrAvi = GetActiveAviFromDoc( m_ptrTarget );
	if( ptrAvi == 0 ) return S_FALSE;
	IImage2Ptr ptrImage(ptrAvi);
	if( ptrImage == 0 ) return S_FALSE;
	IAviObject2Ptr ptrAviObject(ptrAvi);
	if( ptrAviObject == 0 ) return S_FALSE;

	BSTR	bstr=0;
	ptrAviObject->GetAviFileName( &bstr );
	_bstr_t bstrFileName = bstr;
	::SysFreeString( bstr );

	_variant_t var_count;
	ptrAvi->get_TotalFrames( &var_count );
	long lCount		= (long)var_count;

	_bstr_t bstrKeyPath = "\\vt5\\crypto\\private_key.h";
	// bstrKeyPath = ::MakeAppPathName( "private_key.h" );
	bstrKeyPath = ::GetValueString( ::GetAppUnknown(), "\\Signature", "PrivateKeyFile", bstrKeyPath );
	private_key key2 = read_key(bstrKeyPath);
	if(key2.p==0)
	{
		::MessageBox(0, "Private key not found", "Error", MB_OK | MB_ICONERROR );
		return S_FALSE;
	}
	int nMaxStringLen = vlong_to_string(key2.m, 0, 0);

	smart_alloc(sig_data, char, nMaxStringLen*lCount);
	ZeroMemory(sig_data, nMaxStringLen*lCount);

	StartNotification(lCount, 1, 1);

	for( int lFrame=0; lFrame < lCount; lFrame ++ )
	{
		Notify(lFrame);
		ptrAvi->MoveTo( lFrame, 0 );
		byte digest[16];
		CalcSignature(ptrImage, digest, 16);

		vlong v1=0;
		unsigned* p = (unsigned*)digest;
		for(int i=0; i<16/sizeof(unsigned); i++) v1.set_data(i,p[i]);
		vlong v_new = key2.decrypt(v1);
		//StoreVLong(ptrImage, v_new);

        vlong_to_string(v_new, sig_data + nMaxStringLen*lFrame, nMaxStringLen);
	}

	FinishNotification();

	HRESULT hRes = S_FALSE;
	IAVIFile* pAVIFile;
	hRes = ::AVIFileOpen( &pAVIFile, bstrFileName, OF_READWRITE, NULL );
	if( hRes != S_OK ) return E_FAIL;

	try
	{
		char szChunk[] = "VTSC"; // VideoTest Security Code
		DWORD dwChunk = *(dword*)szChunk;
		AVIFileWriteData(pAVIFile, dwChunk, sig_data, nMaxStringLen*lCount);
	}
	catch(...)
	{
		assert( false );
	}

	if( pAVIFile )
	{
		::AVIFileRelease( pAVIFile );
		pAVIFile = 0;
	}		

	return S_OK;
}

IUnknown *CAviSignature::DoGetInterface( const IID &iid )
{
	if( iid == IID_ILongOperation )return (ILongOperation*)this;	
	else return CAction::DoGetInterface( iid );
};
