#if !defined(__ChObjUtils_H__)
#define __ChObjUtils_H__

#include "Chromosome.h"

IUnknownPtr find_child_by_interface(IUnknownPtr sptrParent, const GUID guid);
IChromosomePtr CreateChildObjects(IUnknown *punkCalcObject);

void set_manual_class(ICalcObject *ptrObj, int lClass);
void set_class(ICalcObject *ptrObj, long lClass);
long get_class(ICalcObject *ptrObj, int *plManualClass);

inline _bstr_t _LoadString(int id)
{
	char szBuff[200];
	LoadString(app::instance()->handle(), id, szBuff, 200);
	_bstr_t bstr(szBuff);
	return bstr;
}


#endif
