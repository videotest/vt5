#if !defined(__StdioFile_H__)
#define __StdioFile_H__

#include <stdio.h>
#include "MessageException.h"


class _StdioFileException : public CMessageException
{
	char szFileName[_MAX_PATH];
public:
	_StdioFileException(const char *pszFileName) : CMessageException("")
	{
		strcpy(szFileName, pszFileName);
	}
	void ReportError()
	{
		char szMsg[_MAX_PATH+256];
		sprintf(szMsg, "File %s can not be opened", szFileName);
		_MessageBoxCaption MsgBoxCaption;
		VTMessageBox(szMsg, MsgBoxCaption, MB_OK|MB_ICONEXCLAMATION);
	}
	void Delete()
	{
		delete this;
	}
};

class _StdioFile
{
public:
	FILE *f;
	_StdioFile()
	{
		f = NULL;
	}
	_StdioFile(const char *pszFileName, const char *pszModes)
	{
		Open(pszFileName,pszModes);
	}
	~_StdioFile()
	{
		Close();
	}
	void Open(const char *pszFileName, const char *pszModes)
	{
		f = fopen(pszFileName, pszModes);
		if (f == 0)
			throw new _StdioFileException(pszFileName);
	}
	void Close()
	{
		if (f) fclose(f);
		f = NULL;
	}
};

class _StdioFilePtr : public _StdioFile
{
public:
	FILE **ppf;
	_StdioFilePtr(FILE **ppf, const char *pszFileName, const char *pszModes) :
		_StdioFile(pszFileName, pszModes)
	{
		this->ppf = ppf;
		*ppf = f;
	}
	~_StdioFilePtr()
	{
		*ppf = NULL;
	}
};


#endif
