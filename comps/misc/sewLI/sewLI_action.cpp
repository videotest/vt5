#include "stdafx.h"
#include "sewLI_action.h"
#include "nameconsts.h"
#include "misc_utils.h"
#include "misc_templ.h"
#include "data5.h"
#include "docview5.h"
#include "sewLI.h"
#include "sewLI_page.h"
#include "clone.h"
#include "FindPos.h"
#include "SewUtils.h"
#include "DistMap.h"
#include <atltypes.h>
#include <atlstr.h>
#include <math.h>


_ainfo_base::arg	CSewLIObjectActionInfo::s_pargs[] = 
{
	{"SewLIParam",szArgumentTypeSewLI, 0, false, true },
	{0, 0, 0, false, false },
};

/////////////////////////////////////////////////////////////////////////////////////////
//CSewManual
CSewLI::CSewLI()
{
	
}

CSewLI::~CSewLI()
{
	DeInit();
}

IUnknown *CSewLI::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else if( iid == IID_IInteractiveSewLIAction )return (IInteractiveSewLIAction*)this;
	else return CInteractiveAction::DoGetInterface( iid );
}

void CSewLI::DeInit()
{
	
}

bool CSewLI::Initialize()
{
	if( !CInteractiveAction::Initialize() )
		return false;


	//add property page
	int	nPage = -1;
	IPropertySheet	*pSheet = ::FindPropertySheet();
	IOptionsPage	*pPage = ::FindPage( pSheet, clsidSewLIPage, &nPage );
	m_ptrPage = pPage;
	if( nPage != -1 )pSheet->IncludePage( nPage );
	if( pPage )pPage->Release();
	if( pSheet )pSheet->Release();
	
	DeInit();
	
	return true;
}

HRESULT CSewLI::TerminateInteractive()
{
	return CInteractiveAction::TerminateInteractive();
}


bool CSewLI::Finalize()
{

	if( !CInteractiveAction::Finalize() )
		return false;

	//repove property page

	int	nPage = -1;
	IPropertySheet	*pSheet = ::FindPropertySheet();
	IOptionsPage	*pPage = ::FindPage( pSheet, clsidSewLIPage, &nPage );
	if( nPage != -1 )pSheet->ExcludePage( nPage );
	if( pPage )pPage->Release();
	if( pSheet )pSheet->Release();
	m_ptrPage = 0;

	InvalidateRect( m_hwnd, 0, 0 );

	return true;
}


HRESULT CSewLI::DoInvoke()
{
	return S_OK;
}


HRESULT CSewLI::GetActionState( DWORD *pdwState )
{
	IDataContext2Ptr	ptrContext( m_ptrTarget );
	if (ptrContext == 0)
		*pdwState = 0;
	else
	{
		_bstr_t	bstrImage = szTypeImage;
		long	count;
		ptrContext->GetSelectedCount( bstrImage, &count );
		*pdwState = count > 1;
	}
	return S_OK;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//CSewLIObjectAction
////////////////////////////////////////////////////////////////////////////////////////////////////
CSewLIObjectAction::CSewLIObjectAction()
{
}
//////////////////////////////////////////////////////////////////////
CSewLIObjectAction::~CSewLIObjectAction()
{
}

//////////////////////////////////////////////////////////////////////
IUnknown* CSewLIObjectAction::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	return CAction::DoGetInterface( iid );
}

HRESULT CSewLIObjectAction::DoInvoke()
{
	IUnknown* punk_sewLI = 0;
	
	INamedDataObject2Ptr ptr_new_sewLI = 0;
	{
		IUnknown* punk_new_sewLI = 0;		
		punk_new_sewLI = CreateTypedObject( _bstr_t(szTypeSewLI) );
		if( !punk_new_sewLI )	return S_FALSE;

		ptr_new_sewLI = punk_new_sewLI;
		punk_new_sewLI->Release();
	}
	if( ptr_new_sewLI == 0 )	return S_FALSE;

	//add new table to document
	SetToDocData( m_ptrTarget, ptr_new_sewLI );	

	//if( GetArgLong( "ActivateTable" ) == 1 )
	{
		//ExecuteAction( "ShowSewLI", 0, 0 );
		//SetActiveObject( m_ptrTarget, ptr_new_sewLI, szTypeSewLI );
	}

	return S_OK;
}

