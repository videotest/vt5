// memtrace.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "crtdbg.h"

struct	s
{
	int	n1;
	int	n2;
};

/*void *operator new( UINT nSize )
{
	return ::operator new (nSize, _NORMAL_BLOCK, __FILE__, __LINE__);
} */


#define new	new(_NORMAL_BLOCK, __FILE__, __LINE__)


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	s	*ps = new s;
	//delete ps;
	_CrtDumpMemoryLeaks( );
	return 0;
}



