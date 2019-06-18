#pragma once

#ifdef _DEBUG
	#import "\vt5\ifaces\debug\charts.tlb" exclude ("IUnknown", "GUID", "_GUID", "tagPOINT", "tagRECT" ) no_namespace raw_interfaces_only named_guids 
#else
	#import "\vt5\ifaces\release\charts.tlb" exclude ("IUnknown", "GUID", "_GUID", "tagPOINT", "tagRECT" ) no_namespace raw_interfaces_only named_guids 
#endif

