#include "StdAfx.h"
#include "WoolzObject.h"
#include "measure5.h"
#include "image5.h"
#include "ParamsKeys.h"
#include "Chromosome.h"

#define COORD ICOORD
#define __NOTEXT
extern "C"
{
#include "wstruct.h"
#define class _class
#include "chromanal.h"
#undef class
void freeobj(struct object *);
int getdim(int);
void getvec(register struct chromplist *plist, register int *v);
void normvec(struct chromosome **objs,register int number,struct normaliser *vn);
void normivec(register int *fvec,register int *ivec,int *sfv,register struct normaliser *vn,register int ndim,register int nsfv);
#include "DebugObj.h"
#if defined(_DEBUG)
struct chromosome *_chromfeatures(struct chromosome *obj, int basic, struct object **pmpol,int *skel,struct debugobj *pdebug);
#else
struct chromosome *_chromfeatures(struct chromosome *obj, int basic, struct object **pmpol,int *skel);
#endif
}
#define BASIC 1
#define RADIANS 57.296

typedef struct chromosome Chromosome;

void *woolz_object_by_vt5_object(IUnknown *punkVT5Obj);
void *woolz_object_by_vt5_object(IUnknown *punkVT5Obj)
{
	IWoolzObject *pwo = NULL;
	sptrINamedDataObject2 sptrNObj(punkVT5Obj);
	long posObj = 0;
	sptrNObj->GetFirstChildPosition((long*)&posObj);
	while (posObj)
	{
		IUnknown *punkCh = 0;
		sptrNObj->GetNextChild((long*)&posObj, &punkCh);
		if (punkCh)
		{
			IWoolzObject *pwo1 = NULL;
			punkCh->QueryInterface(IID_IWoolzObject, (void **)&pwo1);
			punkCh->Release();
			if (pwo1)
			{
				pwo = pwo1;
				break;
			}
		}
	}
	void *pobj = NULL;
	if (pwo)
	{
		pwo->GetWoolzObject(&pobj);
		pwo->Release();
	}
	return pobj;
}


IWoolzObjectPtr woolz_obj_by_VT5_obj(IUnknownPtr punkVT5Obj)
{
	sptrINamedDataObject2 sptrNObj(punkVT5Obj);
	long posObj = 0;
	sptrNObj->GetFirstChildPosition((long*)&posObj);
	while (posObj)
	{
		IUnknownPtr punkCh;
		sptrNObj->GetNextChild((long*)&posObj, &punkCh);
		bool bCh = punkCh;
		if (bCh)
		{
			IWoolzObjectPtr pwo;
			punkCh.QueryInterface(IID_IWoolzObject, &pwo);
			bool bWO = pwo;
			if (bWO)
			{
				return pwo;
			}
		}
	}
	return 0;
}


#if defined(_DEBUG)
static debugobj *InitDebugObject()
{
	debugobj *p = new debugobj;
	memset(p,0,sizeof(*p));
	return p;
}

static void FreeDebugObject(debugobj *p)
{
	if (!p) return;
	if (p->shortaxis) free(p->shortaxis);
	if (p->smoothshortaxis) free(p->smoothshortaxis);
	if (p->longaxis) free(p->longaxis);
	if (p->prof0) free(p->prof0);
	if (p->prof1) free(p->prof1);
	if (p->sprof0) free(p->sprof0);
	if (p->sprof1) free(p->sprof1);
	if (p->nvec) free(p->nvec);
	delete p;
}
#endif

CWoolzObject::CWoolzObject()
{
	obj = NULL;
	mpol1 = NULL;
	punkParent = NULL;
#if defined(_DEBUG)
	oldobj = NULL;
	pdebug = NULL;
#endif
}

CWoolzObject::~CWoolzObject()
{
	if (obj)
		freeobj((struct object *)obj);
	if (mpol1)
		freeobj((struct object *)mpol1);
#if defined(_DEBUG)
	if (oldobj)
		freeobj((struct object *)oldobj);
	FreeDebugObject(pdebug);
#endif
}


HRESULT CWoolzObject::Load( IStream *pStream, SerializeParams *pparams )
{
	return S_OK;
}

