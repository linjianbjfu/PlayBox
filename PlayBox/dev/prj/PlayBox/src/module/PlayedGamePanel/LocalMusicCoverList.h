#pragma once
#include <afxmt.h>
#include <deque>
#include "LMCDataMgr.h"


#define LMC_MSG_TRACKERCHANGE WM_USER+200 

class CLocalMusicCoverList : public CWnd
{
	DECLARE_DYNAMIC(CLocalMusicCoverList)
public:
	CLocalMusicCoverList();
	~CLocalMusicCoverList();

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
public:
	
	// 功能：设置每一项的高度及宽度
	// 参数：iItemWidth	宽度
	// 参数：iItemHeight 高度
	// 返回值：void
	void SetItemWH(int iItemWidth, int iItemHeight);

	void SetImgState(LMCImgState is);

	// 功能：插入一项
	// 参数：pImg	显示图片
	// 参数：strItemName 项的名称
	// 参数：blSel	是否选中
	// 参数：nGameType	游戏类型
	// 返回值：int 每一项的标识
	int InsertItem(CDibBitmap* pImg, CString strItemName, CString strdetail, 
		string strGID, string strSvrID, string strAddTime, int nGameType, 
		BOOL blSel=FALSE);
	int InsertItem(LPCTSTR szImgPath, CString strItemName, CString strdetail, 
		string strGID, string strSvrID, string strAddTime, int nGameType, 
		BOOL blSel=FALSE);
	// 功能：获取选中Item的Index
	// 返回值：vector<int> 一组Index 
	vector<int> GetSelectItem();

	LMC_ItemInfo GetItemInfoByIndex(int iItem);

	// 设置滚动条
	void SetScrollBitmap(CDibBitmap* pBitmapVert,CDibBitmap* pBitmapHorz);
	void EnsureVisible(int iIndex);
	virtual void OnMoveDraging();
	virtual void OnEndDrag();
protected:
	// 重绘
	void OnMemoryDraw(BOOL blRePaint=TRUE);
	// 客户区大小改变时调用
	void SizeChanged(BOOL blFouce=FALSE);
	// 绘制每一个Item项
	void DrawItem(CPoint ptS, LMC_ItemInfo& lmci);
	// 计算客户区外Item的位置
	void CalcHideItemRect(CPoint ptS, LMC_ItemInfo& lmci);
	// 竖直方向滚动
	void MoveVWnd(BOOL blIsup);
	// 点击测试
	BOOL HitTest(int& itemIndex,CPoint pt);
	// 计算拖拽区域大小
	void OnCalcTracker();
	// 计算哪些Item被选中
	void OnCalcSelectItem();
	// 绘制拖拽区域
	void OnDrawTracker();
	// 绘制圆角矩形GDI+
	//void DrawRoundRect(Graphics& gc, Pen& pen, int x,int y,int w, int h, int X, int Y);
	GraphicsPath* MakeRoundRect(Point topLeft, Point bottomRight, INT percentageRounded);


	// 将所有项置为未选择状态
	void UnSelectAll();
	// 更新
	void UpdateList();
private:
	int					m_iItemWidth;		// 每一项的宽度--指图片
	int					m_iItemHeight;		// 每一项的高度--指图片
	LMCImgState			m_ImgState;			// 图片状态
	CDC					m_dcMem;			// 内存dc
	CBitmap				m_memBitmap;		// 内存位图
	CBitmap*			m_pOldBitmap;		// 内存位图
	CFont				m_fName;			// 名称字体
	CFont				m_fDeInfo;			// 绘制详细信息的字体
	CRect				m_rect;
	CRect				m_rcRealRect;		// 真实区域大小
	CPoint				m_ptClientStart;	// 列表起始绘制的位置
	CCriticalSection	m_csLockV;			// 同步控制
	deque<CPoint>		m_vPtQueue;			// 滚动队列
	UINT				m_vScrollTimer;		// 滚动定时器检测
	GdiRes				m_gdires;			// 绘图相关资源
	BOOL				m_blMutil;			// 多选
	int					m_iPreSel;
	
	//===============================================================
	CDC					m_dcTracker;		// 位图dc
	CBitmap				m_bmpTracker;		// 内存位图
	CBitmap*			m_bmpoldTracker;	// 原始位图dc的位图
	CPoint				m_ptDown;			// 鼠标LButtonDown点击到的点
	BLENDFUNCTION		m_bf;				// 透明度控制
	CRect				m_rcTracker;		// 拖拽区域
	int					m_iDistance;		// 点击点距离真实区域top的距离
	UINT				m_vScrDownTimer;	// 向下自动滚屏定时器
	UINT				m_vScrUpTimer;		// 向上自动滚屏定时器
	//================================================================

	BOOL				m_blIsJudgeDrag;	// 判断多拽
	CBitmap*			m_pBitmapYes;		// yes 
	CBitmap*			m_pBitmapNo;		// no
	CDibBitmap*	m_pBitmapScrollVert;
	CDibBitmap*	m_pBitmapScrollHorz;
	bool m_bInit;
	BOOL m_PaintScrollBar;
	
	Gdiplus::Image *pImageFrame;

protected:
	BOOL				m_blTracker;		// 是否开始框选
	CLMCDataMgr			m_DataMgr;
	BOOL				m_blDrag;			// 开始拖拽列表中的项
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	LRESULT OnTrackerChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCoolSBCustomdraw(NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnDestroy();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};