_bstr_t CShowSewLI::GetViewProgID()
{
	return _bstr_t(pszSewLIViewProgID);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionSewImport
////////////////////////////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CActionSewImportInfo::s_pargs[] = 
{
	{"SewLI",szArgumentTypeSewLI, 0, false, true },
	{"Source",szArgumentTypeImage, 0, false, true },
	{"x",szArgumentTypeInt, "0", false, false },
	{"y",szArgumentTypeInt, "0", false, false },
	{0, 0, 0, false, false },
};

CActionSewImport::CActionSewImport(void)
{
}

CActionSewImport::~CActionSewImport(void)
{
}

IUnknown *CActionSewImport::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return __super::DoGetInterface( iid );
}

HRESULT CActionSewImport::DoInvoke()
{
	_bstr_t bstrSLI = GetArgString("SewLI");
	_bstr_t bstrImg = GetArgString("Source");
	int x = GetArgLong("x");
	int y = GetArgLong("y");
	IUnknownPtr punkSLI,punkImg;
	if (bstrSLI.length() > 0)
		punkSLI = IUnknownPtr(::GetObjectByName(m_ptrTarget,bstrSLI,_bstr_t(szArgumentTypeSewLI)), false);
	else
		punkSLI = IUnknownPtr(::GetActiveObjectFromDocument(m_ptrTarget,szArgumentTypeSewLI), false);
	ISewImageListPtr sptrSLI(punkSLI);
	if (sptrSLI == 0) return S_FALSE;
	if (bstrImg.length() > 0)
		punkImg = IUnknownPtr(::GetObjectByName(m_ptrTarget,bstrImg,_bstr_t(szArgumentTypeImage)), false);
	else
	{
		punkImg = IUnknownPtr(::GetActiveObjectFromDocument(m_ptrTarget,szArgumentTypeImage), false);
		if (punkImg == 0)
		{
			IDataContext2Ptr sptrDC(m_ptrTarget);
			if (sptrDC != 0)
			{
				long lpos;
				sptrDC->GetFirstObjectPos(_bstr_t(szArgumentTypeImage), &lpos);
				if (lpos != 0)
					sptrDC->GetNextObject(_bstr_t(szArgumentTypeImage), &lpos, &punkImg);
			}
		}
	}
	if (punkImg == 0) return S_FALSE;
	IUnknownPtr punkImgNew(::CreateTypedObject(_bstr_t(szArgumentTypeImage)), false);
	ICompatibleObjectPtr sptrComp(punkImgNew);
	sptrComp->CreateCompatibleObject(punkImg, NULL, 0);
	sptrSLI->AddImage(punkImgNew, _point(x,y), x==0&&y==0?addImageFindPos:0);
	m_sptrSLI = sptrSLI;
	return S_OK;
}

HRESULT CActionSewImport::DoUndo()
{
	long lPosActive,lPosLast;
	m_sptrSLI->GetActiveFragmentPosition(&lPosActive);
	m_sptrSLI->GetLastFragmentPosition(&lPosLast);
	if (lPosActive != 0 && lPosActive == lPosLast)
	{
		ISewFragmentPtr sptrFrag;
		long l = lPosActive;
		m_sptrSLI->GetNextFragment(&l, &sptrFrag);
		sptrFrag->GetOffset(&m_ptPos);
		sptrFrag->GetImage(NULL, &m_punkImage);
		m_sptrSLI->DeleteFragment(lPosActive);
	}
	return S_OK;
}

