// Sharpness.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sharpness.h"
#include "misc_utils.h"
#include <math.h>

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "ImagePane.h"

#include "fft_for_sharpness.h"

#define MaxColor color(-1)
#define Makeweight 0.001


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//		Sharpness
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CSharpnessInfo::s_pargs[] = 
{	

	{"Strengthening",	szArgumentTypeInt, "10", true, false },
	{"Noise",	szArgumentTypeInt, "0", true, false },
	{"Img",             szArgumentTypeImage, 0, true, true },
	{"Sharpness",	        szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false }	
};


/////////////////////////////////////////////////////////////////////////////

bool CSharpnessFilter::InvokeFilter()
{	
	real fStrength = GetArgDouble( "Strengthening" );
	fStrength = abs(fStrength)/10+1;
	
	real fPower = (real)(abs(GetArgDouble( "Noise" ))/10+1);
	fPower = 1 / fPower;



	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	IImage3Ptr	ptrDestImage( GetDataResult() );
	
	if( ptrSrcImage == NULL || ptrDestImage == NULL )
		return false;
	{	
		ICompatibleObjectPtr ptrCO( ptrDestImage );
		
		if( ptrCO == NULL )
			return false;

		if( S_OK != ptrCO->CreateCompatibleObject( ptrSrcImage, NULL, 0 ) )
		{			
			return false;
		}		
	}
	
	 //[Gipsy] 20.03.2006: Усиление резкости при помощи гомоморфой фильтрации

	long int nAdd = 100;


	CImagePaneCache paneCache( ptrSrcImage );

	int nPaneCount = paneCache.GetPanesCount( );
	SIZE size;

	ptrSrcImage->GetSize((int*)&size.cx,(int*)&size.cy);

	long int H = (long int)size.cy;
	long int W = (long int)size.cx;
	int layer = nPaneCount;
	byte *pmask;
	// обработка первого слоя (для серого изображения)
	if(layer == 1){

		int nPane = 0; 
		
		CImg img2(H, W); // исходное изображение
		// задание собственной переменной	
		for(long i = 0; i < H; i++){
			for(long j = 0; j < W; j++){
				color *pSrc;
				ptrSrcImage->GetRow( i, nPane ,&pSrc );
				img2.Cmplx_image[i][j].r = pSrc[j];
				img2.Cmplx_image[i][j].i = 0;
			}
		};
		if((H <= 2*nAdd) || (W <= 2*nAdd)) nAdd = (long)(min(H,W)/2-1);
		CImg img1(H+2*nAdd,W+2*nAdd);
		img2.increase_img(img2.Cmplx_image, H, W, nAdd, img1.Cmplx_image );
		
		H = H + 2*nAdd;
		W = W + 2*nAdd;
		long int Hn = npfa(H);
		long int Wn = npfa(W);
		// старт нотификации
		StartNotification( 3*Hn, nPane + 1);
		// доращивание исходной картинки
		CImg img(Hn, Wn);
		for(long i = 0; i < H; i++){
			for(long j = 0; j < W; j++){
				img.Cmplx_image[i][j] = img1.Cmplx_image[i][j];
			}
		};
		CImg cl_img(Hn, Wn); // логарифм исходного изображения
		for(long i = 0; i < H; i++){
			for(long j = 0; j < W; j++){
				cl_img.Cmplx_image[i][j].r = log(img.Cmplx_image[i][j].r + Makeweight);
				cl_img.Cmplx_image[i][j].i = 0;
			}
		};

		// центрирование логарфма
		cl_img.center(cl_img.Cmplx_image, Hn, Wn);
		// преобразование  через пераметр . Сперва задание "ненужных" параметров
		real yl = 1; 
		real yh = fStrength; 
		real c = 1;
		real D0 = 0.1;

		CImg nfcl_img(Hn, Wn); // фурье - образ логарифма исходного изображения

		// прямое преобразование Фурье
		CFFT_for_Sharpness temp_FFT;
		
		temp_FFT.FFT2(cl_img, Hn, Wn, -1);
		real multiply;
		for(long int u = 0; u < Hn; u++){
			for( long int v = 0; v < Wn; v++){	
				multiply = nfcl_img.R(u, v, yl, yh, c, D0, Hn, Wn, fPower);
				nfcl_img.Cmplx_image[u][v] = crmul(cl_img.Cmplx_image[u][v], multiply);
			};
			Notify(u);
		}

		// обратное преобразование Фурье
		real coeff = 1/((real)( Hn*Wn));
		for(long int u = 0; u < Hn; u++){
			for(long int v = 0; v < Wn; v++){
				nfcl_img.Cmplx_image[u][v] = crmul(nfcl_img.Cmplx_image[u][v], coeff);
			}
			Notify(u+Hn);
		};
		temp_FFT.FFT2(nfcl_img, Hn, Wn, 1);
	
		// обратное центрирование
		nfcl_img.center(nfcl_img.Cmplx_image, Hn, Wn);
	
		// возведение в степень
		for(long int i = 0; i < Hn; i++){
			for(long int j = 0; j < Wn; j++){
				nfcl_img.Cmplx_image[i][j].r = exp(nfcl_img.Cmplx_image[i][j].r);			
			}
			Notify(2*Hn+i);
		};


		
		color *pDst;
		// вывод изображения
		for(long i = nAdd; i < H-nAdd; i++){
			ptrSrcImage->GetRowMask( i-nAdd ,&pmask);
			ptrDestImage->GetRow( i-nAdd, nPane , &pDst);	
			for(long j = nAdd; j < W-nAdd; j++){
				if(*(pmask+j-nAdd) > 127){
					if (nfcl_img.Cmplx_image[i][j].r < 0){
							pDst[j-nAdd] = (color)(0);
					};
					if (nfcl_img.Cmplx_image[i][j].r > colorMax){
							pDst[j-nAdd] = (color)colorMax;
					}
					else{
							pDst[j-nAdd] = (color)(nfcl_img.Cmplx_image[i][j].r);
					}
				}
			}
		};

	FinishNotification();
	};
	if( layer == 3){
		int nPane = 0;
		
		// задание слоя интенсивности	
		real p1, p2, p3;
		p1 = p2 = p3 = (real)1/(real)3;
		CImg img2(H, W); // исходное изображение (интенсивность)

		// задание собственной переменной
		
		for(long i = 0; i < H; i++){
			color *pSrc1, *pSrc2, *pSrc3;
			ptrSrcImage->GetRow( i, 0 ,&pSrc1 );
			ptrSrcImage->GetRow( i, 1 ,&pSrc2 );
			ptrSrcImage->GetRow( i, 2 ,&pSrc3 );
			for(long j = 0; j < W; j++){
				img2.Cmplx_image[i][j].r = p1*(real)pSrc1[j] + p2*(real)pSrc2[j] + p3*(real)pSrc3[j];
				img2.Cmplx_image[i][j].i = 0;
			}
		};
		
		if((H <= 2*nAdd) || (W <= 2*nAdd)) nAdd = (long)(min(H,W)/2-1);
		CImg img1(H+2*nAdd,W+2*nAdd);
		img2.increase_img(img2.Cmplx_image, H, W, nAdd, img1.Cmplx_image );

		H = H + 2*nAdd;
		W = W + 2*nAdd;

		long int Hn = npfa(H);
		long int Wn = npfa(W);
		
		// старт нотификации
		StartNotification( 3*Hn, nPane + 1);

		// доращивание исходной картинки(интенсивности)
		CImg img(Hn, Wn);
		for(long i = 0; i < H; i++){
			for(long j = 0; j < W; j++){
				img.Cmplx_image[i][j] = img1.Cmplx_image[i][j];
			}
		};
		CImg cl_img(Hn, Wn); // логарифм исходного изображения(интенсивности)
		for(long i = 0; i < H; i++){
			for(long j = 0; j < W; j++){
				cl_img.Cmplx_image[i][j].r = log(img.Cmplx_image[i][j].r + Makeweight);
				cl_img.Cmplx_image[i][j].i = 0;
			}
		}; 
		// центрирование логарфма
		cl_img.center(cl_img.Cmplx_image, Hn, Wn);
		
		// преобразование  через пераметр . Сперва задание "ненужных" параметров
		real yl = 1; 
		real yh = fStrength; 
		real c = 1;
		real D0 = 0.1;
		
		CImg nfcl_img(Hn, Wn); // фурье - образ логарифма исходного изображения

		// прямое преобразование Фурье (интенсивности)
		CFFT_for_Sharpness temp_FFT;

		temp_FFT.FFT2(cl_img, Hn, Wn, -1);
		real multiply;
		for(long int u = 0; u < Hn; u++){
			for( long int v = 0; v < Wn; v++){
				multiply = nfcl_img.R(u, v, yl, yh, c, D0, Hn, Wn, fPower);
				nfcl_img.Cmplx_image[u][v] = crmul(cl_img.Cmplx_image[u][v],multiply);
			};
			Notify(u);
		};
		
		// обратное преобразование Фурье(интенсивности)
		real coeff = 1/((real)( Hn*Wn));
		for(long int u = 0; u < Hn; u++){
			for(long int v = 0; v < Wn; v++){
				nfcl_img.Cmplx_image[u][v] = crmul(nfcl_img.Cmplx_image[u][v], coeff);
			}
			Notify(u+Hn);
		};
		temp_FFT.FFT2(nfcl_img, Hn, Wn, 1);
		
		// обратное центрирование(интенсивности)
		nfcl_img.center(nfcl_img.Cmplx_image, Hn, Wn);

		// возведение в степень (интенсивности)
		for(long int i = 0; i < Hn; i++){
			for(long int j = 0; j < Wn; j++){
				nfcl_img.Cmplx_image[i][j].r = exp(nfcl_img.Cmplx_image[i][j].r);			
			}
			Notify(2*Hn+i);
		};

		real intension_old; 
		color *pSrc1, *pSrc2, *pSrc3 ;
		for(long i = nAdd; i < H-nAdd; i++){
			ptrSrcImage->GetRowMask( i-nAdd ,&pmask);
			color *pDst1, *pDst2, *pDst3;
					
			ptrDestImage->GetRow(i-nAdd, 0, &pDst1);
			ptrDestImage->GetRow(i-nAdd, 1, &pDst2);
			ptrDestImage->GetRow(i-nAdd, 2, &pDst3);
			
			real result1, result2, result3;
			ptrSrcImage->GetRow( i-nAdd, 0, &pSrc1);
			ptrSrcImage->GetRow( i-nAdd, 1, &pSrc2);
			ptrSrcImage->GetRow( i-nAdd, 2, &pSrc3);
			for(long j = nAdd; j < W-nAdd; j++){
				if(*(pmask+j-nAdd) > 127){
					if (nfcl_img.Cmplx_image[i][j].r < 0){
						nfcl_img.Cmplx_image[i][j].r = 0;
					}
					else if (nfcl_img.Cmplx_image[i][j].r > colorMax){
						nfcl_img.Cmplx_image[i][j].r = (real)colorMax;
					};
					intension_old = p1*(real)pSrc1[j-nAdd] + p2*(real)pSrc2[j-nAdd] + p3*(real)pSrc3[j-nAdd];
								
					if (intension_old != 0){
						real coeff = nfcl_img.Cmplx_image[i][j].r / intension_old;

						result1 = (real)(pSrc1[j-nAdd])*coeff;
						result2 = (real)(pSrc2[j-nAdd])*coeff;
						result3 = (real)(pSrc3[j-nAdd])*coeff;
				
						pDst1[j-nAdd] = max(0, min(colorMax, result1));
						pDst2[j-nAdd] = max(0, min(colorMax, result2));
						pDst3[j-nAdd] = max(0, min(colorMax, result3));
					}
				}
			}
		};
		FinishNotification(); 
	};
	return true;
}
CImg::CImg(long int M, long int N)
{
	H = M;
	W = N;
	Cmplx_temp = new complex[M*N];

	Cmplx_image = new complex*[M];
	for (long i = 0; i < M; i++){
		Cmplx_image[i] = Cmplx_temp + i*N;
	};

};



