#ifndef __RectFilter_h__
#define __RectFilter_h__

// {38673C5C-E2BE-4C78-9592-4FB143E45DEF}
static const GUID clsidRectFilter = 
{ 0x38673c5c, 0xe2be, 0x4c78, { 0x95, 0x92, 0x4f, 0xb1, 0x43, 0xe4, 0x5d, 0xef } };


#define pszRectFilterProgID "ChromExtrn.RectFilter"

#include "FilterPropBase.h"

class CChromoRecord
{
public:
	BOOL  m_bAutoSeg;
	int   m_nClass;
	RECT  m_rcChromo;
	POINT m_ptPos;
	BYTE *m_pData;
};


class CRectFilter : public CFilterPropBase,
				  public _dyncreate_t<CRectFilter>	
{
	int m_nObjects;
	CChromoRecord *m_pObjects;
	void FreeRecords();
public:
	CRectFilter();
	virtual ~CRectFilter();

	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );
	virtual bool _InitNew();
};


#endif