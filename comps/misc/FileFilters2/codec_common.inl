#ifndef __codec_common_inl__
#define __codec_common_inl__

#include "jpeglib.h"
#include "image5.h"
#include "ansiapi.h"
#include "impl_long.h"
#include "resource.h"
#include "misc_utils.h"
#include "misc_templ.h"



inline bool CompressImage(IUnknown* punkImage, jpeg_compress_struct& cinfo, CLongOperationImpl* pLong = 0)
{
	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */

	try
	{
		IImage2Ptr	sptrI(punkImage);

		if(sptrI == 0)
			return false;

		
		int row_stride;
		IUnknown* punkCC = 0;
		sptrI->GetColorConvertor(&punkCC );
		IColorConvertor2Ptr sptrCC(punkCC);
		if(punkCC)
			punkCC->Release();

		
		char szCCName[255];

		if(sptrCC != 0)
		{
			BSTR bstrName = 0;
			sptrCC->GetCnvName(&bstrName);
			W2A(bstrName, szCCName, 255);
			::SysFreeString(bstrName);
		}

		int nNumPanes = 3;
		BYTE clrBack[3];
		if(!strcmp(szCCName, "RGB"))
		{
			cinfo.in_color_space = JCS_RGB;
			COLORREF clr = GetValueColor(GetAppUnknown(), "\\FileFilters2", "RGBBackground", RGB(128,128,128));
			clrBack[0] = GetRValue(clr);
			clrBack[1] = GetGValue(clr);
			clrBack[2] = GetBValue(clr);
		}
		/*else if(!strcmp(szCCName, "YUV"))
		{
			cinfo.in_color_space = JCS_YCbCr;
		}*/
		else if(!strcmp(szCCName, "GRAY"))
		{
			cinfo.in_color_space = JCS_GRAYSCALE;
			nNumPanes = 1;		/* # of color components per pixel */
			clrBack[0] = (BYTE)GetValueInt(GetAppUnknown(), "\\FileFilters2", "GrayBackground", 128);
		}
		else
		{
			char szError[255];
			::LoadString( app::handle(), IDS_CANT_SAVE_COLORSPACE, (char*)szError, 255);
			MessageBox(NULL, szError, "Error", MB_OK);
			return false;
		}

		cinfo.input_components = nNumPanes;		/* # of color components per pixel */
		
		int nCX = 0, nCY = 0;
		sptrI->GetSize(&nCX, &nCY);

		cinfo.image_width = nCX; 	/* image width and height, in pixels */
		cinfo.image_height = nCY;
		jpeg_set_defaults(&cinfo);

		for (long ci = 0; ci < MAX_COMPONENTS; ci++) 
		{
			cinfo.comp_info[ci].h_samp_factor = 1;
			cinfo.comp_info[ci].v_samp_factor = 1;
		}

		cinfo.smoothing_factor = 0;
		cinfo.dct_method = JDCT_ISLOW;
		jpeg_set_quality(&cinfo, GetValueInt(GetAppUnknown(), "\\FileFilters2", "JPGCompressionLevel", 100 ), FALSE);


		jpeg_start_compress(&cinfo, TRUE);

		color* pRow;
		byte *pRowMask;

		row_stride = nCX*nNumPanes;	/* JSAMPLEs per row in image_buffer */

		row_pointer[0] = new JSAMPLE[row_stride];

		for(long y = 0; y < nCY; y++)
		{
			for(int c = 0; c < nNumPanes; c++)
			{
				sptrI->GetRow(y, c, &pRow);
				sptrI->GetRowMask(y, &pRowMask);
				for(long x = 0; x < nCX; x++)
				{
					// AAM 23.04.2003 - убираем заполнение фоном за пределами маски
					//if (pRowMask[x])
					*(row_pointer[0] + x*nNumPanes + c) = ColorAsByte(pRow[x]);
					//else
					//	*(row_pointer[0] + x*nNumPanes + c) = clrBack[c];
				}
			}
			(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);

			if(pLong)
					pLong->Notify(y);
			
		}

		delete row_pointer[0];
		jpeg_finish_compress(&cinfo);
	}
	catch(TerminateLongException* e)
	{
		e->Delete();
		delete row_pointer[0];
		return false;
	}

	return true;
}

inline bool DeCompressImage(IUnknown* punkImage, jpeg_decompress_struct& cinfo, CLongOperationImpl* pLong = 0)
{
	try
	{
/*		if( cinfo.jpeg_color_space == JCS_GRAYSCALE ||
			cinfo.jpeg_color_space == JCS_RGB ||
			cinfo.jpeg_color_space == JCS_YCbCr )
			cinfo.out_color_space = cinfo.jpeg_color_space;*/

		(void) jpeg_start_decompress(&cinfo);

		

		IImagePtr	sptrI(punkImage);
		
		JSAMPARRAY buffer;		/* Output row buffer */
		int row_stride;		/* physical row width in output buffer */

		_bstr_t bstrColorSpace;
		int numPanes = cinfo.num_components;
		if(cinfo.out_color_space == JCS_YCbCr)
			bstrColorSpace = _bstr_t("YUV");
		else if(cinfo.out_color_space == JCS_RGB)
			bstrColorSpace = _bstr_t("RGB");
		else if(cinfo.out_color_space == JCS_GRAYSCALE)
			bstrColorSpace = _bstr_t("GRAY");
		else
		{
			char szError[255];
			::LoadString(app::handle(), IDS_CANT_LOAD_COLORSPACE, (char*)szError, 255);
			MessageBox(NULL, szError, "Error", MB_OK);
			return false;
		}
		
		long nCX = cinfo.image_width;
		long nCY = cinfo.image_height;
		sptrI->CreateImage(nCX, nCY, bstrColorSpace,-1);
				
		row_stride = cinfo.output_width * cinfo.num_components;

		buffer = (*cinfo.mem->alloc_sarray)	((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

		color* pRow = 0;
		for(long y = 0; y < nCY; y++)
		{	

			try//kir - SBT2140 if file corrupted...
			{
				(void) jpeg_read_scanlines(&cinfo, buffer, 1);
			}
			catch(...)
			{
				if(pLong) 
				{
					ILongOperationPtr ptrLong = pLong;
					if(ptrLong)
						ptrLong->Abort();
				}
				else throw;
			}
			for(long c = 0; c < numPanes; c++)
			{
				sptrI->GetRow(y, c, &pRow);
				for(long x = 0; x < nCX; x++)
				{
					pRow[x] = ByteAsColor(*(buffer[0] + x*numPanes + c));
				}
			}
			if(pLong)
				pLong->Notify(y);

		}
  
		(void) jpeg_finish_decompress(&cinfo);
	}
	catch(TerminateLongException* e)
	{
		e->Delete();
		return false;
	}
	catch(...)
	{
		return false;
	}

	return true;
}

#endif