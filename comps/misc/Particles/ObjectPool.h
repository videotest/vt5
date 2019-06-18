#if !defined(AFX_OBJECTPOOL_H__0007BF75_3348_4084_A767_24C6B948FDB9__INCLUDED_)
#define AFX_OBJECTPOOL_H__0007BF75_3348_4084_A767_24C6B948FDB9__INCLUDED_

#include "image5.h"
#include "data5.h"
#include "misc_list.h"
#include "misc_classes.h"
#include "measure5.h"
#include "action_undo.h"

class CObjectPool:
public CDocHelper
{
public:
	CObjectPool();
	virtual ~CObjectPool();
	void StartProcessing(IUnknown *list, IUnknown *image, int cx, int cy); // начать выбор объектов из листа	(сразу удалив неформатные)
	INamedDataObject2Ptr GetNextObject();
	INamedDataObject2Ptr GetObject(_rect &rect);
	void StopProcessing(); // закончить выбор объектов и удалить лишние
	bool CorrectRect(_rect &rect); 
private:
	INamedDataObject2Ptr m_List;
	IImage3Ptr m_Image;
	long m_lPos;
	int m_cx, m_cy; //predefined size for objects
	_list_t2 <IUnknown*, FreeReleaseUnknown> m_Objects;
};


#endif // !defined(AFX_OBJECTPOOL_H__0007BF75_3348_4084_A767_24C6B948FDB9__INCLUDED_)
