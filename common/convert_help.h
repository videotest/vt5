#ifndef __convert_help_h__
#define __convert_help_h__

#include "image5.h"
#include "misc_utils.h"

//helper
class _convert
{
public:
	_convert();
	virtual ~_convert();
public:
	bool init( IImage2 *pimage );
	// [vanek] - 19.01.2004
	// bool init( IColorConvertor2	*pconvertor );

	COLORREF	get_rgb()				const	{return m_color;}
	color		get_color( int pane )	const	{return m_pcolors[pane];}
	int			get_panes()				const	{return m_panes;}
	bool		is_initialized()		const	{return m_convertor != 0;}
	void		set_rgb( COLORREF color );
	void		set_color( color c, int pane, bool recalc = false );
	bool		is_gray_pane( int idx );
protected:
	IColorConvertor5Ptr	m_convertor;
	_bstr_t				m_bstrConvertorName;
	color				*m_pcolors;
	int					m_panes;
	COLORREF			m_color;
};

bool _convert::is_gray_pane( int idx )
{
	if( m_convertor == 0 )return false;
	unsigned long flags = 0;
	m_convertor->GetPaneFlags( idx, &flags );
	return (flags & pfGray)!=0;
}

inline _convert::_convert()
{
	m_panes = 0;
	m_color = 0;
	m_pcolors = 0;
}

inline _convert::~_convert()
{
	delete m_pcolors;
}

inline bool _convert::init( IImage2 *pimage )
{
	IUnknown	*p = 0;
	pimage->GetColorConvertor( &p );
	IColorConvertor2Ptr	sptrConvertor( p );
	if( p )p->Release();

	// [vanek] - 19.01.2004
	if( m_pcolors )delete m_pcolors;
	m_pcolors = 0;
	m_convertor = sptrConvertor;

	if( m_convertor != 0 )
	{
		BSTR	bstr;
		m_convertor->GetCnvName( &bstr );
		m_bstrConvertorName = bstr;
		::SysFreeString( bstr );
	}

	m_panes = GetImagePaneCount( pimage );
	m_pcolors = new color[ m_panes ];

	return true;
}

inline void _convert::set_rgb( COLORREF rgb )
{
	if( !m_pcolors )
		return;

	byte	bgr[3];
	bgr[0]=GetBValue( rgb );
	bgr[1]=GetGValue( rgb );
	bgr[2]=GetRValue( rgb );

	int	c;
	color	**ppcolor = new color*[m_panes];
	
	for( c = 0; c < m_panes; c++ )
	{
		ppcolor[c] = new color;
		*ppcolor[c] = m_pcolors[c];
	}

	m_convertor->ConvertDIBToImageEx( bgr, ppcolor, 1, true, m_panes );
	
	for( c = 0; c < m_panes; c++ )
	{
		m_pcolors[c] = *ppcolor[c];
		delete ppcolor[c];
	}

	delete ppcolor;

	m_color = rgb;
}


inline void _convert::set_color( color c, int pane, bool recalc )
{
	if( !m_pcolors )return;
	if( pane >= m_panes )return;
	m_pcolors[pane] = c;

	if( recalc )
	{
		int	c;
		byte	bgr[3];
		color	**ppcolor = new color*[m_panes];
		
		for( c = 0; c < m_panes; c++ )
		{
			ppcolor[c] = new color;
			*ppcolor[c] = m_pcolors[c];
		}
		m_convertor->ConvertImageToDIB( ppcolor, bgr, 1, true );
		
		for( c = 0; c < m_panes; c++ )
		{
			m_pcolors[c] = *ppcolor[c];
			delete ppcolor[c];
		}

		delete ppcolor;

		m_color = RGB( bgr[2], bgr[1], bgr[0] );
	}
}


#endif //__convert_help_h__
