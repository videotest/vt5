#ifndef __CONTROL_DEF_H__
#define __CONTROL_DEF_H__

#ifdef _CONTROL_LIB_
#define	control_export_class class __declspec(dllexport)
#define	control_export_struct struct __declspec(dllexport)
#else
#define	control_export_class class __declspec(dllimport)
#define	control_export_struct struct __declspec(dllimport)
#endif //_CONTROL_LIB_


#endif __CONTROL_DEF_H__
