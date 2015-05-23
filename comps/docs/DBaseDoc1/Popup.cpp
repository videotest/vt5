#include "stdafx.h"
#include <objbase.h>
#include <initguid.h>
#include "Popup.h"
#include "PopupTools.h"

#if _MSC_VER >= 1300
AFX_DATADEF TCHAR afxChNil = '\0';
#endif

///////////////////////////////////////////////////////////////////////////////
CPopup::CPopup () : m_pCaller (NULL), m_pControl (NULL), m_pEvents (NULL),
                    m_hPrevParentCtrl (NULL), m_bDestroy (false)
{
}


///////////////////////////////////////////////////////////////////////////////
bool CPopup::Display (CWnd* pControl, CWnd* pCaller, CRect& rcCaller, IPopupCtrlEvent* pEvents,
					  IPopupCtrlReciever* pReciever )
{
    if ( pControl == NULL )
    {
        delete this;
        return false;
    }
    m_pControl = pControl;
    m_pCaller = pCaller;
    m_pEvents = pEvents;
	

    if ( m_pEvents != NULL && !m_pEvents->OnInit() )
    {
        delete this;
        return false;
    }
    // Calculate the most appropriate position for the left-top corner of control
    // By default, at left and bottom of the caller
    CPoint pt (rcCaller.left, rcCaller.bottom);
    CWindowRect rcCtrl (m_pControl);

    // Alignment at right if necessary
    if ( pt.x + rcCtrl.Width() > ::GetSystemMetrics (SM_CXSCREEN) )
    {
        pt.x = rcCaller.right-rcCtrl.Width();
    }
    // Alignment at top if necessary
    if ( pt.y + rcCtrl.Height() > ::GetSystemMetrics (SM_CYSCREEN) )
    {
        pt.y = rcCaller.top-rcCtrl.Height();
    }
    // Adjustments to keep control into screen
    if ( pt.x + rcCtrl.Width() > ::GetSystemMetrics (SM_CXSCREEN) )
    {
        pt.x = ::GetSystemMetrics (SM_CXSCREEN)-rcCtrl.Width();
    }
    if ( pt.y + rcCtrl.Height() > ::GetSystemMetrics (SM_CYSCREEN) )
    {
        pt.y = ::GetSystemMetrics (SM_CYSCREEN)-rcCtrl.Height();
    }
    if ( pt.x < 0 ) pt.x = 0;
    if ( pt.y < 0 ) pt.y = 0;



    if ( !CMiniFrameWnd::Create (NULL, &afxChNil , WS_POPUP|(m_pCaller != NULL ? MFS_SYNCACTIVE : 0),
                                 CRect (pt.x, pt.y, pt.x+rcCtrl.Width(), pt.y+rcCtrl.Height()),
                                 m_pControl->GetParentFrame()) )
    {
        return false;
    }

	m_pEvents->SetReciever ( pReciever );

    m_hPrevParentCtrl = m_pControl->SetParent (this)->GetSafeHwnd();
    m_pControl->SetWindowPos (NULL, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_SHOWWINDOW);
    ShowWindow (SW_SHOW);

    if ( m_pEvents != NULL )
    {
        m_pEvents->OnShow();
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void CPopup::ChildWannaDead()
{
	EndPopup( true, true );
}

///////////////////////////////////////////////////////////////////////////////
void CPopup::EndPopup (bool bAbort, bool bSetFocus)
{
    if ( m_pEvents != NULL )
    {
        m_pEvents->OnHide (bAbort);
    }
    ASSERT (m_pControl->m_hWnd != NULL);

    m_pControl->ShowWindow (SW_HIDE);

    ASSERT (m_hPrevParentCtrl != NULL);
    ::SetParent (m_pControl->m_hWnd, m_hPrevParentCtrl);

    // Do a local copy of the pointer because DestroyWindow delete objet
    // and m_pCaller will not be valid after
    CWnd* pCaller = m_pCaller;

    if ( !m_bDestroy )
    {
        m_bDestroy = true;
        DestroyWindow();
    }
    if ( bSetFocus && pCaller != NULL )
    {
        pCaller->SetFocus();
    }
}

///////////////////////////////////////////////////////////////////////////////
BOOL CPopup::PreTranslateMessage (MSG* pMsg)
{
    bool bEnd = false, bAbort = false, bResult = false;
    int  nCode = doNothing;

    switch ( pMsg->message )
    {
        case WM_KEYDOWN:
            // Default action for <Escape> key
            if ( pMsg->wParam == VK_ESCAPE )
            {
                bEnd = bAbort = bResult = true;
                break;
            }
            if ( m_pEvents != NULL )
            {
                IPopupCtrlEvent* pEvents = m_pEvents;

                // Search for the interface for the correct object (referenced by pMsg->hwnd)
                if ( pMsg->hwnd != m_pControl->m_hWnd )
                {
                    pEvents = m_pEvents->GetInterfaceOf (pMsg->hwnd);
                }
                if ( pEvents != NULL )
                {
                    nCode = pEvents->OnKeyDown (pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
                }
            }
            break;

        case WM_LBUTTONDOWN:
            if ( m_pEvents != NULL )
            {
                IPopupCtrlEvent* pEvents = m_pEvents;

                // Search for the interface for the correct object
                if ( pMsg->hwnd != m_pControl->m_hWnd )
                {
                    pEvents = m_pEvents->GetInterfaceOf (pMsg->hwnd);
                }
                if ( pEvents != NULL )
                {
                    nCode = pEvents->OnLButtonDown (pMsg->wParam, CPoint ((short)LOWORD(pMsg->lParam), (short)HIWORD(pMsg->lParam)));
                }
            }
            break;

        case WM_LBUTTONUP:
            if ( m_pEvents != NULL )
            {
                IPopupCtrlEvent* pEvents = m_pEvents;

                // Search for the interface for the correct object
                if ( pMsg->hwnd != m_pControl->m_hWnd )
                {
                    pEvents = m_pEvents->GetInterfaceOf (pMsg->hwnd);
                }
                if ( pEvents != NULL )
                {
                    nCode = pEvents->OnLButtonUp (pMsg->wParam, CPoint ((short)LOWORD(pMsg->lParam), (short)HIWORD(pMsg->lParam)));
                }
            }
            break;
    }
    switch ( nCode )
    {
        case noSend:
            bResult = true;
            break;

        case end:
            bEnd = bResult = true;
            break;

        case abort:
            bEnd = bAbort = bResult = true;
            break;
    }
    if ( !bResult )
    {
        bResult = CMiniFrameWnd::PreTranslateMessage (pMsg) != 0;
    }
    if ( bEnd )
    {
        EndPopup (bAbort);
    }
    return bResult;
}

///////////////////////////////////////////////////////////////////////////////
void CPopup::PostNcDestroy ()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP (CPopup, CMiniFrameWnd)
    //{{AFX_MSG_MAP(CPopup)
    ON_WM_ACTIVATE()
    ON_WM_CANCELMODE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP ()

IMPLEMENT_DYNCREATE (CPopup, CMiniFrameWnd);

///////////////////////////////////////////////////////////////////////////////
void CPopup::OnActivate (UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    CMiniFrameWnd::OnActivate (nState, pWndOther, bMinimized);

    if ( nState == WA_INACTIVE && !m_bDestroy )
    {
        CFrameWnd* pFrame = NULL;
        bool       bSetFocus = false;

        if ( m_pCaller != NULL && (pFrame=m_pCaller->GetParentFrame()) != NULL && pWndOther != NULL )
        {
            bSetFocus = (pWndOther->m_hWnd == pFrame->m_hWnd);
        }
        EndPopup (true, bSetFocus);
    }
}

///////////////////////////////////////////////////////////////////////////////
void CPopup::OnCancelMode ()
{
    PostMessage (WM_DESTROY);
}
