// JPGFileFilter.cpp : implementation file
//

#include "stdafx.h"
#include "FileFilters.h"
#include "ImageFileFilterBase.h"
#include "JPGfile.h"
#include "resource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJPGFileFilter

IMPLEMENT_DYNCREATE(CJPGFileFilter, CCmdTargetEx)


// {A1618774-6AA5-11D3-A4ED-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CJPGFileFilter, "FileFilters.JPGFileFilter", 0xa1618774, 0x6aa5, 0x11d3, 0xa4, 0xed, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87)

CJPGFileFilter* jpgFilter;

CJPGFileFilter::CJPGFileFilter()
{
	_OleLockApp( this );

	m_strDocumentTemplate = _T("Image");
	m_strFilterName.LoadString( IDS_JPG_FILTERNAME );
	m_strFilterExt = _T(".jpg\n.jpeg\n.jpe");
	AddDataObjectType(szArgumentTypeImage);
	jpgFilter = this;
}

CJPGFileFilter::~CJPGFileFilter()
{
	_OleUnlockApp( this );
	jpgFilter = NULL;
}

void jpgErrorHandler(const unsigned int error,const char *message, const char *qualifier)
{
  DestroyDelegateInfo();
  AfxMessageBox(message);
}


void jpgMonitorHandler(const char *msg,const unsigned int curPos,const unsigned int maxPos)
{
	if(jpgFilter == NULL) return;
	if(!jpgFilter->m_bEnableMonitoring) return;
	if (jpgFilter->m_bFirstTime)
	{
		jpgFilter->m_bFirstTime = false;
		jpgFilter->StartNotification(maxPos, 2);
	}
	jpgFilter->Notify(curPos);
}


void CJPGFileFilter::OnSetHandlers()
{
	//if(jpgFilter == NULL)
		jpgFilter = this;
	m_oldErrorHandle = SetErrorHandler(jpgErrorHandler);
	m_oldMonitorHandle = SetMonitorHandler(jpgMonitorHandler);
}

