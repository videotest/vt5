#ifndef __IMAGE_H__
#define __IMAGE_H__

// Get width of line from x size
#define ROUNDBY4(x) ((((x)+3)>>2)<<2)
// Get width of line from x size in bits
#define ROUNDBY32(x) ((((x)+31)>>5)<<2)
// ensure that number >=0 and <=255
#define ROUNDBYTE(b) ((BYTE)min(255,max(0,b)))

extern int g_iRedCoef;
extern int g_iGreenCoef;
extern int g_iBlueCoef;

inline int _Bright ( int b, int g, int r )	//функция для получения яркости
{
	return (r*60+g*118+b*22)/200;
}

inline int SBright ( int b, int g, int r )	//функция для получения яркости
{
	return (r*g_iRedCoef+g*g_iGreenCoef+b*g_iBlueCoef)/200;
}

template <class COLORREPR> class CImage;
template <class FROMCOLORREPR, class TOCOLORREPR> class CColorConvert
{
public:
	// nSize here in sizeof(FROMCOLORREPR)
	static int  Convert(TOCOLORREPR *pTo, const FROMCOLORREPR *pFrom, int nSize);
	static void DoAttach( CImage<TOCOLORREPR> &Dest, const CImage<FROMCOLORREPR> &Src);
};


class CHisto
{
public:
	long *m_pData0;
	long *m_pData1;
	long *m_pData2;
	long *m_pData;
	int  m_nDataNum;
	int  m_nMax0,m_nMax1,m_nMax2;
	int  m_nMaxBright;
	int  m_nPointsNum;
	BOOL m_fTrueColor;
	void InitHisto(long nDataNum, BOOL fTrueColor = FALSE);
	void ClearHisto();
	void CalcSum();
	CHisto(long nDataNum, BOOL fTrueColor = FALSE);
	CHisto();
	~CHisto();
	void SmoothHisto(int nLev);
	int GetData(int i)
	{
		return m_fTrueColor?
			_Bright(m_pData0[i],m_pData1[i],m_pData2[i]):
			m_pData0[i];
	};
};

class CImageBase : public CObject
{
	DECLARE_DYNAMIC(CImageBase);
public:
	enum ImageType
	{
		GrayScale      = 0,
		TrueColor      = 1,
		Color16Bit     = 2,
		NoInit         = 0x80000000
	};
	CImageBase(void);
	// Virtual destructor
	virtual ~CImageBase(void) {};
	// Creation function.
	virtual void Create( short cx, short cy, int fsOptions ) = 0;
	// Copying and transferring images.
	// Next three functions transfers an image memory blocks from one
	// object to another.
	virtual long  GetDataSize(){return 0;};
	virtual void Assign8( CImage<BYTE>& Image ) = 0;
	virtual void Assign16( CImage<WORD>& Image ) = 0;
	void operator=( CImageBase *pImage );
	virtual void Attach16( CImage<WORD> const &Image ) = 0;
	virtual void Attach8( CImage<BYTE> const &Image ) = 0;
	void Attach( CImageBase *pImage);
	virtual void Attach16( CImage<WORD> const &Image, CRect const &Rect ) = 0;
	virtual void Attach8( CImage<BYTE> const &Image, CRect const &Rect ) = 0;
	void Attach( CImageBase *pImage, CRect const &Rect );
	virtual void Attach( HGLOBAL hDIB ) = 0;
	virtual HGLOBAL GetCopyHandle( int nPalEntries = 0, const RGBQUAD *pPalette = NULL ) = 0;
	virtual void Free() = 0;
	// Conversion functions
	// Both functions and both operators returns pointers to memory
	// block, allocated by AllocMem.
	virtual BYTE *ConvertTo8Bit() const = 0;
	virtual WORD *ConvertTo16Bit() const = 0;
	// Getting and setting attributes
	short GetCX() const { return m_cx; }
	short GetCY() const { return m_cy; }
	short GetCols() const { return m_iCols; }
	short GetIncr() const { return m_iIncr; }
	LPBITMAPINFOHEADER GetBI();
	BOOL IsTrueColor() const;
	BOOL IsGrayScale() const;
	static BOOL IsTrueColor( unsigned nBitCount);
	static BOOL Is16BitColor( unsigned nBitCount);
	static BOOL IsGSPalette( LPBITMAPINFOHEADER lpbi );
	virtual BOOL Is16BitColor() const = 0;
	BOOL IsEmpty() const;
	BOOL IsModified(unsigned nRowNum) const;
	void SetModified(unsigned nRowNum, BOOL fModified = TRUE);
	// Histogram functions
	virtual void CreateHisto(CHisto& Histo, CImageBase *pimgMask = NULL, CRect rc = CRect(0,0,0,0)) const = 0;
	// drawing functions
	virtual BOOL Draw( CDC *pDC, short x=0, short y=0 ) =0;
	virtual BOOL Draw( CDC *pDC, CRect rcSrc, CRect rcDest, 
		CPalette * = 0, BOOL bForseStretch = FALSE ) =0;
	// function working woth palette
	virtual void ReplacePalette(const RGBQUAD *pRGBQuads) = 0;
	virtual void ReadBMP( CFile &file ){};
	virtual void WriteBMP( CFile &file ){};
	virtual void Write( LPCSTR pszFileName ) = 0;
	virtual void Write( CFile *pFile ) = 0;
	static CImageBase *MakeFromFile( const char *pszFileName );
	static CImageBase *MakeFromFile( CFile &file);
	// lpbi will not be freed during deletion of this object.
	// Since, it may be static or other not - heap data.
	static CImageBase *MakeFromBits( LPBITMAPINFOHEADER lpbi, BOOL fCopy = TRUE);
	static CImageBase *MakeFromBits( HGLOBAL h);
	static CImageBase *CreateNew( unsigned iBitCount, int cx, int cy, COLORREF clr );