CImg::~CImg(void)
{	
		delete[] Cmplx_image;
		delete[] Cmplx_temp;
}

void CImg::center(complex** &img, long int H, long int W)
{	
	for (long int i = 0; i < H; i++){
		for (long int j = 0; j < W; j++){
			if ((i+j)%2 > 0){
				img[i][j].r = (-1)*img[i][j].r;
				img[i][j].i =(-1)*img[i][j].i;
			}
		}
	}
};

real CImg::R(long int u, long int v, real yl, real yh, real c, real D0, long int H, long int W, real power )
{
	real result = 1;
	real D2 = ((real)u/H-0.5)*((real)u/H-0.5) + ((real)v/W-0.5)*((real)v/W-0.5);
	
	//if(D2 < 0.03){
	//	result = (yh-yl)*pow(1-exp(-c*(D2/(D0*D0))),power)+yl;
	//}
	//else{
	//	result = yh;
	//}
	result = (yh-yl)*pow(1-exp(-c*(D2/(D0*D0))),power)+yl;
	//result = (yh-yl)*(1-exp(-c*(D2/(D0*D0))))+yl;
	return result;
}

void CImg::increase_img(complex** img, long int H, long int W, long int N, complex** &result)
{
	
	 //creating new (without bounds) image
	for(long i = N; i < H+N; i++){
		for (long j = N; j < W+N; j++){
			result[i][j] = img[i-N][j-N];
		}
	};

	// vertical left
	for(long i = 0; i < H; i++){
		for(long j = 0; j < N; j++){
			result[i+N][j] = img[i][N-j];
		}
	};

	// vertical right
	for(long i = 0; i < H; i++){
		for(long j = 0; j < N; j++){
			result[i+N][W+j+N] = img[i][W-j-1];
		}
	};
	// horisantal up
	for(long j = 0; j < W; j++){
		for(long i = 0; i < N; i++){
			result[i][j+N] = img[N-i][j];
		}
	}
	// horisantal down
	for(long j = 0; j < W; j++){
		for(long i = 0; i < N; i++){
			result[H+i+N][j+N] = img[H-i-1][j];
		}
	}
	// left up corner
	for(long j = 0; j < N; j++){
		for(long i = 0; i < N; i++){
			result[i][j] = img[N-j][N-i];
		}
	};
	// left down corner
	for(long j = 0; j < N; j++){
		for(long i = 0; i < N; i++){
			result[H+2*N-i-1][j] = 	img[H-2*N+j-1][N-i-1];
		}
	};

	// right up corner
	for(long j = 0; j < N; j++){
		for(long i = 0; i < N; i++){
			result[i][W+2*N-j-1] = img[N-j-1][W-2*N+i-1];
		}
	};
	// right down corner
	for(long j = 0; j < N; j++){
		for(long i = 0; i < N; i++){
			result[H+2*N-i-1][W+2*N-j-1] = img[N-j-1][W-i-1];
		}
	};
	
}

