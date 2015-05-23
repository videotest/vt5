#if !defined(__Input_H__)
#define __Input_H__

#include "ComDef.h"
#include "Defs.h"
//#include "Common.h"

#define szPluginInputDrv		"input driver"
#define szDocumentPreview		"Preview"

// If flag set, driver manager calls method IDriver::InputVideoFile for input of video file.
#define FG_SUPPORTSVIDEO         0x00000001
// If flag set, when driver manager rely that driver itself adds image object to document inside IDriver::InputNative.
// Parameter pUnkImage of IDriver::InputNative will be NULL. Otherwise, driver creates image object, passes its
// pointer into IDriver::InputNative as pUnkImage, and adds it to document.
#define FG_DOCUMENT              0x00000002
// If flag set, when driver manager enables accumulation controls in driver setup and presumes that the driver understands
// the entries Accumulate and AccumulateImagesNum in its section of the shell.data.
#define FG_ACCUMULATION          0x00000004
// It merely means that input frame for this driver makes sense. Driver manager itself handles all work with
// input frame, therefore this flag doesn't imposes any obligations to driver.
#define FG_INPUTFRAME            0x00000008
// Driver supports color key in overlay window. Make sense only if driver supports IInputWndOverlay 
#define FG_OVERLAYCOLORKEY       0x00000010
// Device supports trigger mode input by external event.
#define FG_TRIGGERMODE           0x00001000
// Device can generate software trigger events
#define FG_SOFTWARETRIGGER       0x00002000
// Device has focus indicator
#define FG_FOCUSINDICATOR        0x00000020
// IDriver3::GetImage returns valid DIB with BITMAPINFOHEADER
#define FG_GETIMAGE_DIB          0x00000040

interface IDriverManager : public IUnknown
{
	com_call GetCurrentDeviceName(BSTR *pbstrShortName) = 0;
	com_call SetCurrentDeviceName(BSTR bstrShortName) = 0;
	com_call IsInputAvailable(BOOL *pbAvail) = 0;
	com_call ExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nSelectDriver, int nDialogMode,
		BOOL bFirst, BOOL bFromSettings) = 0;
	com_call GetCurrentDeviceAndDriver(BOOL *pbFound, IUnknown **ppunkDriver, int *pnDeviceInDriver) = 0;
	com_call GetSectionName(BSTR *pbstrSecName) = 0;
	com_call GetDevicesCount(int *pnDeviceCount) = 0;
	com_call GetDeviceInfo(int nDevice, BSTR *pbstrDevShortName, BSTR *pbstrDevLongName, BSTR *pbstrCategory,
		int *pnDeviceDriver, int *pnDeviceInDriver) = 0;
};

interface IDriverManager2 : public IDriverManager
{
	com_call ExecuteSettings2(HWND hwndParent, IUnknown *pTarget, int nSelectDriver, int nDialogMode,
		BOOL bFirst, BOOL bFromSettings, BSTR bstrConfName) = 0;
};

// Implements be caller of IDriver::InputDIB
interface IDIBProvider : public IUnknown
{
	virtual void *__stdcall AllocMem(DWORD dwSize) = 0;
};

