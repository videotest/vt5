#include "StdAfx.h"
#include "resource.h"
#include <vfw.h>
#include <io.h>
#include <fcntl.h>
#include "VTAviInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define MAXNUMSTREAMS 100

static int gcpavi;
static PAVISTREAM gapavi[MAXNUMSTREAMS];

void SaveWithDifferentRate(PAVISTREAM ps, LPCSTR lpFilename, DWORD dwNewRate, DWORD dwNewScale, int id)
{
	PAVIFILE         pf; 
    PAVISTREAM       psNew;
	HRESULT          hr; 
    AVISTREAMINFO    strhdr;
//	BITMAPINFOHEADER bi; 
//	BITMAPINFOHEADER biNew;
	LONG             lStreamSize; 
    LPVOID           lpOld;
	LPVOID           lpNew;
	VTVideoInfo      VideoInfo;
    // Determine the size of the format data using     // AVIStreamFormatSize. 
    AVIStreamFormatSize(ps, 0, &lStreamSize); 
//    if (lStreamSize > sizeof(bi)) // Format too large?
//		return;  
//    lStreamSize = sizeof(bi);
	CArray<BYTE,BYTE&> buff;
	buff.SetSize(max(lStreamSize,sizeof(BITMAPINFOHEADER)));
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)buff.GetData();;
	memset(lpbi, 0, max(lStreamSize,sizeof(BITMAPINFOHEADER)));
    hr = AVIStreamReadFormat(ps, 0, lpbi, &lStreamSize); // Read format 
//	if (bi.biCompression != BI_RGB) // Wrong compression format? 
//		return;
	hr = AVIStreamInfo(ps, &strhdr, sizeof(strhdr));  
	// Create new AVI file using AVIFileOpen. 
	hr = AVIFileOpen(&pf, lpFilename, OF_WRITE | OF_CREATE, NULL); 
	if (hr != 0)
		return; 
	// Write supplementary info
	memset(VideoInfo.szLabel, 0, sizeof(VideoInfo.szLabel));
	strcpy(VideoInfo.szLabel, "Videotest video info");
	VideoInfo.nVer = 100;
	VideoInfo.dwOrigRate = strhdr.dwRate;
	VideoInfo.dwOrigScale = strhdr.dwScale;
	AVIFileWriteData(pf, streamtypeVTVI, &VideoInfo, sizeof(VideoInfo));
// Set parameters for the new stream.
//	biNew = bi;
//    biNew.biSizeImage = ((((UINT)biNew.biBitCount * biNew.biWidth 
//                        + 31)&~31) / 8) * biNew.biHeight; 
    SetRect(&strhdr.rcFrame, 0, 0, (int) lpbi->biWidth, 
            (int) lpbi->biHeight);  
    // Create a stream using AVIFileCreateStream. 
	strhdr.dwScale = dwNewScale;
	strhdr.dwRate  = dwNewRate;
    hr = AVIFileCreateStream(pf, &psNew, &strhdr); 
    if (hr != 0)
	{            //Stream created OK? If not, close file.
		CString s;
		s.Format(id, lpFilename[0]);
		AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
        AVIFileRelease(pf);
		return;
	}  
	// Set format of new stream using AVIStreamSetFormat. 
	hr = AVIStreamSetFormat(psNew, 0, lpbi, lStreamSize);
	if (hr != 0)
	{
		CString s;
		s.Format(id, lpFilename[0]);
		AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
		AVIStreamRelease(psNew); 
        AVIFileRelease(pf);
		return;
	}  
    // Allocate memory for the bitmaps. 
	DWORD dwSizeUncompressed1 = ((((UINT)lpbi->biBitCount * lpbi->biWidth
		+ 31)&~31) / 8) * lpbi->biHeight; 
	DWORD dwSizeUncompressed2 = ((((UINT)lpbi->biBitCount * lpbi->biWidth
		+ 31)&~31) / 8) * lpbi->biHeight;
	DWORD dwSize1 = max(dwSizeUncompressed1,lpbi->biSizeImage);
	DWORD dwSize2 = max(dwSizeUncompressed2,lpbi->biSizeImage);
	lpOld = malloc(dwSize1);
	lpNew = malloc(dwSize2);  
    // Read the stream data using AVIStreamRead. 
	for (lStreamSize = AVIStreamStart(ps); lStreamSize <
		AVIStreamEnd(ps); lStreamSize++)
	{ 
		hr = AVIStreamRead(ps, lStreamSize, 1, lpOld, dwSize1,
            NULL, NULL);
		if (hr == 0)
		{
			//  
			// Compress the data. 
//			CompressDIB(&bi, lpOld, &biNew, lpNew);
			memcpy(lpNew,lpOld,lpbi->biSizeImage);
			// Save the compressed data using AVIStreamWrite. 
			hr = AVIStreamWrite(psNew, lStreamSize, 1, lpNew,
				lpbi->biSizeImage, AVIIF_KEYFRAME, NULL, NULL);
		}
		if (hr != 0)
		{
			CString s;
			s.Format(id, lpFilename[0]);
			AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
			break;
		}
	}
	free(lpOld);
	free(lpNew);
	// Close the stream and file.
	AVIStreamRelease(psNew); 
	AVIFileRelease(pf);
} 

