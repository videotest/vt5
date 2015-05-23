#ifndef __misc_h__
#define __misc_h__

class CMainFrame;

#define szDocTerminate	"DocTerminate"

CMainFrame *GetMainFrame();
void ExecuteObjectScript( IUnknown *punkDoc, const char *psz_script, const char *psz_doc_name );

class CPriorityArr : public CTypedPtrArray<CObArray,CWnd*>
{
};

class CPrioritiesMgr : public CTypedPtrArray<CObArray,CPriorityArr*>
{
public:
	CPrioritiesMgr();
	~CPrioritiesMgr();
	void ReadPriority(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nDefault, CWnd *pWnd);
	void Reposition();
};


#endif //__misc_h__