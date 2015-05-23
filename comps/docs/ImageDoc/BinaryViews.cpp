// BinaryViews.cpp : implementation file
//

#include "stdafx.h"
#include "imagedoc.h"
#include "BinaryViews.h"
#include "math.h"
#include "menuconst.h"


IMPLEMENT_DYNCREATE(CBinaryForeView, CCmdTarget)
IMPLEMENT_DYNCREATE(CBinaryBackView, CCmdTarget)
IMPLEMENT_DYNCREATE(CBinaryView, CCmdTarget)
IMPLEMENT_DYNCREATE(CBinaryContourView, CCmdTarget)

// {04DBFA12-AC8F-48c6-BFCD-A0C44886C74A}
GUARD_IMPLEMENT_OLECREATE(CBinaryContourView, szBinaryContourViewProgID, 
0x4dbfa12, 0xac8f, 0x48c6, 0xbf, 0xcd, 0xa0, 0xc4, 0x48, 0x86, 0xc7, 0x4a);
// {3BB78FBD-DBF8-4107-94FD-46B5B52C60D6}
GUARD_IMPLEMENT_OLECREATE(CBinaryView, szBinaryViewProgID, 
0x3bb78fbd, 0xdbf8, 0x4107, 0x94, 0xfd, 0x46, 0xb5, 0xb5, 0x2c, 0x60, 0xd6);
// {8066EB87-6BF8-4df3-B1AF-B45E89049F4D}
GUARD_IMPLEMENT_OLECREATE(CBinaryBackView, szBinaryBackViewProgID, 
0x8066eb87, 0x6bf8, 0x4df3, 0xb1, 0xaf, 0xb4, 0x5e, 0x89, 0x4, 0x9f, 0x4d);
// {C3074ADE-E524-46a3-A2D2-0E1B8449CC08}
GUARD_IMPLEMENT_OLECREATE(CBinaryForeView, szBinaryForeViewProgID, 
0xc3074ade, 0xe524, 0x46a3, 0xa2, 0xd2, 0xe, 0x1b, 0x84, 0x49, 0xcc, 0x8);


/////////////////////////////////////////////////////////////////////////////
// CBinaryViewBase

CBinaryViewBase::CBinaryViewBase()
{
}

CBinaryViewBase::~CBinaryViewBase()
{
	POSITION pos = m_listBinaries.GetHeadPosition();
	while(pos)
		m_listBinaries.GetNext(pos)->Release();
	m_listBinaries.RemoveAll();
}

void CBinaryViewBase::DrawLayers(CDC* pdc, BITMAPINFOHEADER* pbmih, byte* pdibBits, CRect rectPaint, bool bUseScrollZoom)
{
	/*long nFillColor = ::GetValueInt( GetAppUnknown(), "Binary", "FillColor", 0);
  nFillColor = max(0, min(255, nFillColor));*/

	DWORD dwBackColor = ::GetValueColor( GetAppUnknown(), "Binary", "BackColor", RGB( 0, 0, 0 ) );
	
	POSITION pos = m_listBinaries.GetHeadPosition();
	while(pos)
		DrawBinaryRect(m_listBinaries.GetNext(pos), pbmih, pdibBits, rectPaint.TopLeft(), rectPaint, ::GetZoom(GetControllingUnknown()), m_pointScroll, 0, dwBackColor, bUseScrollZoom, m_dwShowFlags);
}

bool CBinaryViewBase::is_have_print_data()
{
	if( CImageSourceView::is_have_print_data() )
		return true;

	return ( m_listBinaries.GetHeadPosition() != 0 );
}

IUnknown* CBinaryViewBase::__GetObjectByPoint( CPoint point)
{
	/*
	IUnknown* punk = 0;
	POSITION posStart = m_posActive;
	if (!posStart)
		posStart = m_listBinaries.GetHeadPosition();

	if (!posStart)
		return 0;
	//skip one object
	m_listBinaries.GetNext(posStart)->Release();

	POSITION pos = posStart;
	while (pos)
	{
		POSITION	posSave = pos;
		CBinImageWrp	binary(m_listBinaries.GetNext(pos), false);
		CPoint ptOffset(0,0);
		binary->GetOffset(&ptOffset);
		CRect rc(ptOffset, CSize(binary.GetWidth(), binary.GetHeight()));
		if(rc.PtInRect(point))
		{
			m_posActive = posSave;
			punk = binary;
			if(punk)
				punk->AddRef();
			return punk;
		}
	}

	pos = m_listBinaries.GetHeadPosition();
	while (pos != posStart)
	{
		POSITION	posSave = pos;
		CBinImageWrp	binary(m_listBinaries.GetNext(pos), false);
		CPoint ptOffset(0,0);
		binary->GetOffset(&ptOffset);
		CRect rc(ptOffset, CSize(binary.GetWidth(), binary.GetHeight()));
		if(rc.PtInRect(point))
		{
			m_posActive = posSave;
			punk = binary;
			if(punk)
				punk->AddRef();
			return punk;
		}
	}
	
	return 0;
	*/
	if( !GetValueInt( GetAppUnknown(), "\\ImageView", "EnableBinarySelection", 0 ) )
		return 0;
	IUnknown* punk = 0;
	POSITION pos = m_listBinaries.GetHeadPosition();
	if(pos)
	{
		CBinImageWrp	binary(m_listBinaries.GetNext(pos));
		if(binary != 0)
		{
			CPoint ptOffset(0,0);
			binary->GetOffset(&ptOffset);
			CRect rc(ptOffset, CSize(binary.GetWidth(), binary.GetHeight()));
			if(rc.PtInRect(point))
			{
				binary->AddRef();
				return binary;
			}
		}
	}
	return 0;
}

