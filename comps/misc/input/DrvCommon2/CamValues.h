#pragma once

#include <limits.h>
#include "input.h"

class CCamValue : public IDrvControlSite
{
	static CCamValue *m_apFirst[16];
	CCamValue *m_pNext;
	bool m_bInit;
	CArray<IDrvControl*,IDrvControl*&> m_arrCtrls;
protected:
	bool m_bWriteToReg,m_bIgnoreMethodic,m_bSkipReinitCamera;
	VALUEHEADER m_vh;
	REPRDATA    m_ReprData;
public:
	static int m_nActiveChain;

	enum CCamValueFlags
	{
		WriteToReg = 1,
		IgnoreMethodic = 2,
		FreeData = 4,
		SkipReinitCamera = 8,
	};

	CCamValue(int nChain, DWORD dwFlags);


	virtual void AddControl(IDrvControl *pControl);
	virtual void RemoveControl(IDrvControl *pControl);
	virtual void GetValueInfo(VALUEINFOEX2 *pvie2);
	void ResetControls();
	void ReinitControls();
	void _Init(bool bForce = false);

	virtual VALUEHEADER *GetHdr() {return &m_vh;}
	virtual BASICVALUEINFO *GetVI() {return NULL;};
	virtual REPRDATA  *GetRepr() {return &m_ReprData;};
	int id() {return GetRepr()->nDlgCtrlId;}
	virtual void SetId(int id) {GetRepr()->nDlgCtrlId = id;}
	virtual int GetValueInt() {return 0;}
	virtual void SetValueInt(int n, bool bReset = false) {}
	virtual double GetValueDouble() {return 0.;}
	virtual void SetValueDouble(double d, bool bReset = false) {}

	virtual void Init() {}
	virtual void Save() {}
	virtual void OnInitControl() {}
	static void InitChain(int nChain = 0, bool bForce = FALSE);
	static CCamValue *FindByName(LPCTSTR lpstrSec, LPCTSTR lpstrEntry);
	static CCamValue *FindById(int nDlgCtrlId);
	bool CheckByName(LPCTSTR lpstrSec, LPCTSTR lpstrEntry);
	bool CheckByName(BSTR bstrSec, BSTR bstrEntry);
	bool NeedReinitCamera() {return !m_bSkipReinitCamera;}
};

class CValuesGroup
{
	CArray<CCamValue*,CCamValue*&> m_arrValues;
public:
	void Add(CCamValue *p) {m_arrValues.Add(p);}
	void InitSection(LPCTSTR lpstrSecName);
};

class CCamStringValue : public CCamValue
{
public:
	VALUEINFOTEXT m_vi;
	CCamStringValue(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, LPCTSTR lpstrDef, int nBuffSize,
		CReprType ReprType = IRepr_Unknown, int nChain = 0,	int nDlgCtrl = -1, DWORD dwFlags = CCamValue::WriteToReg);
	~CCamStringValue();
	virtual BASICVALUEINFO *GetVI() {return (BASICVALUEINFO *)&m_vi;};
	virtual void Init();
	virtual void Save();
	operator const char*() {return m_vi.pszBuffer;}
	const char *operator =(const char *p);
	void SetValueString(const char *p, bool bReset = false);
};


class CCamIntValue : public CCamValue
{
public:
	VALUEINFO m_vi;
	CCamIntValue(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nDefault, int nMin = INT_MIN,
		int nMax = INT_MAX, CReprType ReprType = IRepr_Unknown, int nChain = 0,
		int nDlgCtrl = -1, DWORD dwFlags = CCamValue::WriteToReg);
	virtual BASICVALUEINFO *GetVI() {return (BASICVALUEINFO *)&m_vi;};
	int GetValueInt() {return m_vi.nCurrent;}
	void SetValueInt(int n, bool bReset = false);
	operator int();
	int operator =(int n);
	virtual void Init();
	virtual void Save();
	void InitScope(int nMin, int nMax, int nDef);
};


