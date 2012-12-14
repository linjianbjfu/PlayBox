#pragma once
#include "stdafx.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

#define MSG_CLICKED_BUTTON	WM_USER+234
#define MSG_CLICKED_MENU	WM_USER+235

class VButtonBar: public CWnd
{
public:
	VButtonBar();
	~VButtonBar();
public:

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg	LRESULT	OnMouseLeave(WPARAM,LPARAM);
	afx_msg	LRESULT	OnMouseHover(WPARAM,LPARAM);
	virtual	afx_msg	LRESULT	OnClickButton(WPARAM w,LPARAM p)=0;
	virtual	afx_msg	LRESULT	OnClickMenu(WPARAM w,LPARAM p)=0;
public:
	//设置button的图片和区域
	//vecStrBig表示所有大图片也就是背景的名称，vecStrSmall表示所有小图标也就是浮在大图片之上的小图片的名称
	//vecRectBig表示当某个button被选中之后它所在的rect，vecRectSmall表示当某个小图片显示时所在的区域
	//vecRectNormal表示某个button不被选中时所在的rect
	void SetButtons(vector<string> &vecBmpNameBig
		,vector<string> &vecBmpNamePlaySign
		,vector<string> &vecBmpNameMenu
		,vector<CRect> &vecRectBig
		,vector<CRect> &vecRectSmall
		,vector<CRect> &vecRectPlaySignBig
		,vector<CRect> &vecRectPlaySignSmall
		,vector<CRect> &vecRectMenu
		);
	void SelectButton(int iIdx);
	int  GetSelectedButtonId();
	void ShowPlaySign(int iIdx);
	CRect GetNormalButtonRect(int iIdx);
protected:
	vector<string>	m_vecBmpBtnName;
	vector<string>	m_vecBmpNameMenu;
	vector<string>	m_vecBmpNamePlaySign;//按钮文字前面表示正在播放的小三角
	vector<CRect>	m_vecRectSmall;
	vector<CRect>	m_vecRectBig;
	vector<CRect>	m_vecRectMenu;
	vector<CRect>	m_vecRectPlaySignBig;
	vector<CRect>	m_vecRectPlaySignSmall;
	vector<CDibBitmap*>	m_vecCDBSmallNormal;
	vector<CDibBitmap*>	m_vecCDBSmallOver;
	vector<CDibBitmap*>	m_vecCDBBigNormal;
	vector<CDibBitmap*>	m_vecCDBMenuNormal;
	vector<CDibBitmap*>	m_vecCDBMenuOver;
	vector<CDibBitmap*>	m_vecCDBMenuDown;
	vector<CDibBitmap*>	m_vecCDBPlaySignNormal;
	vector<CDibBitmap*>	m_vecCDBPlaySignOver;
	vector<CDibBitmap*>	m_vecCDBPlaySignBig;

	vector<CDibBitmap*>	m_vecCurCDB;
	vector<CRect>		m_vecCurRect;
	int				m_iBigShow;
	int				m_iBigShowBefore;
	int				m_iMenuShow;
	int				m_iMouseOn;
	int				m_iMouseBefore;
	int				m_iMouseDown;
	int				m_iBigButtonTotal;
	int				m_iAverLength;
	int				m_iDrawMode;
	bool			m_bIsMouseMove;
	bool			m_bIsMenuDown;
	bool			m_bIsMouseInClient;
	int				m_iPlaySignShow;
protected:
	int PtInWhickRect(CPoint pt);
	void DrawBitmap(CDC* dc, CDibBitmap* hbmp, RECT r);
	void LoadSkin();
	void DrawButtons(CDC *cdc);
	void UpdateDrawVec();
	void DrawCurButtonOver(CDC *cdc);
	//void DrawCurSmallButtonDown(CDC *cdc);
	void InitDrawVec();
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
