#include "stdafx.h"
#include "syncactions.h"
#include "resource.h"
#include "utils.h"



//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionSyncContext, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSyncContextAll, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSyncScrollRelative, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSyncZoom, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSyncScroll, CCmdTargetEx);

// olecreate 

// {798689D1-EADE-44db-8CF1-1048BBD261DF}
GUARD_IMPLEMENT_OLECREATE(CActionSyncScrollRelative, "syncview.SyncScrollRelative",
0x798689d1, 0xeade, 0x44db, 0x8c, 0xf1, 0x10, 0x48, 0xbb, 0xd2, 0x61, 0xdf);
// {7205CAA0-210D-4b2e-A2F1-52360D1958CE}
GUARD_IMPLEMENT_OLECREATE(CActionSyncZoom, "syncview.SyncZoom",
0x7205caa0, 0x210d, 0x4b2e, 0xa2, 0xf1, 0x52, 0x36, 0xd, 0x19, 0x58, 0xce);
// {88EFB137-B10B-4a4b-A1C2-9A35254489DD}
GUARD_IMPLEMENT_OLECREATE(CActionSyncScroll, "syncview.SyncScroll",
0x88efb137, 0xb10b, 0x4a4b, 0xa1, 0xc2, 0x9a, 0x35, 0x25, 0x44, 0x89, 0xdd);
// {E1B3829F-97E1-4d36-B717-659A3E0F3066}
GUARD_IMPLEMENT_OLECREATE(CActionSyncContext, "syncview.SyncContext",
0xe1b3829f, 0x97e1, 0x4d36, 0xb7, 0x17, 0x65, 0x9a, 0x3e, 0xf, 0x30, 0x66);

// {77F6BCAE-8AA0-4f88-879F-DA915304599E}
GUARD_IMPLEMENT_OLECREATE(CActionSyncContextAll, "syncview.SyncContextAll",
0x77f6bcae, 0x8aa0, 0x4f88, 0x87, 0x9f, 0xda, 0x91, 0x53, 0x4, 0x59, 0x9e);

// guidinfo 

// {3ACE1187-A518-43a5-9DE9-48E0B0759DE0}
static const GUID guidSyncScrollRelative =
{ 0x3ace1187, 0xa518, 0x43a5, { 0x9d, 0xe9, 0x48, 0xe0, 0xb0, 0x75, 0x9d, 0xe0 } };
// {0C93651E-C4EC-4344-B8BB-133B3B78A42E}
static const GUID guidSyncZoom =
{ 0xc93651e, 0xc4ec, 0x4344, { 0xb8, 0xbb, 0x13, 0x3b, 0x3b, 0x78, 0xa4, 0x2e } };
// {2B9BC0A5-DDE0-4708-A0A3-10CEF56FA2C9}
static const GUID guidSyncScroll =
{ 0x2b9bc0a5, 0xdde0, 0x4708, { 0xa0, 0xa3, 0x10, 0xce, 0xf5, 0x6f, 0xa2, 0xc9 } };
// {78A06FA5-DA10-46c9-8848-96EDB73E04E8}
static const GUID guidSyncContext = 
{ 0x78a06fa5, 0xda10, 0x46c9, { 0x88, 0x48, 0x96, 0xed, 0xb7, 0x3e, 0x4, 0xe8 } };

// {50F6EE8A-45F0-42ed-BD3D-400113BC2B87}
static const GUID guidSyncContextAll = 
{ 0x50f6ee8a, 0x45f0, 0x42ed, { 0xbd, 0x3d, 0x40, 0x1, 0x13, 0xbc, 0x2b, 0x87 } };


//[ag]6. action info

ACTION_INFO_FULL(CActionSyncContext, IDS_ACTION_SYNCCONTEXT, -1, -1, guidSyncContext);
ACTION_INFO_FULL(CActionSyncContextAll, IDS_ACTION_SYNCCONTEXTALL, -1, -1, guidSyncContextAll);
ACTION_INFO_FULL(CActionSyncScrollRelative, IDS_ACTION_SYNCSCROLLRELATIVE, -1, -1, guidSyncScrollRelative);
ACTION_INFO_FULL(CActionSyncZoom, IDS_ACTION_SYNCZOOM, -1, -1, guidSyncZoom);
ACTION_INFO_FULL(CActionSyncScroll, IDS_ACTION_SYNCSCROLL, -1, -1, guidSyncScroll);