HRESULT CActionSewImport::DoRedo()
{
	m_sptrSLI->AddImage(m_punkImage, m_ptPos, 0);
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionSewExport
////////////////////////////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CActionSewExportInfo::s_pargs[] = 
{
	{"SewLI",szArgumentTypeSewLI, 0, true, true },
	{"ImageName",szArgumentTypeString, 0, true, true },
	{"FragNum",szArgumentTypeInt, "-1", true, false },
	{0, 0, 0, false, false },
};

CActionSewExport::CActionSewExport(void)
{
}

CActionSewExport::~CActionSewExport(void)
{
}

void CActionSewExport::SendToDoc(ISewFragment *ptrFrag, BSTR bstrImg)
{
	IUnknownPtr punkImg;
	ptrFrag->LoadImage(1);
	int nZoom = 1;
	ptrFrag->GetImage(&nZoom, &punkImg);
	if (punkImg != 0)
	{
		IUnknownPtr punkImgNew(::CreateTypedObject(_bstr_t(szArgumentTypeImage)), false);
		ICompatibleObjectPtr sptrComp(punkImgNew);
		sptrComp->CreateCompatibleObject(punkImg, NULL, 0);
		INamedObject2Ptr sptrNOImg(punkImgNew);
		if (sptrNOImg != 0 && bstrImg != NULL)
			sptrNOImg->SetName(bstrImg);
		SetToDocData(m_ptrTarget, punkImgNew);
	}
	ptrFrag->FreeImage(1);
}

IUnknown *CActionSewExport::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return __super::DoGetInterface( iid );
}

HRESULT CActionSewExport::DoInvoke()
{
	_bstr_t bstrSLI = GetArgString("SewLI");
	_bstr_t bstrImg = GetArgString("ImageName");
	int nFragNum = GetArgLong("FragNum");
	IUnknownPtr punkSLI,punkImg;
	if (bstrSLI.length() > 0)
		punkSLI = IUnknownPtr(::GetObjectByName(m_ptrTarget,bstrSLI,_bstr_t(szArgumentTypeSewLI)), false);
	else
		punkSLI = IUnknownPtr(::GetActiveObjectFromDocument(m_ptrTarget,szArgumentTypeSewLI), false);
	ISewImageListPtr sptrSLI(punkSLI);
	if (sptrSLI == 0) return S_FALSE;
	if (bstrImg.length() > 0)
		punkImg = IUnknownPtr(::GetObjectByName(m_ptrTarget,bstrImg,_bstr_t(szArgumentTypeImage)), false);
	if (punkImg != 0)
		RemoveFromDocData(m_ptrTarget, punkImg);
	if (nFragNum < 0)
	{
		long lPos;
		sptrSLI->GetActiveFragmentPosition(&lPos);
		if (lPos != 0)
		{
			ISewFragmentPtr sptrFrag;
			sptrSLI->GetNextFragment(&lPos, &sptrFrag);
			SendToDoc(sptrFrag, bstrImg);
		}
	}
	else
	{
		int iNum = 0;
		long lPos = 0;
		sptrSLI->GetFirstFragmentPos(&lPos);
		while (lPos)
		{
			ISewFragmentPtr sptrFrag;
			sptrSLI->GetNextFragment(&lPos, &sptrFrag);
			if (iNum == nFragNum)
			{
				SendToDoc(sptrFrag, bstrImg);
				break;
			}
			iNum++;
		}
	}

	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionSewMoveFragment
////////////////////////////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CActionSewMoveFragmentInfo::s_pargs[] = 
{
	{0, 0, 0, false, false },
};

CActionSewMoveFragment::CActionSewMoveFragment(void)
{
	m_ptFragOrig = m_ptFragPos1 = m_ptCursPos1 = CPoint(0,0);
	m_bEnableMove = false;
	m_bMoved = false;
	m_bMoveLV = false;
}

CActionSewMoveFragment::~CActionSewMoveFragment(void)
{
}

IUnknown *CActionSewMoveFragment::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return __super::DoGetInterface( iid );
}

void CActionSewMoveFragment::MoveFragment(POINT point, bool bFire)
{
	int dx = point.x-m_ptCursPos1.x;
	int dy = point.y-m_ptCursPos1.y;
	int x = m_ptFragPos1.x+(int)dx;
	int y = m_ptFragPos1.y+(int)dy;
	if (m_bMoveLV)
	{
		m_sptrView->MoveLiveVideo(x, y, bFire?moveFragCalcOvr|moveFragFire|moveFragCorrect:0);
	}
	else
	{
		if (m_sptrFrag != 0)
			m_sptrSLI->MoveFragment(m_sptrFrag, CPoint(x,y),
				bFire?moveFragCalcOvr|moveFragFire|moveFragCorrect:0);
	}
	m_bMoved = true;
}

