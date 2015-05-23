#ifndef __sewLI_h__
#define __sewLI_h__

extern const char* szSewLISectName;

//Sew L(arge)I(mage) Base
#define pszSewLIBaseProgID		"sewLI.SewLIBase"
// {B634E8E8-15A7-45b0-8BAC-332AE9DC6FE1}
static const GUID clsidSewLIBase =
{ 0xb634e8e8, 0x15a7, 0x45b0, { 0x8b, 0xac, 0x33, 0x2a, 0xe9, 0xdc, 0x6f, 0xe1 } };

//Property Page sewLI
#define pszSewLIPageProgID		"sewLI.SewLIPage"
// {86A01734-30DD-4030-BEFA-6D75E0FDBA99}
static const GUID clsidSewLIPage = 
{ 0x86a01734, 0x30dd, 0x4030, { 0xbe, 0xfa, 0x6d, 0x75, 0xe0, 0xfd, 0xba, 0x99 } };

//General sewLI
#define pszSewLIProgID			"sewLI.SewLI"
// {C2323401-52B4-4b93-97A3-2FDD206BD86E}
static const GUID clsidSewLI = 
{ 0xc2323401, 0x52b4, 0x4b93, { 0x97, 0xa3, 0x2f, 0xdd, 0x20, 0x6b, 0xd8, 0x6e } };


#define pszSewLIBaseInfoProgID	"sewLI.SewLIBaseInfo"
// {B474CFB3-7566-4ab7-B0E0-861F23E2A362}
static const GUID clsidSewLIBaseInfo = 
{ 0xb474cfb3, 0x7566, 0x4ab7, { 0xb0, 0xe0, 0x86, 0x1f, 0x23, 0xe2, 0xa3, 0x62 } };

#define pszSewLIObjectActionProgID "sewLI.SewLIObjectAction"
// {C1F01165-441A-4ff1-A9E5-FCD3A2E7AB72}
static const GUID clsidSewLIObjectAction = 
{ 0xc1f01165, 0x441a, 0x4ff1, { 0xa9, 0xe5, 0xfc, 0xd3, 0xa2, 0xe7, 0xab, 0x72 } };

#define pszSewLIObjectActionInfoProgID "sewLI.SewLIObjectActionInfo"
// {8730557C-3268-4d82-AAC5-1745CCFD3424}
static const GUID clsidSewLIObjectActionInfo = 
{ 0x8730557c, 0x3268, 0x4d82, { 0xaa, 0xc5, 0x17, 0x45, 0xcc, 0xfd, 0x34, 0x24 } };

#define pszSewLIViewProgID "sewLI.SewLIView"
// {A77567A8-BE06-4c2a-964D-1B3F6A532694}
static const GUID clsidSewLIView = 
{ 0xa77567a8, 0xbe06, 0x4c2a, { 0x96, 0x4d, 0x1b, 0x3f, 0x6a, 0x53, 0x26, 0x94 } };

#define pszSewImportActionProgID "sewLI.SewImport"
// {FD16E448-61B7-4b2e-8E80-959E013E746C}
static const GUID clsidSewImportAction = 
{ 0xfd16e448, 0x61b7, 0x4b2e, { 0x8e, 0x80, 0x95, 0x9e, 0x1, 0x3e, 0x74, 0x6c } };

#define pszSewExportActionProgID "sewLI.SewExport"
// {F8678D77-434C-4b2e-8FE2-9CA415D7AE76}
static const GUID clsidSewExportAction  = 
{ 0xf8678d77, 0x434c, 0x4b2e, { 0x8f, 0xe2, 0x9c, 0xa4, 0x15, 0xd7, 0xae, 0x76 } };

#define pszSewMoveActionProgID "sewLI.SewMoveFragment"
// {257386C3-A818-43db-886B-36D307CBA8CC}
static const GUID clsidSewMove = 
{ 0x257386c3, 0xa818, 0x43db, { 0x88, 0x6b, 0x36, 0xd3, 0x7, 0xcb, 0xa8, 0xcc } };


#define pszSewDeleteActionProgID "sewLI.SewDeleteFragment"
// {7FD7E742-FA69-42a5-A463-1D65BB23C87F}
static const GUID clsidSewDelete = 
{ 0x7fd7e742, 0xfa69, 0x42a5, { 0xa4, 0x63, 0x1d, 0x65, 0xbb, 0x23, 0xc8, 0x7f } };

#define pszSewShowSewLIProgID "sewLI.ShowSewLI"
// {495DE732-6064-43e3-8638-36AFAB12DDA1}
static const GUID clsidShowSewLI = 
{ 0x495de732, 0x6064, 0x43e3, { 0x86, 0x38, 0x36, 0xaf, 0xab, 0x12, 0xdd, 0xa1 } };

#define pszSewLIRenderProgID "sewLI.SewLIRender"
// {C72B67A4-619B-4818-A6CE-92721D2931F4}
static const GUID clsidSewLIRender = 
{ 0xc72b67a4, 0x619b, 0x4818, { 0xa6, 0xce, 0x92, 0x72, 0x1d, 0x29, 0x31, 0xf4 } };

#define pszSewLISewSaveImage "sewLI.SewSaveImage"
// {34CDB4B8-F23F-4b75-AA09-A32D3BED49EF}
static const GUID clsidSewSaveImage = 
{ 0x34cdb4b8, 0xf23f, 0x4b75, { 0xaa, 0x9, 0xa3, 0x2d, 0x3b, 0xed, 0x49, 0xef } };


const int g_nFragmentImages = 5;

#endif //__sewLI_h__