/*
void JPGErrorHandler(const unsigned int error,const char *message, const char *qualifier)
{
  DestroyDelegateInfo();
  AfxMessageBox(message);
}


void JPGMonitorHandler(const char *msg,const unsigned int curPos,const unsigned int maxPos)
{
	if (jpgFirstTime)
	{
		jpgFirstTime = false;
		jpgFilter->StartNotification(maxPos, 2);
	}
	jpgFilter->Notify(curPos);
}

/////////////////////////////////////////////////////////////////////////////
// CJPGFileFilter message handlers
/*
bool CJPGFileFilter::ReadFile( const char *pszFileName )
{
	//CTimeTest timeTest(true, pszFileName, false);

	jpgFirstTime = true;

	ErrorHandler oldErrorHandle = SetErrorHandler(JPGErrorHandler);
	MonitorHandler oldMonitorHandle = SetMonitorHandler(JPGMonitorHandler);

	Image* image;
	ImageInfo image_info;
	GetImageInfo(&image_info);
	strcpy(image_info.filename,pszFileName);
	image=ReadImage(&image_info);

	SetMonitorHandler(oldMonitorHandle);

	if (!image)
	{
		SetErrorHandler(oldErrorHandle);
		FinishNotification();
		return false;
	}

	
	//RGBTransformImage(image, YUVColorspace);
	//image->colorspace = YUVColorspace;
	//TransformRGBImage(image, RGBColorspace);
	//image->colorspace = RGBColorspace;
	//image->colorspace = YUVColorspace;
	const char* strColorSpace;
	if (image->colorspace == RGBColorspace) 
		strColorSpace = "RGB";
	else if (image->colorspace == GRAYColorspace) 
		strColorSpace = "GRAY";
	else if (image->colorspace == YUVColorspace) 
		strColorSpace = "YUV";
	

	color* pColor1 = NULL;
	color* pColor2 = NULL;
	color* pColor3 = NULL;
	color* pColor4 = NULL;

	IUnknown	*punk = CreateNamedObject( GetDataObjectType(0) );

	// get object name from path and check it's exists
	CString	strObjName = ::GetObjectNameFromPath(pszFileName);
	_bstr_t bstrName(strObjName);
	long	NameExists = 0;
	
	m_sptrINamedData->NameExists(bstrName, &NameExists);
	// if new name not exists allready
	if (!NameExists && punk)
	{
		// set this name to object
		sptrINamedObject2 sptrObj(punk);
		sptrObj->SetName(bstrName);
	}
	
	CImageWrp	sptrI( punk );
	punk->Release();
	if (sptrI == 0)
	{
		SetErrorHandler(oldErrorHandle);
		FinishNotification();
		return false;
	}

	if( sptrI.CreateNew(image->columns, image->rows , strColorSpace) != S_OK )
		return false;

	int numPanes = 0;
	numPanes = sptrI.GetColorsCount();

	if (numPanes == 0)
	{
		SetErrorHandler(oldErrorHandle);
		FinishNotification();
		return false;
	}

	NextNotificationStage();

	for (int i = 0; i < image->rows; i++)
	{
		if (numPanes == 1)
			pColor1 = sptrI.GetRow(i, 0);
		else if (numPanes == 3)
		{
			pColor1 = sptrI.GetRow(i, 0);
			pColor2 = sptrI.GetRow(i, 1);
			pColor3 = sptrI.GetRow(i, 2);
		}	
		else if (numPanes == 4)
		{
			pColor1 = sptrI.GetRow(i, 0);
			pColor2 = sptrI.GetRow(i, 1);
			pColor3 = sptrI.GetRow(i, 2);
			pColor4 = sptrI.GetRow(i, 3);
		}
	
		FillVTImageRow(image, i, pColor1, pColor2, pColor3, pColor4);
		Notify(i);
	}
	
	SetErrorHandler(oldErrorHandle);

	FinishNotification();

	
	return true;
}

bool CJPGFileFilter::WriteFile( const char *pszFileName )
{
	// convert object type to BSTR
	_bstr_t bstrType( GetDataObjectType(0) );

	// get active object in which type == m_strObjKind
	IUnknown* punk = NULL;

	if (!(m_bObjectExists && m_sptrAttachedObject != NULL) && m_sptrIDataContext != NULL)
		m_sptrIDataContext->GetActiveObject( bstrType, &punk );
	else
	{
		punk = (IUnknown*)m_sptrAttachedObject.GetInterfacePtr();
		punk->AddRef();
	}

	if( !punk )
		return false;

	CImageWrp	img( punk, false );

	
	jpgFirstTime = true;

	ErrorHandler oldErrorHandle = SetErrorHandler(JPGErrorHandler);
	MonitorHandler oldMonitorHandle = SetMonitorHandler(JPGMonitorHandler);

	Image* image;
	ImageInfo image_info;
	GetImageInfo(&image_info);
	strcpy(image_info.filename, "null:black");
	image=ReadImage(&image_info);

	SetMonitorHandler(oldMonitorHandle);

	if (!image)
	{
		SetErrorHandler(oldErrorHandle);
		FinishNotification();
		return false;
	}
	
	CString strSize;
	int y = img.GetWidth();
	image->columns = y;
	image->rows = img.GetHeight();

	

	CString strColorSpace;
	if (img.GetColorConvertor() == "RGB") 
		image->colorspace = RGBColorspace;
	else if (img.GetColorConvertor() == "GRAY") 
		image->colorspace = GRAYColorspace;
	else if (img.GetColorConvertor() == "YUV") 
		image->colorspace = YUVColorspace;


	color* pColor1 = NULL;
	color* pColor2 = NULL;
	color* pColor3 = NULL;
	color* pColor4 = NULL;


	int numPanes = 0;
	numPanes = img.GetColorsCount();

	if (numPanes == 0)
	{
		SetErrorHandler(oldErrorHandle);
		FinishNotification();
		return false;
	}

	NextNotificationStage();
	
	for (int i = 0; i < image->rows; i++)
	{
		if (numPanes == 1)
			pColor1 = img.GetRow(i, 0);
		else if (numPanes == 3)
		{
			pColor1 = img.GetRow(i, 0);
			pColor2 = img.GetRow(i, 1);
			pColor3 = img.GetRow(i, 2);
		}	
		else if (numPanes == 4)
		{
			pColor1 = img.GetRow(i, 0);
			pColor2 = img.GetRow(i, 1);
			pColor3 = img.GetRow(i, 2);
			pColor4 = img.GetRow(i, 3);
		}
	
		FillMagickImageRow(image, i, pColor1, pColor2, pColor3, pColor4);
		Notify(i);
	}
	

	strcpy(image_info.filename,pszFileName);
	strcpy(image->filename,pszFileName);
	WriteImage(&image_info, image);

	SetErrorHandler(oldErrorHandle);
	
	FinishNotification();

	return true;
}
*/