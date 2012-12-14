#pragma once


// CWToolTips
#define CLIENT_TOOLTIPS 0
#define FRAME_TOOLTIPS 1
#define ID_TEMER_TOOLTIPS               230
#define ID_TIMER_HIDE						231
class CCWToolTips : public CWnd
{
	DECLARE_DYNAMIC(CCWToolTips)
public:
	CCWToolTips();
	virtual ~CCWToolTips();

	COLORREF m_clrFontText;; 
	COLORREF m_clrBorder;
	COLORREF m_clrBK;

	CWnd* m_pClientWnd; 
	CString m_strInfo; 
	CPoint m_lastPoint;

	CRect m_rcRange;
	int DrawMultiLine(CString tempStrInfo, CRect textRect, CDC &dc );
	void ShowToolTips(CString info,CPoint point, CRect rect, int type=CLIENT_TOOLTIPS);

	int m_type;
	int	m_nTimer;

	void ShowToolTips(CString info,CPoint point,int type=CLIENT_TOOLTIPS);

	void ShowToolTips(CString info, CRect rect, int timeout);

	void HideTooTips();
	HWND m_hParent;

	virtual BOOL PreTranslateMessage(MSG *pMsg);

	void SetChangeColor(BOOL bchange);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNcPaint();
private:
	BOOL m_bChangeColor;
};


