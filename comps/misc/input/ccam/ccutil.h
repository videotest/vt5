/* ccutil.h */
/* various helper functions */


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CCUTIL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CCUTIL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef CCUTIL_EXPORTS
#define CCUTIL_API __declspec(dllexport)
#else
#define CCUTIL_API __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C" {
#endif


// currently we know of 2 CFA patterns in use
// 1. the standard BAYER pattern used by CCD manufacturers
// 2. the diagonal pattern developed at IMEC to avoid the BAYER patent!
typedef enum { 
	BAYER = 0, 
	RGBDIAG,
	RGBVERTICAL
} CFA ;

// this enum is to define what the starting pixel in the image to be converted is
typedef	enum { 
	RED = 0, 
	GREEN,		// green next to red for Bayer
	BLUE, 
	GREEN2		// green next to blue for Bayer
} START_COLOR ;


typedef enum {
	BAYER_RED = 0,
	BAYER_GREEN_RED,
	BAYER_GREEN_BLUE,
	BAYER_BLUE
} BAYER_COLOR ;

typedef enum {
	DIAG_RED = 0,
	DIAG_GREEN,
	DIAG_BLUE
} DIAG_COLOR ;

// we need to know some of these things to do the conversion
// although we could put it in the call ...
struct	CCU_ColorParms {
	CFA			CFAType ;
	START_COLOR	StartColor ;
	double		RedLuminance, GreenLuminance, BlueLuminance ;
	double		Saturation, Brightness, Contrast ;
//	double gamma , act_gamma ;
//	unsigned char look_up_gamma[256] ;
	} ;

struct	CCU_CorrCoeffs {
	float	Multiplier ;
	float	Adder ;
} ;

struct	CCU_CorrCoeffsInt {
	SHORT	Multiplier ;
	SHORT	Adder ;
} ;

struct	CCU_NPointCorr {
	ULONG					IntervalTop ;
//	struct CCU_CorrCoeffs	* CorrCoeffs ;
	struct CCU_CorrCoeffs	CorrCoeffs[2056] ;
} ;

// calculates a histogram over a buffer
// if a valid rectangle is given in the hWnd it will draw a 256 wide by 192 high graphic 
// of that histogram
// user must supply enough storage in the histogram array (e.g. 1024 longs for 10 bit data)
// if gravity is og no interest supply a NULL pointer
extern CCUTIL_API int CCU_CalculateDrawHistogram( HWND hWnd , const void * imagebuffer , ULONG pixelcount , USHORT pixelwidth,
											  unsigned long * histogram ,  long * gravity 
											  ) ;

extern CCUTIL_API int CCU_CalculateDrawHistogramColor( HWND hWnd , const void * imagebuffer , ULONG pixelcount , USHORT pixelwidth, USHORT color,
											  unsigned long * histogram ,  long * gravity 
											  ) ;



extern CCUTIL_API int CCU_Colorize( PVOID InBuffer, PVOID OutBuffer, USHORT PixelWidth, USHORT Width, USHORT Height, 
			  					   struct CCU_ColorParms * cp ) ;


extern CCUTIL_API int CCU_CalculateWhiteBalance( PVOID ColBuffer, USHORT PixelWidth, USHORT Width, USHORT XStart, USHORT XEnd, 
												USHORT YStart, USHORT YEnd, struct CCU_ColorParms * cp ) ;


// saving a screen-comaptible buffer to file
// must be 8 bit grey values in buffer
// call function first with NULL pointer for FileSpec to just initialise the static (invariant)
// BitMapInfo things to speed up later writes
extern CCUTIL_API int  CCU_SaveBufferAsBMP_BW( char * FileSpec, PVOID buffer,
											 USHORT xsize, USHORT ysize ,
											 int flipvertical
											 ) ;

extern CCUTIL_API int  CCU_SaveBufferAsBMP_Color( char * FileSpec, PVOID buffer, 
												USHORT xsize, USHORT ysize , 
												int flipvertical 
												) ;

extern CCUTIL_API int  CCU_SaveBufferAsBinary(	char * FileSpec, 
												PVOID buffer, 
												ULONG size 
												) ;

extern CCUTIL_API int  CCU_TwoPointCorrection( PVOID InBuffer, PVOID OutBuffer, USHORT XSize, USHORT YSize , 
											 struct CCU_CorrCoeffs * CorrCoeffs ) ;
extern CCUTIL_API int  CCU_NPointCorrection( PVOID InBuffer, PVOID OutBuffer, USHORT XSize, USHORT YSize , 
											 USHORT Intervals, struct CCU_NPointCorr * NPointCorr ) ;


extern CCUTIL_API int  CCU_FPNCorrection( PVOID InBuffer, PVOID OutBuffer, USHORT PixelWidth, USHORT XSize,
										 USHORT XStart, USHORT YStart, USHORT XEnd, USHORT YEnd, USHORT XInc, USHORT YInc, 
										 PVOID CorrBuffer ) ;

extern CCUTIL_API int  CCU_InitDisplay( void ) ;
extern CCUTIL_API int  CCU_FreeDisplay( void ) ;
extern CCUTIL_API int  CCU_DisplayBuffer( HDC hDC, PVOID Buffer, USHORT XStart, USHORT YStart, USHORT XSize, USHORT YSize, 
										USHORT XPan, USHORT YPan, float Zoom, BOOL Color ) ;
extern CCUTIL_API int  CCU_DisplayBufferFit( HDC hDC, PVOID Buffer, USHORT XDest, USHORT YDest, USHORT XDestSize, USHORT YDestSize, 
										USHORT XSrcSize, USHORT YSrcSize ,BOOL Color ) ;
extern CCUTIL_API int  CCU_ConvertBuffer16To8( PVOID InBuffer, PVOID OutBuffer, ULONG PixelCount, UCHAR BitShift ) ;
extern CCUTIL_API int  CCU_ConvertBuffer10To8( PVOID InBuffer, PVOID OutBuffer, ULONG PixelCount ) ;
extern CCUTIL_API int  CCU_AutointensityParameters( int iAlgorithm , USHORT usMin , USHORT usMax , USHORT usGravity , ULONG ulLow , USHORT *usGain , USHORT *usOffset , ULONG *ulIntTime );

#ifdef __cplusplus
}
#endif
/* end of ccutil.h */
