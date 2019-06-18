#include "stdafx.h"
#include "actions.h"
#include "resource.h"
#include "AuditTrailView.h"
#include "AuditTrailSingleView.h"
#include "AuditTrailTextView.h"
#include "SetupATDlg.h"
//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionSetupAT, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowTextAT, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSetPrevAThumbnail, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSetNextAThumbnail, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowSingleAT, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSetActiveAThumbnail, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionShowAT, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionOnOffAT, CCmdTargetEx);

//olecreate

// {9E88DBCD-79B5-4885-85E4-8A6C0F68E45A}
GUARD_IMPLEMENT_OLECREATE(CActionSetupAT, "AuditTrail.SetupAT",
0x9e88dbcd, 0x79b5, 0x4885, 0x85, 0xe4, 0x8a, 0x6c, 0xf, 0x68, 0xe4, 0x5a);
// {C15ECB4F-6B98-44c9-97C7-B1BC854D6DBB}
GUARD_IMPLEMENT_OLECREATE(CActionShowTextAT, "AuditTrail.ShowTextAT",
0xc15ecb4f, 0x6b98, 0x44c9, 0x97, 0xc7, 0xb1, 0xbc, 0x85, 0x4d, 0x6d, 0xbb);
// {25D18A2D-DC16-40c7-A17F-A2CC319459A6}
GUARD_IMPLEMENT_OLECREATE(CActionSetPrevAThumbnail, "AuditTrail.SetPrevAThumbnail",
0x25d18a2d, 0xdc16, 0x40c7, 0xa1, 0x7f, 0xa2, 0xcc, 0x31, 0x94, 0x59, 0xa6);
// {6FD601D4-7471-4530-834F-4083F0F99384}
GUARD_IMPLEMENT_OLECREATE(CActionSetNextAThumbnail, "AuditTrail.SetNextAThumbnail",
0x6fd601d4, 0x7471, 0x4530, 0x83, 0x4f, 0x40, 0x83, 0xf0, 0xf9, 0x93, 0x84);
// {32BCE617-722F-4f5c-B1B4-751951B530B8}
GUARD_IMPLEMENT_OLECREATE(CActionShowSingleAT, "AuditTrail.ShowSingleAT",
0x32bce617, 0x722f, 0x4f5c, 0xb1, 0xb4, 0x75, 0x19, 0x51, 0xb5, 0x30, 0xb8);
// {CFF87961-D883-4a22-942C-E1E3D8A26E86}
GUARD_IMPLEMENT_OLECREATE(CActionSetActiveAThumbnail, "AuditTrail.SetActiveAThumbnail",
0xcff87961, 0xd883, 0x4a22, 0x94, 0x2c, 0xe1, 0xe3, 0xd8, 0xa2, 0x6e, 0x86);
// {8452BD9F-3ED0-4fe4-A224-54B8A400B69C}
GUARD_IMPLEMENT_OLECREATE(CActionShowAT, "AuditTrail.ShowAT",
0x8452bd9f, 0x3ed0, 0x4fe4, 0xa2, 0x24, 0x54, 0xb8, 0xa4, 0x0, 0xb6, 0x9c);
// {E0726183-1164-41cb-8804-4FB14E2732C5}
GUARD_IMPLEMENT_OLECREATE(CActionOnOffAT, "AuditTrail.OnOffAT",
0xe0726183, 0x1164, 0x41cb, 0x88, 0x4, 0x4f, 0xb1, 0x4e, 0x27, 0x32, 0xc5);