void FindVideoStreamAndSave(LPCSTR lpFilename, DWORD dwNewRate, DWORD dwNewScale, int id)
{
	AVISTREAMINFO     avis; 
//	LONG    r, lHeight = 0;
//	WORD    w;
//	int     i;
//	RECT    rc;  
// Walk through all streams.
	for (int i = 0; i < gcpavi; i++)
	{ 
        AVIStreamInfo(gapavi[i], &avis, sizeof(avis));  
        if (avis.fccType == streamtypeVIDEO)
		{  
			SaveWithDifferentRate(gapavi[i], lpFilename, dwNewRate, dwNewScale, id);
		} 
	}
} 

void SaveAVIFile(PAVIFILE pfile, LPCSTR lpszNewFile, DWORD dwNewRate, DWORD dwNewScale, int id)
{
	int    i; 
	gcpavi = 0;      // Open the streams until a stream is not available. 
	for (i = gcpavi; i < MAXNUMSTREAMS; i++)
	{
		gapavi[i] = NULL; 
		if (AVIFileGetStream(pfile, &gapavi[i], 0L, i - gcpavi) != AVIERR_OK)
			break;
		if (gapavi[i] == NULL)
			break;
	}
	// Display error message-stream not found. 
	if (gcpavi == i)
	{
		AfxMessageBox("Unable to open input stream"); 
//        if (pfile) // If file is open, close it
//			AVIFileRelease(pfile); 
        return;
	}
	else
	{
		gcpavi = i;
	}  //  . 
	FindVideoStreamAndSave(lpszNewFile, dwNewRate, dwNewScale, id);
	for (i = 0; i < gcpavi; i++)
		AVIStreamRelease(gapavi[i]);
}

CString make_unique_AVI_name(const char *pszAVIFileName)
{
	char szBuf[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char name[_MAX_FNAME];

	_splitpath( pszAVIFileName, drive, dir, NULL, NULL );
	for (int i = 0;;i++)
	{
		wsprintf(name,"$-%d",i);
		_makepath( szBuf, drive, dir, name, "AVI" );
		int h = _open(szBuf, _O_RDONLY);
		if (h == -1)
			return CString(szBuf);
		close(h);
	}
}
		
void ConvertAVIFile(const char *pszAVIFileName, DWORD dwNewRate, DWORD dwNewScale, int id)
{
	// renaming old file.
	CString sTemp = make_unique_AVI_name(pszAVIFileName);
	if (rename(pszAVIFileName,sTemp) == -1)
	{
		CString s;
		s.Format(id, sTemp[0]);
		AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
	}
	// opening old file
    LONG hr;
	PAVIFILE pfile;  
    AVIFileInit();          // opens AVIFile library  
    hr = AVIFileOpen(&pfile, sTemp, OF_SHARE_DENY_WRITE, 0L); 
    if (hr != 0)
	{
		CString s;
		s.Format("Error opening AVI file (0x%X)", hr);
		AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
		return; 
    }
	SaveAVIFile(pfile, pszAVIFileName, dwNewRate, dwNewScale, id);
    AVIFileRelease(pfile);  // closes the file 
    AVIFileExit();          // releases AVIFile library 
	unlink(sTemp);
}
