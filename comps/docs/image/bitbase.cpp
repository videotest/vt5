#include "stdafx.h"
#include "bitbase.h"
#include "nameconsts.h"
#include "image5.h"
#include "binimageint.h"
// vk
#include "misc_utils.h"
// end vk

// A.M. BT4415
IImage4Ptr GetMask(IUnknown *punkDoc)
{
	IImage4Ptr sptrImg;
	IUnknownPtr punkImage(GetActiveObjectFromDocument(punkDoc, szArgumentTypeImage), false);
	INamedDataObject2Ptr sptrNDO2Img(punkImage);
	if (sptrNDO2Img == 0)
		return sptrImg;
	IUnknownPtr punkParent;
	sptrNDO2Img->GetParent(&punkParent);
	if (punkParent == 0)
		return sptrImg;
	sptrImg = punkImage;
	return sptrImg;
}

void CropBinaryByMask(IBinaryImage *pBin, IImage4 *pMask)
{
	POINT ptOffset;
	pMask->GetOffset(&ptOffset);
	int cxMask,cyMask;
	pMask->GetSize(&cxMask, &cyMask);
	int cxBin, cyBin;
	pBin->GetSizes(&cxBin,&cyBin);
	for (int y = 0; y < min(ptOffset.y,cyBin); y++)
	{
		byte *pBinRow;
		pBin->GetRow(&pBinRow, y, FALSE);
		for (int x = 0; x < cxBin; x++)
			pBinRow[x] = 0;
	}
	for (int y = ptOffset.y+cyMask; y < cyBin; y++)
	{
		byte *pBinRow;
		pBin->GetRow(&pBinRow, y, FALSE);
		for (int x = 0; x < cxBin; x++)
			pBinRow[x] = 0;
	}
	for (int y = ptOffset.y; y < min(ptOffset.y+cyMask,cyBin); y++)
	{
		byte *pBinRow;
		pBin->GetRow(&pBinRow, y, FALSE);
		byte *pMaskRow;
		pMask->GetRowMask(y-ptOffset.y, &pMaskRow);
		for (int x = 0; x < min(ptOffset.x,cxBin); x++)
			pBinRow[x] = 0;
		for (int x = ptOffset.x+cxMask; x < cxBin; x++)
			pBinRow[x] = 0;
		for (int x = ptOffset.x; x < min(ptOffset.x+cxMask,cxBin); x++)
		{
			if (pMaskRow[x-ptOffset.x] == 0)
				pBinRow[x] = 0;
		}
	}
}


CBitFilter::CBitFilter()
{
	m_nBitOperationType = botNot;
}


