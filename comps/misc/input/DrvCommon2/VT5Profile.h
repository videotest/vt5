#if !defined(__VT5Profile_H__)
#define __VT5Profile_H__

#include "StdProfile.h"


class CVT5ProfileManager : public CStdProfileManager
{
	CString m_sDriverName;
public:
	bool IsVT5Profile();
	CVT5ProfileManager(LPCTSTR lpDrvName = NULL, bool bSingleProfile = true);
	// Additional functions : get or set values in unknown format
	virtual VARIANT _GetProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry, bool bIgnoreMethodic = false);
	virtual VARIANT GetProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry, bool bIgnoreMethodic = false);
	virtual VARIANT GetDefaultProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry, bool bIgnoreMethodic = false);
	virtual void WriteProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry, VARIANT varValue, bool bIgnoreMethodic = false);
	// Overwriten methods: works with VT5 ini file
	virtual int _GetProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef, bool bIgnoreMethodic = false);
	virtual double _GetProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dDef, bool *pbDefValueUsed = NULL, bool bIgnoreMethodic = false);
	virtual CString _GetProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpDefault, bool bIgnoreMethodic = false);
	virtual void WriteProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nValue, bool bIgnoreMethodic = false);
	virtual void WriteProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dValue, bool bIgnoreMethodic = false);
	virtual void WriteProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpValue, bool bIgnoreMethodic = false);
	virtual void InitSettings();
	virtual void ResetSettings();
	virtual int GetDefaultProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef, bool bIgnoreMethodic = false);
	virtual double GetDefaultProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double nDef, bool *pbDefValueUsed = NULL, bool bIgnoreMethodic = false);
	virtual CString GetDefaultProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpDefault, bool bIgnoreMethodic = false);
};







#endif