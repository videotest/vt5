#include "stdafx.h"
#include "wmf.h"


/////////////////////////////////////////////////////////////////////////////
CWmf::CWmf( BYTE* pData, UINT uiSize )
{
	m_pData = pData;
	m_uiSize = uiSize;
}

/////////////////////////////////////////////////////////////////////////////
HENHMETAFILE CWmf::CreateMetaFile( int& nWidth, int& nHeight )
{
	nWidth = nHeight = 0;
	
	if( !m_pData )
		return NULL;

	if( m_uiSize <= 0 )
		return NULL;

	DWORD   		dwIsAldus = 0;
	
  	DWORD    		dwSize = 0;
	DWORD			seekpos = 0;

	

	METAHEADER		mfHeader;
	::ZeroMemory( &mfHeader, sizeof(mfHeader) );

	ALDUSMFHEADER aldusMFHeader;
	::ZeroMemory( &aldusMFHeader, sizeof(aldusMFHeader) );


	HENHMETAFILE emf = 0;

	CMemFile file( m_pData, m_uiSize );		

	if (file.Read(&dwIsAldus, sizeof(dwIsAldus)) != sizeof(dwIsAldus))
		return NULL;
	
	if (dwIsAldus != ALDUSKEY) {
		// A windows metafile, not a placeable wmf 
		seekpos = 0;
		::ZeroMemory(&aldusMFHeader, sizeof(aldusMFHeader));
	}
	else {  
		// This is a placeable metafile 
	        // Convert the placeable format into something that can
	        // be used with GDI metafile functions 
		file.Seek(0, CFile::begin);
		if (file.Read(&aldusMFHeader, ALDUSMFHEADERSIZE) != ALDUSMFHEADERSIZE)
			return NULL;
		seekpos = ALDUSMFHEADERSIZE;
	}
	file.Seek(seekpos, CFile::begin);
	// Read the metafile header 
	if (file.Read(&mfHeader, sizeof(mfHeader)) != sizeof(mfHeader))
		return NULL;
	// At this point we have a metafile header regardless of whether
	// the metafile was a windows metafile or a placeable metafile
	// so check to see if it is valid.  There is really no good
	// way to do this so just make sure that the mtType is either
	// 1 or 2 (memory or disk file) 
	if ((mfHeader.mtType != 1) && (mfHeader.mtType != 2)) 
		return NULL;
	// Allocate memory for the metafile bits 
	dwSize = mfHeader.mtSize * 2;
	BYTE* lpMFBits = new BYTE [dwSize];
	if (lpMFBits == NULL)
		return NULL;
	// Seek to the metafile bits 
	file.Seek(seekpos, CFile::begin);
	// Read metafile bits and create
	// the enhanced metafile
	if (file.Read(lpMFBits, dwSize) == dwSize)
		emf = ::SetWinMetaFileBits(dwSize, lpMFBits, NULL, NULL);
	// Done with the bit data	
	delete [] lpMFBits;

	nWidth = aldusMFHeader.bbox.right - aldusMFHeader.bbox.left;
	nHeight = aldusMFHeader.bbox.bottom - aldusMFHeader.bbox.top;

	if( nWidth < 0 )
		nWidth = -nWidth;

	if( nHeight < 0 )
		nHeight = -nHeight;

	return emf;
}

/////////////////////////////////////////////////////////////////////////////
CWmf::~CWmf( )
{

}

