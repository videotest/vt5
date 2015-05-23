/********************************************************************
Этот модуль распространяется с библиотекой
********************************************************************/

#ifndef AP_H
#define AP_H

#include <stdlib.h>
#include <math.h>

/********************************************************************
Этот символ, если определен, отключает проверку границ массивов.

Если он определен, то при обращении к элементу массива для чтения или
записи НЕ производится проверка корректности переданного индекса.

Если  символ  не  определен,  то  при  обращении к элементу массива с 
неверным номером выбрасывается исключение ap_error

Проверка границ массива делает программу более надежной, но замедляет
работу.
********************************************************************/
#define NO_AP_ASSERT


/********************************************************************
Этот символ используется для отладочных целей.

Не определяйте его и не убирайте комментарии!
********************************************************************/
//#define UNSAFE_MEM_COPY


/********************************************************************
Пространство имен стандартной библиотеки AlgoPascal
********************************************************************/
namespace ap
{


/********************************************************************
КЛАСС ИСКЛЮЧЕНИЯ, КОТОРОЕ ВЫБРАСЫВАЕТСЯ ПРИ РАЗЛИЧНЫХ ОШИБКАХ.

Текущая версия не позволяет определить причину, по которой исключение
было сгенерировано.
********************************************************************/
class ap_error
{
public:
    static void make_assertion(bool bClause)
        { if(!bClause) throw ap_error(); };
private:
};                      


/********************************************************************
ОПЕРАЦИЯ ЛОГИЧЕСКОЕ ИСКЛЮЧАЮЩЕЕ ИЛИ
********************************************************************/
static inline bool XOR(bool lhs, bool rhs)
{
    return (lhs && !rhs) || ( !lhs && rhs);
}


/********************************************************************
КЛАСС-ШАБЛОН ОДНОМЕРНОГО ДИНАМИЧЕСКОГО МАССИВА


ОПИСАНИЕ ЧЛЕНОВ КЛАССА:

---------------------------------------------------------------------
template_1d_array()

Создание пустого массива.

---------------------------------------------------------------------
~template_1d_array()

Удаление массива. При этом освобождается выделенная под массив память

---------------------------------------------------------------------
template_1d_array(const template_1d_array &rhs)

Создание копии массива. При этом выделяется отдельная область памяти,
в которую копируется содержимое массива-источника

---------------------------------------------------------------------
const template_1d_array& operator=(const template_1d_array &rhs)

Присваивание массива. При этом содержимое массива-приемника удаляется
и освобождается  выделенная под него память,  затем заново выделяется 
отдельная область памяти, в которую копируется содержимое источника.

---------------------------------------------------------------------
      T& operator()(int i)
const T& operator()(int i) const

Обращение к элементу массива с номером i

---------------------------------------------------------------------
void setbounds( int iLow, int iHigh )

Выделение памяти  под  массив.  При  этом  старое  содержимое массива
удаляется  и освобождается  выделенная под него память,  затем заново 
выделяется отдельная область памяти размера iHigh-iLow+1 элементов.

Нумерация элементов в новом массива начинается с iLow и заканчивается
iHigh. Содержимое нового массива не определено.

---------------------------------------------------------------------
void setcontent( int iLow, int iHigh, const T *pContent )

Метод  аналогичен  методу  setbounds()  за тем исключением, что после 
выделения памяти в неё копируется содержимое массива pContent[].

---------------------------------------------------------------------
      T* getcontent()
const T* getcontent() const

Метод позволяет получить указатель на содержимое массива. Данные,  на
которые указывает возвращенный указатель, можно изменять, и при  этом
изменится содержимое массива.

---------------------------------------------------------------------
int getlowbound()
int gethighbound()

Методы используются для  получения  информации  о  нижней  и  верхней
границах массива.
********************************************************************/
template<class T>
class template_1d_array
{
public:
    template_1d_array()
    {
        m_Vec=0;
        m_iVecSize = 0;
    };

    ~template_1d_array()
    {
        if(m_Vec)
            delete[] m_Vec;
    };

