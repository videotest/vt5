#if !defined(__WoolzObject_H__)
#define __WoolzObject_H__

#include "data5.h"
#include "data_main.h"
#include "resource.h"
#include "nameconsts.h"
#include "Classifiers.h"
#include "Woolz.h"

#define szTypeWoolzObject     "WoolzObject"
#define szWoolzObjectProgId     "WoolzObject"
#define szWoolzObjectInfoProgId "WoolzObjectInfo"

// {3228AD07-172D-4B07-8783-2B0EB1F8502C}
static const GUID clsidWoolzObject = 
{ 0x3228ad07, 0x172d, 0x4b07, { 0x87, 0x83, 0x2b, 0xe, 0xb1, 0xf8, 0x50, 0x2c } };

// {7CABC17E-986C-4D74-9CF2-82A2377E4C5C}
static const GUID clsidWoolzObjectInfo = 
{ 0x7cabc17e, 0x986c, 0x4d74, { 0x9c, 0xf2, 0x82, 0xa2, 0x37, 0x7e, 0x4c, 0x5c } };

struct debugobj;
class CWoolzObject : public CObjectBase, public IWoolzObject,
	public _dyncreate_t<CWoolzObject>
{
	void *obj;
	void *mpol1;
#if defined(_DEBUG)
	void *oldobj;
	debugobj *pdebug;
#endif

//	INamedDataObject* pParent;
	IUnknown *punkParent;
	route_unknown();
public:
	CWoolzObject();
	virtual ~CWoolzObject();

	virtual IUnknown *DoGetInterface( const IID &iid )
	{
		if (iid == IID_IWoolzObject)
			return (IWoolzObject *)this;
		else
			return CObjectBase::DoGetInterface(iid);
	}

	//IPersist helper
	virtual GuidKey GetInternalClassID(){ return clsidWoolzObject; }

public:
	//ISerializableObject
	com_call Load( IStream *pStream, SerializeParams *pparams );
	com_call Store( IStream *pStream, SerializeParams *pparams );
	//INamedDataObject2
	com_call IsBaseObject(BOOL * pbFlag);
	com_call GetParent( IUnknown **ppunkParent );
	com_call SetParent( IUnknown *punkParent, DWORD dwFlags /*AttachParentFlag*/ );
	//IWoolzObject
	com_call GetWoolzObject(void **);
	com_call SetWoolzObject(void *);
	com_call CalculateParameters(IUnknown *punkCO, int nParamsNum, double *pdParams, IChromosome *pChromos);
	com_call NormalizeParameters(IUnknown *punkOL);
	com_call Compare(int nData, int nSize, void *pData);
	com_call FreeDebugData();
	com_call GetDebugData(void **);
};



class CWoolzObjectInfo : public CDataInfoBase,
	public _dyncreate_t<CWoolzObjectInfo>
{
public:
	CWoolzObjectInfo() : CDataInfoBase(clsidWoolzObject, szTypeWoolzObject, 0, 0)
	{	}
};












#endif
