#include "stdafx.h"
#include "AxDocContainer.h"
#include "ole_utils.h"
#include "resource.h"
#include "misc_utils.h"
#include "constant.h"
#include "oleitem.h"
#include "wndmisc5.h"

/*
 * IOleInPlaceFrame implementation
 */


/*
 * CAxDocContainer::GetWindow
 *
 * Purpose:
 *  Retrieves the handle of the window associated with the object
 *  on which this interface is implemented.
 *
 * Parameters:
 *  phWnd           HWND * in which to store the window handle.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, E_FAIL if there is no
 *                  window.
 */

STDMETHODIMP CAxDocContainer::GetWindow(HWND *phWnd)
{
    *phWnd=m_hwndMain;
    return NOERROR;
}




/*
 * CAxDocContainer::ContextSensitiveHelp
 *
 * Purpose:
 *  Instructs the object on which this interface is implemented to
 *  enter or leave a context-sensitive help mode.
 *
 * Parameters:
 *  fEnterMode      BOOL TRUE to enter the mode, FALSE otherwise.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP CAxDocContainer::ContextSensitiveHelp(BOOL fEnterMode)
    {
    /*
     * Don't bother if there is no active object since we don't do
     * context help on our own.
     */
    if (NULL==m_pIOleIPActiveObject)
        return NOERROR;

    //If the state changes, notify the active object.
	/*
    if (m_fInContextHelp!=fEnterMode)
        {
        m_fInContextHelp=fEnterMode;
        m_pIOleIPActiveObject->ContextSensitiveHelp(fEnterMode);
        }
		*/

    return NOERROR;
    }




/*
 * CAxDocContainer::GetBorder
 *
 * Purpose:
 *  Returns the rectangle in which the container is willing to
 *  negotiate about an object's adornments.
 *
 * Parameters:
 *  prcBorder       LPRECT in which to store the rectangle.
 *
 * Return Value:
 *  HRESULT         NOERROR if all is well, INPLACE_E_NOTOOLSPACE
 *                  if there is no negotiable space.
 */

STDMETHODIMP CAxDocContainer::GetBorder(LPRECT prcBorder)
    {
    if (NULL==prcBorder)
        return E_INVALIDARG;


	IOleFramePtr ptrOF = ::GetMainFrame();
	if( ptrOF )
		ptrOF->GetPseudoClientRect( prcBorder, TRUE );

    return NOERROR;
    }




/*
 * CAxDocContainer::RequestBorderSpace
 *
 * Purpose:
 *  Asks the container if it can surrender the amount of space
 *  in pBW that the object would like for it's adornments.  The
 *  container does nothing but validate the spaces on this call.
 *
 * Parameters:
 *  pBW             LPCBORDERWIDTHS containing the requested space.
 *                  The values are the amount of space requested
 *                  from each side of the relevant window.
 *
 * Return Value:
 *  HRESULT         NOERROR if we can give up space,
 *                  INPLACE_E_NOTOOLSPACE otherwise.
 */

STDMETHODIMP CAxDocContainer::RequestBorderSpace(LPCBORDERWIDTHS pBW)
    {
    //Framer has no border space restrictions
    return NOERROR;
    }




/*
 * CAxDocContainer::SetBorderSpace
 *
 * Purpose:
 *  Called when the object now officially requests that the
 *  container surrender border space it previously allowed
 *  in RequestBorderSpace.  The container should resize windows
 *  appropriately to surrender this space.
 *
 * Parameters:
 *  pBW             LPCBORDERWIDTHS containing the amount of space
 *                  from each side of the relevant window that the
 *                  object is now reserving.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP CAxDocContainer::SetBorderSpace(LPCBORDERWIDTHS pBW)
    {

    /*
     * Since we have no tools, we can accept anything the object sends
     * and must therefore adjust the client-area window accordingly.
     */

    /*
     * If pBW is NULL, the object is not interested in tools, so we
     * don't have to do anything.  In either case we need to save
     * the toolspace allocations in order to resize the client window
     * correctly.
     */

	IOleFramePtr ptrOF = ::GetMainFrame();
	if( ptrOF == 0 )
		return E_FAIL;

	RECT rc;
    if (NULL==pBW)
    {
        //if (!m_fOurMenuShowing)
            SetMenu(NULL, NULL, NULL);

        SetRect(&m_bwIP, 0, 0, 0, 0);
		ptrOF->GetPseudoClientRect( &rc, TRUE );
    }
    else
	{
		ptrOF->GetPseudoClientRect( &rc, TRUE );
		
		rc.left  +=pBW->left;
		rc.right -=pBW->right;
		rc.top   +=pBW->top;
		rc.bottom-=pBW->bottom;

		m_bwIP=*pBW;
    }
							 
	ptrOF->SetOleBorders( pBW );
	
    return NOERROR;
    }




