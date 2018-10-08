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


void jpgErrorHandler(const MagickCore::ExceptionType severity
										 ,const char *reason,const char *description)
{
	AfxMessageBox(CString(reason)+' '+description);
}

MagickBooleanType jpgMonitorHandler(const char *msg,const MagickOffsetType curPos,
											 const MagickSizeType maxPos,void * client_data)
{
	if(jpgFilter == NULL) return MagickFalse;
	if(!jpgFilter->m_bEnableMonitoring) return MagickFalse;
	if (jpgFilter->m_bFirstTime)
	{
		jpgFilter->m_bFirstTime = false;
		jpgFilter->StartNotification((int)maxPos, 2);
	}
	jpgFilter->Notify((int)curPos);
	return MagickTrue;
}


void CJPGFileFilter::OnSetHandlers()
{
	//if(jpgFilter == NULL)
		jpgFilter = this;
	m_oldErrorHandle = SetErrorHandler(jpgErrorHandler);
	m_oldMonitorHandle = SetImageInfoProgressMonitor(&m_image_info, &jpgMonitorHandler, this);
}

void JPGErrorHandler(const MagickCore::ExceptionType severity
										 ,const char *reason,const char *description)
{
	AfxMessageBox(CString(reason)+' '+description);
}


MagickBooleanType JPGMonitorHandler(const char *msg,const MagickOffsetType curPos,
											 const MagickSizeType maxPos,void * client_data)
{
	CJPGFileFilter* jpgFilter=(CJPGFileFilter*)client_data;
	if (jpgFilter->m_bFirstTime)
	{
		jpgFilter->m_bFirstTime = false;
		jpgFilter->StartNotification((int)maxPos, 2);
	}
	jpgFilter->Notify((int)curPos);
	return MagickTrue;
}

/////////////////////////////////////////////////////////////////////////////
// CJPGFileFilter message handlers

bool CJPGFileFilter::ReadFile( const char *pszFileName )
{
	//CTimeTest timeTest(true, pszFileName, false);

	m_bFirstTime = true;
	GetImageInfo(&m_image_info);

	ErrorHandler oldErrorHandle = SetErrorHandler(JPGErrorHandler);
	MagickProgressMonitor oldMonitorHandle = 
		SetImageInfoProgressMonitor(&m_image_info, JPGMonitorHandler, this);

	Image* image;
//	ImageInfo image_info;
	strcpy(m_image_info.filename,pszFileName);
	ExceptionInfo exceptionInfo;
	image=ReadImage(&m_image_info, &exceptionInfo);

	SetImageInfoProgressMonitor(&m_image_info,oldMonitorHandle,NULL);

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
		INamedObject2Ptr sptrObj(punk);
		//sptrINamedObject2 sptrObj(punk);
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

	if( sptrI.CreateNew((int)image->columns, (int)image->rows , strColorSpace) != S_OK )
		return false;

	int numPanes = 0;
	numPanes = sptrI.GetColorsCount();
	color*pColor [numPanes];

	if (numPanes == 0)
	{
		SetErrorHandler(oldErrorHandle);
		FinishNotification();
		return false;
	}

	NextNotificationStage();

	for (int i = 0; i < image->rows; i++)
	{
		for(int j = 0; j < numPanes; j++)
		{
			pColor[j] = img.GetRow(i, j);
		}
		FillVTImageRow(image, i, pColor, numPanes);
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

	
	m_bFirstTime = true;

	ErrorHandler oldErrorHandle = SetErrorHandler(JPGErrorHandler);
	MagickProgressMonitor oldMonitorHandle 
		= SetImageInfoProgressMonitor(&m_image_info,JPGMonitorHandler,this);

	Image* image;
//	ImageInfo image_info;
	GetImageInfo(&m_image_info);
	strcpy(m_image_info.filename, "null:black");
	ExceptionInfo exceptionInfo;
	image=ReadImage(&m_image_info,&exceptionInfo);

	SetImageInfoProgressMonitor(&m_image_info,oldMonitorHandle,NULL);

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


	/*color* pColor1 = NULL;
	color* pColor2 = NULL;
	color* pColor3 = NULL;
	color* pColor4 = NULL;*/


	int numPanes = 0;
	numPanes = img.GetColorsCount();
	color*pColor [numPanes];

	if (numPanes == 0)
	{
		SetErrorHandler(oldErrorHandle);
		FinishNotification();
		return false;
	}

	NextNotificationStage();
	
	for (int i = 0; i < image->rows; i++)
	{
		for(int j = 0; j < numPanes; j++)
		{
			pColor[j] = img.GetRow(i, j);
		}
		FillMagickImageRow(image, i, pColor, numPanes);
		Notify(i);
	}
	

	strcpy(m_image_info.filename,pszFileName);
	strcpy(image->filename,pszFileName);
	WriteImage(&m_image_info, image, &exceptionInfo);

	SetErrorHandler(oldErrorHandle);
	
	FinishNotification();

	return true;
}
