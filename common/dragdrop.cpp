#include "stdafx.h"
#include "afxadv.h"
#include "afxpriv.h"

#include "docviewbase.h"
#include "dragdrop.h"
#include "frame.h"

#include "drag_drop_defs.h"

/////////////////////////////////////////////////////////////////////////////////////////
//CDataSource
//this class is "message listener". When drag operation preformed, it begin d'n'd process
//construct the Data Source object

CDataSource::CDataSource()
{
	m_pviewTarget = 0;
	m_pownerFrame = 0;
}

CDataSource::~CDataSource()	//destroy the Data source object
{
	DeInit();
}

/*IUnknown *CDataSource::GetTarget()
{
	if( !m_pviewTarget )
		return 0;
	return m_pviewTarget->GetControllingUnknown();
}
*/
/*bool CDataSource::CanStartTracking( CPoint pt )
{
	IUnknown	*punkO = m_pviewTarget->GetDragDropObject( pt );
	if( !punkO )
		return false;	//no active dataobjects

	punkO->Release();

	return true;
}
*/

void CDataSource::Init( CViewBase* pview, CFrame*  pOwnerFrame )
{
	m_pviewTarget = pview;
	m_pownerFrame = pOwnerFrame;
}

void CDataSource::DeInit()		//de-init
{
	m_pviewTarget = 0;
	m_pownerFrame = 0;
}

void CDataSource::OnFinalRelease()
{
}

const char *szActionObjectDrag = _T("ObjectDrag");
const char *szActionObjectDrop = _T("ObjectDrop");

bool CDataSource::DoEnterMode(CPoint point)
{
	//get the active object from view
	CRect	rectObject;
	CPoint	pointStartDragDrop = ::ConvertToClient( m_pviewTarget->GetControllingUnknown(), point/*m_pownerFrame->m_pointCurrent*/ );

	if(m_pviewTarget->m_pframe->GetObjectsCount() < 1)
		return false;

	CPoint	pointDragOffset;
	m_pviewTarget->m_pframe->GetFrameOffset(pointDragOffset);
	pointDragOffset.x = pointDragOffset.x - pointStartDragDrop.x;
	pointDragOffset.y = pointDragOffset.y - pointStartDragDrop.y;


	//begin Drag'n'drop actions
	{
		CString	strActionParams;
		////strActionParams.Format( "\"%s\", %d, %d", (const char*)strObjectName, pointDragOffset.x, pointDragOffset.y );
		strActionParams.Format( "%d, %d", pointDragOffset.x, pointDragOffset.y );
		ExecuteAction( szActionObjectDrag, strActionParams, aefNoPreformOperation );
	}

	return false;	//don't tracking
}

/////////////////////////////////////////////////////////////////////////////////////////
//CDropTarget
//
CDropTarget::CDropTarget()
{
	m_pviewTarget = 0;
	m_pointDragOffset = CPoint( 0, 0 );
	m_cf = 0;
	m_dwSaveDragFlags = 0;

	//TRACE( "\nCreate DS" );
}

CDropTarget::~CDropTarget()
{
	DeInit();
}

void CDropTarget::Init( CViewBase	*pview )
{
	sptrIWindow sptrWin(pview->GetControllingUnknown());
	DWORD dwFlags;
	if (sptrWin)
	{
		sptrWin->GetWindowFlags(&dwFlags);
		if (dwFlags & wfPlaceInDialog) // dialog can't be a drop target
			return;
	}

	Register( pview );

	IUnknown	*punkFrame = 0;
	pview->GetMultiFrame(&punkFrame, true);

	//paul 22.02.2002 code coment: why return???
	/*
	if( !punkFrame )return;
	punkFrame->Release();

	m_pviewTarget = pview;
	*/
	//and added
	if( punkFrame )
		punkFrame->Release();

	m_pviewTarget = pview;	
	
}

void CDropTarget::DeInit()		//de-init
{
	Revoke();
	m_pviewTarget = 0;
}


