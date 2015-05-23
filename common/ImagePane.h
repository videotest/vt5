// ImagePane.h:
//
//////////////////////////////////////////////////////////////////////

#include "defs.h"

#ifndef __imagepane_h__
#define __imagepane_h__


#define	szImagePaneSection	"ImagePane"


#ifdef _COMMON_LIB
#define std_dll __declspec(dllexport)
#define FAILURE( val ) ASSERT( val )
#else
#undef std_dll
#define std_dll 
#define FAILURE( val ) dbg_assert( val )
#endif//_COMMON_LIB

class std_dll CImagePaneCache
{
public:
	static IUnknownPtr ActiveView();
	CImagePaneCache( IUnknown* punkImage, bool binit = true, IUnknown *punkView = NULL );
	virtual ~CImagePaneCache();

	bool	IsPaneEnable( int nPane );
	int		GetPanesCount();
	int		GetEnablePanesCount();
	bool	CopyDisabledPanes( IUnknown* punk_image, bool bsrc_image );
	bool	GetImage( IUnknown **ppunk_image );	// return with AddRef
	void    FreeImage();
protected:
	bool*		m_pCache;
	int			m_nCacheCount;
	IUnknown*	m_punkimage;

public:
	bool	InitPaneCache( IUnknown* punkImage, IUnknown *punkView );
};

#endif __imagepane_h__