//guidinfo
// {21069067-653B-4b3b-9C0D-6D677B9BCF73}
static const GUID guidSetupAT = 
{ 0x21069067, 0x653b, 0x4b3b, { 0x9c, 0xd, 0x6d, 0x67, 0x7b, 0x9b, 0xcf, 0x73 } };
// {BB4E3E69-36E2-4b68-89AA-50591202A30E}
static const GUID guidShowTextAT =
{ 0xbb4e3e69, 0x36e2, 0x4b68, { 0x89, 0xaa, 0x50, 0x59, 0x12, 0x2, 0xa3, 0xe } };
// {E702E02D-9BCB-4a5b-B350-6103FEEF2B07}
static const GUID guidSetPrevAThumbnail =
{ 0xe702e02d, 0x9bcb, 0x4a5b, { 0xb3, 0x50, 0x61, 0x3, 0xfe, 0xef, 0x2b, 0x7 } };
// {E3414981-7C11-4b2a-BF7B-2DEB57338BAF}
static const GUID guidSetNextAThumbnail =
{ 0xe3414981, 0x7c11, 0x4b2a, { 0xbf, 0x7b, 0x2d, 0xeb, 0x57, 0x33, 0x8b, 0xaf } };
// {C37FA44E-4BC7-4945-B2B6-2CA9FF3E0125}
static const GUID guidShowSingleAT =
{ 0xc37fa44e, 0x4bc7, 0x4945, { 0xb2, 0xb6, 0x2c, 0xa9, 0xff, 0x3e, 0x1, 0x25 } };
// {AF941C96-56A9-42fd-BDCF-36AF2C04F9F4}
static const GUID guidSetActiveAThumbnail =
{ 0xaf941c96, 0x56a9, 0x42fd, { 0xbd, 0xcf, 0x36, 0xaf, 0x2c, 0x4, 0xf9, 0xf4 } };
// {54F0CDCA-00FE-4a9c-8898-698536502D19}
static const GUID guidShowAT =
{ 0x54f0cdca, 0xfe, 0x4a9c, { 0x88, 0x98, 0x69, 0x85, 0x36, 0x50, 0x2d, 0x19 } };
// {5F15DBFD-20F6-4cf5-A065-CB389E7CCB6F}
static const GUID guidOnOffAT =
{ 0x5f15dbfd, 0x20f6, 0x4cf5, { 0xa0, 0x65, 0xcb, 0x38, 0x9e, 0x7c, 0xcb, 0x6f } };

//[ag]6. action info

ACTION_INFO_FULL(CActionSetupAT, IDS_ACTION_SETUP_AT, -1, -1, guidSetupAT);
ACTION_INFO_FULL(CActionShowTextAT, IDS_ACTION_SHOW_TEXT_AT, -1, -1, guidShowTextAT);
ACTION_INFO_FULL(CActionSetPrevAThumbnail, IDS_ACTION_SET_PREV, -1, -1, guidSetPrevAThumbnail);
ACTION_INFO_FULL(CActionSetNextAThumbnail, IDS_ACTION_SET_NEXT, -1, -1, guidSetNextAThumbnail);
ACTION_INFO_FULL(CActionShowSingleAT, IDS_ACTION_SHOW_SINGLE_AT, -1, -1, guidShowSingleAT);
ACTION_INFO_FULL(CActionSetActiveAThumbnail, IDS_ACTION_SET_ACTIVE, -1, -1, guidSetActiveAThumbnail);
ACTION_INFO_FULL(CActionShowAT, IDS_ACTION_SHOW_AT, -1, -1, guidShowAT);
ACTION_INFO_FULL(CActionOnOffAT, IDS_ACTION_ON_OFF_AT, -1, -1, guidOnOffAT);
//[ag]7. targets

ACTION_TARGET(CActionSetupAT, szTargetMainFrame);
ACTION_TARGET(CActionShowTextAT, szTargetFrame);
ACTION_TARGET(CActionSetPrevAThumbnail, szTargetViewSite);
ACTION_TARGET(CActionSetNextAThumbnail, szTargetViewSite);
ACTION_TARGET(CActionShowSingleAT, szTargetFrame);
ACTION_TARGET(CActionSetActiveAThumbnail, szTargetViewSite);
ACTION_TARGET(CActionShowAT, szTargetFrame);
ACTION_TARGET(CActionOnOffAT, szTargetMainFrame);


ACTION_ARG_LIST(CActionSetActiveAThumbnail)
	ARG_INT("Index", 0)
END_ACTION_ARG_LIST()


//[ag]9. implementation



//////////////////////////////////////////////////////////////////////
//CActionSetupAT implementation
CActionSetupAT::CActionSetupAT()
{
}

bool CActionSetupAT::Invoke()
{
	CSetupATDlg dlg;
	dlg.DoModal();
	return true;
}



//////////////////////////////////////////////////////////////////////
//CActionShowTextAT implementation
CString CActionShowTextAT::GetViewProgID()
{
	return szAuditTrailTextViewProgID;
}

//////////////////////////////////////////////////////////////////////
//CActionSetPrevAThumbnail implementation
CActionSetPrevAThumbnail::CActionSetPrevAThumbnail()
{
}

CActionSetPrevAThumbnail::~CActionSetPrevAThumbnail()
{
}

bool CActionSetPrevAThumbnail::Invoke()
{
	IAuditTrailObjectPtr	sptrAT(GetAT());
	
	if(sptrAT != 0)
	{
		long nIndex = 0;
		sptrAT->GetActiveThumbnail(&nIndex);
		sptrAT->SetActiveThumbnail(nIndex-1);
	}
	return true;
}

