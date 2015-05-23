#pragma once

class CActionAbacusConnect : public CActionBase
{
	DECLARE_DYNCREATE(CActionAbacusConnect)
	GUARD_DECLARE_OLECREATE(CActionAbacusConnect)
	static bool s_bConnected;
	static bool s_bAutoExecute;
public:
	CActionAbacusConnect(void);
	~CActionAbacusConnect(void);
	// Установлено ли соединение.
	static bool IsConnected() {return s_bConnected;}
	// Надо ли автоматически запускать акцию AbacusToDB, если соединение
	// установлено и никакая акция сейчас не выполняется?
	static bool IsAutoExecute() {return s_bAutoExecute;}
public:
	virtual bool Invoke();
	virtual bool IsChecked();

};

struct ABACUSPARAMETER;
class CActionAbacusToDB : public CActionBase
{
	DECLARE_DYNCREATE(CActionAbacusToDB)
	GUARD_DECLARE_OLECREATE(CActionAbacusToDB)

	void AddWarningText(CString &s, LPCTSTR lpstrWarn, LPCTSTR lpstrDef);
	CString MakeSexString(int nSex);
	CString MakeWarningString(DWORD dwWarn);
	CString MakeModeString(int nMode);
	void DoSendToDB(const char *pszName, _variant_t varValue);
	void SendToDB(const char *pszName, int nValue);
	void SendToDB(const char *pszName, double dValue);
	void SendToDB(const char *pszName, struct tm tm1);
	void SendToDB(const char *pszName, const char *pszValue);
	void SendParameterToDB(const char *pszName, double dCoef, ABACUSPARAMETER *pParam);
public:
	CActionAbacusToDB(void);
	~CActionAbacusToDB(void);
public:
	virtual bool Invoke();
	virtual bool IsAvaible();
};
