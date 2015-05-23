#pragma once


#include "actionobjectbase.h"
#include "ObListWrp.h"
#include "resource.h"

class CActionObjectsExport : public CActionBase
{
	DECLARE_DYNCREATE(CActionObjectsExport)
	GUARD_DECLARE_OLECREATE(CActionObjectsExport)

	bool ExportToFile(CObjectListWrp &ObjList, CFile *pFile, LPCTSTR lpstrDecSep,
		BOOL bAddParamNames, BOOL bByteOrderMarker);
public:
	CActionObjectsExport ();
	virtual ~CActionObjectsExport();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
};


