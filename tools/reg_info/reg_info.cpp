// reg_info.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "main_dlg.h"

#include "crtdbg.h"
#include "\vt5\awin\misc_module.h"


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.

	{
		module	_module( hInstance );	
		main_dlg dlg;
		dlg.do_modal( 0 );
	}

	#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
	#endif _DEBUG


	return 0;
}



