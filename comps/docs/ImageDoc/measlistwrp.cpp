#include "stdafx.h"
#include "measlistwrp.h"
#include "\vt5\common2\class_utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CMeasObjectWrp::~CMeasObjectWrp()
{

}
CMeasObjectWrp::CMeasObjectWrp( IUnknown *punkObject )
{
	Attach( punkObject );
}

void CMeasObjectWrp::Attach( IUnknown *punkObject )
{
	m_ptrCalc = punkObject;
	m_ptrObject = punkObject;
	m_ptrMeas = punkObject;

	IUnknown	*punkImage = 0;
	if( m_ptrMeas!=0 )
	{
		m_nZOrder = 0;
		IMeasureObject2Ptr sptrM = m_ptrMeas;
		if( sptrM )
			sptrM->GetZOrder( &m_nZOrder );
		
		m_ptrMeas->GetImage( &punkImage );
	}

	m_image.Attach( punkImage, false );

	

	m_bTotalRectCalculated = false;
	m_rectTotal = NORECT;
	m_class = -2;
	m_nKey = ::GetObjectKey( punkObject );

	m_rectImage = NORECT;
	if( m_image != 0 )m_rectImage = m_image.GetRect();
	long	lpos = 0;
	if( m_ptrObject )m_ptrObject->GetFirstChildPosition( &lpos );
	m_bHasManualMeasurement = lpos != 0;
}

long CMeasObjectWrp::get_class()
{
	if( m_class == -2 )
		m_class = get_object_class( ICalcObjectPtr( m_ptrObject ) );
	return m_class;
}

CRect CMeasObjectWrp::GetTotalRect(IUnknown * punkView)
{
	if( m_bTotalRectCalculated )
		return /*punkView ? ::ConvertToWindow(punkView, m_rectTotal) : */m_rectTotal;


	m_bTotalRectCalculated = true;
	m_rectTotal = m_rectImage;


	if( m_rectImage != NORECT )
		return m_rectImage;
	if( m_bHasManualMeasurement )
	{
		long	lpos;
		bool	bFirst = true;
		m_ptrObject->GetFirstChildPosition( &lpos );

		while( lpos )
		{
			IUnknown	*punk = 0;
			m_ptrObject->GetNextChild( &lpos, &punk );
			if( !punk )continue;
			IDrawObjectPtr	ptrDraw( punk );
			punk->Release();

			CRect	rect;
			ptrDraw->GetRect( &rect );

			if( bFirst && m_rectImage.IsRectEmpty())
				m_rectTotal = rect;
			else
				m_rectTotal.UnionRect( m_rectTotal, rect );

			bFirst = false;
		}
	}
	return /*punkView ? ::ConvertToWindow(punkView, m_rectTotal) : */m_rectTotal;
}



///////////////////////////////////////////////////////////////////////////////////////
//
CMeasObjListWrp::CMeasObjListWrp( IUnknown *punk )
{
	Attach( punk );
}
CMeasObjListWrp::~CMeasObjListWrp()
{
	DeInit();
}

void CMeasObjListWrp::Attach( IUnknown *punk )
{
	DeInit();
	m_ptrObjectList = punk;
	CSupportComposite sc(punk);
	POSITION pos = GetFirstObjectPosition();
	while( pos )
	{
		IUnknown	*punk = GetNextObject( pos );
		_Cache( punk );
		punk->Release();
	}


}

void CMeasObjListWrp::DeInit()
{
	m_ptrObjectList = 0;
	POSITION pos = GetHeadPosition();
	while( pos )delete GetNext( pos );
	RemoveAll();
}

void CMeasObjListWrp::_Cache( IUnknown *punk )
{
	CMeasObjectWrp	*pnew = new CMeasObjectWrp( punk );
	// Insert in list using ZOrder
	POSITION posIns = NULL;
	POSITION pos = GetHeadPosition();
	while (pos)
	{
		POSITION posSaved = pos;
		CMeasObjectWrp *p = GetNext(pos);
		if (p->m_nZOrder > pnew->m_nZOrder)
		{
			posIns = InsertBefore(posSaved,pnew);
			break;
		}
	}
	if (posIns == NULL)
		posIns = AddTail(pnew);
	m_mapPtrToPos[punk] = posIns;
}

