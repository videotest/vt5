#ifndef __unicode_h__
#define __unicode_h__

//UNICODE support
#include <tchar.h>
#include <stdio.h>

#ifdef _UNICODE


#else//_UNICODE

#ifndef _char
#define _char	char
#endif//_char
#define ___T(x)	x		

#define ___strcpy	strcpy
#define ___strncpy	strncpy
#define ___strcat	strcat
#define ___strcmp	strcmp
#define ___stricmp	stricmp
#define ___strncmp	strncmp
#define ___strlen	strlen
#define ___strlwr	strlwr
#define ___sprintf	sprintf
#define ___sscanf	sscanf
#define ___strtok	strtok
#define ___strrchr	strrchr
#define ___strstr	strstr
#define ___strchr	strchr
#define ___strdup	strdup
#define ___strtod	strtod
#endif//_UNICODE

#define	SIZEOF_CHARS(sz_buf)	(sizeof(sz_buf)/sizeof(_char))

#endif//__unicode_h__

//atoi??
//strrchr ??
