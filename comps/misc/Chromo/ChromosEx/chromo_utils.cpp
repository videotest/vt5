#include "stdafx.h"
#include "chromo_utils.h"
#include "carioint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//alpha blend
//r = ( r1 * l + r2 * ( 255 - l ) ) / 255;
//r = r1 * r2 / 255;

bool correct_layer( BITMAPINFOHEADER* pbi, byte* pdib, CRect rcFill, COLORREF clrMask, bool bMirror )
{
	if( !( pbi && pdib && pbi->biBitCount == 24 ) )
		return false;

	if( rcFill.left < 0 )
		rcFill.left = 0;

	if( rcFill.top < 0 )
		rcFill.top = 0;

	if( rcFill.right > pbi->biWidth )
		rcFill.right = pbi->biWidth;

	if( rcFill.bottom > pbi->biHeight )
		rcFill.bottom = pbi->biHeight;

	BYTE rMask = GetRValue( clrMask );
	BYTE gMask = GetGValue( clrMask );
	BYTE bMask = GetBValue( clrMask );


	int cx	= pbi->biWidth;
	int cy	= pbi->biHeight;
	int	cx4	= (cx*3+3)/4*4;

	for( int _y=rcFill.top;_y<rcFill.bottom;_y++ )
	{
		int y = cy - _y - 1;
		long nRowOffset = ( y ) * cx4;
		for( int x=rcFill.left;x<rcFill.right;x++ )
		{
			byte bImg = pdib[ nRowOffset + 3 * x ];
			byte gImg = pdib[ nRowOffset + 3 * x + 1 ];
			byte rImg = pdib[ nRowOffset + 3 * x + 2 ];

			pdib[ nRowOffset + 3 * x + 0 ] = bImg * bMask / 255;
			pdib[ nRowOffset + 3 * x + 1 ] = gImg * gMask / 255;
			pdib[ nRowOffset + 3 * x + 2 ] = rImg * rMask / 255;
		}
	}			

	return true;
}


bool DrawMirror( BITMAPINFOHEADER* pbi, byte* pdib, CRect rcFill, COLORREF clrMirror )
{	
	return correct_layer( pbi, pdib, rcFill, clrMirror, true );
}

bool DrawAlphaBlend( BITMAPINFOHEADER* pbi, byte* pdib, CRect rcFill, COLORREF clrAlphaBlend )
{
	return correct_layer( pbi, pdib, rcFill, clrAlphaBlend, false );
}


//classes
CString GetClassFileNameFromObjectList( IUnknown* punk_object_list )
{
	return ::GetValueString( punk_object_list, CARIO_ROOT, CLASS_FILE_NAME, "" );	
}

bool SetClassFileNameToObjectList( IUnknown* punk_object_list, const char* psz_class_file_name )
{
	::SetValue( punk_object_list, CARIO_ROOT, CLASS_FILE_NAME, psz_class_file_name );
	return true;
}

CString	GetCurrentClassFileName()
{
	CString str = ::GetValueString( ::GetAppUnknown(), "\\Classes", "ClassFile", "" );	

	int nidx = str.ReverseFind( '\\' );
	if( nidx == -1 )
		return "";

	int nlen = str.GetLength();
	if( nlen <= nidx )
		return "";

	return str.Right( nlen - nidx - 1 );
}


