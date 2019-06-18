// ImageFileFilterBase.cpp: implementation of the CImageFileFilterBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageFileFilterBase.h"
#include "resource.h"


//image properties names
/*const char* szWidth		= "Width";
const char* szHeight	= "Height";
const char* szColors	= "Colors";
const char* szDepth		= "Depth";
const char* szNumPages	= "NumPages";
const char* szCompress	= "Compression";
*/





//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CImageFileFilterBase::CImageFileFilterBase()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nColors = 0;
	m_nDepth = 0;
	m_bEnableMonitoring = true;
	m_strCompression = "";
}

CImageFileFilterBase::~CImageFileFilterBase()
{
}


bool CImageFileFilterBase::ReadFile( const char *pszFileName )
{
	Image* image = 0;
	color** pColor = 0;
	IUnknown* punk = 0;
	try
	{
	FinishNotification();

	//CTimeTest timeTest(true, pszFileName, false);
	//timeTest.m_bEnableFileOutput = true;

	m_bFirstTime = true;
	m_bEnableMonitoring = true;

	OnSetHandlers();

	ImageInfo image_info;
	GetImageInfo(&image_info);
	strcpy(image_info.filename,pszFileName);
	image=ReadImage(&image_info);

	SetMonitorHandler(m_oldMonitorHandle);

	if (!image)
	{
		SetErrorHandler(m_oldErrorHandle);
		FinishNotification();
		return false;
	}

	const char* strColorSpace;
	if (image->colorspace == RGBColorspace) 
		strColorSpace = "RGB";
	else if (image->colorspace == GRAYColorspace) 
		strColorSpace = "GRAY";
	else if (image->colorspace == YUVColorspace) 
		strColorSpace = "YUV";
	
	punk = CreateNamedObject( GetDataObjectType(0) );

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
		SetErrorHandler(m_oldErrorHandle);
		FinishNotification();
		if(image)
		{
			DestroyImage(image);
			image = 0;
		};
		image = 0;
		return false;
	}

	if( sptrI.CreateNew(image->columns, image->rows , strColorSpace) != S_OK )
		return false;

	int numPanes = 0;
	numPanes = sptrI.GetColorsCount();

	if (numPanes == 0)
	{
		if(punk)
		{
			punk->Release();
			punk = 0;	
		}
		SetErrorHandler(m_oldErrorHandle);
		FinishNotification();
		if(image)
		{
			DestroyImage(image);
			image = 0;
		};
		return false;
	}

	m_nWidth = image->columns;
	m_nHeight = image->rows;

	pColor = new color*[numPanes];

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
	//m_nColors = GetNumberColors(image,NULL); //real number colors
	m_nColors = 1<<m_nDepth; //fake
	m_nNumPages = -1;


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


	NextNotificationStage();

	for (int i = 0; i < image->rows; i++)
	{
		for(int k = 0; k < numPanes; k++)
		{
			pColor[k] = sptrI.GetRow(i, k);
		}
		FillVTImageRow(image, i, pColor, numPanes);
		Notify(i);
		if(m_bNeedToTerminate)
		{
			//TRACE("@@@@@@We terminate %s\n", pszFileName);
			break;
		}
	}
	
	SetErrorHandler(m_oldErrorHandle);

	if(pColor)
	{
		delete pColor;
		pColor = 0;
	}
	if(image)
	{
		DestroyImage(image);
		image = 0;
	};
	if(punk)
	{
		punk->Release();
		punk = 0;	
	}
	//TRACE("@@@@@@In filter all clear for %s\n", pszFileName);
	if(m_bNeedToTerminate)
	{
		return false;
	}
	FinishNotification();
	//TRACE("@@@@@@We normal proceed %s\n", pszFileName);
	return true;
	}
	catch(CException* e)
	{
		//TRACE("@@@@@@We kill %s\n", pszFileName);
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
		if(punk)
		{
			punk->Release();
			punk = 0;	
		}
		//e->ReportError();
		e->Delete();
		return false;
	}
	
}

bool CImageFileFilterBase::WriteFile( const char *pszFileName )
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

	
	m_bFirstTime = true;
	m_bEnableMonitoring = false;

	OnSetHandlers();
	
	ImageInfo image_info;
	GetImageInfo(&image_info);
	strcpy(image_info.filename, "null:black");
	image=ReadImage(&image_info);

	SetMonitorHandler(m_oldMonitorHandle);

	if (!image)
	{
		if(punk)
		{
			punk->Release();
			punk = 0;	
		}
		SetErrorHandler(m_oldErrorHandle);
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
		SetErrorHandler(m_oldErrorHandle);
		FinishNotification();
		if(punk)
		{
			punk->Release();
			punk = 0;	
		}
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
		for(int k = 0; k < numPanes; k++)
		{
			pColor[k] = img.GetRow(i, k);
		}
		FillMagickImageRow(image, i, pColor, numPanes);
		
		Notify(i);
		if(m_bNeedToTerminate)
			break;
	}
	

	strcpy(image_info.filename,pszFileName);
	strcpy(image->filename,pszFileName);
	WriteImage(&image_info, image);

	SetErrorHandler(m_oldErrorHandle);
	
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
	if(image)
	{
		DestroyImage(image);
		image = 0;
	};
	if(m_bNeedToTerminate)
		return false;
	FinishNotification();
	return true;
	}
	catch(CException* e)
	{
		if(pColor)
		{	
			delete pColor;
			pColor = 0;
		}
		//FinishNotification();
		if(punk)
		{
			punk->Release();
			punk = 0;	
		}
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


int  CImageFileFilterBase::GetPropertyCount()
{
	return 6;
}

void CImageFileFilterBase::GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx)
{

	CString strName;
	DWORD dwFlags = 0;
	DWORD dwGroup = 0;
	_variant_t var;
	switch (nNum)
	{
	case 0:
		dwGroup = 1;
		var = (long)m_nWidth;
		strName.LoadString(IDS_WIDTH);
		break;
	case 1:
		dwGroup = 1;
		var = (long)m_nHeight;
		strName.LoadString(IDS_HEIGHT);
		break;
	case 2:
		dwGroup = 2;
		var = (long)m_nDepth;
		dwFlags = ffpfTextAfterProp;
		strName.LoadString(IDS_DEPTH);
		break;
	case 3:
		dwGroup = 2;
		var = (long)m_nColors;
		dwFlags = ffpfTextAfterProp|ffpfNeedBrackets;
		strName.LoadString(IDS_COLORS);
		break;
	case 4:
		var = m_strCompression;
		strName.LoadString(IDS_COMPRESS);
		break;
	case 5:
		var = (long)m_nNumPages;
		strName.LoadString(IDS_PAGES);
		break;
	}
	if(pbstrName)
		*pbstrName = strName.AllocSysString();
	if(pGroupIdx)
		*pGroupIdx = dwGroup;
	if(pdwFlags)
		*pdwFlags = dwFlags;
	if(pvarVal)
		*pvarVal = var.Detach();
}