#if !defined(__ChromoCnv_H__)
#define __ChromoCnv_H__

#include "Chromosome.h"

class CChromoCnvBase : public CDataObjectBase, public IChromoConvert2
{
protected:
	IChromoConvert2Ptr m_sptrCCPrev;
	CString m_sOpts;
	DECLARE_INTERFACE_MAP();
public:
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject) {return ExternalQueryInterface(&riid,ppvObject);}
	ULONG STDMETHODCALLTYPE AddRef(void) {return ExternalAddRef();}
	ULONG STDMETHODCALLTYPE Release(void) {return ExternalRelease();}
	virtual LPUNKNOWN GetInterfaceHook(const void*);

	com_call Init(IChromoConvert2 *pCCPrev, LPCSTR lpszOpts);
	com_call Recalc(IChromosome2 *pChromo);
	com_call GetInitString(BSTR *pbstrInitStr) {*pbstrInitStr = m_sOpts.AllocSysString(); return S_OK;}
};

class CChromoCnvRotate : public CChromoCnvBase
{
	PROVIDE_GUID_INFO();
	DECLARE_DYNCREATE(CChromoCnvRotate);
	GUARD_DECLARE_OLECREATE(CChromoCnvRotate);
	double CalcAngle(IChromosome2 *pChromo, ROTATEPARAMS *prp);
public:
	com_call CCGetAxis(IChromosome2 *pChromo, DWORD dwFlags, IChromoAxis2 **ppAxis, BOOL *pbIsManual);
	com_call CCGetCentromereCoord(IChromosome2 *pChromo, CENTROMERE *pCen, BOOL *pbIsManual);
	com_call CCGetImage(IChromosome2 *pChromo, IUnknown **ppunk);
};

class CChromoCnvEqualize : public CChromoCnvBase
{
	PROVIDE_GUID_INFO();
	DECLARE_DYNCREATE(CChromoCnvEqualize);
	GUARD_DECLARE_OLECREATE(CChromoCnvEqualize);
public:
	com_call CCGetAxis(IChromosome2 *pChromo, DWORD dwFlags, IChromoAxis2 **ppAxis, BOOL *pbIsManual);
	com_call CCGetCentromereCoord(IChromosome2 *pChromo, CENTROMERE *pCen, BOOL *pbIsManual);
	com_call CCGetImage(IChromosome2 *pChromo, IUnknown **ppunk);
};

class CChromoCnvEmpty : public CChromoCnvBase
{
	PROVIDE_GUID_INFO();
	DECLARE_DYNCREATE(CChromoCnvEmpty);
	GUARD_DECLARE_OLECREATE(CChromoCnvEmpty);
public:
	com_call CCGetAxis(IChromosome2 *pChromo, DWORD dwFlags, IChromoAxis2 **ppAxis, BOOL *pbIsManual);
	com_call CCGetCentromereCoord(IChromosome2 *pChromo, CENTROMERE *pCen, BOOL *pbIsManual);
	com_call CCGetImage(IChromosome2 *pChromo, IUnknown **ppunk);
};

class CChromoCnvSeq : public IChromoConvert2
{
	CStringArray m_saProgIDs;
	CString m_sInit;
	IChromoConvert2** m_pCnvs;
	int m_nSize;
	void ReleaseAll();

	IUnknownPtr m_punkImg;
	CENTROMERE m_Cen;
	CChromoAxis m_ShortAxis,m_LongAxis;
	BOOL  m_bAxisManual,m_bCenManual,m_bAxisInited,m_bCenInited;
public:
	CChromoCnvSeq();
	~CChromoCnvSeq();
//	void Init(LPCTSTR lpSeq);

	CChromoAxis& GetAxis(IChromosome2 *pChromo, DWORD dwFlags, BOOL *pbIsManual);
	CENTROMERE GetCentromereCoord(IChromosome2 *pChromo, BOOL *pbIsManual);
	IUnknownPtr GetImage(IChromosome2 *pChromo);
//	void Reset();

public:
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject) {return E_NOTIMPL;}
	ULONG STDMETHODCALLTYPE AddRef(void) {return 1;}
	ULONG STDMETHODCALLTYPE Release(void) {return 1;;}
	com_call CCGetAxis(IChromosome2 *pChromo, DWORD dwFlags, IChromoAxis2 **ppAxis, BOOL *pbIsManual) ;
	com_call CCGetCentromereCoord(IChromosome2 *pChromo, CENTROMERE *pCen, BOOL *pbIsManual);
	com_call CCGetImage(IChromosome2 *pChromo, IUnknown **ppunk);
	com_call Recalc(IChromosome2 *pChromo);
	com_call Init(IChromoConvert2 *pCCPrev, LPCSTR lpszOpts);
	com_call GetInitString(BSTR *pbstrInitStr);
};


#endif