	virtual long  GetBitsSize() const = 0; // size in bytes ( to use in memcpy, etc.)
	virtual long  GetConvBitsSize() const = 0; // size in sizeof(COLORREPR)
	//image processing
	virtual void Fill(COLORREF clr) = 0;

//andy modification - direct support for m_fNotFree flag	06.09.96
//use for manual clear this flag after load bitmap from file
public:
	void SetFlag( BOOL fVal ){m_fNotFree = fVal;}
	// These flags used for the control over the 16 to 8 bit
	// color conversion and 16-bit images' representation.
	static int m_nBitsOffset;
	static BOOL m_fUseAllBits;
public:
	void Serialize(CArchive& ar); 
protected:

	LPBITMAPINFOHEADER m_lpbi;
	short m_cx, m_cy; // dimensions
	short m_iCols;	// width of row (line) in sizeof(COLORREPR) ( for
	                // true color images multiplyed by 3 )
	short m_iIncr;  // 1 for grayscale, 3 for true color; used in cycles
	BOOL  *m_pfModified;
	BOOL  m_bGSPalette;
public:
	BOOL	m_fNotFree;
	BOOL IsGrayPalette() {return m_bGSPalette;}
public:
	STDMETHOD_(ULONG,AddRef)() {return 1;}
	STDMETHOD_(ULONG,Release)() {return 1;}
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID FAR* ppvObj);

	STDMETHOD_(LPBITMAPINFOHEADER,GetImage)() {return m_lpbi;}
};

inline LPBITMAPINFOHEADER CImageBase::GetBI()
	{ return m_lpbi; }
inline BOOL CImageBase::IsTrueColor() const
	{ return m_lpbi->biBitCount>=24; } // bits may be 24, 30, 36 or 48
inline BOOL CImageBase::IsGrayScale() const
	{ return m_lpbi->biBitCount<=16; } // bit may be 8, 10, 12 or 16
inline BOOL CImageBase::IsTrueColor( unsigned nBitCount)
	{ return nBitCount>=24; } // bit may be 8, 10, 12 or 16
inline BOOL CImageBase::Is16BitColor( unsigned nBitCount)
	{ return nBitCount>8&&nBitCount<24||nBitCount>24&&nBitCount<=48; }
