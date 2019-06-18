#ifndef __ccmacro_h__
#define __ccmacro_h__

//AAM 23.09.2002
//Несколько меняем концепцию пан: не всегда три паны,
//их может быть до 8; color converter должен выставить
//const unsigned uPanesUsed, где младший бит=1 - нужна пана 0 и т.д.;
//если не выставлять - берется значение по умолчанию 7
//для первых трех пан значения в R,G,B, для следующих - pan3, pan4, ...

const unsigned uPanesUsed=0x07;
#define IFPANE(nPane) if(uPanesUsed & (1<<nPane)) 

#define CONVERTTODIB_PART1																					\
	int		cxImg, cyImg;																					\
	pimage->GetSize( &cxImg, &cyImg );																		\
	if( (dwFlags & cidrUseImageOffset) == cidrUseImageOffset ) pimage->GetOffset( &pointImageOffest );\
	int		x, y, xImg, yImg, yBmp;																			\
	byte	fillR = GetRValue(dwFillColor);																	\
	byte	fillG = GetGValue(dwFillColor);																	\
	byte	fillB = GetBValue(dwFillColor);																	\
/*	byte	foreFillR[256];																	\
	byte	foreFillG[256];																	\
	byte	foreFillB[256];																	\
	{for(int i=0; i<256; i++) \
		{foreFillR[i]=fillR; foreFillG[i]=fillG; foreFillB[i]=fillB;} \
		foreFillR[254]=255; foreFillG[254]=0; foreFillB[254]=0;												\
	}*/																										\
	byte	*p, *pmask;																						\
	color	*pcolor0, *pcolor1, *pcolor2, *pcolor3, *pcolor4, *pcolor5, *pcolor6, *pcolor7, *pcolor8, *pcolor9; \
	color	*pcolor10, *pcolor11, *pcolor12, *pcolor13, *pcolor14, *pcolor15, *pcolor16, *pcolor17, *pcolor18, *pcolor19; \
	color	*pcolor20, *pcolor21, *pcolor22, *pcolor23, *pcolor24, *pcolor25, *pcolor26, *pcolor27, *pcolor28, *pcolor29; \
	color	*pcolor30, *pcolor31; \
	int		cx4Bmp = (pbi->biWidth*3+3)/4*4;																\
	int	nMaxX = min( rectDest.left+pbi->biWidth, rectDest.right );\
	int	nMaxY = min( rectDest.top+pbi->biHeight, rectDest.bottom );\
	int	nMinX = max( rectDest.left, 0 );\
	int	nMinY = max( rectDest.top, 0 );\
	int	xBmpStart = max( int(ceil(( pointImageOffest.x)*fZoom-ptScroll.x)), nMinX );				\
	int	xBmpEnd = min( int(floor((pointImageOffest.x+cxImg)*fZoom-ptScroll.x+.5)), nMaxX );		\
	int	yBmpStart = max( int(ceil((pointImageOffest.y)*fZoom-ptScroll.y)), nMinY );					\
	int	yBmpEnd = min( int(floor((pointImageOffest.y+cyImg)*fZoom-ptScroll.y+.5)), nMaxY );		\
	int	yImgOld = -1;																						\
	int	t, cx = max(xBmpEnd-xBmpStart, 0), cy = max(yBmpEnd-yBmpStart, 0);									\
	int	*pxofs = new int[cx];																				\
	int	*pyofs = new int[cy];																				\
																											\
	for( t = 0; t < xBmpEnd-xBmpStart; t++ )																\
		pxofs[t] = int((t+xBmpStart+ptScroll.x)/fZoom-pointImageOffest.x);									\
	for( t = 0; t < yBmpEnd-yBmpStart; t++ )																\
		pyofs[t] = int((t+yBmpStart+ptScroll.y)/fZoom-pointImageOffest.y);									\
																											\
	for( y = yBmpStart, yBmp = yBmpStart; y < yBmpEnd; y++, yBmp++ )										\
	{																										\
		/*get the pointer to the BGR struct*/																\
		p = pdibBits+(pbi->biHeight-yBmp-1+ptBmpOfs.y)*cx4Bmp+(xBmpStart-ptBmpOfs.x)*3;						\
		/*offset coordinates to image*/																		\
		yImg =pyofs[y-yBmpStart];																			\
		if( yImgOld != yImg )																				\
		{																									\
			IFPANE(0) pimage->GetRow( yImg, 0, &pcolor0 );													\
			IFPANE(1) pimage->GetRow( yImg, 1, &pcolor1 );													\
			IFPANE(2) pimage->GetRow( yImg, 2, &pcolor2 );													\
			IFPANE(3) pimage->GetRow( yImg, 3, &pcolor3 );													\
			IFPANE(4) pimage->GetRow( yImg, 4, &pcolor4 );													\
			IFPANE(5) pimage->GetRow( yImg, 5, &pcolor5 );													\
			IFPANE(6) pimage->GetRow( yImg, 6, &pcolor6 );													\
			IFPANE(7) pimage->GetRow( yImg, 7, &pcolor7 );													\
			IFPANE(8) pimage->GetRow( yImg, 8, &pcolor8 );													\
			IFPANE(9) pimage->GetRow( yImg, 9, &pcolor9 );													\
			IFPANE(10) pimage->GetRow( yImg, 10, &pcolor10 );													\
			IFPANE(11) pimage->GetRow( yImg, 11, &pcolor11 );													\
			IFPANE(12) pimage->GetRow( yImg, 12, &pcolor12 );													\
			IFPANE(13) pimage->GetRow( yImg, 13, &pcolor13 );													\
			IFPANE(14) pimage->GetRow( yImg, 14, &pcolor14 );													\
			IFPANE(15) pimage->GetRow( yImg, 15, &pcolor15 );													\
			IFPANE(16) pimage->GetRow( yImg, 16, &pcolor16 );													\
			IFPANE(17) pimage->GetRow( yImg, 17, &pcolor17 );													\
			IFPANE(18) pimage->GetRow( yImg, 18, &pcolor18 );													\
			IFPANE(19) pimage->GetRow( yImg, 19, &pcolor19 );													\
			IFPANE(20) pimage->GetRow( yImg, 20, &pcolor20 );													\
			IFPANE(21) pimage->GetRow( yImg, 21, &pcolor21 );													\
			IFPANE(22) pimage->GetRow( yImg, 22, &pcolor22 );													\
			IFPANE(23) pimage->GetRow( yImg, 23, &pcolor23 );													\
			IFPANE(24) pimage->GetRow( yImg, 24, &pcolor24 );													\
			IFPANE(25) pimage->GetRow( yImg, 25, &pcolor25 );													\
			IFPANE(26) pimage->GetRow( yImg, 26, &pcolor26 );													\
			IFPANE(27) pimage->GetRow( yImg, 27, &pcolor27 );													\
			IFPANE(28) pimage->GetRow( yImg, 28, &pcolor28 );													\
			IFPANE(29) pimage->GetRow( yImg, 29, &pcolor29 );													\
			IFPANE(30) pimage->GetRow( yImg, 30, &pcolor30 );													\
			IFPANE(31) pimage->GetRow( yImg, 31, &pcolor31 );													\
			pimage->GetRowMask( yImg, &pmask );																\
			yImgOld = yImg;																					\
		}																									\
		for( x = xBmpStart; x < xBmpEnd; x++ )																\
		{																									\
			/*get the x coord of source image*/																\
			xImg = pxofs[x-xBmpStart];																\
			if( ((pmask[xImg] & 0x80) == 0) && ((dwFlags & cidrIgnoreMask)==0) )																\
			{																								\
				if(dwFlags & cidrFillBackByColor)															\
				{																							\
					*p = fillB; p++;																		\
					*p = fillG; p++;																		\
					*p = fillR; p++;																		\
				}																							\
				else																						\
				{																							\
					p++;																					\
					p++;																					\
					p++;																					\
				}																							\
			}																								\
			else																							\
			{																								\
				if( dwFlags & cidrFillBodyByColor )															\
				{																							\
					int c=pmask[xImg]; \
					*p = fillB/*foreFillB[c]*/; p++;																		\
					*p = fillG/*foreFillG[c]*/; p++;																		\
					*p = fillR/*foreFillR[c]*/; p++;																		\
				}																							\
				else																						\
				{																							\
				color R = 0,G = 0,B = 0;																	\
				IFPANE(0) R = pcolor0[xImg]; \
				IFPANE(1) G = pcolor1[xImg];																	\
				IFPANE(2) B = pcolor2[xImg];																	\
				color pan3=0,pan4=0,pan5=0,pan6=0,pan7=0, pan8=0,pan9=0; \
				color pan10=0,pan11=0,pan12=0,pan13=0,pan14=0,pan15=0,pan16=0,pan17=0, pan18=0,pan19=0; \
				color pan20=0,pan21=0,pan22=0,pan23=0,pan24=0,pan25=0,pan26=0,pan27=0, pan28=0,pan29=0; \
				color pan30=0,pan31=0; \
				IFPANE(3) pan3=pcolor3[xImg]; \
				IFPANE(4) pan4=pcolor4[xImg]; \
				IFPANE(5) pan5=pcolor5[xImg]; \
				IFPANE(6) pan6=pcolor6[xImg]; \
				IFPANE(7) pan7=pcolor7[xImg]; \
				IFPANE(8) pan8=pcolor8[xImg]; \
				IFPANE(9) pan9=pcolor9[xImg]; \
				IFPANE(10) pan10=pcolor10[xImg]; \
				IFPANE(11) pan11=pcolor11[xImg]; \
				IFPANE(12) pan12=pcolor12[xImg]; \
				IFPANE(13) pan13=pcolor13[xImg]; \
				IFPANE(14) pan14=pcolor14[xImg]; \
				IFPANE(15) pan15=pcolor15[xImg]; \
				IFPANE(16) pan16=pcolor16[xImg]; \
				IFPANE(17) pan17=pcolor17[xImg]; \
				IFPANE(18) pan18=pcolor18[xImg]; \
				IFPANE(19) pan19=pcolor19[xImg]; \
				IFPANE(20) pan20=pcolor20[xImg]; \
				IFPANE(21) pan21=pcolor21[xImg]; \
				IFPANE(22) pan22=pcolor22[xImg]; \
				IFPANE(23) pan23=pcolor23[xImg]; \
				IFPANE(24) pan24=pcolor24[xImg]; \
				IFPANE(25) pan25=pcolor25[xImg]; \
				IFPANE(26) pan26=pcolor26[xImg]; \
				IFPANE(27) pan27=pcolor27[xImg]; \
				IFPANE(28) pan28=pcolor28[xImg]; \
				IFPANE(29) pan29=pcolor29[xImg]; \
				IFPANE(30) pan30=pcolor30[xImg]; \
				IFPANE(31) pan31=pcolor31[xImg]; \
				byte  Rbyte = 0;																			\
				byte  Gbyte = 0;																			\
				byte  Bbyte = 0;


#define CONVERTTODIB_PART2																					\
				*p = Bbyte; p++;																			\
				*p = Gbyte; p++;																			\
				*p = Rbyte; p++;																			\
				}																							\
			}																								\
		}																									\
	}																										\
	if(dwFlags & cidrFillBackByColor)																		\
	{																										\
		for ( y = yBmpEnd; y < nMaxY; y++)																	\
		{																									\
			p = pdibBits+(pbi->biHeight-y-1+ptBmpOfs.y)*cx4Bmp+(xBmpStart-ptBmpOfs.x)*3;					\
			for( x = xBmpStart; x < xBmpEnd; x++ )															\
			{																								\
				*p = fillB; p++;																			\
				*p = fillG; p++;																			\
				*p = fillR; p++;																			\
			}																								\
		}																									\
	}																										\
	delete pxofs;delete pyofs;


