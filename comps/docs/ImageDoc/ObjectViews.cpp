// ObjectViews.cpp : implementation file
//

#include "stdafx.h"
#include "imagedoc.h"
#include "ObjectViews.h"
#include "menuconst.h"
#include "wnd_misc.h"
#include "trajectory_int.h"
#include "\vt5\AWIN\profiler.h"
#include "\vt5\common2\class_utils.h"

IMPLEMENT_DYNCREATE(CObjectsView, CCmdTarget)
IMPLEMENT_DYNCREATE(CMasksView, CCmdTarget)
IMPLEMENT_DYNCREATE(CImageView, CCmdTarget)
IMPLEMENT_DYNCREATE(CPseudoView, CCmdTarget)

// {AC120473-9B39-11d3-A69B-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CObjectsView, szObjectsViewProgID, 
0xac120473, 0x9b39, 0x11d3, 0xa6, 0x9b, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {AC120474-9B39-11d3-A69B-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CMasksView, szMaskViewProgID, 
0xac120474, 0x9b39, 0x11d3, 0xa6, 0x9b, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {B0E325B2-435A-11d3-A60F-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CImageView, "ImageDoc.ImageView",
0xb0e325b2, 0x435a, 0x11d3, 0xa6, 0xf, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {2AD4E10C-4886-49c6-A058-57EAAAE26B7E}
GUARD_IMPLEMENT_OLECREATE(CPseudoView, szPseudoViewProgID, 
0x2ad4e10c, 0x4886, 0x49c6, 0xa0, 0x58, 0x57, 0xea, 0xaa, 0xe2, 0x6b, 0x7e);


BEGIN_MESSAGE_MAP(CObjectViewBase, CImageSourceView)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_CREATE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CObjectViewBase

void CObjectViewBase::OnTimer(UINT_PTR nIDEvent) 
{
	if( m_nHighlightMethod )
	{
		CRect	rect;
		GetWindowRect( rect );
		CPoint	point;
		GetCursorPos( &point );

		if( !rect.PtInRect( point ) )
		{
			//for( int i = 0; i < m_hilighted_objects.GetSize(); i++ )
			for (TPOS lPos = m_hilighted_objects.head(); lPos; lPos = m_hilighted_objects.next(lPos))
			{
				//IUnknown	*punk_o = (IUnknown*)m_hilighted_objects[i];
				IUnknown	*punk_o = m_hilighted_objects.get(lPos);
				CRect	rect = NORECT;
				IRectObjectPtr	ptr( punk_o );
				if( ptr ) 
					ptr->GetRect( &rect );

				InvalidateRect( ::ConvertToWindow( GetControllingUnknown(), rect ) );
			}

			//m_hilighted_objects.RemoveAll();
			m_hilighted_objects.clear();
		}
	}

	CImageSourceView::OnTimer(nIDEvent);
}


CObjectViewBase::CObjectViewBase()
{
	m_pObjectListHead = 0;
	m_pObjectListTail = 0;

	m_nHighlightMethod = 1;
	m_bClassHilightEnabled = false;
	m_bShowContours = true;
	m_bShowChildObjects = true;
	m_nHighlightOversectMethod = 1;
	m_nHighlightPhases = 0;
	
	//extended highlight
	m_clr_highlight_mask		= RGB( 255, 100, 100 );
	m_clr_highlight_inter_mask	= RGB( 240, 240, 240 );
	m_clrHighlightOversectColor = RGB( 255, 255, 255 );
	m_highlight_percent			= 0;
	m_highlight_inter_percent	= 0;
}

CObjectViewBase::~CObjectViewBase()
{
	if(m_pObjectListHead)
		delete m_pObjectListHead;
	m_pObjectListHead = 0;
	m_pObjectListTail = 0;
}

void CObjectViewBase::DrawLayers(CDC* pdc, BITMAPINFOHEADER* pbmih, byte* pdibBits, CRect rectPaint, bool bUseScrollZoom)
{
	//CTimeTest	time( true, "CObjectViewBase::DrawLayers" );
	ObjectList* pCur = m_pObjectListHead;
	while(pCur)
	{
		_DrawObjectList(pCur, *pdc, rectPaint, pbmih, pdibBits, bUseScrollZoom);
		pCur = pCur->pNext;
	}
}

bool CObjectViewBase::is_have_print_data()
{
	if( CImageSourceView::is_have_print_data() )
		return true;

	return ( m_pObjectListHead != 0 );
}


void CObjectViewBase::OnActivateView( bool bActivate, IUnknown *punkOtherView )
{
	if( !bActivate )
		statusShowObjectCountPage( false );
	else
		if( ::GetValueInt( ::GetAppUnknown(), OBJECT_COUNT_PANE_SECTION, "ImageSourceView", 0L ) )
			statusShowObjectCountPage( bActivate );

 	CImageSourceView::OnActivateView( bActivate, punkOtherView );
}


void CObjectViewBase::DoPocessNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if(!strcmp( pszEvent, szEventChangeObjectList))
	{
		long	lHint=*(long*)pdata;

		ObjectList* pCur = m_pObjectListHead;
		while(pCur)
		{
			pCur->m_objects.DoProcessNotify( pszEvent, punkFrom, punkHit, lHint );

			if(::GetObjectKey(punkFrom) == ::GetObjectKey(pCur->m_objects))
			{
				if( lHint == cncReset )
				{
					if( GetSafeHwnd() )
						Invalidate();
				}
				/*else
				if(  lHint == cncAdd|| lHint == cncRemove || lHint == cncChange )
				{
					POSITION pos = pCur->m_objects.GetFirstObjectPosition();
					while(pos)
					{
						IUnknownPtr	ptrObject( pCur->m_objects.GetNextObject(pos), false );
						CRect rect = GetObjectRect( ptrObject );
						rect = ConvertToWindow( GetControllingUnknown(), rect );

						if( GetSafeHwnd() )
							InvalidateRect( rect );
					}
				}*/
																	
				if( lHint == cncAdd||lHint == cncActivate||lHint == cncActivateParameter||lHint == cncReset)
				{
					IUnknown	*punkToActivate = (lHint == cncAdd||lHint == cncActivate)?punkHit : 0;
					_select_measurement( punkToActivate );
					if(punkToActivate!=0)
					{
						INamedDataObject2Ptr ptrNamedObject = punkToActivate;
						if(ptrNamedObject!=0)
						{
							TPOS lpos=0;
							ptrNamedObject->GetFirstChildPosition( &lpos );
							while( lpos )
							{
								IUnknown *punk=0;
								ptrNamedObject->GetNextChild( &lpos, &punk );
								if( !punk )continue;

								//ITrajectoryPtr	ptrTrajectory = punk;
								ISmartSelectorPtr ptrSelector(punk);
								punk->Release();

								//if(ptrTrajectory == 0) continue;
								if(ptrSelector == 0) continue;								
								//m_pframe->SelectObject( ptrTrajectory, true );
                                ptrSelector->SmartSelect(true, m_pframe);
							}
						}
					}
					_redraw_selection();
				}
				if( lHint == cncRemove )
				{
					//for( int i = m_hilighted_objects.GetSize()-1; i>=0; i-- )
					for (TPOS lPos = m_hilighted_objects.tail(); lPos;)
					{
						TPOS lPos1 = m_hilighted_objects.prev(lPos);
						//IUnknown	*punk = (IUnknown*)m_hilighted_objects[i];
						IUnknown	*punk = m_hilighted_objects.get(lPos);

						if( GetObjectKey( punk ) == GetObjectKey( punkHit ) )
						{
							//m_hilighted_objects.RemoveAt( i );
							m_hilighted_objects.remove(lPos);
						}
						lPos=lPos1;
					}
				}
				if( lHint == cncReset )
				{
					_select_active_object(pCur->m_objects);// SBT1742(12.04.2006)
					//m_pframe->EmptyFrame();
				}
			}

			pCur = pCur->pNext;
		}
	}
	else if( !strcmp( pszEvent, szEventNewSettings ) )
	{
		ObjectList* pCur = m_pObjectListHead;
		while(pCur)
		{
			pCur->m_objects.Attach(pCur->m_objects);
			if( GetSafeHwnd() )
				Invalidate();
			pCur = pCur->pNext;
		}
	}
}

void CObjectViewBase::LoadSettings()
{
	IUnknown	*punkApp = ::GetAppUnknown(false);
	CString sSection = GetSectionName();
	m_bShowName	 = ::GetValueInt( punkApp, sSection, szIVShowClassName, 0) != 0;
	m_nNamePos	 = ::GetValueInt( punkApp, sSection, szIVShowClassNamePos, 0);
	m_bShowNumber = ::GetValueInt( punkApp, sSection, szIVShowObjectNumber, 0) != 0;
	m_nNumberPos  = ::GetValueInt( punkApp, sSection, szIVShowObjectNumberPos, 0);
	m_nHighlightMethod = ::GetValueInt( punkApp, sSection, "HilightEnabled", m_nHighlightMethod );	
	m_bClassHilightEnabled  = ::GetValueInt( punkApp, sSection, "ClassHilightEnabled", m_bClassHilightEnabled ) != 0;	
	m_bShowContours  = ::GetValueInt( punkApp, sSection, "ShowContours", m_bShowContours ) != 0;
	m_bShowChildObjects  = ::GetValueInt( punkApp, sSection, "ShowChildObjects", m_bShowChildObjects) != 0;
	m_nHighlightPhases = ::GetValueInt( punkApp, sSection, "HighlightPhases", m_nHighlightPhases );

	//extended highlight
	m_clr_highlight_mask		= ::GetValueColor( punkApp, sSection, "HilightColor", m_clr_highlight_mask );
	m_clr_highlight_inter_mask	= ::GetValueColor( punkApp, sSection, "HilightInteractiveColor", m_clr_highlight_inter_mask );
	m_clrHighlightOversectColor	= ::GetValueColor( punkApp, sSection, "HighlightOversectColor", m_clrHighlightOversectColor );

	m_highlight_percent			= ::GetValueInt( punkApp, sSection, "HilightPercent", 0 );
	m_highlight_inter_percent	= ::GetValueInt( punkApp, sSection, "HilightInteractivePercent", 0 );
	m_nHighlightOversectMethod	= ::GetValueInt( punkApp, sSection, "HighlightOversectMethod", m_nHighlightOversectMethod );

	m_lenable_exended_move		= -1;

}


IUnknown* CObjectViewBase::__GetObjectByPoint( CPoint point)
{
	ObjectList* pCur = m_pObjectListHead;
	while(pCur)
	{
		IUnknown* punk = 0;
		POSITION	pos = _GetObjectByPoint(&pCur->m_objects, point );
		if( pos )
			punk = pCur->m_objects.GetNextObject(pos);

		if( punk )
			return punk;

		pCur = pCur->pNext;
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// CObjectViewBase message handlers

static CSize GetBaseSize(IUnknown *punkObj)
{
	INamedDataObject2Ptr ptrN(punkObj);
	if (ptrN != 0)
	{
		IUnknownPtr	punkData;
		ptrN->GetData(&punkData);
		INamedDataPtr ptrData(punkData);
		if (ptrData)
		{
			GUID guidBase;
			ptrN->GetBaseKey( &guidBase );
			IUnknownPtr punkBase;
			ptrData->GetBaseGroupBaseObject(&guidBase, &punkBase);
			IImagePtr sptrImage(punkBase);
			int cx,cy;
			if (sptrImage != 0)
			{
				sptrImage->GetSize(&cx, &cy);
				return CSize(cx,cy);
			}
		}
	}
	return CSize(0,0);
}

void CObjectViewBase::_recalc_rect( BOOL bUpdate /* = false*/, IUnknown *punkExclude /*= 0*/ )
{
	ObjectList* pCur = m_pObjectListHead;
	while(pCur)
	{
		if(pCur->m_objects != 0)
		{
			if( !bUpdate )
				m_listObjects.AddTail(pCur->m_objects);

			if (m_image == 0)
			{
				CSize szBase = GetBaseSize(pCur->m_objects);
				m_size_client.cx = max(szBase.cx, m_size_client.cx);
				m_size_client.cy = max(szBase.cy, m_size_client.cy );
			}

			TPOS	lpos = 0;
			INamedDataObject2Ptr	ptr_objects( (IUnknown*)pCur->m_objects );

			ptr_objects->GetFirstChildPosition( &lpos );

			while( lpos ) 
			{
				IUnknown	*punk = 0;
				ptr_objects->GetNextChild( &lpos, &punk );
				
				if( bUpdate && punkExclude && punkExclude == punk )
				{
					if( punk )
						punk->Release();

					continue;
				}


				IRectObjectPtr	ptrRect = punk;
				punk->Release();

				if( ptrRect )
				{
					CRect	rect = NORECT;
					ptrRect->GetRect( &rect );

					m_size_client.cx = max( rect.right, m_size_client.cx );
					m_size_client.cy = max( rect.bottom, m_size_client.cy );
				}

			}
		}
		pCur = pCur->pNext;
	}

	CImageSourceView::_recalc_rect( bUpdate, punkExclude );
}

void CObjectViewBase::_AttachObjects(IUnknown* punkContextFrom)
{
	bool bPrevPhases = m_bPhases;

	bool bInvalidate = m_pObjectListHead != 0;
	ObjectList* pCur = m_pObjectListHead;
	while(pCur)
	{
		if( m_pframe )
			m_pframe->UnselectObjectsWithSpecifiedParent( (IUnknown*)pCur->m_objects );
		pCur = pCur->pNext;
	}
	if(m_pObjectListHead)
		delete m_pObjectListHead;
	m_pObjectListHead = 0;
			
	bool bChangeList = ::GetObjectKey( punkContextFrom ) == ::GetObjectKey( GetControllingUnknown() );
	CImageSourceView::_AttachObjects(punkContextFrom);

	IDataContext2Ptr sptrContext(punkContextFrom);

	LONG_PTR nPos = 0;
	sptrContext->GetFirstSelectedPos(_bstr_t(szTypeObjectList), &nPos);

	bool bFirstPass = true;

	while(nPos)
	{

#ifndef SupportLayers
			if(!bFirstPass)
				break;
#endif	

		IUnknown* punkObject = 0;
		sptrContext->GetNextSelected(_bstr_t(szTypeObjectList), &nPos, &punkObject);
		if(punkObject)
		{
			ObjectList* pCur = new ObjectList(punkObject);
			if(!m_pObjectListHead)
				m_pObjectListTail = m_pObjectListHead = pCur;
			else
			{
				m_pObjectListTail->pNext = pCur;
				m_pObjectListTail = pCur;
			}
			if (_GetNamedDataInt(punkObject, _bstr_t(""), _bstr_t("Phases"), 0)==1)
				m_bPhases = true;
			else
				m_bPhases = false;
			punkObject->Release();
			bInvalidate = true;
		}

		bFirstPass = false;
	}

	if( bChangeList )
		_recalc_rect();

	if( GetValueInt( GetAppUnknown(), "\\ImageView", "EnableSelection", 1 ) != 0 )
	{
		//paul 30.05.2002. Activate "active" object
		IViewCtrlPtr ptr = GetControllingUnknown();
		if( ptr == 0 && m_pObjectListHead && ( (IUnknown*)m_pObjectListHead->m_objects ) != 0 )
		{
			INamedDataObject2Ptr ptr_objects( (IUnknown*)m_pObjectListHead->m_objects );
			if( ptr_objects )
			{
				TPOS lpos = 0;
				ptr_objects->GetActiveChild( &lpos );
				if( lpos )
				{
					IUnknown* punk = 0;
					ptr_objects->GetNextChild( &lpos, &punk );
					if( punk )
					{
						m_pframe->SelectObject( punk, false );
						_redraw_selection();
						punk->Release();
					}
				}
			}
		}
	}

	_UpdateScrollSizes();
	
	if (bPrevPhases != m_bPhases)
		LoadSettings();
}

bool CObjectViewBase::_DrawObjectList(ObjectList* pList, CDC &dc, CRect rectClip, BITMAPINFOHEADER *pbi, byte *pdib, bool bUseScrollZoom)
{
	int nCount = 1; 
	
	DWORD	dwDrawFlags = DrawClip;
	//if(m_dwShowFlags & isfContours)
	if( m_bShowContours )
		dwDrawFlags |= DrawContours;

	if((m_dwShowFlags & isfPseudoImage) || (m_dwShowFlags & isfFilledOnlyObjects))
		dwDrawFlags |= DrawMask;
	else
		dwDrawFlags |= DrawImage;

	int bmp_cx=0, bmp_cy=0;
	if( pdib!=0 && pbi!=0 && m_bClassHilightEnabled )
	{
		bmp_cx = pbi->biWidth;
		bmp_cy = pbi->biHeight;
	}
	smart_alloc(pMaskCounters, byte, bmp_cx*bmp_cy);
	if(bmp_cx*bmp_cy) ZeroMemory(pMaskCounters, bmp_cx*bmp_cy); // обнулим счетчики
	
	POSITION	pos = pList->m_objects.GetHeadPosition();
	while( pos && !m_bObjCountingOnly )
	{
		CMeasObjectWrp	*pobject = pList->m_objects.GetNext( pos );
		bool	bActive = m_pframe->CheckObjectInSelection( pobject->m_nKey );

		m_nActiveNumber = m_nActiveNumber = (bActive) ? nCount : m_nActiveNumber;
		COLORREF	cr = _get_class_color( _get_object_class( pobject->m_ptrCalc ) );		

		if (!bActive)
			_DrawImage(dc, pobject->GetImage(), rectClip, dwDrawFlags, cr, pbi, pdib, bUseScrollZoom);
/*
		CRect rect = pobject->GetImageRect();
		if(pobject && pobject->m_ptrMeas != 0)
			pobject->m_ptrMeas->SetDrawingNumber(nCount++);

		if( m_bFromPrint && pobject->m_bHasManualMeasurement && m_bShowChildObjects )
			_DrawMeasObjects(dc, rectClip, cr, *pobject, odsNormal);

		if( m_bFromPrint && rect != NORECT )
		{
			_DrawObjectNumber(dc, *pobject, rectClip, rect, cr);
			_DrawObjectClass(dc, *pobject, rectClip, rect, cr);
		}
*/
	}

	pos = pList->m_objects.GetHeadPosition();
	while( pos && !m_bObjCountingOnly )
	{
		CMeasObjectWrp	*pobject = pList->m_objects.GetNext( pos );
		COLORREF	cr = _get_class_color( _get_object_class( pobject->m_ptrCalc ) );		
		HilightObjectImage(dc, pbi, pdib, pMaskCounters, rectClip, pobject->GetImage(), cr);
	}

	pos = pList->m_objects.GetHeadPosition();
	while( pos )
	{
		CMeasObjectWrp	*pobject = pList->m_objects.GetNext( pos );

		COLORREF	cr = _get_class_color( _get_object_class( pobject->m_ptrCalc ) );

		CRect rect = pobject->GetImageRect();
		if(pobject && pobject->m_ptrMeas != 0)
		{
			long lNum = 0;
			pobject->m_ptrMeas->GetDrawingNumber( &lNum );
			if( lNum != nCount )
			{
				m_object_coords.remove_key((IUnknown*)*pobject );
				m_object_coords_num.remove_key((IUnknown*)*pobject );
			}

			pobject->m_ptrMeas->SetDrawingNumber( nCount++ );
		}

		if( !m_bObjCountingOnly && m_bFromPrint && pobject->m_bHasManualMeasurement && m_bShowChildObjects )
			_DrawMeasObjects(dc, rectClip, cr, *pobject, odsNormal);


		if( !m_bObjCountingOnly && m_bFromPrint && rect != NORECT &&
			!m_pframe->CheckObjectInSelection(*pobject) )
		{
			_DrawObjectNumber(dc, *pobject, rectClip, rect, cr);
			_DrawObjectClass(dc, *pobject, rectClip, rect, cr);
		}

	}

	return true;
}



POSITION CObjectViewBase::_GetObjectByPoint(CMeasObjListWrp* pObjects, CPoint point)
{

	POSITION posStart = pObjects->GetCurPosition();
	if (!posStart)
		posStart = pObjects->GetFirstObjectPosition();

	if (!posStart)
		return 0;

	//skip one object
	pObjects->GetNextObject(posStart)->Release();

	POSITION pos = posStart;
	while (pos)
	{
		POSITION	posSave = pos;
		CObjectWrp	object(pObjects->GetNextObject(pos), false);
		if (CheckChildByPoint(pObjects, object, point))
			return posSave;
		if (object.PtInObject(point))
			return posSave;
	}

	pos = pObjects->GetFirstObjectPosition();

	while (pos != posStart)
	{
		POSITION	posSave = pos;
		CObjectWrp	object(pObjects->GetNextObject(pos), false);

		if (CheckChildByPoint(pObjects, object, point))
			return posSave;
		
		if (object.PtInObject(point))
			return posSave;
	}


	return 0;
}

void CObjectViewBase::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
    if(!strcmp( pszEvent, szEventNewSettings ))
	{
		if (m_pObjectListHead && 
			_GetNamedDataInt(m_pObjectListHead->m_objects, _bstr_t(""), _bstr_t("Phases"), 0)==1)
			m_bPhases = true;
		else
			m_bPhases = false;
		LoadSettings();
		Invalidate();
	}
	else if( !strcmp( pszEvent, szEventChangeObjectList ) )
	{
		long	lHint= *(long*)pdata;
		bool	bfound = false;

		if( lHint == cncAdd )
		{
			m_object_coords.clear();
			m_object_coords_num.clear();
		}

		if( lHint == cncReset )
		{
			GUID guid_from = ::GetObjectKey( punkFrom );

			ObjectList* pCur = m_pObjectListHead;
			while(pCur)
			{
				if( ::GetObjectKey( pCur->m_objects ) == guid_from )
					bfound = true;
				pCur = pCur->pNext;
			}
		}

		if( bfound && GetSafeHwnd() )
			Invalidate();
	}

	CImageSourceView::OnNotify( pszEvent, punkHit, punkFrom, pdata, cbSize );	
}

bool CObjectViewBase::CheckChildByPoint(CMeasObjListWrp* pObjects, IUnknown * punkObj, CPoint point)
{
//	CRect rcZone (point.x - 5, point.y - 5, point.x + 5, point.y + 5);

	INamedDataObject2Ptr ptrNamedObject = punkObj;
	ICalcObjectContainerPtr	ptrCont = (IUnknown*)(*pObjects);
	INamedDataObject2Ptr	ptrList = ptrCont;

	if (ptrNamedObject == 0 || ptrCont == 0)
		return false;

	// get active param
	POSITION lpos; long lkey;
	ptrCont->GetCurentPosition((LONG_PTR*)&lpos, &lkey);


	//get active child position
	POSITION lposA = 0;
	{
		ptrNamedObject->GetFirstChildPosition( &lpos );
		while( lpos )
		{
			IUnknown *punk = 0;
			lposA = lpos;
			ptrNamedObject->GetNextChild( &lpos, &punk );
			if( !punk )continue;
			IManualMeasureObjectPtr	ptrM = punk;
			punk->Release();

			if( ptrM == 0 )continue;
			
			long	lTestKey = -1;
			ptrM->GetParamInfo( &lTestKey, 0 );
			if( lTestKey == lkey )
				break;
			lposA = 0;
		}
	}



	// get pos of active child
	TPOS lstart = 0;
	if( lposA != 0 )
	{
		lstart = lposA;
		ptrNamedObject->GetNextChild( &lstart, 0 );
	}

	// if we have no active child ->try to get first child
	if (!lstart)
		ptrNamedObject->GetFirstChildPosition(&lstart);

	if (!lstart)
		return false;

	// save start pos
	POSITION lend = 0;
	lpos = lstart;
	
	while( lpos != lend )
	{
		//LPOS lposCur = lpos;
		IUnknown *punk = 0;
		ptrNamedObject->GetNextChild( &lpos, &punk );
		if( !punk )continue;
		
		IManualMeasureObjectPtr ptrMeasure = punk;
		punk->Release();
		if( ptrMeasure == 0 )continue;

		long lReturn = false;

		ptrMeasure->CrossPoint(point, &lReturn);

		if( !lReturn ) continue;


		//activate image



		long	lParamKey		= -1;
		DWORD	dwParamType	= -1;

		ptrMeasure->GetParamInfo( &lParamKey, &dwParamType );

		ParameterContainer* pk = 0;
		if( S_OK == ptrCont->ParamDefByKey( lParamKey, &pk ) )
		{
			if( pk && pk->lpos > 0 )
			{
				ptrCont->SetCurentPosition( pk->lpos );
				return true;
			}
		}
	}

	return false;
}

void CObjectViewBase::OnBeginDragDrop()
{
	ObjectList* pCur = m_pObjectListHead;
	while(pCur)
	{
		POSITION pos = pCur->m_objects.GetFirstObjectPosition();

		CRect rc = NORECT;
		while (pos)
		{
			CObjectWrp	object(pCur->m_objects.GetNextObject(pos), false);
			if (!m_pframe->CheckObjectInSelection(object))
			{
				CRect rcObj = _GetMeasObjectRect(object, true);
				rc.UnionRect(&rc, &rcObj);
			}
		}
		InvalidateRect(&rc);

		pCur = pCur->pNext;
	}
}

void CObjectViewBase::OnEndDragDrop()
{
	ObjectList* pCur = m_pObjectListHead;
	while(pCur)
	{
		POSITION pos = pCur->m_objects.GetFirstObjectPosition();

		CRect rc = NORECT;
		while (pos)
		{
			CObjectWrp	object(pCur->m_objects.GetNextObject(pos), false);
			if (!m_pframe->CheckObjectInSelection(object))
			{
				CRect rcObj = _GetMeasObjectRect(object, true);
				rc.UnionRect(&rc, &rcObj);
			}
		}
		InvalidateRect(&rc);

		pCur = pCur->pNext;
	}
}

DWORD CObjectViewBase::GetMatchType( const char *szType )
{
	if( !strcmp( szType, szTypeImage ) )
		return mvFull;//mvPartial;
	else
	if( !strcmp(szType, szDrawingObjectList))
		return mvPartial;
	if( !strcmp(szType, szTypeObjectList))
		return mvPartial;
	if( !strcmp(szType, szTypeObject))
		return mvPartial;
	if( !strcmp(szType, szTypeManualMeasObject))
		return mvPartial;
	else
		return mvNone;
}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
//CObjectsView implementation
CObjectsView::CObjectsView()
{
	m_dwShowFlags = isfFilledImageExceptObjects;
	m_sName = c_szCObjectsView;
	m_sUserName.LoadString(IDS_OBJECTSVIEW_NAME);
}

const char* CObjectsView::GetViewMenuName()
{
	return szObjectsView;
}

/////////////////////////////////////////////////////////////////////////////////////////
//CMasksView implementation
CMasksView::CMasksView()
{
	m_dwShowFlags = isfFilledOnlyObjects;
	m_sName = c_szCMasksView;
	m_sUserName.LoadString(IDS_MASKVIEW_NAME);
}

const char* CMasksView::GetViewMenuName()
{return szObjectsView;}

/////////////////////////////////////////////////////////////////////////////////////////
//CImageView implementation
CImageView::CImageView()
{
	m_dwShowFlags = isfContours;
	m_sName = c_szCImageView;
	m_sUserName.LoadString(IDS_IMAGEVIEW_NAME);
	// Context menu for selection, drawing objects, etc...
	//m_bShowChildContextMenu = false;
}

const char* CImageView::GetViewMenuName()
{return szObjectsView;}

/////////////////////////////////////////////////////////////////////////////////////////
//CPseudoView implementation
CPseudoView::CPseudoView()
{
	m_dwShowFlags = isfPseudoImage;
	m_sName = c_szCPseudoView;
	m_sUserName.LoadString(IDS_PSEUDOVIEW_NAME);
}

const char* CPseudoView::GetViewMenuName()
{return szObjectsView;}

DWORD CPseudoView::GetMatchType( const char *szType )
{
	if( !strcmp(szType, szDrawingObjectList))
		return mvPartial;
	if( !strcmp(szType, szTypeObjectList))
		return mvPartial;
	else
		return mvNone;
}




void CObjectViewBase::_select_measurement( IUnknown *punkObjectToActivate )
{
	bool	bActivateParameter = punkObjectToActivate == 0;
	ObjectList* p = m_pObjectListHead;
	if( !p )return;
	ICalcObjectContainerPtr	ptrContainer( (IUnknown*)p->m_objects );
	long	lActiveKey = -1;
	POSITION	lpos = 0;
	ptrContainer->GetCurentPosition((LONG_PTR*)&lpos, &lActiveKey);

	//key valid only if manual measurement selected
	if( lpos )
	{
		ParameterContainer *ppContainer =0;
		ptrContainer->GetNextParameter((LONG_PTR*)&lpos, &ppContainer);
		if( ppContainer->type == etUndefined )
			lActiveKey = -1;
	}

	IUnknown	*punk = 0;

	INamedDataObject2Ptr	ptrNamedList( ptrContainer );
	INamedDataObject2Ptr	ptrNamedObject = punkObjectToActivate;

	if( ptrNamedObject == 0 )
	{
		ptrNamedList->GetActiveChild( &lpos );
		if( lpos == 0  )return;
		ptrNamedList->GetNextChild( &lpos, &punk );
		ptrNamedObject = punk;
		if( !punk )return;
		punk->Release();
	}

	if( lActiveKey == -1 )
	{
		//if( !bActivateParameter )
		//	m_pframe->SelectObject( ptrNamedObject, false );
		return;
	}

	// A.M. fix. Do not deactivate measure object when no manual measure
	// child (click on "-" in measurement table).
	bool bChildFound = false;
	ptrNamedObject->GetFirstChildPosition( &lpos );
	while( lpos )
	{
		ptrNamedObject->GetNextChild( &lpos, &punk );
		if( !punk )continue;
		IManualMeasureObjectPtr	ptrManualObject = punk;
		punk->Release();
		if( ptrManualObject == 0 )continue;
		long	lTestKey = -1;
		ptrManualObject->GetParamInfo( &lTestKey, 0 );
		if( lTestKey == lActiveKey )
		{
			bChildFound = true;
			break;
		}
	}
	if (!bChildFound)
		return;


	m_pframe->EmptyFrame();
	
	ptrNamedObject->GetFirstChildPosition( &lpos );

	while( lpos )
	{
		ptrNamedObject->GetNextChild( &lpos, &punk );
		if( !punk )continue;

		IManualMeasureObjectPtr	ptrManualObject = punk;
		punk->Release();

		if( ptrManualObject == 0 )continue;

		long	lTestKey = -1;
		ptrManualObject->GetParamInfo( &lTestKey, 0 );

		if( lTestKey == lActiveKey )
		{
			m_pframe->SelectObject( ptrManualObject, false );
			return;
		}
	}
//	if( !bActivateParameter )
//		m_pframe->SelectObject( ptrNamedObject, false );

}

void CObjectViewBase::_redraw_selection()
{
	CRect	rectInvalidate;
	rectInvalidate.SetRectEmpty();
	
	for( int i = 0; i < m_pframe->GetObjectsCount(); i++ )
	{
		IUnknown	*punk = m_pframe->GetObjectByIdx( i );
		CRect	rect = ::ConvertToWindow( GetControllingUnknown(), GetObjectRect( punk ) );
		if( rectInvalidate.IsRectEmpty() )rectInvalidate = rect;
		else	rectInvalidate.UnionRect( rectInvalidate, rect );
		
		punk->Release();
	}
}

void CObjectViewBase::OnMouseMove(UINT nFlags, CPoint point)
{
	CImageSourceView::OnMouseMove( nFlags, point );

	if( m_nHighlightMethod )
	{
		IUnknown	*punkView = GetControllingUnknown();

		CPoint	pt = ConvertToClient( punkView, point );

		CPtrArray		hilighted_objects;
		ObjectList* pCur = m_pObjectListHead;
		while(pCur)
		{
		
			INamedDataObject2Ptr	ptrList( (IUnknown*)pCur->m_objects );

			TPOS	lpos_o = 0;
			ptrList->GetFirstChildPosition( &lpos_o );

			pCur = pCur->pNext;

			while( lpos_o != 0 )
			{
				IUnknown	*punk_obj = 0;

				ptrList->GetNextChild( &lpos_o, &punk_obj );

				if( punk_obj == 0 )continue;

				IRectObjectPtr	ptrRect( punk_obj );
				punk_obj->Release();
				if( ptrRect != 0 )
				{
					long	lres = 0;
					ptrRect->HitTest( pt, &lres );

					if( lres )
					{
						hilighted_objects.Add( ptrRect );
					}
				}
			}
		}

		int	i;
		
		for( i = 0; i < hilighted_objects.GetSize(); i++ )
		{
			bool	bFound = false;
			IUnknown	*punk_o = (IUnknown*)hilighted_objects[i];
			for (TPOS lPos = m_hilighted_objects.head(); lPos; lPos = m_hilighted_objects.next(lPos))
			//for( int j = 0; j < m_hilighted_objects.GetSize(); j++ )
			{
				//IUnknown	*punk_t = (IUnknown*)m_hilighted_objects[j];
				IUnknown	*punk_t = m_hilighted_objects.get(lPos);
				if( punk_t == punk_o )
				{
					bFound = true;
					break;
				}
			}
			if( !bFound )
			{
				CRect	rect = NORECT;
				IRectObjectPtr	ptr( punk_o );
				if( ptr )
					ptr->GetRect( &rect );

				InvalidateRect( ::ConvertToWindow( punkView, rect ) );
			}
		}

		//for( i = 0; i < m_hilighted_objects.GetSize(); i++ )
		for (TPOS lPos = m_hilighted_objects.head(); lPos; lPos = m_hilighted_objects.next(lPos))
		{
			bool	bFound = false;
			//IUnknown	*punk_o = (IUnknown*)m_hilighted_objects[i];
			IUnknown	*punk_o = m_hilighted_objects.get(lPos);
			for( int j = 0; j < hilighted_objects.GetSize(); j++ )
			{
				IUnknown	*punk_t = (IUnknown*)hilighted_objects[j];
				if( punk_t == punk_o )
				{
					bFound = true;
					break;
				}
			}
			if( !bFound )
			{
				CRect	rect = NORECT;
				IRectObjectPtr	ptr( punk_o );
				if( ptr )
					ptr->GetRect( &rect );

				InvalidateRect( ::ConvertToWindow( punkView, rect ) );
			}
		}

		//m_hilighted_objects.RemoveAll();
		m_hilighted_objects.clear();
		//m_hilighted_objects.Append( hilighted_objects );
		for( int i = 0; i < hilighted_objects.GetSize(); i++ )
		{
			IUnknown	*punk_o = (IUnknown*)hilighted_objects[i];
			m_hilighted_objects.add_tail(punk_o);
		}
	}

	//UpdateWindow();
}

int CObjectViewBase::OnCreate(LPCREATESTRUCT lpcs)
{
	LoadSettings();
	return CImageSourceView::OnCreate(lpcs);
}

const int BodyMask = 1<<7;

void CObjectViewBase::DrawHilightedObjects(CDC &dc, BITMAPINFOHEADER *pbmih, byte *pdibBits, CRect rectPaint )
{
	if( m_nHighlightMethod )
	{
		
		bool binteractive = false;
		{
			IUnknown *punkAM = ::_GetOtherComponent( GetAppUnknown(), IID_IActionManager );
			IActionManagerPtr ptmAM( punkAM );
			punkAM->Release();	punkAM = 0;

			if( ptmAM )
			{
				IUnknown* punk_action = 0;
				ptmAM->GetRunningInteractiveAction( &punk_action );
				if( punk_action )
				{								
					punk_action->Release();
					punk_action = 0;
					binteractive = true;
				}
			}
		}

		COLORREF clr_mask = ( !binteractive ? m_clr_highlight_mask : m_clr_highlight_inter_mask );

		double	fZoom = ::GetZoom( GetControllingUnknown() );
		CPoint	ptScroll = ::GetScrollPos( GetControllingUnknown() );

		//for( long lobj = 0; lobj < m_hilighted_objects.GetSize(); lobj++ )
		for (TPOS lPos = m_hilighted_objects.head(); lPos; lPos = m_hilighted_objects.next(lPos))
		{
			//IUnknown	*punk = (IUnknown*)m_hilighted_objects[lobj];
			IUnknown	*punk = m_hilighted_objects.get(lPos);
					
			IMeasureObjectPtr	ptrObject( punk );

			if( ptrObject == 0 )continue;
			IUnknown	*punkImage = 0;
			ptrObject->GetImage( &punkImage );

			if( punkImage == 0 )continue;
			IImage3Ptr	ptrImage( punkImage );
			punkImage->Release();

			if( ptrImage == 0 )continue;

			IUnknown	*punkCC = 0;
			ptrImage->GetColorConvertor( &punkCC );

			if( punkCC == 0 )continue;
			punkCC->Release();

			IColorConvertorExPtr	ptrCCEx( punkCC );
			if( ptrCCEx == 0 )continue;

			CRect	rect = NORECT;
			IRectObjectPtr	ptrRect = punk;
			if( ptrRect )
				ptrRect->GetRect( &rect );

			rect = ::ConvertToWindow( GetControllingUnknown(), rect );
			if( rect.Width() <= 0 || rect.Height() <= 0 )continue;

			if( m_nHighlightMethod==1 )
			{
				ptrCCEx->ConvertImageToDIBRect( pbmih, pdibBits, rectPaint.TopLeft(), ptrImage, rectPaint, 
					CPoint(0, 0), fZoom, ptScroll, RGB( 255, 0, 0 ), cidrHilight|cidrUseImageOffset );
			}
			else
			{
				// для 3 метода подменим цвет на цвет класса
				if(m_nHighlightMethod==3)
				{
					ICalcObjectPtr sptrCalc(ptrObject);
					if(sptrCalc == 0) continue;
					int nClass = get_object_class(sptrCalc);
					clr_mask = get_class_color(	nClass );					
				}
				
				POINT ptOffset;	::ZeroMemory( &ptOffset, sizeof(POINT) );
				int img_cx = 0;
				int img_cy = 0;
				ptrImage->GetSize( &img_cx, &img_cy );
				ptrImage->GetOffset( &ptOffset );

				CRect rcFill;
				if( !rcFill.IntersectRect( &rect, rectPaint ) )
					continue;

				rcFill -= rectPaint.TopLeft();

				if( rcFill.left < 0 )
					rcFill.left = 0;

				if( rcFill.top < 0 )
					rcFill.top = 0;

				if( rcFill.right > pbmih->biWidth )
					rcFill.right = pbmih->biWidth;

				if( rcFill.bottom > pbmih->biHeight )
					rcFill.bottom = pbmih->biHeight;

				BYTE rMask = GetRValue( clr_mask );
				BYTE gMask = GetGValue( clr_mask );
				BYTE bMask = GetBValue( clr_mask );

				int cx	= pbmih->biWidth;
				int cy	= pbmih->biHeight;
				int	cx4	= (cx*3+3)/4*4;

				byte bImg, gImg, rImg;
				int bImgN, gImgN, rImgN;

				CPoint pt_img_offset = rectPaint.TopLeft() - rect.TopLeft();
				for( int _y=rcFill.top;_y<rcFill.bottom;_y++ )
				{
					int y = cy - _y - 1;
					long nRowOffset = ( y ) * cx4;

					int y_img = int( ( pt_img_offset.y  + _y ) / m_fZoom );

					if( y_img < 0 || y_img >= img_cy )
						continue;

					byte *pmask = 0;
					ptrImage->GetRowMask( y_img, &pmask );


					for( int x=rcFill.left;x<rcFill.right;x++ )
					{
						int x_img = int( ( pt_img_offset.x  + x ) / fZoom );

						if( x_img < 0 || x_img >= img_cx )
							continue;

						if( ! (pmask[x_img] & BodyMask) )
							continue;

						bImg = pdibBits[ nRowOffset + 3 * x ];
						gImg = pdibBits[ nRowOffset + 3 * x + 1 ];
						rImg = pdibBits[ nRowOffset + 3 * x + 2 ];

						bImgN = bImg * bMask / 255;
						gImgN = gImg * gMask / 255;
						rImgN = rImg * rMask / 255;

						bImgN += ( !binteractive ? m_highlight_percent : m_highlight_inter_percent ) * 255 / 100;
						gImgN += ( !binteractive ? m_highlight_percent : m_highlight_inter_percent ) * 255 / 100;
						rImgN += ( !binteractive ? m_highlight_percent : m_highlight_inter_percent ) * 255 / 100;

						if( abs( bImg - bImgN ) < 15 )
						{
							if( bImg < 128 )
								bImgN += 15;
							else
								bImgN -= 15;
						}
						if( abs( gImg - gImgN ) < 15 )
						{
							if( gImg < 128 )
								gImgN += 15;
							else
								gImgN -= 15;
						}
						if( abs( rImg - rImgN ) < 15 )
						{
							if( rImg < 128 )
								rImgN += 15;
							else
								rImgN -= 15;
						}

						if( bImgN < 0 )		bImgN = 0;
						if( bImgN > 255 )	bImgN = 255;

						if( gImgN < 0 )		gImgN = 0;
						if( gImgN > 255 )	gImgN = 255;

						if( rImgN < 0 )		rImgN = 0;
						if( rImgN > 255 )	rImgN = 255;

						pdibBits[ nRowOffset + 3 * x + 0 ] = (byte)bImgN;
						pdibBits[ nRowOffset + 3 * x + 1 ] = (byte)gImgN;
						pdibBits[ nRowOffset + 3 * x + 2 ] = (byte)rImgN;
					}
				}			

				
			}
		}
	}
}

void CObjectViewBase::HilightObjectImage(CDC &dc, BITMAPINFOHEADER *pbmih, byte *pdibBits, byte *pMaskCounters, CRect rectPaint, IImage3Ptr ptrImage, COLORREF clr_highlight)
{
	if( pdibBits==0 || pbmih==0 || pMaskCounters==0 ) return;
	if( m_bClassHilightEnabled )
	{
		if( ptrImage == 0 ) return;

		POINT ptOffset;	::ZeroMemory( &ptOffset, sizeof(POINT) );
		int img_cx = 0;
		int img_cy = 0;
		ptrImage->GetSize( &img_cx, &img_cy );
		ptrImage->GetOffset( &ptOffset );
		CRect	rect2(ptOffset,CSize(img_cx,img_cy));
		CRect	rect(
			int( rect2.left*m_fZoom-m_pointScroll.x ), 
			int( rect2.top*m_fZoom-m_pointScroll.y ),
			int( rect2.right*m_fZoom-m_pointScroll.x ), 
			int( rect2.bottom*m_fZoom-m_pointScroll.y ) );


		CRect rcFill;
		if( !rcFill.IntersectRect( &rect, rectPaint ) )
			return;

		rcFill -= rectPaint.TopLeft();

		if( rcFill.left < 0 )
			rcFill.left = 0;

		if( rcFill.top < 0 )
			rcFill.top = 0;

		if( rcFill.right > pbmih->biWidth )
			rcFill.right = pbmih->biWidth;

		if( rcFill.bottom > pbmih->biHeight )
			rcFill.bottom = pbmih->biHeight;

		int rMask = GetRValue( clr_highlight );
		int gMask = GetGValue( clr_highlight );
		int bMask = GetBValue( clr_highlight );

		byte rHighlightOversect = GetRValue( m_clrHighlightOversectColor );
		byte gHighlightOversect = GetGValue( m_clrHighlightOversectColor );
		byte bHighlightOversect = GetBValue( m_clrHighlightOversectColor );

		int cx	= pbmih->biWidth;
		int cy	= pbmih->biHeight;
		int	cx4	= (cx*3+3)/4*4;

		int alpha = int(m_highlight_percent/100.0*256+0.5);

		CPoint pt_img_offset = rectPaint.TopLeft() - rect.TopLeft();
		for( int _y=rcFill.top;_y<rcFill.bottom;_y++ )
		{
			int y = cy - _y - 1;
			long nRowOffset = y*cx4;
			long nCountRowOffset = y*cx;

			int y_img = int( ( pt_img_offset.y  + _y ) / m_fZoom );

			if( y_img < 0 || y_img >= img_cy )
				continue;

			byte *pmask = 0;
			ptrImage->GetRowMask( y_img, &pmask );

			byte *pb = pdibBits + y*cx4 + 3*rcFill.left;
			byte *pcnt = pMaskCounters + y*cx + rcFill.left;

			if(m_nHighlightPhases)
			{	// подсветка по фазам
				for( int x=rcFill.left; x<rcFill.right; x++,pb+=3,pcnt++ )
				{
					int x_img = int( ( pt_img_offset.x  + x ) / m_fZoom );

					if( x_img < 0 || x_img >= img_cx )
						continue;

					int m=pmask[x_img];
					if( !(m & BodyMask) )
						continue;

					int count = *pcnt;
					*pcnt=count+1;
					int alpha2 = alpha*256 / (256 + alpha*count);

					COLORREF clPhase=g_pdwBinaryColors[255-m];
                    // 255->0, 254->1 и так далее - для Binary цветов
					if(m_nHighlightPhases==2) clPhase ^= clr_highlight;

					if(	count==0 ||
						m_nHighlightOversectMethod==0 ||
						( m_nHighlightOversectMethod==1 && ((x+_y+rectPaint.left+rectPaint.top)&1)) )
					{
						pb[0] = pb[0] - (pb[0] - GetBValue(clPhase))*alpha2/256;
						pb[1] = pb[1] - (pb[1] - GetGValue(clPhase))*alpha2/256;
						pb[2] = pb[2] - (pb[2] - GetRValue(clPhase))*alpha2/256;				
					}
					else
					{
						pb[0] = bHighlightOversect;
						pb[1] = gHighlightOversect;
						pb[2] = rHighlightOversect;
					}
				}
			}
			else
			{	// подсветка цветом класса
				for( int x=rcFill.left; x<rcFill.right; x++,pb+=3,pcnt++ )
				{
					int x_img = int( ( pt_img_offset.x  + x ) / m_fZoom );

					if( x_img < 0 || x_img >= img_cx )
						continue;

					if( ! (pmask[x_img] & BodyMask) )
						continue;

					int count = *pcnt;
					*pcnt=count+1;
					int alpha2 = alpha*256 / (256 + alpha*count);

					if(	count==0 ||
						m_nHighlightOversectMethod==0 ||
						( m_nHighlightOversectMethod==1 && ((x+_y+rectPaint.left+rectPaint.top)&1)) )
					{
						pb[0] = pb[0] - (pb[0] - bMask)*alpha2/256;
						pb[1] = pb[1] - (pb[1] - gMask)*alpha2/256;
						pb[2] = pb[2] - (pb[2] - rMask)*alpha2/256;				
					}
					else
					{
						pb[0] = bHighlightOversect;
						pb[1] = gHighlightOversect;
						pb[2] = rHighlightOversect;
					}
				}
			}
		}			

	}
}
