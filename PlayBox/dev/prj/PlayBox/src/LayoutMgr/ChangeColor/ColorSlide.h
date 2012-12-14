#pragma once

#define TRACKBAR_INTERVAL 4       /*控件起始位置与滑跪起始位置的间隔*/
enum ENUM_SLIDERSTATE {set_Normal,set_Enter,set_Down};   
typedef enum  ColorSlideType{PERCENT_SLIDE,LIGHT_SLIDE,HUE_SLIDE, S_SLIDE}ENUM_SLIDE_TYPE;


class IEventListener;
class  CColorSlide : public CSliderCtrl
{
private:
	int FPosition;//滑块位子，同时也是属性用变量
	int FMaxValue;//滑块位子最大值，同时也是属性用变量
	int FMinValue;//滑块位子最小值，同时也是属性用变量
	
	int m_nWidth;     //控件宽
	int m_nHeight;    //控件高
	int m_nWidthPlay; //滑轨宽

	COLORREF m_colBegain;
	COLORREF m_colEnd;
	UINT     m_nHue;

	BOOL    m_bButtonDown;
	WNDPROC mWndProc;

	ENUM_SLIDE_TYPE m_SlideType;       //滑块类型
	int             m_iStatePosX;    //滑块位置（控件宽度范围）
	void            SaveSize();

	IEventListener* mp_slideEventListerner; 
public:
	CColorSlide(); 
	virtual ~CColorSlide();
public:
	void UpdateToolTips();
	void SetHintText(CString strValue);
	void SetSliderCtrlType(ENUM_SLIDE_TYPE aSlideType);
	void SetButtonPos(int aiValue);
	UINT GetButtonPos();
	void SetRangeMax(int aiMax);
	INT  GetRangeMax() const;
	void SetRangeMin(INT aiMin);
	INT  GetRangeMin() const;
	void SetRange(int nMin, int nMax);
	void GetRange(int& nMin, int& nMax) const;
	void SetTicFreq(int nFreq);
	void SetBKColor(COLORREF acolBegain, COLORREF acolEnd, UINT aHue=0);
	void SetBKColor(COLORREF acolBegain);
	void SetListener( IEventListener* apListtener ){ mp_slideEventListerner = apListtener ;}	
protected:
	//方法
	virtual void PreSubclassWindow();
	void DynamicStat(ENUM_SLIDERSTATE aState);
	void DrawSlider(ENUM_SLIDERSTATE aState,CRect aRect,HDC hMemDC);
	void PaintBK(CRect aRect,HDC hMemDC);
	static LRESULT CALLBACK SelfProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	//自定义消息回调函数
	
protected:
	//重载消息响应方法
	//{{AFX_MSG(CColorSlide)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};