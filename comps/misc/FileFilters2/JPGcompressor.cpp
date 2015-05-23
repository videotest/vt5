#include "stdafx.h"
#include "JPGcompressor.h"
#include "jpeglib.h"
#include <setjmp.h>
#include "codec_common.inl"
#include "jerror.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
// error handling
////////////////////////////////////////////////////////////////////////////////////////////////////

struct my_error_mgr 
{
  struct jpeg_error_mgr pub;	/* "public" fields */
  jmp_buf setjmp_buffer;	/* for return to caller */
};
typedef struct my_error_mgr* my_error_ptr;
METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// destination manager
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
  struct jpeg_destination_mgr pub; /* public fields */
  ISequentialStream *pStream;		/* target stream */
  JOCTET * buffer;		/* start of buffer */
} my_destination_manager;

typedef my_destination_manager* my_destination_ptr;
#define OUT_BUFF_SIZE  4096	

/*
 * Initialize destination --- called by jpeg_start_compress
 * before any data is actually written.
 */
METHODDEF(void)
init_destination (j_compress_ptr cinfo)
{
  my_destination_ptr dest = (my_destination_ptr)cinfo->dest;

  /* Allocate the output buffer --- it will be released when done with image */
  dest->buffer = (JOCTET *)(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE, OUT_BUFF_SIZE*sizeof(JOCTET));

  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = OUT_BUFF_SIZE;
}

/*
 * Empty the output buffer --- called whenever buffer fills up.
 *
 * In typical applications, this should write the entire output buffer
 * (ignoring the current state of next_output_byte & free_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been dumped.
 *
 * In applications that need to be able to suspend compression due to output
 * overrun, a FALSE return indicates that the buffer cannot be emptied now.
 * In this situation, the compressor will return to its caller (possibly with
 * an indication that it has not accepted all the supplied scanlines).  The
 * application should resume compression after it has made more room in the
 * output buffer.  Note that there are substantial restrictions on the use of
 * suspension --- see the documentation.
 *
 * When suspending, the compressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_output_byte & free_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point will be regenerated after resumption, so do not
 * write it out when emptying the buffer externally.
 */
METHODDEF(boolean)
empty_output_buffer (j_compress_ptr cinfo)
{
  my_destination_ptr dest = (my_destination_ptr) cinfo->dest;

  ISequentialStream *pStream = dest->pStream;

  ULONG nCountWritten = 0;
  pStream->Write(dest->buffer, OUT_BUFF_SIZE, &nCountWritten);

  if(nCountWritten != OUT_BUFF_SIZE)
	ERREXIT(cinfo, JERR_FILE_WRITE);

  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = OUT_BUFF_SIZE;

  return TRUE;
}


/*
 * Terminate destination --- called by jpeg_finish_compress
 * after all data has been written.  Usually needs to flush buffer.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */
METHODDEF(void)
term_destination (j_compress_ptr cinfo)
{
  my_destination_ptr dest = (my_destination_ptr) cinfo->dest;
  size_t datacount = OUT_BUFF_SIZE - dest->pub.free_in_buffer;

  ISequentialStream *pStream = dest->pStream;

  /* Write any data remaining in the buffer */
  if (datacount > 0) 
  {
	ULONG nCountWritten = 0;
	pStream->Write(dest->buffer, datacount, &nCountWritten);

	if(nCountWritten != datacount)
		ERREXIT(cinfo, JERR_FILE_WRITE);
  }
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// source manager
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
  struct jpeg_source_mgr pub;	/* public fields */

  ISequentialStream *pStream;		/* source stream */
  JOCTET * buffer;		/* start of buffer */
  boolean start_of_file;	/* have we gotten any data yet? */
} my_source_manager;

typedef my_source_manager * my_source_ptr;
#define IN_BUFF_SIZE  4096	/* choose an efficiently fread'able size */


/*
 * Initialize source --- called by jpeg_read_header
 * before any data is actually read.
 */
