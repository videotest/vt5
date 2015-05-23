#include "stdafx.h"
#include "AXDocSite.h"


/*
 * Unused members in CImpIAdviseSink
 *  OnDataChange
 *  OnViewChange
 *  OnRename
 *  OnSave
 */

STDMETHODIMP_(void) CAxDocSite::OnDataChange(LPFORMATETC pFEIn
    , LPSTGMEDIUM pSTM)
    {
    return;
    }

STDMETHODIMP_(void) CAxDocSite::OnViewChange(DWORD dwAspect
    , LONG lindex)
    {    
    return;
    }

STDMETHODIMP_(void) CAxDocSite::OnRename(LPMONIKER pmk)
    {
    return;
    }

STDMETHODIMP_(void) CAxDocSite::OnSave(void)
    {
    return;
    }




/*
 * CAxDocSite::OnClose
 *
 * Purpose:
 *  Informs the advise sink that the OLE object has closed and is
 *  no longer bound in any way.  We use this to do a File/Close
 *  to delete the object since we don't hold onto any.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

STDMETHODIMP_(void) CAxDocSite::OnClose(void)
    {
	//This does the same as File/Close
	//Not quite because CFrame::Close requires some synchronous
	//calls that will fail if done here, so just post a message to
	//do this
    //m_pSite->m_pFR->Close();

	//ASSERT( FALSE );
	//Code coment
	/*
	HWND		hwnd;
	m_pSite->m_pFR->GetWindow(&hwnd);
	PostMessage(
		hwnd, 
		WM_COMMAND,
		MAKELONG(IDM_FILECLOSE, 0),
		0);
    return;
	*/
    }