HRESULT CWoolzObject::Store( IStream *pStream, SerializeParams *pparams )
{
	return S_OK;
}

HRESULT CWoolzObject::IsBaseObject(BOOL * pbFlag)
{
	*pbFlag = false;
	return S_OK;
}

HRESULT CWoolzObject::GetParent( IUnknown **ppunkParent )
{
	*ppunkParent = punkParent;
	return S_OK;
}

HRESULT CWoolzObject::SetParent( IUnknown *punkParent, DWORD dwFlags /*AttachParentFlag*/ )
{
	if (this->punkParent)
	{
		INamedDataObject2Ptr sptrNDOParent(this->punkParent);
		sptrNDOParent->RemoveChild(Unknown());
	}
	this->punkParent = punkParent;
	if (this->punkParent)
	{
		INamedDataObject2Ptr sptrNDOParent(this->punkParent);
		sptrNDOParent->AddChild(Unknown());
	}
	return S_OK;
}

HRESULT CWoolzObject::GetWoolzObject(void **pobj)
{
	*pobj = obj;
	return S_OK;
}

HRESULT CWoolzObject::SetWoolzObject(void *o)
{
	obj = o;
	return S_OK;
}

HRESULT CWoolzObject::CalculateParameters(IUnknown *punkCO, int nParamsNum, double *pdParams, IChromosome *pChromos)
{
	if (!obj) return S_FALSE;
	if (mpol1) // Saved from previous call
		freeobj((struct object *)mpol1);
	// Using function _chromfeatures to calculate parameters
	struct object *mpol;
	int skel;
#if defined(_DEBUG)
	pdebug = InitDebugObject();
	struct chromosome *robj = _chromfeatures((struct chromosome *)obj, BASIC, &mpol, &skel, pdebug);
	pdebug->skel = skel;
#else
	struct chromosome *robj = _chromfeatures((struct chromosome *)obj, BASIC, &mpol, &skel);
#endif
	if (robj != (struct chromosome *)obj)
	{
		((struct chromosome *)obj)->plist = 0;
#if defined(_DEBUG)
		if (oldobj == NULL)
			oldobj = obj;
		else
#endif
			freeobj((struct object *)obj);
	}
	obj = robj;
	// Obtain object's offset
	IMeasureObjectPtr sptrM = punkCO;
	IUnknownPtr punkImage; 
	sptrM->GetImage(&punkImage);
	IImage3Ptr Img(punkImage);
	POINT ptOffs;
	Img->GetOffset(&ptOffs);
	pChromos->SetOffset(ptOffs);
//	ptOffs.y -= ((struct chromosome *)obj)->idom->line1;
//	ptOffs.x -= ((struct chromosome *)obj)->idom->kol1;
	// Initialize axis and centromere.
	ROTATEPARAMS Rotate;
	Rotate.dAngle = robj->plist->rangle/RADIANS;
	Rotate.ptCenter.x = ptOffs.x + (((struct chromosome *)obj)->idom->lastkl - ((struct chromosome *)obj)->idom->kol1)/2;
	Rotate.ptCenter.y = ptOffs.y + (((struct chromosome *)obj)->idom->lastln - ((struct chromosome *)obj)->idom->line1)/2;
	Rotate.ptOffs = ptOffs;
	Rotate.ptOffsR = ptOffs;
	struct polygondomain *pdom = (struct polygondomain *)mpol->idom;
	int nVert = pdom->nvertices;
	_ptr_t2<POINT> pptVerts(nVert);
	for (int i = 0; i < nVert; i++)
	{
		pptVerts[i].x = (pdom->vtx[i].vtX+4)/8-((struct chromosome *)obj)->idom->kol1;
		pptVerts[i].y = (pdom->vtx[i].vtY+4)/8-((struct chromosome *)obj)->idom->line1;
	}
	pChromos->SetAxis(AT_ShortAxis|AT_Rotated, nVert, pptVerts, FALSE);
	if (skel)
		pChromos->SetColor(RGB(0, 0, 128), 0);
	else
		pChromos->SetColor(RGB(0, 255, 128), 0);
	mpol1 = mpol; // Will be freed on destructor or next call
	POINT ptCen;
	ptCen.x = robj->plist->cx-((struct chromosome *)obj)->idom->kol1;
	ptCen.y = robj->plist->cy-((struct chromosome *)obj)->idom->line1;
	pChromos->SetCentromereCoord(AT_Rotated, ptCen, NULL, FALSE);
	// Initialize parameters.
	ICalcObjectPtr sptrCO = punkCO;
	sptrCO->SetValue(KEY_NEW_AREA,robj->plist->area);
	sptrCO->SetValue(KEY_DENSITY,robj->plist->mass/robj->plist->area);
	sptrCO->SetValue(KEY_NEW_PERIMETR,robj->plist->hullperim);
	sptrCO->SetValue(KEY_LENGHT,robj->plist->length);
	sptrCO->SetValue(KEY_WDD1,robj->plist->wdd[0]);
	sptrCO->SetValue(KEY_WDD2,robj->plist->wdd[1]);
	sptrCO->SetValue(KEY_WDD3,robj->plist->wdd[2]);
	sptrCO->SetValue(KEY_WDD4,robj->plist->wdd[3]);
	sptrCO->SetValue(KEY_WDD2P,0.);
	sptrCO->SetValue(KEY_WDD5,robj->plist->wdd[4]);
	sptrCO->SetValue(KEY_WDD6,robj->plist->wdd[5]);
	sptrCO->SetValue(KEY_MWDD1,robj->plist->mwdd[0]);
	sptrCO->SetValue(KEY_MWDD2,robj->plist->mwdd[1]);
	sptrCO->SetValue(KEY_MWDD3,robj->plist->mwdd[2]);
	sptrCO->SetValue(KEY_MWDD4,robj->plist->mwdd[3]);
	sptrCO->SetValue(KEY_MWDD2P,0.);
	sptrCO->SetValue(KEY_MWDD5,robj->plist->mwdd[4]);
	sptrCO->SetValue(KEY_MWDD6,robj->plist->mwdd[5]);
	sptrCO->SetValue(KEY_GWDD1,robj->plist->gwdd[0]);
	sptrCO->SetValue(KEY_GWDD2,robj->plist->gwdd[1]);
	sptrCO->SetValue(KEY_GWDD3,robj->plist->gwdd[2]);
	sptrCO->SetValue(KEY_GWDD4,robj->plist->gwdd[3]);
	sptrCO->SetValue(KEY_GWDD2P,0.);
	sptrCO->SetValue(KEY_GWDD5,robj->plist->gwdd[4]);
	sptrCO->SetValue(KEY_GWDD6,robj->plist->gwdd[5]);
	sptrCO->SetValue(KEY_LENCI,robj->plist->cindexl);
	sptrCO->SetValue(KEY_AREACI,robj->plist->cindexa);
	sptrCO->SetValue(KEY_DENSCI,robj->plist->cindexm);
	sptrCO->SetValue(KEY_CVDD,robj->plist->cvdd);
	sptrCO->SetValue(KEY_NSSD,robj->plist->nssd);
	sptrCO->SetValue(KEY_MDRA,robj->plist->mdra);
	return S_OK;
}