METHODDEF(void)
init_source (j_decompress_ptr cinfo)
{
  my_source_ptr src = (my_source_ptr) cinfo->src;
  /* We reset the empty-input-file flag for each image,
   * but we don't clear the input buffer.
   * This is correct behavior for reading a series of images from one source.
   */
  src->start_of_file = TRUE;
}


/*
 * Fill the input buffer --- called whenever buffer is emptied.
 *
 * In typical applications, this should read fresh data into the buffer
 * (ignoring the current state of next_input_byte & bytes_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been reloaded.  It is not necessary to
 * fill the buffer entirely, only to obtain at least one more byte.
 *
 * There is no such thing as an EOF return.  If the end of the file has been
 * reached, the routine has a choice of ERREXIT() or inserting fake data into
 * the buffer.  In most cases, generating a warning message and inserting a
 * fake EOI marker is the best course of action --- this will allow the
 * decompressor to output however much of the image is there.  However,
 * the resulting error message is misleading if the real problem is an empty
 * input file, so we handle that case specially.
 *
 * In applications that need to be able to suspend compression due to input
 * not being available yet, a FALSE return indicates that no more data can be
 * obtained right now, but more may be forthcoming later.  In this situation,
 * the decompressor will return to its caller (with an indication of the
 * number of scanlines it has read, if any).  The application should resume
 * decompression after it has loaded more data into the input buffer.  Note
 * that there are substantial restrictions on the use of suspension --- see
 * the documentation.
 *
 * When suspending, the decompressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_input_byte & bytes_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point must be rescanned after resumption, so move it to
 * the front of the buffer rather than discarding it.
 */
METHODDEF(boolean)
fill_input_buffer (j_decompress_ptr cinfo)
{
  my_source_ptr src = (my_source_ptr) cinfo->src;
  
  ISequentialStream *pStream = src->pStream;
  ULONG nCountRead = 0;
  pStream->Read(src->buffer, IN_BUFF_SIZE, &nCountRead);
  
  if (nCountRead <= 0) 
  {
    if (src->start_of_file)	/* Treat empty input file as fatal error */
      ERREXIT(cinfo, JERR_INPUT_EMPTY);
    WARNMS(cinfo, JWRN_JPEG_EOF);
    /* Insert a fake EOI marker */
    src->buffer[0] = (JOCTET) 0xFF;
    src->buffer[1] = (JOCTET) JPEG_EOI;
    nCountRead = 2;
  }

  src->pub.next_input_byte = src->buffer;
  src->pub.bytes_in_buffer = nCountRead;
  src->start_of_file = FALSE;

  return TRUE;
}


/*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 *
 * Writers of suspendable-input applications must note that skip_input_data
 * is not granted the right to give a suspension return.  If the skip extends
 * beyond the data currently in the buffer, the buffer can be marked empty so
 * that the next read will cause a fill_input_buffer call that can suspend.
 * Arranging for additional bytes to be discarded before reloading the input
 * buffer is the application writer's problem.
 */

METHODDEF(void)
skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
  my_source_ptr src = (my_source_ptr) cinfo->src;

  /* Just a dumb implementation for now.  Could use fseek() except
   * it doesn't work on pipes.  Not clear that being smart is worth
   * any trouble anyway --- large skips are infrequent.
   */
  if (num_bytes > 0) {
    while (num_bytes > (long) src->pub.bytes_in_buffer) {
      num_bytes -= (long) src->pub.bytes_in_buffer;
      (void) fill_input_buffer(cinfo);
      /* note we assume that fill_input_buffer will never return FALSE,
       * so suspension need not be handled.
       */
    }
    src->pub.next_input_byte += (size_t) num_bytes;
    src->pub.bytes_in_buffer -= (size_t) num_bytes;
  }
}

/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */
METHODDEF(void)
term_source (j_decompress_ptr cinfo)
{
  /* no work necessary here */
}


