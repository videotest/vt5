#ifndef __fish_int_h__
#define __fish_int_h__

#define FISH_ROOT "\\Karyo\\FISH"
#define FISH_COLORS "\\Karyo\\FISH\\PaneColors"

#define FISH_COLOR_PANE "ColorPane"
// Pane shown in view (-1 - composed,...)
#define FISH_SHOWN_PANE "ShownPane"
// Panes which used to form composed image. (when FISH_SHOWN_PANE = -1)
#define FISH_USED_PANES "UsedPanes"
#define FISH_NAME_PANES "PaneName"
#define FISH_PANE_SNAME "PaneShortName"
#define FISH_PREVIEW_WIDTH "PreviewWidth"
#define FISH_SHELLDATA_COPY "CopyToShellData"
#define FISH_PANES_COUNT    "PanesCount"
#define FISH_ENABLED     "Enabled"
#define FISH_COMPOSED_METHOD     "ComposedMethod"
// FishBar устанавливает Properties во View или только шлет нотификации
#define FISH_BAR_SETS_PROPS     "FishBarSetsProps"

interface IColorConvertorFISH : public IUnknown
{
	com_call ReloadColorValue() = 0;
	com_call SetColorValue( UINT nID, DWORD dwColor ) = 0;
};

declare_interface( IColorConvertorFISH, "E006FB51-D3A6-4422-8E63-34AA8FD846EB" )


#endif//__fish_int_h__