#include "stdafx.h"
#include <conio.h>
//#include "appdefs.h"
#include "LowLevel.h"
//#include "fgdrv.h"

int CS = 0;
int Config = 4;
int param[8];
int oldMD=0, oldMIN=0;
int MinBr, MedBr;
int DCConst;

extern int iExpoTime, iFormatN;
//extern FGFormat *pFormats;

DefJob pDefJob[] = {
	{"Bkg Frame Subtraction",	0x0040}, 
	{"Get dark frame",			0x0080},
	{"Flat field correction",	0x0100},
	{"Defect correction",		0x0200},
	{"Smear correction",		0x0800},
	{"Val Discrimin mode",		0x1000},
	{"Autoranging enable",		0x2000},
	{"L_Conv 2/3",				0x4000},
	{"Data Fast Transfer En",	0x8000},
	{NULL,						0x0000},
};

int JOB0 = 0;

