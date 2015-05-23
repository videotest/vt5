#pragma once

class CIniFile
{
public:
	CIniFile(void);
	virtual ~CIniFile(void);
	virtual bool Open(char *filename);
	virtual bool Close();
	virtual bool GetVal(char *section, char *name, char *buf, int bufsize);
	virtual bool SetVal(char *section, char *name, char *buf);
private:
	char m_filename[MAX_PATH];
};