interface IDriver : public IUnknown
{
	// Input in VT5 native format. bFromSettings will be used when DriverManager executes next call sequence:
	// ExecuteSettings(..., ..., TRUE);
	// InputNative(..., ..., TRUE);
	com_call InputNative(IUnknown *pUnkImage, IUnknown *punkTarget, int nDevice, BOOL bFromSettings) = 0;
	// Input in Windows DIB format.
	com_call InputDIB(IDIBProvider *pDIBPrv, IUnknown *punkTarget, int nDevice, BOOL bFromSettings) = 0;
	// Input video file. InputVideoFile is called inside ExecuteSettings, therefor it may contain dialogs. Meaning of
	// nMode such as in ExecuteSettings. *pbstrVideoFile initially can be NULL or contains
	// file name to save.
	com_call InputVideoFile(BSTR *pbstrVideoFile, int nDevice, int nMode) = 0;
	// nMode parameter : 1 - minimal setting (as in VT4 Ctrl+I), 2 - maximal settings (as in VT4 Ctrl+N),
	// 0 - without interface at all. If bFirst==TRUE, driver will set up and save some settings, such as VT4's
	// CreateFirst. If bFirst==FALSE, driver will use these settings, such as VT4's CreateNext. 
	// For parameter bForInput see above.
	com_call ExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput) = 0;
	com_call GetFlags(DWORD *pdwFlags) = 0;
	// These functions obtains short and long names of driver.
	com_call GetShortName(BSTR *pbstrShortName) = 0; // Returns name in short form (e.g. MV500)
	com_call GetLongName(BSTR *pbstrShortName) = 0; // Returns name in long form (e.g. Mutech MV-500/510)
	// Gets and sets values for driver
	com_call GetValue(int nCurDev, BSTR bstrName, VARIANT *pValue) = 0;
	com_call SetValue(int nCurDev, BSTR bstrName, VARIANT Value) = 0;
	// Execute dialog, such as "Source", "Format", etc.
	com_call ExecuteDriverDialog(int nDev, LPCTSTR lpstrName) = 0;
	// Obtain information about devices (one driver can contain several devices).
	com_call GetDevicesCount(int *pnCount) = 0;
	com_call GetDeviceNames(int nDev, BSTR *pbstrShortName, BSTR *pbstrLongName, BSTR *pbstrCategory) = 0;
};

#define ID2_STORE          0x00000001
#define ID2_IGNOREMETHODIC 0x00000002
#define ID2_BOARDIFDEFAULT 0x00000004

interface IDriver2 : public IDriver
{
	// Called then sizes of input image was changed in order to notify all preview sites.
	com_call UpdateSizes() = 0;
	// Next function will be used in dialogs with preview and "Cancel", "Apply" and "Ok" buttons.
	// Such dialogs has "preview mode", when changes in settings immediately reflects in preview window,
	// but doesn't stored in profiles. In "preview mode" functions SetValue, GetValue, SetValue2, GetValue2
	// will work in such manner.
	//
	com_call BeginPreviewMode() = 0; // Enter into preview mode
	com_call EndPreviewMode() = 0; // Exit from preview mode
	com_call CancelChanges() = 0; // Cancels all not-saved changes, but doesn't ends preview mode
	com_call ApplyChanges() = 0; // Applies all not-saved changes, but doesn't ends preview mode
	// Some extended GetValue and SetValue functions, which supports subsections, methodics and storing control.
	com_call GetValueInt(int nCurDev, BSTR bstrSection, BSTR bstrEntry, int *pnValue, UINT nFlags) = 0;
	com_call SetValueInt(int nCurDev, BSTR bstrSection, BSTR bstrEntry, int nValue, UINT nFlags) = 0;
	com_call GetValueString(int nCurDev, BSTR bstrSection, BSTR bstrEntry, BSTR *pbstrValue, UINT nFlags) = 0;
	com_call SetValueString(int nCurDev, BSTR bstrSection, BSTR bstrEntry, BSTR bstrValue, UINT nFlags) = 0;
	com_call GetValueDouble(int nCurDev, BSTR bstrSection, BSTR bstrEntry, double *pdValue, UINT nFlags) = 0;
	com_call SetValueDouble(int nCurDev, BSTR bstrSection, BSTR bstrEntry, double dValue, UINT nFlags) = 0;
	// Called after application change some settings.
	com_call ReinitPreview() = 0;
};

interface IDriverSite : public IUnknown
{
	com_call Invalidate() = 0;
	com_call OnChangeSize() = 0;
};

#define IInputPreviewSite IDriverSite

