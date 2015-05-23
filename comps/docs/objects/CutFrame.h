#if !defined(__CutFrame_H__)
#define __CutFrame_H__

class CActionCutFrameBase : public CFilterBase
{
public:
	virtual void OnProcessObject(bool bContact, IUnknown *punkObjSrc, IUnknown *punkObjLstDst) = 0;
	virtual bool CalcCutArea() {return false;}
	virtual bool InvokeFilter();
};

class CActionCutFrame : public CActionCutFrameBase
{
	DECLARE_DYNCREATE(CActionCutFrame)
	GUARD_DECLARE_OLECREATE(CActionCutFrame)
public:
	CActionCutFrame();
	virtual ~CActionCutFrame();
	virtual void OnProcessObject(bool bContact, IUnknown *punkObjSrc, IUnknown *punkObjLstDst);
	virtual bool CalcCutArea() {return true;}
};

class CActionFindBorderObjects : public CActionCutFrameBase
{
	DECLARE_DYNCREATE(CActionFindBorderObjects)
	GUARD_DECLARE_OLECREATE(CActionFindBorderObjects)
public:
	CActionFindBorderObjects();
	virtual ~CActionFindBorderObjects();
	virtual void OnProcessObject(bool bContact, IUnknown *punkObjSrc, IUnknown *punkObjLstDst);
};



#endif

