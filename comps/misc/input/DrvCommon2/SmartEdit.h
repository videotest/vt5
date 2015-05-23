#if !defined(__SmartEdit_H__)
#define __SmartEdit_H__

#include "input.h"
#include "Utils.h"

class CDrvControl;
class CSmartIntEdit;
interface IEditSite
{
	virtual void OnChangeValue(CSmartIntEdit *pEdit, int nValue) = 0;
};

class CDrvControl : public CObject, public IDrvControl
{
protected:
	CWnd *m_pDlg;
	IDriver5Ptr m_sptrDrv;
	CString m_sSec,m_sEntry;
	IDrvControlSite *m_pSite;
public:
	CDrvControl();
	~CDrvControl();
	void Register(IDrvControlSite *pSite);
	CString &GetSectionName() {return m_sSec;}
	CString &GetEntryName() {return m_sEntry;}
	virtual bool Init(CWnd *pDlg, IDriver5 *pSite) {return false;}
	virtual bool Reset() {return false;}
	virtual bool Reinit() {return false;}
	virtual void Default() {}
	virtual bool IsHandler(int id) {return false;}
	virtual bool OnHScroll(int idScroll, UINT nSBCode) {return false;}
	virtual bool OnEditChange(int idEdit) {return false;}
	virtual bool OnSpinDelta(int idSpin, int nDelta) {return false;}
	virtual bool OnCmdMsg(int nCode, int id, HWND hwndCtrl) {return false;}
};

class CSmartIntEdit : public CDrvControl
{
	void _Init(int nIdEdit, int nIdSpin, int nIdSlider, LPCTSTR lpstrSec, LPCTSTR lpstrEntry,
		int nMin, int nMax, int nDef, int nCur, IEditSite *pSite, UINT nFlags);
protected:
	int m_nIdEdit, m_nIdSpin, m_nIdSlider;
	CSliderCtrl *m_pSlider;
	CSpinButtonCtrl *m_pSpin;
	IEditSite *m_pSite;
	int m_nMin, m_nMax, m_nDef, m_nCur;
	UINT m_nFlags;
	bool m_bOnChange;
	virtual bool InitControls(CWnd *pDlg, IDriver5 *pSite);
	virtual void EnableControls(bool bEnable);
	bool InitDefaults(int &nCurrent);
	virtual int PosByValue(int nValue) {return nValue;}
	virtual int ValueByPos(int nPos) {return nPos;}
	virtual int ValidatePos(int nPos) {return Range(nPos,m_nMin,m_nMax);}
public:
	enum SmartIntFlags
	{
		ThumbPosition = 1,
	};
public:
	CSmartIntEdit(int nIdEdit, int nIdSpin, int nIdSlider, LPCTSTR lpstrSec, LPCTSTR lpstrEntry,
		int nMin, int nMax, int nDef, UINT nFlags = 0);
	CSmartIntEdit(int nIdEdit, int nIdSpin, int nIdSlider, LPCTSTR lpstrSec, LPCTSTR lpstrEntry,
		IEditSite *pSite = NULL, UINT nFlags = 0);
	CSmartIntEdit(int nIdEdit, int nIdSpin, int nIdSlider, int nMin, int nMax, int nDef, 
		int nCur, IEditSite *pSite = NULL, UINT nFlags = 0);
	virtual void SetRange(int nMin, int nMax);
	virtual void SetPos(int nPos);
	virtual bool Init(CWnd *pDlg, IDriver5 *pSite);
	virtual bool Reset();
	virtual void Default();
	virtual bool IsHandler(int id) {return id==m_nIdEdit||id==m_nIdSpin||id==m_nIdSlider;}
	virtual bool OnEditChange(int idEdit);
	virtual bool OnHScroll(int idScroll, UINT nSBCode);
	virtual bool OnSpinDelta(int idSpin, int nDelta);
	virtual int GetId() {return m_nIdEdit;}
};

