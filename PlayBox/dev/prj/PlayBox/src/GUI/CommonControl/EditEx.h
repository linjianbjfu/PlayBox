#pragma once


// CEditEx
// 编辑框扩展, 不同风格的编辑框
// 1. 无边框编辑框； 用于QQ面板里歌手列表搜索框 
// 2. 网络曲库搜索编辑框( 需要代码优化,把 ClocalSearchEdit合并过来 )

#ifndef MSG_MOUSELEAVE_EDIT
#define MSG_MOUSELEAVE_EDIT WM_USER+1002
#endif
#ifndef MSG_MOUSEHOVER_EDIT
#define MSG_MOUSEHOVER_EDIT WM_USER+1003
#endif 

#define MAXLENGHT_SEARCH_TEXT 128
#define SEARCH_HELP  "搜索..."
class CEditEx : public CEdit
{
	DECLARE_DYNAMIC(CEditEx)
private:
	HBRUSH mhBrush;		//控制客户区背景颜色的画刷
	CFont   m_font;
	UINT    m_textSize;
	BOOL    m_bMouseOn;

public:
	CEditEx();
	virtual ~CEditEx();
	void SetTextSize( UINT textSize);
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnMouseLeave(WPARAM   wParam,   LPARAM   lParam) ;
	afx_msg LRESULT OnMouseHover(WPARAM   wParam,   LPARAM   lParam) ;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
};


