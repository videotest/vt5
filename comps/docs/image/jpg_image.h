#pragma once

#include "resource.h"

#include "data_main.h"
#include "ImageStream_int.h"

//////////////////////////////////////////////////////////////////////
//
// class jpg_image
//
//////////////////////////////////////////////////////////////////////
class jpg_image :	public CObjectBase,
					public _dyncreate_t<jpg_image>
{
	route_unknown();

public:
	virtual IUnknown* DoGetInterface( const IID &iid );

public:
	jpg_image();
	virtual ~jpg_image();

	/////////////////////////////////////////////////
	//IPersist helper
	virtual GuidKey GetInternalClassID(){ return CLSID_ImageStream; }

	/////////////////////////////////////////////////
	//ISerializableObject
	com_call Load( IStream *pStream, SerializeParams *pparams );
	com_call Store( IStream *pStream, SerializeParams *pparams );
	//INamedDataObject2
	com_call IsBaseObject(BOOL * pbFlag);

};

//////////////////////////////////////////////////////////////////////
//
// class jpg_image_info
//
//////////////////////////////////////////////////////////////////////
class jpg_image_info : public CDataInfoBase,
					public _dyncreate_t<jpg_image_info>
{
public:
	jpg_image_info()	: CDataInfoBase( CLSID_ImageStream, szTypeImageStream, 0, IDI_IMAGE_STREAM )
	{	}
};