    template_1d_array(const template_1d_array &rhs)
    {
        m_iVecSize = rhs.m_iVecSize;
        m_iLow = rhs.m_iLow;
        m_iHigh = rhs.m_iHigh;
        if(rhs.m_Vec)
        {
            m_Vec = new T[m_iVecSize];
            #ifndef UNSAFE_MEM_COPY
            for(int i=0; i<m_iVecSize; i++)
                m_Vec[i] = rhs.m_Vec[i];
            #else
            memcpy(m_Vec, rhs.m_Vec, m_iVecSize*sizeof(T));
            #endif
        }
        else
            m_Vec=0;
    };

    const template_1d_array& operator=(const template_1d_array &rhs)
    {
        if( this==&rhs )
            return *this;

        m_iLow = rhs.m_iLow;
        m_iHigh = rhs.m_iHigh;
        m_iVecSize = rhs.m_iVecSize;
        if(m_Vec)
            delete[] m_Vec;
        if(rhs.m_Vec)
        {
            m_Vec = new T[m_iVecSize];
            #ifndef UNSAFE_MEM_COPY
            for(int i=0; i<m_iVecSize; i++)
                m_Vec[i] = rhs.m_Vec[i];
            #else
            memcpy(m_Vec, rhs.m_Vec, m_iVecSize*sizeof(T));
            #endif
        }
        else
            m_Vec=0;
        return *this;
    };

    const T& operator()(int i) const
    {
        #ifndef NO_AP_ASSERT
        ap_error::make_assertion(i>=m_iLow && i<=m_iHigh);
        #endif
        return m_Vec[ i-m_iLow ];
    };

    T& operator()(int i)
    {
        #ifndef NO_AP_ASSERT
        ap_error::make_assertion(i>=m_iLow && i<=m_iHigh);
        #endif
        return m_Vec[ i-m_iLow ];
    };

    void setbounds( int iLow, int iHigh )
    {
        if(m_Vec)
            delete[] m_Vec;
        m_iLow = iLow;
        m_iHigh = iHigh;
        m_iVecSize = iHigh-iLow+1;
        m_Vec = new T[m_iVecSize];
    };

    void setcontent( int iLow, int iHigh, const T *pContent )
    {
        setbounds(iLow, iHigh);
        for(int i=iLow; i<=iHigh; i++)
            (*this)(i) = pContent[i-iLow];
    };

    T* getcontent()
    {
        return m_Vec;
    };

    const T* getcontent() const
    {
        return m_Vec;
    };

    int getlowbound(int iBoundNum = 0) const
    {
        return m_iLow;
    };

    int gethighbound(int iBoundNum = 0) const
    {
        return m_iHigh;
    };
private:
    T         *m_Vec;
    long      m_iVecSize;
    long      m_iLow, m_iHigh;
};



/********************************************************************
КЛАСС-ШАБЛОН ДВУХМЕРНОГО ДИНАМИЧЕСКОГО МАССИВА


ОПИСАНИЕ ЧЛЕНОВ КЛАССА:

---------------------------------------------------------------------
template_2d_array()

Создание пустого массива.

---------------------------------------------------------------------
~template_2d_array()

Удаление массива. При этом освобождается выделенная под массив память

---------------------------------------------------------------------
template_2d_array(const template_2d_array &rhs)

Создание копии массива. При этом выделяется отдельная область памяти,
в которую копируется содержимое массива-источника

---------------------------------------------------------------------
const template_2d_array& operator=(const template_2d_array &rhs)

Присваивание массива. При этом содержимое массива-приемника удаляется
и освобождается  выделенная под него память,  затем заново выделяется 
отдельная область памяти, в которую копируется содержимое источника.

---------------------------------------------------------------------
      T& operator()(int i1, int i2)
const T& operator()(int i1, int i2) const

Обращение к элементу массива с индексом [i1,i2]

---------------------------------------------------------------------
void setbounds( int iLow1, int iHigh1, int iLow2, int iHigh2 )

Выделение  памяти   под   массив.    При   этом   старое   содержимое
массива   удаляется   и  освобождается  выделенная  под  него  память, 
затем   заново   выделяется   отдельная   область   памяти   размером 
(iHigh1-iLow1+1)*(iHigh2-iLow2+1) элементов.

Нумерация  элементов в новом массива по первой размерности начинается 
с iLow1 и заканчивается iHigh1, аналогично для второй размерности.

Содержимое нового массива не определено.

---------------------------------------------------------------------
void setcontent( int iLow1, int iHigh1, int iLow2, int iHigh2, 
    const T *pContent )

Метод  аналогичен  методу  setbounds()  за тем исключением, что после 
выделения памяти в неё копируется содержимое массива pContent[].

Массив pContent содержит двухмерный массив, записанный построчно, т.е.
первым идет элемент [iLow1, iLow2], затем [iLow1, iLow2+1] и т.д.

---------------------------------------------------------------------
      T* getcontent()
const T* getcontent() const

Метод позволяет получить указатель на содержимое массива. Данные,  на
которые указывает возвращенный указатель, можно изменять, и при  этом
изменится содержимое массива.

---------------------------------------------------------------------
int getlowbound(int iBoundNum)
int gethighbound(int iBoundNum)

Методы используются для  получения  информации  о  нижней  и  верхней
границах массива по размерности с переданным номером.
********************************************************************/
template<class T>
class template_2d_array
{
public:
    template_2d_array()
    {
        m_Vec=0;
        m_iVecSize=0;
    };

