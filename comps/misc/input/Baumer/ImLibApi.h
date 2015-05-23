#ifndef _CX_IM_LIBAPI_
#define _CX_IM_LIBAPI_
/*****************************************************************************
*    Programm:		 ImLib.dll                                               *   
*	 Aufgebaut auf:															 *
*    Version:        1.0                                                     *
*    Compiler:       Visual C++ 6.0                                          *
*    Betriebssystem: Windows NT 4.0 / Win95 / Win98 / W2K                    *
*    Zweck:          Deklaration der exportierten API-Funktionen             *
*                    zur Nutzung der bildbearbeitenden Funktionen            *        
*                    Bereitstellung von DEFINES zur allgemeinen Verwendung   *
*    Bemerkung:      das Modul erfordert keien Treiber oder zusätzlichen     *
*					 kundenspezifischen Bibliotheken					     *
*																			 *	
*    Erstellt am:    07.09.00                                                *
*    Stand:          12.09.00                                                *
*    Bearbeiter:     M. Pester                                               *
*    Copyright:      (C) Baumer Optronic GmbH		                         *
******************************************************************************
*                                                                            *   
*    Aenderungen:                                                            *   
*														                     *       
******************************************************************************/

// ############################################################################
// Microsoft specific defines
#ifndef _WINDEF_
//#include <windef.h>     // Basic Windows Type Definitions
    typedef unsigned long       DWORD;
    typedef int                 BOOL;
    typedef unsigned char       BYTE;
    typedef unsigned short      WORD;
    typedef void                *PVOID;
#endif

// Calling Conventions 
#if defined(__BORLANDC__) || (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
    #ifndef _WINDEF_
        #define WINAPI __stdcall
    #endif
#else
    #define WINAPI
#endif

