#if !defined(__VT5AVI_H__)
#define __VT5AVI_H__

#define szDocumentVideo "Video"
#define szVideoObject   "VideoObject"


interface IVideoView : public IUnknown
{
	com_call FirstFrame() = 0;
	com_call IsFirstFrame(BOOL *pbFirstFrame) = 0;
	com_call PreviousFrame() = 0;
	com_call NextFrame() = 0;
	com_call LastFrame() = 0;
	com_call IsLastFrame(BOOL *pbLastFrame) = 0;
	com_call Capture(IUnknown *punkImage, bool bStepRight) = 0;
	com_call SetImageDocKey(GUID lDocKey) = 0;
	com_call GetImageDocKey(GUID *plDocKey, BOOL *pbDocKeyInited) = 0;
};

declare_interface(IVideoView, "3BBE6510-D4BF-405A-8CCE-D017DF0EDDC4");

#endif