HRESULT CWoolzObject::NormalizeParameters(IUnknown *punkObjectsList)
{
	int ivec[MAXDIM];
	int fvec[MAXDIM];
	int sfv[MAXDIM];
	int ndim = getdim(0);
	struct normaliser vn[MAXDIM];
	int iObj = 0;
	for (int i=0; i<MAXDIM; i++)
		sfv[i] = i;
	// Allocate sufficient array of data
	sptrINamedDataObject2 sptrN(punkObjectsList);
	long lObjects;
	sptrN->GetChildsCount(&lObjects);
	_ptr_t2<Chromosome*> arrObjects(lObjects);
	_ptr_t2<IUnknown *> arrVT5Objs(lObjects);
	// Iterate all objects and find Woolz object data
	long pos = 0;
	sptrN->GetFirstChildPosition((long*)&pos);
	while (pos)
	{
		IUnknown	*punk = 0;
		sptrN->GetNextChild( (long*)&pos, &punk );
		if (punk)
		{
			ICalcObjectPtr sptrObj(punk);
			// If there is woolz object in VT5 object, find and use it.
			void *pobj = woolz_object_by_vt5_object(punk);
			if (pobj)
			{
				((Chromosome **)arrObjects)[iObj] = (Chromosome *)pobj;
				((IUnknown **)arrVT5Objs)[iObj] = punk;
				iObj++;
			}
			else
				punk->Release();
		}
	}
	// Now normalize object data and save it
	normvec((Chromosome **)arrObjects,iObj,vn);
	for (i = 0; i < iObj; i++)
	{
		Chromosome *obj = ((Chromosome **)arrObjects)[i];
		IUnknown *punk = ((IUnknown **)arrVT5Objs)[i];
		if (obj->plist->otype <= 1 /*&&
			(derived != 0 || obj->plist->history[0] == 0)*/ )
		{
			// feature vector
			getvec(obj->plist,ivec);
			// compute and write normalised feature vector
			normivec(ivec,fvec,sfv,vn,ndim,ndim);
			// Save object's parameters
			ICalcObjectPtr sptrCO(punk);
			sptrCO->SetValue(KEY_NORMA_AREA,fvec[0]);
			sptrCO->SetValue(KEY_NORMA_DENSITY,fvec[2]);
			sptrCO->SetValue(KEY_NORMA_PERIMETR,fvec[27]);
			sptrCO->SetValue(KEY_NORMA_LENGHT,fvec[1]);
			sptrCO->SetValue(KEY_NORMA_WDD1,fvec[8]);
			sptrCO->SetValue(KEY_NORMA_WDD2,fvec[9]);
			sptrCO->SetValue(KEY_NORMA_WDD3,fvec[10]);
			sptrCO->SetValue(KEY_NORMA_WDD4,fvec[11]);
			sptrCO->SetValue(KEY_NORMA_WDD2P,0.);
			sptrCO->SetValue(KEY_NORMA_WDD5,fvec[12]);
			sptrCO->SetValue(KEY_NORMA_WDD6,fvec[13]);
			sptrCO->SetValue(KEY_NORMA_MWDD1,fvec[14]);
			sptrCO->SetValue(KEY_NORMA_MWDD2,fvec[15]);
			sptrCO->SetValue(KEY_NORMA_MWDD3,fvec[16]);
			sptrCO->SetValue(KEY_NORMA_MWDD4,fvec[17]);
			sptrCO->SetValue(KEY_NORMA_MWDD2P,0.);
			sptrCO->SetValue(KEY_NORMA_MWDD5,fvec[18]);
			sptrCO->SetValue(KEY_NORMA_MWDD6,fvec[19]);
			sptrCO->SetValue(KEY_NORMA_GWDD1,fvec[20]);
			sptrCO->SetValue(KEY_NORMA_GWDD2,fvec[21]);
			sptrCO->SetValue(KEY_NORMA_GWDD3,fvec[22]);
			sptrCO->SetValue(KEY_NORMA_GWDD4,fvec[23]);
			sptrCO->SetValue(KEY_NORMA_GWDD2P,0.);
			sptrCO->SetValue(KEY_NORMA_GWDD5,fvec[24]);
			sptrCO->SetValue(KEY_NORMA_GWDD6,fvec[25]);
			sptrCO->SetValue(KEY_NORMA_LENCI,fvec[26]);
			sptrCO->SetValue(KEY_NORMA_AREACI,fvec[3]);
			sptrCO->SetValue(KEY_NORMA_DENSCI,fvec[4]);
			sptrCO->SetValue(KEY_NORMA_CVDD,fvec[5]);
			sptrCO->SetValue(KEY_NORMA_NSSD,fvec[6]);
			sptrCO->SetValue(KEY_NORMA_MDRA,fvec[7]);
#if defined(_DEBUG)
			IWoolzObjectPtr wo = woolz_obj_by_VT5_obj(punk);
			debugobj *pdeb;
			wo->GetDebugData((void **)&pdeb);
			if (pdeb)
			{
				pdeb->nvec = (int *)malloc(MAXDIM*sizeof(int));
				memcpy(pdeb->nvec, fvec, MAXDIM*sizeof(int));
			}
#endif
		}
		punk->Release();
	}
	return S_OK;
}