#ifdef __cplusplus 
extern "C" {
#endif  /* __cplusplus */

// ############################################################################

#define CLAMP(v,l,h)	((v)<(l) ? (l) : (v) > (h) ? (h) : v)
#define MIN3( a,b,c)    (min(min(a,b),c))
#define MIN4( a,b,c,d)  (min(min(min(a,b),c),d))
#define MAX3( a,b,c)    (max(max(a,b),c))
#define MAX4( a,b,c,d)  (max(max(max(a,b),c),d))

typedef union _IM3BPP_ {

	struct { BYTE b,      g,      r; } rgb; 
	struct { BYTE y; CHAR u; CHAR v; } yuv; 
	struct { BYTE l; CHAR a; CHAR b; } lab; 

} IM3BPP, * PIM3BPP;

typedef union _IM3W10PP_ {
	struct { WORD r,      g,      b; } rgb; 
} IM3W10PP, * PIM3W10PP;

typedef union _IM3W12PP_ {
	struct { WORD r,      g,      b; } rgb; 
} IM3W12PP, * PIM3W12PP;

typedef union _IM3FPP_ {

	struct { float h,      s,      i; } HSI;
	struct { float h,      s,      l; } HLS;
	struct { float h,      s,      b; } HSB;
	struct { float L,      a,      b; } Lab; 
	struct { float x,      y,      z; } XYZ; 

} IM3FPP, * PIM3FPP;

/*****************************************************************************
funktionname:	ImLibVersion	
description:    Version number of library
input:			no
return:			Pointer to version string
note:		   
*****************************************************************************/
char * WINAPI ImLibVersion(  );


/*****************************************************************************
funktionname:	ImRgbLut	
description:    Load the LUT for 3 canal rgb images 
input:			fr, fg, fb 	- canal factor
				flag		- lut flag corresponded to enum LUTFLAGS
				
return:			no
				
note:		
*****************************************************************************/
enum {
	LUTALL = 100,
	LUT8   = 0,
	LUT10,
	LUT12
} /* LUTFLAGS */;

void WINAPI ImRgbLut( double fr, double fg, double fb, int flag );


/*****************************************************************************
funktionname:	ImAlloc	
description:    Allocation of memory for image calculation
input:			x			- horizontal dimension in pixel
				y			- vertical dimension in pixel
				itemSize	- pixelsize in byte	
return:			NULL
				Bytepointer
note:		
*****************************************************************************/
PBYTE WINAPI ImAlloc( int x, int y, int itemSize );

/*****************************************************************************
funktionname:	ImFree
description:    Free of memory for image calculation
input:			p			- Bytepointer to array allocated by ImAlloc
return:			
note:		
*****************************************************************************/
void WINAPI ImFree ( PVOID p );


//****************************************************************************
//****************************************************************************
enum { /* Interpolation */
	BOX			= 'b',
	TRIANGLE	= 't', 
	BELL		= 'q', 
	BSPLINE		= 'B', 
	FILTER		= 'h',
	LANCZOS3	= 'l',
	MITCHEL		= 'm'

} /*IPOLMODE*/;

/*****************************************************************************
funktionname:	ImInterpolationRGB	
description:    Interpolation of source 3 canal image of a given size to a 
				3 canal destination image
				
input:			InIm		- pointer to imput data
				InSize		- dimension of input data in pixel
				OutIm		- pointer of outputdata
				OutSize		- dimension of output data in pixel
				ipm      	- interpolationsmode
return:			TRUE / FALSE
				
note:		
*****************************************************************************/
BOOL  WINAPI ImInterpolationRGB( PIM3BPP InIm, SIZE InSize, PIM3BPP OutIm, SIZE OutSize, int ipm );


/*****************************************************************************
funktionname:	ImInterpolationGray
description:    Interpolation of source 1 canal image of a given size to a 
				1 canal destination image
				
input:			InIm		- pointer to imput data
				InSize		- dimension of input data in pixel
				OutIm		- pointer of outputdata
				OutSize		- dimension of output data in pixel
				ipm      	- interpolationsmode
return:			TRUE / FALSE
				
note:		
*****************************************************************************/
BOOL WINAPI ImInterpolationGray( PBYTE InIm, SIZE InSize, PBYTE OutIm, SIZE OutSize, int ipm );

/*****************************************************************************
funktionname:	ImSharpRGB_B
description:    Sharpen of source 3 canal image of a given size to a 
				3 canal destination image
				
input:			InIm		- pointer to imput data
				InSize		- dimension of input data in pixel
				OutIm		- pointer of outputdata
				OutSize		- dimension of output data in pixel
				
return:			TRUE / FALSE
note:		
*****************************************************************************/
BOOL WINAPI ImSharpRGB(   PIM3BPP   InIm, PIM3BPP   OutIm, SIZE ImSize );

/*****************************************************************************
funktionname:	ImSharpenGray
description:    Sharpen of source 1 canal image of a given size to a 
				1 canal destination image
				
input:			InIm		- pointer to imput data
				InSize		- dimension of input data in pixel
				OutIm		- pointer of outputdata
				OutSize		- dimension of output data in pixel
				
return:			TRUE / FALSE
note:		
*****************************************************************************/
BOOL  WINAPI ImSharpenGray( PBYTE InIm, PBYTE OutIm, SIZE ImSize );



/*****************************************************************************
funktionname:	ImColConBaytoRGB_W10	
description:    Convert 10Bit-RGB-Bayerarrangement to 10Bit-RGB-Arragement
input:			InIm 	 - Pointer to inputdata
				OutIm	 - Pointer to outputdata
				ImSize	 - image dimension in pixel
				ImOffset - offset for conversion algorithm
				ImFlip	 - cy = 1 flip vartikal cy = 1 flip vertical
return:			TRUE/FALSE
				
note:			r11 g12	---> rgb11 rgb12	
				g21 b22	---> rgb21 rgb22	
				with ImOffset it is possible to have a variable start point
				for the conversion
*****************************************************************************/
BOOL  WINAPI ImColConBaytoRGB_W10(  PWORD   InIm, PIM3W10PP OutIm , SIZE ImSize , SIZE ImOffset, SIZE ImFlip);

/*****************************************************************************
funktionname:	ImColConBaytoRGB_W12	
description:    Convert 12Bit-RGB-Bayerarrangement to 12Bit-RGB-Arragement
input:			InIm 	 - Pointer to inputdata
				OutIm	 - Pointer to outputdata
				ImSize	 - image dimension in pixel
				ImOffset - offset for conversion algorithm
				ImFlip	 - cy = 1 flip vartikal cy = 1 flip vertical
return:			TRUE/FALSE
				
note:			r11 g12	---> rgb11 rgb12	
				g21 b22	---> rgb21 rgb22	
				with ImOffset it is possible to have a variable start point
				for the conversion
*****************************************************************************/
BOOL  WINAPI ImColConBaytoRGB_W12(  PWORD   InIm, PIM3W12PP OutIm , SIZE ImSize , SIZE ImOffset, SIZE ImFlip);

/*****************************************************************************
funktionname:	ImColConBaytoRGB08	
description:    Convert 8Bit-RGB-Bayerarrangement to 8Bit-RGB-Arragement
input:			InIm 	 - Pointer to inputdata
				OutIm	 - Pointer to outputdata
				ImSize	 - image dimension in pixel
				ImOffset - offset for conversion algorithm
				ImFlip	 - cy = 1 flip vartikal cy = 1 flip vertical
return:			TRUE/FALSE
				
note:			r11 g12	---> rgb11 rgb12	
				g21 b22	---> rgb21 rgb22	
				with ImOffset it is possible to have a variable start point
				for the conversion algorithm
*****************************************************************************/
BOOL  WINAPI ImColConBaytoRGB_B(  PBYTE   InIm, PIM3BPP OutIm , SIZE ImSize , SIZE ImOffset, SIZE ImFlip);

/*****************************************************************************
funktionname:	ImColRGBW10to8	
description:    Convert 10Bit-RGB-Arrangement to 8Bit-RGB-Arragement
input:			InIm 	 - Pointer to inputdata
				OutIm	 - Pointer to outputdata
				ImSize	 - image dimension in pixel
			
return:			TRUE/FALSE
				
note:			Auxilary function for fast downbreak of 8 bit 
*****************************************************************************/
BOOL  WINAPI ImColRGBW10to8(  PIM3W10PP   InIm, PIM3BPP OutIm , SIZE ImSize );


/*****************************************************************************
funktionname:	ImColRGBW12to8	
description:    Convert 12Bit-RGB-Arrangement to 8Bit-RGB-Arragement
input:			InIm 	 - Pointer to inputdata
				OutIm	 - Pointer to outputdata
				ImSize	 - image dimension in pixel
			
return:			TRUE/FALSE
				
note:			Auxilary function for fast downbreak of 8 bit 
*****************************************************************************/
BOOL  WINAPI ImColRGBW12to8(  PIM3W12PP   InIm, PIM3BPP OutIm , SIZE ImSize );



// Test only
#ifdef  IMLIBAPIEXT
#include "ImLibApiExt.h"
#endif

#ifdef __cplusplus
}
#endif

#endif // _CX_IM_LIBAPI_