/*
 * CAxDocContainer::SetActiveObject
 *
 * Purpose:
 *  Provides the container with the object's IOleInPlaceActiveObject
 *  pointer
 *
 * Parameters:
 *  pIIPActiveObj   LPOLEINPLACEACTIVEOBJECT of interest.
 *  pszObj          LPCOLESTR naming the object.  Not used.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP CAxDocContainer::SetActiveObject
    (LPOLEINPLACEACTIVEOBJECT pIIPActiveObj, LPCOLESTR pszObj)
    {
    if (NULL!=m_pIOleIPActiveObject)
        m_pIOleIPActiveObject->Release();

    //NULLs m_pIOleIPActiveObject if pIIPActiveObj is NULL
    m_pIOleIPActiveObject=pIIPActiveObj;

    if (NULL!=m_pIOleIPActiveObject)
        m_pIOleIPActiveObject->AddRef();

	if( m_pIOleIPActiveObject )
		m_pIOleIPActiveObject->GetWindow(&m_hWndObj);

    return NOERROR;
    }




/*
 * CAxDocContainer::InsertMenus
 *
 * Purpose:
 *  Instructs the container to place its in-place menu items where
 *  necessary in the given menu and to fill in elements 0, 2, and 4
 *  of the OLEMENUGROUPWIDTHS array to indicate how many top-level
 *  items are in each group.
 *
 * Parameters:
 *  hMenu           HMENU in which to add popups.
 *  pMGW            LPOLEMENUGROUPWIDTHS in which to store the
 *                  width of each container menu group.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP CAxDocContainer::InsertMenus(HMENU hMenu
    , LPOLEMENUGROUPWIDTHS pMGW)
{    
	ClearMenu();
	IOleFramePtr ptrOF = ::GetMainFrame();	

	BSTR pbstr[OLE_MENU_SIZE];
	::ZeroMemory( pbstr, sizeof(pbstr) );

	ptrOF->GetMenu( m_hFrameMenu, pbstr );

	for( int i=0;i<OLE_MENU_SIZE;i++ )
	{
		if( pbstr[i] )
		{
			m_bstrMenuCaption[i] = pbstr[i];
			::SysFreeString( pbstr[i] );				
		}

		if( m_hFrameMenu[i] != NULL )
		{
			InsertMenu(hMenu, 0, MF_BYPOSITION | MF_POPUP, (UINT)m_hFrameMenu[i], m_bstrMenuCaption[i] );
			pMGW->width[i]=1;//::GetMenuItemCount( m_hFrameMenu[i] );
			//pMGW->width[i]=2;
		}
		//else
			//pMGW->width[i]=0;
	}
		
    return NOERROR;
}




/*
 * CAxDocContainer::SetMenu
 *
 * Purpose:
 *  Instructs the container to replace whatever menu it's currently
 *  using with the given menu and to call OleSetMenuDescritor so OLE
 *  knows to whom to dispatch messages.
 *
 * Parameters:
 *  hMenu           HMENU to show.
 *  hOLEMenu        HOLEMENU to the menu descriptor.
 *  hWndObj         HWND of the active object to which messages are
 *                  dispatched.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP CAxDocContainer::SetMenu(HMENU hMenu, HOLEMENU hOLEMenu
    , HWND hWndObj)
    {
    HRESULT         hr;

    if (NULL!=hMenu)
	{		
        ::SetMenu(m_hwndMain, hMenu);
	}

    DrawMenuBar(m_hwndMain);
	
	hr=OleSetMenuDescriptor(hOLEMenu, m_hwndMain, hWndObj, NULL, NULL);
	
    return hr;
    }




/*
 * CAxDocContainer::RemoveMenus
 *
 * Purpose:
 *  Asks the container to remove any menus it put into hMenu in
 *  InsertMenus.
 *
 * Parameters:
 *  hMenu           HMENU from which to remove the container's
 *                  items.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP CAxDocContainer::RemoveMenus(HMENU hMenu)
    {
    int         cItems;//, i, j;
    //HMENU       hMenuT;

	/*
     * To be defensive, loop through this menu removing anything
     * we recognize (that is, anything in m_phMenu) just in case
     * the server didn't clean it up right.  At least we can
     * give ourselves the prophylactic benefit.
     */

    if (NULL==hMenu)
        return NOERROR;

    cItems=GetMenuItemCount(hMenu);

    for(int i=cItems; i >=0; i--)
    {
		HMENU hMenuT=GetSubMenu(hMenu, i);
        for ( int j=0; j <= OLE_MENU_SIZE; j++)
        {
			//Remove any owned popup, or our special help menu
			if (hMenuT==m_hFrameMenu[j] )
                RemoveMenu(hMenu, i, MF_BYPOSITION);            
        }
	}

    /*
     * Walk backwards down the menu.  For each popup, see if it
     * matches any other popup we know about, and if so, remove
     * it from the shared menu.
     */

	//ASSERT( FALSE );
	//Code commented
	/*
    for (i=cItems; i >=0; i--)
        {
        hMenuT=GetSubMenu(hMenu, i);

        for (j=0; j <= CMENUS; j++)
            {
			//Remove any owned popup, or our special help menu
            if (hMenuT==m_phMenu[j]
                || (hMenuT==m_hMenuHelp && m_hMenuHelp!=NULL))
                RemoveMenu(hMenu, i, MF_BYPOSITION);
            }
        }
	*/
	
	//m_fUsingOurHelp=FALSE;

    //The menu should now be empty.
    return NOERROR;
    }




