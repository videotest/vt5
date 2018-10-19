#ifndef __streamex_h__
#define __streamex_h__

#include "defs.h"
class std_dll CStreamEx
{
	IStreamPtr	m_ptrStream;
	bool		m_bLoading;
	bool		Verify();
public:
	CStreamEx( IStream* pStream, bool bLoading );
	~CStreamEx();
	
	operator IStream*() const;	

	BOOL IsLoading();
	BOOL IsStoring();

	UINT Read(void* lpBuf, UINT nMax);
	void Write(const void* lpBuf, UINT nMax);
	void Flush();


	// insertion operations
	CStreamEx& operator<<(BYTE by);
	CStreamEx& operator<<(WORD w);
	CStreamEx& operator<<(LONG l);
	CStreamEx& operator<<(DWORD dw);
	CStreamEx& operator<<(float f);
	CStreamEx& operator<<(double d);

	CStreamEx& operator<<(int i);
	CStreamEx& operator<<(short w);
	CStreamEx& operator<<(char ch);
	CStreamEx& operator<<(unsigned u);	
//	CStreamEx& operator<<(LONG_PTR l);	
	

	// extraction operations
	CStreamEx& operator>>(BYTE& by);
	CStreamEx& operator>>(WORD& w);
	CStreamEx& operator>>(DWORD& dw);
	CStreamEx& operator>>(LONG& l);
	CStreamEx& operator>>(float& f);
	CStreamEx& operator>>(double& d);

	CStreamEx& operator>>(int& i);
	CStreamEx& operator>>(short& w);
	CStreamEx& operator>>(char& ch);
	CStreamEx& operator>>(unsigned& u);
//	CStreamEx& operator>>(LONG_PTR& l);
	

		
};

std_dll CStreamEx &operator<<(CStreamEx& ar, GuidKey &g);
std_dll CStreamEx &operator>>(CStreamEx& ar, GuidKey &g);


std_dll CStreamEx& operator<<(CStreamEx& ar, SIZE size);
std_dll CStreamEx& operator<<(CStreamEx& ar, POINT point);
std_dll CStreamEx& operator<<(CStreamEx& ar, const RECT& rect);
std_dll CStreamEx& operator>>(CStreamEx& ar, SIZE& size);
std_dll CStreamEx& operator>>(CStreamEx& ar, POINT& point);
std_dll CStreamEx& operator>>(CStreamEx& ar, RECT& rect);


std_dll CStreamEx& operator<<(CStreamEx& ar, const CString& string);
std_dll CStreamEx& operator>>(CStreamEx& ar, CString& string);


#endif//__streamex_h__

