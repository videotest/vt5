#pragma once


namespace VT5File
{
	enum VT5FileType
	{
		ValidNewVT5File = 0,
		CorruptNewVT5File = 1,
		UnsupportedNewVT5File = 2,
		OldNamedDataFile = 3,
		UnknownFile = 4,
	};
	VT5FileType CheckFile(CFile &file);
	void SkipHeaderForFileStream(IStream *pStream);
	bool WriteHeaderToStream(IStream *pStream, BOOL bUnchecked);
	bool UpdateHeader(LPCTSTR lpszFileName, bool BCrypted=false);
};