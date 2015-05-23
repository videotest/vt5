bool _install_function_hook( BYTE* orig_code, 
	LPVOID pfunc_addr_old, LPVOID pfunc_addr_new );
namespace{
BYTE __GetOriginalCode[16]	= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
}
