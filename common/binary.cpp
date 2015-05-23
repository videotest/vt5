#include "stdafx.h"
#include "binary.h"
#include "nameconsts.h"



CBinImageWrp::CBinImageWrp( IUnknown *punk /* = 0*/, bool bAddRef /*= true*/ )
{
	if( punk )
		Attach( punk, bAddRef );
}

void CBinImageWrp::Attach( IUnknown *punk, bool bAddRef /*= true*/ )
{
	sptrIBinaryImage	sptrI( punk );
	sptrIBinaryImage::Attach( sptrI, bAddRef );
}

IUnknown *CBinImageWrp::operator =(IUnknown * punk)
{
	Attach( punk );
	return punk;
}

bool CBinImageWrp::CreateNew( int cx, int cy )
{
	if( *this == 0 )
		Attach( ::CreateTypedObject( szTypeBinaryImage ), false );

	return (*this)->CreateNew( cx, cy ) == S_OK;
}

int CBinImageWrp::GetWidth()
{
	int nCX = 0, nCY = 0;
	(*this)->GetSizes( &nCX, &nCY );
	return nCX;
}

int CBinImageWrp::GetHeight()
{
	int nCX = 0, nCY = 0;
	(*this)->GetSizes( &nCX, &nCY );
	return nCY;
}

void CBinImageWrp::Free()
{
	(*this)->Free();
}

