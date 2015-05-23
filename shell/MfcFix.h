#if _MFC_VER == 0x0700
#define MFCXXD_DLL _T("mfc70d.dll")
#elif  _MFC_VER == 0x0710
#define MFCXXD_DLL _T("mfc71d.dll")
#elif  _MFC_VER == 0x0800
#define MFCXXD_DLL _T("mfc80d.dll")
#define GetInterface_CCmdTarget_Ordinal 3978
#define InvokeHelperV_COleDispatchDriver_Ordinal 5430
#elif  _MFC_VER == 0x0900
#define MFCXXD_DLL _T("mfc90d.dll")
// GetInterface@CCmdTarget
#define GetInterface_CCmdTarget_Ordinal 4072
// void __cdecl COleDispatchDriver::InvokeHelperV(long,unsigned short,unsigned short,void *,unsigned char const *,char *)
#define InvokeHelperV_COleDispatchDriver_Ordinal 5555
#elif  _MFC_VER == 0x0C00
#define MFCXXD_DLL _T("mfc120d.dll")
#define GetInterface_CCmdTarget_Ordinal 6630
#define InvokeHelperV_COleDispatchDriver_Ordinal 9269
#endif


bool _install_function_hook( BYTE* orig_code, LPVOID pfunc_addr_old, LPVOID pfunc_addr_new );
