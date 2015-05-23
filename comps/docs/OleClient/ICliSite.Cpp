

#include "stdafx.h"

#include "AXDocSite.h"


/*
 * CAxDocSite::CImpIOleClientSite
 * CAxDocSite::~CImpIOleClientSite
 *
 * Parameters (Constructor):
 *  pSite           PCSite of the site we're in.
 *  pUnkOuter       LPUNKNOWN to which we delegate.
 */




/*
 * CAxDocSite::SaveObject
 *
 * Purpose:
 *  Requests that the container call OleSave for the object that
 *  lives here.  Typically this happens on server shutdown.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HRESULT         Standard.
 */

STDMETHODIMP CAxDocSite::SaveObject(void)
    {
    //Update();
    return NOERROR;
    }




/*
 * Unimplemented/trivial members
 *  GetMoniker
 *  GetContainer
 *  RequestNewObjectLayout
 *  OnShowWindow
 *  ShowObject
 */

STDMETHODIMP CAxDocSite::GetMoniker(DWORD dwAssign
    , DWORD dwWhich, LPMONIKER *ppmk)
    {
    return E_NOTIMPL;
    }

STDMETHODIMP CAxDocSite::GetContainer(LPOLECONTAINER
    * ppContainer)
    {
		*ppContainer=NULL;
		return E_NOINTERFACE;
    }

STDMETHODIMP CAxDocSite::RequestNewObjectLayout(void)
    {
    return E_NOTIMPL;
    }

STDMETHODIMP CAxDocSite::OnShowWindow(BOOL fShow)
    {
    return NOERROR;
    }

STDMETHODIMP CAxDocSite::ShowObject(void)
    {
    return NOERROR;
    }
