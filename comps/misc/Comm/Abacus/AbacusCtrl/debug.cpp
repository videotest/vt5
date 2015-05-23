#include "stdafx.h"
#include "debug.h"

void dprintf(const char *pszFileName, char * szFormat, ...)
{
    char buf[256];
    va_list va;
    va_start(va, szFormat);
    wvsprintfA(buf, szFormat, va);
    va_end(va);
	HFILE file;
	OFSTRUCT of;
	static bool bFirst = true;
	if (bFirst)
		file = OpenFile(pszFileName,&of,OF_CREATE|OF_WRITE);
	else
		file = OpenFile(pszFileName,&of,OF_WRITE);
	bFirst = false;
	_llseek(file,0,FILE_END);
	_lwrite(file,buf,lstrlen(buf));
	_lclose(file); 
	OutputDebugString(buf);
}

