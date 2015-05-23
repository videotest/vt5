#if !defined __thumbnail_h
#define __thumbnail_h

#define szThumbnailManagerProgID "ThumbnailManager.ThumbnailManager"

//Default Render Object

#define szDefaultRenderProgID4Registry "ThumbnailManager.DefaultRender"
#define szDefaultRenderProgID "ThumbnailManager.DefaultRender"

//Image Render Object

#define szImageRenderProgID4Registry "ThumbnailManager.ImageRender"
#define szImageRenderProgID "ThumbnailManager.ImageRender"

#define szPluginDefaultRender	"DefaultRender"


interface IThumbnailManager : public IUnknown
{
	com_call GetRenderObject( 
		IUnknown* punkDataObject, IUnknown** ppunkRenderObject ) = 0;
	
	com_call ThumbnailDraw( BYTE* pbi, HDC hdc, RECT rcTarget, IUnknown* punkDataObject ) = 0;
	com_call ThumbnailGetSize( BYTE* pbi, DWORD* pdwSize ) = 0;
	com_call ThumbnailAllocate( BYTE** ppbi, SIZE sizeThumbnail ) = 0;
	com_call ThumbnailFree( BYTE* pbi ) = 0;
};

interface IThumbnailSurface : public IUnknown
{
	com_call Draw( BYTE* pbi, HDC hdc, RECT rcTarget, SIZE sizeThumbnail, IUnknown* punkDataObject ) = 0;
};



// Query Application for ThumbnailManager, its aggregated in application.
// ThumbnailManager manage several render objects for generate thumbnail and one default
// render object( default - render object icon ) 
// 
// 
//	Working process:
// 
//	Application->ThumbnailManager->RenderObject->[Thumbnail Bits]
//
// 	BYTE *pbi = NULL;//Do Not Alloc memory for DibBits!!!
//	RenderObject->GenerateThumbnail( ..., &pbi, ) ( Alloc memory by using ThumbnailAllocate(common.dll))
//
//	//then use
// 	::ThumbnailDraw( pbi, ... )
//
//	.....
// 	//Do not use delete []pbi use next function( common.dll )
//	::ThumbnailFree( pbi );
//
//
//	 Serialization notes
//	 if you wanna load thumbnail:
//	 ThumbnailLoad( CArchive& ar, BYTE** ppbi ) (common.dll)
//	 this function allocate memory for thumbnail( ppbi )
//
//
//
//	Thumbnail structure
//
//	1.BITMAPINFOHEADER
//	2.TumbnailInfo
//	3.Dib Bits
//
//	If you wanna calc size of this memory block use
//	DWORD ThumbnailGetSize( BYTE* pbi ) (common.dll)
//
//
//

enum RenderSupport{
	rsNone = 0,	
	rsPartial = 1,	
	rsFull = 2,
};

enum TumbnailFlags
{		
	tfCanStretch2_1	= 1,	
	tfVarStretch	= 2,	

};

struct TumbnailInfo
{
	DWORD dwIcon;
	DWORD dwFlags;
	DWORD dwReserved;
};

		/*RENDERFLAG*/
#define RF_NODEFINE		0
#define RF_BACKGROUND	1

interface IRenderObject : public IUnknown
{													  /* RenderSupport enum */
	com_call Support( IUnknown* punkDataObject, short* pnRenderSupport ) = 0;
	com_call GenerateThumbnail( 
		/*[in]*/	IUnknown* punkDataObject,
		/*[in]*/	int	nBitsCount,
		/*[in]*/	DWORD dwFlag,
		/*[in]*/	LPARAM lParam,
		/*[in]*/	SIZE sizeThumbnail, 
		/*[out]*/	short*  pnRenderSupport,
		/*[out]*/	BYTE** ppbi,
		/*[in]*/	IUnknown* punkBag=0
		) = 0;
};




//szPluginThumbnail

declare_interface( IThumbnailManager, "898D1E8B-1E92-4c9b-BA90-2C8CBC86E32B" )
declare_interface( IRenderObject, "75273CA2-6F78-4a6e-A8A9-7193D34CA23C" )
declare_interface( IThumbnailSurface, "E011E1E1-E8E4-492b-A65E-F202FB5515DF" )


#endif //__thumbnail_h
