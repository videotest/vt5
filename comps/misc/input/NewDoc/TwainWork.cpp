#include "StdAfx.h"
#include "TwainWork.h"


CTwainWork::CTwainWork()
{
	m_pDataSource = NULL;
	m_bUIControllable = false;
}

CTwainWork::~CTwainWork()
{
}

HGLOBAL CTwainWork::ScanNative(CTwainDataSource *pDataSource, BOOL fShowUI)
{
	HGLOBAL hMem = NULL;
	m_pDataSource = pDataSource;
	if(!InitDS(TWSX_NATIVE,fShowUI)) return hMem;
	if (!m_bUIControllable) fShowUI = TRUE;
	hMem = DoNativeXfer(fShowUI);
	DeinitDS();
	m_pDataSource = NULL;
	return hMem;
}


int CTwainWork::WaitForAquisition()
{
	TW_EVENT 	twEvent;
	TW_INT16	rc;
	MSG msg;

	while (GetMessage ( (LPMSG) &msg, NULL, 0, 0) )
	{
		twEvent.pEvent = (TW_MEMREF)&msg;
		twEvent.TWMessage = MSG_NULL;
		rc = m_pDataSource->CallDSM(DG_CONTROL, DAT_EVENT, MSG_PROCESSEVENT,
			(TW_MEMREF)&twEvent);

		TRACE("TWMessage (%d,%d)\n", int(twEvent.TWMessage), int(rc));
		// check for message from Source
		switch (twEvent.TWMessage)
		{
			case MSG_XFERREADY:
				return IDOK;
			case MSG_CLOSEDSREQ:
				return IDCANCEL;
		}

//		if (rc == TWRC_NOTDSEVENT)
		if (rc != TWRC_DSEVENT)
		{
			TranslateMessage( (LPMSG) &msg);
			DispatchMessage( (LPMSG) &msg);
		}
	}
	return IDCANCEL;
}

BOOL CTwainWork::DoEndXfer()
{
	TW_PENDINGXFERS	twPendingXfers;
	// If the return code from DG_IMAGE/DAT_IMAGENATIVEXFER/MSG_GET was     // TWRC_CANCEL or TWRC_DONE
	// Acknowledge the end of the transfer
	int rc = m_pDataSource->CallDSM(DG_CONTROL, DAT_PENDINGXFERS, MSG_ENDXFER,
		(TW_MEMREF)&twPendingXfers);
   	if (rc == TWRC_SUCCESS)
	{
		// Check for additional pending xfers
		if (twPendingXfers.Count == 0)
		{
			// Source is now in state 5. NOTE THE IMPLIED STATE        // TRANSITION! Disable and close the source and
			// return to TransferNativeImage with a FALSE notifiing it // to not attempt further image transfers.
			return FALSE;
		}
		else
		{
			// Source is in state 6 ready to transfer another image
			return TRUE;
		}		
	}
	return TRUE;
}

void CTwainWork::DoAbortXfer()
{
	TW_PENDINGXFERS	twPendingXfers;
	m_pDataSource->CallDSM(DG_CONTROL,	DAT_PENDINGXFERS, MSG_RESET,
		(TW_MEMREF)&twPendingXfers);
}

HGLOBAL CTwainWork::DoNativeXfer(BOOL fShowUI)
{
	HGLOBAL hMem = NULL;
	m_pDataSource->Enable(m_pDataSource->GetDSM()->GetParent(),	fShowUI);
	if (WaitForAquisition() == IDOK)
	{
		TW_IMAGEINFO twImageInfo;
		TW_UINT16    rc;
		TW_UINT32    hBitmap;
		TW_BOOL      PendingXfers = TRUE;
 
		while (PendingXfers)
		{
			rc = m_pDataSource->CallDSM(DG_IMAGE, DAT_IMAGEINFO, MSG_GET,
					(TW_MEMREF)&twImageInfo);

			if (rc == TWRC_SUCCESS)
			{
				//Examine the image information

				// Transfer the image natively
				hBitmap = NULL;

				rc = m_pDataSource->CallDSM(DG_IMAGE, DAT_IMAGENATIVEXFER,
						MSG_GET, (TW_MEMREF)&hBitmap);

				// Check the return code

				switch(rc)
				{
				case TWRC_XFERDONE:
	
					// Notes: hBitmap points to a valid image Native image (DIB or PICT)
					// The application is now responsible for deallocating the memory.
					// The source is currently in state 7.
					// The application must now acknowledge the end of the transfer, 
					// determine if other transfers are pending and shut down the data
					// source.
					PendingXfers = DoEndXfer();
					hMem = (HGLOBAL)hBitmap;
					break;
	
				case TWRC_CANCEL:
					// The user canceled the transfer.
					// hBitmap is an invalid handle but memory was allocated.  Application
					// is responsible for deallocating the memory.
					// The source is still in state 7.
					// The application must check for pending transfers and shut down 
					// the data source.
					PendingXfers = DoEndXfer();
					break;

				case TWRC_FAILURE:
					// The transfer failed for some reason.
					// hBitmap is invalid and no memory was allocated.
					// Condition code will contain more information as to the cause of
					// the failure.
					// The state transition failed, the source is in state 6.
					// The image data is still pending.
					// The application should abort the transfer.
					DoAbortXfer();
					PendingXfers = FALSE;
					break;
				}
			} // if (rc == TWRC_SUCCESS)
		} // while (PendingXfers)
		// Now source in state 5.
	} // if (WaitForAquisition() == IDOK)
	m_pDataSource->Disable(m_pDataSource->GetDSM()->GetParent(), fShowUI);
	return hMem;
}

