#ifndef __ActiveXCtrl_h__
#define __ActiveXCtrl_h__


#include "..\\..\\misc\\EditorView\\axctrlbase.h"


class CActiveXCtrl : public CAXCtrlBase
{
	PROVIDE_GUID_INFO()
	DECLARE_DYNCREATE(CActiveXCtrl);
	GUARD_DECLARE_OLECREATE(CActiveXCtrl);

	CActiveXCtrl();
	virtual ~CActiveXCtrl();

};




class CActiveXFrom : public CAXFormBase
{
	PROVIDE_GUID_INFO()
	DECLARE_DYNCREATE(CActiveXFrom);
	GUARD_DECLARE_OLECREATE(CActiveXFrom);

public:
	CActiveXFrom();
	virtual ~CActiveXFrom();
};



/*

*/
//DataBase BlankForm object

/*
class CBlankFrom : public CActiveXFrom
{
	PROVIDE_GUID_INFO()
	DECLARE_DYNCREATE(CBlankFrom);
	GUARD_DECLARE_OLECREATE(CBlankFrom);

	DECLARE_INTERFACE_MAP();

public:
	CBlankFrom();
	~CBlankFrom();

public:
	virtual bool SerializeObject(CArchive &ar);
	virtual bool CanBeBaseObject();

protected:
	BEGIN_INTERFACE_PART(BlankForm, IBlankForm)
		com_call Stub( );
	END_INTERFACE_PART(BlankForm)
};


class CBlankCtrl : public CActiveXCtrl
{
	DECLARE_DYNCREATE(CBlankCtrl);
	GUARD_DECLARE_OLECREATE(CBlankCtrl);	
public:
	CBlankCtrl();
	~CBlankCtrl();
	virtual bool SerializeObject(CArchive &ar);

};
*/

#endif //__ActiveXCtrl_h__