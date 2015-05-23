/*****************************************************************************\
*    Program:             BxLib
*    Version:                                                                 *
*    Module:              BoShading.h
*    Compiler:            MS Visual C++ 6.0/7.0                               *
*    Operating System:    Windows 98/NT/2000/XP                               *
*    Purpose:                                                                 *
*                                                                             *
*    Notes:                                                                   *
*    Created:             16.10.2003                                          *
*    Last modified:       $Date: 7.11.03 16:55 $
*                         $Revision: 2 $
*                         $Archive: /mp/Bxlib/Bxlib2M/BoShading.h $
*    Author:              M. Pester                                       *
*                         Last $Author: Mp $
*    Copyright:           © Baumer Optronic GmbH, Radeberg                    *
\*****************************************************************************/
#ifndef _BOIMSHADING_H_
#define _BOIMSHADING_H_

/** 
\enum	eBORAWDATATYPE
\brief	eBORAWDATATYPE holds the information for raw data arrangements
*/
enum eBORAWDATATYPE {

	BOSHD_LINE1,		///< ident for a 1 line applied shading feature
	BOSHD_LINE2, 		///< ident for a 2 line applied shading feature
	BOSHD_MATRIX,		///< ident for a matrix applied shading feature 
						
};

/** 
\struct	tSHD
\brief	tSHD holds the information for the applicable shading feature and
        the relationship between io values and multipliers
        Multiplier = shdIo * (shdMuMax - shdMuMin ) / ( shdIoMax - shdIoMin )
*/
typedef struct {
    eBORAWDATATYPE shdType; ///< type of shading  
    int     shdSzHor;       ///< horizontal dimension 
    int     shdSzVer;       ///< vertical dimension      
    int     shdIoMin;       ///< minimal io value, refers to minimal multiplier 
    int     shdIoMax;       ///< maximal io value, refers to maximal multiplier 
    double  shdMuMin;       ///< minimal multiplier
    double  shdMuMax;       ///< maximal multiplier
} tSHD, * tpSHD;

/** 
\struct	tBoShdLineOrg1
\brief	tBoShdLineOrg1 descripes an shading feature that is applied line organized
      				   to raw datas, e.g. grayscale matrix, grayscale line
*/
typedef struct {
	int      iArraySize;	
	WORD *   pArray;	
} tBoShdLineOrg1;

/** 
\struct	tBoShdLineOrg2
\brief	tBoShdLineOrg2 descripes an shading feature that is applied to a pair of raw data lines
    				   , e.g. color matrix, color line
*/
typedef struct {
	int      iArraySize;	
	WORD *   pArrayA;	
	WORD *   pArrayB;	
} tBoShdLineOrg2;

/** 
\struct	tBoShdMatrixOrg
\brief	tBoShdMatrixOrg descripes an shading feature that is applied to the whole matrix of raw data
    				   , e.g. color matrix
*/
typedef struct {
	int      iArraySizeX;	
	int      iArraySizeY;	
	WORD *   pArray;	
} tBoShdMatrixOrg;


/** 
\struct	tShading
\brief	tShading descripes raw data arrangement for camera matrix
*/
typedef struct {
	eBORAWDATATYPE eDescId; 
	union {
		tBoShdLineOrg1	sLineOrg1;
		tBoShdLineOrg2	sLineOrg2;
		tBoShdMatrixOrg	sMatrixOrg;
	} uDesc;
} tShading, *tpShading; 


#endif
