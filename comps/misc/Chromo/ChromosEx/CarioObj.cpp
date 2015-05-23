// CarioObj.cpp : implementation file
//

#include "stdafx.h"
#include "ChromosEx.h"
#include "CarioObj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "chromo_utils.h"
#include "drag_drop_defs.h"
#include "consts.h" 
/*
/////////////////////////////////////////////////////////////////////////////
// CCarioObj

IMPLEMENT_DYNCREATE(CCarioObj, CDataObjectBase)

/////////////////////////////////////////////////////////////////////////////
CCarioObj::CCarioObj()
{
	EnableAutomation();
	AfxOleLockApp();

	m_rectObject	= CRect( 0, 0, 400, 100 );
}

/////////////////////////////////////////////////////////////////////////////
CCarioObj::~CCarioObj()
{	
	AfxOleUnlockApp();
}

/////////////////////////////////////////////////////////////////////////////
void CCarioObj::OnFinalRelease()
{
	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CCarioObj, CCmdTarget)
	//{{AFX_MSG_MAP(CCarioObj)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CCarioObj, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CCarioObj)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ICarioObj to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.



// {5761789C-D91B-4EDF-8742-40AB3EDEBD3E}
static const IID IID_ICarioObj =
{ 0x5761789c, 0xd91b, 0x4edf, { 0x87, 0x42, 0x40, 0xab, 0x3e, 0xde, 0xbd, 0x3e } };

BEGIN_INTERFACE_MAP(CCarioObj, CDataObjectBase)
	INTERFACE_PART(CCarioObj, IID_ICarioObj, Dispatch)
	INTERFACE_PART(CCarioObj, IID_IDrawObject, Draw)
	INTERFACE_PART(CCarioObj, IID_IRectObject, Rect)
	INTERFACE_PART(CCarioObj, IID_ICarioObject, CarioObject)
	INTERFACE_PART(CCarioObj, IID_IMsgListener, MsgListener)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CCarioObj, Rect)
IMPLEMENT_UNKNOWN(CCarioObj, MsgListener)

// {108E66F4-697C-409C-8862-20699E8E2A63}
GUARD_IMPLEMENT_OLECREATE(CCarioObj, "ChromosEx.CarioObj", 
0x108e66f4, 0x697c, 0x409c, 0x88, 0x62, 0x20, 0x69, 0x9e, 0x8e, 0x2a, 0x63)

// {610E94E6-6CE1-4b21-8C4D-E432572E2F39}
static const GUID clsidCarioDataObject = 
{ 0x610e94e6, 0x6ce1, 0x4b21, { 0x8c, 0x4d, 0xe4, 0x32, 0x57, 0x2e, 0x2f, 0x39 } };
DATA_OBJECT_INFO_FULL(IDS_CARIO_TYPE, CCarioObj, szTypeObjectCario, szDrawingObjectList, clsidCarioDataObject, IDI_CARIO );

/////////////////////////////////////////////////////////////////////////////
DWORD CCarioObj::GetNamedObjectFlags()
{
	return nofHasData|nofStoreByParent;
}

/////////////////////////////////////////////////////////////////////////////
CarioMode CCarioObj::GetCarioMode()
{
	return cmBaseMent; 
}

/////////////////////////////////////////////////////////////////////////////
IUnknownPtr CCarioObj::GetDataContextPtr()
{
	return m_punkDrawTarget;
}

/////////////////////////////////////////////////////////////////////////////
IUnknownPtr CCarioObj::GetViewPtr()
{
	return m_punkDrawTarget;
}

/////////////////////////////////////////////////////////////////////////////
HWND CCarioObj::GetHwnd()
{
	CWnd* pwnd = ::GetWindowFromUnknown( GetViewPtr() );
	return ( pwnd ? pwnd->GetSafeHwnd() : 0 );
}
/////////////////////////////////////////////////////////////////////////////
CPoint CCarioObj::GetOffset()
{
	return m_rectObject.TopLeft();
}

/////////////////////////////////////////////////////////////////////////////
CSize CCarioObj::GetSize()
{
	return CSize( m_rectObject.Width(), m_rectObject.Height() );
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioObj::CanSetClientSize()
{
	return false;
}


/////////////////////////////////////////////////////////////////////////////
bool CCarioObj::process_message( MSG *pmsg, LRESULT *plReturn )
{
	
		if( pmsg->message >= WM_DND_FIRST && pmsg->message <= WM_DND_LAST )
		{
			if( pmsg->lParam != sizeof(drag_n_drop_data) )
				return false;

			if( !m_punkDrawTarget )
				return false;

			drag_n_drop_data* pdata = (drag_n_drop_data*)pmsg->wParam;
			if( !( pdata && pdata->punkMultiFrame ) )
				return false;

			IMultiSelectionPtr ptrMS( pdata->punkMultiFrame );
			if( ptrMS == 0 )
				return false;

			DWORD dwCount = 0;
			ptrMS->GetObjectsCount( &dwCount );
			if( !dwCount )
				return false;
			
			bool bFound = false;
			for( DWORD dwIdx=0;dwIdx<dwCount;dwIdx++ )
			{
				IUnknown* punkObj = 0;
				ptrMS->GetObjectByIdx( dwIdx, &punkObj );
				if( !punkObj )
					continue;
				
				IMeasureObjectPtr ptrMeasure( punkObj );
				punkObj->Release();	punkObj = 0;

				if( ptrMeasure )
				{
					bFound = true;
					break;
				}
			}

			if( !bFound )
				return false;


			CPoint ptCursor;
			::GetCursorPos( &ptCursor );
			::ScreenToClient( pdata->hwndOwner, &ptCursor );

			ptCursor = ::ConvertToClient( m_punkDrawTarget, ptCursor );

			if( pmsg->message == WM_DND_ON_DRAG_OVER )
			{
				CRect rcObject = m_rectObject;
				if( rcObject.PtInRect( ptCursor ) )
				{
					pdata->dwDropEfect = DROPEFFECT_COPY;

					*plReturn = 1L;
					return true;
				}
			}	
			else if( pmsg->message == WM_DND_ON_DROP )
			{
				CRect rcObject = m_rectObject;
				if( rcObject.PtInRect( ptCursor ) )
				{										
					*plReturn = ( add_objects_from_multi_selection( ptrMS, ptCursor ) ? 1L : 0L );
					return true;
				}
			}
		}		
	

	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioObj::SerializeObject( CStreamEx &stream, SerializeParams *pparams )
{
	if( !CDataObjectBase::SerializeObject( stream, pparams ) )
		return false;

	long	lVersion = 1;
	if( stream.IsLoading() )
	{
		stream >> lVersion;		
	}
	else
	{		
		stream << lVersion;		
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CCarioObj::DoDraw( CDC &dc )
{
	if( !( m_pbi || m_pdib ) )
		return ;

	if( !m_punkDrawTarget )
		return;

	CRect rcUpdate = m_rcInvalid;

	//conwert to window coordinates
	CRect rcObjectWnd = ConvertToWindow( m_punkDrawTarget, m_rectObject );

	CRect rcUpdateInObject;
	if( rcUpdateInObject.IntersectRect( &rcUpdate, &rcObjectWnd ) )
	{			
		CRect rcOld = rcUpdateInObject;
		rcUpdateInObject -= m_rcInvalid.TopLeft();
		DrawMirror( m_pbi, m_pdib, rcUpdateInObject, RGB(255, 255,128 ) );
	}

	//draw_basement_mode( &dc );
}


/////////////////////////////////////////////////////////////////////////////
//
//	IRectObject interface
//
/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioObj::XRect::SetRect(RECT rc)
{
	METHOD_PROLOGUE_EX(CCarioObj, Rect);
	
	pThis->m_rectObject = rc;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioObj::XRect::GetRect(RECT* prc)
{
	METHOD_PROLOGUE_EX(CCarioObj, Rect);

	if( !prc )
		return E_INVALIDARG;

	*prc = pThis->m_rectObject;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioObj::XRect::HitTest( POINT	point, long *plHitTest )
{
	METHOD_PROLOGUE_EX(CCarioObj, Rect);

	if( !plHitTest )
		return E_INVALIDARG;

	*plHitTest = pThis->m_rectObject.PtInRect( point );

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioObj::XRect::Move( POINT point )
{
	METHOD_PROLOGUE_EX(CCarioObj, Rect);

	CRect rc = pThis->m_rectObject;
	pThis->m_rectObject.left	= point.x;
	pThis->m_rectObject.top		= point.y;

	pThis->m_rectObject.right	= pThis->m_rectObject.left + rc.Width();
	pThis->m_rectObject.bottom	= pThis->m_rectObject.top + rc.Height();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
//	
//	IMsgListener interface
//	
/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioObj::XMsgListener::OnMessage( MSG *pmsg, LRESULT *plReturn )
{
	METHOD_PROLOGUE_EX(CCarioObj, MsgListener);

	if( !pmsg || !plReturn )
		return E_INVALIDARG;

	if( pThis->process_message( pmsg, plReturn ) )
		return S_FALSE;

	return S_OK;
}
*/