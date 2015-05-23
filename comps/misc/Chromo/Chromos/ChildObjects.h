#if !defined(__ChildObjects_H__)
#define __ChildObjects_H__

#include "Chromosome.h"
#include "ChrUtils.h"
#include "woolz.h"
#include "ChromoCnv.h"

class CChromosome : public  CDataObjectBase, public CDrawObjectImpl
{
//	static const GUID guid;
	friend struct _RotateContext;
	PROVIDE_GUID_INFO();
	ENABLE_MULTYINTERFACE();
	DECLARE_DYNCREATE(CChromosome);
	GUARD_DECLARE_OLECREATE(CChromosome);
protected:	//overrideable
	virtual void DoDraw(CDC &dc);
	virtual DWORD GetNamedObjectFlags();	//return the object flags
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );

	DECLARE_INTERFACE_MAP();

	void DoDrawAxisAndCentromere();

	CPoint m_ptOrig; // Origin for caryo, not rotate origin
	CPoint m_ptOffs;
	CChromoAxis m_ShortAxis;
	CChromoAxis m_LongAxis;
	BOOL m_bShortAxisManual,m_bLongAxisManual;
	CChromoAxis m_DrawAxis;
	COLORREF m_clrAxis;
	bool m_bDrawLong;
	CCentromere m_Cen;
	BOOL m_bCenManual;
	ROTATEPARAMS m_Rotate; // Angle, origin, etc
	bool m_bRotateVer4; // offsets in m_Rotate in DCS
	bool m_bCentromere5; // centromere in integer coordinates & no angle and position info
	void ConvertRotate(); // Convert m_Rotate from DCS int OCS
	void ConvertCentromere();
	int m_nManualClass;
	IImage3Ptr m_sptrImageOrg;
	BOOL m_bEqualizeFlag;
	IChromoConvertPtr m_sptrChromoConvert;
	CChromoCnvSeq m_KaryoSeq;
	void ResetSequences();
	void CalcOffsets();
public:
	CChromosome();
	~CChromosome();

	CChromoAxis &AxisByFlags(int nFlags);
	BOOL &IsManualAxis(int nFlags) {return nFlags&AT_ShortAxis?m_bShortAxisManual:m_bLongAxisManual;}
	CPoint Rot2Obj(CPoint pt);
	CPoint Obj2Rot(CPoint pt);
	void Rot2Obj(CChromoAxis &Axis);
	void Obj2Rot(CChromoAxis &Axis);
	HRESULT GetImage(IUnknown **ppunk, BOOL bOrig = TRUE);
	CPoint GetImgOffset();
	long OnGetBrightProfileSize(int nNum);
	void OnGetBrightProfile(int nNum, long *plProf);
	virtual IUnknown* Clone();
	bool IsMirrorV();
public:
	BEGIN_INTERFACE_PART(Chromos, IChromosome2)
		// IChromosome
		com_call GetAxisSize(int nAxis, long *plSize, BOOL *pbIsManual);
		com_call GetAxis(int nAxis, POINT *pAxisPoints);
		com_call SetAxis(int nAxis, long lSize, POINT* pAxisPoints, BOOL bIsManual);
		com_call GetCentromereCoord(BOOL  bRotated, POINT *pptCen, POINT *pptDraw, BOOL *pbIsManual);
		com_call SetCentromereCoord(BOOL  bRotated, POINT ptCen, POINT *ptDraw, BOOL bIsManual);
		com_call GetOriginalImage(IUnknown **ppunk);
		// If angle == 0, GetRotatedImage returns ordinary image and bOrigImage == TRUE.
		com_call GetRotatedImage(BOOL bBuild, IUnknown **ppunk, BOOL *pbOrigImage);
		com_call GetEqualizedImage(BOOL bBuild, IUnknown **ppunk);
		// If angle == 0, GetEqualizedRotatedImage returns equalized image and bOrigImage == TRUE.
		com_call GetEqualizedRotatedImage(BOOL bBuild, IUnknown **ppunk, BOOL *pbOrigImage);
		com_call SetOriginalImage(IUnknown *ppunk);
		com_call SetRotatedImage(IUnknown *pImg);
		com_call SetEqualizedImage(IUnknown *pImg);
		com_call SetEqualizedRotatedImage(IUnknown *pImg);
		com_call GetRotateParams(ROTATEPARAMS *pRotateParams);
		com_call SetRotateParams(ROTATEPARAMS *pRotateParams);
		com_call GetOrigin(POINT *pptOffs); // For caryo
		com_call SetOrigin(POINT ptOffs);
		com_call GetOffset(POINT *pptOffs);
		com_call SetOffset(POINT ptOffs);
		com_call GetBrightProfileSize(int iNum, long *plSize);
		com_call GetBrightProfile(int iNum, long *plBright);
		com_call SetColor(COLORREF color, int iNum);// for debug purposes
		com_call GetManualClass(int *pnClass);
		com_call SetManualClass(int nClass);
		com_call GetChromoConvert(IChromoConvert **);
		com_call SetChromoConvert(IChromoConvert *);
		com_call IsMirrorV(BOOL *pbMirror);
		// IChromosome2
		com_call GetAxis2(int nFlags /*ChromoAxisType*/, IChromoAxis2 *pAxis, BOOL *pbIsManual);
		com_call SetAxis2(int nFlags /*ChromoAxisType*/, IChromoAxis2 *pAxis, BOOL bIsManual);
		com_call GetCentromereCoord2(int nFlags/*ChromoAxisType*/, CENTROMERE *pCen, BOOL *pbIsManual);
		com_call SetCentromereCoord2(int nFlags/*ChromoAxisType*/, CENTROMERE *pCen, BOOL bIsManual);
		com_call GetKaryoSequence(IChromoConvert2 **ppSequence);
	END_INTERFACE_PART(Chromos)
	BEGIN_INTERFACE_PART(Clone, IClonableObject)
		com_call Clone(IUnknown** ppunkCloned);
	END_INTERFACE_PART(Clone);

public:
	static const char *c_szType;
};

#endif