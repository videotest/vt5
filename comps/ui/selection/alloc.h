#if !defined(AFX_ALLOC_H__INCLUDED_)
#define AFX_ALLOC_H__INCLUDED_

template        <class TYPE>
class _ptr_t
{
public:
        _ptr_t( int nCount = 0 )
        {
                ptr = 0;
                nsize = 0;
                alloc( nCount );
        }

        ~_ptr_t()
        {
                free();
        }

        void zero()
        {
                if( ptr )
                        memset( ptr, 0, nsize*sizeof( TYPE ) );
        }
        
        void alloc( int nCount )
        {
                free();

                nsize = nCount;
                if( nsize )ptr = new TYPE[nsize];
        }

        int size()
        {
                return nsize;
        }

        void free()
        {
                delete [] detach();
        }
        TYPE    *detach()
        {
                TYPE    *pret = ptr;
                ptr = 0;
                nsize = 0;
                return pret;
        }
        operator        TYPE*() {return ptr;}
public:
        TYPE    *ptr;
        int             nsize;
};

//макрос для переменных.
// пример: smart_alloc(buf,int,64) создаст переменные buf и smart_buf
#define smart_alloc(var,type,size) \
        _ptr_t<type> smart_##var(size); \
        type * var=smart_##var.ptr;

#endif // !defined(AFX_ALLOC_H__INCLUDED_)
