
#include "stdafx.h"
#include "JPGFilter.h"
#include <setjmp.h>
#include <stdlib.h>
#include "nameconsts.h"
#include "resource.h"

#include "codec_common.inl"

#include "misc5.h"


CJPGFilter::CJPGFilter()
{
	m_bstrDocumentTemplate = _bstr_t("Image");
	char szFilterName[255];	szFilterName[0] = 0;
	::LoadString(App::handle(), IDS_JPG_FILTERNAME, szFilterName, 255);
	m_bstrFilterName = szFilterName;//_bstr_t("JPG files");
	m_bstrFilterExt = _bstr_t(".jpg\n.jpeg\n.jpe");
	AddDataObjectType(szArgumentTypeImage);
}

CJPGFilter::~CJPGFilter()
{
}

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  //(*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}



bool CJPGFilter::ReadFile( const char *pszFileName )
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	FILE * infile;		/* source file */
		
	if ((infile = fopen(pszFileName, "rb")) == NULL)
		return false;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) 
	{
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return false;
	}

	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, infile);
		
	
	IUnknown* punk = CreateNamedObject( GetDataObjectType(0) );
	// get object name from path and check it's exists
	//char*	strObjName = ::GetObjectNameFromPath(pszFileName);
	TCHAR szName[_MAX_PATH];
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_PATH];
	TCHAR ext[_MAX_PATH];
	_splitpath(pszFileName, drive, dir, szName, ext);
	_bstr_t bstrName(szName);
	long	NameExists = 0;
	if( m_sptrINamedData != 0 )
		m_sptrINamedData->NameExists(bstrName, &NameExists);
	// if new name not exists allready
	if (!NameExists && punk)
	{
		// set this name to object
		INamedObject2Ptr sptrObj(punk);
		sptrObj->SetName(bstrName);
	}
	if (!punk)
	{
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return false;
	}

	(void) jpeg_read_header(&cinfo, TRUE);
	bool bRet =false;
	StartNotification(cinfo.image_height);

	bRet = DeCompressImage(punk, cinfo, this);
	FinishNotification();

	punk->Release();

	m_nWidth = cinfo.image_width;
	m_nHeight = cinfo.image_height;
	m_nDepth = (cinfo.out_color_space == JCS_GRAYSCALE) ? 8 : 24;
	m_nColors = 1<<m_nDepth; //fake
	m_nNumPages = 1;
	::LoadString(App::handle(), IDS_JPEGCompression, (char*)m_strCompression, 255);

	
	jpeg_destroy_decompress(&cinfo);

	fclose(infile);

  	return bRet;
}

bool CJPGFilter::WriteFile( const char *pszFileName )
{
	IUnknown* punk = 0;
	
	// convert object type to BSTR
	_bstr_t bstrType( GetDataObjectType(0) );

	// get active object in which type == m_strObjKind
	
	if (!(m_bObjectExists && m_sptrAttachedObject != NULL) && m_sptrIDataContext != NULL)
	{
		m_sptrIDataContext->GetActiveObject( bstrType, &punk );

		INamedDataObject2Ptr	ptrN( punk );
		if( ptrN != 0 )
		{
			IUnknown	*punkP = 0;
			ptrN->GetParent( &punkP );

			if( punkP )
			{
				punk->Release();
				punk = punkP;
			}
		}
	}
	else
	{
		punk = (IUnknown*)m_sptrAttachedObject.GetInterfacePtr();
		punk->AddRef();
	}

	if( !punk )
		return false;

	IImagePtr	sptrI(punk);
	if(sptrI == 0)
		return false;

	struct jpeg_compress_struct cinfo;
	

	FILE * outfile;		/* target file */

	//kir SBT 2140 - preserve old file for case of errors
	char* pszTempFile=0;
	pszTempFile = tmpnam( NULL );


	jpeg_create_compress(&cinfo);
	
	if ((outfile = fopen(pszTempFile, "wb")) == NULL) 
	{
		jpeg_destroy_compress(&cinfo);
		return false;
	}

	struct my_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) 
	{
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
		jpeg_destroy_compress(&cinfo);
		fclose(outfile);
		remove(pszTempFile);
		return false;
	}

	jpeg_stdio_dest(&cinfo, outfile);

	int nCX = 0, nCY = 0;
	sptrI->GetSize(&nCX, &nCY);

	StartNotification(nCY);
	bool bRet = CompressImage(punk, cinfo, this);
	FinishNotification();

	punk->Release();

	fclose(outfile);
	
	if(!bRet)
	{
		remove(pszTempFile);
	}
	else
	{
		remove(pszFileName);
		rename(pszTempFile, pszFileName);
	}

	jpeg_destroy_compress(&cinfo);

	return bRet;
}

bool CJPGFilter::_InitNew()
{
	return true;
}

