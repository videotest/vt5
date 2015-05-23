#include "stdafx.h"
#include "resource.h"
#include "AVIActions.h"
#include "VT5AVI.h"
#include "InputUtils.h"

IMPLEMENT_DYNCREATE(CActionAVIFirst, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAVIPrev, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAVINext, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAVILast, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAVICapture, CCmdTargetEx);



//olecreate

// {E463A92A-59A5-4A8D-A5D3-6B051E6440DD}
GUARD_IMPLEMENT_OLECREATE(CActionAVIFirst, "VideoDoc.First", 
0xe463a92a, 0x59a5, 0x4a8d, 0xa5, 0xd3, 0x6b, 0x5, 0x1e, 0x64, 0x40, 0xdd);

// {C5E2733C-2058-4F10-BB21-6ACD096F6F90}
GUARD_IMPLEMENT_OLECREATE(CActionAVIPrev, "VideoDoc.Prev", 
0xc5e2733c, 0x2058, 0x4f10, 0xbb, 0x21, 0x6a, 0xcd, 0x9, 0x6f, 0x6f, 0x90);

// {1D1B8242-80BD-4BF3-879C-D3B9408AFD80}
GUARD_IMPLEMENT_OLECREATE(CActionAVINext, "VideoDoc.Next", 
0x1d1b8242, 0x80bd, 0x4bf3, 0x87, 0x9c, 0xd3, 0xb9, 0x40, 0x8a, 0xfd, 0x80);

// {880972EC-395E-45CB-A06E-9FB97F56E20C}
GUARD_IMPLEMENT_OLECREATE(CActionAVILast, "VideoDoc.Last", 
0x880972ec, 0x395e, 0x45cb, 0xa0, 0x6e, 0x9f, 0xb9, 0x7f, 0x56, 0xe2, 0xc);

// {16005B1F-1EB8-4FE3-B267-643B4ADACEEA}
GUARD_IMPLEMENT_OLECREATE(CActionAVICapture, "VideoDoc.Capture", 
0x16005b1f, 0x1eb8, 0x4fe3, 0xb2, 0x67, 0x64, 0x3b, 0x4a, 0xda, 0xce, 0xea);

//guidinfo 

// {9FD599CD-E48E-4773-8753-BBE3F4E06D5D}
static const GUID guidAVIFirst = 
{ 0x9fd599cd, 0xe48e, 0x4773, { 0x87, 0x53, 0xbb, 0xe3, 0xf4, 0xe0, 0x6d, 0x5d } };

// {060B05C3-C52F-4595-89DC-5EFFC57BD69D}
static const GUID guidAVIPrev = 
{ 0x60b05c3, 0xc52f, 0x4595, { 0x89, 0xdc, 0x5e, 0xff, 0xc5, 0x7b, 0xd6, 0x9d } };

// {DC1603FC-2831-40CE-A776-97859B33885F}
static const GUID guidAVINext = 
{ 0xdc1603fc, 0x2831, 0x40ce, { 0xa7, 0x76, 0x97, 0x85, 0x9b, 0x33, 0x88, 0x5f } };

// {93475641-4FFF-4093-A497-67607A4B1FFB}
static const GUID guidAVILast = 
{ 0x93475641, 0x4fff, 0x4093, { 0xa4, 0x97, 0x67, 0x60, 0x7a, 0x4b, 0x1f, 0xfb } };

// {35BEE561-A9F4-4A56-A694-6402F5735F64}
static const GUID guidAVICapture = 
{ 0x35bee561, 0xa9f4, 0x4a56, { 0xa6, 0x94, 0x64, 0x2, 0xf5, 0x73, 0x5f, 0x64 } };


//[ag]6. action info

ACTION_INFO_FULL(CActionAVIFirst, IDS_ACTION_AVI_FIRST, IDS_MENU_AVI, IDS_TB_VIDEO, guidAVIFirst);
ACTION_INFO_FULL(CActionAVIPrev, IDS_ACTION_AVI_PREV, IDS_MENU_AVI, IDS_TB_VIDEO, guidAVIPrev);
ACTION_INFO_FULL(CActionAVINext, IDS_ACTION_AVI_NEXT, IDS_MENU_AVI, IDS_TB_VIDEO, guidAVINext);
ACTION_INFO_FULL(CActionAVILast, IDS_ACTION_AVI_LAST, IDS_MENU_AVI, IDS_TB_VIDEO, guidAVILast);
ACTION_INFO_FULL(CActionAVICapture, IDS_ACTION_AVI_CAPTURE, IDS_MENU_AVI, IDS_TB_VIDEO, guidAVICapture);

//[ag]7. targets
ACTION_TARGET(CActionAVIFirst, szDocumentVideo);
ACTION_TARGET(CActionAVIPrev, szDocumentVideo);
ACTION_TARGET(CActionAVINext, szDocumentVideo);
ACTION_TARGET(CActionAVILast, szDocumentVideo);
ACTION_TARGET(CActionAVICapture, szDocumentVideo);

