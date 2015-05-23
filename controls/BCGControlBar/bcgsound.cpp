//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This source code is a part of BCGControlBar library.
// You may use, compile or redistribute it as part of your application 
// for free. You cannot redistribute it as a part of a software development 
// library without the agreement of the author. If the sources are 
// distributed along with the application, you should leave the original 
// copyright notes in the source code without any changes.
// This code can be used WITHOUT ANY WARRANTIES on your own risk.
// 
// For the latest updates to this library, check my site:
// http://welcome.to/bcgsoft
// 
// Stas Levin <bcgsoft@yahoo.com>
//*******************************************************************************

#include "stdafx.h"
#include <process.h>    
#include <afxmt.h>

#include "bcgsound.h"

static int g_nBCGSoundState = BCGSOUND_NOT_STARTED;
static HANDLE g_hThreadSound = NULL;
static const int nThreadDelay = 5;

void BCGSoundThreadProc (LPVOID)
{
	while (g_nBCGSoundState != BCGSOUND_TERMINATE)
	{
		switch (g_nBCGSoundState)
		{
		case BCGSOUND_TERMINATE:
			break;

		case BCGSOUND_MENU_COMMAND:
			::PlaySound (_T("MenuCommand"), NULL, SND_SYNC | SND_NODEFAULT | SND_ALIAS);
			g_nBCGSoundState = BCGSOUND_IDLE;
			break;

		case BCGSOUND_MENU_POPUP:
			::PlaySound (_T("MenuPopup"), NULL, SND_SYNC | SND_NODEFAULT | SND_ALIAS);
			g_nBCGSoundState = BCGSOUND_IDLE;
		}

		::Sleep (nThreadDelay);
	}

	_endthread();
}
//*****************************************************************************************
void BCGPlaySystemSound (int nSound)
{
	if (g_nBCGSoundState == BCGSOUND_NOT_STARTED)
	{
		if (nSound == BCGSOUND_TERMINATE)
		{
			return;
		}

		static CCriticalSection cs;
		cs.Lock ();

		ASSERT (g_hThreadSound == NULL);

		//-------------------------
		// Initialize sound thread:
		//-------------------------
		g_hThreadSound = (HANDLE) ::_beginthread (BCGSoundThreadProc, 0, NULL);
		if (g_hThreadSound > 0 && g_hThreadSound != (HANDLE) -1)
		{
			::SetThreadPriority (g_hThreadSound, THREAD_PRIORITY_BELOW_NORMAL);
			g_nBCGSoundState = nSound;
		}
		else
		{
			g_hThreadSound = NULL;
		}

		cs.Unlock ();
	}
	else
	{
		g_nBCGSoundState = nSound;
		if (g_nBCGSoundState == BCGSOUND_TERMINATE)
		{
			//------------------------
			// Terminate sound thread:
			//------------------------
			g_hThreadSound = NULL;
		}
	}
}
