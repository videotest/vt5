#include "stdafx.h"
#include "Signature.h"
#include "alloc.h"
#include "ImageSignature.h"

#define MaxColor color(-1)

_ainfo_base::arg        CSignatureFilterInfo::s_pargs[] = 
{       
	{"ImgSrc",       szArgumentTypeImage, 0, true, true },
	{"ImgResult",    szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};

CSignatureFilter::CSignatureFilter(void)
{
}

CSignatureFilter::~CSignatureFilter(void)
{
}

bool CSignatureFilter::InvokeFilter()
{
	IImage3Ptr	ptrSrcImage( GetDataArgument( "ImgSrc" ) );

	IImage3Ptr	ptrDstImage( GetDataResult() );
	
	if( ptrSrcImage == 0 || ptrDstImage == 0 )
		return false;

	ICompatibleObjectPtr ptrCO = ptrDstImage;
	
	if( ptrCO == 0 ) 
		return false;
	
	if( ptrCO->CreateCompatibleObject( ptrSrcImage, 0, 0 ) != S_OK )
		return false;

	int cx = 0, cy = 0;
	ptrSrcImage->GetSize( &cx, &cy );

	if(cx*cy<2048) return false; // работаем только с картинками, у которых достаточен размер

	// общая часть для проверки и установки
	byte digest[16] = {0};
	CalcSignature(ptrDstImage, digest, sizeof(digest));

	// проверка
	vlong v = ExtractVLong(ptrDstImage);
	bool bErr = ! check_signature(digest, v);

	// установка
	vlong v1=0;
	unsigned* p = (unsigned*)digest;
	for(int i=0; i<16/sizeof(unsigned); i++) v1.set_data(i,p[i]);

	_bstr_t bstrKeyPath = "\\vt5\\crypto\\private_key.h";
	// bstrKeyPath = ::MakeAppPathName( "private_key.h" );
	bstrKeyPath = ::GetValueString( ::GetAppUnknown(), "\\Signature", "PrivateKeyFile", bstrKeyPath );
	private_key key2 = read_key(bstrKeyPath);
	if(key2.p==0)
	{
		::MessageBox(0, "Private key not found", "Error", MB_OK | MB_ICONERROR );
		return false;
	}
	vlong v_new = key2.decrypt(v1);
	StoreVLong(ptrDstImage, v_new);

	//StoreSignature(ptrDstImage, digest, sizeof(digest));

	StartNotification( 1, 1 );
	FinishNotification();

	return true;
}
