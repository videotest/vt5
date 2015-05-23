#ifndef __STRINGEX_H
#define __STRINGEX_H

#include <String.h>
#include "misc_new.h"

inline char * StrDupNew(char const * src)
{	return src ? strcpy(new char[strlen(src) + 1], src) : 0;	}

inline void StrReplace(char * & dst, char const * src)
{	if (dst)  delete dst;	dst = src ? StrDupNew(src) : 0;	}

inline void StrNCpyEx(char * dst, char const * src, unsigned max_str_len)
{	strncpy(dst, src, max_str_len)[max_str_len] = 0;	}

#endif