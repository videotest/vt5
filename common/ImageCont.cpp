#include "stdafx.h"
#include "ImageCont.h"

//////////////////////////////////////////////////////////////////////////////////////////
//// class CImageContainer implementation

//////////////////////////////////////////////////////////////////////////////////////////
CImageContainer::CImageContainer()
{
	InitCommonControls();
}

//////////////////////////////////////////////////////////////////////////////////////////
CImageContainer::~CImageContainer()
{
    free();	
}

//////////////////////////////////////////////////////////////////////////////////////////
bool	CImageContainer::Load( LPCTSTR lpfile_name )
{
	if( !lpfile_name )
		return false;

	// 1. Clear
	free();

	int ngall_count = 0;

	// 2. Get count of galleries
	ngall_count = ::GetPrivateProfileInt( _T(GALLERIES_SECTION), _T(COUNT_KEY), 0, lpfile_name );
	if( !ngall_count )
		return false;

	// 3. Load galleries
	for( int nidx = 0; nidx < ngall_count; nidx ++ )
	{
        CString str_gall_key( _T("") );        
		str_gall_key.Format( _T(GALLERY_KEY_FORMAT_STRING), nidx );
        
		XContainerItem *pgallery = new XContainerItem;
		if( pgallery->Load( lpfile_name, str_gall_key ) )
			m_lstitems.add_tail( pgallery );
		else
		{
			delete pgallery;
			pgallery = 0;
		}        
	}

	if( !m_lstitems.count() )
	{
		free();
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool	CImageContainer::GetIcon( SIZE size_icon, LPCTSTR lpicon_name, HICON *phicon )
{
	if( !lpicon_name || !phicon )
		return false;

	for( long lpos = m_lstitems.head(); lpos; lpos = m_lstitems.next( lpos ) )
	{
		XContainerItem *pgallary = 0;
		pgallary = m_lstitems.get( lpos );
		if( !pgallary )
			break;
		
		SIZE size = {0};
		pgallary->GetIconSize( &size );
		if( size.cx != size_icon.cx || size.cy != size_icon.cy )
			continue;
        
		if( pgallary->GetIcon( lpicon_name, phicon ) )
			return true;        
	}
	    
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
long	CImageContainer::GetImageCount()
{
	long limg_count = 0;
	for( long lpos = m_lstitems.head(); lpos; lpos = m_lstitems.next( lpos ) )
	{
        XContainerItem* pitem = 0;
		pitem = m_lstitems.get( lpos );
		if( pitem )
			limg_count += pitem->GetImageCount();
	}

	return limg_count;
}

//////////////////////////////////////////////////////////////////////////////////////////
void	CImageContainer::GetMaxSize( SIZE *psize )
{
    if( !psize )
		return;

	psize->cx = psize->cy = 0;

    for( long lpos = m_lstitems.head(); lpos; lpos = m_lstitems.next( lpos ) )
	{
		XContainerItem* pitem = 0;
		pitem = m_lstitems.get( lpos );
		if( pitem )
		{
			SIZE size = {0};
			pitem->GetIconSize( &size );
			psize->cx = max( psize->cx, size.cx );
			psize->cy = max( psize->cy, size.cy );
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool	CImageContainer::GetImageInfoByIdx( long lidx, CString *pstr_name, SIZE *psize )
{
	long lcount_sum = 0;
    for( long lpos = m_lstitems.head(); lpos; lpos = m_lstitems.next( lpos ) )
	{
        XContainerItem* pitem = 0;
		pitem = m_lstitems.get( lpos );
		if( !pitem )
			continue;
		
        long lcurr_count = 0;
		lcurr_count = pitem->GetImageCount();
		lcount_sum += lcurr_count;
		if( lidx >= lcount_sum )
			continue;

        if( psize )		
			pitem->GetIconSize( psize );

		if( pstr_name )
		{
			if( !pitem->GetByIdx( lidx - lcount_sum + lcurr_count, pstr_name, 0 ) )
				break;
		}

        return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool	CImageContainer::GetIconByIdx( long lidx, HICON *phicon )
{
    if( !phicon )
		return false;

	long lcount_sum = 0;
    for( long lpos = m_lstitems.head(); lpos; lpos = m_lstitems.next( lpos ) )
	{
        XContainerItem* pitem = 0;
		pitem = m_lstitems.get( lpos );
		if( !pitem )
			continue;
		
        long lcurr_count = 0;
		lcurr_count = pitem->GetImageCount();
		lcount_sum += lcurr_count;
		if( lidx >= lcount_sum )
			continue;

        pitem->GetByIdx( lidx - lcount_sum + lcurr_count, 0, phicon );
        return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////////////////////
void	CImageContainer::free()
{
	m_lstitems.clear();
}


//////////////////////////////////////////////////////////////////////////////////////////
// nested class XContainerItem implementation
CImageContainer::XContainerItem::XContainerItem()
{
	m_himglst = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
CImageContainer::XContainerItem::~XContainerItem()
{
	free();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool	CImageContainer::XContainerItem::Load( LPCTSTR lpfile_name, LPCTSTR lpsection_name )
{
    if( !lpfile_name || !lpsection_name )    
		return false;

	// 1. Clear
	free();

    CString str_file_name( _T("") );    
	int nicon_width = 0,
		nicon_count = 0;
	COLORREF clr_transparent = CLR_NONE;
    
	// 2. Get file's name of icons
	::GetPrivateProfileString( lpsection_name, _T(FILENAME_KEY), str_file_name, 
		str_file_name.GetBuffer(1000), 1000, lpfile_name );
	str_file_name.ReleaseBuffer();
	if( str_file_name.IsEmpty() )
		return false;

	TCHAR str_disk[_MAX_DRIVE] = {0}, str_dir[_MAX_DIR] = {0};    
	_tsplitpath( str_file_name, str_disk, 0, 0, 0 );
	if( !_tcslen( str_disk ) )
	{
		TCHAR str_full_filename[_MAX_PATH] = {0};
		_tsplitpath( lpfile_name, str_disk, str_dir, 0, 0 );
		_tmakepath( str_full_filename, str_disk, str_dir, str_file_name, 0 );
        str_file_name = str_full_filename;
	}

	// 3. Get count of icons
	nicon_count = ::GetPrivateProfileInt( lpsection_name, _T(COUNT_KEY), 0, lpfile_name );
	if( !nicon_count )
		return false;


	// 4. Get icon's width
	nicon_width = ::GetPrivateProfileInt( lpsection_name, _T(ICONWIDTH_KEY), 0, lpfile_name );
	if( !nicon_width )
		return false;

    // 5. Get transparent color
	CString str_color( _T("") );
	::GetPrivateProfileString( lpsection_name, _T(TRANSPARENT_COLOR_KEY), str_color, 
		str_color.GetBuffer(50), 50, lpfile_name );
	str_color.ReleaseBuffer();
	if( !str_color.IsEmpty() )
	{
		int r = 0, 
			g = 0,
			b = 0;
        if( 3 == _stscanf( str_color, _T("(%d,%d,%d)"), &r, &g, &b ) )
			clr_transparent = RGB( r, g, b );
	}
    
	// 6. Load image list
	m_himglst = ImageList_LoadImage( NULL, str_file_name,  nicon_width, 0, clr_transparent, IMAGE_BITMAP, LR_LOADFROMFILE | LR_CREATEDIBSECTION );
	if( !m_himglst )
		return false;
	
    // 7. Load names of icons
    for( int nicon_idx = 0; nicon_idx < nicon_count; nicon_idx ++ )
	{
		CString str_key( _T("") ),
				str_icon_name( _T("") );

		str_key.Format( _T(ICON_KEY_FORMAT_STRING), nicon_idx );        
		::GetPrivateProfileString( lpsection_name, str_key, str_icon_name, 
				str_icon_name.GetBuffer(1000), 1000, lpfile_name );
		str_icon_name.ReleaseBuffer();

		if( !str_icon_name.IsEmpty() )
			m_mapname2idx.set( nicon_idx, str_icon_name );
	}

	if( !m_mapname2idx.count() )
	{
		free();
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool	CImageContainer::XContainerItem::GetIconSize( SIZE *psize )
{
	if( !psize || !m_himglst )
		return false;
    
	int cx = 0,
		cy = 0;

	if( !ImageList_GetIconSize( m_himglst, &cx, &cy ) )
		return false;

	psize->cx = cx;
	psize->cy = cy;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool	CImageContainer::XContainerItem::GetIcon( LPCTSTR lpicon_name, HICON *phicon )
{
	if( !lpicon_name || !phicon || !m_himglst )
		return false;

	long lpos = 0;
	lpos = m_mapname2idx.find( CString(lpicon_name) );
	if( !lpos )
		return false;

	int nicon_idx = 0;
	nicon_idx = m_mapname2idx.get( lpos );
	*phicon = ImageList_GetIcon( m_himglst, nicon_idx, ILD_NORMAL );
    return *phicon ? true : false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool	CImageContainer::XContainerItem::GetByIdx( long lidx, CString *pstr_name, HICON *phicon )
{
    if( (!pstr_name && !phicon) || lidx >= m_mapname2idx.count() )
		return false;

	for( long lpos = m_mapname2idx.head(), lcurr_idx = 0; lpos; lpos = m_mapname2idx.next( lpos ), lcurr_idx ++ )
	{
        if( lcurr_idx == lidx )
		{
			if( pstr_name )
				*pstr_name = m_mapname2idx.get_key( lpos );

			if( phicon )
				*phicon = ImageList_GetIcon( m_himglst, m_mapname2idx.get( lpos ), ILD_NORMAL );

			return true;
		}
	}

    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void	CImageContainer::XContainerItem::free()
{
	m_mapname2idx.clear();

	if( m_himglst )
		ImageList_Destroy( m_himglst ); m_himglst = 0;
}