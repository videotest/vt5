#if !defined(AFX_THUMBNAILMANAGER_H__633328AF_4DE8_452C_B0A2_25738FA93461__INCLUDED_)
#define AFX_THUMBNAILMANAGER_H__633328AF_4DE8_452C_B0A2_25738FA93461__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ThumbnailManager.h : header file
//

#include "thumbnail.h"
/////////////////////////////////////////////////////////////////////////////
// CThumbnailManager class


class CThumbnailManager : public CCmdTargetEx
{
	DECLARE_DYNCREATE(CThumbnailManager);
	GUARD_DECLARE_OLECREATE(CThumbnailManager);
	DECLARE_INTERFACE_MAP();
protected:
	CThumbnailManager();
	virtual ~CThumbnailManager();

protected:
	BEGIN_INTERFACE_PART(ThumbnailManager, IThumbnailManager)
		com_call GetRenderObject( 
					IUnknown* punkDataObject, IUnknown** ppunkRenderObject );
		com_call ThumbnailDraw( BYTE* pbi, HDC hdc, RECT rcTarget, IUnknown* punkDataObject );
		com_call ThumbnailGetSize( BYTE* pbi, DWORD* pdwSize );
		com_call ThumbnailAllocate( BYTE** ppbi, SIZE sizeThumbnail );
		com_call ThumbnailFree( BYTE* pbi );
	END_INTERFACE_PART(ThumbnailManager)

	CCompManager m_thumbnailManager;
	bool InitDefaultRender();

};


/////////////////////////////////////////////////////////////////////////////
// CImageRender class

class CImageRender : public CCmdTargetEx
{
	DECLARE_DYNCREATE(CImageRender);
	GUARD_DECLARE_OLECREATE(CImageRender);
	DECLARE_INTERFACE_MAP();
protected:
	CImageRender();
	virtual ~CImageRender();

protected:
	BEGIN_INTERFACE_PART(RenderObject, IRenderObject)
		com_call Support( IUnknown* punkDataObject, short* pnRenderSupport );
		com_call GenerateThumbnail( 
			/*[in]*/	IUnknown* punkDataObject,
			/*[in]*/	int	nBitsCount,
			/*[in]*/	DWORD dwFlag,
			/*[in]*/	LPARAM lParam,
			/*[in]*/	SIZE sizeThumbnail, 
			/*[out]*/	short* pnRenderSupport,
			/*[out]*/	BYTE** ppbi,
			/*[in]*/	IUnknown* punkBag
			);
	END_INTERFACE_PART(RenderObject)

	HRESULT DrawWMF(
			/*[in]*/	DWORD dwFlag,
			/*[in]*/	LPARAM lParam,
			/*[in]*/	SIZE sizeThumbnail, 
			/*[out]*/	short* pnRenderSupport,
			/*[out]*/	BYTE** ppbi );

	HRESULT DrawICO(
			/*[in]*/	IUnknown* punkDataObject,
			/*[in]*/	DWORD dwFlag,
			/*[in]*/	LPARAM lParam,
			/*[in]*/	SIZE sizeThumbnail, 
			/*[out]*/	short* pnRenderSupport,
			/*[out]*/	BYTE** ppbi );

};


/////////////////////////////////////////////////////////////////////////////
// CDefaultRender class

class CDefaultRender : public CCmdTargetEx
{
	DECLARE_DYNCREATE(CDefaultRender);
	GUARD_DECLARE_OLECREATE(CDefaultRender);
	DECLARE_INTERFACE_MAP();
protected:
	CDefaultRender();
	virtual ~CDefaultRender();

protected:
	BEGIN_INTERFACE_PART(RenderObject, IRenderObject)
		com_call Support( IUnknown* punkDataObject, short* pnRenderSupport );
		com_call GenerateThumbnail( 
			/*[in]*/	IUnknown* punkDataObject,
			/*[in]*/	int	nBitsCount,
			/*[in]*/	DWORD dwFlag,
			/*[in]*/	LPARAM lParam,
			/*[in]*/	SIZE sizeThumbnail, 
			/*[out]*/	short* pnRenderSupport,
			/*[out]*/	BYTE** ppbi,
			/*[in]*/	IUnknown* punkBag
		);
	END_INTERFACE_PART(RenderObject)

protected:
	// [vanek] : new icons for VT objects - 06.12.2004
	CImageContainer m_icons_overloaded;
	bool			m_binit_icons;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THUMBNAILMANAGER_H__633328AF_4DE8_452C_B0A2_25738FA93461__INCLUDED_)
