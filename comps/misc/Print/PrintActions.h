#ifndef __printactions_h__
#define __printactions_h__



//[ag]1. classes
//////////////////////////////////////////////////////////////////////
class CActionShowPreviewIndirect : public CActionShowViewBase
{
	DECLARE_DYNCREATE(CActionShowPreviewIndirect)
	GUARD_DECLARE_OLECREATE(CActionShowPreviewIndirect)


public:
	virtual CString GetViewProgID();	
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionPrintTemplateDelete : public CActionBase
{
	DECLARE_DYNCREATE(CActionPrintTemplateDelete)
	GUARD_DECLARE_OLECREATE(CActionPrintTemplateDelete)

public:
	CActionPrintTemplateDelete();
	virtual ~CActionPrintTemplateDelete();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

/*
class CActionLeavePrintPreview : public CActionBase
{
	DECLARE_DYNCREATE(CActionLeavePrintPreview)
	GUARD_DECLARE_OLECREATE(CActionLeavePrintPreview)

public:
	CActionLeavePrintPreview();
	virtual ~CActionLeavePrintPreview();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};
*/

class CActionTemplateSetup : public CActionBase
{
	DECLARE_DYNCREATE(CActionTemplateSetup)
	GUARD_DECLARE_OLECREATE(CActionTemplateSetup)

public:
	CActionTemplateSetup();
	virtual ~CActionTemplateSetup();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionFitToPageWidth : public CActionBase
{
	DECLARE_DYNCREATE(CActionFitToPageWidth)
	GUARD_DECLARE_OLECREATE(CActionFitToPageWidth)

public:
	CActionFitToPageWidth();
	virtual ~CActionFitToPageWidth();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

//////////////////////////////////////////////////////////////////////
class CActionFitToPage : public CActionBase
{
	DECLARE_DYNCREATE(CActionFitToPage)
	GUARD_DECLARE_OLECREATE(CActionFitToPage)

public:
	CActionFitToPage();
	virtual ~CActionFitToPage();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

//////////////////////////////////////////////////////////////////////
class CActionPrintSetup : public CActionBase
{
	DECLARE_DYNCREATE(CActionPrintSetup)
	GUARD_DECLARE_OLECREATE(CActionPrintSetup)

public:
	CActionPrintSetup();
	virtual ~CActionPrintSetup();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

//////////////////////////////////////////////////////////////////////
class CActionPageSetup : public CActionBase
{
	DECLARE_DYNCREATE(CActionPageSetup)
	GUARD_DECLARE_OLECREATE(CActionPageSetup)

public:
	CActionPageSetup();
	virtual ~CActionPageSetup();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

//////////////////////////////////////////////////////////////////////
class CActionShowPrintDesignMode : public CActionShowViewBase
{
	DECLARE_DYNCREATE(CActionShowPrintDesignMode)
	GUARD_DECLARE_OLECREATE(CActionShowPrintDesignMode)
public:
	virtual CString GetViewProgID();	
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();

};

//////////////////////////////////////////////////////////////////////
class CActionShowPrintPreview : public CActionBase
{
	DECLARE_DYNCREATE(CActionShowPrintPreview)
	GUARD_DECLARE_OLECREATE(CActionShowPrintPreview)
public:	

	virtual bool Invoke();	
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

//////////////////////////////////////////////////////////////////////
class CActionPrint : public CActionBase
{
	DECLARE_DYNCREATE(CActionPrint)
	GUARD_DECLARE_OLECREATE(CActionPrint)

	ENABLE_SETTINGS();

public:	
	CActionPrint();
	virtual ~CActionPrint();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();		
	virtual bool ExecuteSettings( CWnd *pwndParent );
protected:
	bool m_bPrintDlgOK;
};

//////////////////////////////////////////////////////////////////////
class CActionReportEditText : public CActionBase
{
	DECLARE_DYNCREATE(CActionReportEditText)
	GUARD_DECLARE_OLECREATE(CActionReportEditText)

//	ENABLE_SETTINGS();

public:	
	CActionReportEditText();
	virtual ~CActionReportEditText();

public:
	virtual bool Invoke();
	virtual bool IsAvaible();
};


#endif //__printactions_h__
