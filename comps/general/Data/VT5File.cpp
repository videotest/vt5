#include "StdAfx.h"
#include "Data.h"
#include "VT5File.h"
#include "\vt5\awin\misc_ptr.h"
#include "CryptedFile.h"

const char VT50Signature[] = "Videotest 5.0 data file";

struct FileHeader
{
	short nVersion;
	ULONGLONG ullFileSize;
	DWORD dwCheckSum;
};

class CFileWork
{
public:
	CFile *m_p;
	CFileWork(CFile *p)
	{
		m_p = p;
	}
	~CFileWork()
	{
		if (m_p)
			m_p->SeekToBegin();
	}
};


const int BuffSize = 2000;
VT5File::VT5FileType VT5File::CheckFile(CFile &file)
{
	CFileWork FileWork(&file);
	_ptr_t<char> buff1(sizeof(VT50Signature));
	char *buff = buff1;
	UINT nRead = file.Read((char *)buff, buff1.size());
	// Old named data file has starts with version number of first nameddata entry.
	// Version can be from 0 to 4.
	if (nRead!=buff1.size())
		return nRead>=4&&buff[0]>=0&&buff[0]<=4&&buff[1]==0&&buff[2]==0&&buff[3]==0?
			OldNamedDataFile:UnknownFile; // file can be short?
	if (strncmp((char *)buff,VT50Signature,buff1.size()) != 0)
		return buff[0]>=0&&buff[0]<=4&&buff[1]==0&&buff[2]==0&&buff[3]==0?
			OldNamedDataFile:UnknownFile;
	// It is new named data file!
	FileHeader fh;
	if (file.Read(&fh, sizeof(fh)) != sizeof(fh))
		return CorruptNewVT5File;
	if (::GetValueInt(GetAppUnknown(), "General", "ForceOpenFiles", 0))
		return ValidNewVT5File;
	if (fh.nVersion > 0)
		return UnsupportedNewVT5File;
	if (fh.ullFileSize == _UI64_MAX)
		return ValidNewVT5File;
	if (fh.ullFileSize == 0 || fh.ullFileSize != file.GetLength())
		return CorruptNewVT5File;
	DWORD adwBuffer[BuffSize];
	memset(adwBuffer, 0, BuffSize*sizeof(DWORD));
	DWORD dwCheckSum = 0;
	while (1)
	{
		DWORD dw = 0;
		UINT nRead = file.Read(adwBuffer, BuffSize*sizeof(DWORD));
		int n = nRead/4;
		for (int i = 0; i < n; i++)
			dwCheckSum  ^= adwBuffer[i];
		if (nRead < BuffSize*sizeof(DWORD))
			break;
	}
	if (dwCheckSum != fh.dwCheckSum)
		return CorruptNewVT5File;
	return ValidNewVT5File;
}

void VT5File::SkipHeaderForFileStream(IStream *pStream)
{
	DWORD dwHeaderSize = sizeof(VT50Signature)+sizeof(FileHeader);
	LARGE_INTEGER l;
	l.QuadPart = dwHeaderSize;
	pStream->Seek(l, STREAM_SEEK_CUR, NULL);
}

bool VT5File::WriteHeaderToStream(IStream *pStream, BOOL bUnchecked)
{
	ULONG ulWritten;
	if (FAILED(pStream->Write(VT50Signature, sizeof(VT50Signature), &ulWritten)) ||
		ulWritten != sizeof(VT50Signature))
		return false;
	FileHeader fh;
	memset(&fh, 0, sizeof(fh));
	if (bUnchecked)
		fh.ullFileSize = _UI64_MAX;
	if (FAILED(pStream->Write(&fh, sizeof(fh), &ulWritten) || ulWritten != sizeof(fh)))
		return false;
	return true;
}

bool VT5File::UpdateHeader(LPCTSTR lpszFileName, bool bCrypted)
{
	DWORD dw = GetTickCount();
	try
	{
		CCryptedFile file(lpszFileName, CFile::modeReadWrite );
		file.m_bCrypted = bCrypted;
		_ptr_t<char> buff1(sizeof(VT50Signature));
		char *buff = buff1;
		UINT nRead = file.Read((char *)buff, buff1.size());
		if (nRead != buff1.size() || strncmp(buff, VT50Signature, buff1.size())!=0)
			return true; // old format
		DWORD dwHeaderSize = sizeof(VT50Signature)+sizeof(FileHeader);
		FileHeader fh;
		memset(&fh, 0, sizeof(fh));
		file.Seek(dwHeaderSize, CFile::begin);
		DWORD dwCheckSum=0;
		DWORD adwBuffer[BuffSize];
		memset(adwBuffer, 0, BuffSize*sizeof(DWORD));
		while (1)
		{
			DWORD dw = 0;
			UINT nRead = file.Read(adwBuffer, BuffSize*sizeof(DWORD));
			int n = nRead/4;
			for (int i = 0; i < n; i++)
				dwCheckSum  ^= adwBuffer[i];
			if (nRead < BuffSize*sizeof(DWORD))
				break;
		}
		fh.dwCheckSum = dwCheckSum;
		fh.ullFileSize = file.GetLength();
		file.Seek(sizeof(VT50Signature), CFile::begin);
		file.Write(&fh, sizeof(fh));
	}
	catch(CException *e)
	{
		e->Delete();
		return false;
	}
	catch(...)
	{
		return false;
	}
	DWORD dw2 = GetTickCount();
	TRACE("VT5File::UpdateHeader %d\n", dw2-dw);
	return true;
}


