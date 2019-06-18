// #define DEMOVERSION

#include "\vt5\crypto\crypto.h"
#include "alloc.h"
#include "image5.h"
#include "misc_utils.h"
#include <signal.h>
#include "alloc.h"

#define DEMOVALUE 2182945896

#ifdef DEMOVERSION
	#pragma message ( "DEMOVERSION BuilD" )
#endif

static bool ExtractSignature(IImage2 *pImage, byte *pDigest, int nSize)
{
	if(pImage==0) return false;

	int cx = 0, cy = 0;
	pImage->GetSize( &cx, &cy );
	if(cx*cy<nSize*8) return false; // �� �������

	color *pRow = 0;
	memset(pDigest, 0, nSize );
	for(int nBit=0; nBit<nSize*8; nBit++)
	{
		int x = nBit % cx;
		int y = nBit / cx;
		if(x==0) pImage->GetRow( y, 0, &pRow); // 0 ������ 0 ����
		if( !pRow )
			return false;
		int i=nBit/8, j=nBit%8; // ����� ����� � ���� � ���
		if( pRow[x] & 0x0100 ) // ������� �� 8 ���
			pDigest[i] |= (1<<j);
	}
	return true;
}

static bool StoreSignature(IImage2 *pImage, byte *pDigest, int nSize)
{
	if(pImage==0) return false;

	int cx = 0, cy = 0;
	pImage->GetSize( &cx, &cy );
	if(cx*cy<nSize*8) return false; // �� �������

	color *pRow = 0;
	for(int nBit=0; nBit<nSize*8; nBit++)
	{
		int x = nBit % cx;
		int y = nBit / cx;
		if(x==0) pImage->GetRow( y, 0, &pRow); // 0 ������ 0 ����
		if( !pRow )
			return false;
		int i=nBit/8, j=nBit%8; // ����� ����� � ���� � ���
		pRow[x] &= 0xFE00;
		if( pDigest[i] & (1<<j) ) pRow[x] |= 0x0100; // ��������� ������� �� 8 ���
	}

	return true;
}

static vlong ExtractVLong(IImage2 *pImage)
{
	WORD wLen;
	vlong v = 0;
	ExtractSignature(pImage, (byte*)(&wLen), sizeof(wLen));
	wLen = min(wLen, 2048/8/sizeof(unsigned)); // �� ����� 2048 ���
	smart_alloc(buf, byte, sizeof(wLen) + wLen*sizeof(unsigned));
	ExtractSignature(pImage, buf, sizeof(wLen) + wLen*sizeof(unsigned));
	unsigned* pData = (unsigned*)(buf+sizeof(wLen));
	for(int i=0; i<wLen; i++) v.set_data(i, pData[i]);
	return v;
}

static bool StoreVLong(IImage2 *pImage, vlong v)
{
	WORD wLen = v.get_data_size();
	unsigned *pData = v.get_data_buf();
	smart_alloc(buf, byte, sizeof(wLen) + wLen*sizeof(unsigned));
	memcpy(buf, &wLen, sizeof(wLen));
	memcpy(buf + sizeof(wLen), pData, wLen*sizeof(unsigned));
	StoreSignature( pImage, buf, sizeof(wLen) + wLen*sizeof(unsigned) );
	return true;
}

static bool CalcSignature(IImage2 *pImage, byte *pDigest, int nSize)
{
	int nPaneCount = ::GetImagePaneCount( pImage );
	int cx = 0, cy = 0;
	pImage->GetSize( &cx, &cy );

	MD5_CTX context;
	MD5Init(&context);
	MD5Update(&context,(byte*)(&nPaneCount),sizeof(nPaneCount));
	MD5Update(&context,(byte*)(&cx),sizeof(cx));
	MD5Update(&context,(byte*)(&cy),sizeof(cy));

	smart_alloc (buf, byte, cx);

	const int nBorder=2;
	if(cx>=nBorder*2 && cy>=nBorder*2)
	{
		for( int nPane = 0; nPane < nPaneCount; nPane++ )
		{
			for( int y = nBorder; y < cy-nBorder; y++ ) // �� ������ ������ �������� �� ���� ����� ������ � �����
			{
				color *pRow = 0;
				pImage->GetRow( y, nPane, &pRow);
				if( !pRow )
					return false;
				for( int x=nBorder; x<cx-nBorder; x++ ) // �� ������ ������ �������� �� ���� �������� ������ � �����
					buf[x] = pRow[x] >> 9;
				// ���������� ������ ������� 7 ��� - ����� ����� ���� � ������� ������ ����
				MD5Update(&context,buf+nBorder,cx-nBorder*2);
			}
		}
	}
	MD5Final (pDigest,&context);
	return true;
}

#ifdef DEMOVERSION
static char szsssss[] = "CheckImageSignatureTESTTESTTEST";
#endif

static inline bool check_signature(byte *pDigest, vlong v, bool bSimple=false)
{
	// bSimple = true - ��������� "�������" ������� (�� ��������� �����, ������ MD5)
	vlong v2;
	if(bSimple) v2 = v;
	else v2 = get_public_key().encrypt(v);
	vlong v1=0;
	{
		unsigned* p = (unsigned*)pDigest;
		for(int i=0; i<16/sizeof(unsigned); i++) v1.set_data(i,p[i]);
	}
	return v1==v2;
}

