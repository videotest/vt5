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
#if defined(_WIN64)
#define GetInterface_CCmdTarget_Ordinal 4072
// void __cdecl COleDispatchDriver::InvokeHelperV(long,unsigned short,unsigned short,void *,unsigned char const *,char *)
#define InvokeHelperV_COleDispatchDriver_Ordinal 5555
#else
// GetInterface@CCmdTarget
#define GetInterface_CCmdTarget_Ordinal 4299
// void __cdecl COleDispatchDriver::InvokeHelperV(long,unsigned short,unsigned short,void *,unsigned char const *,char *)
#define InvokeHelperV_COleDispatchDriver_Ordinal 5861
#endif
#elif  _MFC_VER == 0x0A00
#define MFCXXD_DLL _T("mfc100d.dll")
#if defined(_WIN64)
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
#define CWnd_CreateControl_1char_Ordinal 3366
#define COleControlSite_InvokeHelper_Ordinal 8554
#define COleControlSite_GetProperty_Ordinal 6773
#define COleControlSite_GetExStyle_Ordinal 5821
#define CWnd_GetExStyle_Ordinal 5822
#else
#define GetInterface_CCmdTarget_Ordinal 6304
#define InvokeHelperV_COleDispatchDriver_Ordinal 8864
#define CWnd_InitControlContainer_Ordinal 8662
#define COleControlSite_InvokeHelperV_Ordinal 8863
#define COleControlSite_SetPropertyV_Ordinal 15005
#define COleControlSite_SafeSetProperty_Ordinal 13915
#define COleControlSite_ModifyStyle_Ordinal 9633
#define COleControlSite_SetWindowTextA_Ordinal 15333
#define COleControlSite_CreateControlCommon_Ordinal 3467
#define COleControlSite_CreateControl_5POINT_Ordinal 3458
#define COleControlContainer_CreateControlCommon_Ordinal 3467
#define COleControlContainer_CreateControl_2GUID_5POINT_Ordinal 3458
#define CWnd_CreateControl_4POINT_Ordinal 3459
#define CWnd_CreateControl_4RECT_Ordinal 3463 
#define CWnd_CreateControl_1char_Ordinal 3466
#define COleControlSite_InvokeHelper_Ordinal 8860
#define COleControlSite_GetProperty_Ordinal 7019
#define COleControlSite_GetExStyle_Ordinal 6026
#define CWnd_GetExStyle_Ordinal 6027
#endif
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
#define CWnd_CreateControl_1char_Ordinal  2232
#define COleControlSite_InvokeHelper_Ordinal  5551
#define COleControlSite_GetProperty_Ordinal  4511
#define COleControlSite_GetExStyle_Ordinal  3881
#define CWnd_GetExStyle_Ordinal  3882
#elif  _MFC_VER == 0x0C00
#define MFCXXD_DLL _T("mfc120d.dll")
#define GetInterface_CCmdTarget_Ordinal 6630
#define InvokeHelperV_COleDispatchDriver_Ordinal 9269
#elif  _MFC_VER == 0x0E00
#define MFCXXD_DLL _T("mfc140d.dll")
#if defined(_WIN64)
#define GetInterface_CCmdTarget_Ordinal 6727
#define InvokeHelperV_COleDispatchDriver_Ordinal 9386
#define CWnd_InitControlContainer_Ordinal 9190
#define COleControlSite_InvokeHelperV_Ordinal 9385
#define COleControlSite_SetPropertyV_Ordinal 15781
#define COleControlSite_SafeSetProperty_Ordinal 14663
#define COleControlSite_ModifyStyle_Ordinal 10152
#define COleControlSite_SetWindowTextA_Ordinal 16136
#define COleControlSite_CreateControlCommon_Ordinal 3789
#define COleControlSite_CreateControl_5POINT_Ordinal 3783
#define COleControlContainer_CreateControlCommon_Ordinal 3788
#define COleControlContainer_CreateControl_2GUID_5POINT_Ordinal 3779
#define CWnd_CreateControl_4POINT_Ordinal 3785
#define CWnd_CreateControl_4RECT_Ordinal 3784
#define CWnd_CreateControl_1char_Ordinal 3787
#define COleControlSite_InvokeHelper_Ordinal 9382
#define COleControlSite_GetProperty_Ordinal 7445
#define COleControlSite_GetExStyle_Ordinal 6450
#define CWnd_GetExStyle_Ordinal 6451
#else
#define GetInterface_CCmdTarget_Ordinal 6954
#define InvokeHelperV_COleDispatchDriver_Ordinal 9689
#define CWnd_InitControlContainer_Ordinal
#define COleControlSite_InvokeHelperV_Ordinal
#define COleControlSite_SetPropertyV_Ordinal 
#define COleControlSite_SafeSetProperty_Ordinal 
#define COleControlSite_ModifyStyle_Ordinal
#define COleControlSite_SetWindowTextA_Ordinal
#define COleControlSite_CreateControlCommon_Ordinal
#define COleControlSite_CreateControl_5POINT_Ordinal 
#define COleControlContainer_CreateControlCommon_Ordinal 
#define COleControlContainer_CreateControl_2GUID_5POINT_Ordinal 
#define CWnd_CreateControl_4POINT_Ordinal 
#define CWnd_CreateControl_4RECT_Ordinal  
#define CWnd_CreateControl_1char_Ordinal 
#define COleControlSite_InvokeHelper_Ordinal 
#define COleControlSite_GetProperty_Ordinal 
#define COleControlSite_GetExStyle_Ordinal 
#define CWnd_GetExStyle_Ordinal 
#endif
#endif


bool _install_function_hook( BYTE* orig_code, LPVOID pfunc_addr_old, LPVOID pfunc_addr_new );
