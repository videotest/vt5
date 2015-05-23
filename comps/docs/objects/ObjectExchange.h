#ifndef __object_exchange_h__
#define __object_exchange_h__


//////////////////////////////////////////////////////////////////////
class CActionMapObjects2Image : public CActionBase
{
	DECLARE_DYNCREATE(CActionMapObjects2Image)
	GUARD_DECLARE_OLECREATE(CActionMapObjects2Image)

public:
	CActionMapObjects2Image();
	virtual ~CActionMapObjects2Image();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();

	virtual long get_flags(){ return apfCopyToParent|apfAttachParentData;}
	virtual void after_data_change( IUnknown* punk_image, IUnknown* punk_object_list );
};


//////////////////////////////////////////////////////////////////////
class CActionMakeObjectsVirtual : public CActionMapObjects2Image
{
	DECLARE_DYNCREATE(CActionMakeObjectsVirtual)
	GUARD_DECLARE_OLECREATE(CActionMakeObjectsVirtual)

	virtual long get_flags(){ return sdfAttachParentData;}
	virtual void after_data_change( IUnknown* punk_image, IUnknown* punk_object_list );
};

//////////////////////////////////////////////////////////////////////
class CActionMakeObjectsNonVirtual : public CActionBase
{
	DECLARE_DYNCREATE(CActionMakeObjectsNonVirtual)
	GUARD_DECLARE_OLECREATE(CActionMakeObjectsNonVirtual)

public:
	CActionMakeObjectsNonVirtual();
	virtual ~CActionMakeObjectsNonVirtual();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();

//	virtual long get_flags(){ return apfCopyToParent|apfAttachParentData;}
//	virtual void after_data_change( IUnknown* punk_image, IUnknown* punk_object_list );
};


#endif//__object_exchange_h__