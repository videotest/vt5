#include "stdafx.h"
#include "oblistwrp.h"
#include "imagebase.h"
#include "contourapi.h"

/////////////////////////////////////////////////////////////////////////////////////////
//CObjectWrp class implementation
//the wrapper class for object
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
//unknown manipulation
CObjectWrp::CObjectWrp( IUnknown *punkObj, bool bAddRef )
{
	if( punkObj )
		Attach( punkObj, bAddRef );
}

void CObjectWrp::Attach( IUnknown *punkObj, bool bAddRef )
{
	m_sptrO = punkObj;
	m_sptrD = punkObj;
	IUnknownPtr::Attach( punkObj, bAddRef );
}

//get the image from the object
IUnknown *CObjectWrp::GetImage()
{
	IUnknown	*punkImage = 0;
	if(CheckInterface(m_sptrO, IID_IMeasureObject))
	{
		if( !CheckState() )
			return 0;
		m_sptrO->GetImage( &punkImage );
	}
	return punkImage;
}

//draw the object in the specified device context
void CObjectWrp::Draw( CDC &dc, IUnknown *punkVS )
{
	if( !CheckState() )
		return;

	//m_sptrD->Paint( dc );
	CImageWrp	image( GetImage(), false );

	if( image == 0 )
		return;
	double	fZoom = ::GetZoom( punkVS );
	CPoint	ptScroll= ::GetScrollPos( punkVS );

	//return the contours count
	for( int nPos = 0; nPos < image.GetContoursCount(); nPos++ )
	{
		Contour *p = image.GetContour( nPos );
		::ContourDraw( dc, p, fZoom, ptScroll, 0 );
	}
}

//setup the image to the object
void CObjectWrp::SetImage( IUnknown *punkImage )
{
	if( !CheckState() )
		return;
	m_sptrO->SetImage( punkImage );
}

//return true, if this point match object
bool CObjectWrp::PtInObject( CPoint point )
{
	CImageWrp	image( GetImage(), false );
	if( image == 0 )
		return false;

	CRect	rect = image.GetRect();
	if( !rect.PtInRect( point ) )
		return false;

	byte	*pbyte = image.GetRowMask( point.y-rect.top );
	if( (pbyte[point.x-rect.left] & 0x80) != 0x80)
		return false;

	return true;

}




/////////////////////////////////////////////////////////////////////////////////////////
//CObjectListWrp class implementation
//the wrapper class for objects list
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
//unknown manipulation
CObjectListWrp::CObjectListWrp( IUnknown *punkList, bool bAddRef )
{
	Attach( punkList, bAddRef );
}
void CObjectListWrp::Attach( IUnknown *punkList, bool bAddRef )
{
	m_sptrN = punkList;
	if(punkList)
		__super::Attach(punkList,bAddRef);
	else{
		if (GetInterfacePtr() != 0)
			__super::Release();
	}
}

void CObjectListWrp::Clear()
{
}

/////////////////////////////////////////////////////////////////////////////////////////
//draw the object list in given context
void CObjectListWrp::Draw( CDC &dc, IUnknown *punkVS )
{
	if( !CheckState() )return;

	POSITION	pos = GetFirstObjectPosition();

	while( pos )
	{
		CObjectWrp	object( GetNextObject( pos ), false );
		object.Draw( dc, punkVS );
	}

}

/////////////////////////////////////////////////////////////////////////////////////////
//operation with the object list
//add a new object
POSITION CObjectListWrp::Add( IUnknown *punkObject )
{
	if( !CheckState() )return 0;

	sptrINamedDataObject2	sptrN( punkObject );
	sptrN->SetParent( m_sptrN, 0 );

	return Find( punkObject );
}

//remove a specified object
void CObjectListWrp::Remove( IUnknown *punkObject )
{
	if( !CheckState() )return;

	sptrINamedDataObject2	sptrN( punkObject );
	sptrN->SetParent( 0, 0 );
}

//remove an object at the specified position
void CObjectListWrp::RemoveAt( POSITION posObject )
{
	if( !CheckState() )return;

	POSITION	pos = GetFirstObjectPosition();

	while( pos )
	{
		if( pos == posObject )
		{
			IUnknown	*punkObject = GetNextObject( pos );
			Remove( punkObject );
			punkObject->Release();

			return;
		}
		GetNextObject( pos )->Release();
	}

	ASSERT( false );
}

//locate the object in the list
POSITION CObjectListWrp::Find( IUnknown *punkObject )
{
	if (!CheckState())
		return 0;

	long lPos = 0;
	if (SUCCEEDED(m_sptrN->GetChildPos(punkObject, &lPos)) && lPos)
		return (POSITION)lPos;

	POSITION	pos = GetFirstObjectPosition(), posSave;
	GuidKey lKey = ::GetObjectKey( punkObject );

	while( pos )
	{
		posSave = pos;

		IUnknown	*punkObjectTest = GetNextObject( pos );
		GuidKey lTestKey = ::GetObjectKey( punkObjectTest );
		punkObjectTest->Release();

		if( lKey  == lTestKey )
			return posSave;
	}

	ASSERT( false );
	return 0;
}

//return the count of the object
long CObjectListWrp::GetCount()
{
	if( !CheckState() )return 0;

	long	nCount = 0;
	if (SUCCEEDED(m_sptrN->GetChildsCount(&nCount)))
		return nCount;

	POSITION	pos = GetFirstObjectPosition();

	while( pos )
	{
		GetNextObject( pos )->Release();
		nCount++;
	}
	return nCount;
}

//get the first object position
POSITION CObjectListWrp::GetFirstObjectPosition()
{
	if( !CheckState() )return 0;

	POSITION	pos = 0;
	m_sptrN->GetFirstChildPosition( (long*)&pos );

	return pos;
}

//get the object at the specified position and move the POS
IUnknown *CObjectListWrp::GetNextObject( POSITION &pos )
{
	if( !CheckState() )return 0;

	IUnknown	*punk = 0;
	m_sptrN->GetNextChild( (long*)&pos, &punk );

	return punk;
}

//get the last object position
POSITION CObjectListWrp::GetLastObjectPosition()
{
	if (!CheckState())
		return 0;

	POSITION pos = 0;
	m_sptrN->GetLastChildPosition((long*)&pos);

	return pos;
}

//get the object at the specified position and move the POS
IUnknown * CObjectListWrp::GetPrevObject(POSITION & pos)
{
	if (!CheckState())
		return 0;

	IUnknown * punk = 0;
	m_sptrN->GetPrevChild((long*)&pos, &punk);

	return punk;
}

//remove all objects in the object list
void CObjectListWrp::RemoveAll()
{
	if( !CheckState() )return;

	POSITION	pos = GetFirstObjectPosition();

	while( pos )
	{
		IUnknown	*punkObjectTest = GetNextObject( pos );
		Remove( punkObjectTest );
		punkObjectTest->Release();
	}
}


//get the active object position
POSITION	CObjectListWrp::GetCurPosition()
{
	if( !CheckState() )return 0;

	POSITION	pos;
	m_sptrN->GetActiveChild( (long*)&pos );
	return pos;

}
//set it!
void CObjectListWrp::SetCurPosition( POSITION pos )
{
	if( !CheckState() )
		return;

	m_sptrN->SetActiveChild( (long) pos );
}

