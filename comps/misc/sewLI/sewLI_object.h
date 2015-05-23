#pragma once

#include "sewLI.h"
#include "iSewLI.h"
#include "object5.h"
#include <atlstr.h>
#include "resource.h"
#include "\vt5\awin\misc_list.h"
#include "SewUtils.h"
#include "DistMap.h"
#include "atltypes.h"
#include "NotifySpec.h"

// {8A43D293-383D-42d5-81B7-05C423D6F01E}
static const GUID clsidSewFragment = 
{ 0x8a43d293, 0x383d, 0x42d5, { 0x81, 0xb7, 0x5, 0xc4, 0x23, 0xd6, 0xf0, 0x1e } };

class CSewLIFragment;
class CFragmentImage
{
public:
	CFragmentImage();

	IUnknownPtr m_ptrImage;
//	CTranspMap m_TranspMap;
	CDistMap m_DistMap;
	int m_nZoom;
	int m_nLoadCount;
	bool m_bChanged;
	bool m_bSavingImg;
	CSewLIFragment *m_pFragment;

	_bstr_t MakePath(_bstr_t *pbstrUndo = NULL);

	void MakeTransMap();
	bool CheckImageValid();

	void LoadImage();
	void FreeImage();
	IUnknownPtr RebuildImage(IUnknown *punkImg11);
	bool SaveImage(IUnknown *punkImg);
	void DeleteImage();
	void RestoreImage();
};


class CSewLIFragment : public CObjectBase, public ISewFragment
{
	route_unknown();
	CFragmentImage *m_pImages;
	int m_nImages;
	CString m_sExt;
	friend class CFragmentImage;
	bool m_bDirInited;
//	bool m_bSavingImg;
	void InitDir();
public:
	bool m_bTerminal;
	bool m_bChanged;
	_point m_ptOffset;			//fragment position
	_size m_szSize;		//fragment size
	long m_lFragNum;
	ISewImageList *m_pList; // without AddRef
	CSewLIFragment();
	~CSewLIFragment();

	static CSewLIFragment *Make();
	int FindImage(int nZoom);
	void CheckImages();

	virtual IUnknown *DoGetInterface( const IID &iid );

	//IPersist helper
	virtual GuidKey GetInternalClassID(){ return clsidSewFragment; }

	//ISerializableObject
	com_call Load( IStream *pStream, SerializeParams *pparams );
	com_call Store( IStream *pStream, SerializeParams *pparams );

	// ISewFragment
	com_call GetSize(SIZE *psz);
	com_call SetSize(SIZE sz);
	com_call GetOffset(POINT *pptOffset);
	com_call SetOffset(POINT ptOffs);
	com_call GetImage(int *pnZoom, IUnknown **ppunkImage);
	com_call SetImage(IUnknown *punkImage);
//	com_call GetTransparencyMap(int *pnZoom, ITransparencyMap **ppTranspMap);
	com_call GetDistanceMap(int *pnZoom, IDistanceMap **ppDistanceMap);
//	com_call RebuildTransparencyMaps();
	com_call SetFragNum(long lNum);
	com_call GetFragNum(long *plNum);
	com_call LoadImage(int nZoom);
	com_call FreeImage(int nZoom);
	com_call SetListPtr(ISewImageList *pList);
	com_call RestoreImages();
};

class CTotalImage
{
	IImagePtr m_ptrImage;
public:
	CTotalImage();
	~CTotalImage();
	void Load(IStream *pStream, SerializeParams *pparams);
	void Save(IStream *pStream, SerializeParams *pparams);
	bool IsEmpty();
	CSize GetSize();
	IImagePtr GetImage();
	void Resize(CRect rcCoordSpacePrev, CRect rcCoordSpaceNew);
	CRect CalcDstRect(CRect rcCoordSpace, CRect rcSrc);
	void MapImage(CRect rcDst, IImage *pimgSrc);
	void ClearRect(CRect rcDst);
};


