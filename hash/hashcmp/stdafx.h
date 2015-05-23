#pragma once

#include <tchar.h>
#include <string>
#include <iostream>
#include <direct.h>
#include <stdio.h>
#include <windows.h>
#include <fstream>
#include <map>
using namespace std;
typedef basic_string<TCHAR> tstring;
typedef basic_ifstream<TCHAR, char_traits<TCHAR> > tifstream;
typedef basic_ostream<TCHAR, char_traits<TCHAR> > tostream;
typedef basic_ofstream<TCHAR, char_traits<TCHAR> > tofstream;
//using tstring::size_type;