class CSmartSpanEdit : public CSmartIntEdit
{
	CUIntArray m_arrVals;
	void InitValues(int nMin, int nMax);
protected:
	virtual int PosByValue(int nValue);
	virtual int ValueByPos(int nPos) {return (int)m_arrVals[nPos];}
	virtual int ValidatePos(int nPos) {return Range(nPos,0,(int)m_arrVals.GetSize()-1);}
public:
	CSmartSpanEdit(int nIdEdit, int nIdSpin, int nIdSlider, LPCTSTR lpstrSec, LPCTSTR lpstrEntry,
		int nMin, int nMax, int nDef);
	CSmartSpanEdit(int nIdEdit, int nIdSpin, int nIdSlider, LPCTSTR lpstrSec, LPCTSTR lpstrEntry);
	CSmartSpanEdit(int nIdEdit, int nIdSpin, int nIdSlider, int nMin, int nMax, int nDef,
		int nCur, IEditSite *pSite);
	virtual bool Init(CWnd *pDlg, IDriver5 *pSite);
};

class CSmartLogDoubleEdit : public CSmartIntEdit
{
protected:
	CArray<double,double> m_arrValues;
	int m_nRange;
	int PosByDValue(double d);
	double DValueByPos(int i) {return m_arrValues[i];}
public:
	CSmartLogDoubleEdit(int nIdEdit, int nIdSpin, int nIdSlider, LPCTSTR lpstrSec, LPCTSTR lpstrEntry,
		double dMin, double dMax, double dDef, UINT nFlags = 0);
//	void SetRange(int nRange);
	void SetPos(double dVal);
	virtual bool Reset();
	virtual bool Init(CWnd *pDlg, IDriver5 *pSite);
	virtual bool OnEditChange(int idEdit);
	virtual bool OnHScroll(int idScroll, UINT nSBCode);
	virtual bool OnSpinDelta(int idSpin, int nDelta);
//	int PosByDValue(double dValue);
//	double DValueByPos(int nPos);
};

class CSmartRangeDoubleEdit : public CSmartIntEdit
{
protected:
	double m_dMin,m_dMax,m_dStep,m_dDef;
public:
	CSmartRangeDoubleEdit(int nIdEdit, int nIdSpin, int nIdSlider, LPCTSTR lpstrSec, LPCTSTR lpstrEntry,
		double dMin, double dMax, double dStep, double dDef) :
		CSmartIntEdit(nIdEdit, nIdSpin, nIdSlider, lpstrSec, lpstrEntry, 0, 0, 0)
	{
		m_dMin = dMin;
		m_dMax = dMax;
		m_dStep = dStep;
		m_dDef = dDef;
	};
	void SetRange(double dMin, double dMax, double dStep);
	void SetPos(double dVal);
	virtual bool Init(CWnd *pDlg, IDriver5 *pSite);
	virtual bool Reset();
	virtual void Default();
	virtual bool OnEditChange(int idEdit);
	virtual bool OnHScroll(int idScroll, UINT nSBCode);
	virtual bool OnSpinDelta(int idSpin, int nDelta);
};

// editor with two spins and slider
class CSmartIntEdit2 : public CSmartIntEdit
{
protected:
	int m_nIdSpin2, m_nIdSlider2;
	int m_nStep1, m_nStep2;
	CSliderCtrl *m_pSlider2;
	CSpinButtonCtrl *m_pSpin2;
	virtual bool InitControls(CWnd *pDlg, IDriver5 *pSite);
	virtual void EnableControls(bool bEnable);
public:
	CSmartIntEdit2(int nIdEdit, int nIdSpin, int nIdSlider, int nIdSpin2, int nIdSlider2,
		LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nMin, int nMax, int nDef, int nStep1,
		int Step2, UINT nFlags = 0);
	CSmartIntEdit2(int nIdEdit, int nIdSpin, int nIdSlider, int nIdSpin2, int nIdSlider2,
		LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nStep1, int nStep2, IEditSite *pSite = NULL, UINT nFlags = 0);
	CSmartIntEdit2(int nIdEdit, int nIdSpin, int nIdSlider, int nIdSpin2, int nIdSlider2,
		int nMin, int nMax, int nDef, int nCur, int nStep1, int nStep2, IEditSite *pSite = NULL, UINT nFlags = 0);