//[ag]7. targets

ACTION_TARGET(CActionSyncContext, "anydoc");
ACTION_TARGET(CActionSyncContextAll, "anydoc");
ACTION_TARGET(CActionSyncScrollRelative, "anydoc");
ACTION_TARGET(CActionSyncZoom, "anydoc");
ACTION_TARGET(CActionSyncScroll, "anydoc");

//[ag]8. arguments

ACTION_ARG_LIST(CActionSyncContext)
	ARG_STRING("Param", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionSyncContextAll)
	ARG_STRING("Param", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionSyncScroll)
	ARG_STRING("Param", 0)
END_ACTION_ARG_LIST()

// vanek - 26.08.2003
ACTION_ARG_LIST(CActionSyncScrollRelative)
	ARG_STRING("Param", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionSyncZoom)
	ARG_STRING("Param", 0)
END_ACTION_ARG_LIST()
// 

//[ag]9. implementation

static bool __Whether2ImageView(IUnknown *punkDoc, bool binclude_allview = false)
{
	IDocumentSitePtr ptrDS(punkDoc);
	TPOS pos;
	int nViews = 0;
	if (FAILED(ptrDS->GetFirstViewPosition(&pos)))
		return false;
	while (pos)
	{
		IUnknownPtr punkView;
		if (FAILED(ptrDS->GetNextView(&punkView, &pos)))
			break;
		if( binclude_allview )
			nViews++;
		else if ( CheckInterface(punkView,IID_IImageView) || CheckInterface(punkView,IID_IMeasureView) )
			nViews++;
	}
	return nViews >= 2;
}

//////////////////////////////////////////////////////////////////////
//CActionSyncContext implementation
CActionSyncContext::CActionSyncContext()
{
	IUnknown	*punk = ::GetSyncManager();
	m_ptrSyncManager = punk;

	if( punk )
		punk->Release();
}

CActionSyncContext::~CActionSyncContext()
{
}

bool CActionSyncContext::Invoke()
{
	CString strParam = GetArgumentString( "Param" );

	if( strParam.IsEmpty() )
	{
		if( IsChecked() ) m_ptrSyncManager->SetSyncContext( false );
		else m_ptrSyncManager->SetSyncContext( true );
	}
	else
	{
		int nVal = -1;
		if( strParam == "0" || strParam == "1" )
			nVal = atoi( strParam );

		if( nVal != -1)
		{
			if( !nVal ) m_ptrSyncManager->SetSyncContext( false );
			else m_ptrSyncManager->SetSyncContext( true );
		}
	}
	return true;
}

//extended UI
bool CActionSyncContext::IsAvaible()
{
	return m_ptrSyncManager!= 0 && __Whether2ImageView(m_punkTarget);
}

bool CActionSyncContext::IsChecked()
{
	if( m_ptrSyncManager==0 )
		return false;

	BOOL	bSync;
	m_ptrSyncManager->GetSyncContext( &bSync );
	return bSync == TRUE;
}

bool CActionSyncContextAll::IsAvaible()
{
	return m_ptrSyncManager!= 0;// && __Whether2ImageView(m_punkTarget, true);
}

//////////////////////////////////////////////////////////////////////
//CActionSyncScrollRelative implementation
CActionSyncScrollRelative::CActionSyncScrollRelative()
{
	IUnknown	*punk = ::GetSyncManager();
	m_ptrSyncManager = punk;

	if( punk )
		punk->Release();
}

CActionSyncScrollRelative::~CActionSyncScrollRelative()
{
}

bool CActionSyncScrollRelative::Invoke()
{
	// vanek - 26.08.2003
	/*if( IsChecked() ) m_ptrSyncManager->SetSyncMode( false, false, false, 0 );
	else m_ptrSyncManager->SetSyncMode( true, true, false, 0 );
	
	return true;*/

	CString strParam = GetArgumentString( "Param" );

	if( strParam.IsEmpty() )
	{
		if( IsChecked() ) m_ptrSyncManager->SetSyncMode( false, false, false, 0 );
		else m_ptrSyncManager->SetSyncMode( true, true, false, 0 );
	}
	else
	{
		int nVal = -1;
		if( strParam == "0" || strParam == "1" )
			nVal = atoi( strParam );

		if( nVal != -1)
		{
			if( !nVal ) m_ptrSyncManager->SetSyncMode( false, false, false, 0 );
			else m_ptrSyncManager->SetSyncMode( true, true, false, 0 );
		}
	}

	return true;
}

