#pragma once

#ifdef _DEBUG
#import "\vt5\vt5\debug\comps\statdata.tlb" exclude ("IUnknown", "GUID", "_GUID", "tagPOINT") no_namespace raw_interfaces_only named_guids 
#else
#import "\vt5\vt5\release\comps\statdata.tlb" exclude ("IUnknown", "GUID", "_GUID", "tagPOINT") no_namespace raw_interfaces_only named_guids 
#endif