#define CONVERTTODIB_PART1_BILINEAR																			\
	int		cxImg, cyImg;																					\
	pimage->GetSize( &cxImg, &cyImg );																		\
	if( (dwFlags & cidrUseImageOffset) == cidrUseImageOffset ) pimage->GetOffset( &pointImageOffest );\
	int		x, y, xImg, xImgM, xImgP, yImg, yImgM, yImgP, yBmp;																			\
	byte	fillR = GetRValue(dwFillColor);																	\
	byte	fillG = GetGValue(dwFillColor);																	\
	byte	fillB = GetBValue(dwFillColor);																	\
/*	byte	foreFillR[256];																	\
	byte	foreFillG[256];																	\
	byte	foreFillB[256];																	\
	{for(int i=0; i<256; i++) \
		{foreFillR[i]=fillR; foreFillG[i]=fillG; foreFillB[i]=fillB;} \
		foreFillR[254]=255; foreFillG[254]=0; foreFillB[254]=0;												\
	}*/																										\
	byte	*p, *pmask, *pmaskM, *pmaskP;																						\
	color	*pcolor0M, *pcolor1M, *pcolor2M;																	\
	color	*pcolor0P, *pcolor1P, *pcolor2P;																	\
	int ax,ay;	/*0..256 для билинейной интерполяции*/																\
	int		cx4Bmp = (pbi->biWidth*3+3)/4*4;																\
	int	nMaxX = min( rectDest.left+pbi->biWidth, rectDest.right );\
	int	nMaxY = min( rectDest.top+pbi->biHeight, rectDest.bottom );\
	int	nMinX = max( rectDest.left, 0 );\
	int	nMinY = max( rectDest.top, 0 );\
	int	xBmpStart = max( int(ceil(( pointImageOffest.x)*fZoom-ptScroll.x)), nMinX );				\
	int	xBmpEnd = min( int(floor((pointImageOffest.x+cxImg)*fZoom-ptScroll.x+.5)), nMaxX );		\
	int	yBmpStart = max( int(ceil((pointImageOffest.y)*fZoom-ptScroll.y)), nMinY );					\
	int	yBmpEnd = min( int(floor((pointImageOffest.y+cyImg)*fZoom-ptScroll.y+.5)), nMaxY );		\
	int	yImgOld = -1;																						\
	int	yImgOldP = -100;																						\
	int	yImgOldM = -100;																						\
	int	t, cx = max(xBmpEnd-xBmpStart, 0), cy = max(yBmpEnd-yBmpStart, 0);									\
	int	*pxofs = new int[cx];																				\
	int	*pyofs = new int[cy];																				\
	int	*pax = new int[cx];	/*таблицы ax*/																			\
	int	*pay = new int[cy];	/*таблицы ay*/																			\
	int	*pxofsM = new int[cx];																				\
	int	*pyofsM = new int[cy];																				\
	int	*pxofsP = new int[cx];																				\
	int	*pyofsP = new int[cy];																				\
																											\
	for( t = 0; t < xBmpEnd-xBmpStart; t++ )																\
	{	pxofs[t] = int(floor((t+xBmpStart+ptScroll.x+0.5)/fZoom-pointImageOffest.x)); \
		pxofsM[t] = int(floor((t+xBmpStart+ptScroll.x+0.5)/fZoom-pointImageOffest.x-0.5)); \
		pxofsP[t] = int(floor((t+xBmpStart+ptScroll.x+0.5)/fZoom-pointImageOffest.x+0.5)); \
		pax[t] = int(floor(((t+xBmpStart+ptScroll.x+0.5)/fZoom-pointImageOffest.x-0.5-pxofsM[t])*256+0.5)); \
		pxofs[t] = min(max(pxofs[t],0),cxImg-1);	/*ограничить*/							\
		pxofsM[t] = min(max(pxofsM[t],0),cxImg-1);	/*ограничить*/							\
		pxofsP[t] = min(max(pxofsP[t],0),cxImg-1);	/*ограничить*/							\
	}																										\
	for( t = 0; t < yBmpEnd-yBmpStart; t++ )																\
	{	pyofs[t] = int(floor((t+yBmpStart+ptScroll.y+0.5)/fZoom-pointImageOffest.y)); \
		pyofsM[t] = int(floor((t+yBmpStart+ptScroll.y+0.5)/fZoom-pointImageOffest.y-0.5)); \
		pyofsP[t] = int(floor((t+yBmpStart+ptScroll.y+0.5)/fZoom-pointImageOffest.y+0.5)); \
		pay[t] = int(floor(((t+yBmpStart+ptScroll.y+0.5)/fZoom-pointImageOffest.y-0.5-pyofsM[t])*256+0.5)); \
		pyofs[t] = min(max(pyofs[t],0),cyImg-1);	/*ограничить*/ \
		pyofsM[t] = min(max(pyofsM[t],0),cyImg-1);	/*ограничить*/ \
		pyofsP[t] = min(max(pyofsP[t],0),cyImg-1);	/*ограничить*/ \
	}																										\
																											\
	for( y = yBmpStart, yBmp = yBmpStart; y < yBmpEnd; y++, yBmp++ )										\
	{																										\
		/*get the pointer to the BGR struct*/																\
		p = pdibBits+(pbi->biHeight-yBmp-1+ptBmpOfs.y)*cx4Bmp+(xBmpStart-ptBmpOfs.x)*3;						\
		/*offset coordinates to image*/																		\
		yImg =pyofs[y-yBmpStart];																			\
		yImgM =pyofsM[y-yBmpStart];																			\
		yImgP =pyofsP[y-yBmpStart];																			\
		ay =pay[y-yBmpStart];																			\
		if( yImgOld != yImg )																				\
		{																									\
			pimage->GetRowMask( yImg, &pmask );																\
			yImgOld = yImg;																					\
		}																									\
		if( yImgOldM != yImgM )																				\
		{																									\
			pimage->GetRowMask( yImgM, &pmaskM );																\
			pimage->GetRow( yImgM, 0, &pcolor0M );															\
			pimage->GetRow( yImgM, 1, &pcolor1M );															\
			pimage->GetRow( yImgM, 2, &pcolor2M );															\
			yImgOldM = yImgM;																					\
		}																									\
		if( yImgOldP != yImgP )																				\
		{																									\
			pimage->GetRowMask( yImgP, &pmaskP );																\
			pimage->GetRow( yImgP, 0, &pcolor0P );															\
			pimage->GetRow( yImgP, 1, &pcolor1P );															\
			pimage->GetRow( yImgP, 2, &pcolor2P );															\
			yImgOldP = yImgP;																					\
		}																									\
		for( x = xBmpStart; x < xBmpEnd; x++ )																\
		{																									\
			/*get the x coord of source image*/																\
			xImg = pxofs[x-xBmpStart];																\
			xImgM = pxofsM[x-xBmpStart];																\
			xImgP = pxofsP[x-xBmpStart];																\
			ax = pax[x-xBmpStart];																\
			/*int mask_M = (pmaskM[xImgM]<0x80?0:256-ay) + (pmaskP[xImgM]<0x80?0:ay);*/																	\
			/*int mask_P = (pmaskM[xImgP]<0x80?0:256-ay) + (pmaskP[xImgP]<0x80?0:ay);*/																	\
			/*int mask = mask_M*(256-ax)+mask_P*ax;*/ \
			int mask; \
			if(dwFlags & cidrIgnoreMask) mask=1; \
			else if(nMaskInterpolationMethod) \
			{ \
				int mMM=pmaskM[xImgM]<0x80, mMP=pmaskM[xImgP]<0x80, mPM=pmaskP[xImgM]<0x80, mPP=pmaskP[xImgP]<0x80; \
				if(xImg<=1 || xImg>=cxImg-1) { \
					if(xImgP<=0) {mMM=1; mPM=1;} \
					if(xImgM>=cxImg-1) {mMP=1; mPP=1;} \
				} \
				if(yImg<=1 || yImg>=cyImg-1) { \
					if(yImgP<=0) {mMM=1; mMP=1;} \
					if(yImgM>=cyImg-1) {mPM=1; mPP=1;} \
				} \
				int bx=ax-128, by=ay-128; \
				mask=(mMM?-64:64-bx-by)+(mMP?-64:64+bx-by)+(mPM?-64:64-bx+by)+(mPP?-64:64+bx+by); \
				/*if( (pmask[xImg] & 0x80) != 0x80 )*/ \
				mask=(mask<=0); \
			} \
			else mask=pmask[xImg]<0x80; \
			if( mask )\
			{																								\
				if(dwFlags & cidrFillBackByColor)															\
				{																							\
					*p = fillB; p++;																		\
					*p = fillG; p++;																		\
					*p = fillR; p++;																		\
				}																							\
				else																						\
				{																							\
					p++;																					\
					p++;																					\
					p++;																					\
				}																							\
			}																								\
			else																							\
			{																								\
				if( dwFlags & cidrFillBodyByColor )															\
				{																							\
					int c=pmask[xImg]; \
					*p = fillB/*foreFillB[c]*/; p++;																		\
					*p = fillG/*foreFillG[c]*/; p++;																		\
					*p = fillR/*foreFillR[c]*/; p++;																		\
				}																							\
				else																						\
				{																							\
				color R_M = (pcolor0M[xImgM]*(256-ay)+pcolor0P[xImgM]*ay)/256;																	\
				color G_M = (pcolor1M[xImgM]*(256-ay)+pcolor1P[xImgM]*ay)/256;																	\
				color B_M = (pcolor2M[xImgM]*(256-ay)+pcolor2P[xImgM]*ay)/256;																	\
				color R_P = (pcolor0M[xImgP]*(256-ay)+pcolor0P[xImgP]*ay)/256;																	\
				color G_P = (pcolor1M[xImgP]*(256-ay)+pcolor1P[xImgP]*ay)/256;																	\
				color B_P = (pcolor2M[xImgP]*(256-ay)+pcolor2P[xImgP]*ay)/256;																	\
				color R = (R_M*(256-ax)+R_P*ax)/256; \
				color G = (G_M*(256-ax)+G_P*ax)/256; \
				color B = (B_M*(256-ax)+B_P*ax)/256; \
				byte  Rbyte = 0;																			\
				byte  Gbyte = 0;																			\
				byte  Bbyte = 0;



#define CONVERTTODIB_PART1_BILINEAR_HLS																			\
	int		cxImg, cyImg;																					\
	pimage->GetSize( &cxImg, &cyImg );																		\
	if( (dwFlags & cidrUseImageOffset) == cidrUseImageOffset ) pimage->GetOffset( &pointImageOffest );\
	int		x, y, xImg, xImgM, xImgP, yImg, yImgM, yImgP, yBmp;																			\
	byte	fillR = GetRValue(dwFillColor);																	\
	byte	fillG = GetGValue(dwFillColor);																	\
	byte	fillB = GetBValue(dwFillColor);																	\
/*	byte	foreFillR[256];																	\
	byte	foreFillG[256];																	\
	byte	foreFillB[256];																	\
	{for(int i=0; i<256; i++) \
		{foreFillR[i]=fillR; foreFillG[i]=fillG; foreFillB[i]=fillB;} \
		foreFillR[254]=255; foreFillG[254]=0; foreFillB[254]=0;												\
	}*/																										\
	byte	*p, *pmask, *pmaskM, *pmaskP;																						\
	color	*pcolor0M, *pcolor1M, *pcolor2M;																	\
	color	*pcolor0P, *pcolor1P, *pcolor2P;																	\
	int ax,ay;	/*0..256 для билинейной интерполяции*/																\
	int		cx4Bmp = (pbi->biWidth*3+3)/4*4;																\
	int	nMaxX = min( rectDest.left+pbi->biWidth, rectDest.right );\
	int	nMaxY = min( rectDest.top+pbi->biHeight, rectDest.bottom );\
	int	nMinX = max( rectDest.left, 0 );\
	int	nMinY = max( rectDest.top, 0 );\
	int	xBmpStart = max( int(ceil(( pointImageOffest.x)*fZoom-ptScroll.x)), nMinX );				\
	int	xBmpEnd = min( int(floor((pointImageOffest.x+cxImg)*fZoom-ptScroll.x+.5)), nMaxX );		\
	int	yBmpStart = max( int(ceil((pointImageOffest.y)*fZoom-ptScroll.y)), nMinY );					\
	int	yBmpEnd = min( int(floor((pointImageOffest.y+cyImg)*fZoom-ptScroll.y+.5)), nMaxY );		\
	int	yImgOld = -1;																						\
	int	yImgOldP = -100;																						\
	int	yImgOldM = -100;																						\
	int	t, cx = max(xBmpEnd-xBmpStart, 0), cy = max(yBmpEnd-yBmpStart, 0);									\
	int	*pxofs = new int[cx];																				\
	int	*pyofs = new int[cy];																				\
	int	*pax = new int[cx];	/*таблицы ax*/																			\
	int	*pay = new int[cy];	/*таблицы ay*/																			\
	int	*pxofsM = new int[cx];																				\
	int	*pyofsM = new int[cy];																				\
	int	*pxofsP = new int[cx];																				\
	int	*pyofsP = new int[cy];																				\
																											\
	for( t = 0; t < xBmpEnd-xBmpStart; t++ )																\
	{	pxofs[t] = int(floor((t+xBmpStart+ptScroll.x+0.5)/fZoom-pointImageOffest.x)); \
		pxofsM[t] = int(floor((t+xBmpStart+ptScroll.x+0.5)/fZoom-pointImageOffest.x-0.5)); \
		pxofsP[t] = int(floor((t+xBmpStart+ptScroll.x+0.5)/fZoom-pointImageOffest.x+0.5)); \
		pax[t] = int(floor(((t+xBmpStart+ptScroll.x+0.5)/fZoom-pointImageOffest.x-0.5-pxofsM[t])*256+0.5)); \
		pxofs[t] = min(max(pxofs[t],0),cxImg-1);	/*ограничить*/							\
		pxofsM[t] = min(max(pxofsM[t],0),cxImg-1);	/*ограничить*/							\
		pxofsP[t] = min(max(pxofsP[t],0),cxImg-1);	/*ограничить*/							\
	}																										\
	for( t = 0; t < yBmpEnd-yBmpStart; t++ )																\
	{	pyofs[t] = int(floor((t+yBmpStart+ptScroll.y+0.5)/fZoom-pointImageOffest.y)); \
		pyofsM[t] = int(floor((t+yBmpStart+ptScroll.y+0.5)/fZoom-pointImageOffest.y-0.5)); \
		pyofsP[t] = int(floor((t+yBmpStart+ptScroll.y+0.5)/fZoom-pointImageOffest.y+0.5)); \
		pay[t] = int(floor(((t+yBmpStart+ptScroll.y+0.5)/fZoom-pointImageOffest.y-0.5-pyofsM[t])*256+0.5)); \
		pyofs[t] = min(max(pyofs[t],0),cyImg-1);	/*ограничить*/ \
		pyofsM[t] = min(max(pyofsM[t],0),cyImg-1);	/*ограничить*/ \
		pyofsP[t] = min(max(pyofsP[t],0),cyImg-1);	/*ограничить*/ \
	}																										\
																											\
	for( y = yBmpStart, yBmp = yBmpStart; y < yBmpEnd; y++, yBmp++ )										\
	{																										\
		/*get the pointer to the BGR struct*/																\
		p = pdibBits+(pbi->biHeight-yBmp-1+ptBmpOfs.y)*cx4Bmp+(xBmpStart-ptBmpOfs.x)*3;						\
		/*offset coordinates to image*/																		\
		yImg =pyofs[y-yBmpStart];																			\
		yImgM =pyofsM[y-yBmpStart];																			\
		yImgP =pyofsP[y-yBmpStart];																			\
		ay =pay[y-yBmpStart];																			\
		if( yImgOld != yImg )																				\
		{																									\
			pimage->GetRowMask( yImg, &pmask );																\
			yImgOld = yImg;																					\
		}																									\
		if( yImgOldM != yImgM )																				\
		{																									\
			pimage->GetRowMask( yImgM, &pmaskM );																\
			pimage->GetRow( yImgM, 0, &pcolor0M );															\
			pimage->GetRow( yImgM, 1, &pcolor1M );															\
			pimage->GetRow( yImgM, 2, &pcolor2M );															\
			yImgOldM = yImgM;																					\
		}																									\
		if( yImgOldP != yImgP )																				\
		{																									\
			pimage->GetRowMask( yImgP, &pmaskP );																\
			pimage->GetRow( yImgP, 0, &pcolor0P );															\
			pimage->GetRow( yImgP, 1, &pcolor1P );															\
			pimage->GetRow( yImgP, 2, &pcolor2P );															\
			yImgOldP = yImgP;																					\
		}																									\
		for( x = xBmpStart; x < xBmpEnd; x++ )																\
		{																									\
			/*get the x coord of source image*/																\
			xImg = pxofs[x-xBmpStart];																\
			xImgM = pxofsM[x-xBmpStart];																\
			xImgP = pxofsP[x-xBmpStart];																\
			ax = pax[x-xBmpStart];																\
			/*int mask_M = (pmaskM[xImgM]<0x80?0:256-ay) + (pmaskP[xImgM]<0x80?0:ay);*/																	\
			/*int mask_P = (pmaskM[xImgP]<0x80?0:256-ay) + (pmaskP[xImgP]<0x80?0:ay);*/																	\
			/*int mask = mask_M*(256-ax)+mask_P*ax;*/ \
			int mask; \
			if(dwFlags & cidrIgnoreMask) mask=1; \
			else if(nMaskInterpolationMethod) \
			{ \
				int mMM=pmaskM[xImgM]<0x80, mMP=pmaskM[xImgP]<0x80, mPM=pmaskP[xImgM]<0x80, mPP=pmaskP[xImgP]<0x80; \
				if(xImg<=1 || xImg>=cxImg-1) { \
					if(xImgP<=0) {mMM=1; mPM=1;} \
					if(xImgM>=cxImg-1) {mMP=1; mPP=1;} \
				} \
				if(yImg<=1 || yImg>=cyImg-1) { \
					if(yImgP<=0) {mMM=1; mMP=1;} \
					if(yImgM>=cyImg-1) {mPM=1; mPP=1;} \
				} \
				int bx=ax-128, by=ay-128; \
				mask=(mMM?-64:64-bx-by)+(mMP?-64:64+bx-by)+(mPM?-64:64-bx+by)+(mPP?-64:64+bx+by); \
				/*if( (pmask[xImg] & 0x80) != 0x80 )*/ \
				mask=(mask<=0); \
			} \
			else mask=pmask[xImg]<0x80; \
			if( mask )\
			{																								\
				if(dwFlags & cidrFillBackByColor)															\
				{																							\
					*p = fillB; p++;																		\
					*p = fillG; p++;																		\
					*p = fillR; p++;																		\
				}																							\
				else																						\
				{																							\
					p++;																					\
					p++;																					\
					p++;																					\
				}																							\
			}																								\
			else																							\
			{																								\
				if( dwFlags & cidrFillBodyByColor )															\
				{																							\
					int c=pmask[xImg]; \
					*p = fillB/*foreFillB[c]*/; p++;																		\
					*p = fillG/*foreFillG[c]*/; p++;																		\
					*p = fillR/*foreFillR[c]*/; p++;																		\
				}																							\
				else																						\
				{																		\
				int mM = pcolor0M[xImgM], pP = pcolor0P[xImgM]; 						\
				color T = -1;															\
				T/=2;																	\
				if( mM > pP  && mM - pP >T)												\
				{																		\
					mM-=T*2;																\
				}																		\
				else if(pP > mM  && pP - mM >T)											\
				{																		\
					pP-=T*2;																\
				}																		\
																						\
				int R_M = (mM*(256-ay)+pP*ay)/256;									\
				color G_M = (pcolor1M[xImgM]*(256-ay)+pcolor1P[xImgM]*ay)/256;			\
				color B_M = (pcolor2M[xImgM]*(256-ay)+pcolor2P[xImgM]*ay)/256;			\
																						\
				mM = pcolor0M[xImgP], pP = pcolor0P[xImgP];								\
				if( mM > pP  && mM - pP >T)												\
				{																		\
					mM-=T*2;																\
				}																		\
				else if(pP > mM  && pP - mM >T)											\
				{																		\
					pP-=T*2;																\
				}																		\
				int R_P = (mM*(256-ay)+pP*ay)/256;									\
				color G_P = (pcolor1M[xImgP]*(256-ay)+pcolor1P[xImgP]*ay)/256;			\
				color B_P = (pcolor2M[xImgP]*(256-ay)+pcolor2P[xImgP]*ay)/256;			\
																						\
				mM = R_M, pP = R_P;														\
				if( mM > pP  && mM - pP >T)												\
				{																		\
					mM-=T*2;																\
				}																		\
				else if(pP > mM  && pP - mM >T)											\
				{																		\
					pP-=T*2;																\
				}																		\
				int rR =  (mM*(256-ax)+pP*ax)/256;\
				\
				color R = rR>0? rR: rR+T*2;										\
				color G = (G_M*(256-ax)+G_P*ax)/256;									\
				color B = (B_M*(256-ax)+B_P*ax)/256;									\
				byte  Rbyte = 0;														\
				byte  Gbyte = 0;														\
				byte  Bbyte = 0;



#define CONVERTTODIB_PART2_BILINEAR																					\
				*p = Bbyte; p++;																			\
				*p = Gbyte; p++;																			\
				*p = Rbyte; p++;																			\
				}																							\
			}																								\
		}																									\
	}																										\
	delete pxofsM;delete pyofsM; \
	delete pxofsP;delete pyofsP; \
	delete pax;delete pay; \
	delete pxofs;delete pyofs;

																											

#define CONVERTTODIB_PART1_TRANSP																					\
	int		cxImg, cyImg;																					\
	pimage->GetSize( &cxImg, &cyImg );																		\
	if( (dwFlags & cidrUseImageOffset) == cidrUseImageOffset ) pimage->GetOffset( &pointImageOffest );\
	int		x, y, xImg, yImg, yBmp;																			\
	byte	fillR = GetRValue(dwFillColor);																	\
	byte	fillG = GetGValue(dwFillColor);																	\
	byte	fillB = GetBValue(dwFillColor);																	\
/*	byte	foreFillR[256];																	\
	byte	foreFillG[256];																	\
	byte	foreFillB[256];																	\
	{for(int i=0; i<256; i++) \
		{foreFillR[i]=fillR; foreFillG[i]=fillG; foreFillB[i]=fillB;} \
		foreFillR[254]=255; foreFillG[254]=0; foreFillB[254]=0;												\
	}*/																										\
	byte	*p, *pmask;\
	WORD	*pDistSum;\
	color	*pcolor0, *pcolor1, *pcolor2, *pcolor3, *pcolor4, *pcolor5, *pcolor6, *pcolor7, *pcolor8, *pcolor9; \
	color	*pcolor10, *pcolor11, *pcolor12, *pcolor13, *pcolor14, *pcolor15, *pcolor16, *pcolor17, *pcolor18, *pcolor19; \
	color	*pcolor20, *pcolor21, *pcolor22, *pcolor23, *pcolor24, *pcolor25, *pcolor26, *pcolor27, *pcolor28, *pcolor29; \
	color	*pcolor30, *pcolor31; \
	WORD	*pDistRow; \
	int		cx4Bmp = (pbi->biWidth*3+3)/4*4;																\
	int	nMaxX = min( rectDest.left+pbi->biWidth, rectDest.right );\
	int	nMaxY = min( rectDest.top+pbi->biHeight, rectDest.bottom );\
	int	nMinX = max( rectDest.left, 0 );\
	int	nMinY = max( rectDest.top, 0 );\
	int	xBmpStart = max( int((( pointImageOffest.x)*fZoom-ptScroll.x)), nMinX );				\
	int	xBmpEnd = min( int(((pointImageOffest.x+cxImg)*fZoom-ptScroll.x)), nMaxX );		\
	int	yBmpStart = max( int(((pointImageOffest.y)*fZoom-ptScroll.y)), nMinY );					\
	int	yBmpEnd = min( int(((pointImageOffest.y+cyImg)*fZoom-ptScroll.y)), nMaxY );		\
	int	yImgOld = -1;																						\
	int	t, cx = max(xBmpEnd-xBmpStart, 0), cy = max(yBmpEnd-yBmpStart, 0);									\
	int	*pxofs = new int[cx];																				\
	int	*pyofs = new int[cy];																				\
																											\
	for( t = 0; t < xBmpEnd-xBmpStart; t++ )																\
		pxofs[t] = int((t+xBmpStart+ptScroll.x)/fZoom-pointImageOffest.x);									\
	for( t = 0; t < yBmpEnd-yBmpStart; t++ )																\
		pyofs[t] = int((t+yBmpStart+ptScroll.y)/fZoom-pointImageOffest.y);									\
																											\
	for( y = yBmpStart, yBmp = yBmpStart; y < yBmpEnd; y++, yBmp++ )										\
	{																										\
		/*get the pointer to the BGR struct*/																\
		p = pdibBits+(pbi->biHeight-yBmp-1+ptBmpOfs.y)*cx4Bmp+(xBmpStart-ptBmpOfs.x)*3;						\
		pDistSum = pDistBits+(pbi->biHeight-yBmp-1+ptBmpOfs.y)*pbi->biWidth+(xBmpStart-ptBmpOfs.x);			\
		/*offset coordinates to image*/																		\
		yImg =pyofs[y-yBmpStart];																			\
		if (yImg < 0 || yImg >= cyImg) continue;															\
		if( yImgOld != yImg )																				\
		{																									\
			IFPANE(0) pimage->GetRow( yImg, 0, &pcolor0 );													\
			IFPANE(1) pimage->GetRow( yImg, 1, &pcolor1 );													\
			IFPANE(2) pimage->GetRow( yImg, 2, &pcolor2 );													\
			IFPANE(3) pimage->GetRow( yImg, 3, &pcolor3 );													\
			IFPANE(4) pimage->GetRow( yImg, 4, &pcolor4 );													\
			IFPANE(5) pimage->GetRow( yImg, 5, &pcolor5 );													\
			IFPANE(6) pimage->GetRow( yImg, 6, &pcolor6 );													\
			IFPANE(7) pimage->GetRow( yImg, 7, &pcolor7 );													\
			IFPANE(8) pimage->GetRow( yImg, 8, &pcolor8 );													\
			IFPANE(9) pimage->GetRow( yImg, 9, &pcolor9 );													\
			IFPANE(10) pimage->GetRow( yImg, 10, &pcolor10 );													\
			IFPANE(11) pimage->GetRow( yImg, 11, &pcolor11 );													\
			IFPANE(12) pimage->GetRow( yImg, 12, &pcolor12 );													\
			IFPANE(13) pimage->GetRow( yImg, 13, &pcolor13 );													\
			IFPANE(14) pimage->GetRow( yImg, 14, &pcolor14 );													\
			IFPANE(15) pimage->GetRow( yImg, 15, &pcolor15 );													\
			IFPANE(16) pimage->GetRow( yImg, 16, &pcolor16 );													\
			IFPANE(17) pimage->GetRow( yImg, 17, &pcolor17 );													\
			IFPANE(18) pimage->GetRow( yImg, 18, &pcolor18 );													\
			IFPANE(19) pimage->GetRow( yImg, 19, &pcolor19 );													\
			IFPANE(20) pimage->GetRow( yImg, 20, &pcolor20 );													\
			IFPANE(21) pimage->GetRow( yImg, 21, &pcolor21 );													\
			IFPANE(22) pimage->GetRow( yImg, 22, &pcolor22 );													\
			IFPANE(23) pimage->GetRow( yImg, 23, &pcolor23 );													\
			IFPANE(24) pimage->GetRow( yImg, 24, &pcolor24 );													\
			IFPANE(25) pimage->GetRow( yImg, 25, &pcolor25 );													\
			IFPANE(26) pimage->GetRow( yImg, 26, &pcolor26 );													\
			IFPANE(27) pimage->GetRow( yImg, 27, &pcolor27 );													\
			IFPANE(28) pimage->GetRow( yImg, 28, &pcolor28 );													\
			IFPANE(29) pimage->GetRow( yImg, 29, &pcolor29 );													\
			IFPANE(30) pimage->GetRow( yImg, 30, &pcolor30 );													\
			IFPANE(31) pimage->GetRow( yImg, 31, &pcolor31 );													\
			pimage->GetRowMask( yImg, &pmask );																\
			pDistMap->GetRow( yImg, &pDistRow );													\
			yImgOld = yImg;																					\
		}																									\
		for( x = xBmpStart; x < xBmpEnd; x++ )																\
		{																									\
			/*get the x coord of source image*/																\
			xImg = pxofs[x-xBmpStart];																\
			if (xImg < 0 || xImg >= cxImg) continue;														\
			if( ((pmask[xImg] & 0x80) == 0) && ((dwFlags & cidrIgnoreMask)==0) )																\
			{																								\
				if(dwFlags & cidrFillBackByColor)															\
				{																							\
					*p = fillB; p++;																		\
					*p = fillG; p++;																		\
					*p = fillR; p++;																		\
				}																							\
				else																						\
				{																							\
					p++;																					\
					p++;																					\
					p++;																					\
				}																							\
			}																								\
			else																							\
			{																								\
				if( dwFlags & cidrFillBodyByColor )															\
				{																							\
					int c=pmask[xImg]; \
					*p = fillB/*foreFillB[c]*/; p++;																		\
					*p = fillG/*foreFillG[c]*/; p++;																		\
					*p = fillR/*foreFillR[c]*/; p++;																		\
				}																							\
				else																						\
				{																							\
				color R = 0,G = 0,B = 0;																	\
				IFPANE(0) R = pcolor0[xImg]; \
				IFPANE(1) G = pcolor1[xImg];																	\
				IFPANE(2) B = pcolor2[xImg];																	\
				color pan3=0,pan4=0,pan5=0,pan6=0,pan7=0, pan8=0,pan9=0; \
				color pan10=0,pan11=0,pan12=0,pan13=0,pan14=0,pan15=0,pan16=0,pan17=0, pan18=0,pan19=0; \
				color pan20=0,pan21=0,pan22=0,pan23=0,pan24=0,pan25=0,pan26=0,pan27=0, pan28=0,pan29=0; \
				color pan30=0,pan31=0; \
				IFPANE(3) pan3=pcolor3[xImg]; \
				IFPANE(4) pan4=pcolor4[xImg]; \
				IFPANE(5) pan5=pcolor5[xImg]; \
				IFPANE(6) pan6=pcolor6[xImg]; \
				IFPANE(7) pan7=pcolor7[xImg]; \
				IFPANE(8) pan8=pcolor8[xImg]; \
				IFPANE(9) pan9=pcolor9[xImg]; \
				IFPANE(10) pan10=pcolor10[xImg]; \
				IFPANE(11) pan11=pcolor11[xImg]; \
				IFPANE(12) pan12=pcolor12[xImg]; \
				IFPANE(13) pan13=pcolor13[xImg]; \
				IFPANE(14) pan14=pcolor14[xImg]; \
				IFPANE(15) pan15=pcolor15[xImg]; \
				IFPANE(16) pan16=pcolor16[xImg]; \
				IFPANE(17) pan17=pcolor17[xImg]; \
				IFPANE(18) pan18=pcolor18[xImg]; \
				IFPANE(19) pan19=pcolor19[xImg]; \
				IFPANE(20) pan20=pcolor20[xImg]; \
				IFPANE(21) pan21=pcolor21[xImg]; \
				IFPANE(22) pan22=pcolor22[xImg]; \
				IFPANE(23) pan23=pcolor23[xImg]; \
				IFPANE(24) pan24=pcolor24[xImg]; \
				IFPANE(25) pan25=pcolor25[xImg]; \
				IFPANE(26) pan26=pcolor26[xImg]; \
				IFPANE(27) pan27=pcolor27[xImg]; \
				IFPANE(28) pan28=pcolor28[xImg]; \
				IFPANE(29) pan29=pcolor29[xImg]; \
				IFPANE(30) pan30=pcolor30[xImg]; \
				IFPANE(31) pan31=pcolor31[xImg]; \
				byte  Rbyte = 0;\
				byte  Gbyte = 0;\
				byte  Bbyte = 0;


#define CONVERTTODIB_PART2_TRANSP																					\
				WORD wDistSum = *pDistSum+pDistRow[xImg];\
				*p = (byte)(((DWORD)*p)**pDistSum/wDistSum+((DWORD)Bbyte)*pDistRow[xImg]/wDistSum); p++;																			\
				*p = (byte)(((DWORD)*p)**pDistSum/wDistSum+((DWORD)Gbyte)*pDistRow[xImg]/wDistSum); p++;																			\
				*p = (byte)(((DWORD)*p)**pDistSum/wDistSum+((DWORD)Rbyte)*pDistRow[xImg]/wDistSum); p++;																			\
				*pDistSum = wDistSum; pDistSum++;\
				}																							\
			}																								\
		}																									\
	}																										\
	delete pxofs;delete pyofs;

																						\

#endif // __ccmacro_h__