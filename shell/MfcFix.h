#if _MFC_VER == 0x0700
#define MFCXXD_DLL _T("mfc70d.dll")
#elif _MFC_VER == 0x0710
#define MFCXXD_DLL _T("mfc71d.dll")
#elif  _MFC_VER == 0x0800
#define MFCXXD_DLL _T("mfc80d.dll")
#elif  _MFC_VER == 0x0900
#define MFCXXD_DLL _T("mfc90d.dll")
#endif

bool _install_function_hook( BYTE* orig_code, LPVOID pfunc_addr_old, LPVOID pfunc_addr_new );
