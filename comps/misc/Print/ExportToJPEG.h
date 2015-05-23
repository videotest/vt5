#pragma once

class CActionExportRptToJPEG :
	public CActionBase
{
	DECLARE_DYNCREATE(CActionExportRptToJPEG)
	GUARD_DECLARE_OLECREATE(CActionExportRptToJPEG)
	IReportView2* rptView;
	IReportPageView* rpageView;
	CRect pageRect;
	float zoom;
	int pageCount;
	CString saveFileName;
	bool print_to_file();
public:
	CActionExportRptToJPEG(void);
	virtual ~CActionExportRptToJPEG(void);
	virtual bool Invoke();

};
class CActionBeforePrintView :public CActionBase
{
	DECLARE_DYNCREATE(CActionBeforePrintView)
	GUARD_DECLARE_OLECREATE(CActionBeforePrintView)

public:
	CActionBeforePrintView(void){};
	virtual ~CActionBeforePrintView(void){};
	virtual bool Invoke();


};