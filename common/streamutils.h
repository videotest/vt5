#ifndef __streamutils_h__
#define __streamutils_h__

#include "defs.h"
#include "data5.h"

struct std_dll ObjectStreamInfo
{
	DWORD			dwStructSize;
	DWORD			dwVersion;
	GUID			clsidObj;
	DWORD			dwStartPos;
	DWORD			dwObjectSize;	
	BOOL			bSerializeAsText;
	char			szObjectType[30];

};

std_dll bool SafeReadObject( IStream* pStream, IUnknown** ppunkObj, LPCSTR szObjectName, bool* pbRecoverSeekSucceded, SerializeParams *pparams );


std_dll bool ReadObject( IStream* pStream, IUnknown** ppunkObj, ObjectStreamInfo* pInfo, SerializeParams *pparams );
std_dll bool WriteObject( IStream* pStream, IUnknown* punkObj, ObjectStreamInfo* pInfo, bool bForceWriteAsText, SerializeParams *pparams );

std_dll DWORD GetStreamPos( IStream* pStream );
std_dll bool StreamSeek( IStream* pStream, DWORD dwOrigin, long lOffset ); 

#endif//__streamutils_h__
