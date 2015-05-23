#if !defined(__DilateObjects_H__)
#define __DilateObjects_H__

#include "action_filter.h"
#include "binimageint.h"
#include "image.h"
#include "resource.h"

class CBinDilateObjects : public CFilter,
			   	  public _dyncreate_t<CBinDilateObjects>
{
public:
	route_unknown();
public:
	
	CBinDilateObjects();
	virtual bool InvokeFilter();
};


class CBinDilateObjectsInfo : public _ainfo_t<ID_ACTION_BIN_DILATE_OBJECTS, _dyncreate_t<CBinDilateObjects>::CreateObject, 0>,
					   public _dyncreate_t<CBinDilateObjectsInfo>
{
	route_unknown();
public:
	CBinDilateObjectsInfo()
	{
		return;
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};







#endif
