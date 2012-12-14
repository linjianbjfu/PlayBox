
#ifndef TabCtrlExH
#define TabCtrlExH

class CTabCtrlEx : public CTabCtrl
{
public:
	CTabCtrlEx();
	virtual ~CTabCtrlEx();
	void RepaintClientRect();

public:
	enum ENUM_ORIENT{TOP,RIGHT,BOTTON,LIFT};
	void SetSelTipOrient(ENUM_ORIENT aiOrient);
protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void PreSubclassWindow();

	static LRESULT CALLBACK SelfProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	//自定义消息回调函数
	DECLARE_MESSAGE_MAP()

private:
	WNDPROC mWndProc;
	ENUM_ORIENT miOrient;
	void DrawBorder(HWND hWnd, HDC hDC);
	void DrawTabs(HWND hWnd, HDC hDC);
};
#endif