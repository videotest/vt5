#if !defined(__DriverWrp_H__)
#define __DriverWrp_H__

#include "input.h"


class CDriverWrp : public sptrIDriver2
{
public:
	CDriverWrp(IUnknown *punk  = 0, bool bAddRef = true);
	void Attach(IUnknown *punk, bool bAddRef = true);
	IUnknown *operator =(IUnknown *);

	int     GetInt(int nDev, LPCTSTR sSection, LPCTSTR sEntry, int nDef, UINT nFlags = 0);
	void    SetInt(int nDev, LPCTSTR sSection, LPCTSTR sEntry, int nValue, UINT nFlags = 0);
	CString GetString(int nDev, LPCTSTR sSection, LPCTSTR sEntry, LPCTSTR sDef, UINT nFlags = 0);
	void    SetString(int nDev, LPCTSTR sSection, LPCTSTR sEntry, LPCTSTR sValue, UINT nFlags = 0);
	double  GetDouble(int nDev, LPCTSTR sSection, LPCTSTR sEntry, double dDef, UINT nFlags = 0);
	void    SetDouble(int nDev, LPCTSTR sSection, LPCTSTR sEntry, double dValue, UINT nFlags = 0);
};






#endif

