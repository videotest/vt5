#pragma once

#include "measure5.h"
#include "class_utils.h"

class CClassifyActionsBase
{
public:
	CClassifyActionsBase(void);
	virtual ~CClassifyActionsBase(void);
	com_call DoUndo();
	com_call DoRedo();
	void NotifyParent();
	void FireEvent( IUnknown *punkCtrl, const char *pszEvent, IUnknown *punkFrom, IUnknown *punkData, void *pdata, long cbSize );
protected:
	ICalcObjectPtr m_ptrObject;
	_bstr_t m_bstrClassFile;
	long m_lClass;
	long m_undo_lClass;
	bool m_manual_flag;
	bool m_undo_manual_flag;
};
