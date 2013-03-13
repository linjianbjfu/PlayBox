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
	
	// ���ܣ�����ÿһ��ĸ߶ȼ����
	// ������iItemWidth	���
	// ������iItemHeight �߶�
	// ����ֵ��void
	void SetItemWH(int iItemWidth, int iItemHeight);

	void SetImgState(LMCImgState is);

	// ���ܣ�����һ��
	// ������pImg	��ʾͼƬ
	// ������strItemName �������
	// ������blSel	�Ƿ�ѡ��
	// ������nGameType	��Ϸ����
	// ����ֵ��int ÿһ��ı�ʶ
	int InsertItem(CDibBitmap* pImg, CString strItemName, CString strdetail, 
		string strGID, string strSvrID, string strAddTime, int nGameType, 
		BOOL blSel=FALSE);
	int InsertItem(LPCTSTR szImgPath, CString strItemName, CString strdetail, 
		string strGID, string strSvrID, string strAddTime, int nGameType, 
		BOOL blSel=FALSE);
	// ���ܣ���ȡѡ��Item��Index
	// ����ֵ��vector<int> һ��Index 
	vector<int> GetSelectItem();

	LMC_ItemInfo GetItemInfoByIndex(int iItem);

	// ���ù�����
	void SetScrollBitmap(CDibBitmap* pBitmapVert,CDibBitmap* pBitmapHorz);
	void EnsureVisible(int iIndex);
	virtual void OnMoveDraging();
	virtual void OnEndDrag();
protected:
	// �ػ�
	void OnMemoryDraw(BOOL blRePaint=TRUE);
	// �ͻ�����С�ı�ʱ����
	void SizeChanged(BOOL blFouce=FALSE);
	// ����ÿһ��Item��
	void DrawItem(CPoint ptS, LMC_ItemInfo& lmci);
	// ����ͻ�����Item��λ��
	void CalcHideItemRect(CPoint ptS, LMC_ItemInfo& lmci);
	// ��ֱ�������
	void MoveVWnd(BOOL blIsup);
	// �������
	BOOL HitTest(int& itemIndex,CPoint pt);
	// ������ק�����С
	void OnCalcTracker();
	// ������ЩItem��ѡ��
	void OnCalcSelectItem();
	// ������ק����
	void OnDrawTracker();
	// ����Բ�Ǿ���GDI+
	//void DrawRoundRect(Graphics& gc, Pen& pen, int x,int y,int w, int h, int X, int Y);
	GraphicsPath* MakeRoundRect(Point topLeft, Point bottomRight, INT percentageRounded);


	// ����������Ϊδѡ��״̬
	void UnSelectAll();
	// ����
	void UpdateList();
private:
	int					m_iItemWidth;		// ÿһ��Ŀ��--ָͼƬ
	int					m_iItemHeight;		// ÿһ��ĸ߶�--ָͼƬ
	LMCImgState			m_ImgState;			// ͼƬ״̬
	CDC					m_dcMem;			// �ڴ�dc
	CBitmap				m_memBitmap;		// �ڴ�λͼ
	CBitmap*			m_pOldBitmap;		// �ڴ�λͼ
	CFont				m_fName;			// ��������
	CFont				m_fDeInfo;			// ������ϸ��Ϣ������
	CRect				m_rect;
	CRect				m_rcRealRect;		// ��ʵ�����С
	CPoint				m_ptClientStart;	// �б���ʼ���Ƶ�λ��
	CCriticalSection	m_csLockV;			// ͬ������
	deque<CPoint>		m_vPtQueue;			// ��������
	UINT				m_vScrollTimer;		// ������ʱ�����
	GdiRes				m_gdires;			// ��ͼ�����Դ
	BOOL				m_blMutil;			// ��ѡ
	int					m_iPreSel;
	
	//===============================================================
	CDC					m_dcTracker;		// λͼdc
	CBitmap				m_bmpTracker;		// �ڴ�λͼ
	CBitmap*			m_bmpoldTracker;	// ԭʼλͼdc��λͼ
	CPoint				m_ptDown;			// ���LButtonDown������ĵ�
	BLENDFUNCTION		m_bf;				// ͸���ȿ���
	CRect				m_rcTracker;		// ��ק����
	int					m_iDistance;		// ����������ʵ����top�ľ���
	UINT				m_vScrDownTimer;	// �����Զ�������ʱ��
	UINT				m_vScrUpTimer;		// �����Զ�������ʱ��
	//================================================================

	BOOL				m_blIsJudgeDrag;	// �ж϶�ק
	CBitmap*			m_pBitmapYes;		// yes 
	CBitmap*			m_pBitmapNo;		// no
	CDibBitmap*	m_pBitmapScrollVert;
	CDibBitmap*	m_pBitmapScrollHorz;
	bool m_bInit;
	BOOL m_PaintScrollBar;
	
	Gdiplus::Image *pImageFrame;

protected:
	BOOL				m_blTracker;		// �Ƿ�ʼ��ѡ
	CLMCDataMgr			m_DataMgr;
	BOOL				m_blDrag;			// ��ʼ��ק�б��е���
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