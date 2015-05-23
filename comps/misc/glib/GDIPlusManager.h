#pragma once
#include <unknwn.h>
#include <gdiplus.h>

using namespace Gdiplus;

namespace glib_utils
{
	class CGDIPlusManager
	{
		ULONG_PTR m_gdiplusToken;
	public:
		CGDIPlusManager(void);
		virtual ~CGDIPlusManager(void);
	};
}
