#ifndef __actions_h__
#define __actions_h__

#include "action_main.h"
#include "action_interactive.h"
#include "action_undo.h"
#include "action_filter.h"
#include "impl_long.h"
#include "resource.h"
#include "LearningInfo.h"
#include "CalibrInt.h"


void RedrawDocument(IUnknown *punkDoc);
void RedrawContext(IUnknown *punkView);
void RedrawObjectList(IUnknown *punk);


class CObjIntActionBase : public CInteractiveAction, public CUndoneableActionImpl
{
protected:
	int m_nFoundObjNo;
	virtual IUnknown *DoGetInterface( const IID &iid );
	IUnknownPtr GetFoundObject();
	virtual bool CheckTracking() {return false;}
	bool _DoLButtonDownOnObject( _point point, long &posObj, INamedDataObject2 *pNDO2, bool bSetActive, int iObjNo);
public:
	com_call DoInvoke();
	virtual bool DoLButtonDown( _point point );
	virtual bool ActivateObject() {return true;}
	virtual bool DoLButtonDownOnObject( _point point, IUnknown *punkObj, IUnknown *punkObjList ) {return false;}
	com_call GetActionState(DWORD *pdwState);
	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );
};

class CActionClassify :	public CAction,	public CLongOperationImpl, public CUndoneableActionImpl,
	public _dyncreate_t<CActionClassify>
{
	IUnknownPtr m_punkObjList;
	_ptr_t2<int> m_arrClasses;
protected:
	virtual IUnknown *DoGetInterface( const IID &iid );
public:
	route_unknown();
public:
	
	CActionClassify();
	com_call DoInvoke();
	com_call DoUndo();
	com_call DoRedo();
	com_call GetActionState(DWORD *pdwState);
};

class CActionClassifyInfo : public _ainfo_t<ID_ACTION_CLASSIFY, _dyncreate_t<CActionClassify>::CreateObject, 0>,
	public _dyncreate_t<CActionClassifyInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


class CActionTeach:	public CAction,	public CLongOperationImpl, public CUndoneableActionImpl,
	public _dyncreate_t<CActionTeach>
{
public:
	route_unknown();
public:
	
	CActionTeach();
	com_call DoInvoke();
};

class CActionTeachInfo : public _ainfo_t<ID_ACTION_TEACH, _dyncreate_t<CActionTeach>::CreateObject, 0>,
	public _dyncreate_t<CActionTeachInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "app";}
	static arg	s_pargs[];
};

class CActionEmptyClassifier:	public CAction,	public CLongOperationImpl, public CUndoneableActionImpl,
	public _dyncreate_t<CActionEmptyClassifier>
{
	CLearningInfoState m_LearningInfoState;
protected:
	virtual IUnknown *DoGetInterface( const IID &iid );
public:
	route_unknown();
public:
	
	CActionEmptyClassifier();
	com_call DoInvoke();
	com_call DoUndo();
	com_call DoRedo();
};

class CActionEmptyClassifierInfo : public _ainfo_t<ID_ACTION_EMPTY, _dyncreate_t<CActionEmptyClassifier>::CreateObject, 0>,
	public _dyncreate_t<CActionEmptyClassifierInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "app";}
	static arg	s_pargs[];
};

class CActionAddObject : public CObjIntActionBase, public CLongOperationImpl,
	public _dyncreate_t<CActionAddObject>
{
	CLearningInfoState m_LearningInfoState;
protected:
	virtual IUnknown *DoGetInterface( const IID &iid );
public:
	route_unknown();
public:
	
	CActionAddObject();
	com_call DoInvoke();
//	virtual bool DoLButtonDown( _point point );
	virtual bool DoLButtonDownOnObject( _point point, IUnknown *punkObj, IUnknown *punkObjList );
	com_call DoUndo();
	com_call DoRedo();
	com_call GetActionState(DWORD *pdwState) {return CObjIntActionBase::GetActionState(pdwState);};
	virtual LRESULT	DoMessage(UINT nMsg, WPARAM wParam, LPARAM lParam) {return CObjIntActionBase::DoMessage(nMsg, wParam, lParam);}
};

class CActionAddObjectInfo : public _ainfo_t<ID_ACTION_ADD_OBJECT, _dyncreate_t<CActionAddObject>::CreateObject, 0>,
	public _dyncreate_t<CActionAddObjectInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};

class CActionAddAllObjects : public CAction, public CLongOperationImpl, public CUndoneableActionImpl,
	public _dyncreate_t<CActionAddAllObjects>
{
	CLearningInfoState m_LearningInfoState;
protected:
	virtual IUnknown *DoGetInterface( const IID &iid );
public:
	route_unknown();
public:
	
	CActionAddAllObjects();
	com_call DoInvoke();
	com_call DoUndo();
	com_call DoRedo();
	com_call GetActionState(DWORD *pdwState);
};

class CActionAddAllObjectsInfo : public _ainfo_t<ID_ACTION_ADD_ALL, _dyncreate_t<CActionAddAllObjects>::CreateObject, 0>,
	public _dyncreate_t<CActionAddAllObjectsInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


#endif //__actions_h__