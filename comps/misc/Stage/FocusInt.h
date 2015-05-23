#if !defined(__FocusInt_H__)
#define __FocusInt_H__

interface IFocusPane : public IUnknown
{
	com_call SetFocusIndicator(bool bAuto, int nFocus) = 0;
	com_call GetFocusIndicator(bool *pbAuto, int *pnFocus) = 0;
};

declare_interface(IFocusPane,"69039353-B47F-43a1-9746-8861E3B72671");

#endif
