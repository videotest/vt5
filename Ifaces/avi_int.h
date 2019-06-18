#ifndef __avi_int_h__
#define __avi_int_h__

#ifdef _DEBUG
#import "\vt5\vt5\debug\comps\avi.tlb" exclude ("IUnknown", "GUID", "_GUID", "tagPOINT") no_namespace raw_interfaces_only named_guids 
#else
#import "\vt5\vt5\release\comps\avi.tlb" exclude ("IUnknown", "GUID", "_GUID", "tagPOINT") no_namespace raw_interfaces_only named_guids 
#endif

#define szAviSection			"\\Avi"

//AVI Gallery View
#define szThumbnailWidth		"ThumbnailWidth"
#define szThumbnailHeight		"ThumbnailHeight"
#define szGalleryFrameStep		"GalleryFrameStep"

//Play Action
#define szAutoPlay				"AutoPlay"
#define szAutoStop				"AutoStop"

//double buffering
#define szEnableDoubleBuffering	"EnableDoubleBuffering"
#define szEvenThenOdd			"EvenThenOdd"

#define szEvenStageRowOffset	"EvenStageRowOffset"
#define szOddStageRowOffset		"OddStageRowOffset"

/////////////////////////////////////////////////////////////////
//Avi Object
// {D6D93136-5A88-47bb-A119-D491A64A5937}
static const GUID clsidAviObj = CLSID_AviImage;
//{ 0xd6d93136, 0x5a88, 0x47bb, { 0xa1, 0x19, 0xd4, 0x91, 0xa6, 0x4a, 0x59, 0x37 } };
#define szTypeAviObjProgID		"AviBase.AviObject"

// {F46CAFAC-A92E-408e-A9B1-EFAE999D5BBA}
static const GUID clsidAviObjInfo = 
{ 0xf46cafac, 0xa92e, 0x408e, { 0xa9, 0xb1, 0xef, 0xae, 0x99, 0x9d, 0x5b, 0xba } };
#define szTypeAviObjInfoProgID "AviBase.AviObjInfo"

// {80A6B33F-9E9E-4b6b-89D3-60027658B423}
static const GUID clsidAviPlayPage = 
{ 0x80a6b33f, 0x9e9e, 0x4b6b, { 0x89, 0xd3, 0x60, 0x2, 0x76, 0x58, 0xb4, 0x23 } };
#define szAviPlayPageProgID "AviBase.AviPlayPropPage"


// {7701E119-E42D-47d9-A25E-3F9BD45BD80C}
static const GUID clsidAviGalleryView = 
{ 0x7701e119, 0xe42d, 0x47d9, { 0xa2, 0x5e, 0x3f, 0x9b, 0xd4, 0x5b, 0xd8, 0xc } };
#define szAviGalleryViewProgID "AviBase.AviGallery"

// {F1B070CE-3405-4bb1-B1DE-936C80F00C7A}
static const GUID clsidAviGalleryPage = 
{ 0xf1b070ce, 0x3405, 0x4bb1, { 0xb1, 0xde, 0x93, 0x6c, 0x80, 0xf0, 0xc, 0x7a } };
#define szAviGalleryPageProgID "AviBase.AviGalleryPropPage"

#endif //__avi_int_h__