#if defined(_DEBUG)
void dprintf(char * szFormat, ...)
{
    char buf[256];
    va_list va;
    va_start(va, szFormat);
    vsprintf(buf, szFormat, va);
    va_end(va);
	OutputDebugString(buf);
	static BOOL bMsgBox = TRUE;
	if (bMsgBox)
	{
		MessageBox(NULL, "Error in chromo param calc, see debug.", "Debug", MB_OK|MB_ICONEXCLAMATION);
		bMsgBox = FALSE;
	}
}


HRESULT _CompareObject(debugobj *pdebug, struct object *obj1, INTERVALIMAGE *intvls, char *pszDescr)
{
//		intvls = (INTERVALIMAGE *)pData;
	int i,j;
	if (intvls->nrows != obj1->idom->lastln-obj1->idom->line1+1)
	{
		dprintf("Y size of %s differs : P %d, VT5 %d\n", pszDescr, obj1->idom->lastln-obj1->idom->line1+1,
			intvls->nrows);
		return S_FALSE;
	}
	int x0p = obj1->idom->kol1;
	int x0v = intvls->ptOffset.x;
	for (i = 0; i < intvls->nrows; i++)
	{
		if (intvls->prows[i].nints != obj1->idom->intvlines[i].nintvs)
		{
			dprintf("Intervals count of %s differs on line %d: P %d, VT5 %d\n", pszDescr, i,
				obj1->idom->lastln-obj1->idom->line1+1, intvls->prows[i].nints);
			return S_FALSE;
		}
		for (j = 0; j < intvls->prows[i].nints; j++)
		{
			if (intvls->prows[i].pints[j].left+x0v != obj1->idom->intvlines[i].intvs[j].ileft+x0p ||
				intvls->prows[i].pints[j].right+x0v != obj1->idom->intvlines[i].intvs[j].iright+x0p)
			{
				dprintf("%s - interval %d differs on line %d : P (%d .. %d), VT5 (%d .. %d)\n", pszDescr, j, i,
					obj1->idom->intvlines[i].intvs[j].ileft, obj1->idom->intvlines[i].intvs[j].iright,
					intvls->prows[i].pints[j].left, intvls->prows[i].pints[j].right);
				return S_FALSE;
			}
		}
	}
	return S_OK;
}

