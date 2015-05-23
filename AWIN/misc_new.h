#pragma once
///////////////////////////////////////////////////////////////////////////////
// This file is part of AWIN class library
// Copyright (c) 2001-2002 Andy Yamov
//
// Permission to use, copy, modify, distribute, and sell this software and
// its documentation for any purpose is hereby granted without fee
// 
// THE SOFTWARE IS PROVIDED_T("AS-IS") AND WITHOUT WARRANTY OF ANY KIND,
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//

#ifndef __misc_new_h__
#define __misc_new_h__

#ifdef _DEBUG

#pragma warning(disable:4005)
#pragma warning(disable:4786)

#include "memory"
#include "crtdbg.h"


#define new			::new(_NORMAL_BLOCK, __FILE__, __LINE__)

#else

#define _CrtDumpMemoryLeaks()

#endif _DEBUG

#endif //__misc_new_h__