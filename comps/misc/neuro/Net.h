#pragma once

#include "Matrix.h"
#include "Sigmoid.h"
#include "IniFile.h"

#define MAXLAYERS 10
class CNet
{
public:
	CNet(int n0=0, int n1=0, int n2=0, int n3=0, int n4=0, int n5=0, int n6=0, int n7=0, int n8=0);
	~CNet(void);
	CVector* Process(CVector &in); //возвращает указатель на выходные нейроны сети
	bool RandomFill();
	bool Learn(CVector *pin, CVector *pout, int nCount, double fSpeed, int nEpoch=100);
	bool Test(CVector *pin, CVector *pout, int nCount, double *perr, double *pcerr);
	bool Save(char *filename);
	bool Load(char *filename);
	bool SaveIni(CIniFile *pIni);
	bool LoadIni(CIniFile *pIni);
	bool Create(int nNeurons[MAXLAYERS]);
	int m_nLayers;
	CMatrix m_W[MAXLAYERS];
	CVector m_Y[MAXLAYERS];
	CVector m_S[MAXLAYERS];
	CVector m_E[MAXLAYERS];
	CVector m_Delta[MAXLAYERS];
	int m_nEpoch;
	double m_fErr;
};
