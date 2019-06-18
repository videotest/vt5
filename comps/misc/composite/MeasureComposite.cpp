#include "StdAfx.h"

#include "resource.h"
#include "math.h"
#include "NameConsts.h"
#include "float.h"
#include "binimageint.h"
#include "units_const.h"
#include "misc_utils.h"
#include "\vt5\awin\profiler.h"
#include "measurecomposite.h"
//#include "class_utils.h"
#include "PropBag.h"

#ifdef _DEBUG
	static long dbgCount =0;
#endif

IMPLEMENT_DYNCREATE(CActionMeasureComposite, CCmdTargetEx);


// {4E99828B-DAC8-4726-8749-0F8EAA056D16}
GUARD_IMPLEMENT_OLECREATE(CActionMeasureComposite, "Composite.MeasureComposite", 
0x4e99828b, 0xdac8, 0x4726, 0x87, 0x49, 0xf, 0x8e, 0xaa, 0x5, 0x6d, 0x16);

// {587E031C-D806-4726-911C-0F60E8798D38}
static const GUID guidMeasureComposite = 
{ 0x587e031c, 0xd806, 0x4726, { 0x91, 0x1c, 0xf, 0x60, 0xe8, 0x79, 0x8d, 0x38 } };



ACTION_INFO_FULL(CActionMeasureComposite,IDS_ACTION_MEASURE_COMPOSITE, -1, -1, guidMeasureComposite); 

ACTION_TARGET(CActionMeasureComposite, szTargetAnydoc);

//#define ARG_BINIMAGE_(arg_name) {arg_name, szArgumentTypeBinaryImage, 0, true, true},

ACTION_ARG_LIST(CActionMeasureComposite)
	
	ARG_INT(_T("Calc"), 1 )
	RES_OBJECT( _T("CompositeObjects") )
	ARG_IMAGE(_T("Image"))
	ARG_BINIMAGE(_T("Binary1"))
	ARG_BINIMAGE(_T("Binary2"))
	ARG_BINIMAGE(_T("Binary3"))
	ARG_BINIMAGE(_T("Binary4"))
	ARG_BINIMAGE(_T("Binary5"))
	ARG_BINIMAGE(_T("Binary6"))
	ARG_BINIMAGE(_T("Binary7"))
	ARG_BINIMAGE(_T("Binary8"))	
	ARG_BINIMAGE(_T("Binary9"))
	ARG_BINIMAGE(_T("Binary10"))
	ARG_BINIMAGE(_T("Binary11"))
	ARG_BINIMAGE(_T("Binary12"))		
	ARG_BINIMAGE(_T("Binary13"))
	ARG_BINIMAGE(_T("Binary14"))
	ARG_BINIMAGE(_T("Binary15"))
	ARG_BINIMAGE(_T("Binary16"))		
	ARG_BINIMAGE(_T("Binary17"))
	ARG_BINIMAGE(_T("Binary18"))
	ARG_BINIMAGE(_T("Binary19"))
	ARG_BINIMAGE(_T("Binary20"))
END_ACTION_ARG_LIST()
//#undef ARG_BINIMAGE_

static void SetObjectClass( IUnknown *pUnk, long lclass, CString& classifier )
{

	INamedPropBagPtr bag(pUnk);
	if(bag!=0)
	{

	
		int pos = classifier.ReverseFind('\\')+1;
		TCHAR* buffer = classifier.GetBuffer();
		buffer+=pos;
		bstr_t clName(buffer);
		bag->SetProperty(clName, _variant_t(lclass));
	}
}
static inline void SetMainObjectFlag( IUnknown *pUnk)
{
	INamedPropBagPtr bag(pUnk);
	if(bag!=0)
	{

		bstr_t flag("MainObject");
		bag->SetProperty(flag, _variant_t(1L));
	}
}



CActionMeasureComposite::CActionMeasureComposite(void)
{
	ZeroMemory(bin,sizeof(IBinaryImagePtr)*20);
	images =0;
	notifyLevel =0;
	
}

