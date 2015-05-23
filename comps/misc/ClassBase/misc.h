#pragma once

#include <atlstr.h>
static void split( CString strSource, CString strSplitter, CString **Dest, long *lSz )
{
	int nLen = strSource.GetLength();
	int nLen2 = strSplitter.GetLength();

	*lSz = 0;
	*Dest = new CString[nLen];

	int nPrev = 0;
	for( int i = 0; i < nLen; i++ )
	{
		if( strSource[i] == strSplitter[0] )
		{
			bool bOK = true;
			for( int j = 1; j < nLen2; j++ )
			{
				if( strSource[i + j] != strSplitter[j] )
					bOK = false;
			}

			if( bOK )
			{
				(*Dest)[*lSz] = strSource.Mid( nPrev, i - nPrev );
				nPrev = i + 1;
				(*lSz)++;
			}
		}
	}
	if( nPrev != nLen )
	{
		(*Dest)[*lSz] = strSource.Mid( nPrev, nLen - nPrev );
		(*lSz)++;
	}

}