//[ag]8. arguments
ACTION_ARG_LIST(CActionAVIFirst)
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionAVIPrev)
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionAVINext)
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionAVILast)
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionAVICapture)
END_ACTION_ARG_LIST();


//[ag]9. implementation


IUnknown *CAVIActionBase::_GetActiveContext( bool bAddRef )
{
	IUnknown *punkView = 0;

	sptrIDocumentSite	sptrS( m_punkTarget );
	sptrS->GetActiveView( &punkView );

	if( !punkView )
		return 0;

	if( !bAddRef )
		punkView->Release();

	return punkView;
}

CActionAVIFirst::CActionAVIFirst()
{
}

CActionAVIFirst::~CActionAVIFirst()
{
}

bool CActionAVIFirst::IsAvaible()
{
	sptrIVideoView sptrVV(_GetActiveContext());
	BOOL b;
	sptrVV->IsFirstFrame(&b);
	return b?false:true;
}

bool CActionAVIFirst::Invoke()
{
	sptrIVideoView sptrVV(_GetActiveContext());
	sptrVV->FirstFrame();
	return true;
}

CActionAVIPrev::CActionAVIPrev()
{
}

CActionAVIPrev::~CActionAVIPrev()
{
}

bool CActionAVIPrev::IsAvaible()
{
	sptrIVideoView sptrVV(_GetActiveContext());
	BOOL b;
	sptrVV->IsFirstFrame(&b);
	return b?false:true;
}

bool CActionAVIPrev::Invoke()
{
	sptrIVideoView sptrVV(_GetActiveContext());
	sptrVV->PreviousFrame();
	return true;
}

CActionAVINext::CActionAVINext()
{
}

CActionAVINext::~CActionAVINext()
{
}

bool CActionAVINext::IsAvaible()
{
	sptrIVideoView sptrVV(_GetActiveContext());
	BOOL b;
	sptrVV->IsLastFrame(&b);
	return b?false:true;
}


bool CActionAVINext::Invoke()
{
	sptrIVideoView sptrVV(_GetActiveContext());
	sptrVV->NextFrame();
	return true;
}

CActionAVILast::CActionAVILast()
{
}

CActionAVILast::~CActionAVILast()
{
}

bool CActionAVILast::IsAvaible()
{
	sptrIVideoView sptrVV(_GetActiveContext());
	BOOL b;
	sptrVV->IsLastFrame(&b);
	return b?false:true;
}

bool CActionAVILast::Invoke()
{
	sptrIVideoView sptrVV(_GetActiveContext());
	sptrVV->LastFrame();
	return true;
}

CActionAVICapture::CActionAVICapture()
{
}

CActionAVICapture::~CActionAVICapture()
{
}


bool CActionAVICapture::Invoke()
{
	sptrIVideoView sptrVV(_GetActiveContext());
	BOOL bDocKeyInited = FALSE;
	GuidKey lDocKey;
	sptrVV->GetImageDocKey(&lDocKey,&bDocKeyInited);
	if (bDocKeyInited)
	{
		IUnknown *punkDoc = __FindDocByKey(lDocKey,_T("Image"));
		if (punkDoc)
		{
			sptrIDocumentSite sptrDS(punkDoc);
			IUnknown *punkView;
			sptrDS->GetActiveView(&punkView);
			if (punkView)
			{
				sptrIImageView sptrIV(punkView);
				punkView->Release();
				IUnknown *pimg = NULL;
				IUnknown *pimgSel = NULL;
				sptrIV->GetActiveImage(&pimg, &pimgSel);
				::DeleteObject(punkDoc, pimg);
				if (pimg)
					pimg->Release();
				if (pimgSel)
					pimgSel->Release();
			}
			IUnknown *punkObj = ::CreateTypedObject("Image");
			if (punkObj)
			{
				sptrVV->Capture(punkObj, TRUE);
				_variant_t	var(punkObj);
				::SetValue(punkDoc, 0, 0, var);
				punkObj->Release();
			}
			punkDoc->Release();
			return true;
		}
	}
	CArray<GUID, GUID&> uiaFoundDocs;
	__EnlistAllDocs(uiaFoundDocs,_T("Image"));
	IUnknown *punkAM = _GetOtherComponent( GetAppUnknown(), IID_IActionManager );
	sptrIActionManager	sptrAM(punkAM);
	punkAM->Release();
	DoExecute(sptrAM, _T("FileNew"), _T("\"Image\""));
	IUnknown *punkDoc = __FindNewDocs(uiaFoundDocs,lDocKey,_T("Image"));
	IUnknown *punkVw = NULL;
	if (punkDoc)
	{
		IUnknown *punkObj = ::CreateTypedObject("Image");
		if (punkObj)
		{
			sptrVV->Capture(punkObj, TRUE);
			sptrVV->SetImageDocKey(lDocKey);
			_variant_t	var(punkObj);
			::SetValue(punkDoc, 0, 0, var);
			punkObj->Release();
		}
		punkDoc->Release();
	}
	return true;
}




