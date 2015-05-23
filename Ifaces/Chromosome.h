#if !defined(__Chromosome_H__)
#define __Chromosome_H__

#include "defs.h"
#include "Object5.h"

// The values from enum will be used in Get/SetAxis and GetSetCentromereCoord. Different values
// can be OR'ed
enum ChromoAxisType
{
	AT_ShortAxis = 1,          // Short axis is set. Not used for centromere
	AT_LongAxis = 2,           // Long axis is set. Ignored if AT_ShortAxis. Not used for centromere
	AT_Centromere = 4,         // Used only during draw operation, ignored in GetCenromereCoord
	AT_AxisDouble = 0x1000000, // pAxisPoints will be pointer to array of double points, not integer points
	AT_Draw = 0x2000000,
	AT_Original = 0x4000000,   // Get original axis, centromere, etc., do not query convertor object
	AT_VisAngle = 0x8000000,   // Chromosome rotated not into vertical position but into some angle with
	                           // vertical position.
	AT_Mirror = 0x10000000,    // If set axis and centromere will be returned mirrored in GetAxis and
	                           // GetCentromereCoord if KEY_POLARITY = 0.
	AT_Rotated = 0x20000000,   // Rotated axis or centromere
	AT_NotRecalc = 0x40000000, // Object usualy recalculates rotated values if not-rotated are set and v.v.
	                           // This flag supresses this recalculation.
	AT_Reset = 0x80000000,     // Empty all values
};

// All points in object coordinate system, except offsets and ptCenter. Non-rotated axises and
// centromere will be represented in non-rotated object coordinate system (begin of non-rotated
// object == (0,0)). Rotated axises and centromere will be in rotated object coordinate system
// (begin of rotated object == (0,0)). If ptOffsR == (-10000,-10000) in SetRotateParams, it will
// be calculated automatically.
struct ROTATEPARAMS
{
	double dAngle; // vertical == 0
	POINT ptCenter; // rotational center of the object in document coordinates
	POINT ptOffs; // offset of original object in document coordinates
	POINT ptOffsR; // offset of rotated object in document coordinates
	BOOL bManual; // Can be set TRUE in SetRotateParams. chromos.dll wouldn't search angle
	              // in this case.
	BOOL bMirrorV; // If bMirror?==TRUE, images, axes and centromeres will mirrored automaticaly
	BOOL bMirrorH; // inside BuildRotatedImage and DoDraw
	double dAngleVisible;
};

struct ROTATEPARAMS2
{
	DWORD dwSize;  // size of structure
	double dAngle; // vertical == 0
	POINT ptCenter; // rotational center of the object in document coordinates
	POINT ptOffs; // offset of original object in document coordinates
	POINT ptOffsR; // offset of rotated object in document coordinates
	BOOL bManual; // Can be set TRUE in SetRotateParams. chromos.dll wouldn't search angle
	              // in this case.
	BOOL bMirrorV; // If bMirror?==TRUE, images, axes and centromeres will mirrored automaticaly
	BOOL bMirrorH; // inside BuildRotatedImage and DoDraw
	double dAngleVisible;
	POINT ptOffsRM; // offset of rotated and mirrored object in document coordinates
};

struct FPOINT
{
   double x;
   double y;
};

struct CENTROMERE
{
	POINT  ptOffset;       // If ptCen and aptCenDraw in OCS ptOffset will be offset of object's
	FPOINT ptCen;          // image. If they are in DCS, ptOffset will be (0,0)
	FPOINT aptCenDraw[2];
	double dAngle;         // Angle of axis in point of centromere
	int nCentromere;       // Position on long axis
};

enum AxisType
{
	NoData         = 0,
	IntegerAxis    = 1,
	FloatAxis      = 2,
	IntegerProfile = 3,
};