interface IDriver3 : public IDriver2
{
	// This function is invented to be called from IInputPreviewSite::Invalidate.
	// Therefore, it neither performs any initialization nor copying data, but returns
	// instead the pointer to driver's data buffer, which will be valid only inside
	// IInputPreviewSite::Invalidate. If no data buffer or driver doesn't support getting
	// images now (e.g. in videooverlay or VIP mode), driver must return S_FALSE.
	// Format of data in buffer can vary depending of driver: it can contain BITMAPINFO
	// or other header or only raw image data. But some functions of NewDoc or DrvCommon
	// require DIB with BITMAPINFOHEADER in this data array
	com_call GetImage(int nCurDev, LPVOID *ppData, DWORD *pdwDataSize) = 0;
	// This function can convert data saved by GetImage into native format.
	com_call ConvertToNative(int nCurDev, LPVOID lpData, DWORD dwDataSize, IUnknown *punk) = 0;
	com_call GetTriggerMode(int nCurDev, BOOL *pgTriggerMode) = 0;
	com_call SetTriggerMode(int nCurDev, BOOL bSet) = 0;
	com_call DoSoftwareTrigger(int nCurDev) = 0;
	// Image aqusition is a process of permanent reading of image data from device.
	// Image aquisition started by BeginPreview, InitOverlay, etc.
	com_call StartImageAquisition(int nCurDev, BOOL bStart) = 0; // if bStart == TRUE - start, else stop
	com_call IsImageAquisitionStarted(int nCurDev, BOOL *pbAquisition) = 0;
	com_call AddDriverSite(int nCurDev, IDriverSite *pSite) = 0;
	com_call RemoveDriverSite(int nCurDev, IDriverSite *pSite) = 0;
};

interface IDriver4 : public IDriver3
{
	// Configuration names will be used for FISH: if SetConfiguration was called before InputImage,
	// InputDIB, ExecuteSettings, BeginPreview, etc., driver will restore its settings from
	// sectin Input/<drivername>/<configurationname> in shell.data
	com_call SetConfiguration(BSTR bstrName) = 0;
	com_call GetConfiguration(BSTR *bstrName) = 0;
};

enum CValueType
{
	ValueType_Int = 0,
	ValueType_Double = 1,
	ValueType_Text = 2,
};

struct BASICVALUEINFO
{
	int nType;
};

struct VALUEINFO
{
	int nType;
	int nMin;
	int nMax;
	int nDefault;
	int nCurrent;
};

struct VALUEINFOTEXT : public BASICVALUEINFO
{
	const char *pszDefault;
	char *pszBuffer;
	int nMaxBuffer;
};



struct VALUEINFODOUBLE : public BASICVALUEINFO
{
	double dMin;
	double dMax;
	double dDefault;
	double dCurrent;
};

struct VALUEHEADER
{
	LPCTSTR pszSection;
	LPCTSTR pszEntry;
};

enum CReprType
{
	IRepr_Unknown = -1,
	IRepr_SmartIntEdit = 0,  // smart editor with slider and spinbutton
	IRepr_SmartSpanEdit = 1, // span editor, such as exposure in baumer or pixera
	IRepr_SmartIntEdit2 = 2, // extended smart edit with two sliders and spinbuttons
	IRepr_LogDoubleEdit = 5,
	IRepr_RangeDoubleEdit = 6,
	IRepr_Checkbox = 10,      // checkbox
	IRepr_Radiobuttons = 11,  // group of radiobuttons
	IRepr_Combobox = 12,      // combobox
	IRepr_Static = 20,
};


struct REPRDATA
{
	CReprType Type;
	int nDlgCtrlId;
};

struct SMARTINTREPRDATA2 : public REPRDATA
{
	int nStep1; // 0 - use VALUEINFO::nMin
	int nStep2;
};

struct INTCOMBOITEMDESCR
{
	int     nValue;
	LPCTSTR lpstrValue;
	int     idString; // if lpstrValue==0 then combobox will initialized by string from resource
};

enum CComboBoxReprDataType
{
	CBox_Integer = 0,
};

struct COMBOBOXREPRDATA : public REPRDATA
{
	CComboBoxReprDataType ComboType;
	int nValues;
	union
	{
		INTCOMBOITEMDESCR *pnValues;
	};
};

struct VALUEINFOEX
{
	VALUEHEADER Hdr;
	VALUEINFO   Info;
	DWORD       dwCamDataType;
	DWORD       dwCamData;
};