DROPEFFECT CDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,
	DWORD dwKeyState, CPoint point)
{
	if( !m_pviewTarget )return DROPEFFECT_NONE;
	if( !m_pviewTarget->m_pframeDrag )return DROPEFFECT_NONE;


	{
		//[ay] - почему-то не посылалась очень нужна€ нотификаци€ WM_DND_ON_DRAG_ENTER
		
		//[ay] - даже пон€тно почему - в кариовью использовалась рамка, а она загружаетс€
		// после нотификации
		drag_n_drop_data dnd;
		::ZeroMemory( &dnd, sizeof(drag_n_drop_data) );
		
		dnd.point			= point;
		dnd.hwndOwner		= m_pviewTarget->m_hWnd;
		dnd.hwndDragTo		= pWnd->m_hWnd;
		dnd.dwKeyState		= dwKeyState;	

		if( pDataObject )
			dnd.punkDataObject	= pDataObject->m_lpDataObject;

		LRESULT lres = 0;
		if( 1L == ::SendMessage( pWnd->m_hWnd, WM_DND_ON_DRAG_ENTER, (WPARAM)&dnd, (LPARAM)sizeof(drag_n_drop_data) ) )
		{	
			return dnd.dwDropEfect;
		}
	}
	
	HGLOBAL hGlobal = 0;

	hGlobal = pDataObject->GetGlobalData( m_cf = ::GetClipboardFormat() );

	if( !hGlobal )
	{
		hGlobal = pDataObject->GetGlobalData( m_cf = CF_HDROP );

		if( hGlobal )
		{
			::GlobalUnlock (hGlobal);
			return DROPEFFECT_COPY;
		}
			
		m_cf = 0;
		return DROPEFFECT_NONE;
	}

	::GlobalUnlock (hGlobal);

	ASSERT(m_pviewTarget);

	SerializeParams	params;
	ZeroMemory( &params, sizeof( params ) );
	params.flags |= sf_DontCheckSignature;

	::RestoreMultiFrameFromHandle( hGlobal,  
									m_pviewTarget->m_pframeDrag->GetControllingUnknown(), 
									&m_pointDragOffset,
									&params);

	HGLOBAL hh = ::GlobalFree(hGlobal);
	if(hh != NULL)
	{
		DWORD dw = GetLastError();
		ASSERT(FALSE);
	}

	point = ::ConvertToClient( m_pviewTarget->GetControllingUnknown(), point );
	point += m_pointDragOffset;

	DWORD nObjects = m_pviewTarget->m_pframeDrag->GetObjectsCount();

	if( nObjects > 0 )
	{
		HWND	hwnd = m_pviewTarget->GetSafeHwnd();
		while( hwnd )
		{
			HWND	hwndParent = ::GetParent( hwnd );
			DWORD	dwStyle = ::GetWindowLong( hwnd, GWL_STYLE );
			if( dwStyle & WS_CAPTION )
			{
				::SendMessage( hwndParent, WM_MDIACTIVATE, (WPARAM)hwnd, 0 );
				//::SetFocus( hwndParent );
				break;
			}
			hwnd = hwndParent;
		}


		m_pviewTarget->m_pframeDrag->SetFrameOffset( point );
		m_pviewTarget->m_pframeDrag->OnBeginDragDrop();
	}

	m_dwSaveDragFlags = m_pviewTarget->m_pframeDrag->GetFlags();

	if( m_pviewTarget->m_pframe )
		m_pviewTarget->m_pframeDrag->SetFlags( m_pviewTarget->m_pframe->GetFlags() );

	//m_pviewTarget->SetActiveWindow();
	m_pviewTarget->CWnd::GetParent()->SetFocus();
	//m_pviewTarget->SendMessage( WM_LBUTTONDOWN );
	//m_pviewTarget->SendMessage( WM_LBUTTONUP );
	//m_pviewTarget->SendMessage( WM_NCACTIVATE


	return (dwKeyState & MK_CONTROL)?DROPEFFECT_COPY : DROPEFFECT_MOVE;
}

DROPEFFECT CDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
	DWORD dwKeyState, CPoint point)
{
	
	if( !m_pviewTarget )
		return DROPEFFECT_NONE;

	{
		//[paul]24.12.2001
		drag_n_drop_data dnd;
		::ZeroMemory( &dnd, sizeof(drag_n_drop_data) );
		
		dnd.point			= point;
		dnd.hwndOwner		= m_pviewTarget->m_hWnd;
		dnd.hwndDragTo		= pWnd->m_hWnd;
		dnd.dwKeyState		= dwKeyState;	

		if( pDataObject )
			dnd.punkDataObject	= pDataObject->m_lpDataObject;

		LRESULT lres = 0;
		if( 1L == ::SendMessage( pWnd->m_hWnd, WM_DND_ON_DRAG_OVER, (WPARAM)&dnd, (LPARAM)sizeof(drag_n_drop_data) ) )
		{	
			return dnd.dwDropEfect;
		}
	}

	if( m_pviewTarget->m_pframeDrag )
	{
		if( m_cf == CF_HDROP )
			return DROPEFFECT_COPY;

		point = ::ConvertToClient( m_pviewTarget->GetControllingUnknown(), point );
		point += m_pointDragOffset;

		if( !m_pviewTarget->DoDrag( point ) )
			return DROPEFFECT_NONE;
	}

	if( !m_pviewTarget->m_pframeDrag )
		return DROPEFFECT_NONE;


	return (dwKeyState & MK_CONTROL)?DROPEFFECT_COPY : DROPEFFECT_MOVE;
}

