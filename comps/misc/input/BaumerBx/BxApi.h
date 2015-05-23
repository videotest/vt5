#ifndef __BXCAMLIB_H__
#define __BXCAMLIB_H__

// required utils

// global definitions and functions
#include "BxLibApi.h"

// classes provided by Lib

// libraries required for linking
#if defined(_DEBUG) || DBG
#pragma comment(lib,"BxLibD.lib")
#else
#pragma comment(lib,"BxLib.lib")
#endif

#endif  // __BXCAMLIB_H__