#ifndef __class_utils_h__
#define __class_utils_h__

#ifdef COMMON1
#include "utils.h"
#else
#include "misc_utils.h"
#endif //_COMMON_LIB

#include "params.h"
#include "measure5.h"
#include <stdio.h>
#include "PropBag.h"

inline const char *def_filename( char *sz_path, const char *sz_section, const char *sz_file, int cb )
{
	GetModuleFileName( 0, sz_path, cb );
	strcpy( strrchr( sz_path, '\\' )+1, sz_section );
	strcat( sz_path, "\\" );
	strcpy( sz_path, GetValueString( GetAppUnknown(), "\\Paths", sz_section, sz_path ) );
	strcat( sz_path, sz_file );

	return sz_path;
}

inline const char *short_filename( const char *sz_path )
{
	const char *sz1 = strrchr( sz_path, '\\' );
	return 0==sz1 ? sz_path : sz1+1;
}

inline const char *short_classifiername(const char *szClassifierName)
{
	if(szClassifierName && *szClassifierName) return short_filename(szClassifierName);
	// (если имя не пустое - не 0 и не "")

	static char sz_ini[MAX_PATH];
	def_filename( sz_ini, "Classes", "classes.ini", sizeof(sz_ini) );
	strcpy( sz_ini, (const char*)::GetValueString( GetAppUnknown(), "\\Classes", "ClassFile", sz_ini ) );
	return short_filename(sz_ini);
}

inline const char *full_classifiername(const char *szClassifierName)
{
	static char sz_ini[MAX_PATH];
	if(szClassifierName && *szClassifierName)
	{ // (если имя не пустое - не 0 и не "")
		def_filename( sz_ini, "Classes", short_filename(szClassifierName), sizeof(sz_ini) );
	}
	else
	{
		def_filename( sz_ini, "Classes", "classes.ini", sizeof(sz_ini) );
		strcpy( sz_ini, (const char*)::GetValueString( GetAppUnknown(), "\\Classes", "ClassFile", sz_ini ) );
	}
	return sz_ini;
}


inline long get_object_class( ICalcObject *pcalc, const char *szClassifierName=0 )
{
	if( !pcalc )return -1;
	INamedPropBagPtr bag(pcalc);
	if(bag!=0)
	{
		_variant_t	var;
		bag->GetProperty(_bstr_t(short_classifiername(szClassifierName)), &var);
		if(var.vt == VT_I4) return var.lVal;
	}

	// если не вернули значение - используем старую процедуру
	double	dblClass;
	if( pcalc->GetValue( KEY_CLASS, &dblClass ) != S_OK )return -1;
	return (long)dblClass;
}


inline bool is_object_class_manual( ICalcObject *pcalc, char *szClassifierName=0 )
{
	if( !pcalc )return false;
	INamedPropBagPtr bag(pcalc);
	if(bag!=0)
	{
		_bstr_t bstrClassMF("ManualFlag_");
		bstrClassMF += _bstr_t(short_classifiername(szClassifierName));
		_variant_t	var;
		bag->GetProperty(bstrClassMF, &var);
		if(var.vt == VT_I4) return var.lVal!=0;
	}
	return false;
}


inline void set_object_class( ICalcObject *pcalc, long lclass, char *szClassifierName=0 )
{
	if( !pcalc )return;

	if(szClassifierName==0 || *szClassifierName==0)
	{ // если пишем в активный классификатор - на всякий случай сдублируем значение в параметр - для старых скриптов
		pcalc->SetValue( KEY_CLASS, (double)lclass );
	}

	INamedPropBagPtr bag(pcalc);
	if(bag!=0)
	{
		_variant_t	var;
		bag->SetProperty(_bstr_t(short_classifiername(szClassifierName)), _variant_t(lclass));
	}
}