inline BOOL CImageBase::IsEmpty() const
	{ if(this==NULL) return TRUE; return m_lpbi==NULL; }
inline BOOL CImageBase::IsModified(unsigned nRowNum) const
{
	ASSERT((int)nRowNum<m_cy);
	ASSERT(m_pfModified); // array must be allocated in derived classes
	return m_pfModified[nRowNum];
}
inline void CImageBase::SetModified(unsigned nRowNum, BOOL fModified)
{
	ASSERT((int)nRowNum<m_cy);
	ASSERT(m_pfModified); // array must be allocated in derived classes
	m_pfModified[nRowNum] = fModified;
}

template <class COLORREPR> class CImage : public CImageBase
{
public:
	friend class CImageBase;
	// Constructors and destructors
	CImage<COLORREPR>(void);
	CImage<COLORREPR>( HGLOBAL hDIB ); // creates a new image from handle to DIB (makes a copy of block)
	CImage<COLORREPR>( short cx, short cy, int fsOptions );
	CImage<COLORREPR>( const CImage &Image ); // copies an image
	~CImage<COLORREPR>(void);
	// Creation functions
	// !!! void Create ( short cx, short cy, int fsOptions );
	void Create( short cx, short cy, int fsOptions );
	// Copying and transferring images
	// Next two functions transfers an image memory blocks from one
	// object to another.
	virtual void Assign8( CImage<BYTE>& Image );
	virtual void Assign16( CImage<WORD>& Image );
	// Attaches to an existing handle to DIB (makes a copy of block)
	// Data will be converted into suitable format
	void Attach( HGLOBAL hDIB );
	// Next two functions ataches to existing memory block. Data
	// will NOT be converted anywhere.
	void Attach( int nSize, void *pData, CRect *pRect = NULL, BOOL AsIs = FALSE );
	// Function not copies the data. Data block will not be freed.
	void AttachNoCopy( int nSize, void *pData, CRect *pRect = NULL );
	void AttachNoCopy( LPBITMAPINFOHEADER lpbi, CRect *pRect = NULL );
	// Copies an image
	void Attach16( CImage<WORD> const &Image );
	void Attach8( CImage<BYTE> const &Image );
	void Attach( CImageBase *pImage)
		{ CImageBase::Attach(pImage); }
	void Attach16( CImage<WORD> const &Image, CRect const &Rect );
	void Attach8( CImage<BYTE> const &Image, CRect const &Rect );
	void Attach( CImageBase *pImage, CRect const &Rect )
		{ CImageBase::Attach(pImage,Rect); }
	void AttachNoCopy( const CImage<COLORREPR>& Image );
	// Creates global memory block and copies image into it.
	// Block has flags GMEM_MOVEABLE|GMEM_DDESHARE
	HGLOBAL GetCopyHandle( int nPalEntries = 0, const RGBQUAD *pPalette = NULL ) ;
	void Free();
	// Conversion functions
	// Both functions and both operators returns pointers to memory
	// block, allocated by AllocMem.
	BYTE *ConvertTo8Bit() const;
	WORD *ConvertTo16Bit() const;
	// Obtaining the image bits
	inline COLORREPR *operator[](int y) const;
	COLORREPR *GetBitsPtr() const; // returns pointer to bits ( base + header + size of palette )
	// Getting and setting attributes
	BOOL Is16BitColor() const ;//{ return sizeof(COLORREPR)==2;}
	BOOL Is8BitColor() const ;
// Operations
	// Histo may be new created or already used. In last case, all
	// memory will be safely destroyed.
	void CreateHisto(CHisto& Histo, CImageBase *pimgMask = NULL, CRect rc = CRect(0,0,0,0)) const;
	// control functions

	void SetImageDataPtr( COLORREPR *pbyte )
	{
		m_pBits = pbyte;
		AllocRows();
	}

	/* drawing functions */
	BOOL Draw( CDC *pDC, short x=0, short y=0 );
	BOOL Draw( CDC *pDC, CRect rcSrc, CRect rcDest, 
		CPalette * = 0, BOOL bForseStretch = FALSE );

	/* reading and writing */
	void Read( LPCSTR szFileName );
	void Read( CFile *pFile ){ReadBMP(*pFile);}
	void Write( LPCSTR szFileName );
	void Write( CFile *pFile ) {WriteBMP(*pFile);}

	virtual void Serialize(CArchive& ar); 

	// function, workinf with palette
	void ReplacePalette(const RGBQUAD *pRGBQuads);
	const RGBQUAD *GetRGBQuad() const;

	// static function - needed for nonversion
	static BYTE AsByte(COLORREPR Sample);
	static BYTE AsByteEx(COLORREPR Sample);
	static BYTE DrawByte(COLORREPR Sample);
	static WORD AsWord(COLORREPR Sample);
	static WORD AsWordEx(COLORREPR Sample);
	static COLORREPR Convert(BYTE Sample);
	static COLORREPR Convert(WORD Sample);
	static int MaxIntensity();

	// information functions
	long  GetBitsSize() const; // size in bytes ( to use in memcpy, etc.)
	long  GetConvBitsSize() const; // size in sizeof(COLORREPR)

	//image processing
	void Fill(COLORREF clr);

protected:
	friend class CColorConvert<BYTE,BYTE>;
	friend class CColorConvert<BYTE,WORD>;
	friend class CColorConvert<WORD,BYTE>;
	friend class CColorConvert<WORD,WORD>;
	static short CalcColsNum( short cx, int fsOptions );
	static short CalcInc( int fsOptions );
	// Returns number of palette entries for given header.
	static int   GetPaletteEntriesNum( const BITMAPINFOHEADER *pBI );
	// Returns number of palette entries for this CImage.
	int   GetPaletteEntriesNum() const;
	// Returns size of RGB palette in bytes.
	int   GetPaletteSize() const;
	// For histograms.
	int   GetColorsNum() const; // number of colors depends from COLORREPR
	int   GetColor(COLORREPR c) const;
	// Uses information, stored in member vars. If buffer already
	// allocated, changes the size
	void  AllocBuffer( int nSize, unsigned nRows, int fsOptions ); // alloc data buffer by given size
	void  AllocRows();   // allocs array of rows
	void  DeallocBuffer();
	// size of all buffer in bytes for data allocation
	// first function uses its parameters, second - data members,
	// which must be initialized
public:
	static long  GetDataSize( short cx, short cy, int fsOptions );
	virtual long  GetDataSize();
	static void InitColorConv();
	static void CalcConvData();
	static void DeInitColorConv();
	static double m_CnvKoef;
	static BOOL m_fUseGC;
	void AttachRGB555(LPBITMAPINFOHEADER lpbi);
protected:
	void  DeInit(); // variables will be deinitialized, but memory
	                // will not be freed
	// Initializes internal variable using information in *pHeader
	// to describe data as it will be converted into internal
	// representation, returns values from enum ImageType
	int  ImportHeader(BITMAPINFOHEADER *pHeader, BOOL bNotChangeSize = FALSE);
	// Initializes *m_lpbi by default values, using other class
	// members. All member variables must be initialized but m_lpbi
	void  InitHeader(int fsOptions);
	// Fills *pHeader to describe data as it will be exported
	void  ExportHeader(BITMAPINFOHEADER *pHeader) const;
	// Converts header after convertation data
	void  ConvertHeader();
	// Checking header for validness
	static void TestHeader(const LPBITMAPINFOHEADER pHeader);
	// Functions, working with palettes
	CPalette *GetPalette();
	void CreatePalette();
	void DestroyPalette();
	// This functions copies colors from logical palette to m_pRGBQuad
	// data members, but NOT creates CPalette object
	void CopyPaletteData( CPalette *pPal );
	// This function initializes the palette bits (used only for
	// grayscale images).
	void InitPaletteBits(RGBQUAD *pRGBQuad) const;
	// internal functions for Read(...) and Write(...)
	void ReadBMP( CFile &file );
	void WriteBMP( CFile &file );
	// data members
	CPalette *m_pPalette;
	RGBQUAD  *m_pRGBQuad;
	CArray<COLORREPR *,COLORREPR *&> m_Rows;  // array of pointers to rows
	COLORREPR *m_pBits;
	long 	m_nAllocSize; // size of block of the memory, pointed by 
	                      // m_pBits
//10.09.96 - m_fNotFree remove to base class for give acess to it
	CRITICAL_SECTION m_crsec;
	BOOL m_bLocked;
	void  DoIsGSPalette(); // defines, whether palette is gray and sets
	                       // variable m_bGSPalette.
public:
	static BYTE *m_pCnvBuffer;
};

