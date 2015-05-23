// GIFfile.cpp: implementation of the GIFfile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileFilters.h"
#include "ImageFileFilterBase.h"
#include "GIFfile.h"
#include "resource.h"



//#include <Magick++.h>
//using namespace std;
//using namespace Magick;


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CGIFileFilter, CCmdTargetEx);

// {0852B160-669B-11d3-A4EA-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CGIFileFilter, "FileFilters.GIFileFilter", 
0x852b160, 0x669b, 0x11d3, 0xa4, 0xea, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);


CGIFileFilter* gifFilter;
bool gifFirstTime;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIFileFilter::CGIFileFilter()
{
	_OleLockApp( this );
	m_strDocumentTemplate = _T("Image");
	m_strFilterName.LoadString( IDS_GIF_FILTERNAME );
	m_strFilterExt = _T(".gif");
	AddDataObjectType(szArgumentTypeImage);
	gifFilter = this;
}

CGIFileFilter::~CGIFileFilter()
{
	_OleUnlockApp( this );
	gifFilter = NULL;
}


void gifErrorHandler(const unsigned int error,const char *message, const char *qualifier)
{
  DestroyDelegateInfo();
  AfxMessageBox(message);
}


void gifMonitorHandler(const char *msg,const unsigned int curPos,const unsigned int maxPos)
{
	if(gifFilter == NULL) return;
	if(!gifFilter->m_bEnableMonitoring) return;
	if (gifFilter->m_bFirstTime)
	{
		gifFilter->m_bFirstTime = false;
		gifFilter->StartNotification(maxPos, 2);
	}
	gifFilter->Notify(curPos);
}


void CGIFileFilter::OnSetHandlers()
{
	//if(gifFilter == NULL)
		gifFilter = this;
	m_oldErrorHandle = SetErrorHandler(gifErrorHandler);
	m_oldMonitorHandle = SetMonitorHandler(gifMonitorHandler);
}


