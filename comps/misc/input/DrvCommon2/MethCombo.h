#pragma once

#include "CamValues.h"

class CMethodicsComboValue : public CCamIntComboValue
{
	int m_nValues;
	INTCOMBOITEMDESCR *m_pDescr;
	CStringArray m_saMethodics;
	CString MethodicByMethodicIni(LPCTSTR lpstrMethIni);
	CString MethUsrNameByMethIni(LPCTSTR lpstrMethIni, BOOL *pbDefault = FALSE);
	void Deinit();
	CString UniqueName(LPCTSTR lpstrMethName);
	CString UniqueValue(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, LPCTSTR lpstrValue);
public:
	static int s_idsPredefined;

	CMethodicsComboValue(int id, int nChain = 0);
	~CMethodicsComboValue();

	virtual void OnInitControl();
	void SetValueInt(int n, bool bReset = false);

	int NewMethodic();
	int GetMethodicsCount() {return m_nValues;}
	//sergey 26/01/06
	INTCOMBOITEMDESCR * GetMethodics(){return m_pDescr;}
	CStringArray * GetMethodics1(){return &m_saMethodics;}
	
	//CStringArray m_saMethodics;
	//INTCOMBOITEMDESCR *m_pDescr;
	//CString MethUsrNameByMethIni(LPCTSTR lpstrMethIni, BOOL *pbDefault = FALSE);
	//end
};

extern CMethodicsComboValue g_Methodics;