interface IDrvControlSite;
struct VALUEINFOEX2
{
	int nType; // must be 0
	VALUEHEADER     *pHdr;
	BASICVALUEINFO  *pInfo;
	REPRDATA        *pReprData;
	IDrvControlSite *pDrvCtrlSite;
};

interface IVideoHook : public IUnknown
{
	com_call OnStart(int nStages) = 0;
	com_call OnBeginStage(int nStage, const char *pszStageName, int nFrames) = 0;
	com_call OnEndStage(int nStage) = 0;
	com_call OnFrame(int nFrame) = 0;
	com_call OnEnd() = 0;
};

interface IDriver5 : public IDriver4
{
	// Driver can calculate focus (if flag FG_FOCUSINDICATOR is set). In other
	// cases driver doesn't support IDriver5 interface or all focus functions will
	// return E_NOTIMPLEMENTED
	com_call GetFocusMode(BOOL *pbFocusMode) = 0;
	com_call SetFocusMode(BOOL bFocusMode) = 0;
	com_call GetFocusIndicator(int *pnFocusIndicator) = 0;
	// A way to obtain informations about camera settings: minimum, maximum, defaults, etc.
	// (can change during session). Functions returns one of struct VALUEINFOINT or VALUEINFODOUBLE.
	com_call GetValuesCount(int nCurDev, int *pnValues) = 0;
	com_call GetValueInfo(int nCurDev, int nValue, VALUEINFOEX *pValueInfo) = 0;
	com_call GetValueInfoByName(int nCurDev, const char *pszSection, const char *pszEntry, VALUEINFOEX *pValueInfo) = 0;
	com_call InputVideoFile2(int nDevice, int nMode, IVideoHook *pVideoHook) = 0;
};

enum CTriggerNotifyMode
{
	TrigNtfMode_Notify = 1, // use IDriverSite::Invalidate
	TrigNtfMode_SeqNum = 2, // use IDriver5::GetImageSequenceNumber
};

interface IDriver6 : public IDriver5
{
	// Alternative way of working with trigger: driver doesn't calls IDriverSite::Invalidate,
	// instead it keeps the sequence number of image, increments it on trigger signal and
	// returns it to caller by GetImageSequenceNumber. If driver supports IDriverSite::Invalidate
	// it will sets *plTriggerNotifyMode in GetTriggerNotifyMode as 1, if it supports GetImageSequenceNumber,
	// it will sets 2, if it supports both methods, it sets 3. If GetTriggerNotifyMode
	// returns E_NOTIMPL or it doesn't implements IDriver6 at all, then, if its supports
	// trigger mode, it works only with IDriverSite.
	com_call GetTriggerNotifyMode(int nCurDev, DWORD *plTriggerNotifyMode) = 0;
	com_call GetImageSequenceNumber(int nCurDev, long *plSeqNum) = 0;
	// Some boards (e.g. MV-500) require manual reset of trigger.
	com_call ResetTrigger(int nCurDev) = 0;
};

interface IInputPreview : public IUnknown
{
	com_call BeginPreview(IInputPreviewSite *pSite) = 0;
	com_call EndPreview(IInputPreviewSite *pSite) = 0;
	com_call GetPreviewFreq(DWORD *pdwFreq) = 0;
	com_call GetSize(short *pdx, short *pdy) = 0;
	com_call DrawRect(HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst) = 0;
};

// Image capture during preview - feature which make possible using IDriver3::GetImage
// (and image will be appropriate quality) and fast calls of InputNative. Features,
// incompatible with image capture during preview are YUV, 16-bit modes, binning, etc.
// There are 3 scenarios of fast image capture during preview (using preview image)
// for the different cameras :
// 1. Capture during preview impossible for this camera.
// 2. Capture during preview always available.
// 3. Capture during preview possible not for all input image formats and can be
// incompatible with some preview modes.
// In last case when it is impossible for current input image format for some
// purposes we can capture image in small format (e.g. for focus calculation), in other
// circumstances input format can not be changed.
enum PreviewModes
{
	// Default mode, can be incompatible with fast capture
	Preview_Fast = 0,
	Preview_Default = Preview_Fast,
	// Mode compatible with fast capture without changing captured image format.
	Preview_Full = 1,
	Preview_FastCapture = Preview_Full,
	// Mode compatible with fast capture, captured image format can be changed.
	Preview_FastCaptureSmall = 2,
};