void CMeasObjListWrp::_sort_list()
{
	return;
	m_sorted_list.RemoveAll();

	long	lpos = 0;
	m_ptrObjectList->GetFirstChildPosition( &lpos );

	POSITION pos = GetHeadPosition();
	while( pos )
	{
		POSITION _min_pos = pos;

		CMeasObjectWrp	*pnew = (CMeasObjectWrp	*)GetNext( pos );

		long _long_min_pos = lpos;

		int nMinOrder = pnew->m_nZOrder;
		if( pnew )
		{
			POSITION pos2 = pos;
			while( pos2 )
			{
				POSITION _p = pos2;
				CMeasObjectWrp	*pnew2 = (CMeasObjectWrp	*)GetNext( pos2 );

				long	_lpos = lpos;

				{
					IUnknown	*punk = 0;
					m_ptrObjectList->GetNextChild( &lpos, &punk );
					punk->Release();
				}

				if( pnew2 )
				{
					if( pnew2->m_nZOrder < nMinOrder )
					{
						nMinOrder = pnew2->m_nZOrder;
						_min_pos = _p;
						_long_min_pos = _lpos;
					}
				}
			}
		}

		m_sorted_list.AddTail( _long_min_pos );
	}
}


void CMeasObjListWrp::_Update( IUnknown *punk )
{
	POSITION	pos = FindObject( punk );
	if(pos==0) return; //AAM: аварийный случай

	CMeasObjectWrp	*p = GetAt( pos );
	p->Attach( punk );
}

void CMeasObjListWrp::_UnCache( IUnknown *punk)
{
	POSITION	pos = FindObject( punk );

	CMeasObjectWrp	*p = GetAt( pos );
	RemoveAt( pos );
	delete p;

	m_mapPtrToPos.RemoveKey( punk );
}

POSITION	CMeasObjListWrp::GetCurPosition()
{
	if( m_ptrObjectList == 0 )return 0;

	long	lpos;
	m_ptrObjectList->GetActiveChild( &lpos );
	return (POSITION)lpos;
}

POSITION	CMeasObjListWrp::GetFirstObjectPosition()
{
	if( m_ptrObjectList == 0 )return 0;
	long	lpos = 0;
	m_ptrObjectList->GetFirstChildPosition( &lpos );
	return (POSITION)lpos;
//	return (POSITION)m_sorted_list.GetHeadPosition();
}

IUnknown	*CMeasObjListWrp::GetNextObject( POSITION &rPos )
{
	if( m_ptrObjectList == 0 )return 0;
	long	lpos = (long)rPos;
	IUnknown	*punk = 0;
	m_ptrObjectList->GetNextChild( &lpos, &punk );
	rPos = (POSITION)lpos;

//	long	lpos = m_sorted_list.GetNext( rPos );
//	IUnknown	*punk = 0;
//	m_ptrObjectList->GetNextChild( &lpos, &punk );

	return punk;
}

void CMeasObjListWrp::DoProcessNotify( const char *pszEvent, IUnknown *punkFrom, IUnknown *punkHit, long lHint )
{
	if( m_ptrObjectList == 0 )return;

	if( strcmp( pszEvent, szEventChangeObjectList ) )
		return;

	if( punkFrom != m_ptrObjectList )
		return;

	if( lHint == cncReset )
		Attach( punkFrom );
	else if( lHint == cncAdd )
		_Cache( punkHit );
	else if( lHint == cncRemove )
		_UnCache( punkHit );
	else if( lHint == cncChange )
		_Update( punkHit );

	if( lHint == cncReset || lHint == cncAdd || lHint == cncRemove || lHint == cncChange )
		_sort_list();
}

POSITION CMeasObjListWrp::FindObject( IUnknown *punkObject )
{
	if( !punkObject )return 0;

	POSITION	pos = 0;
	if( m_mapPtrToPos.Lookup( punkObject, pos ) )
		return pos;

	pos = GetHeadPosition();
	GuidKey lTestKey = ::GetObjectKey( punkObject );

	while( pos )
	{
		POSITION	posSave = pos;
		CMeasObjectWrp	*pw = GetNext( pos );

		if( GetObjectKey( pw->m_ptrObject ) == lTestKey )
		{
			return posSave;
		}
	}

	//ASSERT( false );
	return 0;
}