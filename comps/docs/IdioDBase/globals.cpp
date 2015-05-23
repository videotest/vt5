#include "StdAfx.h"
#include "globals.h"

LPSTREAM AFXAPI _AfxCreateMemoryStream()
{
	LPSTREAM lpStream = NULL;

	// Create a stream object on a memory block.
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, 0);
	if (hGlobal != NULL)
	{
		if (FAILED(CreateStreamOnHGlobal(hGlobal, TRUE, &lpStream)))
		{
			TRACE0("CreateStreamOnHGlobal failed.\n");
			GlobalFree(hGlobal);
			return NULL;
		}

		ASSERT_POINTER(lpStream, IStream);
	}
	else
	{
		TRACE0("Failed to allocate memory for stream.\n");
		return NULL;
	}

	return lpStream;
}
