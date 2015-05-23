#ifndef __proppage_h__
#define __proppage_h__

interface IColorPickerPropPage : public IUnknown
{
	com_call ChangeColor( DWORD dwRGB, color H, color S, color L ) = 0;
};

declare_interface( IColorPickerPropPage, "8C45ABC5-E1F4-4803-AA8D-D247CD88170B");

#endif //__proppage_h__
