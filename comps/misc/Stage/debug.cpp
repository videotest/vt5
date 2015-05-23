#include "StdAfx.h"
#include "Debug.h"
#include "StdioFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

_StdioFile File;

void dprintf(char * szFormat, ...)
{
	static bool bCalled = false;
	if (!bCalled)
	{
		remove(__LOGFILE__);
		bCalled = true;
	}
	static int nDebug = -1;
	if (nDebug == -1)
		nDebug = ::GetValueInt(GetAppUnknown(), __DEBUG_SECTION__, "Debug", 0);
	if (nDebug == 1)
	{
		static int nReopen = -1;
		if (nReopen == -1)
			nReopen = ::GetValueInt(GetAppUnknown(), __DEBUG_SECTION__, "DebugFileReopen", 0);
		FILE *f;
		if (nReopen)
		{
			f = fopen(__LOGFILE__, "at");
			if (!f)
				f = fopen(__LOGFILE__, "wt");
		}
		else
		{
			try
			{
				if (File.f == NULL)
					File.Open(__LOGFILE__, "wt");
				f = File.f;
			}
			catch(_StdioFileException *e)
			{
				e->Delete();
				return;
			}
		}
		if (!f) return;
		va_list va;
		va_start(va, szFormat);
		vfprintf(f, szFormat, va);
		va_end(va);
		if (nReopen)
			fclose(f);
	}
}

void dflush()
{
	if (File.f)
		fflush(File.f);
/*	{
		File.Close();
		File.Open(__LOGFILE__, "at");
	}*/
}
