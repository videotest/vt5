#ifndef __image_h__
#define __image_h__

#include "data5.h"
//#include "image5.h"
#include "data_main.h"
#include "binimageint.h"
#include "resource.h"
#include "image_app.h"
#include "nameconsts.h"
#include "calibrint.h"
#include "object5.h"
#include "\vt5\ifaces\binimageint.h"
#include "\vt5\ifaces\BinaryImage_int.h"

class CBinaryImage : public CObjectBase,
			public _dyncreate_t<CBinaryImage>,
			public IBinaryImage,
			public ICalibr,
			public IRectObject,
			public IBinaryImageDisp
{
	route_unknown();
public:
	CBinaryImage();
	virtual ~CBinaryImage();

	virtual IUnknown *DoGetInterface( const IID &iid )
	{
		if( iid == IID_IBinaryImage )return (IBinaryImage*)this;
		else if( iid == IID_ICalibr )return (ICalibr*)this;
		else if( iid == IID_IRectObject )return (IRectObject*)this;
		else if( iid == IID_IDispatch )
			return (IDispatch*)this;
		else return CObjectBase::DoGetInterface( iid );
	}

	//IPersist helper
	virtual GuidKey GetInternalClassID(){ return clsidBinImage; }

public:
	com_call CreateNew( int cx, int cy );
	com_call GetRow( byte **prow, int nRow, BOOL bWithDWORDBorders );
	com_call GetSizes( int *pcx, int *pcy );
	com_call Free();
	com_call GetOffset( POINT *pt );
	com_call SetOffset( POINT pt, BOOL bMoveImage );

	//ISerializableObject
	com_call Load( IStream *pStream, SerializeParams *pparams );
	com_call Store( IStream *pStream, SerializeParams *pparams );
	//INamedDataObject2
	com_call IsBaseObject(BOOL * pbFlag);

	com_call GetCalibration( double *pfPixelPerMeter, GUID *pguidC );
	com_call SetCalibration( double fPixelPerMeter, GUID *guidC );


	com_call SetRect(RECT rc);
	com_call GetRect(RECT* prc);

	com_call HitTest( POINT	point, long *plHitTest );
	com_call Move( POINT point );

	//IBinaryImageDisp
	com_call get_Name(BSTR *pbstr );
	com_call put_Name(BSTR bstr );
	com_call GetType( /*[out, retval]*/ BSTR *pbstrType );

protected:
	int		m_cx, m_cy, m_cx4;
	POINT	m_ptOffset;
	byte**	m_ppRows;
	GUID	m_guidC;
	double	m_fCalibr;

	/////////////////////////////////////////////////
	//Dispatch
    com_call GetTypeInfoCount( 
            /* [out] */ UINT *pctinfo);
	com_call GetTypeInfo( 
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
	com_call GetIDsOfNames( 
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
	com_call Invoke( 
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
	ITypeLib*	m_pi_type_lib;
	ITypeInfo*	m_pi_type_info;
};

class CBinaryImageInfo : public CDataInfoBase,
					public _dyncreate_t<CBinaryImageInfo>
{
public:
	CBinaryImageInfo()	: CDataInfoBase( clsidBinImage, szTypeBinaryImage, 0, IDI_BINIMAGE )
	{	}
};




/////////////////////////////////////////////////////////////////////////////////////////
//wrapper for bit operations 
class CBinaryBitImage : public ComObjectBase,
					              public IBinaryImage
{
	route_unknown();
public:
	CBinaryBitImage();
	~CBinaryBitImage();

public:
	com_call CreateNew( int cx, int cy );
	com_call Free();
	com_call GetSizes( int *pcx, int *pcy );
	com_call GetRow( byte **prow, int nRow, BOOL bWithDWORDBorders );
	com_call GetOffset( POINT *pt );
	com_call SetOffset( POINT pt, BOOL bMoveImage );

	com_call InitFromBin( IBinaryImage *pbin ); //convert to bits
	com_call StoreToBin ( IBinaryImage *pbin ); //convert to bytes
	
	void ClearBorders(); // чистит то, что за краями (справа и слева за пределами изображения)
	void FillBorders(); // забивает единицами то, что за краями
	void ExpandBorders(); // забивает краевым битом
protected:
	int		m_cx, m_cy;
	POINT	m_ptOffset;
	byte**	m_ppRows;
};


#endif //__image_h__