#ifndef __imagemisc_h__
#define __imagemisc_h__



interface IHumanColorConvertor : public IUnknown
{
	com_call GetPaneShortName( int numPane, BSTR *pbstrName ) = 0;
	com_call Convert( color colorInternal, int numPane, color* pcolorHuman ) = 0;
};

interface IHumanColorConvertor2 : public IHumanColorConvertor
{
	// In such color convertors as FISH pane names can be different on different images and views
	com_call GetPaneShortName2(IUnknown *punkImage, IUnknown *punkParams, int numPane, BSTR *pbstrName ) = 0;
	// Image pane support. Make color string by image, view and coordinates. If returns S_FALSE
	// image pane makes color string itself.
	com_call GetColorString(IUnknown *punkImage, IUnknown *punkParams, POINT pt, BSTR *pbstr) = 0;
};

declare_interface( IHumanColorConvertor, "05299E0F-822C-4b80-9C73-60422C315E7A" )
declare_interface( IHumanColorConvertor2, "309ABEAA-B584-4071-B32D-0DF7A385A1EC" )

#endif // __imagemisc_h__