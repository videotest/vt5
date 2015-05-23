#ifndef _int_faces
#define _int_faces

#include <comdef.h>


#define com_call virtual HRESULT STDMETHODCALLTYPE

const IID IID_IVTPrintCtrl = {0xE69689EF,0x216B,0x42d3,{0xb1,0x39,0xc0,0x33,0xa6,0xd7,0x58,0xf2}};			
	

struct __declspec(uuid("E69689EF-216B-42d3-B139-C033A6D758F2")) 
IVTPrintCtrl : public IUnknown
{
	com_call Draw( HDC hDC, SIZE sizeVTPixel ) = 0;
	com_call FlipHorizontal(  ) = 0;
	com_call FlipVertical(  ) = 0;	
	com_call SetPageInfo( int nCurPage, int nPageCount ) = 0;
	com_call NeedUpdate( BOOL* pbUpdate ) = 0;

};


_COM_SMARTPTR_TYPEDEF(IVTPrintCtrl, __uuidof(IVTPrintCtrl));




#define IMPLEMENT_UNKNOWN(CVT3DBar, PrintCtrl)\
    STDMETHODIMP CVT3DBar::X##PrintCtrl::QueryInterface(REFIID riid, void **ppv)\
	{\
		if( riid == IID_IVTPrintCtrl )\
		{\
			*ppv = static_cast<IVTPrintCtrl*>(this);\
			AddRef();\
			return S_OK;\
		}\
		return S_OK;\
	}\
    ULONG STDMETHODCALLTYPE CVT3DBar::X##PrintCtrl::AddRef()\
	{\
		return 1;\
	}\
    ULONG STDMETHODCALLTYPE CVT3DBar::X##PrintCtrl::Release()\
	{\
		return 0;\
	}\


#endif //_int_faces
