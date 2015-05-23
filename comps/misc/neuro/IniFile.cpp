#include "StdAfx.h"
#include "inifile.h"

CIniFile::CIniFile(void)
{
	m_filename[0]=0;
}

CIniFile::~CIniFile(void)
{
	Close();
}

bool CIniFile::Open(char *filename)
{
	strncpy(m_filename, filename, MAX_PATH);
	return(true);
}

bool CIniFile::Close()
{
	return(true);
}

bool CIniFile::GetVal(char *section, char *name, char *buf, int bufsize)
{
	if (buf==0 || bufsize<1) return false;
	buf[0]=0;
	::GetPrivateProfileString(section, name, "", buf, bufsize, m_filename);
	return(true);
}

bool CIniFile::SetVal(char *section, char *name, char *buf)
{
	::WritePrivateProfileString(section, name, buf, m_filename);
	return(true);
}
