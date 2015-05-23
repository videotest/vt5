#ifndef __JPGcompressor_h__
#define __JPGcompressor_h__
/*
#ifdef _DEBUG
// {1723099F-D000-445e-8114-815F78A8B666}
static const GUID clsidJPGcompressor = 
{ 0x1723099f, 0xd000, 0x445e, { 0x81, 0x14, 0x81, 0x5f, 0x78, 0xa8, 0xb6, 0x66 } };

#define pszJPGcompressorProgID "FileFilters2.JPGcompressorD"
#else
*/
// {A44C22D8-4869-4f24-8056-52E5E5EFE01D}
static const GUID clsidJPGcompressor = 
{ 0xa44c22d8, 0x4869, 0x4f24, { 0x80, 0x56, 0x52, 0xe5, 0xe5, 0xef, 0xe0, 0x1d } };

#define pszJPGcompressorProgID "FileFilters2.JPGcompressor"
/*
#endif
*/

#include "compressint.h"

class CJPGcompressor : public ComAggregableBase,
					  public ICompressionManager2
{
	route_unknown();
public:
	CJPGcompressor();
	~CJPGcompressor();

	virtual IUnknown *DoGetInterface( const IID &iid );

protected:
	com_call Compress( IUnknown *punkImage, ISequentialStream *pStream );
	com_call DeCompress( IUnknown *punkImage, ISequentialStream *pStream );
	com_call SetOption( BSTR bstrVal, long lVal );
	com_call GetOption( BSTR bstrVal, long *plVal );
	com_call WriteDIB24ToFile( BITMAPINFOHEADER *pbmih, byte *pdib, BSTR bstrFileName );
	com_call Compress2( IUnknown *punkImage, ISequentialStream *pStream, long *plCompressVersion );
	com_call DeCompress2( IUnknown *punkImage, ISequentialStream *pStream, long lCompressVersion );
};


#endif