struct CHROMOAXIS
{
	AxisType m_Type;
	int m_nScale;
	unsigned m_nSize;
	POINT m_ptOffset;
	union
	{
		POINT  *m_iptAxis;
		FPOINT *m_fptAxis;
		int    *m_pnProf;
	};
};

interface IChromoAxis2 : public IUnknown
{
	virtual void __stdcall Init(AxisType Type, unsigned long nSize, void *pData = NULL) = 0;
	virtual void __stdcall Init(IChromoAxis2 *pCopyFrom) = 0;
	virtual AxisType __stdcall GetType() = 0;
	virtual long __stdcall GetSize() = 0;
	virtual void *__stdcall GetData() = 0;
	virtual FPOINT __stdcall GetOffset() = 0;
	virtual void __stdcall SetOffset(FPOINT pt) = 0;
	virtual int __stdcall GetScale() = 0;
	virtual void __stdcall SetScale(int dScale) = 0;
	virtual void __stdcall Offset(FPOINT pt) = 0; // Offsets data poits and changes ptOffset
	virtual void __stdcall Rotate(FPOINT fptOrg, double dAngle) = 0;
	virtual void __stdcall MirrorX(double dxMirror) = 0;
	virtual void __stdcall MirrorY(double dyMirror) = 0;
	virtual void __stdcall Smooth(int nIterations, bool bScaled) = 0;
	virtual void __stdcall Convert(AxisType NewType) = 0;
	virtual void __stdcall Shorten(int nBeg, int nEnd) = 0;
};

interface IChromosome;
interface IChromosome2;
interface IChromoConvert : public IUnknown
{
	// During draw operation
	com_call CCGetAxisSize(IChromosome *pChromo, int nFlags /*ChromoAxisType*/, long *plSize, BOOL *pbIsManual) = 0;
	com_call CCGetAxis(IChromosome *pChromo, int nFlags /*ChromoAxisType*/, POINT *pAxisPoints) = 0;
	com_call CCGetCentromereCoord(IChromosome *pChromo, int nFlags/*ChromoAxisType*/, POINT *pptCen, POINT *pptDraw, BOOL *pbIsManual) = 0; // pptDraw can be NULL
	// During IChromosome::GetRotatedImage 
	com_call CCGetImage(IChromosome *pChromo, IUnknown **ppunk) = 0;
	com_call Recalc(IChromosome *pChromo) = 0;
	com_call Invalidate() = 0;
};

// this interface is supported by both individual convertors and convertor sequences
interface IChromoConvert2 : public IUnknown
{
	// During draw operation
	com_call CCGetAxis(IChromosome2 *pChromo, DWORD dwFlags, IChromoAxis2 **ppAxis, BOOL *pbIsManual) = 0;
	com_call CCGetCentromereCoord(IChromosome2 *pChromo, CENTROMERE *pCen, BOOL *pbIsManual) = 0;
	// During IChromosome::GetRotatedImage 
	com_call CCGetImage(IChromosome2 *pChromo, IUnknown **ppunk) = 0;
	com_call Recalc(IChromosome2 *pChromo) = 0;
	com_call Init(IChromoConvert2 *pCCPrev, LPCSTR lpszOpts) = 0; // For sequence and first convertor pCCPrev will be NULL
	com_call GetInitString(BSTR *pbstrInitStr) = 0;
};

#define pszErectCCProgID "chrom_aam.ErectCC"