interface IInputPreview2 : public IInputPreview
{
	com_call BeginPreview(int nDevice, IInputPreviewSite *pSite) = 0;
	com_call EndPreview(int nDevice, IInputPreviewSite *pSite) = 0;
	com_call GetPreviewFreq(int nDevice, DWORD *pdwFreq) = 0;
	com_call GetSize(int nDevice, short *pdx, short *pdy) = 0;
	com_call DrawRect(int nDevice, HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst) = 0;
	com_call GetPeriod(int nDevice, int nMode, DWORD *pdwPeriod) = 0;
	com_call GetPreviewMode(int nDevice, int *pnMode) = 0;
	// If preview mode can not be set, return E_FAIL
	com_call SetPreviewMode(int nDevice, int nMode) = 0;
};

interface IInputPreview3 : public IInputPreview2
{
	com_call MouseMove(UINT nFlags, POINT pt) = 0;
	com_call LButtonDown(UINT nFlags, POINT pt) = 0;
	com_call LButtonUp(UINT nFlags, POINT pt) = 0;
	com_call RButtonDown(UINT nFlags, POINT pt) = 0;
	com_call RButtonUp(UINT nFlags, POINT pt) = 0;
	com_call SetCursor(UINT nFlags, POINT pt) = 0;
};

interface IInputWindow : public IUnknown
{
	com_call CreateInputWindow(int nDev, HWND hwndParent, long *phCreated) = 0;
	com_call DestroyInputWindow(long lCap) = 0;
	com_call OnSize(long lCap, HWND hwndParent, SIZE sz) = 0;
	com_call GetSize(long lCap, short *pdx, short *pdy) = 0;
	com_call SetInputWindowPos(long lCap, int x, int y, int cx, int cy, DWORD dwFlags) = 0;
};

interface IInputWndOverlay : public IUnknown
{
	com_call InitOverlay() = 0;
	com_call DeinitOverlay() = 0;
	com_call GetSize(short *pdx, short *pdy) = 0;
	com_call StartOverlay(HWND hwndDraw, LPRECT lprcSrc, LPRECT lprcDst) = 0;
	com_call StopOverlay(HWND hwndDraw) = 0;
	com_call SetDstRect(HWND hwndDraw, LPRECT lprcDst) = 0;
	com_call SetSrcRect(HWND hwndDraw, LPRECT lprcSrc) = 0;
};

interface IDSCompressionSite
{
	virtual int GetCompressorsCount() = 0;
	virtual BSTR GetCompressorName(int iCompr) = 0;
	virtual void SetCompressor(int iCompr) = 0;
	virtual void CompressorDlg(int iCompr) = 0;
	virtual bool HasCompressorDialog(int iCompr) = 0;
};

interface IDocCreater : public IUnknown
{
	com_call ExecuteSettings(HWND hwndParent, BSTR bstrDocType, int nDialogMode) = 0;
	com_call InitDocument(IUnknown *punkDoc) = 0;
};

enum CDriverDialogType
{
	DriverDialog_ChildDialog = 0,
	DriverDialog_SettingsPanel = 1,
	DriverDialog_PreviewDialog = 2,
};

interface IDriverDialog : public IUnknown
{
	virtual int GetId() = 0;
	virtual int GetDialogType() = 0; // On of CDriverDialogType
	// makes sence only for DriverDialog_PreviewDialog
	virtual void AttachChildDialogToButton(int idButton, int idChildDlgRes) = 0;
	virtual void OnResetSettings(LPCTSTR lpstrSec, LPCTSTR lpstrEntry) = 0;
	virtual void UpdateCmdUI() = 0;
};