BOOL CDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
	DROPEFFECT dropEffect, CPoint point)
{
	if( !m_pviewTarget )return false;

	{
		//[paul]24.12.2001
		drag_n_drop_data dnd;
		::ZeroMemory( &dnd, sizeof(drag_n_drop_data) );
		
		dnd.dwDropEfect		= dropEffect;
		dnd.point			= point;
		dnd.hwndOwner		= m_pviewTarget->m_hWnd;
		dnd.hwndDragTo		= pWnd->m_hWnd;

		
		if( pDataObject )
			dnd.punkDataObject	= pDataObject->m_lpDataObject;

		LRESULT lres = 0;
		if( 1L == ::SendMessage( pWnd->m_hWnd, WM_DND_ON_DROP, (WPARAM)&dnd, (LPARAM)sizeof(drag_n_drop_data) ) )
		{
			m_punkDroppedDataObject = 0;
			//OnDragLeave( pWnd );
			return false;
		}
	}


	if( !m_pviewTarget->m_pframeDrag )return false;

	//OnDragLeave( pWnd );
	if( m_cf == CF_HDROP )
	{
		HWND	hWnd = 0;
		sptrIApplication	sptrA( GetAppUnknown() );
		sptrA->GetMainWindowHandle( &hWnd );
		CWnd::FromHandle( hWnd )->
			SendMessage( WM_DROPFILES, (WPARAM)pDataObject->GetGlobalData( CF_HDROP ) );
	}

	if( dropEffect != DROPEFFECT_COPY &&
		dropEffect != DROPEFFECT_MOVE )
		return false;

	BOOL	bCopy = dropEffect == DROPEFFECT_COPY;

	point = ::ConvertToClient( m_pviewTarget->GetControllingUnknown(), point );
	point +=m_pointDragOffset;

	m_punkDroppedDataObject = pDataObject->m_lpDataObject;


	IDocumentSitePtr sptrDoc(m_pviewTarget->GetDocument());
	if(sptrDoc != 0)
		sptrDoc->SetActiveView(m_pviewTarget->GetControllingUnknown());

	CString	strActionParams;
	strActionParams.Format( "%d, %d, %d", point.x, point.y, bCopy );
	::ExecuteAction( szActionObjectDrop, strActionParams, aefNoPreformOperation );

	m_punkDroppedDataObject = 0;

	OnDragLeave( pWnd );

	return true;
}

void CDropTarget::OnDragLeave(CWnd* pWnd)
{
	if( !m_pviewTarget )return;
	if( !m_pviewTarget->m_pframeDrag )return;

	if( m_cf == ::GetClipboardFormat() )
	{
		//m_pviewTarget->DoAttachDragDropObject( 0 );
		IUnknown* punkFrame = 0;
		m_pviewTarget->GetMultiFrame(&punkFrame, true);

		if( punkFrame )
		{
			sptrIMultiSelection sptrMF(punkFrame);
			punkFrame->Release();
			if( sptrMF )
				sptrMF->EmptyFrame();
		}
	}
	m_sptrDragObject = 0;
	m_cf = 0;

	m_pviewTarget->m_pframeDrag->OnEndDragDrop();
	m_pviewTarget->m_pframeDrag->SetFlags( m_dwSaveDragFlags );
	m_pviewTarget->m_pframeDrag->Redraw( false );
}

DROPEFFECT CDropTarget::OnDragScroll(CWnd* pWnd, DWORD dwKeyState,
	CPoint point)
{
//	TRACE( "OnDragScroll\n" );
	if( m_cf == CF_HDROP )
		return DROPEFFECT_COPY;

	return (dwKeyState & MK_CONTROL)?DROPEFFECT_COPY : DROPEFFECT_MOVE;
}

void CDropTarget::OnFinalRelease()
{
}

