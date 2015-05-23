#ifndef __argbase_h__
#define __argbase_h__

#include "nameconsts.h"
		
struct ArgumentInfo
{
	char	*pszArgName;
	char	*pszArgType;
	char	*pszDefValue;
	bool	bArg;
	bool	bDataObject;	//it is data object from NamedData
};


#define ACTION_UI( class_name, ui_name )									\
class __ActionUIInfo##class_name											\
{																			\
public:	__ActionUIInfo##class_name()										\
	{__ActionInfo##class_name##::s_pclassUI = RUNTIME_CLASS( ui_name );}	\
};																			\
__ActionUIInfo##class_name	__UIarg##class_name##Helper;


#define ACTION_TARGET( class_name, target_name )				\
class __ActionTargetInfo##class_name							\
{																\
public:	__ActionTargetInfo##class_name()						\
	{__ActionInfo##class_name##::s_strTarget = target_name;}	\
};																\
__ActionTargetInfo##class_name	__Targ##class_name##Helper;

#define ACTION_ARG_LIST( class_name )					\
class __ActionArgInfo##class_name						\
{	public:												\
	static ArgumentInfo s_pargs[];						\
	__ActionArgInfo##class_name()						\
	{__ActionInfo##class_name##::s_pargs = s_pargs;}	\
};														\
__ActionArgInfo##class_name	__Arg##class_name##Helper;	\
ArgumentInfo __ActionArgInfo##class_name::s_pargs[]={	

#define END_ACTION_ARG_LIST()	{0, 0, 0, false}};

#define ARG_( arg_name, arg_type, def_value, bData ) \
	{ arg_name, arg_type, def_value, true, bData },

#define ARG_BOOL( arg_name, def_value )		ARG_( arg_name, szArgumentTypeBool, #def_value, false )
#define ARG_INT( arg_name, def_value )		ARG_( arg_name, szArgumentTypeInt, #def_value, false )
#define ARG_DOUBLE( arg_name, def_value )		ARG_( arg_name, szArgumentTypeDouble, #def_value, false )
#define ARG_STRING( arg_name, def_value )		ARG_( arg_name, szArgumentTypeString, def_value, false )
#define ARG_IMAGE( arg_name )					ARG_( arg_name, szArgumentTypeImage, 0, true )
#define ARG_BINIMAGE( arg_name )				ARG_( arg_name, szArgumentTypeBinaryImage, 0, true )
#define ARG_OBJECT( arg_name )				ARG_( arg_name, szArgumentTypeObjectList, 0, true )

#define RES_( arg_name, arg_type, bData )	\
	{ arg_name, arg_type, 0, false, bData },

#define RES_BOOL( arg_name )		RES_( arg_name, szArgumentTypeBool, false  )
#define RES_INT( arg_name )			RES_( arg_name, szArgumentTypeInt, false )
#define RES_DOUBLE( arg_name )		RES_( arg_name, szArgumentTypeDouble, false )
#define RES_STRING(arg_name)		RES_( arg_name, szArgumentTypeString, false )
#define RES_IMAGE( arg_name )		RES_( arg_name, szArgumentTypeImage, true )
#define RES_BINIMAGE( arg_name )	RES_( arg_name, szArgumentTypeBinaryImage, true )
#define RES_OBJECT( arg_name )		RES_( arg_name, szArgumentTypeObjectList, true )



#endif //__argbase_h__