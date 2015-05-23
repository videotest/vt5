#ifndef __compressint_h__
#define __compressint_h__

#define IMAGE_COMPRESS_SECTION		"ImageCompress"
#define IMAGE_COMPRESS_ENABLE_KEY	"EnableCompress"


interface ICompressionManager : public IUnknown
{
	com_call Compress( IUnknown *punkImage, ISequentialStream *pStream ) = 0;
	com_call DeCompress( IUnknown *punkImage, ISequentialStream *pStream ) = 0;
	com_call SetOption( BSTR bstrVal, long lVal ) = 0;
	com_call GetOption( BSTR bstrVal, long *plVal ) = 0;
	com_call WriteDIB24ToFile( BITMAPINFOHEADER *pbmih, byte *pdib, BSTR bstrFileName ) = 0;
};

interface ICompressionManager2 : public ICompressionManager
{
	com_call Compress2( IUnknown *punkImage, ISequentialStream *pStream, long *plCompressVersion ) = 0;
	com_call DeCompress2( IUnknown *punkImage, ISequentialStream *pStream, long lCompressVersion ) = 0;
};

declare_interface( ICompressionManager, "5993E10F-96F0-4eae-98CF-1906725A5133" )
declare_interface( ICompressionManager2, "1D6620EC-8998-4781-BE96-1185EB18AB80" )

#endif //__compressint_h__