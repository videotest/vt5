#include "StdAfx.h"
#include "gdiplusmanager.h"

CGDIPlusManager::CGDIPlusManager(void)
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

CGDIPlusManager::~CGDIPlusManager(void)
{
	GdiplusShutdown(m_gdiplusToken);
}
