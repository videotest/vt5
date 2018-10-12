// ImageH.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <string.h>
#include <api.h>
#include <image.h>
#include <stddef.h>
#include <stdlib.h>
#include <crtdbg.h>


#define printOffs(struc,member) printf( "%x " #struc " " #member "\n", offsetof (struc,member))

typedef unsigned short color;
typedef struct _CTIFileFilter
{
	int m_bEnableMonitoring;
} CTIFileFilter;

CTIFileFilter* tifFilter;
int tifFirstTime;

void TIFErrorHandler(const unsigned int error, const char *message, const char *qualifier)
{
	DestroyDelegateInfo();
	fprintf(stderr, "%s %s \n", message, "TIF Error");
}

void TIFMonitorHandler(const char *msg, const unsigned int curPos, const unsigned int maxPos)
{
	if (tifFilter == NULL) return;
	if (!tifFilter->m_bEnableMonitoring) return;
	if (tifFirstTime)
	{
		tifFirstTime = FALSE;
		//		tifFilter->StartNotification(maxPos);
	}
	//	if (curPos > 0)
	//		tifFilter->Notify(curPos);
}

void fill()
{
	Image* image = 0;
	color** pColor = 0;
	color** pColorC = 0;
	Image* clone, *clone2;
	int bUnImage, bUnClone;
	int j = 0; 
	unsigned i,k;

	int m_nWidth, m_nHeight, m_nDepth, m_nColors, numPanes;

	ErrorHandler oldErrorHandle = SetErrorHandler(TIFErrorHandler);
	MonitorHandler oldMonitorHandle = SetMonitorHandler(TIFMonitorHandler);

	ImageInfo image_info, clone_info;
	GetImageInfo(&image_info);
	strcpy(image_info.filename, "rgb-3c-16b.tiff");
	image = ReadImage(&image_info);
	bUnImage = UncondenseImage(image);

	GetImageInfo(&clone_info);
	strcpy(clone_info.filename, "rgb-3c-16b.tiff");
	clone = ReadImage(&clone_info);
	bUnClone = UncondenseImage(clone);


//	clone = CloneImage(image, 0, 0, 0);

	SetMonitorHandler(oldMonitorHandle);

	if (!image)
		return;

	m_nWidth = image->columns;
	m_nHeight = image->rows;
	if (!IsPseudoClass(image) && !IsGrayImage(image))
	{
		m_nDepth = image->matte ? 32 : 24;
	}
	else
	{
		m_nDepth = 8;
		if (IsMonochromeImage(image))
		{
			m_nDepth = 1;
		}
	}

	m_nColors = 1 << m_nDepth; //fake


	numPanes = 4;

	pColor = calloc(sizeof(color*), numPanes);
	pColorC = calloc(sizeof(color*), numPanes);
	for (j=0; j < numPanes; ++j)
	{
		pColor[j] = calloc(sizeof(color), m_nWidth*(m_nHeight+1));
		pColorC[j] = calloc(sizeof(color), m_nWidth*(m_nHeight+1));
	}
	for (i = 0; i < image->rows; ++i)
	{
		unsigned pos = i*image->columns;
		FillVTImageRow(clone, i, pColor, numPanes);
		FillVTImageRow(image, i, pColorC, numPanes);
		for (j = 0; j < numPanes; j++)
		{
			memcpy(pColorC[j] + (i+1)*image->columns, pColorC[j], image->columns*sizeof(color));
			for (k = 0; k < image->columns; ++k)
			{
				_ASSERTE(pColor[j][k] == pColorC[j][k]);
			}
		}
	}

	//clone2 = CloneImage(image, 0, 0, 0);
	for (i = 0; i < image->rows; ++i)
	{
		unsigned pos = i*image->columns;
		for (j = 0; j < numPanes; j++)
		{
			memcpy(pColorC[j], pColorC[j] + (i + 1)*image->columns, image->columns*sizeof(color));
		}
		//		FillMagickImageRow(image, i, pColor, numPanes);
		FillMagickImageRow(image, i, pColorC, numPanes);
		for (k = 0; k < image->columns; ++k)
		{
			unsigned ik = pos + k;

			switch (numPanes>4 ? 4 : numPanes){
			case 4:
				_ASSERTE(*((char*)(image->pixels + ik) + 4) == *((char*)(clone->pixels + ik) + 4));
			case 3:
				_ASSERTE(image->pixels[ik].blue == clone->pixels[ik].blue);
			case 2:
				_ASSERTE(image->pixels[ik].green == clone->pixels[ik].green);
			case 1:
				_ASSERTE(image->pixels[ik].red == clone->pixels[ik].red);
			default:;
			}
		}
	}
	DestroyImage(clone);
	for (j = 0; j < numPanes; ++j)
	{
		free(pColor[j]);
		free(pColorC[j]);
	}
	free(pColor);
	free(pColorC);
	pColor = 0;

	DestroyImage(image);

	SetErrorHandler(oldErrorHandle);
}

int main(int argc, char* argv[])
{
	printOffs(Image, class);
	printOffs(Image, compression);
	printOffs(Image, columns);
	printOffs(Image, rows);
	printOffs(Image, pixels);
	printOffs(Image, orphan);
	printOffs(Image, orphan);
	printOffs(Image, orphan);
	fill();
	return 0;
}

