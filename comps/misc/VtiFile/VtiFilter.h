#if !defined(__VtiFilter_H__)
#define __VtiFilter_H__

#include "Common.h"

#include "Image.h"
#include "Snaptext.h"
#include "ObjMease.h"
#include "CellInfo.h"

class CGXGridParam;
class CVtiFileFilter : public CFileFilterBase  
{
	DECLARE_DYNCREATE(CVtiFileFilter);
	GUARD_DECLARE_OLECREATE(CVtiFileFilter);

protected:
	CTextArray	m_Text;
	CTextArray	m_GalText;
	CImageBase *m_pImg,*m_pIconImage;
	C8Image m_BinImg;
	CObjMove m_objOrig;
    CObjArray		m_ObjArray;			//manual objects - 0...m_iHandyObjCount-1
										//automatic objects -	  m_iHandyObjCount-m_ObjArray.GetSize-1
										//for faster memory access handy object placed in first part of array, because
										//more actions with automatic object are time-criticly
	int     m_nMinObjClass;
	int		m_iActiveObject;
	int		m_iHandyObjCount;
	double	m_fArea;

	CGXGridParam *m_pparamAuto;
	CGXGridParam *m_pparamHandy;
	CGalParams	 *m_pGalParam; //galery params - cell's position etc

	IUnknownPtr m_Image;
	BOOL m_bIsRGB;
	CArray<GuidKey,GuidKey&> m_arrGuids;
	GUID m_guid1;

	void SerializeClassTableInfo( CArchive &ar );
	void SerializeIcon( CArchive &ar );
	void SerializeImage( CArchive &ar );
	void SerializeBinImage( CArchive &ar );
	void SerializeDataList( CArchive &ar );
	void SerializeTableParam( CArchive &ar );
	void SerializeObjects( CArchive &ar );
	void DoLoadDemoVTI( CArchive &ar );
	void DoLoadVTI( CArchive &ar );


	bool DoReadFile(const char *pszFileName);
	void InitImage(const char *pszFileName);
	void InitClassesList(const char *pszFileName);
	void InitObjectsList(const char *pszFileName);
	void ClearAfterRead();

public:
	CVtiFileFilter();
	virtual ~CVtiFileFilter();

	virtual bool InitNew(){return true;}
	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );

	virtual void OnSetHandlers(){};

	virtual int  GetPropertyCount();
	virtual void GetPropertyByName(BSTR bstrName, VARIANT *pvarVal);
	virtual void GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx);
	
	int m_nWidth;
	int m_nHeight;
	int m_nColors;
	int m_nDepth;
	int m_nNumPages;
	CString m_strCompression;
};




#endif