CActionMeasureComposite::~CActionMeasureComposite(void)
{
	POSITION pos = usefullGroups.GetHeadPosition();
	while(pos)
	{
		IUnknown* unk = (IUnknown*)usefullGroups.GetNext(pos);
		unk->Release();
	}
	usefullGroups.RemoveAll();
}
void CActionMeasureComposite::clean()
{
	if(!images)return;
	for(int i=0;i<m_nPlaneCount;i++)
	{
		delete[] images[i];
	}
	delete[] images; 
}
bool CActionMeasureComposite::InvokeFilter()
{	

	CLockAM lam;
	CoverFactor = GetValueDouble(GetAppUnknown(),_T("Composite"), _T("CoverFactor"),.95);
	m_strClassifier = GetValueString(GetAppUnknown(),_T("Classes"),_T("CompositeObjectsClassFile"),_T("Composite.complex")); 
	
	{
		CFile f;
		CFileException fex;
		BOOL b = f.Open(m_strClassifier, CFile::modeRead, &fex);
		if(b)
		{
			f.Close();
		}
		else
		{
			AfxMessageBox("Classifier file not found! Action failed.", MB_ICONERROR | MB_OK, 0);
			return false;
		}
	}
	StartNotification(100,1,1);
	CString str;
	for(int i=1;i<=20;i++)
	{
		TCHAR t[3];
		t[2]=0;
		_itot(i,t,10);
		 str = _T("Binary");
		str+=t;
		IUnknown* tempUnk = GetDataArgument(str);
		bin[i-1] = tempUnk;
		if(tempUnk == 0) break;
		/*tempUnk->Release();*/
	}


	m_nPlaneCount = i-1;
	CImageWrp imageMain( GetDataArgument("Image") );
	m_bCalc = GetArgumentInt(_T("Calc"))!=0;

	int cx; 
	int cy; 
	bin[0]->GetSizes( &cx, &cy );
	CRect frameRect= imageMain.GetRect();


	INamedDataObject2Ptr ndo2 = imageMain;
	IUnknown* unk =0;
	ndo2->GetParent(&unk);
	CImageWrp baseImage;
	if(unk)	{baseImage = unk; unk->Release(); unk=0;}
	else baseImage = imageMain;

	//для каждого бинарного изображения создаем и инициализируем  
	images = new CTreeElement<IImage4>*[m_nPlaneCount];
	notifyLevel=3;
	for(i=0;i<m_nPlaneCount;i++)
	{
		Notify(notifyLevel);
		CWalkFillInfo wi(frameRect);
		for(int j=0;j<frameRect.Height();j++)
		{
			byte* wiRow = wi.GetRowMask(j);
			byte* source;
			byte* rowMask;
			bin[i]->GetRow(&source, j+frameRect.top, 0);
			source+= frameRect.left;
			rowMask = imageMain.GetRowMask(j);
			for(int k=0;k<frameRect.Width();k++,wiRow++,source++,rowMask++)
			{
				*wiRow = *source>0 && *rowMask?255:0;
			}
		}
		IUnknown* imUnk =0;
		list<IUnknown*> tempImages;
		CPoint pt(-1,-1);

		while(wi.InitContours(true, pt,&pt, NORECT, baseImage, &imUnk))
		{
			if(imUnk)
			{
				tempImages.push_back(imUnk);
				CImageWrp iw(imUnk);
				for( int n = 0; n < wi.GetContoursCount(); n++ )
				{
					Contour	*pc = wi.GetContour( n );
					iw->AddContour( pc );
				}


			}
		wi.DetachContours();
		}
		long imagesCount = tempImages.size();
		imCounts.push_back(imagesCount);
		if(imagesCount)	
			images[i]=new CTreeElement<IImage4>[imagesCount];
		else 
		{
			images[i]=0;
			continue;
		}
		
		for(int l=0;l<imagesCount;l++)
		{

			CImageWrp image(tempImages.front());
			tempImages.pop_front();
			images[i][l].pixCount = init_row_mask(image/*, bin[i], c, rect*/);//rowmasks будут служить бинарными изображениями			
			images[i][l].image = image;
			/*images[i][l].image->AddRef();*/
			images[i][l].binNumber = i;
			images[i][l].classifier = &m_strClassifier;
			
		}

		notifyLevel++;
	}

	//объектные листы созданы - теперь из объектов в них строим иерархию 	
	if(!build_hierarchy(images)) {clean();return false;}


	/*	*/
	clean();

	FinishNotification();
	return true;
}
bool CActionMeasureComposite::build_hierarchy(CTreeElement<IImage4>** images)
{	
	notifyLevel++;
	//имеем массив images с интерфейсами объектов, разнесенными по плоскостям 
	for(int i=0;i<m_nPlaneCount;i++ )
	{
		Notify(notifyLevel+=(20-notifyLevel)/m_nPlaneCount);
		if(images[i])
		{
			for(int j=0;j<imCounts[i];j++)
			{
				find_kins(images[i][j], images);
			}
		}
	}
#ifdef _DEBUG
	CString dStr;
	dStr.Format("--- Operation count %i\n", dbgCount);
	OutputDebugStr(dStr);
#endif
	if(!build_tree(images)) return false;
	Notify(40);
	construct_composite(images);

	return true;
}
void CActionMeasureComposite::construct_composite(CTreeElement<IImage4>** images)
{
	CObjectListWrp objects(GetDataResult());
	SetValue(objects, _T("Composite"), _T("CompositeObjectsList"), 1L);
	int minArea = GetValueInt(GetAppUnknown(),_T("measurement"), _T("MinArea"),0);
	ICompositeObjectPtr comp(objects);
	if(comp) comp->SetCompositeFlag();
	
	CListLockUpdate lock(objects);
	double notifyStep = 59./imCounts[m_nMainLevel];
	if(m_bCalc) prepare_calculation(objects);
	IDataObjectListPtr sptrList(objects);
	for(int j=0;j<imCounts[m_nMainLevel];j++)
	{
		if(images[m_nMainLevel][j].pixCount<minArea) continue;
		CObjectWrp	object(::CreateTypedObject(szTypeObject), false);
		SetObjectClass(object, m_nMainLevel, m_strClassifier);
		SetMainObjectFlag(object);
		object.SetImage(images[m_nMainLevel][j].image);
		ICompositeSupportPtr comp(object);
		if(comp) 
		{
			comp->SetPixCount(images[m_nMainLevel][j].pixCount);
			comp->SetLevel(images[m_nMainLevel][j].level);
		}
		images[m_nMainLevel][j].attach_childs(sptrList, object, usefullGroups, m_bCalc, minArea);
		INamedDataObject2Ptr ndo(object);
		if(ndo)
		{
			ndo->SetParent(objects,0);
		}
		Notify(40+notifyStep*(j+1));
	}

	CCompositeInfo ci;
	ci.m_nCount = m_nPlaneCount;
	ci.p_nClass = new int[m_nPlaneCount];
	for(int i=0;i<m_nPlaneCount;i++)
	{
		ci.p_nClass[i] = images[i][0].level;
	}
	ci.m_bActual = TRUE;
	
	ICompositeObjectPtr co(objects);
	co->SetCompositeInfo(&ci);
	delete[] ci.p_nClass;

	co->BuildTree(m_nPlaneCount);
	if(m_bCalc) 
	{
		IMeasParamGroupPtr	ptrGroup( ptrCompMG);
		IUnknown* unkMainLevel;
		co->GetMainLevelObjects(&unkMainLevel);
		INamedDataObject2Ptr ndoML(unkMainLevel);

		if(ndoML)
		{
			long pos;
			ndoML->GetFirstChildPosition(&pos);
			while(pos)
			{
				IUnknown* unk;
				ndoML->GetNextChild(&pos, &unk);
				if(unk)
				{
					IMeasureObject2Ptr mo(unk);
					if(mo ==0) continue;
					IUnknown* img;
					mo->GetImage(&img);
					if(img ==0) continue;
					POSITION pos = usefullGroups.GetHeadPosition();
					while(pos)
					{
						IMeasParamGroupPtr measGrp = (IUnknown*)usefullGroups.GetNext(pos);

						measGrp->CalcValues( unk, img);
						

					}
					img->Release();
					sptrList->UpdateObject(unk);
					unk->Release();
				}
			}
		}
		finalize_calculation();	
	}



}
bool CActionMeasureComposite::build_tree(CTreeElement<IImage4>** images)
{
	//проверка количества родителей - должен остаться только один
	//если сирота - главный объект
	m_nMainLevel =-1;
	bool mainLevel =false;
	for(int i=0;i<m_nPlaneCount;i++)
	{
		
		for(int j=0;j<imCounts[i];j++)
		{
			mainLevel =true;
			if(!images[i][j].have_no_parents()) {mainLevel=false; break;}

		}
		if(mainLevel)
		{
			m_nMainLevel = i;

			
			break;
		}
	}

	if(m_nMainLevel==-1) {AfxMessageBox("MeasureComposite error: Unable determine base binary !---\n", MB_ICONERROR | MB_OK, 0);return false;}


	//нашли старший уровень - рекурсивно помечаем дочерние элементы
	for(int j=0;j<imCounts[m_nMainLevel];j++)
	{	
		images[m_nMainLevel][j].set_level(m_nPlaneCount,0);
	}

	//удаляем "лишних" родителей
	for(int i=0;i<m_nPlaneCount;i++)
	{
		for(int j=0;j<imCounts[i];j++)
		{
			images[i][j].check_parents();
		}
	}
	//для каждого слоя устанавливаем Level равный максимальному из помеченных на пред. этапе

	for(int i=0;i<m_nPlaneCount;i++)
	{	
		
		int l =-1;
		for(int j=0;j<imCounts[i];j++)
		{
			if(i==m_nMainLevel) break;
			if(images[i][j].level>l) l = images[i][j].level;
		}
		for(j=0;j<imCounts[i];j++)
		{	
			if(i==m_nMainLevel) l=0; 
			images[i][j].level = l;
		}
	}



return true;
}