bool CActionSewMoveFragment::Initialize()
{
	if( !CInteractiveAction::Initialize() )
		return false;
	IUnknownPtr punkSLI(::GetActiveObjectFromContext(m_ptrTarget, szTypeSewLI), false);
	m_sptrSLI = punkSLI;
	if (m_sptrSLI == 0)
		return false;
	m_sptrView = m_ptrTarget;
	if (m_sptrView == 0)
		return false;
	return true;
}

bool CActionSewMoveFragment::Finalize()
{
	m_sptrView = 0;
	if( !CInteractiveAction::Finalize() )
		return false;
	return true;
}

bool CActionSewMoveFragment::GetMoveableFragmentRect(RECT &rcMoveFrag)
{
	long lposLast = 0, lposActive = 0;
	m_sptrSLI->GetLastFragmentPosition(&lposLast);
	m_sptrSLI->GetActiveFragmentPosition(&lposActive);
	if (lposActive == lposLast && lposLast != NULL)
	{
//		CPoint ptOrig;
//		m_sptrSLI->GetOrigin(&ptOrig);
		ISewFragmentPtr sptrFrag;
		m_sptrSLI->GetNextFragment(&lposLast, &sptrFrag);
		POINT ptLastFrag;
		sptrFrag->GetOffset(&ptLastFrag);
		SIZE szLastFrag;
		sptrFrag->GetSize(&szLastFrag);
		rcMoveFrag = CRect(ptLastFrag,szLastFrag);
		return true;
	}
	rcMoveFrag = CRect(0,0,0,0);
	return false;
}


bool CActionSewMoveFragment::DoLButtonDown( _point point )
{
	m_bEnableMove = false;
	m_bMoved = false;
	m_bMoveLV = false;
	BOOL bShowLV = FALSE;
	m_sptrView->get_ShowLiveVideo(&bShowLV);
	if (bShowLV)
	{
		CRect rcLV;
		m_sptrView->GetLiveVideoRect((int *)&rcLV.left, (int *)&rcLV.top, (int *)&rcLV.right, (int *)&rcLV.bottom);
		CPoint ptOrg;
		m_sptrView->GetDocumentOrg((int *)&ptOrg.x, (int *)&ptOrg.y);
		rcLV.OffsetRect(-ptOrg.x, -ptOrg.y);
		if (rcLV.PtInRect(point))
		{
			m_bEnableMove = true;
			m_bMoveLV = true;
		}
		return __super::DoLButtonDown(point);
	}
	else
	{
		long lpos = 0;
		m_sptrSLI->GetLastFragmentPosition(&lpos);
		if (lpos != 0)
			m_sptrSLI->GetNextFragment(&lpos, &m_sptrFrag);
		if (m_sptrFrag == 0)
//		{
//			m_sptrSLI = 0;
			return false;
//		}
		m_sptrFrag->GetOffset(&m_ptFragOrig);
		CPoint ptOrig;
		m_sptrSLI->GetOrigin(&ptOrig);
		if (GetMoveableFragmentRect(m_rcMoveFrag) && m_rcMoveFrag.PtInRect(point+ptOrig))
			m_bEnableMove = true;
		return __super::DoLButtonDown(point);
	}
}

bool CActionSewMoveFragment::DoStartTracking( _point point )
{
	if (!m_bLeftButton)
		return false;
	if (m_bMoveLV)
	{
		CRect rcLV;
		m_sptrView->GetLiveVideoRect((int *)&rcLV.left, (int *)&rcLV.top,
			(int *)&rcLV.right, (int *)&rcLV.bottom);
		m_ptFragPos1 = rcLV.TopLeft();
		m_sptrView->put_LiveVideoMoving(TRUE);
	}
	else if (m_sptrFrag != 0)
		m_sptrFrag->GetOffset(&m_ptFragPos1);
	m_ptCursPos1 = point;
	return true;
}

bool CActionSewMoveFragment::DoFinishTracking( _point point )
{
	if (m_bEnableMove)
	{
		m_sptrView->put_LiveVideoMoving(FALSE);
		MoveFragment(point, true);
		Finalize();
	}
	return true;
}

bool CActionSewMoveFragment::DoTrack( _point  point )
{
	if (m_bEnableMove)
		MoveFragment(point, false);
	return true;
}