HRESULT _CompareAxis(debugobj *pdebug, int nData, int nSize1, void *pData, object *obj1, polygondomain *pdom, const char *pszDescr, int nDiv)
{
	/*if (pdebug->skel)
	{
		dprintf("Skeletized image\n");
		return S_FALSE;
	}*/
	CHROMOAXIS *pca = (CHROMOAXIS *)pData;
	int nSize = pca->m_nSize;
	if (nSize != pdom->nvertices)
	{
		dprintf("different sizes of %s, P: %d, VT5: %d\n", pszDescr, pdebug->shortaxis->nvertices, nSize);
		return S_FALSE;
	}
	if (pca->m_Type != IntegerAxis)
	{
		dprintf("invalid type of %s", pszDescr);
		return S_FALSE;
	}
	POINT *ppt = pca->m_iptAxis;
	for (int i = 0; i < nSize; i++)
	{
		if (ppt[i].x+pca->m_ptOffset.x != pdom->vtx[i].vtX/nDiv ||
			ppt[i].y+pca->m_ptOffset.y != pdom->vtx[i].vtY/nDiv)
		{
			dprintf("Different %s (point %d) - P : (%d,%d), VT5 : (%d,%d)\n", pszDescr, i,
				pdom->vtx[i].vtX/nDiv, pdom->vtx[i].vtY/nDiv, ppt[i].x+pca->m_ptOffset.x,
				ppt[i].y+pca->m_ptOffset.y);
			return S_FALSE;
		}
	}
	return S_OK;
}