inline void set_object_class_manual( ICalcObject *pcalc, bool bmanual, char *szClassifierName=0 )
{
	if( !pcalc )return;

	INamedPropBagPtr bag(pcalc);
	if(bag!=0)
	{
		_bstr_t bstrClassMF("ManualFlag_");
		bstrClassMF += _bstr_t(short_classifiername(szClassifierName));
		_variant_t	var(bmanual?1L:0L);
		bag->SetProperty(bstrClassMF, var);
	}
}

inline long	class_count( const char *szClassifierName=0 )
{
	long lret = ::GetPrivateProfileInt( "Classes", "ClassCount", 0, full_classifiername(szClassifierName) );
	return lret;
}

inline char *_class_buffer()
{	
	static char sz[100];
	return sz;
}

inline const char *get_class_name( LPOS lpos, const char *szClassifierName=0 )
{
	//if( lpos == -1 )
	//{
	//	strcpy( _class_buffer(), (const char*)GetValueString( GetAppUnknown(), "\\Classes", "UnkClassName", "Unknown" ) );
	//}
	//else
	{
		char	sz[20];
		sprintf( sz, "%d", lpos );

		char	sz_text[100];

		::GetPrivateProfileString( "Classes", sz, "", sz_text, sizeof( sz_text ), full_classifiername(szClassifierName) );
		long	r = 255, g = 0, b = 0;
		strcpy( _class_buffer(), "?" );
		if( sz_text[0] == 0 && lpos == -1 )
		{
			strcpy( _class_buffer(), (const char*)GetValueString( GetAppUnknown(), "\\Classes", "UnkClassName", "Unknown" ) );
		}
		else
		{
			sscanf( sz_text, "(%d,%d,%d),%[^;]s", &r, &g, &b, _class_buffer() );
		}
	}

	return _class_buffer();
}

inline COLORREF get_class_color( LPOS lpos, const char *szClassifierName=0 )
{
	//if( lpos == -1 )
	//{
	//	return GetValueColor( GetAppUnknown(), "\\Classes", "UnkClassColor", RGB( 255, 255, 0 ) );;
	//}
	//else
	{
		char	sz[20];
		sprintf( sz, "%d", lpos );

		char	sz_text[100];

		::GetPrivateProfileString( "Classes", sz, "", sz_text, sizeof( sz_text ), full_classifiername(szClassifierName) );
		long	r = 255, g = 0, b = 0;
		if( sz_text[0] == 0 && lpos == -1 )
		{
			return GetValueColor( GetAppUnknown(), "\\Classes", "UnkClassColor", RGB( 255, 255, 0 ) );;
		}
		::sscanf( sz_text, "(%d,%d,%d)", &r, &g, &b );
		return RGB( r, g, b );
	}
}

inline COLORREF get_smooth_class_color( LPOS lpos, long nClassCount, char *szClassifierName=0 )
{
	int nClasses=class_count(szClassifierName);
	if(nClassCount==0) nClassCount=nClasses;
	if(nClassCount<nClasses) nClassCount=nClasses;

	double fClass = 0;
	int nClass0=-1, nClass1=-1;

	if(lpos!=-1)
	{
		fClass = double(lpos)/__max(nClassCount-1,1)*(nClasses-1);
		nClass0 = int(fClass);
		nClass1 = __min(nClass0+1,nClasses-1);
		fClass -= nClass0;
	}

	COLORREF rgb0 = get_class_color(nClass0, szClassifierName);
	COLORREF rgb1 = get_class_color(nClass1, szClassifierName);

	int r = int( GetRValue(rgb0)*(1-fClass) + GetRValue(rgb1)*fClass + 0.5);
	int g = int( GetGValue(rgb0)*(1-fClass) + GetGValue(rgb1)*fClass + 0.5 );
	int b = int( GetBValue(rgb0)*(1-fClass) + GetBValue(rgb1)*fClass + 0.5 );

	return RGB( r, g, b );
}

#endif //__class_utils_h__