static bool IsCGHImage(IUnknown *punk)
{
	IImagePtr sptrImage(punk);
	IUnknownPtr punkCC;
	sptrImage->GetColorConvertor(&punkCC);
	IColorConvertor2Ptr sptrCC(punkCC);
	BSTR bstr;
	sptrCC->GetCnvName(&bstr);
	_bstr_t bstrName(bstr);
	return bstrName == _bstr_t("CGH");
}

static int GetPanesCount(IUnknown *punk)
{
	return ::GetImagePaneCount( punk );
}

IUnknownPtr GetPaneFromCGH(IUnknown *punkImageCGH, int nPane)
{
	IImage3Ptr sptrImgCGH(punkImageCGH);
	if (sptrImgCGH == 0) return IUnknownPtr();
	int nHeight,nWidth;
	sptrImgCGH->GetSize(&nWidth,&nHeight);
	IUnknownPtr punkImgPane(CreateTypedObject(_bstr_t("Image")),false);
	IImage3Ptr sptrImgPane(punkImgPane);
	if (sptrImgPane == 0) return IUnknownPtr();
	sptrImgPane->CreateImage(nWidth, nHeight, _bstr_t("Gray"),-1);
	for (int y = 0; y < nHeight; y++)
	{
		color *pRowSrc,*pRowDst;
//		byte *pRowSrcMask,*pRowDstMask;
		sptrImgPane->GetRow(y, 0, &pRowDst);
		sptrImgCGH->GetRow(y, nPane, &pRowSrc);
		memcpy(pRowDst, pRowSrc, nWidth*sizeof(color));
//		sptrImgPane->GetRowMask(y, &pRowDstMask);
//		sptrImgCGH->GetRowMask(y, &pRowSrcMask);
//		memcpy(pRowDstMask, pRowSrcMask, nWidth);
	}
	return sptrImgPane;
}

bool MakeCGHPane(IUnknown *punkImgCGH, IUnknown *punkImgPane, int iPane)
{
	IImage3Ptr sptrImgCGH(punkImgCGH);
	if (sptrImgCGH == 0) return false;
	IImage3Ptr sptrImgPane(punkImgPane);
	int nHeight,nWidth;
	sptrImgPane->GetSize(&nWidth,&nHeight);
	if (iPane == 0)
		sptrImgCGH->CreateImage(nWidth, nHeight, _bstr_t("CGH"),-1);
	for (int y = 0; y < nHeight; y++)
	{
		color *pRowSrc,*pRowDst;
//		byte *pRowSrcMask,*pRowDstMask;
		sptrImgPane->GetRow(y, 0, &pRowSrc);
		sptrImgCGH->GetRow(y, iPane, &pRowDst);
		
		if( pRowDst )
			memcpy(pRowDst, pRowSrc, nWidth*sizeof(color));
		else
 		{
			return false;
		}
/*		if (iPane == 0)
		{
			sptrImgPane->GetRowMask(y, &pRowSrcMask);
			sptrImgCGH->GetRowMask(y, &pRowDstMask);
			memcpy(pRowDstMask, pRowSrcMask, nWidth);
		}*/
	}
	return true;
}


CJPGcompressor::CJPGcompressor()
{
}

CJPGcompressor::~CJPGcompressor()
{
}

IUnknown* CJPGcompressor::DoGetInterface( const IID &iid )
{
	if(iid == IID_ICompressionManager)		return (ICompressionManager*)this;
	else if(iid == IID_ICompressionManager2)		return (ICompressionManager2*)this;
	else return ComObjectBase::DoGetInterface( iid );
}

class file_stream : public ISequentialStream
{
public:
	dummy_unknown()
	file_stream( const char	*psz_filename, bool fRead )
	{		pfile = fopen( psz_filename, fRead ? "rb" : "wb" );}
	~file_stream()
	{		::fclose( pfile );}

	com_call Read( void *pv, ULONG cb, ULONG *pcbRead )
	{	
		*pcbRead = fread( pv, 1, cb, pfile );	
		return (ferror( pfile ) == 0) ? S_OK:E_FAIL;}

