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
	afx_msg void			OnPaint();
	afx_msg void			OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void			OnSize(UINT nType, int cx, int cy);
	afx_msg int				OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void			OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void			OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void			OnDestroy();
public:
	virtual void			ITabBarOb_CreateNewTab(TAB_ITEM &item);
	virtual void			ITabBarOb_OpenExistTab(TAB_ITEM& item);
	virtual void			ITabBarOb_DelTab(TAB_ITEM& item);
	virtual void			ITabBarOb_OpenTabError(int iErrorCode);
private:
	CCWToolTips*			m_pToolTip;
	CFont					m_fontNormal;
	CFont					m_fontDown;
	vector<CRect>			m_vecTab;			//����tab��λ��
	vector<CRect>			m_vecClose;			//����tab�ϵĹرհ�ťλ��
	vector<TAB_ITEM>		m_vecTi;			//������Ϣ
	int						m_iTabCur;			//��ǰ��ʾ��tab
	int						m_iTabOver;			//��ǰ���over��tab
	int						m_iCloseOver;		//��ǰ���over��close
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

	BITMAPINFOHEADER		m_bmih;

	BOOL					m_bLButtonDown;
private:
	void	DrawBar( CDC* pDc );
	int		GetOverTab( CPoint pt );
	int		GetOverClose( CPoint pt );
	void	CalcTabPosition(); //����m_vecTab��m_vecClose	
	void	DrawTabItemRect( CDC* pDc, CRect& rc,
					CDibBitmap* pRectLeft, CDibBitmap* pRectMiddle, 
					CDibBitmap* pRectRight, CRect& rcClose, 
					CDibBitmap* pClose, string strText, bool bDown );
};