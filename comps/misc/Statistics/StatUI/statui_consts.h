#pragma once

//////////////////////////////////////////////////////////////////////
//Stat Object
#define szStatObjectProgID		"StatUI.StatObject"
// {4535B1DF-2FB1-4940-96EB-CB0AEC2D15C6}
static const CLSID clsidStatObject =
{ 0x4535b1df, 0x2fb1, 0x4940, { 0x96, 0xeb, 0xcb, 0xa, 0xec, 0x2d, 0x15, 0xc6 } };

#define szStatObjectInfoProgID		"StatUI.StatObjectInfo"
// {9DAA063B-4CAE-4c55-B98E-FADDB81E439D}
static const CLSID clsidStatObjectInfo =
{ 0x9daa063b, 0x4cae, 0x4c55, { 0xb9, 0x8e, 0xfa, 0xdd, 0xb8, 0x1e, 0x43, 0x9d } };

#define szCmpStatObjectProgID		"StatUI.CmpStatObject"
// {14a78fd4-23a0-4c3e-ac00-8573b57309a1}
static const CLSID clsidCmpStatObject = 
{ 0x14a78fd4, 0x23a0, 0x4c3e, { 0xac, 0x0, 0x85, 0x73, 0xb5, 0x73, 0x9, 0xa1 } };

#define szCmpStatObjectInfoProgID		"StatUI.CmpStatObjectInfo"
// {3210BEAB-C28A-45b9-BDD3-2D6B08BCE7DA}
static const CLSID clsidCmpStatObjectInfo =
{ 0x3210beab, 0xc28a, 0x45b9, { 0xbd, 0xd3, 0x2d, 0x6b, 0x8, 0xbc, 0xe7, 0xda } };

#define szStatObjectFileFilterProgID	"StatUI.StatObjectFileFilter"
// {255C5551-4770-4558-AD72-EE02218BE3F3}
static const GUID clsidStatObjectFileFilter = 
{ 0x255c5551, 0x4770, 0x4558, { 0xad, 0x72, 0xee, 0x2, 0x21, 0x8b, 0xe3, 0xf3 } };


//////////////////////////////////////////////////////////////////////
// StatObject View
#define szStatObjectViewProgID		"StatUI.StatObjectView"
// {84771DB9-9377-4246-B5F9-878617869B56}
static const CLSID clsidStatObjectView = 
{ 0x84771db9, 0x9377, 0x4246, { 0xb5, 0xf9, 0x87, 0x86, 0x17, 0x86, 0x9b, 0x56 } };

// CmpStatObject View
#define szCmpStatObjectViewProgID		"StatUI.CmpStatObjectView"
// {315B119F-B608-4eae-B92D-F973C155F5D4}
static const CLSID clsidCmpStatObjectView = 
{ 0x315b119f, 0xb608, 0x4eae, { 0xb9, 0x2d, 0xf9, 0x73, 0xc1, 0x55, 0xf5, 0xd4 } };

//////////////////////////////////////////////////////////////////////
//Action ShowStatObjectView
#define szShowStatObjectProgID		"StatUI.ShowStatView"
// {ECD12B20-0211-4ff0-B45A-06AF139D36D5}
static const CLSID clsidShowStatObject = 
{ 0xecd12b20, 0x211, 0x4ff0, { 0xb4, 0x5a, 0x6, 0xaf, 0x13, 0x9d, 0x36, 0xd5 } };

//////////////////////////////////////////////////////////////////////
//Action ActionChartViewOnOff
#define szActionChartViewOnOffProgID		"StatUI.ShowStatChartView"
// {E567749C-2282-4026-B9FD-FB28FC63609D}
static const CLSID clsidChartViewOnOff = 
{ 0xe567749c, 0x2282, 0x4026, { 0xb9, 0xfd, 0xfb, 0x28, 0xfc, 0x63, 0x60, 0x9d } };