typedef CImage<BYTE> C8Image;
typedef CImage<WORD> C16Image;
#define  COLORREPR_  WORD

inline BYTE CImage<BYTE>::AsByte(BYTE Sample) { return Sample;}
inline BYTE CImage<BYTE>::DrawByte(BYTE Sample) { return m_pCnvBuffer[Sample];}
inline BYTE CImage<BYTE>::AsByteEx(BYTE Sample) { return Sample;}
inline WORD CImage<BYTE>::AsWord(BYTE Sample) { return ((WORD)Sample)<<8;}
inline WORD CImage<BYTE>::AsWordEx(BYTE Sample) { return ((WORD)Sample)<<m_nBitsOffset;}
inline BYTE CImage<WORD>::AsByte(WORD Sample) { return (BYTE)(Sample>>8);}
inline BYTE CImage<WORD>::DrawByte(WORD Sample) { return m_pCnvBuffer[Sample];}
inline BYTE CImage<WORD>::AsByteEx(WORD Sample) { return (BYTE)(Sample>>m_nBitsOffset);}
inline WORD CImage<WORD>::AsWord(WORD Sample) { return Sample;}
inline WORD CImage<WORD>::AsWordEx(WORD Sample) { return Sample;}
inline BYTE CImage<BYTE>::Convert(BYTE Sample) { return Sample;}
inline BYTE CImage<BYTE>::Convert(WORD Sample) { return (BYTE)(Sample>>8);}
inline WORD CImage<WORD>::Convert(BYTE Sample) { return ((WORD)Sample)<<8;}
inline WORD CImage<WORD>::Convert(WORD Sample) { return Sample;}
inline int  CImage<BYTE>::MaxIntensity() {return 255;}
inline int  CImage<WORD>::MaxIntensity() {return 0xFFFF;}