//extended UI
bool CActionSyncScrollRelative::IsAvaible()
{
	return m_ptrSyncManager!= 0 && __Whether2ImageView(m_punkTarget);
}

bool CActionSyncScrollRelative::IsChecked()
{
	if( m_ptrSyncManager==0 )
		return false;

	BOOL	bSync, bZoom, bRelative;
	m_ptrSyncManager->GetSyncMode( &bSync, &bZoom, &bRelative);
	return bSync == TRUE && bZoom == FALSE && bRelative == TRUE;
}

//////////////////////////////////////////////////////////////////////
//CActionSyncZoom implementation
CActionSyncZoom::CActionSyncZoom()
{
	IUnknown	*punk = ::GetSyncManager();
	m_ptrSyncManager = punk;

	if( punk )
		punk->Release();
}

CActionSyncZoom::~CActionSyncZoom()
{
}

bool CActionSyncZoom::Invoke()
{
	// vanek - 26.08.2003
	/*if( IsChecked() ) m_ptrSyncManager->SetSyncMode( false, false, false, 0 );
	else m_ptrSyncManager->SetSyncMode( true, false, true, 0 );

	return true;*/

	CString strParam = GetArgumentString( "Param" );

	if( strParam.IsEmpty() )
	{
		if( IsChecked() ) m_ptrSyncManager->SetSyncMode( false, false, false, 0 );
		else m_ptrSyncManager->SetSyncMode( true, false, true, 0 );
	}
	else
	{
		int nVal = -1;
		if( strParam == "0" || strParam == "1" )
			nVal = atoi( strParam );

		if( nVal != -1)
		{
			if( !nVal ) m_ptrSyncManager->SetSyncMode( false, false, false, 0 );
			else m_ptrSyncManager->SetSyncMode( true, false, true, 0 );
		}
	}

	return true;
}

//extended UI
bool CActionSyncZoom::IsAvaible()
{
	return m_ptrSyncManager!= 0 && __Whether2ImageView(m_punkTarget);
}
bool CActionSyncZoom::IsChecked()
{
	if( m_ptrSyncManager==0 )
		return false;

	BOOL	bSync, bZoom, bRelative;
	m_ptrSyncManager->GetSyncMode( &bSync, &bZoom, &bRelative );
	return bSync && bZoom && !bRelative;
}


//////////////////////////////////////////////////////////////////////
//CActionSyncScroll implementation
CActionSyncScroll::CActionSyncScroll()
{
	IUnknown	*punk = ::GetSyncManager();
	m_ptrSyncManager = punk;

	if( punk )
		punk->Release();
}

CActionSyncScroll::~CActionSyncScroll()
{
}

bool CActionSyncScroll::Invoke()
{
	CString strParam = GetArgumentString( "Param" );

	if( strParam.IsEmpty() )
	{
		if( IsChecked() ) m_ptrSyncManager->SetSyncMode( false, false, false, 0 );
		else m_ptrSyncManager->SetSyncMode( true, false, false, 0 );
	}
	else
	{
		int nVal = -1;
		if( strParam == "0" || strParam == "1" )
			nVal = atoi( strParam );

		if( nVal != -1)
		{
			if( !nVal ) m_ptrSyncManager->SetSyncMode( false, false, false, 0 );
			else m_ptrSyncManager->SetSyncMode( true, false, false, 0 );
		}
	}

	return true;
}


bool CActionSyncScroll::IsAvaible()
{
	return m_ptrSyncManager!= 0 && __Whether2ImageView(m_punkTarget);
}

bool CActionSyncScroll::IsChecked()
{
	if( m_ptrSyncManager==0 )
		return false;

	BOOL	bSync, bZoom, bRelative;
	m_ptrSyncManager->GetSyncMode( &bSync, &bZoom, &bRelative );
	return bSync == TRUE && bZoom == FALSE && bRelative == FALSE;
}
