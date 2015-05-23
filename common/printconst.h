#ifndef __printconst_h__
#define __printconst_h__

#define VTPIXEL_IN_MM		(100)

#ifndef FROM_DOUBLE_TO_VTPIXEL
#define FROM_DOUBLE_TO_VTPIXEL( fVal ) ( ((long) (((double )fVal) * ((double)VTPIXEL_IN_MM)) ) )
#endif

#ifndef FROM_VTPIXEL_TO_DOUBLE
#define FROM_VTPIXEL_TO_DOUBLE( nPixel ) ( (((double)nPixel) / ((double)VTPIXEL_IN_MM)) )
#endif									 


#define DISPLAY_FONT_RATIO	5.35

//"small" version
#ifndef D2V
#define D2V( fVal ) FROM_DOUBLE_TO_VTPIXEL( fVal )
#endif

#ifndef V2D
#define V2D( fVal ) FROM_VTPIXEL_TO_DOUBLE( fVal )
#endif


#endif//__printconst_h__


