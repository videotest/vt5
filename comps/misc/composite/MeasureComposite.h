#pragma once
#include <list>
#include <vector>
using namespace std;


static void SetObjectClass( IUnknown *pUnk, long lclass, CString& classifier );
// вспомогательный класс для построения иерархии объектов в акции MeasureComposite
template  <class I>
class CTreeElement
{
public:
	
	I* image;
	int level;
	long pixCount;
	int binNumber;
	CString* classifier;
	CTreeElement(I* im =0, bool needRelease =true)
	{
		if(im)	need_release = needRelease;
		else need_release = false;		
		image = im;
		level =-1;
		parents.clear();
		children.clear();
	};
	~CTreeElement()
	{
		/*if(need_release)*/ image->Release();
	};
	//I* operator =(I* im)
	//{
	//	if(im && need_release) image->Release();
	//	image = im;
	//	need_release=true;
	//	return image;
	//};
	I* operator ->()
	{
		return image;
	};
	void AddChild(CTreeElement<I>* i)
	{
		children.push_back(i);

	};
	void AddParent(CTreeElement<I>* i)
	{
		parents.push_back(i);

	};

	//проверка - элемент имеет единственного родителя помеченного level 
	bool have_no_parents()
	{
		return parents.size()==0;
	}
	void set_level(int binCount, int lvl)
	{
		if(lvl>= binCount) return;
		if(level <lvl ) level = lvl;
		else return;
		
		list <CTreeElement<I>*>::iterator iter;
		iter = children.begin();
		while(iter!=children.end())
		{
			CTreeElement<I>* el = *iter;
			(*el).set_level(binCount, lvl+1);
			iter++;
		}
	}
	void attach_childs(IDataObjectList* o_list, CObjectWrp&	object, CPtrList& usefullGroups, bool bCalc, int minArea =0)
	{
		list <CTreeElement<I>*>::iterator iter;
		iter = children.begin();
		while(iter!=children.end())
		{
			CTreeElement<I>* el = *iter;
			if((*el).is_only_parent(this) && (*el).pixCount>=minArea )
			{
				CObjectWrp	object1(::CreateTypedObject(szTypeObject), false);
				object1.SetImage((*el).image);
				ICompositeSupportPtr comp(object1);
				if(comp) 
				{
					comp->SetPixCount((*el).pixCount);
					comp->SetLevel((*el).level);
				}
				SetObjectClass(object1, (*el).binNumber, *classifier);
				INamedDataObject2Ptr ndo(object1);
				if(ndo)
				{
					ndo->SetParent(object,0);
					if(bCalc)
					{
						POSITION pos = usefullGroups.GetHeadPosition();
						IMeasParamGroupPtr group;
						while(pos)
						{
							group = (IUnknown*)usefullGroups.GetNext(pos);
							group->CalcValues( ndo, (*el).image);
						}
					}
					o_list->UpdateObject(ndo);
				}
				(*el).attach_childs(o_list, object1, usefullGroups, bCalc, minArea);		
			}
			iter++;
		}
	}

	bool is_only_parent(CTreeElement<I>* parent)
	{
		ASSERT(parents.size()<2);
		if(parent==*(parents.begin()))
			return true;
		return false;
	}

	void check_parents()
	{
		int maxParentLvl = -1;
		int ps = parents.size();
		list <CTreeElement<I>*>::iterator iter;
		iter = parents.begin();
		CTreeElement<I>* el;
		while(iter!=parents.end())
		{
			el = *iter;
			ASSERT((*el).level>=0);
			if(maxParentLvl<(*el).level) maxParentLvl=(*el).level;
			iter++;
		}
		iter = parents.begin();
		bool erase =false;
		while(iter!=parents.end())
		{
			el = *iter;
			if((*el).level==maxParentLvl ) break; 
			iter++;
		}
		parents.clear();
		parents.push_back(el);
		/*(*el).AddChild(this);*/
	}

private:
	bool need_release;
	list<CTreeElement<I>*> parents;
	list<CTreeElement<I>*> children;
};

class CLockAM
{
public:
	CLockAM()
	{
		IUnknownPtr punkAM(_GetOtherComponent(GetAppUnknown(), IID_IActionManager),false);
		m_sptrAM = punkAM;
		m_sptrAM->EnableManager(FALSE);
	}
	~CLockAM()
	{
		m_sptrAM->EnableManager(TRUE);
	}
private:
	IActionManagerPtr m_sptrAM;
};

class CActionMeasureComposite :
	public CFilterBase
{
	DECLARE_DYNCREATE(CActionMeasureComposite)
	GUARD_DECLARE_OLECREATE(CActionMeasureComposite)
public:
	CActionMeasureComposite(void);
	virtual ~CActionMeasureComposite(void);
	virtual bool IsAllArgsRequired()			{return false;};
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa ){return false;};
private:
	IBinaryImagePtr bin[20];
	long init_row_mask(CImageWrp& image/*, IBinaryImage* bin, Contour* c, CRect rect*/);
	//построение дерева вложенных объектов
	bool build_hierarchy(CTreeElement<IImage4>** images);
	int m_nMainLevel;
	int m_nPlaneCount;
	CTreeElement<IImage4>** images;
	int notifyLevel;
	vector<long> imCounts;
	double CoverFactor;
	ICompManagerPtr	ptrManager;
	ICompositeMeasureGroupPtr ptrCompMG;
	CPtrList usefullGroups;
	//проверка вложенности 
	//return 1 - 1-й во 2-м, -1 - 2-й в 1-м, 0 - невложенные прямоугольники
	int test_rects_enclosure(const CRect& r1, const CRect& r2);//не используется
	void find_kins(CTreeElement<IImage4>&, CTreeElement<IImage4>** );
	//true, если image1 дочерний объект для image2
	bool check_if_kin(CImageWrp& image1, CImageWrp& image2,long image1_pixCount);
	bool build_tree(CTreeElement<IImage4>**);
	void sort(CTreeElement<IImage4>**);
	void construct_composite(CTreeElement<IImage4>**);
	void clean();
	bool prepare_calculation(CObjectListWrp& objects);
	bool finalize_calculation();
	bool check_group_for_useless(IMeasParamGroupPtr& group);
	CString m_strClassifier;
	bool m_bCalc;


};