static inline bool CheckImageSignature(IUnknown *punkImage)
{
#ifdef DEMOVERSION
	szsssss[0] = 'C';

	IImage2Ptr ptrImage(punkImage);
	if(ptrImage==0) return false;

	int cx,cy;
	ptrImage->GetSize(&cx,&cy);
	if(cx*cy<2048) return false;

	DWORD dwFlags = 0;
	ptrImage->GetImageFlags( &dwFlags );
	if( dwFlags & ifVirtual ) return false; // �� ��������� �����������

	byte digest[16] = {0};
	CalcSignature(ptrImage, digest, sizeof(digest));

	vlong v = ExtractVLong(ptrImage);

	if( !check_signature(digest, v) )
	{
		int nPaneCount = ::GetImagePaneCount( ptrImage );
		int cx = 0, cy = 0;
		ptrImage->GetSize( &cx, &cy );

		for( int nPane = 0; nPane < nPaneCount; nPane++ )
		{
			for( int y = 0; y < cy; y++ )
			{
				color *pRow = 0;
				ptrImage->GetRow( y, nPane, &pRow);
				if( !pRow )
					return false;
				for( int x=0; x<cx; x++ ) 
					pRow[x] += (rand()&32767) - (rand()&32767); // ����������
			}
		}
		return false;
	};
	return true;
#else // #ifdef DEMOVERSION
	return false; // �� ���������
#endif // #ifdef DEMOVERSION
}

static inline bool CheckDllSignature(char *szLib)
{ // �� �����
#ifdef DEMOVERSION
	// AAM: ��������� DLL �� ��������
	{
		char	sz_file[MAX_PATH];
		::GetModuleFileName( 0, sz_file, sizeof( sz_file ) );
		strcpy( strrchr(sz_file, '\\') + 1, szLib );
		HANDLE hLib = ::GetModuleHandle(sz_file);
		if(hLib == 0)
		{
			MessageBox(0, "Required library not loaded", szLib, MB_OK | MB_ICONERROR);
			raise(SIGABRT);
		}


		byte digest[16];
		MD5_CTX context;
		MD5Init(&context);

		FILE *f = fopen(sz_file,"rb");
		if(!f)
		{
			MessageBox(0, "No such file", szLib, MB_OK | MB_ICONERROR);
			raise(SIGABRT);
		}
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

		FILE *f2 = fopen(sz_file_chk,"rt");
		if(!f2)
		{
			MessageBox(0, "No digital signature", szLib, MB_OK | MB_ICONERROR);
			raise(SIGABRT);
		}
		int nMaxStringLen = vlong_to_string(get_public_key().m, 0, 0);
		smart_alloc(sig_buf, char, nMaxStringLen+128); // �� ������ �������� ���� ������
		fgets(sig_buf, nMaxStringLen+120, f2);
		fclose(f2);
		vlong v = string_to_vlong(sig_buf,nMaxStringLen);
		
		if( !check_signature(digest, v) )
		{
			MessageBox(0, "Invalid library", szLib, MB_OK | MB_ICONERROR);
			raise(SIGABRT);
		}
	}
	return true;

#else // #ifdef DEMOVERSION
	return false; // OK
#endif // #ifdef DEMOVERSION
}

static inline bool CheckFileSignature(const char *sz_file, bool bSimple=false)
{
	// AAM: ��������� ���� �� �������� (������������ �������)
	// bSimple = true - ��������� "�������" ������� (�� ��������� �����, ������ MD5)
	{
		byte digest[16];
		MD5_CTX context;
		MD5Init(&context);

		FILE *f = fopen(sz_file,"rb");
		if(!f) return false; // ��� �����

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

		FILE *f2 = fopen(sz_file_chk,"rt");
		if(!f2) return false; // ��� �������

		int nMaxStringLen = vlong_to_string(get_public_key().m, 0, 0);
		smart_alloc(sig_buf, char, nMaxStringLen+128); // �� ������ �������� ���� ������
		fgets(sig_buf, nMaxStringLen+120, f2);
		fclose(f2);
		vlong v = string_to_vlong(sig_buf,nMaxStringLen);
		
		if( !check_signature(digest, v, bSimple) ) return false; // ������� �� ���������
	}
	return true;
}

static inline bool CreateFileSignature(const char *sz_file, bool bSimple=false)
{
	// AAM: ��������� ����. ������� ������� \vt5\crypto\private_key.h
	byte digest[16];
	MD5_CTX context;
	MD5Init(&context);

	FILE *f = fopen(sz_file,"rb");
	if(!f) return false; // ��� �����

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

	vlong v_new;
	if(bSimple)
		v_new = v1;
	else
	{
		private_key key2 = read_key("\\vt5\\crypto\\private_key.h");
		if(key2.p==0 || key2.q==0) return false; // �� ���������� ��������� ����
		v_new = key2.decrypt(v1);
	}

	int nMaxStringLen = vlong_to_string(v_new, 0, 0)+128;
	smart_alloc(sig_data, char, nMaxStringLen);
	ZeroMemory(sig_data, nMaxStringLen);
	vlong_to_string(v_new, sig_data, nMaxStringLen);

	FILE *f2 = fopen(sz_file_chk,"wa");
	if(!f2) return false; // �� ������� ������� �������

	fputs(sig_data, f2);
	fclose(f2);

	return true;
}
