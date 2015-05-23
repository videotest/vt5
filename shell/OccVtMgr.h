#pragma once

// COccVtMgr command target

class COccVtMgr : public COccManager
{
public:
	COccVtMgr();
	virtual ~COccVtMgr();
	COleControlSite* CreateSite(COleControlContainer* pCtrlCont);
};


