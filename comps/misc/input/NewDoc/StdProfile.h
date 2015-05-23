#if !defined(__StdProfile_H__)
#define __StdProfile_H__

class CStdProfileManager
{
private:
	CString m_strDefaultIni;
	CString m_sDefaultProfile;
	CString m_sMethodic;
protected:
	CString SectionName(LPCTSTR  lpSecDef, bool bIgnoreMethodic);
public:
	// This variable used to access methods of CStdProfileManager, e.g. CStdProfileManager::m_pMgr->GetProfileInt(..)
	static CStdProfileManager *m_pMgr;
	// Extract section and antry in string: "Section"\"Entry"
	static void ExtractSectionAndEntry(LPCTSTR lpName, CString &sSec, CString &sEntry);
	// Constructor stores "this" pointer in m_pMgr, so if any variable of type "CStdProfileManager" declared,
	// m_pMgr will not be empty.
	CStdProfileManager(bool bSingleProfile = true);
	~CStdProfileManager();
	// These methods has similar semantics with CWinApp methods
	virtual int _GetProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef, bool bIgnoreMethodic = false);
	virtual double _GetProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dDef, bool *pbDefValueUsed = NULL, bool bIgnoreMethodic = false);
	virtual CString _GetProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpDefault, bool bIgnoreMethodic = false);
	virtual void WriteProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nValue, bool bIgnoreMethodic = false);
	virtual void WriteProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dValue, bool bIgnoreMethodic = false);
	virtual void WriteProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpValue, bool bIgnoreMethodic = false);
	// Next method are used for work with default profile. Default profile usually is .ini file with default
	// (not changeable by user) values.
	// Use InitSettings in InitInstance to setup default profile name.
	virtual void InitSettings();
	virtual void InitMethodic(LPCTSTR lpMeth);
	virtual CString GetMethodic() {return m_sMethodic;}
	virtual void ResetSettings();
	// These methods gets values from default profile if it is enabled and no values in registry or application
	// .ini file exists. If bWriteToReg == true and no values exists, default values will be stored.
	virtual int GetProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef, bool bWriteToReg = false, bool bIgnoreMethodic = false);
	virtual double GetProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dDef, bool *pbDefValueUsed = NULL, bool bWriteToReg = false, bool bIgnoreMethodic = false);
	virtual CString GetProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpDefault, bool bWriteToReg = false, bool bIgnoreMethodic = false);
	// These methods gets values from default profile.
	virtual int GetDefaultProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef);
	virtual double GetDefaultProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double nDef, bool *pbDefValueUsed = NULL);
	virtual CString GetDefaultProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpDefault);
	// Validate values using minimal and maximal values
	virtual void ValidateInt(LPCTSTR lpSection, LPCTSTR lpEntry, int &nValidate) {};
	virtual void ValidateDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double &nValidate) {};
};

#define g_StdProf (*CStdProfileManager::m_pMgr)

#endif