bool CBitFilter::InvokeFilter()
{
	IBinaryImagePtr	binaryDst = GetDataResult();
	IBinaryImagePtr	binarySrc1 = GetDataArgument("BinSource1");
	IBinaryImagePtr	binarySrc2 = GetDataArgument("BinSource2");
	if(binaryDst == 0 || binarySrc1 == 0 || (binarySrc2 == 0 && m_nBitOperationType != botNot))
		return false;

	IImage4Ptr sptrMask = GetMask(m_ptrTarget);

	int cx, cy, cx1, cy1, cx2, cy2;
	binarySrc1->GetSizes(&cx1, &cy1);
	if(m_nBitOperationType != botNot)
		binarySrc2->GetSizes(&cx2, &cy2);
	else
	{
		cx2 = cx1; 
		cy2 = cy1;
	}

	// vk
	cx = max(cx1, cx2);
	cy = max(cy1, cy2);
    
	//cx = min(cx1, cx2);
	//cy = min(cy1, cy2);
	//end vk
	if( binaryDst->CreateNew(cx, cy) != S_OK )
		return false;

	StartNotification(cy);
	// vk
	IUnknownPtr punk1( ::CreateTypedObject(_bstr_t(szTypeBinaryImage)), false );
	IBinaryImagePtr binTempSrc1 = punk1;
	IUnknownPtr punk2( ::CreateTypedObject(_bstr_t(szTypeBinaryImage)), false );
	IBinaryImagePtr binTempSrc2 = punk2;
	if ( (binTempSrc1->CreateNew(cx, cy) != S_OK) || (binTempSrc2->CreateNew(cx, cy) != S_OK) )
		return false;
	byte *rowTempSrc1 = 0, *rowTempSrc2 = 0;
	long x = 0, y = 0;
	// end vk
	byte *pRowSrc1 = 0, *pRowSrc2 = 0, *pRowDst = 0;

	// vk
	// Создаем копии исходных изображений нужного размера  	
	for( y = 0; y < cy1; y++ )
	{
		binarySrc1->GetRow( &pRowSrc1, y, FALSE );
		binTempSrc1->GetRow( &rowTempSrc1, y, FALSE );
		for( x = 0; x < cx1; x++ )
			rowTempSrc1[x] = pRowSrc1[x];
		for ( x = cx1; x < cx; x++)
			rowTempSrc1[x] = 0;
	}
	for( y = cy1; y < cy; y++ )
	{
		binTempSrc1->GetRow( &rowTempSrc1, y, FALSE );
		for( x = 0; x < cx; x++ )
			rowTempSrc1[x] = 0;
	}


	if ( m_nBitOperationType != botNot )
	{
		for( y = 0; y < cy2; y++ )
		{
			binarySrc2->GetRow( &pRowSrc2, y, FALSE );
			binTempSrc2->GetRow( &rowTempSrc2, y, FALSE );
			for( x = 0; x < cx2; x++ )
				rowTempSrc2[x] = pRowSrc2[x];
			for ( x = cx2; x < cx; x++)
				rowTempSrc2[x] = 0;
		}
		for( y = cy2; y < cy; y++ )
		{
			binTempSrc2->GetRow( &rowTempSrc2, y, FALSE );
			for( x = 0; x < cx; x++ )
				rowTempSrc2[x] = 0;
		}
	}
	// end vk
	for(long y = 0; y < cy; y++)
	{
		binaryDst->GetRow(&pRowDst, y, FALSE);
		// vk
		binTempSrc1->GetRow(&rowTempSrc1, y, FALSE);
		//binarySrc1->GetRow(&pRowSrc1, y, FALSE);
		// end vk
		if(m_nBitOperationType != botNot)
			// vk
			binTempSrc2->GetRow(&rowTempSrc2, y, FALSE);
			//binarySrc2->GetRow(&pRowSrc2, y, FALSE);
			// end vk
		
		// vk
		if ( m_nBitOperationType == botNot )
		{
			long j = 0;
			for( j = 0; j < cx; j++ )
			{
				if ( rowTempSrc1[j] < 127) 
					pRowDst[j] = 255;
				else
					pRowDst[j] = 0;
			}
		}
		// end vk

		for(long x = cx/4; x>0 ; x--)
		{
			DWORD dwSource2 = 0;
			if(m_nBitOperationType != botNot)
			{
				// vk
				dwSource2 = *((DWORD*)rowTempSrc2);
				rowTempSrc2 += 4;
				//dwSource2 = *((DWORD*)pRowSrc2);
				//pRowSrc2 += 4;
				// end vk
			}

			// vk
			DWORD dwSource1 = *((DWORD*)rowTempSrc1);
			rowTempSrc1 += 4;
			//DWORD dwSource1 = *((DWORD*)pRowSrc1);
			//pRowSrc1 += 4;
			// end vk
			
			switch(m_nBitOperationType)
			{
			// vk
			//case botNot:
			//	*((DWORD*)pRowDst) = ~dwSource1;
			//	break;
			// end vk
			case botOr:
				*((DWORD*)pRowDst) = dwSource1 | dwSource2;
				*((DWORD*)pRowDst) = dwSource1 | dwSource2;
				break;
			case botAnd:
				*((DWORD*)pRowDst) = dwSource1 & dwSource2;
				break;
			case botXor:
				*((DWORD*)pRowDst) = dwSource1 ^ dwSource2;
				break;
			}

			pRowDst += 4;
		}

		for(x = cx & 3; x>0; x--)
		{
			BYTE cSource2 = 0;
			if(m_nBitOperationType != botNot)
			{
				// vk
				cSource2 = *((BYTE*)rowTempSrc2);
				rowTempSrc2 += 1;
				//cSource2 = *((BYTE*)pRowSrc2);
				//pRowSrc2 += 1;
				// end vk
			}

			// vk
			BYTE cSource1 = *((BYTE*)rowTempSrc1);
			rowTempSrc1 += 1;
			//BYTE cSource1 = *((BYTE*)pRowSrc1);
			//pRowSrc1 += 1;
			// end vk
			
			switch(m_nBitOperationType)
			{
			// vk
			//case botNot:
			//	*((BYTE*)pRowDst) = ~cSource1;
			//	break;
			// end vk
			case botOr:
				*((BYTE*)pRowDst) = cSource1 | cSource2;
				break;
			case botAnd:
				*((BYTE*)pRowDst) = cSource1 & cSource2;
				break;
			case botXor:
				*((BYTE*)pRowDst) = cSource1 ^ cSource2;
				break;
			}

			pRowDst += 1;
		}

		Notify(y);
	}
	if (sptrMask != 0)
		CropBinaryByMask(binaryDst, sptrMask);

	FinishNotification();

	return true;
}
