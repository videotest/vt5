#ifndef __CONTROL_DEF_H__
#define __CONTROL_DEF_H__

#ifdef _CONTROL_LIB_
#define	control_export_class class _declspec(dllexport)
#define	control_export_data _declspec(dllexport)
#define	control_export_struct struct _declspec(dllexport)
#else
#define	control_export_class class _declspec(dllimport)
#define	control_export_data _declspec(dllimport)
#define	control_export_struct struct _declspec(dllimport)
#endif //_COMMON_DLL


#endif __CONTROL_DEF_H__
