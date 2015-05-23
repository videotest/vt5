#ifndef __oleclient_render_h__
#define __oleclient_render_h__


#include "thumbnail.h"

class COleRender : public ComObjectBase,
				public IRenderObject
{
	route_unknown();

public:
	COleRender();
	virtual ~COleRender();

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



#endif //__oleclient_render_h__
