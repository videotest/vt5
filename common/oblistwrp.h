#ifndef __oblistwrp_h__
#define __oblistwrp_h__

#include "data5.h"
#include "measure5.h"
#include "object5.h"
/////////////////////////////////////////////////////////////////////////////////////////
//the wrapper class for measure object
class std_dll CObjectWrp : public IUnknownPtr
{
public:
	//unknown manipulation
	CObjectWrp( IUnknown *punkList = 0, bool bAddRef = true );
	void Attach( IUnknown *punkList, bool bAddRef = true );
public:
//attributes
	inline bool CheckState() const;			//return true if object sucesfully initialized
	IUnknown *GetImage();					//get the image from the object
public:
//operators
	void Draw( CDC &dc, IUnknown *punkVS );	//draw the object in the specified device context
	void SetImage( IUnknown *punkImage );	//setup the image to the object
	bool PtInObject( CPoint point );		//return true, if this point match object
protected:
	IMeasureObjectPtr	m_sptrO;
	sptrIDrawObject		m_sptrD;
};

/////////////////////////////////////////////////////////////////////////////////////////
//the wrapper class for objects list
class std_dll CObjectListWrp : public IUnknownPtr
{
public:
	//unknown manipulation
	CObjectListWrp( IUnknown *punkList = 0, bool bAddRef = true );
	virtual ~CObjectListWrp()
	{	Clear();	}
	virtual void Attach( IUnknown *punkList, bool bAddRef = true );
	virtual	void Clear();

public:
//attributes
	inline bool CheckState() const;			//return true if object sucesfully initialized

public:
	void Draw( CDC &dc, IUnknown *punkVS );//draw the object list in given context
public:
//operation with the object list
	POSITION Add( IUnknown *punkObject );	//add a new object
	void Remove( IUnknown *punkObject );	//remove a specified object
	void RemoveAt( POSITION posObject );	//remove an object at the specified position

	POSITION Find( IUnknown *punkObject );	//locate the object in the list
		
	long GetCount();						//return the count of the object
											

	POSITION GetFirstObjectPosition();		//get the first object position
	IUnknown *GetNextObject( POSITION &pos );//get the object at the specified position and move the POS

	POSITION GetLastObjectPosition();		//get the last object position
	IUnknown * GetPrevObject(POSITION & pos);//get the object at the specified position and move the POS

	void RemoveAll();						//remove all objects in the object list

	POSITION	GetCurPosition();			//get the active object position
	void SetCurPosition( POSITION pos );	//set it!
//implementation details
protected:
	
protected:
	sptrINamedDataObject2	m_sptrN;

};

/////////////////////////////////////////////////////////////////////////////////////////
//inlines
inline bool CObjectWrp::CheckState() const
{	return (*this) != 0;	}

inline bool CObjectListWrp::CheckState() const
{	return (*this) != 0 && m_sptrN != 0;	}

#endif //__oblistwrp_h__