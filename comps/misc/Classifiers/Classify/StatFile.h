#if !defined(__StatFile_H__)
#define __StatFile_H__

#include "StdioFile.h"

class _StatFile : public _StdioFile
{
public:
	_StatFile()
	{
	}
	_StatFile(char *pszName, int nStat = -1, char *pszMode = "wt")
	{
		_Open(pszName, nStat, pszMode);
	}
	void _Open(char *pszName, int nStat = -1, char *pszMode = "wt")
	{
		if (nStat == -1)
			nStat = ::GetValueInt(GetAppUnknown(), "Woolz", "Statistics", 0);
		if (nStat)
		{
			char szRptPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR];
			GetModuleFileName(NULL,szRptPath,_MAX_PATH);
			_splitpath(szRptPath,szDrive,szDir,NULL,NULL);
			_makepath(szRptPath,szDrive,szDir,pszName, NULL);
			try
			{
				Open(szRptPath, pszMode);
			}
			catch(_StdioFileException *e)
			{
				e->ReportError();
				e->Delete();
			}
		}
	}
};


#endif
