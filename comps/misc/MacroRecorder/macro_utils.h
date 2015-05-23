#pragma once
#include "\vt5\awin\profiler.h"

inline DWORD __GetTickCount( )
{
    return (DWORD)(__cputick__() / __cpuspeed__());    
}