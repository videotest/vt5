#ifndef __Named2DArrays_h__
#define __Named2DArrays_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "action_filter.h"
#include "image5.h"
#include "docview5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "binimageint.h"
//#include "ObjectPool.h"

/////////////////////////////////////////////////////////////////////////////

class CNamed2DArrays
{
public:
	CNamed2DArrays();
	virtual ~CNamed2DArrays();
	int GetObjectStep(); //получить шаг в массиве между объектами
	int GetFrameStep(); //получить шаг в массиве между кадрами
	// элемент = *(GetArray + GetFrameStep*frame + GetObjectStep*object)

	bool SetArraySize(int nMaxFrameNo, int nMaxObjectNo, int nMaxItemSize);
	void GetArraySize(int *pnMaxFrameNo, int *pnMaxObjectNo, int *pnMaxItemSize);
	bool AddArray(const char *pszArrName, int nElementSize);

	byte *GetExistingArray(const char *pszArrName); //получить массив
	byte *GetArray(const char *pszArrName, int nElementSize);

	bool AttachArrays(IUnknown *punkObj);
	
private:
	IUnknownPtr m_punkObj;
};

template <class T> class _byte_ptr
{
public:
	byte *m_ptr;
	_byte_ptr(byte *p=0) { m_ptr=p; }
	T &operator[](int i) { return *(T*)(m_ptr+i); }
	_byte_ptr<T> operator+(int i) { return _byte_ptr<T>(m_ptr+i); }
	_byte_ptr<T> operator-(int i) { return _byte_ptr<T>(m_ptr-i); }
	void operator+=(int i) { m_ptr += i; }
	void operator-=(int i) { m_ptr -= i; }
	operator void*() { return m_ptr; }
};

template <class T> class _multi_byte_ptr
{
public:
	byte *m_ptr;
	int m_nStep;
	_multi_byte_ptr(byte *p=0, int nStep=0) { m_ptr=p; m_nStep=nStep; }
	_multi_byte_ptr(_byte_ptr p, int nStep) { m_ptr=p.m_ptr; m_nStep=nStep; }
	T &operator[](int i) { return *(T*)(m_ptr+i*m_nStep); }
	_multi_byte_ptr<T> operator+(int i) { return _multi_byte_ptr<T>(m_ptr+i*m_nStep,m_nStep); }
	_multi_byte_ptr<T> operator-(int i) { return _multi_byte_ptr<T>(m_ptr-i*m_nStep,m_nStep); }
	void operator+=(int i) { m_ptr += i*m_nStep; }
	void operator-=(int i) { m_ptr -= i*m_nStep; }
	operator void*() { return m_ptr; }
};

#endif //__Named2DArrays_h__
