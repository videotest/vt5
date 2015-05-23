// ImagePane.cpp: implementation of the ImagePane class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"


#ifdef _COMMON_LIB
#include "Utils.h"
#else
#include "..\\common2\\misc_utils.h"
#include "..\\common2\\misc_utils.h"
#include "..\\common2\\com_main.h"
#endif

#include "misc_utils.h"

#include "Image5.h"
#include "ImagePane.h"
#include "core5.h"
#include "DocView5.h"




#ifdef _COMMON_LIB
#define FAILURE( val ) ASSERT( val )
#else
#define FAILURE( val ) dbg_assert( val )
#endif//_COMMON_LIB




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CImagePaneCache::CImagePaneCache( IUnknown* punkImage, bool binit, IUnknown *punkView )
{
	m_pCache = 0;
	m_nCacheCount = 0;

	m_punkimage = 0;
	
	if( binit )
		InitPaneCache( punkImage, punkView );	

}

//////////////////////////////////////////////////////////////////////
CImagePaneCache::~CImagePaneCache()
{
	if( m_pCache )
		delete m_pCache; m_pCache = 0;

	if( m_punkimage )
        m_punkimage ->Release(); m_punkimage  = 0;

}

//////////////////////////////////////////////////////////////////////
bool CImagePaneCache::InitPaneCache( IUnknown* punkImage, IUnknown *punkView )
{
	if( punkImage == 0 )
		return false;

	IImage3Ptr ptrImage( punkImage );
	if( ptrImage == 0 )
		return false;

	IUnknown* punkCC = 0;
	ptrImage->GetColorConvertor( &punkCC );
	if( !punkCC )
		return false;

	IColorConvertor2Ptr ptrCC( punkCC );
	punkCC->Release();	punkCC = 0;

	if( ptrCC == 0 )
		return false;

	int nCount = ::GetImagePaneCount( ptrImage );
	if( nCount < 1 )
		return false;

	if (m_pCache)
	{
		delete m_pCache;
		m_pCache = NULL;
	}
	m_nCacheCount = nCount;

	///
	if( m_punkimage )
		m_punkimage->Release();	m_punkimage = 0;

	m_punkimage = punkImage;
	punkImage->AddRef();
	///

	m_pCache = new bool[m_nCacheCount];

	BSTR bstr = 0;	
	ptrCC->GetCnvName( &bstr );
	bstr_t bstrConvName;
	if( bstr )
	{
		bstrConvName = bstr;
		::SysFreeString( bstr );	bstr = 0;
	}

	bstr_t bstrSection("\\");
	bstrSection += szImagePaneSection;
	bstrSection += "\\";
	bstrSection += bstrConvName;


	for( int i=0;i<m_nCacheCount;i++ )
		m_pCache[i] = true;
//	IColorConvertor6Ptr sptrCC6(ptrCC);
//	if (sptrCC6 != 0)
//		sptrCC6->GetUsedPanes(punkImage, punkView, m_pCache, m_nCacheCount);

	for( int i=0;i<m_nCacheCount;i++ )
	{
		bstr = 0;
		ptrCC->GetPaneName( i, &bstr );		
		bstr_t bstrPaneName;
		if( bstr )
		{
			bstrPaneName = bstr;
			::SysFreeString( bstr );	bstr = 0;
		}
		if (bstrPaneName.length() <= 0) // for FISH
		{
			char szBuff[50];
			sprintf(szBuff, "Pane%d", i);
			bstrPaneName = szBuff;
		}
		
		if (bstrPaneName.length() > 0)
		{
			long lEnable = ::GetValueInt( ::GetAppUnknown(), bstrSection, bstrPaneName, 1L );
			if( lEnable == 0L )
				m_pCache[i] = false;
		}

	}

	return true;
}



//////////////////////////////////////////////////////////////////////
bool CImagePaneCache::IsPaneEnable( int nPane )
{
	if( nPane < 0 || nPane >= m_nCacheCount || m_pCache == 0 )
	{
		FAILURE( false );
		return false;
	}

	return m_pCache[nPane];
}

//////////////////////////////////////////////////////////////////////
int CImagePaneCache::GetPanesCount()
{
	return m_nCacheCount;
}

//////////////////////////////////////////////////////////////////////
int	CImagePaneCache::GetEnablePanesCount()
{
	int nEnableCount = 0;
	if( !m_pCache )
	{
		FAILURE( false );
		return false; 
	}

	for( int i=0;i<m_nCacheCount;i++ )
		if( m_pCache[i] )
			nEnableCount++;

	return nEnableCount;
}

//////////////////////////////////////////////////////////////////////
// punk_image is source image if bsrc_image is true, 
// otherwise punk_image is destination image
bool	CImagePaneCache::CopyDisabledPanes( IUnknown* punk_image, bool bsrc_image )
{
    if( !punk_image || !m_punkimage )
		return false;

    IImage3Ptr sptr_dest;
	IImage3Ptr sptr_src;
	if( bsrc_image )
	{
        sptr_src = punk_image;
		sptr_dest = m_punkimage;       
	}
	else
	{
		sptr_src = m_punkimage;
		sptr_dest = punk_image;       
	}

	if( sptr_dest == 0 || sptr_src == 0 )
		return false;

    int ndest_panes = ::GetImagePaneCount( sptr_dest  );
	if( m_nCacheCount !=  ndest_panes )
		return false;	// разное количество пан

	int ncx_src  = 0, ncy_src = 0, ncx_dest = 0, ncy_dest = 0;

	sptr_dest->GetSize( &ncx_dest, &ncy_dest );
	sptr_src->GetSize( &ncx_src, &ncy_src );
	if( ncx_dest != ncx_src || ncy_dest != ncy_src )
		return false;	// разные размеры

	for( int npane = 0; npane < m_nCacheCount; npane ++ )
	{
		if( m_pCache[ npane ] )
			continue;

		for( int j = 0; j < ncy_dest; j ++ )
		{
			color *pdest_color = 0;
			color *psrc_color = 0;

			sptr_dest->GetRow( j, npane, &pdest_color );
			sptr_src->GetRow(  j, npane, &psrc_color );
		
			for( int i = 0; i < ncx_dest; i++ )
				pdest_color[i] = psrc_color[i];
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool	CImagePaneCache::GetImage( IUnknown **ppunk_image )
{
	if( !ppunk_image || !m_punkimage ) 
		return false;

	(*ppunk_image) = m_punkimage;
	(*ppunk_image)->AddRef( );
    return true;    
}

IUnknownPtr CImagePaneCache::ActiveView()
{
	IUnknownPtr punkView;
	IApplicationPtr sptrApp(GetAppUnknown());
	IUnknownPtr punkDoc;
	sptrApp->GetActiveDocument(&punkDoc);
	IDocumentSitePtr sptrDS(punkDoc);
	if (sptrDS) sptrDS->GetActiveView(&punkView);
	return punkView;
}

void CImagePaneCache::FreeImage()
{
	if (m_punkimage)
	{
		m_punkimage->Release();
		m_punkimage = NULL;
	}
}
