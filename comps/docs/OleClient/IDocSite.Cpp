
#include "stdafx.h"

#include "AXDocSite.h"


/*
 * CAxDocSite::CImpIOleDocumentSite
 * CAxDocSite::~CImpIOleDocumentSite
 *
 * Parameters (Constructor):
 *  pSite           PCSite of the site we're in.
 *  pUnkOuter       LPUNKNOWN to which we delegate.
 */



/*
 * CAxDocSite::ActivateMe
 *
 * Purpose:
 *  Instructs the container to activate the object in this site as
 *  a document object.
 *
 * Parameters:
 *  pView           IOleDocumentView * of the object to activate.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, error code otherwise.
 */

STDMETHODIMP CAxDocSite::ActivateMe(IOleDocumentView *pView)
    {
    RECT                rc;
    IOleDocument*       pDoc;
    
    /*
     * If we're passed a NULL view pointer, then try to get one from
     * the document object (the object within us).
     */
    if (NULL==pView)
        {

        if (FAILED(m_pContainer->m_pObj->QueryInterface(IID_IOleDocument
            , (void **)&pDoc)))
            return E_FAIL;

        if (FAILED(pDoc->CreateView( (IOleInPlaceSite*)this
            , m_pContainer->m_pIStream, 0, &pView)))            
            return E_OUTOFMEMORY;

        // Release doc pointer since CreateView is a good com method that addrefs
        pDoc->Release();
        }        
    else
        {
        //Make sure that the view has our client site
        pView->SetInPlaceSite( (IOleInPlaceSite*)this );

        //We're holding onto the pointer, so AddRef it.
        pView->AddRef();
        }


    /*
     * Activation steps, now that we have a view:
     *
     *  1.  Call IOleDocumentView::SetInPlaceSite (assume done since
     *      either the view already knows, or IOleDocument::CreateView
     *      has done it already.
     *
     *  2.  Call IOleDocumentView::SetRect to give a bunch of space to
     *      the view.  In our case this is the whole client area of
     *      the CPages window.  (Patron doesn't use SetRectComplex)
     *
     *  3.  Call IOleDocumentView::Show to make the thing visible.
     *
     *  4.  Call IOleDocumentView::UIActivate to finish the job.
     *
     */

    //m_fDocObj=TRUE;
    m_pContainer->m_pIOleDocView=pView;
    
    //This sets up toolbars and menus first    
    pView->UIActivate(TRUE);

    //Set the window size sensitive to new toolbars
    GetClientRect( m_pContainer->m_hWndClient, &rc);
    pView->SetRect(&rc);

	//Makes it all active
    pView->Show(TRUE);    
    return NOERROR;
}
