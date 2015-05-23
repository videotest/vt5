#pragma once

struct CDenverClasses
{
	static int m_anDenverClasses[24];
	int DenverClass(int nClass) {return m_anDenverClasses[nClass];}
	int ClassByIntclass(int nDenverClass, int nIntClass)
	{
		for (int i = 0, k = 0; i <24; i++)
		{
			if (m_anDenverClasses[i] == nDenverClass)
			{
				if (k++ == nIntClass)
					return i;
			}
		}
		return -1;
	}
};


class mahalanobis
{
public:
	int nclass;
	int nsfv;
	_ptr_t2<long> sfv;
	_ptr_t2<double> classmeans[MAXCLASS];
	_ptr_t2<double> classinvvars[MAXCLASS];
	double clinvvarsqrtdet[MAXCLASS];
	int cutoff;

	void read_class_params(FILE *rpt, int nDenverClass = -1);
	int classdist(double *svec, double *lpriorp, cllik *cl);
	double distvar(double *svec, double *means, double *ivars, int nsfv);
};

void rearrange(chromcl *chrcl,classcont *clcontents,int numobj,int *clwn,int nclass, int cutoff);
void _parse_class_string(char *pszParse, int nPairs, double &d, _ptr_t2<double> &ad1, _ptr_t2<double> &ad2);
void _parse_long_list(const char *pstrParse, _ptr_t2<long> &FeatBuff, int &nFeatures);
void _getself(ICalcObject *ptrObj, int nsfv, long *sfv, double *svec);
int liksort(cllik *a, cllik *b);

extern int clwn[MAXCLASS];
extern double lpriorp[MAXCLASS];
