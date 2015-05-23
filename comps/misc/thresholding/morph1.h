#ifndef __morpf1_h__
#define __morpf1_h__

#include "action_filter.h"
#include "binimageint.h"

/////////////////////////////////////////////////////////////////////////////////////////
//morpfology image - image with bytes data
class CMorphImage : public ComObjectBase,
					public IBinaryImage
{
	route_unknown();
public:
	CMorphImage();
	~CMorphImage();

public:
	com_call CreateNew( int cx, int cy );
	com_call Free();
	com_call InitFromBin( IBinaryImage *pbin );
	com_call StoreToBin( IBinaryImage *pbin );
	com_call GetSizes( int *pcx, int *pcy );
	com_call GetRow( byte **prow, int nRow, BOOL bWithDWORDBorders );
	com_call GetOffset( POINT *pt );
	com_call SetOffset( POINT pt, BOOL bMoveImage );
	
protected:
	byte	*m_pbin;
	int		m_cx, m_cy, m_cx4;
	int		m_x, m_y;
};

#endif // __morpf1_h__