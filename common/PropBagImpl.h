#ifndef __PropBagImpl_h__
#define __PropBagImpl_h__

#include "defs.h"
#include "\vt5\awin\misc_list.h"
#include "\vt5\awin\misc_ptr.h"
#include "PropBag.h"

class CNamedPropBagImpl //: public CImplBase
: public INamedPropBagSer
{
	//ENABLE_MULTYINTERFACE();
public:
	//BEGIN_INTERFACE_PART_EXPORT(NamedPropBag, INamedPropBagSer)
		com_call SetProperty( BSTR bstrName, VARIANT var );
		com_call GetProperty( BSTR bstrName, VARIANT* pvar );
		com_call GetFirstPropertyPos( long* plos );
		com_call GetNextProperty (BSTR* pbstrName, VARIANT* pvar, long* plos );
		com_call DeleteProperty (long lPos);
		com_call Load( IStream *pStream );
		com_call Store( IStream *pStream );
	//END_INTERFACE_PART(NamedPropBag)

protected:
	class CNamedProperty
	{
	public:
		static DWORD hash_f(BSTR s)
		{
			DWORD h=0;
			while(*s)
			{
				h = (h<<6) ^ (h>>(32-6)) ^ (*s);
				s++;
			}
			return h;
		}
		_bstr_t bstrName; //имя записи
		_variant_t var; //значение
		DWORD dwHash; //хэш - для быстрого поиска
		CNamedProperty()
		{
			//bstrName="";
			//var="";
			dwHash=0;
		};
		CNamedProperty(BSTR bstrName1, VARIANT var1)
		{
			bstrName=bstrName1;
			var=var1;
			// [vanek] - 09.04.2004
			if(var.vt == VT_I2)
				var.ChangeType( VT_I4 );

			dwHash=hash_f(bstrName1);
		}
	};
	virtual int GetPropertyPos( BSTR bstrName, LPOS* plos );
	virtual void OnChange() {};
private:
	_ptr_t<CNamedProperty> m_bag;
	bool Search(BSTR bstr, long *ppos); //найти элемент в массиве;
	// Если найден - true, его номер в ppos
	// Если не найден - false, в ppos - номер первого большего
	int CompareItem(LPOS lpos, DWORD h2, BSTR bstr2);		
};

#include "PropBagImpl.inl"

#endif //__PropBagImpl_h__

