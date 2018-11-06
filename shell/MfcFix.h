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
#elif  _MFC_VER == 0x0A00
#define MFCXXD_DLL _T("mfc100d.dll")
#define GetInterface_CCmdTarget_Ordinal 6077
#define InvokeHelperV_COleDispatchDriver_Ordinal 8558
#define CWnd_InitControlContainer_Ordinal 8370
#define COleControlSite_InvokeHelperV_Ordinal 8557
#define COleControlSite_SetPropertyV_Ordinal 14609
#define COleControlSite_SafeSetProperty_Ordinal 13563
#define COleControlSite_ModifyStyle_Ordinal 9303
#define COleControlSite_SetWindowTextA_Ordinal 8605
#define COleControlSite_CreateControlCommon_Ordinal 3368
#define COleControlSite_CreateControl_5POINT_Ordinal 3362
#define COleControlContainer_CreateControlCommon_Ordinal 3367
#define COleControlContainer_CreateControl_2GUID_5POINT_Ordinal 3358
#define CWnd_CreateControl_4POINT_Ordinal 3364
#define CWnd_CreateControl_4RECT_Ordinal 3363 
#define CWnd_CreateControl_Ordinal_1char 3366
#define COleControlSite_InvokeHelper_Ordinal 8554
#define COleControlSite_GetProperty_Ordinal 6773
#define COleControlSite_GetExStyle_Ordinal 5821
#define CWnd_GetExStyle_Ordinal 5822
#elif  _MFC_VER == 0x0B00
#define MFCXXD_DLL _T("mfc110d.dll")
//#define GetInterface_CCmdTarget_Ordinal 
//#define InvokeHelperV_COleDispatchDriver_Ordinal
#define CWnd_InitControlContainer_Ordinal 5422
#define COleControlSite_InvokeHelperV_Ordinal 5554
#define COleControlSite_SetPropertyV_Ordinal 8394
#define COleControlSite_SafeSetProperty_Ordinal  7759
#define COleControlSite_ModifyStyle_Ordinal  5909
#define COleControlSite_SetWindowTextA_Ordinal  8605
#define COleControlSite_CreateControlCommon_Ordinal  2234
#define COleControlSite_CreateControl_5POINT_Ordinal  2228
#define COleControlContainer_CreateControlCommon_Ordinal  2233
#define COleControlContainer_CreateControl_2GUID_5POINT_Ordinal  2224
#define CWnd_CreateControl_4POINT_Ordinal  2230
#define CWnd_CreateControl_4RECT_Ordinal  2229
#define CWnd_CreateControl_Ordinal_1char  2232
#define COleControlSite_InvokeHelper_Ordinal  5551
#define COleControlSite_GetProperty_Ordinal  4511
#define COleControlSite_GetExStyle_Ordinal  3881
#define CWnd_GetExStyle_Ordinal  3882
#elif  _MFC_VER == 0x0C00
#define MFCXXD_DLL _T("mfc120d.dll")
#define GetInterface_CCmdTarget_Ordinal 6630
#define InvokeHelperV_COleDispatchDriver_Ordinal 9269
#endif


bool _install_function_hook( BYTE* orig_code, LPVOID pfunc_addr_old, LPVOID pfunc_addr_new );
