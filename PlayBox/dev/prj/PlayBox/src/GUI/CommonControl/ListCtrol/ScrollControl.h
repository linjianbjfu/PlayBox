#pragma once
# define SB_POS 9

class CScrollControl
{
public:
    CScrollControl(int itemHight=20);
    ~CScrollControl();
    void   AttachWnd(CWnd* pWnd);
    void   DetachWnd();
    void   SetDisplaySize(int displayWidth, int displayHeight, int nBar=SB_BOTH);
    const CSize& GetDisplaySize() const;   // 出现滚动条的 宽，高

    const CSize* GetScrollPos();
    const CSize& GetPageSize() const;

    void   ScrollToOrigin(bool scrollLeft, bool scrollTop);
	void   ScrollToEnd( bool scrollRight, bool scrollBotton);
    void   OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    void   OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    BOOL   OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    void   OnSize(UINT nType, int cx, int cy);
	void   SetAItemMinHight(int nHight);
	void   ScrollHead(int pos); // 表头数据更新
private:
    int    GetScrollPos(int bar, CScrollBar* pScrollBar);
    void   UpdateScrollInfo(int nBar);
    void   UpdateScrollBar(int bar, int windowSize, int displaySize, LONG& pageSize, LONG& scrollPos, LONG& deltaPos);

    CWnd*  m_attachWnd;
    CSize  m_pageSize;
    CSize  m_displaySize;
    CSize  m_scrollPos;    // 滚动位置
	int    m_hight;
};


