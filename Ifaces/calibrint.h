#ifndef __calibrint_h__
#define __calibrint_h__

#ifdef _DEBUG
#import <debug\measure.tlb> exclude ("IUnknown", "GUID", "_GUID", "tagPOINT") no_namespace raw_interfaces_only named_guids 
#else
#import <release\measure.tlb> exclude ("IUnknown", "GUID", "_GUID", "tagPOINT") no_namespace raw_interfaces_only named_guids 
#endif

#endif //__calibrint_h__