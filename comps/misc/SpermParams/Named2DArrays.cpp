#include "stdafx.h"
//#include "AddFrame.h"
#include "math.h"
#include <stdio.h>
#include "measure5.h"
#include "alloc.h"
#include "Named2DArrays.h"

CNamed2DArrays::CNamed2DArrays()
{
	m_punkObj=0;
}

CNamed2DArrays::~CNamed2DArrays()
{
	m_punkObj=0;
}

bool CNamed2DArrays::AttachArrays(IUnknown *punkObj)
{
	INamedDataObject2Ptr ptrObj=punkObj;
	if(ptrObj==0) { m_punkObj=0; return false; }
	INamedDataPtr sptrNDO = punkObj;
	if( sptrNDO == 0 ) ptrObj->InitAttachedData();
	m_punkObj=punkObj;
	return true;
}

bool CNamed2DArrays::SetArraySize(int nMaxFrameNo, int nMaxObjectNo, int nMaxItemSize)
{
	if(m_punkObj==0) return false;
	int nOldMaxFrameNo = ::GetValueInt(m_punkObj, "\\particles", "MaxFrameNo", 0);
	int nOldMaxObjectNo = ::GetValueInt(m_punkObj, "\\particles", "MaxObjectNo", 0);
	int nOldMaxItemSize = ::GetValueInt(m_punkObj, "\\particles", "MaxItemSize", 0);

	if(nMaxFrameNo<0) nMaxFrameNo=nOldMaxFrameNo; // <0 - сохранить старое
	if(nMaxObjectNo<0) nMaxObjectNo=nOldMaxObjectNo; // <0 - сохранить старое
	if(nMaxItemSize<0) nMaxItemSize=nOldMaxItemSize; // <0 - сохранить старое

	if(nOldMaxFrameNo==nMaxFrameNo && nOldMaxObjectNo==nMaxObjectNo && nOldMaxItemSize==nMaxItemSize)
		return true; //размеры не изменились - ничего делать не надо.

	nMaxFrameNo=max(nMaxFrameNo,nOldMaxFrameNo); // никогда не уменьшать массив
	nMaxFrameNo=max(nMaxObjectNo,nOldMaxObjectNo);
	nMaxFrameNo=max(nMaxItemSize,nOldMaxItemSize);

	if(	nMaxFrameNo==nOldMaxFrameNo && 
		nMaxObjectNo==nOldMaxObjectNo &&
		nMaxItemSize==nOldMaxItemSize) return false; // ничего не надо делать
	
	long lSize=nOldMaxFrameNo*nOldMaxObjectNo*nOldMaxItemSize;
	long lNewSize=nMaxFrameNo*nMaxObjectNo*nMaxItemSize;

	byte *pNewData = new byte[sizeof( lNewSize )+lNewSize];
	*(long*)pNewData = lNewSize;
	memset( pNewData+sizeof(lNewSize), 0 , lNewSize );

	_variant_t var = ::GetValue(m_punkObj, "\\particles", "data", _variant_t(long(0)));

	if(var.vt==VT_BITS && var.lVal!=0)
	{
		byte *pData = (byte *)var.lVal;
		for(int nFrameNo=0; nFrameNo<nOldMaxFrameNo; nFrameNo++)
		{
			byte *pSrc=pData+sizeof(long) + nFrameNo*nOldMaxObjectNo*nOldMaxItemSize;
			byte *pDst=pNewData+sizeof(long) +nFrameNo*nMaxObjectNo*nMaxItemSize;
			for(int nObjectNo=0; nObjectNo<nOldMaxObjectNo; nObjectNo++)
			{
				memcpy(pDst, pSrc, nOldMaxItemSize);
				pSrc += nOldMaxItemSize;
				pDst += nMaxItemSize;
			}
		}
		delete pData;
	}

	_variant_t var1((long)pNewData);
	var1.vt = VT_BITS;

	::SetValue(m_punkObj, "\\particles", "data", var1);
	::SetValue(m_punkObj, "\\particles", "MaxFrameNo", long(nMaxFrameNo));
	::SetValue(m_punkObj, "\\particles", "MaxObjectNo", long(nMaxObjectNo));
	::SetValue(m_punkObj, "\\particles", "MaxItemSize", long(nMaxItemSize));

	return true;
}

void CNamed2DArrays::GetArraySize(int *pnMaxFrameNo, int *pnMaxObjectNo, int *pnMaxItemSize)
{
	if(pnMaxFrameNo) *pnMaxFrameNo = ::GetValueInt(m_punkObj, "\\particles", "MaxFrameNo", 0);
	if(pnMaxObjectNo) *pnMaxObjectNo = ::GetValueInt(m_punkObj, "\\particles", "MaxObjectNo", 0);
	if(pnMaxItemSize) *pnMaxItemSize = ::GetValueInt(m_punkObj, "\\particles", "MaxItemSize", 0);
}

bool CNamed2DArrays::AddArray(const char *pszArrName, int nElementSize)
{
	if(m_punkObj==0) return false;
	int nOldMaxItemSize = ::GetValueInt(m_punkObj, "\\particles", "MaxItemSize", 0);
	bool bRet=SetArraySize(-1, -1, nOldMaxItemSize+nElementSize);
	if(!bRet) return false;
	::SetValue(m_punkObj, "\\particles", pszArrName, long(nOldMaxItemSize));
	return bRet;
}

int CNamed2DArrays::GetObjectStep() //получить шаг в массиве между объектами
{
	if(m_punkObj==0) return 0;
	int nMaxFrameNo = ::GetValueInt(m_punkObj, "\\particles", "MaxFrameNo", 0);
	int nMaxObjectNo = ::GetValueInt(m_punkObj, "\\particles", "MaxObjectNo", 0);
	int nMaxItemSize = ::GetValueInt(m_punkObj, "\\particles", "MaxItemSize", 0);
	return nMaxItemSize;
}


int CNamed2DArrays::GetFrameStep() //получить шаг в массиве между кадрами
{
	if(m_punkObj==0) return 0;
	int nMaxFrameNo = ::GetValueInt(m_punkObj, "\\particles", "MaxFrameNo", 0);
	int nMaxObjectNo = ::GetValueInt(m_punkObj, "\\particles", "MaxObjectNo", 0);
	int nMaxItemSize = ::GetValueInt(m_punkObj, "\\particles", "MaxItemSize", 0);
	return nMaxItemSize*nMaxObjectNo;
}

byte * CNamed2DArrays::GetExistingArray(const char *pszArrName) //получить массив, если его нет - 0
{
	if(m_punkObj==0) return 0;
	_variant_t var = ::GetValue(m_punkObj, "\\particles", "data", _variant_t(long(0)));
	int offset = ::GetValueInt(m_punkObj, "\\particles", pszArrName, -1);

	if(var.vt==VT_BITS && var.lVal!=0 && offset!=-1)
	{
		byte *pData = (byte *)var.lVal+sizeof(long)+offset;
		return pData;
	}
	return 0;
}

// переработать: на данный момент GetArray может сделать значения с предыдущих вызовов некорректными
byte * CNamed2DArrays::GetArray(const char *pszArrName, int nElementSize) //получить массив, если его нет - создать
{
	byte *pRet=GetExistingArray(pszArrName);
	if(pRet==0)
	{
		AddArray(pszArrName, nElementSize);
		pRet=GetExistingArray(pszArrName);
	}
	return pRet;
}

