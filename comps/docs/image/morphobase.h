#ifndef __MORPHOBASE_H__
#define __MORPHOBASE_H__

#include "action_filter.h"
#include "binimageint.h"
#include "image.h"

typedef unsigned __int64 DDWORD;

const long g_nMaxMaskSize = 32;

class CDCProvider
{
public:
	CDCProvider(long cx, long cy);
	~CDCProvider();

	HDC GetDC(){return m_dc;};
	byte* GetBits(long& nRowSize){nRowSize = m_cx4; return m_pbits;};

private:
	long m_cx;
	long m_cy;
	long m_cx4;
	HDC  m_dc;
	byte* m_pbits;
	HBITMAP m_hBmp;
	HBITMAP m_hBmpOld;
};


class CMorphoFilter : public CFilter
{
public:
	CMorphoFilter();
	virtual ~CMorphoFilter();
	
protected:
	bool _InitMorpho();
	bool _InitMorpho2();
	void _DeInitMorpho(bool bApply=true); // освобождение вспомогательных переменных, если bApply - то данные из них отправл€ютс€ в результат акции и делаетс€ FinishNotification()
	// с false можно вызывать сколько угодно раз подр€д
	void _CreateMask();
	void _DoMorphology(bool bErode, CBinaryBitImage *pbinDst=0, CBinaryBitImage *pbinSrc=0);
	int GetHeight(){return m_cy;};
	CBinaryBitImage*	m_pbinDst;
	CBinaryBitImage*	m_pbinSrc;
	CBinaryBitImage*	m_pbinIntr;

private:
	int m_cx, m_cy;
	long m_nMaskSize;
	DDWORD* m_pnMask;
	DDWORD* m_pnWorkingMask;
};

#endif //__MORPHOBASE_H__