void CBinaryViewBase::CalcClientSize()
{
	CImageSourceView::CalcClientSize();
	POSITION pos = m_listBinaries.GetHeadPosition();
	while(pos)
	{
		IUnknown *punkBin = m_listBinaries.GetNext(pos);
		IRectObjectPtr	ptrRect(punkBin);
		CRect	rect;
		ptrRect->GetRect(&rect);
		m_size_client.cx = max( m_size_client.cx, rect.right );
		m_size_client.cy = max( m_size_client.cy, rect.bottom );
	}
}


void CBinaryViewBase::_AttachObjects( IUnknown *punkContextFrom )
{
	POSITION pos = m_listBinaries.GetHeadPosition();
	while(pos)
	{
		m_listBinaries.GetNext(pos)->Release();
	}
	m_listBinaries.RemoveAll();

	bool bChangeList = punkContextFrom == GetControllingUnknown();
	CImageSourceView::_AttachObjects(punkContextFrom);

	//attach binary image
	IDataContext2Ptr sptrContext(punkContextFrom);
	if(sptrContext != 0)
	{
		LONG_PTR nPos = 0;
		sptrContext->GetFirstSelectedPos(_bstr_t(szTypeBinaryImage), &nPos);

		bool bFirstPass = true;

		while(nPos)
		{

#ifndef SupportLayers
			if(!bFirstPass)
				break;
#endif

			IUnknown* punkObject = 0;
			sptrContext->GetNextSelected(_bstr_t(szTypeBinaryImage), &nPos, &punkObject);

			if(punkObject)
			{
				m_listBinaries.AddTail(punkObject);
				//punkObject->Release();

				IRectObjectPtr	ptrRect( punkObject );
				CRect	rect;
				ptrRect->GetRect( &rect );

				m_size_client.cx = max( m_size_client.cx, rect.right );
				m_size_client.cy = max( m_size_client.cy, rect.bottom );
			}

			bFirstPass = false;
		}
	}
	if( bChangeList )
	{
		//if( m_binary!=0 )m_listObjects.AddTail( m_binary );
		POSITION pos = m_listBinaries.GetHeadPosition();
		while(pos)
		{
			m_listObjects.AddTail(m_listBinaries.GetNext(pos));
		}
	}

	_UpdateScrollSizes();
}


DWORD CBinaryViewBase::GetMatchType( const char *szType )
{
	if( !strcmp( szType, szTypeImage ) )
		return mvPartial;
	else
	if( !strcmp( szType, szDrawingObjectList ) )
		return mvPartial;
	else if( !strcmp( szType, szTypeBinaryImage) )
		return mvFull | mvFullWithBinary;
	else
		return mvNone;
}

void CBinaryViewBase::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if (!strcmp( pszEvent, szEventChangeObject ))
	{
		POSITION pos = m_listBinaries.GetHeadPosition();
		while(pos)
		{
			CBinImageWrp pbin = m_listBinaries.GetNext(pos);
			if( ::GetObjectKey( punkFrom ) == ::GetObjectKey( pbin ) )
				InvalidateRect(0);
		}
	}
	__super::OnNotify( pszEvent, punkHit, punkFrom, pdata, cbSize );
}



////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
//CBinaryForeView implementation
CBinaryForeView::CBinaryForeView()
{
	m_dwShowFlags = isfBinaryFore;
	m_sName = c_szCBinaryForeView;//"BinaryForeView";
	m_sUserName.LoadString(IDS_BINARYFOREVIEW_NAME);
}

/////////////////////////////////////////////////////////////////////////////////////////
const char* CBinaryForeView::GetViewMenuName()
{return szBinaryView;}

/////////////////////////////////////////////////////////////////////////////////////////
//CBinaryBackView implementation
CBinaryBackView::CBinaryBackView()
{
	m_dwShowFlags = isfBinaryBack;
	m_sName = c_szCBinaryBackView;
	m_sUserName.LoadString(IDS_BINARYBACKVIEW_NAME);
}

/////////////////////////////////////////////////////////////////////////////////////////
const char* CBinaryBackView::GetViewMenuName()
{return szBinaryView;}

/////////////////////////////////////////////////////////////////////////////////////////
//CBinaryView implementation
CBinaryView::CBinaryView()
{
	m_dwShowFlags = isfBinary;
	m_sName = c_szCBinaryView;
	m_sUserName.LoadString(IDS_BINARYVIEW_NAME);
}

/////////////////////////////////////////////////////////////////////////////////////////
const char* CBinaryView::GetViewMenuName()
{return szBinaryView;}

DWORD CBinaryView::GetMatchType( const char *szType )
{
	if( !strcmp( szType, szDrawingObjectList ) )
		return mvPartial;
	else if( !strcmp( szType, szTypeBinaryImage) )
		return mvFull | mvFullWithBinary;
	else
		return mvNone;
}


/////////////////////////////////////////////////////////////////////////////////////////
//CBinaryContourView implementation
CBinaryContourView::CBinaryContourView()
{
	m_dwShowFlags = isfBinaryContour;
	m_sName = c_szCBinaryContourView;
	m_sUserName.LoadString(IDS_BINARYCONTOURVIEW_NAME);
}

/////////////////////////////////////////////////////////////////////////////////////////
const char* CBinaryContourView::GetViewMenuName()
{return szBinaryView;}