	com_call Write( const void *pv, ULONG cb, ULONG *pcbWritten )
	{	
		*pcbWritten = ::fwrite( pv, 1, cb, pfile );	
		return (ferror( pfile ) == 0) ? S_OK:E_FAIL;}
protected:
	FILE	*pfile;
};

HRESULT CJPGcompressor::WriteDIB24ToFile( BITMAPINFOHEADER *pbmih, byte *pdib, BSTR bstrFileName )
{
	struct jpeg_compress_struct cinfo;
	jpeg_create_compress(&cinfo);
	struct my_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) 
	{
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
		jpeg_destroy_compress(&cinfo);
		return E_FAIL;
	}

	file_stream	stream( _bstr_t(bstrFileName), false );

	//set destination
	cinfo.dest = (struct jpeg_destination_mgr *)(*cinfo.mem->alloc_small) ((j_common_ptr) &cinfo, JPOOL_PERMANENT, sizeof(my_destination_manager));
  	my_destination_ptr dest = (my_destination_ptr)cinfo.dest;
	dest->pub.init_destination = init_destination;
	dest->pub.empty_output_buffer = empty_output_buffer;
	dest->pub.term_destination = term_destination;
	dest->pStream = &stream;

	bool	fRes = true;

	{
		JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */

		try
		{
			cinfo.in_color_space = JCS_RGB;
	
			cinfo.input_components = 3;		/* # of color components per pixel */
			
			cinfo.image_width = pbmih->biWidth; 	/* image width and height, in pixels */
			cinfo.image_height = pbmih->biHeight;
			jpeg_set_defaults(&cinfo);

			for (long ci = 0; ci < MAX_COMPONENTS; ci++) 
			{
				cinfo.comp_info[ci].h_samp_factor = 1;
				cinfo.comp_info[ci].v_samp_factor = 1;
			}

			cinfo.smoothing_factor = 0;
			cinfo.dct_method = JDCT_ISLOW;
			jpeg_set_quality(&cinfo, ::GetValueInt(GetAppUnknown(), "\\FileFilters2", "JPGCompressionLevel", 100 ), FALSE);


			jpeg_start_compress(&cinfo, TRUE);

			int	cx4 = (pbmih->biWidth*3+3)/4*4;

			for(long y = 0; y < pbmih->biHeight; y++)
			{
				row_pointer[0] = pdib+(pbmih->biHeight-y-1)*cx4;
				// A.M. BT2753 - jpeg_write_scanlines accepts RGB, now really BGR
				for (long x = 0; x < pbmih->biWidth; x++)
				{
					JSAMPLE b = row_pointer[0][3*x];
					row_pointer[0][3*x] = row_pointer[0][3*x+2];
					row_pointer[0][3*x+2] = b;
				}
				jpeg_write_scanlines(&cinfo, row_pointer, 1);
				for (x = 0; x < pbmih->biWidth; x++)
				{
					JSAMPLE b = row_pointer[0][3*x];
					row_pointer[0][3*x] = row_pointer[0][3*x+2];
					row_pointer[0][3*x+2] = b;
				}
			}

			jpeg_finish_compress(&cinfo);
		}
		catch(TerminateLongException* e)
		{
			e->Delete();
			fRes = false;
		}
	}
		
	jpeg_destroy_compress(&cinfo);

	return fRes ? S_OK : S_FALSE;
}