interface IChromosome : public IUnknown
{
	com_call GetAxisSize(int nFlags /*ChromoAxisType*/, long *plSize, BOOL *pbIsManual) = 0;
	com_call GetAxis(int nFlags /*ChromoAxisType*/, POINT *pAxisPoints) = 0;
	com_call SetAxis(int nFlags /*ChromoAxisType*/, long lSize, POINT *pAxisPoints, BOOL bIsManual) = 0;
	com_call GetCentromereCoord(int nFlags/*ChromoAxisType*/, POINT *pptCen, POINT *pptDraw, BOOL *pbIsManual) = 0; // pptDraw can be NULL
	com_call SetCentromereCoord(int nFlags/*ChromoAxisType*/, POINT ptCen, POINT *ptDraw, BOOL bIsManual) = 0; // pptDraw can be NULL
	// Original image may differs from parent->getImage(..) e.g after equalization. 
	com_call GetOriginalImage(IUnknown **ppunk) = 0;
	// These functions returns image with some angle i.e. not necessary vertical image.
	// If angle == 0, GetRotatedImage returns ordinary image and bOrigImage == TRUE.
	com_call GetRotatedImage(int nFlags, IUnknown **ppunk, BOOL *pbOrigImage) = 0;
	com_call GetEqualizedImage(int nFlags, IUnknown **ppunk) = 0;
	// If angle == 0, GetEqualizedRotatedImage returns equalized image and bOrigImage == TRUE.
	com_call GetEqualizedRotatedImage(int nFlags, IUnknown **ppunk, BOOL *pbOrigImage) = 0;
	com_call SetOriginalImage(IUnknown *ppunk) = 0;
	com_call SetRotatedImage(IUnknown *pImg) = 0;
	com_call SetEqualizedImage(IUnknown *pImg) = 0;
	com_call SetEqualizedRotatedImage(IUnknown *pImg) = 0;
	com_call GetRotateParams(ROTATEPARAMS *pRotateParams) = 0;
	com_call SetRotateParams(ROTATEPARAMS *pRotateParams) = 0;
	com_call GetOrigin(POINT *pptOffs) = 0; // For caryo. Origin of chromosome in caryogram.
	com_call SetOrigin(POINT ptOffs) = 0;   // (0,0) by default.
	com_call GetOffset(POINT *pptOffs) = 0; // Same ar ROTATEPARAMS.ptOffs
	com_call SetOffset(POINT ptOffs) = 0;
	com_call GetBrightProfileSize(int iNum, long *plSize) = 0;
	com_call GetBrightProfile(int iNum, long *plBright) = 0;
	com_call SetColor(COLORREF color, int iNum) = 0;// for debug purposes. Sets color of axis line
	com_call GetManualClass(int *pnClass) = 0; // nClass can be == 1 (object was manually classified)
	com_call SetManualClass(int nClass) = 0;   // or 0 (object was left for auto classification).
	com_call GetChromoConvert(IChromoConvert **) = 0;
	com_call SetChromoConvert(IChromoConvert *) = 0;
	com_call IsMirrorV(BOOL *pbMirror) = 0;
};

interface IChromosome2 : public IChromosome
{
	com_call GetAxis2(int nFlags /*ChromoAxisType*/, IChromoAxis2 *pAxis, BOOL *pbIsManual) = 0;
	com_call SetAxis2(int nFlags /*ChromoAxisType*/, IChromoAxis2 *pAxis, BOOL bIsManual) = 0;
	com_call GetCentromereCoord2(int nFlags/*ChromoAxisType*/, CENTROMERE *pCen, BOOL *pbIsManual) = 0; // pptDraw can be NULL
	// if pCen->aptCenDraw are INVFPOINT it will be calculated
	com_call SetCentromereCoord2(int nFlags/*ChromoAxisType*/, CENTROMERE *pCen, BOOL bIsManual) = 0; // pptDraw can be NULL
	com_call GetKaryoSequence(IChromoConvert2 **ppSequence) = 0;
};

declare_interface(IChromosome, "788CB801-F85E-11D5-BA8D-0002B38ACE2C")
declare_interface(IChromosome2, "74CF3686-0D98-4F9C-843E-E52B51320B2A")
declare_interface(IChromoConvert, "38E88FE8-83D8-4dbd-AA16-0440DFDFDC48")
declare_interface(IChromoConvert2, "6B7BB6AC-7079-45E2-9502-D6B12E8CB8ED")
declare_interface(IChromoAxis2, "BE987464-BE8E-428D-8736-7121B1260138")

#endif
