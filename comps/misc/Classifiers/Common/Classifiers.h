#if !defined(__Classifiers_H__)
#define __Classifiers_H__


interface IClassifier : public IUnknown
{
	// Changes objects in punkObjectsList accordingly the new classification
	com_call GetFlags(DWORD *pdwFlags) = 0;
	com_call Classify(IUnknown *punkObjectsList, IUnknown *punkClassesList, IUnknown *punkDoc) = 0;
	com_call Teach(IUnknown *punkObjectsList, IUnknown *punkClassesList, IUnknown *punkDoc) = 0;
};


declare_interface(IClassifier, "DB433411-48A6-4CFB-A8DB-A32B1F042E77");


#endif