	void _SetPos(int nPos, DWORD dwFlags); // 1 - skip edit, 2- spin1, 4 - spin2, 8 - slider1, 16 - slider2
	virtual void SetRange(int nMin, int nMax);
	virtual void SetPos(int nPos);
	virtual bool Init(CWnd *pDlg, IDriver5 *pSite);
	virtual bool IsHandler(int id);
	virtual bool OnEditChange(int idEdit);
	virtual bool OnHScroll(int idScroll, UINT nSBCode);
	virtual bool OnSpinDelta(int idSpin, int nDelta);
};

class CControlHandler : public CDrvControl
{
protected:
	int m_id;
public:
	CControlHandler(int id, LPCTSTR lpstrSec, LPCTSTR lpstrEntry);
	virtual bool Init(CWnd *pDlg, IDriver5 *pSite);
	virtual bool IsHandler(int id) {return id==m_id;}
	void DoReinit(int id, LPCTSTR lpstrSec, LPCTSTR lpstrEntry);
};

class CCheckboxHandler : public CDrvControl
{
	int m_idCheckBox;
	BOOL m_bDefault;
public:
	CCheckboxHandler(int idCheckBox, LPCTSTR lpstrSec, LPCTSTR lpstrEntry, BOOL bDefault);
	virtual bool Init(CWnd *pDlg, IDriver5 *pSite);
	virtual void Default();
	virtual bool IsHandler(int id) {return id==m_idCheckBox;}
	virtual bool OnCmdMsg(int nCode, int id, HWND hwndCtrl);
};

class CRadioHandler : public CDrvControl
{
	CArray<int,int> m_arrIds;
	int m_nDefault,m_nCurrent;
	int m_nFirstButton,m_nLastButton;
	int ValueById(int id);
public:
	CRadioHandler(int *pnArray, int nArraySize, LPCTSTR lpstrSec, LPCTSTR lpstrEntry,
		int nDefault);
	virtual bool Init(CWnd *pDlg, IDriver5 *pSite);
	virtual void Default();
	virtual bool IsHandler(int id);
	virtual bool OnCmdMsg(int nCode, int id, HWND hwndCtrl);
};

class CComboboxHandler : public CControlHandler
{
protected:
	int m_nDefault;
	COMBOBOXREPRDATA *m_pReprData;
public:
	CComboboxHandler(int id, LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nDefault,
		COMBOBOXREPRDATA *pReprData = NULL);
	virtual bool Init(CWnd *pDlg, IDriver5 *pSite);
	virtual void Default();
	virtual bool OnCmdMsg(int nCode, int id, HWND hwndCtrl);
};

class CListboxHandler : public CControlHandler
{
protected:
	int m_nDefault;
	COMBOBOXREPRDATA *m_pReprData;
public:
	CListboxHandler(int id, LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nDefault,
		COMBOBOXREPRDATA *pReprData = NULL);
	virtual bool Init(CWnd *pDlg, IDriver5 *pSite);
	virtual void Default();
	virtual bool OnCmdMsg(int nCode, int id, HWND hwndCtrl);
	virtual bool Reinit();
};

class CStaticHandler : public CControlHandler
{
	int m_nType; // 0 - integer, 1 - string
	int m_nDefault;
	CString m_sDefault;
public:
	CStaticHandler(int id, LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nDefault);
	CStaticHandler(int id, LPCTSTR lpstrSec, LPCTSTR lpstrEntry, LPCTSTR lpstrDefault);
	virtual bool Init(CWnd *pDlg, IDriver5 *pSite);
	virtual bool Reset();
};

#endif