class CCamIntValue2 : public CCamIntValue
{
public:
	SMARTINTREPRDATA2 m_SIRD2;
	// ReprType == IRepr_SmartIntEdit2
	CCamIntValue2(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nDefault, int nMin = INT_MIN,
		int nMax = INT_MAX, int nStep1 = 1, int nStep2 = 1, int nChain = 0, int nDlgCtrl = -1,
		DWORD dwFlags = CCamValue::WriteToReg);
	virtual REPRDATA  *GetRepr() {return &m_SIRD2;};
	virtual void SetId(int id) {m_SIRD2.nDlgCtrlId = id;}
	void InitScope(int nMin, int nMax, int nDef);
};

class CCamBoolValue : public CCamIntValue
{
public:
	CCamBoolValue(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, bool bDefault, int nChain = 0,
		int nDlgCtrl = -1, DWORD dwFlags = CCamValue::WriteToReg);
	operator bool();
	bool operator=(bool b);
	void SetValueInt(int n, bool bReset = false);
};

class CCamIntComboValue : public CCamIntValue
{
protected:
	int ValueByIndex(int nIndex);
	int IndexByValue(int nValue);
public:
	COMBOBOXREPRDATA m_CBoxRData;
	CCamIntComboValue(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, INTCOMBOITEMDESCR *pArray, int nValuesCount,
		int nValueDef, int nChain = 0, int nDlgCtrl = -1, DWORD dwFlags = CCamValue::WriteToReg);
	operator int();
	virtual REPRDATA  *GetRepr() {return &m_CBoxRData;};
};

class CCamIntComboValue2 : public CCamIntComboValue
{
	CArray<INTCOMBOITEMDESCR,INTCOMBOITEMDESCR&> m_Data;
public:
	CCamIntComboValue2(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nValueDef, int nChain = 0, int nDlgCtrl = -1, DWORD dwFlags = CCamValue::WriteToReg);
	~CCamIntComboValue2();
	void Add(int nVal, LPCTSTR lpstrString, int idString);
};

class CCamDoubleValue : public CCamValue
{
public:
	VALUEINFODOUBLE m_vi;
	CCamDoubleValue(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, double dDefault, double dMin,
		double dMax, CReprType ReprType = IRepr_LogDoubleEdit, int nChain = 0,
		int nDlgCtrl = -1, DWORD dwFlags = CCamValue::WriteToReg);
	virtual BASICVALUEINFO *GetVI() {return (BASICVALUEINFO *)&m_vi;};
	virtual double GetValueDouble() {return m_vi.dCurrent;}
	virtual void SetValueDouble(double d, bool bReset = false);
	operator double();
	double operator =(double n);
	virtual void Init();
	virtual void Save();
	void InitScope(double dMin, double dMax, double dDef);
};

enum CRectZone
{
	Rect_None = 0,
	Rect_Top = 7,
	Rect_Bottom = 11,
	Rect_Left = 13,
	Rect_Right = 14,
	Rect_TopLeft = 5,
	Rect_TopRight = 6,
	Rect_BottomLeft = 9,
	Rect_BottomRight = 10,
	Rect_Move = 15,
};


class CCamValueRect : public CCamBoolValue
{
	static CCamValueRect *m_apFirstRect[16];
	CCamValueRect *m_pNextRect;
	CRectZone RectZone(CPoint pt);
public :
	CRect m_rc;
	CCamValueRect(LPCTSTR lpstrSec, int nChain = 0, int nDlgCtrl = -1, DWORD dwFlags = CCamValue::WriteToReg);
	RECT operator=(RECT rc);
	operator RECT();
	virtual void Init();
	virtual void Save();
	bool IsEnabled();
	void Enable(bool bEnable);
	void SetValueInt(int n, bool bReset = false);

	CRect SafeGetRect(int cx, int cy);

	static void MapAllRects(LPBITMAPINFOHEADER lpbi);
	static void DrawAllRects(CDC *pDC, LPRECT prcSrc, LPRECT prcDst);
	static bool FindRect(CPoint pt, CRectZone *prz, CCamValueRect **ppVal);
	virtual bool NeedDrawRect() {return m_vi.nCurrent>0;}
};
