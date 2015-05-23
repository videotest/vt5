#include "DataBook.h"

//////////////////////////////////////////////////////////////////////
class CDataBook : public CObjectBase,
				public _dyncreate_t<CDataBook>
{
	route_unknown();

public:

	//com_call IsModified( BOOL *pbModified );

	//IPersist helper
	virtual GuidKey GetInternalClassID(){ return clsidDataBook; }

	/*
	virtual IUnknown *DoGetInterface( const IID &iid )
	{
		if( iid == IID_IDataBook )return (IDataBook*)this;
		else return CObjectBase::DoGetInterface( iid );
	}
	*/

public:	
	com_call AttachData( IUnknown *punkNamedData );	//called from NamedData

public:
	CDataBook();
	virtual ~CDataBook();

	//ISerializableObject
	//com_call Load( IStream *pStream, SerializeParams *pparams );
	//com_call Store( IStream *pStream, SerializeParams *pparams );
	//INamedDataObject2
	//com_call IsBaseObject(BOOL * pbFlag);
};

class CDataBookInfo : public CDataInfoBase,
					public _dyncreate_t<CDataBookInfo>
{
public:
	CDataBookInfo()	: CDataInfoBase( clsidDataBook, szTypeDataBookProgId, 0, IDI_DATABOOKIMAGE )
	{	}

	com_call GetUserName( BSTR *pbstr );
};

