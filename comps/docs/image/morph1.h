#ifndef __morpf1_h__
#define __morpf1_h__

#include "action_filter.h"
#include "image.h"

/////////////////////////////////////////////////////////////////////////////////////////
//morphology filter base class (work with MorpImage
class CMorphFilterBase : public CFilter
{
public:
	CMorphFilterBase();
	~CMorphFilterBase();
public:
	virtual bool Initialize( int nMaskSize );
	void InitCurrentRow( int nRow );
	void MoveCurrentRowRight();
protected:
	IBinaryImagePtr	m_psrc;
	IBinaryImagePtr m_pnew;
	

	byte		**m_ppcurrent;
	int			m_nBodyArea;
	int			m_nBackArea;
	int			m_nTotalArea;

	int			m_nCurrentX;

	int			m_nMaskCenter;	//index of current row in list;
	int			m_nMaskSize;
	int			m_cx, m_cy;
};

/////////////////////////////////////////////////////////////////////////////////////////
//some morphology filters and infos
class CMorphSmooth : public CMorphFilterBase,
					public _dyncreate_t<CMorphSmooth>
{	
	route_unknown();
	virtual bool InvokeFilter();
};

class CMorphSmoothInfo : public _ainfo_t<ID_ACTION_BIN_SMOOTH, _dyncreate_t<CMorphSmooth>::CreateObject, 0>,
					   public _dyncreate_t<CMorphSmoothInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};



#endif // __morpf1_h__