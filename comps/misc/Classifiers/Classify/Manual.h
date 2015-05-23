#if !defined(__Manual_H__)
#define __Manual_H__

#include "Actions.h"

class CActionManClassify :	public CObjIntActionBase, public CLongOperationImpl,
	public _dyncreate_t<CActionManClassify >
{
	long m_lClass;
	int  m_nManualClass;
	ICalcObjectPtr m_sptrObj;
	IUnknownPtr m_ptrObjList; // For redraw after undo/redo
	void ChangeClass();
	_point ToApplication(_point pt);
	_rect  ToApplication(_rect rc);
protected:
	virtual IUnknown *DoGetInterface( const IID &iid );
public:
	route_unknown();
public:
	CActionManClassify();
	com_call DoInvoke();
	virtual bool DoLButtonDownOnObject( _point point, IUnknown *punkObj, IUnknown *punkObjList );
	com_call DoUndo();
	com_call DoRedo();
	com_call GetActionState(DWORD *pdwState) {return CObjIntActionBase::GetActionState(pdwState);};
	virtual LRESULT	DoMessage(UINT nMsg, WPARAM wParam, LPARAM lParam) {return CObjIntActionBase::DoMessage(nMsg, wParam, lParam);}
};

class CActionManClassifyInfo : public _ainfo_t<ID_ACTION_MANUAL_CLASSIFY, _dyncreate_t<CActionManClassify>::CreateObject, 0>,
	public _dyncreate_t<CActionManClassifyInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};

void RegisterManualClass();

#endif


