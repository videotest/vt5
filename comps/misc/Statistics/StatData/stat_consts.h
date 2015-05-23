#pragma once

#define STAT_TABLE_SECTION				"\\StatDataTable"
#define COMPRESS_IMAGE					"CompressImage"
#define ENABLE_UNDO						"EnableUndo"

#define szEventObjectOptionsChange		"ObjectOptionsChange"


//////////////////////////////////////////////////////////////////////
//StatTable Object
#define szStatTableObjectProgID		"StatData.StatTableObject"
// {3B02ABAD-54D6-422c-99F8-9A3D6177849D}
static const CLSID clsidStatTableObject =
{ 0x3b02abad, 0x54d6, 0x422c, { 0x99, 0xf8, 0x9a, 0x3d, 0x61, 0x77, 0x84, 0x9d } };

#define szStatTableObjectInfoProgID		"StatData.StatTableObjectInfo"
// {980FA4C2-7226-416d-AFA6-9AAD38D3569B}
static const CLSID clsidStatTableObjectInfo =
{ 0x980fa4c2, 0x7226, 0x416d, { 0xaf, 0xa6, 0x9a, 0xad, 0x38, 0xd3, 0x56, 0x9b } };

//////////////////////////////////////////////////////////////////////
//StatTable File Filter
#define szStatTableFileFilterProgID		"StatData.StatTableFileFilter"
// {C6701861-C99A-47f9-9668-6134A5AA30BE}
static const GUID clsidStatTableFileFilter = 
{ 0xc6701861, 0xc99a, 0x47f9, { 0x96, 0x68, 0x61, 0x34, 0xa5, 0xaa, 0x30, 0xbe } };

//////////////////////////////////////////////////////////////////////
//StatTable View
#define szStatTableViewProgID		"StatData.StatTableView"
// {A4B40D8D-7539-4726-A5D7-2922534B3431}
static const CLSID clsidStatTableView = 
{ 0xa4b40d8d, 0x7539, 0x4726, { 0xa5, 0xd7, 0x29, 0x22, 0x53, 0x4b, 0x34, 0x31 } };

//////////////////////////////////////////////////////////////////////
//StatTableDependence View
#define szStatTableDependenceViewProgID		"StatData.StatTableDependenceView"
// {EDDEA83C-C0C2-4f45-BE0A-659BC188D4CE}
static const CLSID clsidStatTableDependenceView = 
{ 0xeddea83c, 0xc0c2, 0x4f45, { 0xbe, 0xa, 0x65, 0x9b, 0xc1, 0x88, 0xd4, 0xce } };


//////////////////////////////////////////////////////////////////////
//Action CreateStatTable
#define szCreateStatTableProgID		"StatData.CreateStatTable"
// {D4D47AE7-4835-4e58-8E0D-B837D6088252}
static const CLSID clsidCreateStatTable = 
{ 0xd4d47ae7, 0x4835, 0x4e58, { 0x8e, 0xd, 0xb8, 0x37, 0xd6, 0x8, 0x82, 0x52 } };

//////////////////////////////////////////////////////////////////////
//Action ShowStatTable
#define szShowStatTableProgID		"StatData.ShowStatTable"
// {ED50C23B-F2C4-4c1e-9AF9-AB06668EFC53}
static const CLSID clsidShowStatTable = 
{ 0xed50c23b, 0xf2c4, 0x4c1e, { 0x9a, 0xf9, 0xab, 0x6, 0x66, 0x8e, 0xfc, 0x53 } };

//////////////////////////////////////////////////////////////////////
//Action show_stat_dependence
#define szShowStatTableDependenceProgID		"StatData.ShowStatTableDependence"
// {8C4F544B-A397-4126-8FF4-45CFA8DB2EC4}
static const CLSID clsidShowStatTableDependence = 
{ 0x8c4f544b, 0xa397, 0x4126, { 0x8f, 0xf4, 0x45, 0xcf, 0xa8, 0xdb, 0x2e, 0xc4 } };



//////////////////////////////////////////////////////////////////////
//Action ExportStatTable
#define szExportStatTableProgID		"StatData.ExportStatTable"
// {496487D6-9AF4-4077-A814-91FB00359768}
static const GUID clsidExportStatTable = 
{ 0x496487d6, 0x9af4, 0x4077, { 0xa8, 0x14, 0x91, 0xfb, 0x0, 0x35, 0x97, 0x68 } };
// {ED50C23B-F2C4-4c1e-9AF9-AB06668EFC53}

//////////////////////////////////////////////////////////////////////
//Action DeleteObjectFromStatTable
#define szDeleteObjectFromStatTableProgID		"StatData.DeleteObjectFromStatTable"
// {3FBC3D05-8FDE-4dd4-A471-2B44FABE8B9F}
static const GUID clsidDeleteObjectFromStatTable = 
{ 0x3fbc3d05, 0x8fde, 0x4dd4, { 0xa4, 0x71, 0x2b, 0x44, 0xfa, 0xbe, 0x8b, 0x9f } };

//////////////////////////////////////////////////////////////////////
//Action FilterStatTable
#define szFilterStatTableProgID		"StatData.FilterStatTable"
// {BD982564-C128-4f97-8A55-17492301C76E}
static const GUID clsidFilterStatTable = 
{ 0xbd982564, 0xc128, 0x4f97, { 0x8a, 0x55, 0x17, 0x49, 0x23, 0x1, 0xc7, 0x6e } };

//////////////////////////////////////////////////////////////////////
//Action AddStatTable
#define szSewStatTableProgID		"StatData.SewStatTable"
// {670BA42E-3784-4ad1-ADE7-D22F9FA9BCE9}
static const GUID clsidSewStatTable = 
{ 0x670ba42e, 0x3784, 0x4ad1, { 0xad, 0xe7, 0xd2, 0x2f, 0x9f, 0xa9, 0xbc, 0xe9 } };


//////////////////////////////////////////////////////////////////////
//Action HideStatTableColumn
#define szHideStatTableColumnProgID		"StatData.HideStatTableColumn"
// {28EC1B7D-218C-4b5a-B6E2-3B7647669D9F}
static const GUID clsidHideStatTableColumn = 
{ 0x28ec1b7d, 0x218c, 0x4b5a, { 0xb6, 0xe2, 0x3b, 0x76, 0x47, 0x66, 0x9d, 0x9f } };

//////////////////////////////////////////////////////////////////////
//Action ShowAllStatTableColumns
#define szShowAllStatTableColumnsProgID		"StatData.ShowAllStatTableColumns"
// {2DC10B2A-6636-4f1b-97B1-0263FB3FB79B}
static const GUID clsidShowAllStatTableColumns = 
{ 0x2dc10b2a, 0x6636, 0x4f1b, { 0x97, 0xb1, 0x2, 0x63, 0xfb, 0x3f, 0xb7, 0x9b } };

//////////////////////////////////////////////////////////////////////
//Action 
#define szCustomizeStatTableColumnsProgID		"StatData.CustomizeStatTableColumns"
// {F4113A95-E5A9-4939-BECC-5BE3C0B63443}
static const GUID clsidCustomizeStatTableColumns = 
{ 0xf4113a95, 0xe5a9, 0x4939, { 0xbe, 0xcc, 0x5b, 0xe3, 0xc0, 0xb6, 0x34, 0x43 } };
