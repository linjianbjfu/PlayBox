#pragma once


// CEditEx
// �༭����չ, ��ͬ���ı༭��
// 1. �ޱ߿�༭�� ����QQ���������б������� 
// 2. �������������༭��( ��Ҫ�����Ż�,�� ClocalSearchEdit�ϲ����� )

#ifndef MSG_MOUSELEAVE_EDIT
#define MSG_MOUSELEAVE_EDIT WM_USER+1002
#endif
#ifndef MSG_MOUSEHOVER_EDIT
#define MSG_MOUSEHOVER_EDIT WM_USER+1003
#endif 

#define MAXLENGHT_SEARCH_TEXT 128
#define SEARCH_HELP  "����..."
class CEditEx : public CEdit
{
	DECLARE_DYNAMIC(CEditEx)
private:
	HBRUSH mhBrush;		//���ƿͻ���������ɫ�Ļ�ˢ
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


