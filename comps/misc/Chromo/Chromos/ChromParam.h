#if !defined(__ChromParam_H__)
#define __ChromParam_H__

#include "Chromosome.h"
#include "ChrUtils.h"
#include "Woolz.h"

class CAxis;
class CCentromere;
class CChromosome;
class CChromParam
{
public:
	IWoolzObjectPtr wo; // for debug purposes
	bool bOk;
	CImageWrp pImgWrp; // Working image (may be rotated)
	CImageWrp pImgWrpOrig; // Original image
	Contour *pCntr;
	CIntervalImage *pIntvl;
	CPoint m_ptOffs; // Offset of orginal image
//	BOOL m_bManualAngle; // If manual angle, angle will not be calculated
	bool bRotated;
	double dAngle;
	CPoint m_ptCenter; // Center of rotation
	// Drawable objects
	IChromosome2Ptr m_sptrChromos;
	// Axis data.
	double dLen; // Bulk object length on maximal dimension
	int nPtsDist; // Distance between points of "short" axis
	int nPoints; // Maximal number of  points of "short" axis
//	BOOL m_bManualShortAxis,m_bManualLongAxis;
	CChromoAxis m_ShortAxis; //"Short" axis with nodes (base points).
	CChromoAxis m_LongAxis;
	// Centromere.
//	BOOL m_bManualCentromere;
	int nCmerePos; // Position of centromere in "long" axis
	POINT ptCentromere; // Centromere point (in object coordinates).
	bool bPolarity;
	// Profiles
	// Piper - like parameters
	CChromoAxis m_Prof0;
	CChromoAxis m_Prof2;
	// Measure parameters
	//Size parameters
	double dArea; // (CalcParams1) Number of points
	double dDensity; // (CalcParams1)Average density. Same as gray-scale value of brightness of color point.
	double dLenght; // (CalcParams2) Lenght of central line in pixels
	double dPerim; // (CalcParams1) cvhp
	// "Size" parameter calculated only after normalization.
	// Global band pattern
	double cvdd;  // (CalcParams2)
	double nssd;  // (CalcParams2)
	double wdd1;  // (CalcParams3)
	double wdd2;  // (CalcParams2)
	double wdd2p; // (CalcParams3)
	double wdd3;  // (CalcParams3)
	double wdd4;  // (CalcParams2)
	double wdd5;  // (CalcParams3)
	double wdd6;  // (CalcParams2)
	double gwdd1; // (CalcParams3)
	double gwdd2; // (CalcParams2)
	double gwdd2p;// (CalcParams3)
	double gwdd3; // (CalcParams3)
	double gwdd4; // (CalcParams2)
	double gwdd5; // (CalcParams3)
	double gwdd6; // (CalcParams2)
	// Global "shape"
	double mwdd1; // (CalcParams3)
	double mwdd2; // (CalcParams2)
	double mwdd2p;// (CalcParams3)
	double mwdd3; // (CalcParams3)
	double mwdd4; // (CalcParams2)
	double mwdd5; // (CalcParams3)
	double mwdd6; // (CalcParams2)
	// Centromere index
	double dLenghtCI; // (CalcParams4)
	double dAreaCI;   // (CalcParams4)
	double dDensCI;   // (CalcParams4)
	double mdra;
	// Bands
	int nBands;
	double dNBIndex;

	CChromParam();
	~CChromParam();
	void CalcObjAngle(); // Require only object contour
	bool CalcShortAxis(); // Require only object contour and angle
	bool CalcLongAxis(); // Require "short" axis
	void CalcProfiles(); // Require "long" axis
	void CalcCentromere(); // Require profile
	void CalcParams1(); // Require only object contour
	void CalcParams2(); // Require "long" axis
	void CalcParams3(); // Require profiles
	void CalcParams4(); // Require centromere
	void GetParamVector(int *pn); // Get piper-like params
//	bool Init(CImageWrp &ImgWrp);
	bool Init(IMeasureObject *punkCalcObject, IChromosome *pChromos, IUnknown *punkImage = NULL);
};

#endif