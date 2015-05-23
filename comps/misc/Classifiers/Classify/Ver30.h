#if !defined(__Ver30_H__)
#define __Ver30_H__

#include "data5.h"
#include "data_main.h"
#include "resource.h"
#include "nameconsts.h"

bool IsVersion3x();

#define szTypeVer3xObject     "Version3x"
#define szVer3xObjectProgId     "Version3x"
#define szVer3xObjectInfoProgId "Version3xInfo"


// {56BE6310-0C1A-4D4B-B161-626DDDBD0CBE}
static const GUID clsidVersion3x = 
{ 0x56be6310, 0xc1a, 0x4d4b, { 0xb1, 0x61, 0x62, 0x6d, 0xdd, 0xbd, 0xc, 0xbe } };

// {7633D4CF-16D7-461C-A905-96FF360D5B98}
static const GUID clsidVersion3xInfo = 
{ 0x7633d4cf, 0x16d7, 0x461c, { 0xa9, 0x5, 0x96, 0xff, 0x36, 0xd, 0x5b, 0x98 } };


class CVersion3xObject : public CObjectBase, public _dyncreate_t<CVersion3xObject>
{
public:
	route_unknown();
	virtual GuidKey GetInternalClassID(){ return clsidVersion3x; }
};

class CVersion3xObjectInfo : public CDataInfoBase,
	public _dyncreate_t<CVersion3xObjectInfo>
{
public:
	CVersion3xObjectInfo() : CDataInfoBase(clsidVersion3x, szTypeVer3xObject, 0, 0)
	{	}
};


#endif
