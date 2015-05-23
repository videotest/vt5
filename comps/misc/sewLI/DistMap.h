#pragma once

#include "object5.h"
#include "image5.h"

// {E55E6C2B-EA78-41b0-924C-B2833CA9D1A8}
static const GUID clsidDistanceMap = 
{ 0xe55e6c2b, 0xea78, 0x41b0, { 0x92, 0x4c, 0xb2, 0x83, 0x3c, 0xa9, 0xd1, 0xa8 } };


class CDistMap : public CObjectBase, public IDistanceMap
{
	WORD **m_ppRows;
	unsigned m_cx,m_cy;
	void Free();
public:
	dummy_unknown();
	CDistMap();
	~CDistMap();
	bool IsEmpty() {return m_ppRows==NULL;}
	com_call CreateNew(int cx, int cy);
	com_call Destroy();
	com_call GetRow(int nRow, WORD **ppRow);
	com_call GetCX(int *pcx);
	com_call GetCY(int *pcy);
	com_call AddRect(RECT rc);
	virtual GuidKey GetInternalClassID(){ return clsidDistanceMap; }
};

// {94B9EAE2-0B3F-4075-B216-92BF843113FF}
static const GUID clsidTranspMap = 
{ 0x94b9eae2, 0xb3f, 0x4075, { 0xb2, 0x16, 0x92, 0xbf, 0x84, 0x31, 0x13, 0xff } };


class CTranspMap : public CObjectBase, public ITransparencyMap
{
	dummy_unknown();
	BYTE **m_ppRows;
	unsigned m_cx,m_cy;
	void Deinit();
public:
	CTranspMap();
	~CTranspMap();
	bool IsEmpty() {return m_ppRows==NULL;}
	com_call CreateNew( int cx, int cy );
	com_call Free();
	com_call GetRow( int nRow, byte **pprow );
	com_call GetSize( int *pnCX, int *pnCY );
	virtual GuidKey GetInternalClassID(){ return clsidTranspMap; }
};

void MakeTranspMapByDistMaps(CTranspMap &TranspMap, CDistMap &DMTotal,
	CDistMap &DMCur, POINT ptOffs, SIZE szTM);
