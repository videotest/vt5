#ifndef __ObjUser_H__
#define __ObjUser_H__

#include "ObjMease.h"

class CObjUser: public CObjMeasure
{
	DECLARE_SERIAL(CObjUser);
public:
	CObjNative *Clone();
	virtual void DoMeasure();
	virtual double GetVal(){return 0;}
	virtual BOOL GetObjType(){return OBJ_USERCALC;};
};

class CObjCircle : public CObjUser
{
	DECLARE_SERIAL(CObjCircle);
public:
	CObjNative *Clone();
};

class CObjFree : public CObjUser
{
	DECLARE_SERIAL(CObjFree);
public:
	CObjNative *Clone();
};


class CObjRect : public CObjUser
{
	DECLARE_SERIAL(CObjRect);
public:
	CObjRect();
	CObjNative *Clone();
	virtual void DoMeasure();
	void CreateBin();
	virtual BOOL IsPermanentAngle() {return TRUE;}
};



#endif //__ObjUser_H__