#pragma once
#include ".\scrollbarEx.h"
#include "SkinBotton.h"
#include "IEvent.h"

// CSkinWnd frame
#include <map>
#include <list>
using namespace std; 
class CSkinWnd : public CFrameWnd, public IEventListener
{
	DECLARE_DYNCREATE(CSkinWnd)
public:
	CSkinWnd();           // protected constructor used by dynamic creation
	virtual ~CSkinWnd();
	enum{ BOTTON_BEGAIN_ID = 20000 };
	enum{ LIFT_SPACE = 0,TOP_SPACE = 6, SCROOLBAR_WIDTH = 25 ,FONT_HEIGHT=25,};
	enum{ ROW_COUNT = 4 };  
public:
	void InitScrollBar();
private:
	CScrollBarEx mpVerticleScrollbar;	//纵向滚动条
	CScrollBarEx mpHorizontalScrollbar;	//横向滚动条

	CRect	mrcOriginalRect;  // 初始RECT
	int		miScrollPos;      // SCROLL位置
	int		miHeight;         // 当前高度
	bool    mbIsHasScroll;   
	int     m_nMaxViewPos;

	list<CSkinButton*> m_colBtnList;
	list<CSkinButton*>::iterator m_colBtnList_itor;

	map<CString, CSkinButton*> m_noDownloadSkinMap;
	void UpdateScrollBar();
	void UpdateScrollBarPosition();

	IEventListener* mp_EventListener;
	CFont			m_font;
public:
	void UpdateControlsEnable(BOOL abEnable);
	void InsertSmallSkin(string &astrName,CBitmap* apBmp, string strPath,bool bHasSkin);
	void SetSendEventListener(IEventListener* aEventListener ){mp_EventListener = aEventListener ;}
	void EnableSkinBtn( BOOL enable= TRUE );
	void DownloadSkinFinish( CString strSkinName );
	void SetTipText( CString astrName,CString strTip);
	void SetUseTip( bool bUseTip);
	
	void	ShowWindow(BOOL bShow);
	void	OnChangeSubject();
	void	SkinChanged(const string& strNewSkin);
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnCliekSlide(CSliderCtrl* pBotton){;}
	virtual void OnSendChangeColor( COLORREF acolCol, UINT anLightValue ){;}
	virtual void OnSendChangeSkin ( CString strPath){;}
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnDestroy();
	afx_msg void OnNcPaint();
	afx_msg LRESULT OnSkinDownLoadFailed(WPARAM wParam, LPARAM lParam);
	virtual void OnCliekButton(CButton* pBotton);
};


