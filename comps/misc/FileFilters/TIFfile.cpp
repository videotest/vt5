// TIFile.cpp : implementation file
//

#include "stdafx.h"
#include "FileFilters.h"
#include "ImageFileFilterBase.h"
#include "TIFfile.h"
#include "resource.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




/////////////////////////////////////////////////////////////////////////////
// CTIFile

IMPLEMENT_DYNCREATE(CTIFileFilter, CCmdTargetEx)


// {7FCAC27A-69A6-11D3-A4EC-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CTIFileFilter, "FileFilters.TIFileFilter", 0x7fcac27a, 0x69a6, 0x11d3, 0xa4, 0xec, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87)

CTIFileFilter* tifFilter;
bool tifFirstTime;


void TIFErrorHandler(const unsigned int error,const char *message, const char *qualifier)
{
  DestroyDelegateInfo();
  AfxMessageBox(message);
}

void TIFMonitorHandler(const char *msg,const unsigned int curPos,const unsigned int maxPos)
{
	if(tifFilter == NULL) return;
	if(!tifFilter->m_bEnableMonitoring) return;
	if (tifFirstTime)
	{
		tifFirstTime = false;
		tifFilter->StartNotification(maxPos);
	}
	if (curPos > 0)
		tifFilter->Notify(curPos);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTIFileFilter::CTIFileFilter()
{
	_OleLockApp( this );
	m_strDocumentTemplate = _T("Image");
	m_strFilterName.LoadString( IDS_TIF_FILTERNAME );
	m_strFilterExt = _T(".tif\n.tiff");
	AddDataObjectType(szArgumentTypeImage);
	tifFilter = this;
}

CTIFileFilter::~CTIFileFilter()
{
	_OleUnlockApp( this );
	tifFilter = NULL;
}


bool CTIFileFilter::ReadFile( const char *pszFileName )
{
	Image* image = 0;
	Image* tmpImg = 0;
	color** pColor = 0;
	IUnknown	*punk = 0;
	try
	{
	FinishNotification();

	CTimeTest timeTest(true, pszFileName, false);
	timeTest.m_bEnableFileOutput = true;

	//if(tifFilter == NULL)
		tifFilter = this;

	tifFirstTime = true;
	m_bEnableMonitoring = true;

	ErrorHandler oldErrorHandle = SetErrorHandler(TIFErrorHandler);
	MonitorHandler oldMonitorHandle = SetMonitorHandler(TIFMonitorHandler);

	ImageInfo image_info;
	GetImageInfo(&image_info);
	strcpy(image_info.filename,pszFileName);
	image=ReadImage(&image_info);

	SetMonitorHandler(oldMonitorHandle);
	FinishNotification();

	if (!image)
		return false;

	const char* strColorSpace;
	if (image->colorspace == RGBColorspace) 
		strColorSpace = "RGB";
	else if (image->colorspace == GRAYColorspace) 
		strColorSpace = "GRAY";
	else if (image->colorspace == YUVColorspace) 
		strColorSpace = "YUV";
	
	
	int idx = 0;
	tmpImg = image;//CloneImage(image, image->columns, image->rows, False);
	while (tmpImg)
	{
		idx++;
		tmpImg = tmpImg->next;
	}
	tmpImg = 0;
	tmpImg = image;
	if (!idx) idx++;

	m_nWidth = image->columns;
	m_nHeight = image->rows;
	if(!IsPseudoClass(image) && !IsGrayImage(image))
    {
         m_nDepth=image->matte ? 32 : 24;
    }
    else
    {
		m_nDepth = 8;
        if (IsMonochromeImage(image))
        {
           m_nDepth = 1;
        }
    }

	switch(image->compression)
	{
	case NoCompression:
		m_strCompression.LoadString(IDS_NoCompression);
		break;
	case BZipCompression:
		m_strCompression.LoadString(IDS_BZipCompression);
		break;
	case FaxCompression:
		m_strCompression.LoadString(IDS_FaxCompression);
		break;
	case Group4Compression:
		m_strCompression.LoadString(IDS_Group4Compression);
		break;
	case JPEGCompression:
		m_strCompression.LoadString(IDS_JPEGCompression);
		break;
	case LZWCompression:
		m_strCompression.LoadString(IDS_LZWCompression);
		break;
	case RunlengthEncodedCompression:
		m_strCompression.LoadString(IDS_RunlengthEncodedCompression);
		break;
	case ZipCompression:
		m_strCompression.LoadString(IDS_ZipCompression);
		break;
	default:
		m_strCompression.LoadString(IDS_UndefinedCompression);
		break;
	}

	//m_nColors = GetNumberColors(image,NULL); //real number colors
	m_nColors = 1<<m_nDepth; //fake
	if(idx == 1)
		m_nNumPages = -1;
	else
		m_nNumPages = idx;

	StartNotification(image->rows, idx, 1);

	idx = 1;
	do
	{

		punk = CreateNamedObject( GetDataObjectType(0) );

		// get object name from path and check it's exists
		CString	strObjName = ::GetObjectNameFromPath(pszFileName);
		if (idx > 1)
		{
			CString strTmp = strObjName;
			strObjName.Format("%s_page%d", (const char *)strTmp, idx);
		}
		idx++;
		_bstr_t bstrName(strObjName);
		long	NameExists = 0;
	
		m_sptrINamedData->NameExists(bstrName, &NameExists);
		// if new name not exists allready
		if (!NameExists && punk)
		{
			// set this name to object
			INamedObject2Ptr sptrObj(punk);
			sptrObj->SetName(bstrName);
		}
	
		CImageWrp	sptrI( punk );
		if(punk)
		{
			punk->Release();
			punk = 0;	
		}
		if (sptrI == 0)
		{
			SetErrorHandler(oldErrorHandle);
			FinishNotification();
			if(tmpImg)
			{
				DestroyImage(tmpImg);
				tmpImg = 0;
			};
			return false;
		}
	
		if( !sptrI.CreateNew(image->columns, image->rows , strColorSpace) != S_OK )
			return false;
	
		int numPanes = 0;
		numPanes = sptrI.GetColorsCount();

		if (numPanes == 0)
		{
			SetErrorHandler(oldErrorHandle);
			FinishNotification();
			if(tmpImg)
			{
				DestroyImage(tmpImg);
				tmpImg = 0;
			};
			if(punk)
			{
				punk->Release();
				punk = 0;	
			}
			return false;
		}

		pColor = new color*[numPanes];

		for (int i = 0; i < image->rows; i++)
		{
			for(int j = 0; j < numPanes; j++)
			{
				pColor[j] = sptrI.GetRow(i, j);
			}
		
			FillVTImageRow(image, i, pColor, numPanes);
			Notify(i);
			if(m_bNeedToTerminate)
				break;
		}

		delete pColor;
		pColor = 0;

		//tmpImg = image;
		image = image->next;
		//DestroyImage(tmpImg);
		//tmpImg = 0;

		if(m_bNeedToTerminate)
			break;

		if (image)
			NextNotificationStage();
	}while(image);

	SetErrorHandler(oldErrorHandle);
	
	if(tmpImg)
	{
		DestroyImage(tmpImg);
		tmpImg = 0;
	}

	if(punk)
	{
		punk->Release();
		punk = 0;	
	}
	if(m_bNeedToTerminate)
	{
		//TRACE("@@@@@@We kill %s\n", pszFileName);
		return false;
	}
	FinishNotification();
	//TRACE("@@@@@@We normal proceed %s\n", pszFileName);
	return true;
	}
	catch(CException* e)
	{
		//TRACE("@@@@@@We kill %s\n", pszFileName);
		if(punk)
		{
			punk->Release();
			punk = 0;	
		}
		if(pColor)
		{
			delete pColor;
			pColor = 0;
		}
		FinishNotification();
		if(tmpImg)
		{
			DestroyImage(tmpImg);
			tmpImg = 0;
		}
		e->ReportError();
		e->Delete();
		return false;
	}
}

bool CTIFileFilter::WriteFile( const char *pszFileName )
{
	Image* image = 0;
	color** pColor = 0;
	IUnknown* punk = 0;
	try
	{

	FinishNotification();
	// convert object type to BSTR
	_bstr_t bstrType( GetDataObjectType(0) );

	// get active object in which type == m_strObjKind
	
	if (!(m_bObjectExists && m_sptrAttachedObject != NULL) && m_sptrIDataContext != NULL)
	{
		m_sptrIDataContext->GetActiveObject( bstrType, &punk );

		INamedDataObject2Ptr	ptrN( punk );
		if( ptrN != 0 )
		{
			IUnknown	*punkP = 0;
			ptrN->GetParent( &punkP );

			if( punkP )
			{
				punk->Release();
				punk = punkP;
			}
		}
	}
	else
	{
		punk = (IUnknown*)m_sptrAttachedObject.GetInterfacePtr();
		punk->AddRef();
	}

	if( !punk )
		return false;

	CImageWrp	img( punk, false );

	//if(tifFilter == NULL)
		tifFilter = this;

	tifFirstTime = true;
	m_bEnableMonitoring = false;

	ErrorHandler oldErrorHandle = SetErrorHandler(TIFErrorHandler);
	MonitorHandler oldMonitorHandle = SetMonitorHandler(TIFMonitorHandler);

	ImageInfo image_info;
	GetImageInfo(&image_info);
	strcpy(image_info.filename, "null:black");
	image=ReadImage(&image_info);

	SetMonitorHandler(oldMonitorHandle);

	if (!image)
	{
		if(punk)
		{
			punk->Release();
			punk = 0;	
		}
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


	int numPanes = 0;
	numPanes = img.GetColorsCount();

	if (numPanes == 0)
	{
		if(punk)
		{
			punk->Release();
			punk = 0;	
		}
		SetErrorHandler(oldErrorHandle);
		FinishNotification();
		if(image)
		{
			DestroyImage(image);
			image = 0;
		};
		return false;
	}

	pColor = new color*[numPanes];

	//NextNotificationStage();
	StartNotification(image->rows, 1, 1);
	
	for (int i = 0; i < image->rows; i++)
	{

		for(int j = 0; j < numPanes; j++)
		{
			pColor[j] = img.GetRow(i, j);
		}
		
		FillMagickImageRow(image, i, pColor, numPanes);
		Notify(i);
		if(m_bNeedToTerminate)
			break;
	}

	strcpy(image_info.filename,pszFileName);
	strcpy(image->filename,pszFileName);
	WriteImage(&image_info, image);

	SetErrorHandler(oldErrorHandle);

	if(image)
	{
		DestroyImage(image);
		image = 0;
	};
	if(pColor)
	{
		delete  pColor;
		pColor = 0;
	}

	if(punk)
	{
		punk->Release();
		punk = 0;	
	}
	if(m_bNeedToTerminate)
		return false;

	FinishNotification();
	return true;
	}
	catch(CException* e)
	{
		if(punk)
		{
			punk->Release();
			punk = 0;	
		}
		if(pColor)
		{
			delete pColor;
			pColor = 0;
		}
		FinishNotification();
		if(image)
		{
			DestroyImage(image);
			image = 0;
		};
		e->ReportError();
		e->Delete();
		return false;
	}
}