    ~template_2d_array()
    {
        if(m_Vec)
            delete[] m_Vec;
    };

    template_2d_array(const template_2d_array &rhs)
    {
        m_iVecSize = rhs.m_iVecSize;
        m_iLow1 = rhs.m_iLow1;
        m_iLow2 = rhs.m_iLow2;
        m_iHigh1 = rhs.m_iHigh1;
        m_iHigh2 = rhs.m_iHigh2;
        m_iConstOffset = rhs.m_iConstOffset;
        m_iLinearMember = rhs.m_iLinearMember;
        if(rhs.m_Vec)
        {
            m_Vec = new T[m_iVecSize];
            #ifndef UNSAFE_MEM_COPY
            for(int i=0; i<m_iVecSize; i++)
                m_Vec[i] = rhs.m_Vec[i];
            #else
            memcpy(m_Vec, rhs.m_Vec, m_iVecSize*sizeof(T));
            #endif
        }
        else
            m_Vec=0;
    };
    const template_2d_array& operator=(const template_2d_array &rhs)
    {
        if( this==&rhs )
            return *this;

        m_iLow1 = rhs.m_iLow1;
        m_iLow2 = rhs.m_iLow2;
        m_iHigh1 = rhs.m_iHigh1;
        m_iHigh2 = rhs.m_iHigh2;
        m_iConstOffset = rhs.m_iConstOffset;
        m_iLinearMember = rhs.m_iLinearMember;
        m_iVecSize = rhs.m_iVecSize;
        if(m_Vec)
            delete[] m_Vec;
        if(rhs.m_Vec)
        {
            m_Vec = new T[m_iVecSize];
            #ifndef UNSAFE_MEM_COPY
            for(int i=0; i<m_iVecSize; i++)
                m_Vec[i] = rhs.m_Vec[i];
            #else
            memcpy(m_Vec, rhs.m_Vec, m_iVecSize*sizeof(T));
            #endif
        }
        else
            m_Vec=0;
        return *this;
    };

    const T& operator()(int i1, int i2) const
    {
        #ifndef NO_AP_ASSERT
        ap_error::make_assertion(i1>=m_iLow1 && i1<=m_iHigh1);
        ap_error::make_assertion(i2>=m_iLow2 && i2<=m_iHigh2);
        #endif
        return m_Vec[ m_iConstOffset + i2 +i1*m_iLinearMember];
    };

    T& operator()(int i1, int i2)
    {
        #ifndef NO_AP_ASSERT
        ap_error::make_assertion(i1>=m_iLow1 && i1<=m_iHigh1);
        ap_error::make_assertion(i2>=m_iLow2 && i2<=m_iHigh2);
        #endif
        return m_Vec[ m_iConstOffset + i2 +i1*m_iLinearMember];
    };

    void setbounds( int iLow1, int iHigh1, int iLow2, int iHigh2 )
    {
        if(m_Vec)
            delete[] m_Vec;
        m_iVecSize = (iHigh1-iLow1+1)*(iHigh2-iLow2+1);
        m_Vec = new T[m_iVecSize];
        m_iLow1  = iLow1;
        m_iHigh1 = iHigh1;
        m_iLow2  = iLow2;
        m_iHigh2 = iHigh2;
        m_iConstOffset = -m_iLow2-m_iLow1*(m_iHigh2-m_iLow2+1);
        m_iLinearMember = (m_iHigh2-m_iLow2+1);
    };

