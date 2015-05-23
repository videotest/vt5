#if !defined(__StreamImp_H__)
#define __StreamImp_H__

#include "MessageException.h"

class CStreamImp : public IStream
{
public:
	CStreamImp();
	CStreamImp(const char *pszPathName, int nMode, int nAttr);
	~CStreamImp();
	
	virtual void Open(const char *pszPathName, int nMode, int nAttr);
	void Close();

// Implementation
	int m_nFile;

	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(QueryInterface)(REFIID, LPVOID*);

	STDMETHOD(Read)(void*, ULONG, ULONG*);
	STDMETHOD(Write)(const void*, ULONG cb, ULONG*);
	STDMETHOD(Seek)(LARGE_INTEGER, DWORD, ULARGE_INTEGER*);
	STDMETHOD(SetSize)(ULARGE_INTEGER);
	STDMETHOD(CopyTo)(LPSTREAM, ULARGE_INTEGER, ULARGE_INTEGER*,
		ULARGE_INTEGER*);
	STDMETHOD(Commit)(DWORD);
	STDMETHOD(Revert)();
	STDMETHOD(LockRegion)(ULARGE_INTEGER, ULARGE_INTEGER,DWORD);
	STDMETHOD(UnlockRegion)(ULARGE_INTEGER, ULARGE_INTEGER, DWORD);
	STDMETHOD(Stat)(STATSTG*, DWORD);
	STDMETHOD(Clone)(LPSTREAM*);
};

class CClassifierStream : public CStreamImp
{
public:
	CClassifierStream(const char *pszPathName, int nMode, int nAttr, const char *pszClsName);
	virtual void Open(const char *pszPathName, int nMode, int nAttr);

	_bstr_t m_bstrClsName;
};


#endif
