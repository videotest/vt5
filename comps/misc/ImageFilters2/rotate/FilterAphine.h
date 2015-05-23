// FilterAphine.h: interface for the CFilterAphine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTERAPHINE_H__E46B1F61_1E45_11D5_8F84_008048FD42FE__INCLUDED_)
#define AFX_FILTERAPHINE_H__E46B1F61_1E45_11D5_8F84_008048FD42FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "action_filter.h"
#include "AphineParams.h"	// Added by ClassView
#include "image5.h"
#include "..\\resource.h"

class CFilterAphine : public CFilter  
{
public:
	bool m_fast;
	CAphineParams m_params;
	//double a11,a12,a21,a22; // заменить на матрицу и нормальный доступ
  //double x0,y0,u0,v0; // сделать нормальный доступ

	virtual bool InitAphine() = 0;
	virtual bool InvokeFilter();
	
	CFilterAphine();
	virtual ~CFilterAphine();

	IImage2Ptr pArg,  pResult;
};

class CFilterNewResize : public CFilterAphine, public 
								//[AY] - filter dyncreate
										_dyncreate_t<CFilterNewResize>
{
	//[AY] - route to base class
	route_unknown();
public:
	virtual bool InitAphine();
};

class CFilterNewRotate : public CFilterAphine, public _dyncreate_t<CFilterNewRotate>
{
	route_unknown();
public:
	virtual bool InitAphine();
};




//[AY]
class CFilterNewResizeInfo : 
						  /*action string        dyncreate filter  class                       */
			public _ainfo_t<ID_ACTION_NEWRESIZE, _dyncreate_t<CFilterNewResize>::CreateObject, 0>,
						 /*self dyncreate*/
							public _dyncreate_t<CFilterNewResizeInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CFilterNewRotateInfo : public _ainfo_t<ID_ACTION_NEWROTATE, _dyncreate_t<CFilterNewRotate>::CreateObject, 0>,
							public _dyncreate_t<CFilterNewRotateInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};




#endif // !defined(AFX_FILTERAPHINE_H__E46B1F61_1E45_11D5_8F84_008048FD42FE__INCLUDED_)
