#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../gui/CommonControl/BasicSkinWnd.h"
#include "../../LayoutMgr/ISkinMgr.h"
#include "ITabBarObserver.h"

class CDibBitmap;
class CCWToolTips;

class CTabBarWnd : public CBasicWnd,	
				   public CBasicSkinWnd,
				   public ITabBarObserver
{
	DECLARE_DYNAMIC(CTabBarWnd)
public:
	CTabBarWnd();
	virtual ~CTabBarWnd();
	void	LoadSkin();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int	 OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
public:
	void ITabBarOb_CreateNewTab(TAB_ITEM &item);
	void ITabBarOb_OpenExistTab(TAB_ITEM& item);
	void ITabBarOb_DelTab(TAB_ITEM& item);
	void ITabBarOb_OpenTabError(int iErrorCode);
	void ITabBarOb_TabItemDataChanged(TAB_ITEM & item);
private:
	CCWToolTips*			m_pToolTip;
	CFont					m_fontNormal;
	CFont					m_fontDown;
	vector<CRect>			m_vecTab;			//所有tab的位置
	vector<CRect>			m_vecClose;			//所有tab上的关闭按钮位置
	vector<TAB_ITEM>		m_vecTi;			//文字信息
	CRect					m_rectNewWindow;
	bool					m_bOverNewWindow;	//鼠标当前是否在newwindow按钮上
	int						m_iTabCur;			//当前显示的tab
	int						m_iTabOver;			//当前鼠标over的tab
	int						m_iCloseOver;		//当前鼠标over的close
	COLORREF				m_colNormalTextColor;
	COLORREF				m_colDownTextColor;
	COLORREF				m_colMask;

	CDibBitmap*				m_pBmpRectNormalLeft;
	CDibBitmap*				m_pBmpRectNormalMiddle;
	CDibBitmap*				m_pBmpRectNormalRight;

	CDibBitmap*				m_pBmpRectOverLeft;
	CDibBitmap*				m_pBmpRectOverMiddle;
	CDibBitmap*				m_pBmpRectOverRight;

	CDibBitmap*				m_pBmpRectDownLeft;
	CDibBitmap*				m_pBmpRectDownMiddle;
	CDibBitmap*				m_pBmpRectDownRight;

	CDibBitmap*				m_pBmpCloseNormal;
	CDibBitmap*				m_pBmpCloseOver;
	CDibBitmap*				m_pBmpCloseDown;

	CDibBitmap*				m_pBmpSelCloseNormal;
	CDibBitmap*				m_pBmpSelCloseOver;
	CDibBitmap*				m_pBmpSelCloseDown;

	CDibBitmap*				m_pBmpNewWindowNormal;
	CDibBitmap*				m_pBmpNewWindowOver;
	CDibBitmap*				m_pBmpNewWindowDown;

	BITMAPINFOHEADER		m_bmih;

	BOOL					m_bLButtonDown;
	string	m_strDefaultNewBrowserUrl;
private:
	void	DrawBar( CDC* pDc );
	int		GetOverTab( CPoint pt );
	int		GetOverClose( CPoint pt );
	void	CalcTabPosition(); //计算m_vecTab和m_vecClose	
	void	DrawTabItemRect( CDC* pDc, CRect& rc,
					CDibBitmap* pRectLeft, CDibBitmap* pRectMiddle, 
					CDibBitmap* pRectRight, CRect& rcClose, 
					CDibBitmap* pClose, string strText, bool bDown );
};