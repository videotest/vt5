#ifndef __idio_object_h__
#define __idio_object_h__

#include "idio_int.h"
#include "atlstr.h"

#include "com_main.h"
#include "data_main.h"
#include "misc_list.h"
#include "object5.h"

static const GUID clsid_idio = 
{ 0xf4675193, 0x8ef1, 0x49d7, { 0xba, 0x91, 0x37, 0xf2, 0x1, 0x7e, 0x1d, 0xa9 } };
// {8848FF19-3F2B-477e-A596-8427EFF8B292}
static const GUID clsid_idio_info = 
{ 0x8848ff19, 0x3f2b, 0x477e, { 0xa5, 0x96, 0x84, 0x27, 0xef, 0xf8, 0xb2, 0x92 } };

#define BAND_NUMBER_COUNT	5
//"top" position is position of centromere
#define BAND_FLAG_CENTROMEREPOS1	1<<0
//"bottom" position is position of centromere
#define BAND_FLAG_CENTROMEREPOS2	2<<0

class idio_info : public CDataInfoBase,
	public _dyncreate_t<idio_info>
{
	route_unknown();
public:
	idio_info();
};

#ifndef _NUM_SIZE
	#define _NUM_SIZE 30
#endif

struct	band
{
	double	pos1, pos2;
	int		color;
	int lLevel;

	char	sz_number[_NUM_SIZE];
	unsigned	flags;

	char A;
	int b, c, d;
	band()
	{
		pos1 = pos2 = 0;
		color = lLevel = 0;

		::ZeroMemory( sz_number , sizeof( char ) * _NUM_SIZE ) ;
		flags = 0;

		A = 0;
		b = c = d = 0;
	}
};

inline void free_band( void *p )
{	delete (band*)p;} 

class idio : public CChildObjectBase,
	public _dyncreate_t<idio>,
	public IIdiogram,
	public IIdioBends,
	public IIdioDrawObject
{
	RECT m_IdioRect; // Used for idio edit
	BOOL m_bShowNums;
	unsigned m_uFlags;
	route_unknown();
public:
	class XRect : public IRectObject
	{
		POINT m_pt;
		RECT m_rcObject;
		idio *m_pMain;
	public:
		RECT &get_rect(){ return m_rcObject; };
		XRect( idio *pMain )
		{
			m_pMain = pMain;
			ZeroMemory( &m_rcObject, sizeof( RECT ) );
			ZeroMemory( &m_pt, sizeof( POINT ) );
		}

		com_call_ref AddRef()
		{	
			return m_pMain->_addref();
		}
		com_call_ref Release()
		{
			return m_pMain->_release();
		}
		com_call QueryInterface( const IID &iid, void **pret )
		{
			return m_pMain->_qi( iid, pret );
		}

		com_call SetRect(RECT rc);
		com_call GetRect(RECT* prc);

		com_call HitTest( POINT	point, long *plHitTest );
		com_call Move( POINT point );
	} m_rectObject;



	virtual GuidKey GetInternalClassID()				{return clsid_idio;};
	virtual IUnknown *DoGetInterface( const IID &iid )	
	{
		if( iid == IID_IIdiogram )
			return (IIdiogram*)this;
		else if( iid == IID_IIdioBends )
			return (IIdioBends*)this;
		else if( iid == IID_IIdioDrawObject )
			return (IIdioDrawObject*)this;
		else if( iid == IID_IRectObject )
			return (IRectObject*)&m_rectObject;
		return CObjectBase::DoGetInterface( iid );
	}

	idio();
	~idio() 
	{
	}
//  IIdiogramm
	com_call draw( HDC hdc, SIZE size, RECT *ptotal, int centromere, unsigned flags, double fZoom );
	com_call open_text( BSTR filename );
	com_call store_text( BSTR filename );	
	com_call clear();
	com_call get_class( long *pclass )					{*pclass=chromo_no;return S_OK;}
	com_call set_class( long lclass )					{ chromo_no = lclass;return S_OK;}
	com_call get_params( double *plen, double *pcentr ) {if(plen)*plen=relative_length;if(pcentr)*pcentr=centromere;return S_OK;}
	com_call set_params( double len, double centr )		{relative_length = len; centromere = centr; return S_OK;}

	com_call get_show_num( BOOL *bShowNum ) { if( bShowNum ) { *bShowNum = m_bShowNums; return S_OK; } return E_FAIL; }
	com_call set_show_num( BOOL bShowNum ) { m_bShowNums = bShowNum; return S_OK; }

	com_call set_flags( unsigned flags );
	com_call get_flags( unsigned *flags );

//  IIdioBend
	com_call Add( double pos1, double pos2, int color, BSTR sz_number, int flags );
	com_call AddAfter( int nID, double pos1, double pos2, int color, BSTR sz_number, int flags );

	com_call SetBendParams( int nID, double pos1, double pos2, int color, BSTR sz_number, int flags );
	com_call GetBendParams( int nID, double *pos1, double *pos2, int *color, BSTR *sz_number, int *flags );

	com_call SetBendFillStyle( int nID, int color );
	com_call GetBendFillStyle( int nID, int *color );

	com_call SetBendRange( int nID, double pos1, double pos2 );
	com_call GetBendRange( int nID, double *pos1, double *pos2 );

	com_call SetBendNumber( int nID, BSTR sz_number );
	com_call GetBendNumber( int nID, BSTR *sz_number );

	com_call SetBendFlag( int nID, int flag );
	com_call GetBendFlag( int nID, int *flag );

	com_call GetBengsCount( long *nSz );
	com_call Remove( int nID );

//IIdioDrawObject
	com_call GetBrush( int nType, HBRUSH *phBrush );
	com_call GetRect( RECT *rc );

//ISerializableObject
	com_call Load( IStream *pStream, SerializeParams *pparams );
	com_call Store( IStream *pStream, SerializeParams *pparams );
public:
	double	relative_length;	//0.0 - 1.0	reltive length (to largest)
	double	centromere;			//0.0 - 0.5 - position of centromere
	long	chromo_no;

	_list_t2<band*, free_band>	bands;
protected:
	TPOS _get_pos_by_id(int nID);
	HRESULT _set_bend_params( TPOS lPos, const double *pos1 = 0, const double *pos2 = 0, const int *color = 0, const BSTR *sz_number = 0, const int *flags = 0 );
	HRESULT _get_bend_params(TPOS lPos, double *pos1 = 0, double *pos2 = 0, int *color = 0, BSTR *sz_number = 0, int *flags = 0);

	HBRUSH _create_brush( int nType);
	void _calc_level( char *sz_number, band *pbend  );
};



#endif //__idio_object_h__