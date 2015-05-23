#pragma once

#include "Input.h"

class CImageDocCreator : public CCmdTargetEx
{
	DECLARE_DYNCREATE(CImageDocCreator)
	ENABLE_MULTYINTERFACE()
	CImageDocCreator(void);

protected:
	virtual ~CImageDocCreator(void);
public:

	GUARD_DECLARE_OLECREATE(CImageDocCreator)
	DECLARE_INTERFACE_MAP()
	BEGIN_INTERFACE_PART(Creator, IDocCreater)
		com_call ExecuteSettings(HWND hwndParent, BSTR bstrDocType, int nDialogMode);
		com_call InitDocument(IUnknown *punkDoc);
	END_INTERFACE_PART(Creator);

private:
	BOOL m_bSelectDriver;
	int m_nDialogMode;
	BOOL m_bFirst;
	CString m_sName;
	CString m_sConf;
};
