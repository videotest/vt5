#pragma once

#include "thumbnail.h"

class CSewRender : public ComObjectBase,
				public IRenderObject
{
	route_unknown();

public:
	CSewRender();
	virtual ~CSewRender();

public:
	virtual IUnknown *DoGetInterface( const IID &iid );

	com_call Support( IUnknown* punkDataObject, short* pnRenderSupport );
	com_call GenerateThumbnail( 
		/*[in]*/	IUnknown* punkDataObject,
		/*[in]*/	int	nBitsCount,
		/*[in]*/	DWORD dwFlag,
		/*[in]*/	LPARAM lParam,
		/*[in]*/	SIZE sizeThumbnail, 
		/*[out]*/	short*  pnRenderSupport,
		/*[out]*/	BYTE** ppbi,
		/*[in]*/	IUnknown* punkBag=0
		);

	void Draw( HDC hdc, RECT rcTarget, DWORD dwFlag, LPARAM lParam,
					bool bDrawIcon, IUnknown* punkDataObject,
					TumbnailFlags& tf,
					DWORD& dwIcon
					);
	


};
