#include "StdAfx.h"
#include <stdio.h>
#include <math.h>
#include "misc_utils.h"
#include "vcv.h"
#include "Profile.h"

/*
 * compute variance and covariances  and invert
 * (upper triangular, stored columnwise).
 * "self" MUST BE IN ASCENDING ORDER.
 */

void makevcv(int nself,int *self,int nclass,int nsamp,double clvcv[MAXCLASS][MAXDIM*(MAXDIM+1)/2],
	double clcp[MAXCLASS][MAXDIM][MAXDIM],int *clnsamp,double cp[MAXDIM][MAXDIM],double clm[MAXCLASS][MAXDIM],
	double m[MAXDIM],double vcv[MAXDIM*(MAXDIM+1)/2])
{
	register int i,j,k,_class;
	int g,h;
	k = 0;
	for (h=0; h<nself; h++) {
		i = self[h];
		for (g=0; g<=h; g++) {
			j = self[g];
			for (_class=0; _class<nclass; _class++)
				clvcv[_class][k] = (clcp[_class][j][i] - clnsamp[_class]*clm[_class][i]*clm[_class][j])/(clnsamp[_class]-1);	
			vcv[k++] = (cp[j][i] - nsamp*m[i]*m[j])/(nsamp-1);	
		}
	}
}


//void trainclass(int nself,int *self,int nclass,int ndim,double clvcv[MAXCLASS][MAXDIM*(MAXDIM+1)/2],
//	double clm[MAXCLASS][MAXDIM],double clsd[MAXCLASS][MAXDIM],double m[MAXDIM],double vcv[MAXDIM*(MAXDIM+1)/2],
//	double clsqrtdet[MAXDIM],double psd[MAXDIM],long *keys,FILE *rpt)
void _trainclass(int nself,int *self,int nclass,int ndim, double clm[MAXCLASS][MAXDIM],
	double clsd[MAXCLASS][MAXDIM],long *keys,FILE *rpt, LPCTSTR sz_ini, LPCSTR lpszSec)
{
	int h,i,_class;
	double logsqrtdet, ivar;
	// Calculate classinvvars
	double classinvvars[MAXCLASS][MAXDIM];
	double clinvvarsqrtdet[MAXCLASS];
	// inverted variances and log of square root of
	// determinant of variance diagonal matrix
	for (_class=0; _class < nclass; _class++)
	{
		logsqrtdet = 0.0;
		for (h=0; h<nself; h++)
		{
			i = self[h];
			ivar = clsd[_class][i];
			// prevent overflow
			if (ivar == 0.0)
				ivar = 1.0;
			logsqrtdet -= log(ivar);
			ivar *= ivar;
			ivar = 1.0/ivar;
			classinvvars[_class][h] = ivar;
		}
		clinvvarsqrtdet[_class] = logsqrtdet;
	}
	// Now save data in ini file
	char sz_buf[20];
	_itoa(nclass, sz_buf, 10);
	::WritePrivateProfileString(lpszSec, "ClassCount", sz_buf, sz_ini);
	_bstr_t bstrKeys;
	// Write keys string
	for (i = 0; i < nself; i++)
	{
		long key = keys[self[i]];
		ltoa(key,sz_buf,10);
		bstrKeys += sz_buf;
		if (i < nself-1) bstrKeys += ",";
	}
	::WritePrivateProfileString(lpszSec, "Keys", bstrKeys, sz_ini);
	// Write classes
	for (_class = 0; _class < nclass; _class++)
	{
		_itoa(_class+1, sz_buf, 10); // class numbers from 1
		_bstr_t bstrClassVal;
		char sz_buf1[250];
		sprintf(sz_buf1, "%f", clinvvarsqrtdet[_class]);
		bstrClassVal += sz_buf1;
		for (h = 0; h < nself; h++)
		{
			i = self[h];
			sprintf(sz_buf1, ", (%g,%g)", clm[_class][i], classinvvars[_class][h]);
			bstrClassVal += sz_buf1;
		}
		::WritePrivateProfileString(lpszSec, sz_buf, bstrClassVal, sz_ini);
	}
}

void trainclass(CSelFeatsFull &Self,int nclass,int ndim, double clm[MAXCLASS][MAXDIM],
	double clsd[MAXCLASS][MAXDIM],long *keys,const char *pszClassFile,FILE *rpt)
{
	// Make ini file name
	char sz_ini[MAX_PATH];
	const char *pszDestFile = pszClassFile?pszClassFile:sz_ini;
	if (!pszClassFile)
	{
		local_filename(sz_ini, "classes\\ClassifyInfo.ini", sizeof(sz_ini));
		strcpy( sz_ini, (const char*)::GetValueString(GetAppUnknown(), "\\Classes", CLASSIFYINFO, sz_ini ));
		pszClassFile = sz_ini;
	}
	_trainclass(Self.m_Sel.m_nSelFeats,Self.m_Sel.m_pnFeats,nclass,ndim,clm,
		clsd,keys,rpt,pszDestFile,CLASSIFYINFO);
	if (_GetClassifierInt("Classification", "UseDenverClasses", 0, pszClassFile))
	{
		for (int i = 0; i < 7; i++)
		{
			char szBuff[256],szBuff1[3];
			strcpy(szBuff,CLASSIFYINFO);
			szBuff1[0] = '_';
			szBuff1[1] = 'A'+i;
			szBuff1[2] = 0;
			strcat(szBuff,szBuff1);
			_trainclass(Self.m_SelDv[i].m_nSelFeats,Self.m_SelDv[i].m_pnFeats,nclass,ndim,clm,
				clsd,keys,rpt,pszDestFile,szBuff);
		}
	}
}
