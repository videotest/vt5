// BlotOb.cpp : Implementation of CBlotOb

#include "stdafx.h"
#include <comdef.h>
#include <data5.h>
#include <statistics.h>
#include "matrix.h"
#include <iostream>
#include <fstream>
#include "BlotOb.h"

#define display(var) (" " #var " ") << var

enum NumImg {
	AllImages=-1,
	MaxImageId=9999
};
inline NumImg operator++( NumImg &rs, int )
{
	return rs = (NumImg)(rs + 1);
}

struct LessGUID{
	bool operator()(const GUID&g1,const GUID&g2)const{
		return memcmp(&g1,&g2,sizeof GUID)<0;
	}
};

typedef map<GUID, NumImg, LessGUID> MapGUID;

// CBlotOb


STDMETHODIMP CBlotOb::get_Classes(VARIANT* pVal)
{
//	_fout<<"get_Classes"<<endl;
	CComSafeArray<VARIANT> sa((ULONG)_setClasses.size());
	int i=0;
	for(set<SHORT>::const_iterator it=_setClasses.begin(); 
		it!=_setClasses.end(); ++it, ++i)
	{
		sa[i]=*it;
	}
	pVal->vt = VT_ARRAY | VT_VARIANT;
	pVal->parray=sa.Detach();
	return S_OK;
}

