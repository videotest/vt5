#pragma once

#include "statistics.h"
#include "measure5.h"

#include "PropBagImpl.h"

#include "\vt5\awin\misc_list.h"
#include "\vt5\awin\misc_map.h"

#include "\vt5\common\measure_misc.h"


//////////////////////////////////////////////////////////////////////
//
//	class stat_value_ex
//
//////////////////////////////////////////////////////////////////////
class stat_value_ex : public stat_value
{
public:
	stat_value_ex();
	virtual ~stat_value_ex();
	
	long	m_lkey;
};

//////////////////////////////////////////////////////////////////////
inline void stat_value_ex_free( void* p )
{
	delete (stat_value_ex*)p;
}

typedef _list_map_t<stat_value_ex*, long/*key*/, cmp_long, stat_value_ex_free>	map_value;


//////////////////////////////////////////////////////////////////////
//
//	class stat_row_ex
//
//////////////////////////////////////////////////////////////////////
class stat_row_ex : public ComObjectBase,
					public stat_row,
					public ICalcObject,
					public CNamedPropBagImpl,
					public CNumeredObjectImpl
					
{
	route_unknown();

public:
	stat_row_ex();
	virtual ~stat_row_ex();

	virtual IUnknown*	DoGetInterface( const IID &iid );

	//ICalcObject
	com_call GetValue(long lParamKey, double *pfValue);
	com_call SetValue(long lParamKey, double fValue);
	com_call RemoveValue(long lParamKey){	return E_NOTIMPL; }
	com_call ClearValues(){	return E_NOTIMPL; }

	com_call GetFirstValuePos(long * plPos)	{	return E_NOTIMPL; }
	com_call GetNextValue(long * plPos, long * plKey, double * pfValue){	return E_NOTIMPL; }
	com_call GetValuePos(long lKey, long * plPos){	return E_NOTIMPL; }
	com_call RemoveValueByPos(long lPos){	return E_NOTIMPL; }

	//data
	map_value	m_map_value;
};

//////////////////////////////////////////////////////////////////////
inline void stat_row_ex_free( void* p )
{
	stat_row_ex* prow = (stat_row_ex*)p;
	prow->Release();
}

typedef _list_t<stat_row_ex*, stat_row_ex_free>	list_row;


//////////////////////////////////////////////////////////////////////
//
//	class stat_param_ex
//
//////////////////////////////////////////////////////////////////////
class stat_param_ex : public stat_param
{
public:
	stat_param_ex();
	virtual ~stat_param_ex();
};

inline void stat_param_ex_free( void* pdata )
{
	delete (stat_param_ex*)pdata;
};

typedef _list_t<stat_param_ex*, stat_param_ex_free>			list_param;
typedef _list_map_t<TPOS/*lpos*/, long/*key*/, cmp_long>	map_param;

//////////////////////////////////////////////////////////////////////
//
//	class stat_group_ex
//
//////////////////////////////////////////////////////////////////////
class stat_group_ex : public stat_group
{
public:
	stat_group_ex();
	virtual ~stat_group_ex();
};

inline void stat_group_ex_free( void* pdata )
{
	delete (stat_group_ex*)pdata;
};

typedef _list_t<stat_group_ex*, stat_group_ex_free>	list_group;

//stat_params helpers
//////////////////////////////////////////////////////////////////////
void init_stat_param( stat_param* pparam );
void copy_stat_param( stat_param* psrc, stat_param* ptarget );

//stat_row helpers
//////////////////////////////////////////////////////////////////////
void init_stat_row( stat_row* prow );
void copy_stat_row( stat_row* psrc, stat_row* ptarget );
void release_stat_row( stat_row* prow );

//stat_value helpers
//////////////////////////////////////////////////////////////////////
void init_stat_value( stat_value* pvalue );
void copy_stat_value( stat_value* psrc, stat_value* ptarget );

//stat_group helpres
//////////////////////////////////////////////////////////////////////
void init_stat_group( stat_group* pgroup );
void copy_stat_group( stat_group* psrc, stat_group* ptarget );
void release_stat_group( stat_group* pgroup );




//////////////////////////////////////////////////////////////////////
//
//	class stat_col_info
//
//////////////////////////////////////////////////////////////////////
class stat_col_info
{
public:
	stat_col_info();
	virtual ~stat_col_info();

	long	m_width;	
	bool	m_bvisible;
	long	m_order;
	long	m_lkey;	

	bool	load( INamedData* pi_nd );
	bool	save( INamedData* pi_nd ) const;
};

inline void stat_col_info_free( void* pdata )
{
	delete (stat_col_info*)pdata;
};
