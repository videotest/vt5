#include "stdafx.h"
#include "scriptdoc.h"
#include "templ.h"

CString CScriptDocTemplate::s_strProgID = "ScriptDoc.Template";

IMPLEMENT_DYNCREATE(CScriptDocTemplate, CCmdTargetEx);

// {17F91CD2-0C2C-11d3-A5B6-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CScriptDocTemplate, "ScriptDoc.Template", 
0x17f91cd2, 0xc2c, 0x11d3, 0xa5, 0xb6, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);

CString CScriptDocTemplate::GetDocTemplString()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString	s;
	s.LoadString( IDR_SCRIPT );

	return s;
}