HRESULT CJPGcompressor::Compress2( IUnknown *punkImage, ISequentialStream *pStream, long *plCompressVersion )
{
	IImage3Ptr sptr(punkImage);
	byte *pMask;
	sptr->GetRowMask(0, &pMask);


	
	if(!punkImage || !pStream || !plCompressVersion)
		return E_INVALIDARG;

	*plCompressVersion = 2;
	struct jpeg_compress_struct cinfo;
	jpeg_create_compress(&cinfo);
	struct my_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) 
	{
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
		jpeg_destroy_compress(&cinfo);
		return false;
	}

	//set destination
	cinfo.dest = (struct jpeg_destination_mgr *)(*cinfo.mem->alloc_small) ((j_common_ptr) &cinfo, JPOOL_PERMANENT, sizeof(my_destination_manager));
  	my_destination_ptr dest = (my_destination_ptr)cinfo.dest;
	dest->pub.init_destination = init_destination;
	dest->pub.empty_output_buffer = empty_output_buffer;
	dest->pub.term_destination = term_destination;
	dest->pStream = pStream;
	bool bCGH = IsCGHImage(punkImage),bRet;
	BYTE byFlags = bCGH?1:0;
	ULONG cb;
	pStream->Write(&byFlags, 1, &cb);
	if (bCGH)
	{
		int nPanes = GetPanesCount(punkImage);
		BYTE byPanes = (BYTE)nPanes;
		pStream->Write(&byPanes, 1, &cb);
		for (int i = 0; i < nPanes; i++)
		{
			IUnknownPtr punkPane(GetPaneFromCGH(punkImage,i));
			if (!CompressImage(punkPane, cinfo))
				break;
		}
		bRet = i == nPanes;
	}
	else
		bRet = CompressImage(punkImage, cinfo);
		
	jpeg_destroy_compress(&cinfo);


	sptr->GetRowMask(0, &pMask);

	return bRet ? S_OK : S_FALSE;
}

HRESULT CJPGcompressor::Compress( IUnknown *punkImage, ISequentialStream *pStream )
{
	long l;
	return Compress2(punkImage, pStream, &l);
}


HRESULT CJPGcompressor::DeCompress2( IUnknown *punkImage, ISequentialStream *pStream, long lCompressVersion )
{
	if(!punkImage || !pStream)
		return E_INVALIDARG;

	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) 
	{
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
		jpeg_destroy_decompress(&cinfo);
		return false;
	}

	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);


	//set source
	my_source_ptr src;
	cinfo.src = (struct jpeg_source_mgr *)(*cinfo.mem->alloc_small) ((j_common_ptr)&cinfo, JPOOL_PERMANENT, sizeof(my_source_manager));
    src = (my_source_ptr)cinfo.src;
    src->buffer = (JOCTET *)(*cinfo.mem->alloc_small)((j_common_ptr)&cinfo, JPOOL_PERMANENT, IN_BUFF_SIZE*sizeof(JOCTET));
    src->pub.init_source = init_source;
	src->pub.fill_input_buffer = fill_input_buffer;
	src->pub.skip_input_data = skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	src->pub.term_source = term_source;
	src->pStream = pStream;
	src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL; /* until buffer loaded */

	BYTE byFlags = 0,byPanes;
	ULONG cb;
	bool bRet;
	if (lCompressVersion >= 2)
		pStream->Read(&byFlags, 1, &cb);
	if (byFlags&1) // CGH
	{
		pStream->Read(&byPanes, 1, &cb);
		for (int i = 0; i < byPanes; i++)
		{
			jpeg_read_header(&cinfo, TRUE);
			IUnknownPtr punkImgPane(::CreateTypedObject(_bstr_t("Image")),false);
			if (!DeCompressImage(punkImgPane, cinfo))
				break;
			if (!MakeCGHPane(punkImage, punkImgPane, i))
				break;
		}
		bRet = i == byPanes;
	}
	else
	{
		jpeg_read_header(&cinfo, TRUE);
		bRet = DeCompressImage(punkImage, cinfo);
	}
	jpeg_destroy_decompress(&cinfo);

	return bRet ? S_OK : S_FALSE;
}


HRESULT CJPGcompressor::DeCompress( IUnknown *punkImage, ISequentialStream *pStream )
{
	return DeCompress2(punkImage, pStream, 1);
}


HRESULT CJPGcompressor::SetOption( BSTR bstrVal, long lVal )
{
	return E_NOTIMPL;
}

HRESULT CJPGcompressor::GetOption( BSTR bstrVal, long *plVal )
{
	return E_NOTIMPL;
}