#if !defined(__InputInt_H__)
#define __InputInt_H__


struct CButtonInfo
{
	size_t m_nSize;
	int m_nButtonId;
	char m_szButtonName[128];
	CButtonInfo()
	{
		m_nSize = sizeof(*this);
	}
};

interface ISettingsSite;
interface IRedrawHook;
interface IDialogSite
{
	virtual int GetButtonsNumber() = 0;
	virtual void GetButtonInfo(int nButton, CButtonInfo *pButtonInfo) = 0;
	virtual void ButtonPress(int nButtonId, ISettingsSite *pSettingsSite, IRedrawHook **ppRedrawHook) = 0;
};

interface IDriverEx
{
	virtual void UpdateSizes() = 0;
};

interface ISettingsSite
{
	virtual void OnChangeOvrMode(bool bOvrMode) = 0;
	virtual void OnChangeNaturalSize(bool bNaturalSize) = 0;
};

interface IRedrawHook
{
	virtual void OnRedraw() = 0;
	virtual void OnDelayedRedraw() = 0;
};

#endif