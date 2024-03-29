#ifndef __imagefilters_h__
#define __imagefilters_h__


static const GUID clsidAutoBCInfo = 
{ 0xe5df94e6, 0xeb26, 0x4f94, { 0x82, 0x6f, 0xd8, 0xcd, 0x69, 0xb0, 0x95, 0xd1 } };
static const GUID clsidSelectColorInfo = 
{ 0xb4127aca, 0x2a94, 0x4d83, { 0xa5, 0x8, 0x88, 0x85, 0x6f, 0x50, 0xec, 0xbe } };
static const GUID clsidApplyLookupInfo = 
{ 0x44312e97, 0xdc32, 0x4e14, { 0x93, 0x40, 0xdb, 0x36, 0x7b, 0x72, 0x95, 0xc3 } };
static const GUID clsidCombineInfo = 
{ 0x9d309704, 0x3ca2, 0x433c, { 0x83, 0x94, 0xb8, 0x90, 0x4a, 0xb9, 0xc7, 0x9c } };
static const GUID clsidAddInfo = 
{ 0xca150473, 0x6a83, 0x4d0a, { 0xb2, 0x99, 0x5e, 0x44, 0xdf, 0x86, 0x29, 0xed } };
static const GUID clsidSubInfo = 
{ 0x5d9ea6c1, 0xaca1, 0x4542, { 0x95, 0x2f, 0x8d, 0x1d, 0x8e, 0x7e, 0xb3, 0xf1 } };
static const GUID clsidDiffInfo = 
{ 0xe171ceff, 0xf7be, 0x42c6, { 0xbd, 0xa4, 0xc1, 0xc, 0x7d, 0xb6, 0x4b, 0xb2 } };
static const GUID clsidSubLightInfo = 
{ 0x1f8daea8, 0xf3bc, 0x4125, { 0x9d, 0x3e, 0x32, 0x44, 0x7d, 0x15, 0x57, 0x76 } };
static const GUID clsidMulInfo = 
{ 0x1d954cbc, 0x2724, 0x4f7e, { 0xb9, 0xc3, 0xdb, 0x12, 0xc, 0x45, 0x69, 0x1 } };
static const GUID clsidDivInfo = 
{ 0x1359456b, 0xbc27, 0x4516, { 0x84, 0xdd, 0x74, 0x9a, 0x71, 0x23, 0x86, 0x5f } };
// {68563E4E-D568-4ab5-BFD9-ECA0D14ECD1D}
static const GUID clsidSubInfoEx = 
{ 0x68563e4e, 0xd568, 0x4ab5, { 0xbf, 0xd9, 0xec, 0xa0, 0xd1, 0x4e, 0xcd, 0x1d } };




#define pszApplyLookupProgID	"ImageFilters.ApplyLookup"
#define pszSelectColorProgID	"ImageFilters.SelectColor"
#define pszAutoBCProgID	"ImageFilters.AutoBrightnessContrast"

#define pszCombineProgID	"ImageFilters.Combine"
#define pszAddProgID		"ImageFilters.Add"
#define pszSubProgID		"ImageFilters.Sub"
#define pszSubLightProgID	"ImageFilters.SubLight"
#define pszDiffProgID		"ImageFilters.Diff"
#define pszMulProgID		"ImageFilters.Mul"
#define pszDivProgID		"ImageFilters.Div"
#define pszSubExProgID		"ImageFilters.SubEx"



#endif //__imagefilters_h__