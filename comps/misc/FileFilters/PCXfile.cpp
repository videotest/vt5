// PCXfile.cpp : implementation file
//

#include "stdafx.h"
#include "FileFilters.h"
#include "ImageFileFilterBase.h"
#include "PCXfile.h"
#include "resource.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPCXFileFilter


IMPLEMENT_DYNCREATE(CPCXFileFilter, CCmdTargetEx)

// {A1618777-6AA5-11D3-A4ED-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CPCXFileFilter, "FileFilters.PCXFileFilter", 0xa1618777, 0x6aa5, 0x11d3, 0xa4, 0xed, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87)

CPCXFileFilter* pcxFilter;
bool pcxFirstTime;


CPCXFileFilter::CPCXFileFilter()
{
	_OleLockApp( this );
	m_strDocumentTemplate = _T("Image");
	m_strFilterName.LoadString( IDS_PCX_FILTERNAME );
	m_strFilterExt = _T(".pcx");
	AddDataObjectType(szArgumentTypeImage);
	pcxFilter = this;
}

CPCXFileFilter::~CPCXFileFilter()
{
	_OleUnlockApp( this );
	pcxFilter = NULL;
}


void pcxErrorHandler(const MagickCore::ExceptionType severity
										 ,const char *reason,const char *description)
{
	AfxMessageBox(CString(reason)+' '+description);
}

MagickBooleanType pcxMonitorHandler(const char *msg,const MagickOffsetType curPos,
		const MagickSizeType maxPos,void * client_data)
{
	if(pcxFilter == NULL) return MagickFalse;
	if(!pcxFilter->m_bEnableMonitoring) return MagickFalse;
	if (pcxFilter->m_bFirstTime)
	{
		pcxFilter->m_bFirstTime = false;
		pcxFilter->StartNotification((int)maxPos, 2);
	}
	pcxFilter->Notify((int)curPos);
	return MagickTrue;
}


void CPCXFileFilter::OnSetHandlers()
{
	//if(pcxFilter == NULL)
		pcxFilter = this;
	m_oldErrorHandle = SetErrorHandler(pcxErrorHandler);
	m_oldMonitorHandle = SetImageInfoProgressMonitor(&m_image_info, pcxMonitorHandler, this);
}


