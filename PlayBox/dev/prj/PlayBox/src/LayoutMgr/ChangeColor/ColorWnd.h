#pragma once
#include <string>
#include <list>
#include "SkinBotton.h"
#include "colorslide.h"

#include "IEvent.h"
using namespace std; 

// CColorWnd frame

//class ChangeColor;
class CColorWnd : public CFrameWnd, public IEventListener
{
	DECLARE_DYNCREATE(CColorWnd)

	enum{ LIFT_SPACE = 9 , /*两边间隔*/ RIGHT_SPACE = 9 , TOP_SPACE = 35, /*上下间隔*/ BOTTOM_SPACE = 75 };
	enum{ ROW_COUNT = 5,/*每行有几列颜色模板*/ LINE_COUNT = 2 };  
	enum{ BOTTON_BEGAIN_ID = 10000 };
	enum{ LINE_TOP = 30, DEFAULT_BOTTON_WIDTH = 20 };
	enum{ HUE_SLIDE_TOP =   130, HUE_SLIDE_BUTTON =   150 };
	enum{ S_SLIDE_TOP =   174, S_SLIDE_BUTTON =   194 };
	enum{ SLIDE_LIFT_SPACE = 40, SLIDE_RIGHT_SPACE =  13 }; 
	enum{ LIGHT_SLIDE_TOP = 152, LIGHT_SLIDE_BUTTON = 172 };
public:
	CColorWnd();           // protected constructor used by dynamic creation
	virtual ~CColorWnd();

private:
	list<CSkinButton*> m_colBtnList;
	list<CSkinButton*>::iterator m_colBtnList_itor;
	CSkinButton m_defaultBotton;
	CColorSlide m_LightSlide;
	CColorSlide m_HueSlide;
	CColorSlide m_SSlide;
	IEventListener* mp_EventListener;
	float		m_nBaseHue;
    void		InitDrawText( CPaintDC* aDC );
	void		CreateClrButtons();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnSendChangeColor( COLORREF acolCol, UINT anLightValue ){;}
	virtual void OnSendChangeSkin ( CString strPath){;}
public:
	void InsertBaseColTemplate(string &astrName, COLORREF &acolBegain, COLORREF  &acolEnd );
	void CColorWnd::InsertBaseColTemplateNew(string &astrName, int hue, int sat, int light, int basehue);
	void SetSendEventListener(IEventListener* aEventListener ){mp_EventListener = aEventListener ;}
	void SetSelectedColor(COLORREF aColor);
	bool RefreshColorButtons(_ChangeColor changecolor);
	void SetSelectedLightPos(UINT anLightPos);
	void SetColorSliderPos(float hue, float sat, float light);

	afx_msg void OnDestroy();
	virtual void OnCliekButton(CButton* pBotton);
	virtual void OnCliekSlide(CSliderCtrl* pBotton);
};


