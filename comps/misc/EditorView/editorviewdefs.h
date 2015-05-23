
#ifndef __editorviewdefs_h__
#define __editorviewdefs_h__

#ifndef DLL

#ifdef _EDITOR_VIEW_DLL
#define	DLL __declspec(dllexport)
#else
#define	DLL __declspec(dllimport)
#endif //_EDITOR_VIEW_DLL

#endif //export_class


#define BEGIN_INTERFACE_PART_EXP(localClass, baseClass) \
	public:	\
	class DLL X##localClass : public baseClass \
	{ \
	public: \
		STDMETHOD_(ULONG, AddRef)(); \
		STDMETHOD_(ULONG, Release)(); \
		STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj); \


#endif __editorviewdefs_h__

