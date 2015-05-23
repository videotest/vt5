#include "stdafx.h"
#include "twcore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CImageMemXfer::CImageMemXfer(int nMemFlags, int nMemSize, void *pMem)
{
	m_Data.Compression   = TWON_DONTCARE16;
	m_Data.BytesPerRow   = TWON_DONTCARE32;
	m_Data.Columns       = TWON_DONTCARE32;
	m_Data.Rows          = TWON_DONTCARE32;
	m_Data.XOffset       = TWON_DONTCARE32;
	m_Data.YOffset       = TWON_DONTCARE32;
	m_Data.BytesWritten  = TWON_DONTCARE32;
	m_Data.Memory.Flags  = nMemFlags;
	m_Data.Memory.Length = nMemSize;
	m_Data.Memory.TheMem = pMem;
}
