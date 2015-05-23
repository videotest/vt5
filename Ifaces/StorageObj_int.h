#pragma once

//ImageStream
#define szTypeStorageObject "Storage Object"
#define szStorageObjectProgID "Storage.StorageObject"

//ImageStreamInfo
#define pszStorageObjectInfoProgID	"Storage.StorageObjectInfo"
// {466EF50F-7F53-4cfb-8C17-0AC778465482}
static const GUID clsidStorageObjectInfo = 
{ 0x466ef50f, 0x7f53, 0x4cfb, { 0x8c, 0x17, 0xa, 0xc7, 0x78, 0x46, 0x54, 0x82 } };

#import <StorageObj.tlb> exclude ("IUnknown", "GUID", "_GUID", "tagPOINT") no_namespace raw_interfaces_only named_guids 

