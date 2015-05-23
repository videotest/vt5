#ifndef __chromo_utils_h__
#define __chromo_utils_h__

bool DrawMirror( BITMAPINFOHEADER* pbi, byte* pdib, CRect rcFill, COLORREF clrMirror );
bool DrawAlphaBlend( BITMAPINFOHEADER* pbi, byte* pdib, CRect rcFill, COLORREF clrAlphaBlend );

//classes
CString	GetClassFileNameFromObjectList( IUnknown* punk_object_list );
bool	SetClassFileNameToObjectList( IUnknown* punk_object_list, const char* psz_class_file_name );

CString	GetCurrentClassFileName();


#endif//__chromo_utils_h__