//////////////////////////////////////////////////////////////////////
//Action ActionLegendViewOnOff
#define szActionLegendViewOnOffProgID		"StatUI.ShowStatLegendView"
// {0BD85656-2128-4613-A9E4-1E93234FDEAC}
static const CLSID clsidLegendViewOnOff = 
{ 0xbd85656, 0x2128, 0x4613, { 0xa9, 0xe4, 0x1e, 0x93, 0x23, 0x4f, 0xde, 0xac } };


//////////////////////////////////////////////////////////////////////
//Action ActionTableViewOnOff
#define szActionTableViewOnOffProgID		"StatUI.ShowStatTableView"
// {355DB389-AB2A-4d22-BDB7-2B348E848120}
static const CLSID clsidTableViewOnOff = 
{ 0x355db389, 0xab2a, 0x4d22, { 0xbd, 0xb7, 0x2b, 0x34, 0x8e, 0x84, 0x81, 0x20 } };


//////////////////////////////////////////////////////////////////////
//Action ActionCreateStatObject
#define szActionCreateStatObjectProgID		"StatUI.CreateStatObject"
// {D35F4F6F-C974-4da3-A6E4-8817084A095C}
static const CLSID clsidActionCreateStatObject = 
{ 0xd35f4f6f, 0xc974, 0x4da3, { 0xa6, 0xe4, 0x88, 0x17, 0x8, 0x4a, 0x9, 0x5c } };

//////////////////////////////////////////////////////////////////////
//Action ActionAssignVirtualClasses
#define szActionAssignVirtualClassesProgID		"StatUI.AssignVirtualClasses"
// {42C3933F-97F8-46fd-8979-1AE5A11E9946}
static const CLSID clsidActionAssignVirtualClasses = 
{ 0x42c3933f, 0x97f8, 0x46fd, { 0x89, 0x79, 0x1a, 0xe5, 0xa1, 0x1e, 0x99, 0x46 } };

//////////////////////////////////////////////////////////////////////
//Action ActionShowVirtualClasses
#define szActionShowVirtualClassesProgID		"StatUI.ShowVirtualClasses"
// {E2D177E5-4DCA-4505-ACF2-7CB15AA0AA5F}
static const CLSID clsidActionShowVirtualClasses = 
{ 0xe2d177e5, 0x4dca, 0x4505, { 0xac, 0xf2, 0x7c, 0xb1, 0x5a, 0xa0, 0xaa, 0x5f } };


#define szShowStatCmpProgID		"StatUI.ShowStatCmp"
// {BA6C5869-B2BE-48a5-B770-026988926C35}
static const CLSID clsidShowStatCmp = 
{ 0xba6c5869, 0xb2be, 0x48a5, { 0xb7, 0x70, 0x2, 0x69, 0x88, 0x92, 0x6c, 0x35 } };

#define szActionChartCmpOnOffProgID		"StatUI.ShowStatChartCmp"
// {EB9EA9AE-1C4A-407a-A3C4-82D9C02403F8}
static const CLSID clsidChartCmpOnOff = 
{ 0xeb9ea9ae, 0x1c4a, 0x407a, { 0xa3, 0xc4, 0x82, 0xd9, 0xc0, 0x24, 0x3, 0xf8 } };

#define szActionLegendCmpOnOffProgID		"StatUI.ShowStatLegendCmp"
// {15497118-D2FB-42d4-BC29-A51B867AAB1D}
static const CLSID clsidLegendCmpOnOff = 
{ 0x15497118, 0xd2fb, 0x42d4, { 0xbc, 0x29, 0xa5, 0x1b, 0x86, 0x7a, 0xab, 0x1d } };

#define szActionTableCmpOnOffProgID		"StatUI.ShowStatTableCmp"
// {5ED98AD5-7B08-4b66-BB00-835EA6E2CD07}
static const CLSID clsidTableCmpOnOff = 
{ 0x5ed98ad5, 0x7b08, 0x4b66, { 0xbb, 0x0, 0x83, 0x5e, 0xa6, 0xe2, 0xcd, 0x7 } };