template<class COLORREPR> inline COLORREPR *CImage<COLORREPR>::operator[](int y) const
//	{ return m_Rows[m_cy-y+1]; }
	{ return m_Rows[y+1]; }
template<class COLORREPR> inline COLORREPR *CImage<COLORREPR>::GetBitsPtr() const
	{ return m_pBits; }
template<class COLORREPR> inline int CImage<COLORREPR>::GetPaletteEntriesNum() const
	{ return IsTrueColor()||Is16BitColor()?0:256; }
template<class COLORREPR> inline int CImage<COLORREPR>::GetPaletteSize() const
	{ return GetPaletteEntriesNum()*sizeof(RGBQUAD); }
template<class COLORREPR> inline BOOL CImage<COLORREPR>::Is16BitColor() const
	{ return sizeof(COLORREPR)==2; }
template<class COLORREPR> inline BOOL CImage<COLORREPR>::Is8BitColor() const
	{ return sizeof(COLORREPR)==1; }
template<class COLORREPR> inline long  CImage<COLORREPR>::GetBitsSize() const
	{ return m_iCols*m_cy*sizeof(COLORREPR);}
template<class COLORREPR> inline long  CImage<COLORREPR>::GetConvBitsSize() const
	{ return m_iCols*m_cy;}
template<class COLORREPR> inline long  CImage<COLORREPR>::GetDataSize( short cols, short cy, int fsOptions)
{ return fsOptions&TrueColor?
	sizeof(BITMAPINFOHEADER)+cols*cy*sizeof(COLORREPR):
	sizeof(BITMAPINFOHEADER)+cols*cy*sizeof(COLORREPR)+256*sizeof(RGBQUAD);
}
template<class COLORREPR> inline long  CImage<COLORREPR>::GetDataSize() 
	{ return sizeof(BITMAPINFOHEADER)+m_iCols*m_cy*sizeof(COLORREPR)+GetPaletteSize();}