/*
void CTwainWork::DoMemoryXfer(BOOL fShowUI)
{
	m_pDataSource->Enable(m_pDataSource->GetDSM()->GetParent(), fShowUI);
	if (m_pDriver->m_nHdwFlags & FG_PERMTRANSFER ||
		WaitForAquisition() == IDOK)
	{
		CImageInfo ImageInfo;
		TW_SETUPMEMXFER SetupMemXfer;
		long nCols,nColNum;

		m_pDataSource->GetImageInfo(ImageInfo);
		m_img.Create(ImageInfo.GetWidth(), ImageInfo.GetHeight(),
			ImageInfo.IsTrueColor() ? CImageBase::TrueColor : 
			CImageBase::GrayScale);
		m_pDataSource->SetupMemXfer(SetupMemXfer);
		nColNum = m_img.GetCY() - 1;
		nCols = SetupMemXfer.Preferred / (m_img.GetCols() *
			m_img.GetIncr());
		COLORREPR *ptr = m_img.GetBitsPtr();
		while (1)
		{
			CImageMemXfer ImageMemXfer(TWMF_APPOWNS | TWMF_POINTER,
				SetupMemXfer.Preferred, ptr);
			int rc = m_pDataSource->CallDSM(DG_IMAGE, DAT_IMAGEMEMXFER,
				MSG_GET, (TW_MEMREF)&ImageMemXfer);
			if (rc == TWRC_XFERDONE || rc == TWRC_CANCEL)
			{
				TW_PENDINGXFERS twPendingXfer;
				int rc = m_pDataSource->CallDSM(DG_CONTROL, DAT_PENDINGXFERS,
					MSG_ENDXFER, (TW_MEMREF)&twPendingXfer);
				if (twPendingXfer.Count == 0)
					break;
			}
			m_pDataSource->TwainCheckStatus(rc);
			ptr += ImageMemXfer.GetBytes()/sizeof(COLORREPR);
			NotifyParent(MAKELONG(nColNum - nCols,nColNum));
		}
	}
	m_img.SetFlag(TRUE);
	CDIBArgument *pArg = (CDIBArgument *)GetResult();
	pArg->m_lpbi = m_img.GetBI();
	m_pDataSource->Disable(m_pDataSource->GetDSM()->GetParent(),
		! ( m_pDriver->m_nHdwFlags & FG_PERMTRANSFER ) );
}
*/

bool CTwainWork::InitDS(int nXferMech, BOOL fShowUI)
{
	try
	{
		m_pDataSource->Open();
	}
	catch(CException *e)
	{
		e->Delete();
		return false;
	}
	catch (...)
	{
		return false;
	}
	try
	{
		m_pDataSource->SetXferMech(nXferMech);
	}
	catch (CException *e)
	{
		e->Delete();
		return false;
	}
	catch (...)
	{
		return false;
	}
	if (!m_pDataSource->GetDeviceOnlineNoThrow(TRUE))
	{
		m_pDataSource->Close();
		return false;
	}
	if (!fShowUI)
		m_bUIControllable = m_pDataSource->GetUIControllableNoThrow(FALSE);
	return true;
}

void CTwainWork::DeinitDS()
{
	m_pDataSource->Close();
}