bool CActionSetPrevAThumbnail::IsAvaible()
{
	IAuditTrailObjectPtr	sptrAT(GetAT());
	
	if(sptrAT == 0)
		return 0;

	long nIndex = 0;
	sptrAT->GetActiveThumbnail(&nIndex);

	if( nIndex <= 0 )
		return 0;

	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionSetNextAThumbnail implementation
CActionSetNextAThumbnail::CActionSetNextAThumbnail()
{
}

CActionSetNextAThumbnail::~CActionSetNextAThumbnail()
{
}

bool CActionSetNextAThumbnail::Invoke()
{
	IAuditTrailObjectPtr	sptrAT(GetAT());
	
	if(sptrAT != 0)
	{
		long nIndex = 0;
		sptrAT->GetActiveThumbnail(&nIndex);
		sptrAT->SetActiveThumbnail(nIndex+1);
	}
	return true;
}

bool CActionSetNextAThumbnail::IsAvaible()
{
	IAuditTrailObjectPtr	sptrAT(GetAT());
	
	if(sptrAT == 0)
		return 0;

	long nIndex = 0, nAll = 0;
	sptrAT->GetActiveThumbnail(&nIndex);
	sptrAT->GetThumbnailsCount( &nAll );

	if( nAll <= 0 )
		return 0;

	if( nIndex >= nAll - 1 )
			return 0;

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionShowSingleAT implementation
CString CActionShowSingleAT::GetViewProgID()
{
	return szAuditTrailSingleViewProgID;
}


//////////////////////////////////////////////////////////////////////
//CActionSetActiveAThumbnail implementation
CActionSetActiveAThumbnail::CActionSetActiveAThumbnail()
{
}

CActionSetActiveAThumbnail::~CActionSetActiveAThumbnail()
{
}

IAuditTrailObject* CActionSetActiveAThumbnail::GetAT()
{
	IImageViewPtr sptrIV(m_punkTarget);
	IAuditTrailObjectPtr	sptrAT;
	if(sptrIV != 0)
	{
		IUnknown* punkATM = _GetOtherComponent(GetAppUnknown(), IID_IAuditTrailMan);
		IAuditTrailManPtr sptrATman(punkATM);
		if(punkATM)
			punkATM->Release();
		if(sptrATman != 0)
		{
			IUnknown* punkImage = 0;
			sptrIV->GetActiveImage(&punkImage, 0);
			if(punkImage)
			{
				IUnknown* punkAT = 0;
				sptrATman->GetAuditTrail(punkImage, &punkAT);
				sptrAT = punkAT;
				if(punkAT)
					punkAT->Release();
				punkImage->Release();
			}
		}

	}
	else
	{
		//maybe it's AT view
		IViewPtr	sptrV(m_punkTarget);
		if(sptrV != 0)
		{
			long pos = 0; 
			sptrV->GetFirstVisibleObjectPosition(&pos);
			while(pos)
			{
				IUnknown* punkObject = 0;
				sptrV->GetNextVisibleObject(&punkObject, &pos);
				sptrAT = punkObject;
				if(punkObject)
					punkObject->Release();
				if(sptrAT != 0)
					break;
			}
		}
	}
	return sptrAT;
}

bool CActionSetActiveAThumbnail::Invoke()
{
	long nIndex = GetArgumentInt( _T("Index") );

	if(nIndex < 0)
		return false;

	IAuditTrailObjectPtr	sptrAT(GetAT());
	
	if(sptrAT != 0)
	{
		sptrAT->SetActiveThumbnail(nIndex);
	}
		
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionShowAT implementation
CString CActionShowAT::GetViewProgID()
{
	return szAuditTrailViewProgID;
}

//////////////////////////////////////////////////////////////////////
//CActionOnOffAT implementation
CActionOnOffAT::CActionOnOffAT()
{
}

CActionOnOffAT::~CActionOnOffAT()
{
}

bool CActionOnOffAT::Invoke()
{
	IUnknown* punkATM = _GetOtherComponent(GetAppUnknown(), IID_IAuditTrailMan);
	IAuditTrailManPtr sptrATM(punkATM);
	if(punkATM)
		punkATM->Release();
	if(sptrATM != 0)
	{
		sptrATM->ToggleOnOffFlag();
	}

	return true;
}

bool CActionOnOffAT::IsAvaible()
{
	IUnknown* punkATM = _GetOtherComponent(GetAppUnknown(), IID_IAuditTrailMan);
	if(punkATM)
	{
		punkATM->Release();
		return true;
	}

	return false;
}

bool CActionOnOffAT::IsChecked()
{
	IUnknown* punkATM = _GetOtherComponent(GetAppUnknown(), IID_IAuditTrailMan);
	IAuditTrailManPtr sptrATM(punkATM);
	if(punkATM)
		punkATM->Release();

	if(sptrATM != 0)
	{
		BOOL bWillCreate = FALSE;
		sptrATM->GetCreateFlag(&bWillCreate);
		return bWillCreate == TRUE;
	}

	return false;
}
