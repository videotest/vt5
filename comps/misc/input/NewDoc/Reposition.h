#if !defined(__Reposition_H__)
#define __Reposition_H__

class CCtrlInfo
{
public:
	HWND m_hwnd;
	CRect m_rcWnd;
	bool m_bTemp;
	CCtrlInfo()
	{
		memset(this, 0, sizeof(*this));
	};
	CCtrlInfo(CCtrlInfo &t)
	{
		memcpy(this, &t, sizeof(*this));
	}
	CCtrlInfo(HWND hwnd, CRect rcWnd, bool bTemp = false)
	{
		m_hwnd = hwnd;
		m_rcWnd = rcWnd;
		m_bTemp = bTemp;
	}
};

class CRepositionNode : public CArray<CCtrlInfo,CCtrlInfo&>
{
	CRect m_rcNode;
	void InsertTempControl(CCtrlInfo &CtrlInfo, int nPos);
public:
	void InitRect(CRect rc) {m_rcNode = rc;} // in screen coordinates
	void Add(HWND hwnd, CRect rcWnd); // in screen coordinates
	void Reposition(CPoint ptPos);
	CSize GetWndSize() {return m_rcNode.Size();}
	void RecalcRect();
	void AddTempControl(HWND hwnd, CRect rc, int nPos);
	void RemoveTempControl(HWND hwnd);
};

class CRepositionManager
{
	HWND m_hwndParent;
	int m_idBase;
	CSize m_szMaxClient;
	CRepositionNode m_Left,m_Right,m_Top,m_Bottom,m_LeftTop,m_LeftBottom,m_RightTop,m_RightBottom;
	CRect CalcMargins();
	CSize GetParentWindowMargins();
public:
	CRect m_rcBase;
	CRect m_rcMinMargins;
	bool  m_bReposByPreview;
	CRepositionManager();
	void Init(HWND hwndParent, int idBase, CSize szMaxClient);
	void Reposition(CSize szBase, CRect &rcBase, CSize &szTotal);
	void RepositionByTotal();
	CSize GetMinSize();
	CSize CalcSizeByCx(CSize szPreview, int cx, bool bWndSize = false);
	CSize CalcSizeByCy(CSize szPreview, int cy, bool bWndSize = false);
	void SetMaximalSize(CSize szPrvOrig, bool bOrig = true);
	void SetSizeByPreview(CSize szPreview, bool bOrig = true);
	void HandleSizing(CSize szPrvOrig, UINT nSide, LPRECT lpRect);
	void AddTempControl(HWND hwnd, int nNode, int nPos);
	void RemoveTempControl(HWND hwnd);
};

#endif
