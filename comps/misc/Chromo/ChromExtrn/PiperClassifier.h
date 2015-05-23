#ifndef __PiperClassifier_H__
#define __PiperClassifier_H__

#include "data5.h"
#include "data_main.h"
#include "resource.h"
#include "nameconsts.h"
#include "Classifiers.h"

#define szTypePiperClassifier       "Classifiers.Piper"
#define szPiperClassifierProgId     "Classifiers.PiperClassifier"
#define szPiperClassifierInfoProgId "Classifiers.PiperClassifierInfo"

// {2E85940D-ECFC-4C59-A651-C4D4348B75C8}
static const GUID clsidPiperClassifier = 
{ 0x2e85940d, 0xecfc, 0x4c59, { 0xa6, 0x51, 0xc4, 0xd4, 0x34, 0x8b, 0x75, 0xc8 } };

// {D4069328-2655-498B-BCB5-7C147743F399}
static const GUID clsidPiperClassifierInfo = 
{ 0xd4069328, 0x2655, 0x498b, { 0xbc, 0xb5, 0x7c, 0x14, 0x77, 0x43, 0xf3, 0x99 } };


class CPiperClassifier : public CObjectBase, public IClassifier,
	public _dyncreate_t<CPiperClassifier>
{
	void *pVcvData;
	void *pTeachData;

	route_unknown();

public:
	CPiperClassifier();
	virtual ~CPiperClassifier();

	virtual IUnknown *DoGetInterface( const IID &iid )
	{
		if (iid == IID_IClassifier) return (IClassifier*)this;
		else return CObjectBase::DoGetInterface( iid );
	}

	//IPersist helper
	virtual GuidKey GetInternalClassID(){ return clsidPiperClassifier; }

public:
	//ISerializableObject
	com_call Load( IStream *pStream, SerializeParams *pparams );
	com_call Store( IStream *pStream, SerializeParams *pparams );
	//INamedDataObject2
	com_call IsBaseObject(BOOL * pbFlag);
	//IClassifier
	com_call GetFlags(DWORD *pdwFlags);
	com_call Classify(IUnknown *punkObjectsList, IUnknown *punkClassesList, IUnknown *punkDoc);
	com_call Teach(IUnknown *punkObjectsList, IUnknown *punkClassesList, IUnknown *punkDoc);
};

class CPiperClassifierInfo : public CDataInfoBase,
	public _dyncreate_t<CPiperClassifierInfo>
{
public:
	CPiperClassifierInfo() : CDataInfoBase(clsidPiperClassifier, szTypePiperClassifier, 0, IDI_PIPERCLASSIFIER)
	{	}
};




#endif //__image_h__