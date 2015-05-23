#if !defined(__VideoNameObject_H__)
#define __VideoNameObject_H__


class CVideoNameObject : public  CDataObjectBase
//						public  CDrawObjectImpl
{
	PROVIDE_GUID_INFO()

	ENABLE_MULTYINTERFACE();
	DECLARE_DYNCREATE(CVideoNameObject);
	GUARD_DECLARE_OLECREATE(CVideoNameObject);
public:
	CVideoNameObject();
	~CVideoNameObject();

	static const char *c_szType;

//overrided virtuals
//	virtual DWORD GetNamedObjectFlags();
//	virtual bool SerializeObject(CArchive &ar);
//	virtual void DoDraw( CDC &dc );
//	virtual CRect DoGetRect();
//	void UpdateObject();
//	void _SetRectByText();

	DECLARE_INTERFACE_MAP();
protected:
	virtual bool GetTextParams( CString &strParams );
	virtual bool SetTextParams( const char *pszParams );
};


#endif