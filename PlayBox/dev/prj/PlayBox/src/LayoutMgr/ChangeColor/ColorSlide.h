#pragma once

#define TRACKBAR_INTERVAL 4       /*�ؼ���ʼλ���뻬����ʼλ�õļ��*/
enum ENUM_SLIDERSTATE {set_Normal,set_Enter,set_Down};   
typedef enum  ColorSlideType{PERCENT_SLIDE,LIGHT_SLIDE,HUE_SLIDE, S_SLIDE}ENUM_SLIDE_TYPE;


class IEventListener;
class  CColorSlide : public CSliderCtrl
{
private:
	int FPosition;//����λ�ӣ�ͬʱҲ�������ñ���
	int FMaxValue;//����λ�����ֵ��ͬʱҲ�������ñ���
	int FMinValue;//����λ����Сֵ��ͬʱҲ�������ñ���
	
	int m_nWidth;     //�ؼ���
	int m_nHeight;    //�ؼ���
	int m_nWidthPlay; //�����

	COLORREF m_colBegain;
	COLORREF m_colEnd;
	UINT     m_nHue;

	BOOL    m_bButtonDown;
	WNDPROC mWndProc;

	ENUM_SLIDE_TYPE m_SlideType;       //��������
	int             m_iStatePosX;    //����λ�ã��ؼ���ȷ�Χ��
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
	//����
	virtual void PreSubclassWindow();
	void DynamicStat(ENUM_SLIDERSTATE aState);
	void DrawSlider(ENUM_SLIDERSTATE aState,CRect aRect,HDC hMemDC);
	void PaintBK(CRect aRect,HDC hMemDC);
	static LRESULT CALLBACK SelfProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	//�Զ�����Ϣ�ص�����
	
protected:
	//������Ϣ��Ӧ����
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