HRESULT _CompareProfile(int nSize, int *pProf, histogramdomain *hdom, const char *pszDescr, int nShift)
{
	if (hdom->npoints != nSize)
	{
		dprintf("%s size differs, P : %d, VT5 : %d\n", pszDescr, hdom->npoints, nSize);
		return S_FALSE;
	}
	for (int i = 0; i < nSize; i++)
	{
		int nP = hdom->hv[i];
		if (hdom->hv[i] != (pProf[i]>>nShift))
		{
			dprintf("%s differs, point %d, P : %d (%d), VT5 %d\n", pszDescr, i, hdom->hv[i], (hdom->hv[i]<<nShift),
				pProf[i]);
			return S_FALSE;
		}
	}
	return S_OK;
}

HRESULT _CompareFeatures(int nSize, int *anFeatsP, int *pnFeatsVT5, const char *pszDescr)
{
	for (int i = 0; i < nSize; i++)
	{
		if (anFeatsP[i] != pnFeatsVT5[i])
		{
			dprintf("%s %d differs, P - %d, VT5 - %d\n", pszDescr, i, anFeatsP[i], pnFeatsVT5[i]);
			return S_FALSE;
		}
	}
	return S_OK;
}

HRESULT _CompareFeatures1(int nSize, int *anFeatsP, int *pnFeatsVT5, const char *pszDescr)
{
	for (int i = 0; i < nSize; i++)
	{
		int n = abs(anFeatsP[i]-pnFeatsVT5[i]);
		if (n > 0)
		{
			dprintf("%s %d differs, P - %d, VT5 - %d\n", pszDescr, i, anFeatsP[i], pnFeatsVT5[i]);
			return S_FALSE;
		}
	}
	return S_OK;
}
#endif



HRESULT CWoolzObject::Compare(int nData, int nSize, void *pData)
{
#if defined(_DEBUG)
	struct object *obj1 = (struct object *)obj;
	int anFeatsP[30];
	char szBuff[50];
	CHROMOFEATURES *pFeats;
	switch(nData)
	{
	case COMPARE_ORIGOBJECT:
		return _CompareObject(pdebug, (struct object *)oldobj, (INTERVALIMAGE *)pData, "orig intvl img");
	case COMPARE_OBJECT:
		return _CompareObject(pdebug, (struct object *)obj, (INTERVALIMAGE *)pData, "intvl img");
	case COMPARE_SHORTAXIS:
		return _CompareAxis(pdebug, nData, nSize, pData, obj1, pdebug->shortaxis, "short axis", 1);
	case COMPARE_SMOOTHSHORTAXIS:
		return _CompareAxis(pdebug, nData, nSize, pData, obj1, pdebug->smoothshortaxis, "smoothed short axis", 1);
	case COMPARE_LONGAXIS:
		return _CompareAxis(pdebug, nData, nSize, pData, obj1, pdebug->longaxis, "long axis", 1);
	case COMPARE_PROFILE0:
		return _CompareProfile(nSize, (int *)pData, pdebug->prof0, "Profile 0", 0);
	case COMPARE_PROFILE1:
		return _CompareProfile(nSize, (int *)pData, pdebug->prof1, "Profile 1", 0);
	case COMPARE_SPROFILE0:
		return _CompareProfile(nSize, (int *)pData, pdebug->sprof0, "Smoothed profile 0", 0);
	case COMPARE_SPROFILE1:
		return _CompareProfile(nSize, (int *)pData, pdebug->sprof1, "Smoothed profile 1", 0);
	case COMPARE_FEATURES:
		getvec((struct chromplist *)obj1->plist, anFeatsP);
		return _CompareFeatures(nSize, anFeatsP, (int *)pData, "Feature");
	case COMPARE_NFEATURES:
		pFeats = (CHROMOFEATURES *)pData;
		if (!pdebug || !pdebug->nvec)
			return S_FALSE;
		sprintf(szBuff, "Obj %d norm feat", pFeats->nObj);
		return _CompareFeatures1(nSize, pdebug->nvec, pFeats->pnFeats, szBuff);
	}

#endif
	return S_OK;
}

HRESULT CWoolzObject::FreeDebugData()
{
#if defined(DEBUG)
	FreeDebugObject(pdebug);
	pdebug = NULL;
#endif
	return S_OK;
}

HRESULT CWoolzObject::GetDebugData(void **p)
{
#if defined(_DEBUG)
	*p = pdebug;
#endif
	return S_OK;
}




