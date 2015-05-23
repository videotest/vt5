#ifndef __wmf_h__
#define __wmf_h__

// Placeable metafile data definitions 
typedef struct tagOLDRECT
{
    short left;
    short top;
    short right;
    short bottom;
} OLDRECT;


typedef struct {
        DWORD   key;
        WORD	hmf;
        OLDRECT	bbox;
        WORD    inch;
        DWORD   reserved;
        WORD    checksum;
} ALDUSMFHEADER;

#define	METAFILE_VERSION	1
#define	ALDUSKEY		0x9AC6CDD7
#define	ALDUSMFHEADERSIZE	22	// Avoid sizeof is struct alignment >1


class CWmf
{
public:
	CWmf( BYTE* pData, UINT uiSize );
	HENHMETAFILE CreateMetaFile( int& nWidth, int& nHeight );
	~CWmf( );

protected:
	BYTE* m_pData;
	UINT m_uiSize;

};

#endif//wmf