bool CActionSewMoveFragment::DoLButtonUp( _point point )
{
	bool b = __super::DoLButtonUp(point);
	if (!m_bMoved && !m_bMoveLV)
	{
		CPoint ptOrig;
		m_sptrSLI->GetOrigin(&ptOrig);
		long lPosActive = 0;
		m_sptrSLI->GetActiveFragmentPosition(&lPosActive);
		long lPosFound = 0;
		long lPos = 0;
		m_sptrSLI->GetFirstFragmentPos(&lPos);
		while (lPos)
		{
			long lPosPrev = lPos;
			ISewFragmentPtr sptrFrag;
			m_sptrSLI->GetNextFragment(&lPos, &sptrFrag);
			if (lPosPrev == lPosActive )
			{
				if (lPosFound == 0)
					continue;
				else
					break;
			}
			POINT ptFrag;
			sptrFrag->GetOffset(&ptFrag);
			SIZE szFrag;
			sptrFrag->GetSize(&szFrag);
			CRect rcFrag(ptFrag,szFrag);
			if (rcFrag.PtInRect(point+ptOrig))
				lPosFound = lPosPrev;
		}
		if (lPosFound != 0)
			m_sptrSLI->SetActiveFragmentPosition(lPosFound);
	}
	return b;
}

bool CActionSewMoveFragment::DoRButtonUp( _point point )
{
	bool b = __super::DoRButtonUp(point);
	BOOL bShowLV = FALSE;
	m_sptrView->get_ShowLiveVideo(&bShowLV);
	if (bShowLV)
	{
		CRect rcLV;
		m_sptrView->GetLiveVideoRect((int *)&rcLV.left, (int *)&rcLV.top, (int *)&rcLV.right, (int *)&rcLV.bottom);
		CPoint ptOrg;
		m_sptrView->GetDocumentOrg((int *)&ptOrg.x, (int *)&ptOrg.y);
		point.x += ptOrg.x;
		point.y += ptOrg.y;
		m_sptrView->MoveLiveVideoTo(point.x, point.y, 0);
	}
	else
	{
		// 1. Calculate point in document coordinates.
		long lposLast = 0;
		m_sptrSLI->GetLastFragmentPosition(&lposLast);
		ISewFragmentPtr sptrLastFrag;
		if (lposLast != 0)
			m_sptrSLI->GetPrevFragment(&lposLast, &sptrLastFrag);
		if (sptrLastFrag == 0)
			return b;
		CPoint ptFragOrig;
		sptrLastFrag->GetOffset(&ptFragOrig);
		CSize szLastFrag;
		sptrLastFrag->GetSize(&szLastFrag);
		CPoint ptOrig;
		m_sptrSLI->GetOrigin(&ptOrig);
		point.x += ptOrig.x;
		point.y += ptOrig.y;
//		point.x -= szLastFrag.cx/2;
//		point.y -= szLastFrag.cy/2;
		// 2. Find base fragment.
		ISewFragmentPtr sptrFragFound;
		long lPos = lposLast;
		while (lPos)
		{
			ISewFragmentPtr sptrFrag;
			m_sptrSLI->GetPrevFragment(&lPos, &sptrFrag);
			POINT ptFrag;
			sptrFrag->GetOffset(&ptFrag);
			SIZE szFrag;
			sptrFrag->GetSize(&szFrag);
			if (point.x >= ptFrag.x && point.x < ptFrag.x+szFrag.cx &&
				point.y >= ptFrag.y && point.y < ptFrag.y+szFrag.cy)
			{
				sptrFragFound = sptrFrag;
				break;
			}

		}
		// 3. Correct position using found fragment.
		if (sptrFragFound != 0)
		{
			CPoint ptPos(point);
			if (FindOptimalPositionByPoint(m_sptrSLI, sptrFragFound, sptrLastFrag,
				&ptPos))
			{
				sptrLastFrag->GetOffset(&m_ptFragOrig);
				m_sptrFrag = sptrLastFrag;
				m_sptrSLI->MoveFragment(sptrLastFrag, ptPos, moveFragCalcOvr|moveFragFire);
				Finalize();
			}
		}
	}
	return b;
}