    void setcontent( int iLow1, int iHigh1, int iLow2, int iHigh2, const T *pContent )
    {
        setbounds(iLow1, iHigh1, iLow2, iHigh2);
        for(int i=0; i<m_iVecSize; i++)
            m_Vec[i]=pContent[i];
    };

    T* getcontent()
    {
        return m_Vec;
    };

    const T* getcontent() const
    {
        return m_Vec;
    };

    int getlowbound(int iBoundNum) const
    {
        return iBoundNum==1 ? m_iLow1 : m_iLow2;
    };

    int gethighbound(int iBoundNum) const
    {
        return iBoundNum==1 ? m_iHigh1 : m_iHigh2;
    };
private:
    T           *m_Vec;
    long        m_iVecSize;
    long        m_iLow1, m_iLow2, m_iHigh1, m_iHigh2;
    long        m_iConstOffset, m_iLinearMember;
};


typedef template_1d_array<int>    integer_1d_array;
typedef template_1d_array<double> real_1d_array;
typedef template_1d_array<int>   boolean_1d_array;
typedef template_2d_array<int>    integer_2d_array;
typedef template_2d_array<double> real_2d_array;
typedef template_2d_array<int>   boolean_2d_array;


/********************************************************************
КОНСТАНТЫ И ФУНКЦИИ, СОВМЕСТИМЫЕ С ALGOPASCAL
********************************************************************/
static double machineepsilon = 5E-16;
static double maxrealnumber = 1E300;
static double minrealnumber = 1E-300;

static double sign(double x)
{
    if( x>0 ) return  1.0;
    if( x<0 ) return -1.0;
    return 0;
}

static double randomreal()
{
    int i = rand();
    while(i==RAND_MAX)
        i =rand();
    return double(i)/double(RAND_MAX);
}

static int randominteger(int maxv)
{  return rand()%maxv; }

static double round(double x)
{ return floor(x+0.5); }

static double trunc(double x)
{ return x>0 ? floor(x) : ceil(x); }

static double pi()
{ return 3.14159265358979323846; }

static double sqr(double x)
{ return x*x; }

static int maxint(int m1, int m2)
{
    return m1>m2 ? m1 : m2;
}

static int minint(int m1, int m2)
{
    return m1>m2 ? m2 : m1;
}

static double maxreal(double m1, double m2)
{
    return m1>m2 ? m1 : m2;
}

static double minreal(double m1, double m2)
{
    return m1>m2 ? m2 : m1;
}

};//namespace ap

extern "C"{
__declspec(dllexport) double lngamma(double x, double& sgngam);
__declspec(dllexport) double incompletebeta(double a, double b, double x);
__declspec(dllexport) double invincompletebeta(double a, double b, double y);
__declspec(dllexport) double invnormaldistribution(double y0);
__declspec(dllexport) double incompletegamma(double a, double x);
__declspec(dllexport) double invincompletegammac(double a, double y0);
__declspec(dllexport) double incompletegammac(double a, double x);
__declspec(dllexport) double gamma(double x);
__declspec(dllexport) double chisquarecdistribution(double v, double x);
__declspec(dllexport) double chisquaredistribution(double v, double x);
__declspec(dllexport) double invchisquaredistribution(double v, double y);
__declspec(dllexport) double studenttdistribution(int k, double t);
__declspec(dllexport) double invstudenttdistribution(int k, double p);
}
//#pragma comment(linker, "/include:_lngamma")
//#pragma comment(linker, "/include:_incompletebeta")
//#pragma comment(linker, "/include:_invincompletebeta")
//#pragma comment(linker, "/include:_invnormaldistribution")
//#pragma comment(linker, "/include:_incompletegamma")
//#pragma comment(linker, "/include:_invincompletegammac")
//#pragma comment(linker, "/include:_incompletegammac")
//#pragma comment(linker, "/include:_gamma")
//#pragma comment(linker, "/include:_chisquarecdistribution")
//#pragma comment(linker, "/include:_chisquaredistribution")
//#pragma comment(linker, "/include:_invchisquaredistribution")
//#pragma comment(linker, "/include:_studenttdistribution")
//#pragma comment(linker, "/include:_invstudenttdistribution")

#endif
