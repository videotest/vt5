#ifndef __avi_base_h__
#define __avi_base_h__

#include "nameconsts.h"
#include "\vt5\ifaces\avi_int.h"
#include "window5.h"

IAviImagePtr GetActiveAviFromContext( IUnknown* punkContext );
IAviImagePtr GetActiveAviFromDoc( IUnknown* punkDoc );

IUnknown*	CreateAviObject();

bool GetPropPageStuff( IPropertySheetPtr& ptrSheet, IOptionsPagePtr& ptrPage, int& nPageNum, CLSID clsid );

#define szDllName	"AviBase"

/////////////////////////////////////////////////////////////////
//Action list
// {B5233BA3-13C5-41d5-A84A-9F1BDA0A226A}
static const GUID clsidInsertAviObjectInfo = 
{ 0xb5233ba3, 0x13c5, 0x41d5, { 0xa8, 0x4a, 0x9f, 0x1b, 0xda, 0xa, 0x22, 0x6a } };
#define szInsertAviObjectProgID "AviBase.InsertAviObject"


// {72CAE3B4-8F30-4e94-A54F-665E6D5952A7}
static const GUID clsidAviMoveFirstInfo = 
{ 0x72cae3b4, 0x8f30, 0x4e94, { 0xa5, 0x4f, 0x66, 0x5e, 0x6d, 0x59, 0x52, 0xa7 } };
#define szAviMoveFirstProgID "AviBase.AviMoveFirst"

// {1F83ED82-D542-4794-8046-AE93DAB887C4}
static const GUID clsidAviMoveNextInfo = 
{ 0x1f83ed82, 0xd542, 0x4794, { 0x80, 0x46, 0xae, 0x93, 0xda, 0xb8, 0x87, 0xc4 } };
#define szAviMoveNextProgID "AviBase.AviMoveNext"


// {59C8AC36-1448-4a43-BCC9-F64D36BE6442}
static const GUID clsidAviMoveLastInfo = 
{ 0x59c8ac36, 0x1448, 0x4a43, { 0xbc, 0xc9, 0xf6, 0x4d, 0x36, 0xbe, 0x64, 0x42 } };
#define szAviMoveLastProgID "AviBase.AviMoveLast"


// {ECD5FE53-D8C9-4eae-89E9-9A5AF4B7856C}
static const GUID clsidAviMovePrevInfo = 
{ 0xecd5fe53, 0xd8c9, 0x4eae, { 0x89, 0xe9, 0x9a, 0x5a, 0xf4, 0xb7, 0x85, 0x6c } };
#define szAviMovePrevProgID "AviBase.AviMovePrev"


// {2ED7B622-7980-47ec-A73A-2AF6952CF7B9}
static const GUID clsidAviMoveToInfo = 
{ 0x2ed7b622, 0x7980, 0x47ec, { 0xa7, 0x3a, 0x2a, 0xf6, 0x95, 0x2c, 0xf7, 0xb9 } };
#define szAviMoveToProgID "AviBase.AviMoveTo"

// {3C3C80C0-F02E-441c-B175-C2ED23A790AA}
static const GUID clsidAviPlay = 
{ 0x3c3c80c0, 0xf02e, 0x441c, { 0xb1, 0x75, 0xc2, 0xed, 0x23, 0xa7, 0x90, 0xaa } };
#define szAviPlayProgID "AviBase.AviPlay"

// {1252F41C-9035-42bf-80FB-30A4C69E393E}
static const GUID clsidAviStop = 
{ 0x1252f41c, 0x9035, 0x42bf, { 0x80, 0xfb, 0x30, 0xa4, 0xc6, 0x9e, 0x39, 0x3e } };
#define szAviStopProgID "AviBase.AviStop"


// {6F1496DA-763A-4bc5-9F19-D94C36A9194C}
static const GUID clsidShowAviGallery = 
{ 0x6f1496da, 0x763a, 0x4bc5, { 0x9f, 0x19, 0xd9, 0x4c, 0x36, 0xa9, 0x19, 0x4c } };
#define szShowAviGalleryProgID "AviBase.ShowAviGallery"

// {5272731D-9246-42d5-B20D-ED1F46F1370D}
static const GUID clsidAviSignature = 
{ 0x5272731d, 0x9246, 0x42d5, { 0xb2, 0xd, 0xed, 0x1f, 0x46, 0xf1, 0x37, 0xd } };
#define szAviSignatureProgID "AviBase.AviSignature"

// {202F6443-F99B-4a88-8776-87137219F39C}
static const GUID clsidAviFileFilter = 
{ 0x202f6443, 0xf99b, 0x4a88, { 0x87, 0x76, 0x87, 0x13, 0x72, 0x19, 0xf3, 0x9c } };
#define szAviFileFilterProgID "AviBase.AviFileFilter"


#endif// __avi_base_h__
