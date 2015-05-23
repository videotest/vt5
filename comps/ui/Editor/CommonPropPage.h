#include "vtwidth.h"
#include "vtcolorpicker.h"


class CEditorPropBase : public CPropertyPageBase
{
public:
	CEditorPropBase(UINT nIDTemplate);

	virtual void OnFinalRelease();
	void OnWidth(long nBtn);
	void MakeButtons(long nCount, long lVisibleCount = -1);

	void SetColor(COLORREF color, bool bFore);
	// 0 - grayscale, 1 - color, 2 - with palette. If returns 2 and pPalette != 0, fills it
	int  GetColorMode(COLORREF *pPalette = NULL);
	
protected:
	void _ManageWidthButtons(bool bManageCheck = true);
	void _Init(CString strBase, bool bLineWidthWithBase);
	void _DeInit();

	CVTWidth*	m_pbWidth;
	long		m_nWidthButtons;
	long		m_nActiveWidth;
	long*		m_arrWidths;
	CString		m_strBase;
	bool		m_bLineWidthWithBase;
	bool		m_bColor;
	bool        m_bPalette; // Used when m_bColor == false (for FISH, etc.)
	long		m_nVisibleButtons;
};