void CActionMeasureComposite::sort(CTreeElement<IImage4>** image)
{
	
}

void CActionMeasureComposite::find_kins(CTreeElement<IImage4>& el, CTreeElement<IImage4>** images)
{

	CImageWrp image(el.image);
	CRect rect = image.GetRect();

	for(int i=0;i<m_nPlaneCount;i++)
	{
		if(el.binNumber == i) continue;
		long count = imCounts[i];
		for(int n=0;n<count;n++)
		{

#ifdef _DEBUG
			dbgCount++;
#endif
			CImageWrp im_t(images[i][n].image);

			CRect r= im_t.GetRect();
			RECT dummy;
			BOOL test = IntersectRect(&dummy,rect,r);
			if(test)
			{
				
				if(el.pixCount< images[i][n].pixCount)//м.б. el/image/ дочерний для images[i][n] /im_t/
				{
					if(check_if_kin(image, im_t, el.pixCount))
					{
						el.AddParent(&images[i][n]);
						images[i][n].AddChild(&el);
					}
				}
				//else if(el.pixCount> images[i][n].pixCount) //м.б. el/image/ родитель для images[i][n]/im_t/
				//{
				//	if(check_if_kin(im_t, image, images[i][n].pixCount))
				//	{
				//		el.AddChild(&images[i][n]);
				//	}
				//}
			}
		}
	}
}
bool CActionMeasureComposite::prepare_calculation(CObjectListWrp& objects)
{

	IUnknown *punk = FindComponentByName(GetAppUnknown(false), IID_IMeasureManager, szMeasurement);
	ptrManager= punk ;
	if( punk )punk->Release();
	if(ptrManager ==0) return false;
	int	nGroup, nGroupCount;
	ICompositeMeasureGroupPtr cmg;
	ptrManager->GetCount( &nGroupCount );
	for( nGroup = 0; nGroup < nGroupCount; nGroup++ )
	{
		IUnknown*	unkGroup;
		ptrManager->GetComponentUnknownByIdx( nGroup, &unkGroup );
		IMeasParamGroupPtr	ptrGroup( unkGroup );
		if(unkGroup) unkGroup->Release();

		if( ptrGroup == 0 )continue;

		//if(ptrCompMG == 0)  
		//{
		//
		//	if((ptrCompMG = ptrGroup)!= 0)
		//	continue;

		//}
		
		if(check_group_for_useless(ptrGroup))//? - содержится ли в группе хотя бы один "Enabled" parameter
		{
			ptrGroup->InitializeCalculation(objects );
			usefullGroups.AddTail(ptrGroup.Detach());
		}
	}
	//if(ptrCompMG)
	//{
	//	IMeasParamGroupPtr	ptrGroup( ptrCompMG);
	//	ptrGroup->InitializeCalculation(objects );
	//}

	return true;
}
bool CActionMeasureComposite::finalize_calculation()
{
	//if(ptrManager==0) return false;
	//int	nGroup, nGroupCount;
	//ptrManager->GetCount( &nGroupCount );
	//for( nGroup = 0; nGroup < nGroupCount; nGroup++ )
	//{
	//	IUnknownPtr	ptrG = 0;
	//	ptrManager->GetComponentUnknownByIdx( nGroup, &ptrG );
	//	IMeasParamGroupPtr	ptrGroup( ptrG );
	//	if( )continue;
	//	ptrGroup->FinalizeCalculation();
	//}
	POSITION pos = usefullGroups.GetHeadPosition();
	while(pos)
	{
		
		IUnknown* unk = (IUnknown*)usefullGroups.GetNext(pos);
		IMeasParamGroupPtr	ptrGroup( unk );
		if(ptrGroup != 0 ) 
			ptrGroup->FinalizeCalculation();
		unk->Release();
	}
	IUnknown* unk = this->ptrCompMG.Detach();
	IMeasParamGroupPtr ptrGroup(unk);
	if(unk)unk->Release();
	if(ptrGroup != 0 ) 	ptrGroup->FinalizeCalculation();
	unk = this->ptrManager.Detach();
	unk->Release();
	usefullGroups.RemoveAll();

	return true;
}
bool CActionMeasureComposite::check_if_kin(CImageWrp& image1, CImageWrp& image2, long image1_pixCount)
{
	//площадь image1 меньше image2
	CRect r1 = image1.GetRect();
	CRect r2 = image2.GetRect();
	CRect R;
	long count=0;
	long limit = CoverFactor*image1_pixCount;
	if(!R.IntersectRect(r1,r2)) return false;
	CPoint offset1(R.TopLeft()-r1.TopLeft());
	CPoint offset2(R.TopLeft()-r2.TopLeft());
	bool out =false;
	for(int j=0;j<R.Height();j++)
	{
		byte *row1 = image1.GetRowMask(j+offset1.y);
		byte *row2= image2.GetRowMask(j+offset2.y);
		for(int i=0;i<R.Width();i++)
		{
			if((row1[i+ offset1.x] & row2[i+offset2.x])==255 ) count++;//считаем попадания
		}
		if(count>limit) 
			return true;
	}
	return false;
}
int CActionMeasureComposite::test_rects_enclosure(const CRect& r1, const CRect& r2)
{
	if(r1.left <= r2.left && r1.top <= r2.top && r1.right >= r2.right && r1.bottom >= r2.bottom)
	return 1;

	if(r1.left >= r2.left && r1.top >= r2.top && r1.right <= r2.right && r1.bottom <= r2.bottom)
	return -1;

	return 0;
}


bool CActionMeasureComposite::check_group_for_useless(IMeasParamGroupPtr& group)
{

	long	lposP = 0;
	group->GetFirstPos( &lposP );
	while( lposP )
	{
		ParameterDescriptor	*pdescr;
		group->GetNextParam( &lposP, &pdescr );
		if( pdescr->lEnabled ) return true;			
	}
	return false;

}
#define CONTOUR_MARK 0xfe
long CActionMeasureComposite::init_row_mask(CImageWrp& image/*, IBinaryImage* bin, Contour* c, CRect rect*/)
{
	bool bInside;
	long pixCount=0;
	//mark contour on the rowmasks
	byte* rowMask;
	int cx,cy;
	image->GetSize( &cx, &cy );
	for(int j=0;j<cy;j++)
	{
		bInside=false;
		
		image->GetRowMask(j, &rowMask);

		for(int i=0;i<cx;i++)
		{			
			if(rowMask[i]==255) pixCount++;
		}
	}
	return pixCount;
}

#undef CONTOUR_MARK









