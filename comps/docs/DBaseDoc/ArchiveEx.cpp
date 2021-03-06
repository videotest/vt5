#include "stdafx.h"
#include "ArchiveEx.h"
#include "zlib.h"

CString signature = _TEXT("akz");

CArchiveExt::CArchiveExt(CFile* pFile, UINT nMode, int nBufSize, void* lpBuf, CString Key, BOOL bCompress)
			:CArchive(pFile, nMode, nBufSize, lpBuf)
{
	m_strKey = Key;
	m_bCompress = bCompress;
	m_pFileTmp = m_pFile;

	if(!Key.IsEmpty())
		m_bCompress = TRUE;

	if(m_bCompress)
	{
		if(IsLoading())
		{
		    int iComprErr;
			CString Contents, Compress;
			_TCHAR sign[4];

			ArcHeader ahead;
			m_pFileTmp -> Read(&ahead, sizeof(ahead));

			sign[0] = ahead.chSignature[0];
			sign[1] = ahead.chSignature[1];
			sign[2] = ahead.chSignature[2];
			sign[3] = _TEXT('\0');

			DWORD dwFileSize;
			if (signature == sign)
				dwFileSize = ahead.dwSize;
			else
				dwFileSize = (DWORD)m_pFileTmp -> GetLength();

			LPSTR szContents = Contents.GetBuffer(dwFileSize);

			m_pBuf = (BYTE *)new char[dwFileSize];
			m_pFile = new CMemFile(m_pBuf, dwFileSize, 1024);

			if (signature == sign)
			{
#if _MSC_VER >= 1300
				dwFileSize = m_pFileTmp -> Read(szContents, dwFileSize);
#else
				dwFileSize = m_pFileTmp -> ReadHuge(szContents, dwFileSize);
#endif
				if(ahead.uchFlag == 3)
				{
					ULONG crc = adler32(0L, Z_NULL, 0);
					Crypto(szContents, dwFileSize, m_strKey);
					crc = adler32(crc, (const unsigned char*)szContents, dwFileSize);
					if(crc != ahead.ulCRC)
					{
						AfxMessageBox("Password incorrect!!!");
						Abort();
						AfxThrowUserException();
					}
				}

				DWORD dwComprSize = ahead.dwOriginalSize;
				LPSTR szCompress = Compress.GetBuffer(dwComprSize);
				iComprErr = uncompress((LPBYTE)szCompress, &dwComprSize, (LPBYTE)szContents, dwFileSize);
				ASSERT(iComprErr == Z_OK);

#if _MSC_VER >= 1300
				m_pFile -> Write(szCompress, dwComprSize);
#else
				m_pFile -> WriteHuge(szCompress, dwComprSize);
#endif
			}
			else
			{
				m_pFileTmp -> Seek(- (LONG)sizeof(ahead), CFile::current);

#if _MSC_VER >= 1300
				dwFileSize = m_pFileTmp -> Read(szContents, dwFileSize);
				m_pFile -> Write(szContents, dwFileSize);
#else
				dwFileSize = m_pFileTmp -> ReadHuge(szContents, dwFileSize);
				m_pFile -> WriteHuge(szContents, dwFileSize);
#endif
			}
			m_pFile -> SeekToBegin();
		}
		else
		{
			m_pBuf = (BYTE *)new char[1024];
			m_pFile = new CMemFile(m_pBuf, 1024, 1024);
		}
	}
}

CArchiveExt::~CArchiveExt()
{
	if (m_pFile != NULL && !(m_nMode & bNoFlushOnDelete))
		Close();

	Abort();    // abort completely shuts down the archive
}

void CArchiveExt::Abort()
{
	if(m_bCompress && m_pFile != NULL)
	{
		m_pBuf = (BYTE *)(((CMemFile*)m_pFile)->Detach());
		delete m_pBuf;
		m_pFile -> Close();
		delete m_pFile;
	}

	CArchive::Abort();
}

void CArchiveExt::Close()
{
	ASSERT_VALID(m_pFile);
	Flush();

	if(m_bCompress)
	{
		if(IsStoring())
		{
		    int iComprErr;
			CString Compress;
			DWORD dwFileSize = (DWORD)m_pFile -> GetLength();

			m_pBuf = (BYTE *)(((CMemFile*)m_pFile)->Detach());
			DWORD dwComprSize = (DWORD)(dwFileSize * 1.1) + 12;
			LPSTR szCompress = Compress.GetBuffer(dwComprSize);
			iComprErr = compress((LPBYTE)szCompress, &dwComprSize, m_pBuf, dwFileSize);
			ASSERT(iComprErr == Z_OK);

			ULONG crc = adler32(0L, Z_NULL, 0);
			crc = adler32(crc, (const unsigned char*)szCompress, dwComprSize);
			Crypto(szCompress, dwComprSize, m_strKey);

			ArcHeader ahead;
			ahead.chSignature[0] = _TEXT('a');
			ahead.chSignature[1] = _TEXT('k');
			ahead.chSignature[2] = _TEXT('z');
			ahead.dwSize = dwComprSize;
			ahead.dwOriginalSize = dwFileSize;
			ahead.uchFlag = m_strKey.IsEmpty()?1:3;
			ahead.ulCRC = crc;

			m_pFileTmp -> Write(&ahead, sizeof(ahead));
#if _MSC_VER >= 1300
			m_pFileTmp -> Write(szCompress, dwComprSize);
#else
			m_pFileTmp -> WriteHuge(szCompress, dwComprSize);
#endif
		}
		else
		{
			m_pBuf = (BYTE *)(((CMemFile*)m_pFile)->Detach());
		}
		delete m_pBuf;
		m_pFile -> Close();
		delete m_pFile;
	}
	m_pFile = NULL;
}

bool CArchiveExt::Crypto(LPSTR lpstrBuffer, ULONG ulLen, CString strKey)
{
	if(strKey.IsEmpty())
		return FALSE;
	
	int nLenKey = strKey.GetLength();

	for(ULONG ii=0; ii<ulLen; ii++)
	{
		for(int jj=0; jj<nLenKey; jj++)
		{
			lpstrBuffer[ii] ^= strKey[jj];
		}
	}
	return TRUE;
}
