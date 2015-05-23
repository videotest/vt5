#if !defined(__StdProfile_H__)
#define __StdProfile_H__

class CStdProfileManager
{
private:
	//sergey
	friend class CBaseDialog;
	//end

	friend class CMethodicsComboValue;
	CString m_sCommonIni;   // methodic - independent settings
	CString m_sMethodicIni; // methodic settings
	CString m_sDefaultIni;  // defaults for methodic - independent settings
	CString m_sMethDfltIni; // defaults for methodic settings
	CString m_sMethodic;    // Methodic name
	void InitCommonIni();
	void InitMethodicIni();
protected:
	CString SectionName(LPCTSTR lpSecDef, bool bIgnoreMethodic);
public:
	// Helper functions
	LPCTSTR GetIniName(bool bIgnoreMethodic = false);
	LPCTSTR GetDefIniName(bool bIgnoreMethodic = false);
	static LPCTSTR _MakeIniFileName(LPCTSTR lpstrAdd);
	// This variable used to access methods of CStdProfileManager, e.g. CStdProfileManager::m_pMgr->GetProfileInt(..)
	static CStdProfileManager *m_pMgr;
	// Extract section and antry in string: "Section"\"Entry"
	static void ExtractSectionAndEntry(LPCTSTR lpName, CString &sSec, CString &sEntry);
	// Constructor stores "this" pointer in m_pMgr, so if any variable of type "CStdProfileManager" declared,
	// m_pMgr will not be empty.
	CStdProfileManager(bool bSingleProfile = true);
	~CStdProfileManager();
	// These methods has similar semantics with CWinApp methods
	virtual VARIANT _GetProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry, bool bIgnoreMethodic = false);
	virtual int _GetProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef, bool bIgnoreMethodic = false);
	virtual double _GetProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dDef, bool *pbDefValueUsed = NULL, bool bIgnoreMethodic = false);
	virtual CString _GetProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpDefault, bool bIgnoreMethodic = false);
	virtual void WriteProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry, VARIANT varValue, bool bIgnoreMethodic = false);
	virtual void WriteProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nValue, bool bIgnoreMethodic = false);
	virtual void WriteProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dValue, bool bIgnoreMethodic = false);
	virtual void WriteProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpValue, bool bIgnoreMethodic = false);
	// Init methodic
	virtual void InitMethodic(LPCTSTR lpMeth);
	virtual CString GetMethodic() {return m_sMethodic;}
	virtual void ResetSettings();
	// These methods gets values from default profile if it is enabled and no values in registry or application
	// .ini file exists. If bWriteToReg == true and no values exists, default values will be stored.
	// Algorythm of value retrieving for methodic settings:
	// 1. From m_sMethodicIni
	// 2. If no - from m_sMethDfltIni
	// Algorythm of value retrieving for methodic - independent settings and all settings when methodic is empty:
	// 1. From m_sCommonIni
	// 2. If no - from m_sDefaultIni
	virtual VARIANT GetProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry, bool bIgnoreMethodic = false);
	virtual int GetProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef, bool bWriteToReg = false, bool bIgnoreMethodic = false);
	virtual bool GetProfileBool(LPCTSTR lpSection, LPCTSTR lpEntry, bool bDef, bool bWriteToReg = false, bool bIgnoreMethodic = false)
		{return GetProfileInt(lpSection, lpEntry, bDef, bWriteToReg, bIgnoreMethodic)!=0;}
	virtual double GetProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double dDef, bool *pbDefValueUsed = NULL, bool bWriteToReg = false, bool bIgnoreMethodic = false);
	virtual CString GetProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpDefault, bool bWriteToReg = false, bool bIgnoreMethodic = false);
	virtual COLORREF GetProfileColor(LPCTSTR lpSection, LPCTSTR lpEntry, COLORREF clrDefault, bool bWriteToReg = false, bool bIgnoreMethodic = false);
	// These methods gets values from default profile.
	virtual VARIANT GetDefaultProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry, bool bIgnoreMethodic = false);
	virtual int GetDefaultProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef, bool bIgnoreMethodic = false);
	virtual double GetDefaultProfileDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double nDef, bool *pbDefValueUsed = NULL, bool bIgnoreMethodic = false);
	virtual CString GetDefaultProfileString(LPCTSTR lpSection, LPCTSTR lpEntry, LPCTSTR lpDefault, bool bIgnoreMethodic = false);
	// Validate values using minimal and maximal values
	virtual void ValidateInt(LPCTSTR lpSection, LPCTSTR lpEntry, int &nValidate) {};
	virtual void ValidateDouble(LPCTSTR lpSection, LPCTSTR lpEntry, double &nValidate) {};
};

#define g_StdProf (*CStdProfileManager::m_pMgr)

void GetAllEntries(LPCTSTR lpFileName, LPCTSTR lpSection, CStringArray &arrKeys);


#endif