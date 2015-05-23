#pragma once

struct	VarCodes
{
	VARENUM	vt;
	char	*szUserName;
};

#define TYPE_MULTILINESTRING		"MultiLineString"
#define	TYPE_UNKNOWN				"Unknown"
#define METHOD_TEXT			"Text"
#define METHOD_BINARY		"Binary"
#define NAMED_DATA_SIGN		"NamedData file. Version 1.0a"
#define CONTEXT_SIGN		"__Context__Begin__"
#define CONTEXT_END_SIGN	"__Context__End__"

static VarCodes g_VarCodes[] = 
{
	{VT_NULL, "Empty"},
	{VT_I4, "Long"},
	{VT_UNKNOWN, TYPE_UNKNOWN},
	{VT_DISPATCH, TYPE_UNKNOWN},
	{VT_BSTR, "String"},
	{VT_BSTR, TYPE_MULTILINESTRING},
	{VT_R8, "Double"},
	{VT_BITS, "Pointer"}
};

//convert VARENUM values to user-friendly strings
static inline CString VarEnumToString( VARIANT *pvar )
{
	if( !pvar )
		return g_VarCodes[0].szUserName;

	if( pvar->vt == VT_BSTR )
	{
		CString	str( pvar->bstrVal );
		if( str.Find( "\n" ) != -1 )
			return TYPE_MULTILINESTRING;
	}

	for( int i = 0; i < sizeof(g_VarCodes)/sizeof(g_VarCodes[0]); i++ )
		if( g_VarCodes[i].vt == pvar->vt )
			return g_VarCodes[i].szUserName;
	CString	s;
	s.Format( "%d", pvar->vt );

	return s;
}

//convert string value to the VARENUM constant
static inline VARENUM	StringToVarEnum( const char *szVarType )
{
	for( int i = 0; i < sizeof(g_VarCodes)/sizeof(g_VarCodes[0]); i++ )
		if( !strcmp(g_VarCodes[i].szUserName, szVarType ) )
			return g_VarCodes[i].vt;

	VARENUM	vt = VT_EMPTY;
	if( ::sscanf( szVarType, "%d", &vt ) == 1 )
		return vt;

	return VT_EMPTY;
}
