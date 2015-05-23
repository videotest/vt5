#ifndef __apportionment_h__
#define __apportionment_h__


extern const char* szApportionmentSectName;


// {EB73C14F-FF15-4b34-9484-DA7C050CD3DB}
static const GUID clsidAView = 
{ 0xeb73c14f, 0xff15, 0x4b34, { 0x94, 0x84, 0xda, 0x7c, 0x5, 0xc, 0xd3, 0xdb } };
#define szAViewProgID		"Apportionment.AView"

// {9605B3C4-FA4F-4a51-B48B-142AD69F2E6F}
static const GUID clsidActionShowApportionment = 
{ 0x9605b3c4, 0xfa4f, 0x4a51, { 0xb4, 0x8b, 0x14, 0x2a, 0xd6, 0x9f, 0x2e, 0x6f } };
#define szActionShowApportionmentProgID "Apportionment.ActionShowApportionment"

// {02B7DE38-A84A-45f5-A440-A3219DFCFF46}
static const GUID clsidActionClassesArrange = 
{ 0x2b7de38, 0xa84a, 0x45f5, { 0xa4, 0x40, 0xa3, 0x21, 0x9d, 0xfc, 0xff, 0x46 } };
#define szActionClassesArrangeProgID "Apportionment.ActionClassesArrange"



#endif //__apportionment_h__