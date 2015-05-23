#include "StdAfx.h"
#include "StreamImp.h"
#include "stdio.h"
#include "io.h"
#include "fcntl.h"

CStreamImp::CStreamImp()
{
	m_nFile = -1;
}

CStreamImp::CStreamImp(const char *pszPathName, int nMode, int nAttr)
{
	Open(pszPathName, nMode, nAttr);
}

CStreamImp::~CStreamImp()
{
	Close();
}

void CStreamImp::Open(const char *pszPathName, int nMode, int nAttr)
{
	Close();
	m_nFile = open(pszPathName, nMode, nAttr);
}

void CStreamImp::Close()
{
	if (m_nFile >= 0)
		close(m_nFile);
}

STDMETHODIMP_(ULONG)CStreamImp::AddRef()
{
	return 1;
}

STDMETHODIMP_(ULONG)CStreamImp::Release()
{
	return 0;
}

STDMETHODIMP CStreamImp::QueryInterface(REFIID iid, LPVOID* ppvObj)
{
	if (iid == IID_IUnknown || iid == IID_IStream)
	{
		*ppvObj = this;
		return NOERROR;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP CStreamImp::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
	int nRead = 0;
	nRead = read(m_nFile, pv, cb);
	if (nRead < 0)
		return E_UNEXPECTED;
	if (pcbRead != NULL)
		*pcbRead = nRead;
	return NOERROR;
}

STDMETHODIMP CStreamImp::Write(const void *pv, ULONG cb, ULONG *pcbWritten)
{
	int nWrite = 0;
	nWrite = write(m_nFile, pv, cb);
	if (nWrite < 0)
		return E_UNEXPECTED;
	if (pcbWritten != NULL)
		*pcbWritten = nWrite;
	return NOERROR;
}

STDMETHODIMP CStreamImp::Seek(LARGE_INTEGER uliOffset, DWORD dwOrigin,
	ULARGE_INTEGER* puliNew)
{
	// can't handle offsets with really large magnitude
	if ((uliOffset.HighPart != 0) &&
		((uliOffset.HighPart != -1) || ((long)uliOffset.LowPart >= 0)))
		return E_NOTIMPL;
	int nOrig;
	if (dwOrigin == STREAM_SEEK_SET)
		nOrig = SEEK_SET;
	else if (dwOrigin == STREAM_SEEK_CUR)
		nOrig = SEEK_CUR;
	else if (dwOrigin == STREAM_SEEK_END)
		nOrig = SEEK_END;
	else
		return E_UNEXPECTED;
	long lNew = lseek(m_nFile, (long)uliOffset.LowPart, nOrig);
	if (puliNew != NULL)
		ULISet32(*puliNew, lNew);
	return NOERROR;
}

STDMETHODIMP CStreamImp::SetSize(ULARGE_INTEGER)
{
	return E_NOTIMPL;
}

STDMETHODIMP CStreamImp::CopyTo(LPSTREAM, ULARGE_INTEGER, ULARGE_INTEGER*,
	ULARGE_INTEGER*)
{
	return E_NOTIMPL;
}

STDMETHODIMP CStreamImp::Commit(DWORD)
{
	return E_NOTIMPL;
}

STDMETHODIMP CStreamImp::Revert()
{
	return E_NOTIMPL;
}

STDMETHODIMP CStreamImp::LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
{
	return E_NOTIMPL;
}

STDMETHODIMP CStreamImp::UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER,
	DWORD)
{
	return E_NOTIMPL;
}

STDMETHODIMP CStreamImp::Stat(STATSTG*, DWORD)
{
	return E_NOTIMPL;
}

STDMETHODIMP CStreamImp::Clone(LPSTREAM*)
{
	return E_NOTIMPL;
}

CClassifierStream::CClassifierStream(const char *pszPathName, int nMode, int nAttr, const char *pszClsName) :
	CStreamImp(), m_bstrClsName(pszClsName)
{
	Open(pszPathName, nMode, nAttr);
}

static void __read(int file, void *buf, unsigned size, const char *pszMsg)
{
	unsigned n = read(file, buf, size);
	if (n != size)
		throw new CMessageException(pszMsg);
}

static void __write(int file, void *buf, unsigned size, const char *pszMsg)
{
	unsigned n = write(file, buf, size);
	if (n != size)
		throw new CMessageException(pszMsg);
}

void CClassifierStream::Open(const char *pszPathName, int nMode, int nAttr)
{
	CStreamImp::Open(pszPathName, nMode, nAttr);
	if (m_nFile >= 0)
	{
		if (nMode & _O_CREAT)
		{
			__write(m_nFile, "Clsf", 4, "Error during write classifier marker");
			long lBufSiz = m_bstrClsName.length();
			__write(m_nFile, &lBufSiz, sizeof(long), "Error during write classifier name size");
			__write(m_nFile, (char *)m_bstrClsName, lBufSiz, "Error during write classifier name");
		}
		else
		{
			char szMarker[4];
			int nRead = read(m_nFile, szMarker, 4);
			if (nRead != 4 || memcmp(szMarker, "Clsf", 4))
				throw new CMessageException("Invalid classifier file (marker)");
			long lBufSiz;
			__read(m_nFile, &lBufSiz, sizeof(long), "Invalid classifier file (bufsize)");
			_ptr_t2<char> Buf(lBufSiz+1);
			__read(m_nFile, Buf, lBufSiz, "Invalid classifier file (classifier name)");
			Buf[lBufSiz] = 0;
			if (strcmp(m_bstrClsName,Buf))
				throw new CMessageException("This file is for another classifier");
		}
	}
}

