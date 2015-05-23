// OccVtMgr.cpp : implementation file
//

#include "stdafx.h"
#include "shell.h"
#include "OccVtMgr.h"
#include "VtControlSite.h"


// COccVtMgr

COccVtMgr::COccVtMgr()
{
}

COccVtMgr::~COccVtMgr()
{
}


// COccVtMgr member functions

COleControlSite* COccVtMgr::CreateSite(COleControlContainer* pCtrlCont)
{
	return new CVtControlSite(pCtrlCont);
}