LRESULT	CActionSewMoveFragment::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	if (nMsg == 0x202)
	{
		OutputDebugString("WM_LBUTTONUP\n");
	}
	if (nMsg == WM_SETCURSOR && m_sptrView != 0 && m_sptrSLI != 0)
	{
		CPoint ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient((HWND)wParam, &ptCursor);
		ptCursor = _client(ptCursor, IScrollZoomSitePtr(m_ptrTarget));
		CPoint ptOrig;
		BOOL bShowLV = FALSE;
		m_sptrView->get_ShowLiveVideo(&bShowLV);
		if (bShowLV)
		{
			CRect rcLV;
			m_sptrView->GetLiveVideoRect((int *)&rcLV.left, (int *)&rcLV.top,
				(int *)&rcLV.right, (int *)&rcLV.bottom);
			m_sptrView->GetDocumentOrg((int *)&ptOrig.x, (int *)&ptOrig.y);
			rcLV.OffsetRect(-ptOrig.x, -ptOrig.y);
			if (rcLV.PtInRect(ptCursor))
				::SetCursor(::LoadCursor(NULL, IDC_HAND));
			else
				::SetCursor(::LoadCursor(NULL, IDC_ARROW));
		}
		else
		{
			m_sptrSLI->GetOrigin(&ptOrig);
			CRect rcMoveFrag;
			if (GetMoveableFragmentRect(rcMoveFrag) && ::PtInRect(&rcMoveFrag, ptCursor+ptOrig))
				::SetCursor(::LoadCursor(NULL, IDC_HAND));
			else
				::SetCursor(::LoadCursor(NULL, IDC_ARROW));
		}
		return 1L;
	}
	return CInteractiveAction::DoMessage(nMsg,wParam,lParam);
}

bool CActionSewMoveFragment::DoSetCursor()
{
	CPoint ptCursor;
	GetCursorPos(&ptCursor);
	return true;
}

HRESULT CActionSewMoveFragment::DoInvoke()
{
	if (m_bMoveLV) return S_OK; // Do not add to undo in this case
	IViewPtr ptrView(m_ptrTarget);
	if (ptrView == 0) return S_OK;
	IUnknownPtr punk;
	ptrView->GetDocument(&punk);
	if (punk != 0)
		OnAttachTarget(punk);
	return S_OK;
}

HRESULT CActionSewMoveFragment::DoUndo()
{
	POINT ptNow;
	if (m_sptrFrag != 0)
	{
		m_sptrFrag->GetOffset(&ptNow);
		m_sptrSLI->MoveFragment(m_sptrFrag, m_ptFragOrig, moveFragCalcOvr);
		m_ptFragOrig = ptNow;
	}
	return S_OK;
}

HRESULT CActionSewMoveFragment::DoRedo()
{
	return DoUndo();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionSewDeleteFragment
////////////////////////////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CActionSewDeleteFragmentInfo::s_pargs[] = 
{
	{"SewLI",szArgumentTypeSewLI, 0, true, true },
	{0, 0, 0, false, false },
};

CActionSewDeleteFragment::CActionSewDeleteFragment(void)
{
}

CActionSewDeleteFragment::~CActionSewDeleteFragment(void)
{
}

IUnknown *CActionSewDeleteFragment::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return __super::DoGetInterface( iid );
}

HRESULT CActionSewDeleteFragment::DoInvoke()
{
	_bstr_t bstrSLI = GetArgString("SewLI");
	IUnknownPtr punkSLI;
	if (bstrSLI.length() > 0)
		punkSLI = IUnknownPtr(::GetObjectByName(m_ptrTarget,bstrSLI,_bstr_t(szArgumentTypeSewLI)), false);
	else
		punkSLI = IUnknownPtr(::GetActiveObjectFromDocument(m_ptrTarget,szArgumentTypeSewLI), false);
	ISewImageListPtr sptrSLI(punkSLI);
	if (sptrSLI == 0) return S_FALSE;
	long lPosActive,lPosLast;
	sptrSLI->GetActiveFragmentPosition(&lPosActive);
	sptrSLI->GetLastFragmentPosition(&lPosLast);
	if (lPosActive != 0 && lPosActive == lPosLast)
	{
		ISewFragmentPtr sptrFrag;
		long l = lPosActive;
		sptrSLI->GetNextFragment(&l, &sptrFrag);
		sptrFrag->GetOffset(&m_ptPos);
		sptrFrag->GetImage(NULL, &m_punkImage);
		sptrSLI->DeleteFragment(lPosActive);
		m_sptrSLI = sptrSLI;
	}
	return S_OK;
}