STDMETHODIMP CBlotOb::put_Classes(VARIANT newVal)
{
//	_fout<<"put_Classes";
	bool bRef=false;
	if((VT_ARRAY & newVal.vt) && (VT_VARIANT & newVal.vt))
	{
		_setClasses.clear();
		const CComSafeArray<VARIANT>& sa=(VT_BYREF & newVal.vt)?*newVal.pparray:newVal.parray;
		for(int i=0; i<(int)sa.GetCount(); ++i)
		{
			const VARIANT& rfConstVar = sa[i];
			if(VT_I2==rfConstVar.vt){
				_setClasses.insert(rfConstVar.iVal);
//				_fout<<" "<<rfConstVar.iVal;
			}
			else
			{
//			CComVariant var;
//			HRESULT hr=VariantChangeType(&var,&const_cast<VARIANT&>(rfConstVar),0,VT_I2);
			}
		}
//		_fout<<endl;
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP CBlotOb::get_Table(IUnknown** pVal)
{
//	_fout<<"---- "<<"get_Table"<<endl;
	*pVal=(LPUNKNOWN)_sptrTable;
	return S_OK;
}

STDMETHODIMP CBlotOb::put_Table(IUnknown* newVal)
{
//	_fout<<"---- "<<"put_Table"<<endl;
	_pTableND=newVal;
	if(_pTableND==0)
		return E_INVALIDARG;
	_sptrTable=newVal;
	if(_sptrTable==0)
		return E_INVALIDARG;
	return S_OK;
}

STDMETHODIMP CBlotOb::get_XPrecision(FLOAT* pVal)
{
//	_fout<<"---- "<<"get_XPrecision"<<endl;
	*pVal=_fXPrecision;
	return S_OK;
}

STDMETHODIMP CBlotOb::put_XPrecision(FLOAT newVal)
{
//	_fout<<"---- "<<"put_XPrecision "<<newVal<<endl;
	_fXPrecision=newVal;
	return S_OK;
}

STDMETHODIMP CBlotOb::get_YPrecision(FLOAT* pVal)
{
//	_fout<<"---- "<<"get_YPrecision"<<endl;
	*pVal=_fYPrecision;
	return S_OK;
}

STDMETHODIMP CBlotOb::put_YPrecision(FLOAT newVal)
{
//	_fout<<"---- "<<"put_YPrecision "<<newVal<<endl;
	_fYPrecision=newVal;
	return S_OK;
}

STDMETHODIMP CBlotOb::get_Count(SHORT* pVal)
{
	*pVal=(short)_Count;
	return S_OK;
}

STDMETHODIMP CBlotOb::get_Area(SHORT Index, FLOAT* pVal)
{
	*pVal=_vit[Index].second.sf;
	return S_OK;
}

STDMETHODIMP CBlotOb::get_MassX(SHORT Index, FLOAT* pVal)
{
//	_fout<<"---- "<<"get_MassX "<<*pVal<<endl;
	*pVal=_vit[Index].second.xf;
	return S_OK;
}

STDMETHODIMP CBlotOb::get_MassY(SHORT Index, FLOAT* pVal)
{
	*pVal=_vit[Index].second.yf;
//	_fout<<"---- "<<"get_MassY "<<*pVal<<endl;
	return S_OK;
}

struct greaterS
{
	greaterS(const CBlotOb& blot):blot(blot)
	{
		xFrameCentre=blot._fXMin + blot._fXFrameSize/2.f;
		yFrameCentre=blot._fYMin + blot._fYFrameSize/2.f;
	}
	float xFrameCentre, yFrameCentre;

	const CBlotOb& blot;

	bool operator() (const PtSxy& a, const PtSxy& b) const
	{
		if( fabs(a.second.sf - b.second.sf)/(fabs(a.second.sf) + fabs(b.second.sf))
			<= numeric_limits<float>::epsilon())
		{
			Gdiplus::PointF ca(xFrameCentre + blot._fStepX*a.first.x, yFrameCentre + blot._fStepY*a.first.y);
			float ro_a=a.second.CentreMassRo(ca);

			Gdiplus::PointF cb(yFrameCentre + blot._fStepX*b.first.x, yFrameCentre + blot._fStepY*b.first.y);
			float ro_b=b.second.CentreMassRo(cb);

			return ro_a < ro_b;
		}
		else
		{
			return a.second.sf > b.second.sf ;
		}
	}
};

bool  CBlotOb::overlap(const PtSxy& a, const PtSxy& b)
{
	Gdiplus::RectF ra(_fXMin + _fStepX*a.first.x, _fYMin + _fStepY*a.first.y, _fXSize, _fYSize);
	Gdiplus::RectF rb(_fXMin + _fStepX*b.first.x, _fYMin + _fStepY*b.first.y, _fXSize, _fYSize);
	Gdiplus::RectF rc;
	if(ra.Intersect(rc,ra,rb))
		if(rc.Width*rc.Height>=_fMaxOverlapArea)
			return true;
	return false;
}

STDMETHODIMP CBlotOb::Find(void)
{
//	_fout<<"--------start Find"<<endl;
	map<GUID,pair<NumImg,Gdiplus::PointF>,LessGUID> mapGuidGroup2Img;
	{
		LONG_PTR lPosGroup = 0;
		_sptrTable->GetFirstGroupPos( &lPosGroup );

		while( lPosGroup )
		{
			stat_group *pgroup = 0;
			_sptrTable->GetNextGroup( &lPosGroup, &pgroup );
			map<CComBSTR,Gdiplus::PointF>::iterator itImg=_pictureOffsets.find(pgroup->bstr_image_name);
			if(itImg!=_pictureOffsets.end())
			{
				if(mapGuidGroup2Img.size()){
					_fXMin=__min(_fXMin,itImg->second.X);
					_fYMin=__min(_fYMin,itImg->second.Y);
					_fXMax=__max(_fXMax,itImg->second.X);
					_fYMax=__max(_fYMax,itImg->second.Y);
				}else{
					_fXMin=itImg->second.X;
					_fYMin=itImg->second.Y;
					_fXMax=itImg->second.X;
					_fYMax=itImg->second.Y;
				}
				mapGuidGroup2Img.insert(map<GUID,pair<NumImg,Gdiplus::PointF>,LessGUID>::value_type(pgroup->guid_group,
					pair<NumImg,Gdiplus::PointF>((NumImg)mapGuidGroup2Img.size(),Gdiplus::PointF(itImg->second))));
			}
		}
		if(_fXSize>0.f && _fYSize>0.f){
			_fXMax +=_fXSize;
			_fYMax +=_fYSize;
			//_fXMax =_fXMin + 4.f* _fXSize;
			//_fYMax =_fYMin + 4.f* _fYSize;
		}else{
			return E_FAIL;
		}
	}


	INamedDataObject2Ptr sptrDNO = (LPUNKNOWN)_sptrTable;
	if( sptrDNO == 0 )
		return E_FAIL;

	if(_fXPrecision>0.f && _fYPrecision>0.f){
		_fStepX =_fXPrecision;
		_fStepY =_fYPrecision;
	}else{
		return E_FAIL;
	}

	int _NX=(int)ceil((_fXMax-_fXMin)/_fStepX);
	int _NY=(int)ceil((_fYMax-_fYMin)/_fStepY);
	matrix<sxy> mapPtSxy(_NX,_NY);

	if(_isnan(_fMaxOverlapArea))
	{
		_fMaxOverlapArea = _fStepX*_fStepY;
	}
	//_fout<<"_fMaxOverlapArea="<<_fMaxOverlapArea<<endl;
	//_fout<<" _fXPrecision="<<_fXPrecision<<" _fYPrecision="<<_fYPrecision<<endl;
	//_fout<<" _fXSize="<<_fXSize<<" _fYSize="<<_fYSize<<endl;
	//_fout<<" _fXFrameSize="<<_fXFrameSize<<" _fYFrameSize="<<_fYFrameSize<<endl;

	//_fout<<" _fXMin="<<_fXMin<<" _fYMin="<<_fYMin<<endl;
	//_fout<<" _fXMax="<<_fXMax<<" _fYMax="<<_fYMax<<endl;
	//_fout<<" _fStepX="<<_fStepX<<" _fStepY="<<_fStepY<<endl;

	TPOS lPos_AreaParam = 0;
	TPOS lPos_XParam = 0;
	TPOS lPos_YParam = 0;

	_sptrTable->GetParamPosByKey( 0, &lPos_AreaParam );	
	_sptrTable->GetParamPosByKey( 23, &lPos_XParam );	
	_sptrTable->GetParamPosByKey( 24, &lPos_YParam );	

	LONG_PTR lPosRow = 0; 
	_sptrTable->GetFirstRowPos( &lPosRow ); 
	while( lPosRow ) 
	{ 
		stat_row *pRow = 0; 
		LONG_PTR lChild = lPosRow;
		_sptrTable->GetNextRow( &lPosRow, &pRow );	

		IUnknown *punkO = 0;
		{
			TPOS lP = (TPOS)lChild;
			sptrDNO->GetNextChild( &lP, &punkO );
		}

		ICalcObjectPtr sptrCalc = punkO;
		if( punkO )
			punkO->Release(); punkO = 0;

		short lclass = (short)get_object_class( sptrCalc, short_classifiername( 0 ) ); 


		if(_setClasses.find(lclass)!=_setClasses.end())
		{

			double fArea = std::numeric_limits<double>::quiet_NaN();
			double fx = std::numeric_limits<double>::quiet_NaN();
			double fy = std::numeric_limits<double>::quiet_NaN();

			stat_value *pvalue;
			pvalue = 0;
			_sptrTable->GetValue( lChild, (LONG_PTR)lPos_AreaParam, &pvalue );

			bool bValidData=false;
			if(pvalue && pvalue->bwas_defined)
			{
				fArea = pvalue->fvalue;

				pvalue = 0;
				_sptrTable->GetValue(lChild, (LONG_PTR)lPos_XParam, &pvalue);
				if(pvalue && pvalue->bwas_defined)
				{
					fx = pvalue->fvalue;

					pvalue = 0;
					_sptrTable->GetValue(lChild, (LONG_PTR)lPos_YParam, &pvalue);
					if(pvalue && pvalue->bwas_defined)
					{
						fy = pvalue->fvalue;

						bValidData=true;
					}
				}
			}

			if(bValidData)
			{
				const pair<NumImg,Gdiplus::PointF>& pr=mapGuidGroup2Img[pRow->guid_group];
				{
					if(0.f<=fx && fx<_fXSize && 0.f<=fy && fy<_fYSize)
					{
						float fX=(float)fx + pr.second.X;
						float fY=(float)fy + pr.second.Y;

						CPoint pt((int)floor((fX-_fXMin)/_fStepX), (int)floor((fY-_fYMin)/_fStepY));
						sxy& SXY=mapPtSxy[pt];
						++SXY.n;
						SXY.s	+=(float)fArea;
						SXY.x	+=fX*(float)fArea;
						SXY.y	+=fY*(float)fArea;
//						_fout<<display(pr.first)<<display(fX)<<display(pr.second.X)<<display(pr.second.Y)<<display(pt.x)<<display(pt.y)<<endl;
					}else{
						::MessageBox(0,"Объект вне заданного изображения. Проверь правильность координат объектов и изображения",
							0,MB_ICONERROR);
					}
				}
			}
			else
			{
				return E_FAIL;
			}
		}
	}

	_vit.clear();
	int iEdge=(int)ceil(_fXFrameSize/_fStepX);
	int jEdge=(int)ceil(_fYFrameSize/_fStepY);
	for(CPoint pt(0,0); pt.y<_NY; ++pt.y)
	{
		for(pt.x=0; pt.x<_NX; ++pt.x)
		{
			sxy& SXY=mapPtSxy[pt];
			int iEnd=pt.x+iEdge;
			int jEnd=pt.y+jEdge;
			float nf=0.f, sf=0.f, xf=0.f, yf=0.f;
			for(CPoint ptij(pt); ptij.y<__min(_NY, jEnd); ++ptij.y)
			{
				for(ptij.x=pt.x; ptij.x<__min(_NX, iEnd); ++ptij.x)
				{
					{
						sxy& sij=mapPtSxy[ptij];
						sf += sij.s;
						nf += float(sij.n);
						xf += sij.x;
						yf += sij.y;
					}
				}
			}
			if(nf>0.f)
			{
				SXY.nf=nf;
				SXY.sf=sf;
				SXY.xf = xf/sf;
				SXY.yf = yf/sf;
				float xFrameCentre=_fXMin + _fXFrameSize/2.f;
				float yFrameCentre=_fYMin + _fYFrameSize/2.f;
				Gdiplus::PointF ca(xFrameCentre + _fStepX*pt.x, yFrameCentre + _fStepY*pt.y);
				SXY.roCentreMass=SXY.CentreMassRo(ca);
				_vit.push_back(PtSxy(pt,SXY));
			}
		}
	}

	std::sort(_vit.begin(),_vit.end(),greaterS(*this));

	//VecPtSxy::const_iterator it= _vit.begin();
	//for(int i=0; /*i<40 && */it!=_vit.end(); ++i, ++it)
	//{
	//	const VecPtSxy::value_type& ptSxy= *it;
	//	const CPoint& pt=ptSxy.first;
	//	const sxy& SXY=ptSxy.second;

	//	_fout<<i<<" "<<pt.x<<" "<<pt.y<<" "<<SXY.nf<<" "<<SXY.sf<<" "<<SXY.roCentreMass<<" "<<SXY.xf<<" "<<SXY.yf
	//		<<" "<<(SXY.xf-_fXMin)/_fStepX<<" "<<(SXY.yf-_fYMin)/_fStepY<<endl;
	//}

	int n=0; 
	for(VecPtSxy::iterator lastMember=_vit.begin(); 
		n<_NCadr && lastMember!=_vit.end(); ++n, ++lastMember)
	{
		//VecPtSxy::iterator candidat=_vit.end();
					//const VecPtSxy::value_type& ptSxy= *lastMember;
					//const CPoint& pt=ptSxy.first;
					//const sxy& SXY=ptSxy.second;
					//_fout<<"lastMember "<<pt.x<<" "<<pt.y<<endl;
//		remove_if( lastMember)
		for(VecPtSxy::iterator candidat=_vit.end(); --candidat!=lastMember;)
		{
			//VecPtSxy::iterator candidatTest=candidat--;
			//{
			//		const VecPtSxy::value_type& ptSxy= *candidat;
			//		const CPoint& pt=ptSxy.first;
			//		const sxy& SXY=ptSxy.second;
			//		_fout<<"candidat "<<pt.x<<" "<<pt.y<<endl;
			//}
			//{
			//		const VecPtSxy::value_type& ptSxy= *candidatTest;
			//		const CPoint& pt=ptSxy.first;
			//		const sxy& SXY=ptSxy.second;
			//		_fout<<"candidatTest "<<pt.x<<" "<<pt.y<<endl;
			//}
			//VecPtSxy::iterator member=lastMember;
			{
				if(overlap(*lastMember,*candidat)){
					//const VecPtSxy::value_type& ptSxy= *candidat;
					//const CPoint& pt=ptSxy.first;
					//const sxy& SXY=ptSxy.second;
					//_fout<<"erase "<<pt.x<<" "<<pt.y<<endl;
					_vit.erase(candidat);
				}
			}
		} 
	}

  
//	_fout<<"--------------------------------------------------------------------------"<<endl;

	_Count=__min((long)_vit.size(), _NCadr);

//	_fout<<display(_Count)<<endl;


	//_fout<<"i"<<"\tpt.x"<<"\tpt.y"<<"\tnf"<<"\tsf"<<"\troCentreMass"<<"xf"<<"\tSXY.yf "
	//		<<"(xf-fXMin)/StepX"<<"(yf-fYMin)/StepY     YPic   XPic"<<endl;

	//it= _vit.begin();
	//for(int i=0; /*i<40 &&*/ it!=_vit.end(); ++i, ++it)
	//{
	//	const VecPtSxy::value_type& ptSxy= *it;
	//	const CPoint& pt=ptSxy.first;
	//	const sxy& SXY=ptSxy.second;

	//	_fout<<i<<" "<<pt.x<<" "<<pt.y<<" "<<SXY.nf<<" "<<SXY.sf<<" "<<SXY.roCentreMass<<" "<<SXY.xf<<" "<<SXY.yf
	//		<<" "<<(SXY.yf-_fYMin)/_fYSize+1<<" "<<(SXY.xf-_fXMin)/_fXSize+1<<endl;
	//}
	//_fout<<"--------end Find"<<endl;


	return S_OK;
}

STDMETHODIMP CBlotOb::AddClass(SHORT iClass)
{
//	_fout<<endl<<"--------AddClass "<<iClass<<endl;
	_setClasses.insert(iClass);
	return S_OK;
}

STDMETHODIMP CBlotOb::SetPictureSize(FLOAT x, FLOAT y)
{
//	_fout<<endl<<"--------SetPictureSize "<<display(x)<<display(y)<<endl;
	_fXSize=x;
	_fYSize=y;
	return S_OK;
}

STDMETHODIMP CBlotOb::SetOffs(BSTR Picture, FLOAT x, FLOAT y)
{
//	_fout<<endl<<"--------SetOffs "<<(char*)_bstr_t(Picture)<<display(x)<<display(y)<<endl;
	Gdiplus::PointF pointF(x,y);
	_pictureOffsets[Picture]=pointF;
	return S_OK;
}

STDMETHODIMP CBlotOb::Exclude(BSTR PictureName)
{
	_excludedPictures.insert(PictureName);
	return S_OK;
}

STDMETHODIMP CBlotOb::get_MaxOverlapArea(FLOAT* pVal)
{
	*pVal=_fMaxOverlapArea;
	return S_OK;
}

STDMETHODIMP CBlotOb::put_MaxOverlapArea(FLOAT newVal)
{
//	_fout<<endl<<"--------put_MaxOverlapArea "<<newVal<<endl;
	_fMaxOverlapArea=newVal;
	return S_OK;
}

STDMETHODIMP CBlotOb::get_XShift(SHORT Index, FLOAT* pVal)
{
	*pVal=_vit[Index].first.x*_fXPrecision + _fXMin;
	return S_OK;
}

STDMETHODIMP CBlotOb::get_YShift(SHORT Index, FLOAT* pVal)
{
	*pVal=_vit[Index].first.y*_fYPrecision + _fYMin;
	return S_OK;
}

STDMETHODIMP CBlotOb::get_NCadr(LONG* pVal)
{
	*pVal=_NCadr;
	return S_OK;
}

STDMETHODIMP CBlotOb::put_NCadr(LONG newVal)
{
	_NCadr=newVal;	
	return S_OK;
}

STDMETHODIMP CBlotOb::SetFrameSize(FLOAT x, FLOAT y)
{
//	_fout<<endl<<"--------SetFrameSize "<<display(x)<<display(y)<<endl;
	_fXFrameSize=x;
	_fYFrameSize=y;
	return S_OK;
}
