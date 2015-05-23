#include "StdAfx.h"
#include "ObjUser.h"

#define ADD_IDD		1000

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CObjUser, CObjMeasure, VERSIONABLE_SCHEMA|3)
IMPLEMENT_SERIAL(CObjFree, CObjUser, VERSIONABLE_SCHEMA|3)
IMPLEMENT_SERIAL(CObjCircle, CObjUser, VERSIONABLE_SCHEMA|3)
IMPLEMENT_SERIAL(CObjRect, CObjUser, VERSIONABLE_SCHEMA|3)

void CObjUser::DoMeasure()
{
	ASSERT(FALSE); // Not implemented here
}

CObjNative *CObjUser::Clone()
{
	CObjUser *pNew = (CObjUser *)CObjMeasure::Clone();
	ASSERT_KINDOF(CObjUser,pNew);
	return (CObjNative *)pNew;
}

CObjNative *CObjFree::Clone()
{
	CObjFree *pNew = (CObjFree *)CObjMeasure::Clone();
	ASSERT_KINDOF(CObjFree,pNew);
	return (CObjNative *)pNew;
}

CObjNative *CObjCircle::Clone()
{
	CObjCircle *pNew = (CObjCircle *)CObjMeasure::Clone();
	ASSERT_KINDOF(CObjCircle,pNew);
	return (CObjNative *)pNew;
}

CObjRect::CObjRect()
{
	m_RectHandle.SetFract(0.5);
}

CObjNative *CObjRect::Clone()
{
	CObjRect *pNew = (CObjRect *)CObjUser::Clone();
	ASSERT_KINDOF(CObjRect,pNew);
	return (CObjNative *)pNew;
}

void CObjRect::CreateBin()
{
	CRect rc(GetRect());
	CObjUser::CreateBIN(CSize(rc.Width(), rc.Height()));
	for( int y = 0; y < rc.Height(); y++)
	{
		for( int x = 0; x < rc.Width(); x++)
		{
			(*m_pimgBin)[y][x] = BYTE_RAWBODY;
		}
	}
}

void CObjRect::DoMeasure()
{
	CObjUser::DoMeasure();
	ASSERT(FALSE); // Not implemented here
}