HRESULT CActionSewDeleteFragment::DoUndo()
{
	m_sptrSLI->AddImage(m_punkImage, m_ptPos, 0);
	return S_OK;
}

HRESULT CActionSewDeleteFragment::DoRedo()
{
	long lPosActive,lPosLast;
	m_sptrSLI->GetActiveFragmentPosition(&lPosActive);
	m_sptrSLI->GetLastFragmentPosition(&lPosLast);
	if (lPosActive != 0 && lPosActive == lPosLast)
	{
		ISewFragmentPtr sptrFrag;
		long l = lPosActive;
		m_sptrSLI->GetNextFragment(&l, &sptrFrag);
		sptrFrag->GetOffset(&m_ptPos);
		sptrFrag->GetImage(NULL, &m_punkImage);
		m_sptrSLI->DeleteFragment(lPosActive);
	}
	return S_OK;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionSewSaveImage
////////////////////////////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CActionSewSaveImageInfo::s_pargs[] = 
{
	{"CX",szArgumentTypeInt, 0, true, false },
	{"SewLI",szArgumentTypeSewLI, 0, true, true },
	{"Image",szArgumentTypeImage, 0, false, true },
	{0, 0, 0, false, false },
};

CActionSewSaveImage::CActionSewSaveImage(void)
{
}

CActionSewSaveImage::~CActionSewSaveImage(void)
{
}

/*IUnknown *CActionSewSaveImage::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return __super::DoGetInterface( iid );
}*/

class CSaveSewImgDlg : public CDialogImpl
{
	int CalculateSizeKb(CSize sz, int nPanes)
	{
		int nSize = sz.cx*sz.cy*nPanes*sizeof(color);
		return nSize/1024;
	}
	SIZE *m_pSizes;
	int m_nSizes;
	int m_nPanes;
public:
	dummy_unknown();
	UINT idd() {return IDD_SEWSAVEIMAGE_ZOOM;};

	int m_nFmt;

	CSaveSewImgDlg(SIZE *pSizes, int nSizes, int nPanes)
	{
		m_pSizes = pSizes;
		m_nSizes = nSizes;
		m_nPanes = nPanes;
		m_nFmt = 0;
	}

	virtual bool DoInitDialog()
	{
		HWND hwndCB = ::GetDlgItem(m_hwnd, IDC_COMBO_FORMAT);
		for (int i = 0; i < m_nSizes; i++)
		{
			SIZE sz = m_pSizes[i];
			CString sFmt;
			sFmt.Format(_T("%dx%d"), sz.cx, sz.cy);
			SendMessage(hwndCB, CB_ADDSTRING,0,(LPARAM)(LPCTSTR)sFmt);
		}
		SendMessage(hwndCB, CB_SETCURSEL,0,0);
		int nSizeKb = CalculateSizeKb(m_pSizes[0], m_nPanes);
		char szBuff[256];
		sprintf(szBuff, "%d kb", nSizeKb);
		::SetDlgItemText(m_hwnd, IDC_STATIC_SIZE, szBuff);
		return true;
	}
	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
	{
		if (nMsg == WM_COMMAND)
		{
			int id = LOWORD(wParam);
			int nNotify = HIWORD(wParam);
			if (id == IDC_COMBO_FORMAT)
			{
				HWND hwndCB = ::GetDlgItem(m_hwnd, IDC_COMBO_FORMAT);
				if (nNotify == CBN_SELCHANGE)
				{
					m_nFmt = ::SendMessage(hwndCB, CB_GETCURSEL,0,0);
					int nSizeKb = CalculateSizeKb(m_pSizes[m_nFmt], m_nPanes);
					char szBuff[256];
					sprintf(szBuff, "%d kb", nSizeKb);
					::SetDlgItemText(m_hwnd, IDC_STATIC_SIZE, szBuff);
					return 0;
				}
			}
		}
		return __super::DoMessage(nMsg,wParam,lParam);
	}
};

bool CActionSewSaveImage::InvokeFilter()
{
	ISewImageListPtr sptrSLI(GetDataArgument());
	IImagePtr img(GetDataResult());
	int cx = GetArgLong("CX"), cy, nZoom;
	CSize sizeSLI;
	sptrSLI->GetSize(&sizeSLI);
	CPoint ptSLIOffs;
	sptrSLI->GetOrigin(&ptSLIOffs);
	if (cx == 0)
	{
		int nLimitX = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\Limits", "LimitX", 4096);
		int nLimitY = GetValueInt(GetAppUnknown(), "\\Sew Large Image\\Limits", "LimitY", 3072);
		if (sizeSLI.cx > nLimitX || sizeSLI.cy > nLimitY)
		{
			double dZoomX = double(nLimitX)/double(sizeSLI.cx);
			double dZoomY = double(nLimitY)/double(sizeSLI.cy);
			double dZoom = min(dZoomX,dZoomY);
			nZoom = (int)ceil(1./dZoom);
			SIZE aSizes[2];
			aSizes[0].cx = (int)(sizeSLI.cx/nZoom);
			aSizes[0].cy = (int)(sizeSLI.cy/nZoom);
			aSizes[1].cx = (int)(sizeSLI.cx/nZoom/2);
			aSizes[1].cy = (int)(sizeSLI.cy/nZoom/2);
			CSaveSewImgDlg dlg(aSizes, 2, 3);
			if (dlg.DoModal() == IDCANCEL)
				return S_FALSE;
			cx = aSizes[dlg.m_nFmt].cx;
			cy = aSizes[dlg.m_nFmt].cy;
			if (dlg.m_nFmt == 1)
				nZoom *= 2;
		}
		else
		{
			nZoom = 1;
			cx = sizeSLI.cx;
			cy = sizeSLI.cy;
		}
	}
	else
	{
		int nZoom = sizeSLI.cx/cx;
		cy = sizeSLI.cy/nZoom;
	}
	img->CreateImage(cx, cy, _bstr_t("RGB"), -1);
	FillImage(img, cx, cy, false, RGB(255,255,255));
	CDistMap DMTotal;
	DMTotal.CreateNew(cx,cy);
	long lpos = 0;
/*	sptrSLI->GetFirstFragmentPos(&lpos);
	while (lpos)
	{
		ISewFragmentPtr sptrFrag;
		sptrSLI->GetNextFragment(&lpos, &sptrFrag);
		CRect rc = GetFragmentRect(sptrFrag);
		rc.OffsetRect(-ptSLIOffs.x, -ptSLIOffs.y);
		CRect rcZ = CRect(rc.left/nZoom, rc.top/nZoom, rc.right/nZoom, rc.bottom/nZoom);
		DMTotal.AddRect(rcZ);
	}*/
	long lFragCount;
	sptrSLI->GetFragmentsCount(&lFragCount);
	int iFragment = 0;
	StartNotification(lFragCount,1,1);
	lpos = 0;
	sptrSLI->GetFirstFragmentPos(&lpos);
	while (lpos)
	{
		ISewFragmentPtr sptrFrag;
		sptrSLI->GetNextFragment(&lpos, &sptrFrag);
		CRect rc = GetFragmentRect(sptrFrag);
		rc.OffsetRect(-ptSLIOffs.x, -ptSLIOffs.y);
		CRect rcZ = CRect(rc.left/nZoom, rc.top/nZoom, rc.right/nZoom, rc.bottom/nZoom);
		CLoadFragment lf(sptrFrag, 1);
		int nZoom11 = 1;
		IUnknownPtr punkImgFrag;
		sptrFrag->GetImage(&nZoom11, &punkImgFrag);
		IImagePtr imgFrag(punkImgFrag);
		CDistMap DMCur;
		DMCur.CreateNew(cx,cy);
		DMCur.AddRect(rcZ);
		DMTotal.AddRect(rcZ);
		CopyImage(img, imgFrag, rcZ.TopLeft(), CPoint(0,0), rc.Size(), nZoom,
			&DMTotal, &DMCur);
		Notify(iFragment++);
	}
	FinishNotification();
	return true;

}

