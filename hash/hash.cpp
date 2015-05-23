// hash.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define BUFSIZE 1024
#define MD5LEN  16

int PrintHash(const TCHAR* FileName)
{
	TCHAR currDir[_MAX_PATH]; _tgetcwd(currDir, _MAX_PATH);
    DWORD dwStatus = 0;
    BOOL bResult = FALSE;
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    HANDLE hFile = NULL;
    BYTE rgbFile[BUFSIZE];
    DWORD cbRead = 0;
    BYTE rgbHash[MD5LEN];
    DWORD cbHash = 0;
    CHAR rgbDigits[] = "0123456789abcdef";

    //Logic to check usage goes here.

    hFile = CreateFile(FileName,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN,
        NULL);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        dwStatus = GetLastError();
        printf("Error opening file %s\nError: %d\n", FileName, dwStatus); 
        return dwStatus;
    }

    //Get handle to the crypto provider
    if (!CryptAcquireContext(&hProv,
        NULL,
        NULL,
        PROV_RSA_FULL,
        CRYPT_VERIFYCONTEXT))
    {
        dwStatus = GetLastError();
        printf("CryptAcquireContext failed: %d\n", dwStatus); 
        CloseHandle(hFile);
        return dwStatus;
    }

    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
    {
        dwStatus = GetLastError();
        printf("CryptAcquireContext failed: %d\n", dwStatus); 
        CloseHandle(hFile);
        CryptReleaseContext(hProv, 0);
        return dwStatus;
    }

    while (bResult = ReadFile(hFile, rgbFile, BUFSIZE, &cbRead, NULL))
    {
        if (0 == cbRead)
        {
            break;
        }

        if (!CryptHashData(hHash, rgbFile, cbRead, 0))
        {
            dwStatus = GetLastError();
            printf("CryptHashData failed: %d\n", dwStatus); 
            CryptReleaseContext(hProv, 0);
            CryptDestroyHash(hHash);
            CloseHandle(hFile);
            return dwStatus;
        }
    }

    if (!bResult)
    {
        dwStatus = GetLastError();
        printf("ReadFile failed: %d\n", dwStatus); 
        CryptReleaseContext(hProv, 0);
        CryptDestroyHash(hHash);
        CloseHandle(hFile);
        return dwStatus;
    }

    cbHash = MD5LEN;
    if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
    {
		const TCHAR* pFileName;
		size_t lenCurrDir=_tcslen(currDir);
		if(_tcsnicmp(FileName, currDir, lenCurrDir)){
			pFileName=FileName;
		}else
			pFileName=FileName+lenCurrDir+1;

        _tprintf(_T("%s\t"), pFileName);
        for (DWORD i = 0; i < cbHash; i++)
        {
            printf("%c%c", rgbDigits[rgbHash[i] >> 4],
                rgbDigits[rgbHash[i] & 0xf]);
        }
        printf("\n");
    }
    else
    {
        dwStatus = GetLastError();
        printf("CryptGetHashParam failed: %d\n", dwStatus); 
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    CloseHandle(hFile);

    return dwStatus;   
}
namespace {
	tstring dirExclude=_T("Debug;Release");
	tstring fileExclude=_T("_i.c;_p.c;_h.h");
	tstring fileInclude=_T(".cpp;.h;.rc;.rc2;.bmp;.ico;.odl;.idl");
//	using std::string::size_type;

	void ApplyFileExclude(const tstring& fileName){
		for(string::size_type pos=0,len,newpos ;; pos=newpos+1){
			newpos=fileExclude.find(';',pos);
			if(newpos==string::npos){
				len=fileExclude.size()-pos;
			}else{
				len=newpos-pos;
			}
			tstring sIncl=fileExclude.substr(pos,len);
			if(fileName.size() >= len && sIncl==fileName.substr(fileName.size()-len)){
				return;
			}
			if(newpos==string::npos) break;
		}
		PrintHash(fileName.c_str());
	}

	void HashingDir(const tstring& sDir){
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind=FindFirstFile((sDir+_T("\\*")).c_str(), &FindFileData);
		for(BOOL bDoNext=(INVALID_HANDLE_VALUE!=hFind);
			bDoNext ;
			bDoNext=FindNextFile(hFind, &FindFileData))
		{
			tstring fileName(FindFileData.cFileName);
			if(FILE_ATTRIBUTE_DIRECTORY & FindFileData.dwFileAttributes){
				if(_tcscmp(_T("."),FindFileData.cFileName) &&
					_tcscmp(_T(".."),FindFileData.cFileName) &&
					tstring::npos==dirExclude.find(fileName)){
					HashingDir(sDir+_T('\\')+fileName);
				}
			}else{
				for(tstring::size_type pos=0,len,newpos ;; pos=newpos+1){
					newpos=fileInclude.find(';',pos);
					if(newpos==string::npos){
						len=fileInclude.size()-pos;
					}else{
						len=newpos-pos;
					}
					tstring sIncl=fileInclude.substr(pos,len);
					if(fileName.size()>=len && sIncl==fileName.substr(fileName.size()-len)){
						ApplyFileExclude(sDir+_T('\\')+fileName);
						break;
					}
					if(newpos==string::npos) break;
				}
			}
		}
		FindClose(hFind);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	for(int i=0; i<argc; ++i){
		if(argv[i][0]==_T('/') ||
			argv[i][0]==_T('-'))
		{
			TCHAR* pkey=argv[i]+1;
			if(!_tcsicmp(pkey,_T("dirExclude")))
				dirExclude=pkey;
			else if(!_tcsicmp(pkey,_T("fileExclude")))
				fileExclude=pkey;
			else if(!_tcsicmp(pkey,_T("fileInclude")))
				fileInclude=pkey;
		}
	}

	TCHAR currDir[_MAX_PATH]; _tgetcwd(currDir, _MAX_PATH);
	HashingDir(currDir);

}