//template<class COLORREPR> inline LPBITMAPINFOHEADER CImage<COLORREPR>::GetBI()
//	{ return m_lpbi;}
template<class COLORREPR> inline BYTE *CImage<COLORREPR>::ConvertTo8Bit() const 
{
	BYTE *pBytes = new BYTE[GetConvBitsSize()];
	CColorConvert<COLORREPR,BYTE>::Convert(pBytes,m_pBits,GetConvBitsSize());
	return pBytes;
}
template<class COLORREPR> inline WORD *CImage<COLORREPR>::ConvertTo16Bit() const 
{
	WORD *pWords = new WORD[GetConvBitsSize()];
	CColorConvert<COLORREPR,WORD>::Convert(pWords,m_pBits,GetConvBitsSize());
	return pWords;
}
template<class COLORREPR> inline short CImage<COLORREPR>::CalcInc( int fsOptions )
{
	return fsOptions&TrueColor?3:1;
}

inline int CImage<BYTE>::GetColorsNum() const
	{ return 0x100; }

inline int CImage<WORD>::GetColorsNum() const
{
	if (m_lpbi->biBitCount==10||m_lpbi->biBitCount==30)
		return 1<<10;
	else if (m_lpbi->biBitCount==12||m_lpbi->biBitCount==36)
		return 1<<12;
	else return 1<<16;
}

inline int CImage<BYTE>::GetColor(BYTE c) const
	{ return c; }

inline int CImage<WORD>::GetColor(WORD c) const
{
	if (m_lpbi->biBitCount==10||m_lpbi->biBitCount==30)
		return c>>6;
	else if (m_lpbi->biBitCount==12||m_lpbi->biBitCount==36)
		return c>>4;
	else return c;
}

inline short CImage<BYTE>::CalcColsNum( short cx, int fsOptions )
{
	return fsOptions&TrueColor?ROUNDBY4(3*cx):ROUNDBY4(cx);
}

inline short CImage<WORD>::CalcColsNum( short cx, int fsOptions )
{
	return fsOptions&TrueColor?ROUNDBY4(3*cx):ROUNDBY4(cx);
}
template<class COLORREPR> inline const RGBQUAD *CImage<COLORREPR>::GetRGBQuad() const
	{ return m_pRGBQuad;}

class CDIBArgument;
template<class COLORREPR1, class COLORREPR2> CImage<COLORREPR1> *CreateImage(CImage<COLORREPR2> const &img, const CRect *pRect,
	CImage<COLORREPR1> &stuff, CDIBArgument *pDibArgument, CDIBArgument *pDibResult);
template<class COLORREPR1, class COLORREPR2> void AttachImage(
	CImage<COLORREPR1> &imgDst, CImage<COLORREPR2> const &img, const CRect *pRect,
	CDIBArgument *pDibArgument = NULL, CDIBArgument *pDibResult = NULL);
template<class COLORREPR1, class COLORREPR2> void SetImage(
	CImage<COLORREPR1> &imgDst, CImage<COLORREPR2> const &img,
	int xOffs, int yOffs);
// Destination image more than source.
void SetImage( CImageBase *pimgDst, CImageBase *pimg, int xOffs, int yOffs);
// Source image more than destination.
void GetImage( CImageBase *pimgDst, CImageBase *pimg, int xOffs, int yOffs);
void SwapImage( CImageBase *pimgDst, CImageBase *pimg, int xOffs, int yOffs);
void SwapImage( CImageBase *pimgDst, CImageBase *pimg, CRect rcDst, CRect rcSrc);

void DrawImage( CImageBase *pimg, CDC *pDC, CRect rcDib, CRect rcPaint, BOOL bForceStreatch=FALSE );
#endif //__IMAGE_H__
