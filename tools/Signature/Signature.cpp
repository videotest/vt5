// Signature.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "\vt5\crypto\crypto.h"
#include "windows.h"
#include "\vt5\common\alloc.h"

typedef unsigned char byte;

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc<2)
	{
		puts("Usage: Signature file_to_sign");
		return 1;
	}

	byte digest[16];
	MD5_CTX context;
	MD5Init(&context);

	char *sz_file = argv[1];
	FILE *f = fopen(sz_file,"rb");
	byte buf[1024];
	while(int nRead = fread(buf,1,sizeof(buf),f))
	{
		MD5Update(&context,buf,nRead);
	}
	MD5Final (digest,&context);
	fclose(f);

	char	sz_file_chk[MAX_PATH];
	strcpy( sz_file_chk, sz_file );
	strcpy( strrchr(sz_file_chk, 0), ".chk" );

	vlong v1=0;
	unsigned* p = (unsigned*)digest;
	for(int i=0; i<16/sizeof(unsigned); i++) v1.set_data(i,p[i]);

	private_key key2 = read_key("\\vt5\\crypto\\private_key.h");
	vlong v_new = key2.decrypt(v1);

	int nMaxStringLen = vlong_to_string(key2.m, 0, 0);
	smart_alloc(sig_data, char, nMaxStringLen);
	ZeroMemory(sig_data, nMaxStringLen);
	vlong_to_string(v_new, sig_data, nMaxStringLen);

	FILE *f2 = fopen(sz_file_chk,"wa");
	fputs(sig_data, f2);
	fclose(f2);

	return 0;
}