class  CSewLIBase : public CObjectBase,
					public ISewImageList,
					public INotifyObject,
					public INotifyPlace,
					public _dyncreate_t<CSewLIBase>
{
	route_unknown();

	_bstr_t m_bstrFragmentDir;
	_bstr_t m_bstrFragmentFmt;
	CFragList m_listTerm;
	TPOS m_lPosActive;
	CRect m_rcTotal;
	int m_nOvrPerc;
	bool m_bSaving;
	CTotalImage m_TotalImage;
	void RebuildTotalImage(int nRects, RECT *pRects);
	void RebuildTotalImage(CRect rcCoordSpace);
	GuidKey m_guidDoc;
	bool m_bDirInited;
	void InitDir();
public:
	CSewLIBase();
	virtual ~CSewLIBase();

protected:
	//IPersist helper
	virtual GuidKey		GetInternalClassID();
	virtual IUnknown*	DoGetInterface( const IID &iid );

	IUnknownPtr MakeFragment(IUnknown *punkImage, int nRange, POINT ptOffs, SIZE szImage);
	void RecalcTotalRect();
	void RecalcOvrPercent(bool bFireEvent);
	int CalcOverPercentByRect(CRect rc, TPOS lPosSkip);
	void ClearUndo();
//	bool PromptForDir();
//	bool CheckFragmentDirValid(bool *pbChanged);
	void MoveFragmentsDir();
	void ClearOrphantImages();

public:
	//ISerializableObject
	com_call Load( IStream *pStream, SerializeParams *pparams );
	com_call Store( IStream *pStream, SerializeParams *pparams );

	// INotifyPlace
	com_call NotifyPutToData( IUnknown* punkObj );
	com_call NotifyPutToDataEx( IUnknown* punkObj, IUnknown** punkUndoObj );
	com_call Undo( IUnknown* punkUndoObj );
	com_call Redo( IUnknown* punkUndoObj );


	//INamedDataObject2
	com_call IsBaseObject(BOOL * pbFlag) ;
	com_call GetType( /*[out, retval]*/ BSTR *pbstrType );

	com_call OnActivateObject();
	com_call GetObjectFlags(DWORD *pdwObjectFlags);
	com_call GetFirstFragmentPos(TPOS *plPos);
	com_call GetNextFragment(TPOS *plPos, ISewFragment **ppFragment);
	com_call GetPrevFragment(TPOS *plPos, ISewFragment **ppFragment);
	com_call GetFragmentsCount(long *plCount);
	com_call GetOrigin(POINT *pOrg);
	com_call GetSize(SIZE *pSize);
	com_call GetPath(BSTR *pbstrPath, BOOL bSaving);
	com_call GetRangeCoef(int *pnRangeCoef);
	com_call AddImage(IUnknown *punkImage, POINT ptOffs, DWORD dwFlags);
	com_call GetActiveFragmentPosition(TPOS *plPos);
	com_call SetActiveFragmentPosition(TPOS lPos);
	com_call GetLastFragmentPosition(TPOS *plPos);
	com_call AddFragment(ISewFragment *pFragment);
	com_call MoveFragment(ISewFragment *pFragment, POINT ptNewPos,DWORD dwFlags);
	com_call DeleteFragment(TPOS lPosFrag);
	com_call GetOverlapPercent(int *pnOvrPerc);
	com_call CheckOverlapPercent();
	com_call CalcOrigFragOffset(ISewFragment **ppPrev, POINT *pptOffs);
	com_call IsSavingOrLoading(BOOL *pbSaving, BOOL *pbLoading);
	com_call GetTotalImage(IUnknown **ppunk);
	com_call GetFragmentsFormat(BSTR *pbstrFragFmt);

// INotifyObject
	com_call NotifyCreate();
	com_call NotifyDestroy();
	com_call NotifyActivate( bool bActivate );
	com_call NotifySelect( bool bSelect );


	ITypeLib*	m_pi_type_lib;
	ITypeInfo*	m_pi_type_info;

protected:
	DWORD	m_dwFlags;
	GuidKey	m_keyBase;
	INamedDataInfo	*m_pinfo;
	BOOL		m_bModified;	
	long	m_lFragNum; // Unique fragment number
	long	m_l1stFragNum; // m_lFragNum when object was loaded
	int		m_nRangeCoef;
};

class CSewLIBaseInfo : public CDataInfoBase,
					public _dyncreate_t<CSewLIBaseInfo>
{
public:
	CSewLIBaseInfo()	: CDataInfoBase( clsidSewLIBase, szTypeSewLI, 0, IDI_SEWLI )
	{ }
};