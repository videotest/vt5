#if !defined(__Equalize_H__)
#define __Equalize_H__

#include "action_main.h"
#include "action_interactive.h"
#include "action_undo.h"
#include "action_filter.h"
#include "impl_long.h"
#include "resource.h"
#include "LearningInfo.h"
#include "actions.h"


class CActionChromoEqualize : public CAction, public CUndoneableActionImpl, public _dyncreate_t<CActionChromoEqualize>
{
	IUnknownPtr m_punkObjList;
	bool Process(IUnknown *punkObjList, bool bEqualize);
protected:
	virtual IUnknown *DoGetInterface( const IID &iid );
public:
	route_unknown();
public:
	
	CActionChromoEqualize();
	com_call DoInvoke();
	com_call DoUndo();
	com_call DoRedo();
	com_call GetActionState(DWORD *pdwState);
};


class CActionChromoEqualizeInfo : public _ainfo_t<ID_ACTION_CHROMO_EQUALIZE, _dyncreate_t<CActionChromoEqualize>::CreateObject, 0>,
	public _dyncreate_t<CActionChromoEqualizeInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CActionChromoEqualizeObj : public CObjIntActionBase, public _dyncreate_t<CActionChromoEqualizeObj>
{
	IUnknownPtr m_ptrObj;
protected:
	virtual IUnknown *DoGetInterface( const IID &iid );
public:
	route_unknown();
public:
	CActionChromoEqualizeObj();
	virtual bool DoLButtonDownOnObject( _point point, IUnknown *punkObj, IUnknown *punkObjList );
	com_call DoUndo();
	com_call DoRedo();
	com_call GetActionState(DWORD *pdwState) {return CObjIntActionBase::GetActionState(pdwState);};
	virtual LRESULT	DoMessage(UINT nMsg, WPARAM wParam, LPARAM lParam) {return CObjIntActionBase::DoMessage(nMsg, wParam, lParam);}
};

class CActionChromoEqualizeObjInfo : public _ainfo_t<ID_ACTION_CHROMO_EQUALIZE_OBJ, _dyncreate_t<CActionChromoEqualizeObj>::CreateObject, 0>,
	public _dyncreate_t<CActionChromoEqualizeObjInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};

class CActionChromoUnEqualizeObj : public CObjIntActionBase, public _dyncreate_t<CActionChromoUnEqualizeObj>
{
	IUnknownPtr m_ptrObj;
protected:
	virtual IUnknown *DoGetInterface( const IID &iid );
public:
	route_unknown();
public:
	CActionChromoUnEqualizeObj();
	virtual bool DoLButtonDownOnObject( _point point, IUnknown *punkObj, IUnknown *punkObjList );
	com_call DoUndo();
	com_call DoRedo();
	com_call GetActionState(DWORD *pdwState);
//	com_call GetActionState(DWORD *pdwState) {return CObjIntActionBase::GetActionState(pdwState);};
	virtual LRESULT	DoMessage(UINT nMsg, WPARAM wParam, LPARAM lParam) {return CObjIntActionBase::DoMessage(nMsg, wParam, lParam);}
};

class CActionChromoUnEqualizeObjInfo : public _ainfo_t<ID_ACTION_CHROMO_UNEQUALIZE_OBJ, _dyncreate_t<CActionChromoUnEqualizeObj>::CreateObject, 0>,
	public _dyncreate_t<CActionChromoUnEqualizeObjInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};


#endif


