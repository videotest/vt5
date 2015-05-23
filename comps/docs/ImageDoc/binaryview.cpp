#include "stdafx.h"
#include "binaryview.h"
#include "resource.h"
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


void CImageViewBase::_AttachObjects(IUnknown* punkContextFrom)
{
	/*bool bChangeList = punkContextFrom == GetControllingUnknown();
	CImageSourceView::_AttachObjects(punkContextFrom);
	if((m_dwShowFlags & isfSourceImage) != isfSourceImage)
	{
		IUnknown* punkObject = ::GetActiveObjectFromContext( punkContextFrom, szTypeObjectList );

		if( ::GetObjectKey( m_objects ) != ::GetObjectKey( punkObject ) )
		{
			if( m_pframe )
				m_pframe->UnselectObjectsWithSpecifiedParent( (IUnknown*)m_objects );

			m_objects = punkObject;
			m_nActiveNumber = -1;
			if(GetSafeHwnd() )
				Invalidate();
		}

		if( punkObject )
			punkObject->Release();
	}

	if( bChangeList )
	{
		if( m_objects!=0 )m_listObjects.AddTail( m_objects );
	}*/
	
}

void CBinaryViewBase::_AttachObjects(IUnknown* punkContextFrom)
{
	/*bool bChangeList = punkContextFrom == GetControllingUnknown();
	CImageSourceView::_AttachObjects(punkContextFrom);

	//attach binary image
	IDataContext2Ptr sptrContext(punkContextFrom);
	if(sptrContext != 0)
	{
		m_binary = 0;
		POSITION pos = m_listBinaries.GetHeadPosition();
		while(pos)
		{
			m_listBinaries.GetNext(pos)->Release();
		}
		m_listBinaries.RemoveAll();
		long nPos = 0;
		sptrContext->GetFirstSelectedPos(_bstr_t(szTypeBinaryImage), &nPos);
		while(nPos)
		{
			IUnknown* punkObject = 0;
			sptrContext->GetNextSelected(_bstr_t(szTypeBinaryImage), &nPos, &punkObject);

			if(punkObject)
			{
				m_listBinaries.AddTail(punkObject);
				if(m_binary == 0)
					m_binary = punkObject;
				//punkObject->Release();
			}
		}
		if( IsWindow(GetSafeHwnd()) )
			Invalidate();
	}
	if( bChangeList )
	{
		//if( m_binary!=0 )m_listObjects.AddTail( m_binary );
		POSITION pos = m_listBinaries.GetHeadPosition();
		while(pos)
		{
			m_listObjects.AddTail(m_listBinaries.GetNext(pos));
		}
	}*/
}


////////////////////////////////////////////////////////////////////////////////////////
//CBinaryForeView implementation
CBinaryForeView::CBinaryForeView()
{
	m_dwShowFlags = isfBinaryFore;

	m_sName.LoadString(IDS_BINARYFOREVIEW_NAME);
}

/////////////////////////////////////////////////////////////////////////////////////////
const char* CBinaryForeView::GetViewMenuName()
{
	return szBinaryForeView;
}

/////////////////////////////////////////////////////////////////////////////////////////
//CBinaryBackView implementation
CBinaryBackView::CBinaryBackView()
{
	m_dwShowFlags = isfBinaryBack;

	m_sName.LoadString(IDS_BINARYBACKVIEW_NAME);
}

/////////////////////////////////////////////////////////////////////////////////////////
const char* CBinaryBackView::GetViewMenuName()
{
	return szBinaryBackView;
}

/////////////////////////////////////////////////////////////////////////////////////////
//CBinaryView implementation
CBinaryView::CBinaryView()
{
	m_dwShowFlags = isfBinary;

	m_sName.LoadString(IDS_BINARYVIEW_NAME);
}

/////////////////////////////////////////////////////////////////////////////////////////
const char* CBinaryView::GetViewMenuName()
{
	return szBinaryView;
}

/////////////////////////////////////////////////////////////////////////////////////////
//CBinaryContourView implementation
CBinaryContourView::CBinaryContourView()
{
	m_dwShowFlags = isfBinaryContour;

	m_sName.LoadString(IDS_BINARYCONTOURVIEW_NAME);
}

/////////////////////////////////////////////////////////////////////////////////////////
const char* CBinaryContourView::GetViewMenuName()
{
	return szBinaryContourView;
}



