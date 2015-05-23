#ifndef __idio_int_h__
#define __idio_int_h__

#include "defs.h"

#ifndef com_call
#define com_call		virtual HRESULT __stdcall
#endif //com_call


const char sz_idio_type[] = "Idiogram";
// {F4675193-8EF1-49d7-BA91-37F2017E1DA9}

#define IDF_IGNORECENTROMERE	( 1 << 0 )
#define IDF_IGNORELENGTH		( 1 << 1 )
#define IDF_CALCRECT			( 1 << 2 )
#define IDF_BLUE				( 1 << 3 )
#define IDF_NOTEXT				( 1 << 4 )
#define IDF_RED					( 1 << 5 )


#define IDIC_BLACK   0
#define IDIC_OTHER   1
#define IDIC_DCROSS  2
#define IDIC_CROSS   3
#define IDIC_LT      4
#define IDIC_GR      5
#define IDIC_DK      6
#define IDIC_LINK    7

#define IDIC_WHITE   9 // Change this number then you'll add new bend's color. IDIC_WHITE must have last number (not 0xFF)


#define IDIB_CENTROMERE 0x100
#define IDIB_SPUTNIC	0x200
#define IDIB_LINK		0x150

#define IDIB_SIMPLE		0x0

#define IDIT_INSERTED  0
#define IDIT_EDITABLE  1

interface IIdiogram : public IUnknown
{
	com_call draw( HDC hdc,			//device context for draw 
						SIZE size,		//size (width and height of maximal-length idiogramm (not zoomed)) 
						RECT *ptotal,		//place to paint (in window coordinates)
						int	centromere,	//relative position of centromere (total.top+centromere)
						unsigned flags,	//flags (one idea is direction (horizontal or vertical))
						double fZoom) = 0;
	com_call open_text( BSTR filename ) = 0;
	com_call store_text( BSTR filename ) = 0;
	com_call clear() = 0;
	com_call set_class( long lclass ) = 0;
	com_call get_class( long *pclass ) = 0;
	com_call get_params( double *plen, double *pcentr ) = 0;
	com_call set_params( double len, double centr ) = 0;

	com_call set_flags( unsigned flags ) = 0;
	com_call get_flags( unsigned *flags ) = 0;

	com_call get_show_num( BOOL *bShowNum ) = 0;
	com_call set_show_num( BOOL bShowNum ) = 0;
};

interface IIdioBends : public IUnknown
{
	com_call Add( double pos1, double pos2, int color, BSTR sz_number, int flags ) = 0;
	com_call AddAfter( int nID, double pos1, double pos2, int color, BSTR sz_number, int flags ) = 0;
	com_call Remove( int nID ) = 0;

	com_call SetBendParams( int nID, double pos1, double pos2, int color, BSTR sz_number, int flags ) = 0;
	com_call GetBendParams( int nID, double *pos1, double *pos2, int *color, BSTR *sz_number, int *flags ) = 0;

	com_call SetBendFillStyle( int nID, int color ) = 0;
	com_call GetBendFillStyle( int nID, int *color ) = 0;

	com_call SetBendRange( int nID, double pos1, double pos2 ) = 0;
	com_call GetBendRange( int nID, double *pos1, double *pos2 ) = 0;

	com_call SetBendNumber( int nID, BSTR sz_number ) = 0;
	com_call GetBendNumber( int nID, BSTR *sz_number ) = 0;

	com_call SetBendFlag( int nID, int flag ) = 0;
	com_call GetBendFlag( int nID, int *flag ) = 0;

	com_call GetBengsCount( long *lSz ) = 0;
};

interface IIdioDrawObject : public IUnknown
{
	com_call GetBrush( int nType, HBRUSH *phBrush ) = 0;
	com_call GetRect( RECT *rc ) = 0;
};

declare_interface(IIdiogram, "9FE14624-4426-4d64-9014-F2FB57FCA5F3")
declare_interface(IIdioBends, "65C8E8D5-A8D7-48ac-8C13-90B21F800FAD")
declare_interface(IIdioDrawObject, "36A095E6-C6A8-4610-A107-A5797F8C955A");


#endif //__idio_int_h__