interface IDriverDialogSite : public IUnknown
{
	com_call OnInitDialog(IDriverDialog *pDlg) = 0;
	com_call OnInitDlgSize(IDriverDialog *pDlg) = 0;
	com_call OnCmdMsg(int nCode, UINT nID, long lHWND) = 0;
	// Called inside base driver's OnInitDialog. There are four possible variants:
	// 1. GetControlInfo returns S_OK and pvie->bstrSection/bstrEntry is not null. Base driver
	//    dialog will handle control notifications and notify driver via IDriver2::SetValuexxx.
	// 2. GetControlInfo returns S_OK and pvie->bstrSection/bstrEntry is null. Base driver dialog
	//    will notify driver via IDriverDialogSite.
	// 3. GetControlInfo returns S_FALSE. Base driver not initializes the control but leaves
	//    it enabled.
	// 4. GetControlInfo returns any other value. Base driver dialog will disable control.
	com_call GetControlInfo(int nID, VALUEINFOEX2 *pValueInfoEx2) = 0;
	com_call OnSetControlInt(int nIDCtrl, int nValue) = 0;
	com_call OnUpdateCmdUI(HWND hwnd, int nId) = 0;
	com_call OnCloseDialog(IDriverDialog *pDlg) = 0;
	// Called from main thread after each call of IDriverSite::Invalidate, usually from base
	// driver dialog's processing of WM_APP or WM_TIMER
	com_call OnDelayedRepaint(HWND hwndDlg, IDriverDialog *pDlg) = 0;
};

interface IDrvControl
{
	// Reread current value from profile
	virtual bool Reset() = 0;
	// Reset all settings: minimum, maximum, default, current value, list for listboxes and
	// comboboxes, etc.
	virtual bool Reinit() = 0;
};

interface IDrvControlSite
{
	virtual void AddControl(IDrvControl *pControl) = 0;
	virtual void RemoveControl(IDrvControl *pControl) = 0;
	virtual void GetValueInfo(VALUEINFOEX2 *pvie2) = 0;
};

declare_interface(IDriverManager, "394913EA-2CEC-463B-AE00-1388A2FD6F3D");
declare_interface(IDriverManager2, "C4AAFA42-19F2-475E-9A20-276B92D2D728");
declare_interface(IDriver, "274498B8-961A-4B1D-9059-FBBF5C94298E");
declare_interface(IDriver2, "E8E7BBD3-5033-4670-8FE1-286CE9AFCA9F");
declare_interface(IDriver3, "AE14B40E-93A7-4C35-996F-0C4CAEEFE867");
declare_interface(IDriver4, "A6E9367A-B431-4148-A020-D2F9C9ED87CE");
declare_interface(IDriver5, "E3CFF65A-0345-4990-B19C-DEA0465E0145");
declare_interface(IDriver6, "E16822C0-5F51-4097-A798-68E66D3C984F");
declare_interface(IDriverSite, "187F9F7A-F456-4AC2-A524-C1F0EE6B0701"); // == IInputPreviewSite
declare_interface(IInputPreview, "DBBEAA83-4DFC-4E60-91E8-C36F729BC859");
declare_interface(IInputPreview2, "784796D3-24BE-4F51-8742-807D6091598C");
declare_interface(IInputPreview3, "4F8F4FEB-E92F-4A2C-9581-C41FF5C08627");
declare_interface(IInputPreviewSite, "187F9F7A-F456-4AC2-A524-C1F0EE6B0701");
declare_interface(IInputWindow, "6672550C-E800-4470-957C-FB6323EBAC36");
declare_interface(IInputWndOverlay, "7262FC09-CEAD-4214-8EBC-8FAE15F3FDF7");
declare_interface(IDocCreater, "52DA7B27-BC46-47e3-915D-BEF320F691D2");
declare_interface(IDriverDialog, "A4C40265-C623-481f-AD89-FC87C4EADCC0");
declare_interface(IDriverDialogSite, "E400072E-974E-49de-84AA-6C2C278B5A8A");

#endif