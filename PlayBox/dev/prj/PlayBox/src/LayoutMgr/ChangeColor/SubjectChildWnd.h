#pragma once
#include "SkinBotton.h"
#include "scrollbarEx.h"
#include "IEvent.h"
#include "SubLabel.h"

#define MSG_CHANGE_DEF_BTN_COLOR (WM_USER+100)

class CSubjectChildWnd : public CFrameWnd, public IEventListener
{
	DECLARE_DYNCREATE(CSubjectChildWnd)
public:
	CSubjectChildWnd();           // protected constructor used by dynamic creation
	virtual ~CSubjectChildWnd();
	
	enum{ BOTTON_BEGAIN_ID = 20000 };
	enum{ SUB_LABEL_ID = 25000 };
	enum{ LIFT_SPACE = 0,TOP_SPACE = 3, SCROOLBAR_WIDTH = 25 ,INTER = 17};
	enum{ ROW_COUNT = 4 };  

public:
	void UpdateControlsEnable(BOOL abEnable);
	void InsertSmallObjectBmp(string &astrName,CBitmap* apBmp, string strPath,bool bHasSkin=false);
	void InitScrollBar();
	void SetSendEventListener(IEventListener* aEventListener ){mp_EventListener = aEventListener ;}	

	void OnSendChangeSkin ( CString strPath);	
	void EnableSkinBtn( BOOL enable );
	void SetTipText( CString astrName,CString strTip,CString strShowName = NULL);
	void ResetScroll();
	void InsertSubjectBtn(string &astrName,CBitmap* apBmp, string strPath,bool bHasSkin);
	BOOL IsSubExist(string strName);
	int		miScrollPos;      // SCROLL位置
private:
	CScrollBarEx mpVerticleScrollbar;	//纵向滚动条
	CScrollBarEx mpHorizontalScrollbar;	//横向滚动条

	CRect	mrcOriginalRect;  // 初始RECT
	
	int		miHeight;         // 当前高度
	bool    mbIsHasScroll;
	int     m_nMaxViewPos;

	list<CSkinButton*> m_colBtnList;
	list<CSkinButton*>::iterator m_colBtnList_itor;
	list<CSkinButton*>::iterator m_BtnNextList_itor; //被删除主题的下一个主题，删除当前主题时切换到此主题
	list<CSubLabel *> m_SubLabelList;
	int m_SubBtnID;

	map<CString, CSkinButton*> m_noDownloadSkinMap;

	void UpdateScrollBar();
	void UpdateScrollBarPosition();
	void CreateSubBtn();

	IEventListener* mp_EventListener;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnNcPaint();
	virtual void OnCliekButton(CButton* pBotton);	
	virtual void OnCliekButton(const CString& str);	

	void		 ShowWindow(BOOL bShow);
	void		 ClickSubBtn(const CString& strSkinPath,const CString& strSubName);
protected:
	void DelAllSubjectBtn();
	
	void DelSubjectBtn(CButton * pdelbtn);
	void MoveSubjectBtn();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
