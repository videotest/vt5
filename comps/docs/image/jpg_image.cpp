#include "stdafx.h"
#include "jpg_image.h"

//////////////////////////////////////////////////////////////////////
//
// class jpg_image
//
//////////////////////////////////////////////////////////////////////
jpg_image::jpg_image()
{

}

//////////////////////////////////////////////////////////////////////
jpg_image::~jpg_image()
{

}

//////////////////////////////////////////////////////////////////////
IUnknown* jpg_image::DoGetInterface( const IID &iid )
{
	return CObjectBase::DoGetInterface( iid );
}

//ISerializableObject
/////////////////////////////////////////////////
HRESULT jpg_image::Load( IStream *pStream, SerializeParams *pparams )
{
	ULONG nRead = 0;
	//At first version control
	long nVersion  = 1;
	pStream->Read( &nVersion, sizeof(long), &nRead );

	return S_OK;
}

/////////////////////////////////////////////////
HRESULT jpg_image::Store( IStream *pStream, SerializeParams *pparams )
{
	ULONG nWritten = 0;
	//At first version control
	long nVersion  = 1;
	pStream->Write( &nVersion, sizeof(long), &nWritten );

	return S_OK;
}

//INamedDataObject2
/////////////////////////////////////////////////
HRESULT jpg_image::IsBaseObject( BOOL* pbFlag )
{
	*pbFlag = TRUE;
	return S_OK;
}