/*
 * CAxDocContainer::SetStatusText
 *
 * Purpose:
 *  Asks the container to place some text in a status line, if one
 *  exists.  If the container does not have a status line it
 *  should return E_FAIL here in which case the object could
 *  display its own.
 *
 * Parameters:
 *  pszText         LPCOLESTR to display.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, S_TRUNCATED if not all
 *                  of the text could be displayed, or E_FAIL if
 *                  the container has no status line.
 */

STDMETHODIMP CAxDocContainer::SetStatusText(LPCOLESTR pszText)
    {
    //We have no status line...
    return E_NOTIMPL;
    }



/*
 * CAxDocContainer::EnableModeless
 *
 * Purpose:
 *  Instructs the container to show or hide any modeless popup
 *  windows that it may be using.
 *
 * Parameters:
 *  fEnable         BOOL indicating to enable/show the windows
 *                  (TRUE) or to hide them (FALSE).
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP CAxDocContainer::EnableModeless(BOOL fEnable)
    {
    return NOERROR;
    }




/*
 * CAxDocContainer::TranslateAccelerator
 *
 * Purpose:
 *  When dealing with an in-place object from an EXE server, this
 *  is called to give the container a chance to process accelerators
 *  after the server has looked at the message.
 *
 * Parameters:
 *  pMSG            LPMSG for the container to examine.
 *  wID             WORD the identifier in the container's
 *                  accelerator table (from IOleInPlaceSite
 *                  ::GetWindowContext) for this message (OLE does
 *                  some translation before calling).
 *
 * Return Value:
 *  HRESULT         NOERROR if the keystroke was used,
 *                  S_FALSE otherwise.
 */

STDMETHODIMP CAxDocContainer::TranslateAccelerator(LPMSG pMSG, WORD wID)
    {
    /*
     * wID already has anything translated from m_hAccel for us,
     * so we can just check for the commands we want and process
     * them instead of calling TranslateAccelerator which would be
     * redundant and which also has a possibility of dispatching to
     * the wrong window.
     */
	//ASSERT( FALSE );
	//Code comented
	/*

    if (IDM_ENTERCONTEXTHELP==wID || IDM_ESCAPECONTEXTHELP==wID)
        {
        //wID properly expands to 32-bits
        OnCommand(m_hWnd, (WPARAM)wID, 0L);
        return NOERROR;
        }

  */

	
	ITranslatorWndPtr	ptrTW( ::GetMainFrame() );

	BOOL bRes = FALSE;
	if( ptrTW != 0 )
		ptrTW->TranslateAccelerator( pMSG, &bRes );
		

    return ( bRes ? S_OK : S_FALSE );

    }
	


/*
 * IOleCommandTarget methods, provided to make PowerPoint happy
 * with this frame.
 */

STDMETHODIMP CAxDocContainer::QueryStatus(const GUID *pguidCmdGroup, ULONG cCmds
    , OLECMD *prgCmds, OLECMDTEXT *pCmdText)
	{
	return OLECMDERR_E_UNKNOWNGROUP;
	}
        
STDMETHODIMP CAxDocContainer::Exec(const GUID *pguidCmdGroup, DWORD nCmdID
    , DWORD nCmdexecopt, VARIANTARG *pvaIn, VARIANTARG *pvaOut)
	{
	return OLECMDERR_E_UNKNOWNGROUP;
	}

