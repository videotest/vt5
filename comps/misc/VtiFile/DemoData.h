#if !defined(__Data_H__)
#define __Data_H__

class CImageBase;
namespace DataCnv
{

const int DataSize = 256;

BYTE GetSeed1(int x, int y);
BYTE GetSeed2(BYTE *pbData, int x, int y);
BYTE GetSeed3(BYTE *pbData, int x, int y);
void InitData(BYTE *pbData);
void InitData4(BYTE *pbData);
CImageBase *ConvertImage(CImageBase *pimgSrc, BYTE *pbData);

};

#endif

