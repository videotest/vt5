#ifndef __LikClassifier_H__
#define __LikClassifier_H__

#include "data5.h"
#include "data_main.h"
#include "resource.h"
#include "nameconsts.h"
#include "Classifiers.h"

#define szTypeLikClassifier       "Classifiers.Likelihood"
#define szLikClassifierProgId     "Classifiers.LikelihoodClassifier"
#define szLikClassifierInfoProgId "Classifiers.LikelihoodClassifierInfo"

// {57FDE35D-1DCE-4F40-8C17-74B95029D873}
static const GUID clsidLikClassifier = 
{ 0x57fde35d, 0x1dce, 0x4f40, { 0x8c, 0x17, 0x74, 0xb9, 0x50, 0x29, 0xd8, 0x73 } };

// {69C612D7-4091-4F4F-85CF-138F4B02955B}
static const GUID clsidLikClassifierInfo = 
{ 0x69c612d7, 0x4091, 0x4f4f, { 0x85, 0xcf, 0x13, 0x8f, 0x4b, 0x2, 0x95, 0x5b } };



class CLikClassifier : public CObjectBase, public IClassifier,
	public _dyncreate_t<CLikClassifier>
{
	void *pVcvData;
	void *pTeachData;

	route_unknown();

public:
	CLikClassifier();
	virtual ~CLikClassifier();

	virtual IUnknown *DoGetInterface( const IID &iid )
	{
		if (iid == IID_IClassifier) return (IClassifier*)this;
		else return CObjectBase::DoGetInterface( iid );
	}

	//IPersist helper
	virtual GuidKey GetInternalClassID(){ return clsidLikClassifier; }

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

class CLikClassifierInfo : public CDataInfoBase,
	public _dyncreate_t<CLikClassifierInfo >
{
public:
	CLikClassifierInfo () : CDataInfoBase(clsidLikClassifier, szTypeLikClassifier, 0, IDI_LIKCLASSIFIER)
	{	}
};




#endif //__LikClassifier_H__