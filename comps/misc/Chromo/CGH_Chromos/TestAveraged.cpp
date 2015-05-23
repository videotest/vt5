#include "stdafx.h"
#include "testaveraged.h"
#include "resource.h"
#include "misc_utils.h"
#include "alloc.h"
#include "math.h"
#include "\vt5\common2\class_utils.h"

IMPLEMENT_DYNCREATE(CTestAveraged,	CCmdTargetEx);

GUARD_IMPLEMENT_OLECREATE(CTestAveraged, "CGH_Chromos.TestAveraged", 
0x50d0af5e, 0x5d57, 0x4d1c, 0xb7, 0x2c, 0x30, 0xf1, 0x16, 0x8b, 0x85, 0x87);

static const GUID guidTestAveraged = 
{ 0xd97889bd, 0x34f2, 0x435b, { 0x90, 0x93, 0x78, 0x58, 0xe7, 0x6, 0x82, 0x9c } };

ACTION_INFO_FULL(CTestAveraged, IDS_ACTION_TESTAVERAGED, -1, -1, guidTestAveraged);
ACTION_TARGET(CTestAveraged, szTargetAnydoc );

ACTION_ARG_LIST(CTestAveraged)
	ARG_OBJECT( _T("Objects") )
END_ACTION_ARG_LIST();

CTestAveraged::CTestAveraged(void)
{
}

CTestAveraged::~CTestAveraged(void)
{
}

bool CTestAveraged::InvokeFilter()
{
	INamedDataObject2Ptr sptrObjects(GetDataArgument("Objects"));

	if(sptrObjects==0) return false;

	long nObjects=0;
	sptrObjects->GetChildsCount(&nObjects);

	CString strBad = "";
	long nBad = 0;
	double fAvgDevThreshold = ::GetValueDouble(::GetAppUnknown(), CGH_ROOT "\\TestAveraged", "AvgDevThreshold", 0.2 );

	// пробежимся по объектам
	POSITION pos=0; sptrObjects->GetFirstChildPosition(&pos);
	while( pos ) //по всем объектам
	{
		IUnknownPtr ptrObj;
		sptrObjects->GetNextChild(&pos, &ptrObj);
		IMeasureObjectPtr sptrObj(ptrObj);
		if(sptrObj==0) continue;

		IUnknownPtr ptrImg;
		sptrObj->GetImage(&ptrImg);
		IImage4Ptr sptrImg(ptrImg);
		if(sptrImg==0) continue;

		int cx1=0, cy1=0; // для мелкого Img
		sptrImg->GetSize( &cx1, &cy1 );
		_point ptOffset1(0,0); // для мелкого Img
		sptrImg->GetOffset(&ptOffset1);
		CRect rect1(ptOffset1, CSize(cx1,cy1));

		// возьмем info из NamedData листа
		GUID guidObj={0};
		{
			INumeredObjectPtr sptrNum(sptrObj);
			if(sptrNum!=0) sptrNum->GetKey(&guidObj);
		}
		BSTR	bstr = 0;
		::StringFromCLSID( guidObj, &bstr );
		_bstr_t bstrGuid = bstr;	
		::CoTaskMemFree( bstr );	bstr = 0;

		double avg_deviation = ::GetValueDouble(sptrObjects,
			CGH_ROOT "\\" szCghObjects "\\" + bstrGuid,
			szCghAvgDeviation, 0.0);

		if(avg_deviation>fAvgDevThreshold)
		{
			long nClass = get_object_class(ICalcObjectPtr(sptrObj));
			strBad = strBad + ", " + get_class_name(nClass);
			nBad++;
		}
	}
	
	if(nBad>0)
	{
		strBad.Delete(0,2);
		CString strMessage;
		CString strMessageFormat;
		if(! strMessageFormat.LoadString(IDS_ERRONEOUSAVERAGING) )
			strMessageFormat = "There is %u erroneous averaging - in classes %s.";
		strMessage.Format(strMessageFormat, unsigned(nBad), strBad.GetBuffer() );
		AfxMessageBox(strMessage, MB_OK | MB_APPLMODAL | MB_ICONSTOP);
	}
	::SetValue(::GetAppUnknown(), CGH_ROOT "\\TestAveraged\\Result", "Erroneous", long(nBad) );

	//------------------------------------------------------------------------------
	StartNotification(1000);
	Notify(999);
	FinishNotification();

	return true;
}
