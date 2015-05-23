#if !defined(__SmartEdit_H__)
#define __SmartEdit_H__

class CSmartIntEdit;
interface IEditSite
{
	virtual void OnChangeValue(CSmartIntEdit *pEdit, int nValue) = 0;
};

class CSmartIntEdit
{
protected:
	int m_nIdEdit, m_nIdSpin, m_nIdSlider;
	CSliderCtrl *m_pSlider;
	CSpinButtonCtrl *m_pSpin;
	CWnd *m_pDlg,*m_pParent;
	IEditSite *m_pSite;
	int m_nMin, m_nMax;
	bool m_bOnChange;
public:
	CSmartIntEdit(int nIdEdit, int nIdSpin, int nIdSlider);
	void Init(CWnd *pDlg, CWnd *pParentSheet, IEditSite *pSite);
	void SetRange(int nMin, int nMax);
	void SetPos(int nPos);
	virtual void OnEditChange();
	virtual void OnHScroll();
	virtual void OnSpinDelta(int nDelta);
};

class CSmartLogDoubleEdit : public CSmartIntEdit
{
protected:
	int m_nRange;
public:
	CSmartLogDoubleEdit(int nIdEdit, int nIdSpin, int nIdSlider) : CSmartIntEdit(nIdEdit, nIdSpin, nIdSlider) {};
	void SetRange(int nRange);
	void SetPos(double dVal);
	virtual void OnEditChange();
	virtual void OnHScroll();
	virtual void OnSpinDelta(int nDelta);
	int PosByDValue(double dValue);
	double DValueByPos(int nPos);
};



#endif