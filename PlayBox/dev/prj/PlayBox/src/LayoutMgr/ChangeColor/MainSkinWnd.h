#pragma once
#include "../../gui/CommonControl/SkinButton2.h"
#include "../../gui/CommonControl/xSkinButton.h"
#include "IEvent.h"
#include "StaticTextEx.h"

class CCWToolTips;
// CMainSkinWnd frame
class /*__declspec( dllexport )*/IWndEventListener
{
public:
	virtual bool OnChangeSkinTemplate(CString strPath){return false;};
	virtual void OnchangeColor(COLORREF acolCol, UINT anLightValue){};
private:
	friend class CMainSkinWnd; 
	virtual void OnTabSelChange( UINT anIndex) = 0;
};

#define ID_CLOSE_CHANGESKIN_BTN  (10000+1)
#define ID_SUB_CHANGESKIN_BTN  (10000+2)
#define ID_SKIN_CHANGESKIN_BTN  (10000+3)
#define ID_MORECOLOR_CHANGESKIN_BTN  (10000+4)
#define ID_DOWNMORE_CHANGESKIN_BTN	 (10000+5)
#define ID_CLO_BTN_BEGAIN         30000
#define ID_MORE_COLOR             20000
#define ID_MORE_COLOR_LINK        20000+1

#define ID_WEBSUB_CHANGESKIN_BTN  (10000+6)

class CMainSkinWnd : public CFrameWnd
{
	DECLARE_DYNCREATE(CMainSkinWnd)
public:
	enum{ BOTTON_BEGAIN_ID = 20000 };
	enum{ LIFT_SPACE = 3,TOP_SPACE = 22, SCROOLBAR_WIDTH = 25 };
	enum{ ROW_COUNT = 6 };  
	enum{ COL_BTN_LEFT = 9,COL_BTN_TOP=265 ,COL_BTN_INTER=6};

public:
	UINT m_hight;
	UINT m_width;

	CMainSkinWnd();           // protected constructor used by dynamic creation
	virtual ~CMainSkinWnd();
	void CloseWindows();
	void SetListener( IEventListener* apListtener ) { mp_EventListener = apListtener; }
	void SetWndListener(IWndEventListener* pWndListener){ mp_WndListener = pWndListener; }
	
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

private:

	IEventListener*			mp_EventListener;
	IWndEventListener*		mp_WndListener;

	vector<CSkinButton2*> m_colbtnVec;
	CxSkinButton			m_BtnClose;
	CxSkinButton			m_btnSkin;
	CxSkinButton			m_btnSub;
	CxSkinButton			m_btnWebSub;
	CxSkinButton			m_btnMoreColr;
	CxSkinButton			m_btnDownMore;

	CSkinButton2          m_colBtnMore;

	CCWToolTips*	m_pToolTip;
//	CStaticText        m_staticOtherCol;
	COLORREF m_colDefault;
	CString		m_ToolTipText;
public:
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnPaint();
	afx_msg void	OnCloseDlg();
	afx_msg void	OnBtnSub();
	afx_msg	void	OnBtnSkin();
	afx_msg void	OnColor(UINT nID);
	afx_msg void	OnClickMoreCol();
	afx_msg void	OnClickDownMore();
	afx_msg LRESULT OnChangeBtnColor(WPARAM wParam, LPARAM lParam);

	afx_msg void	OnBtnWebSub();
	void			LoadSkin();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	void			ShowColorBtn(BOOL bShow);
	afx_msg void OnMove(int x, int y);

	void	ShowToolTip(BOOL bShow);
	void	SetToolTip(CString str);

	void	SetCheckBtn(int ibtn);
};


