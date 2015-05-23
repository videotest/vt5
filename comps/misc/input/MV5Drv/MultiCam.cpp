// MultyCam.cpp: implementation of the CMV500Grab class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MultiCam.h"
#include "MV500Grab.h"
#include <mv5.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

long CMultyCamThread::s_bRun = 0;


CMultyCamThread::CMultyCamThread()
{
//	m_hevStop = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CMultyCamThread::~CMultyCamThread()
{
//	CloseHandle(m_hevStop);
}


BOOL CMultyCamThread::InitInstance()
{
	while(s_bRun)
	{
		CMV500Grab::s_pGrab->OnTimer();
	}
	return FALSE;
}
