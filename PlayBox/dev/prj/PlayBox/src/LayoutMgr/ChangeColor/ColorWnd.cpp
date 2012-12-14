// ColorWnd.cpp : implementation file
//

#include "stdafx.h"
#include "../ISkinMgr.h"
#include "ColorWnd.h"

#include "BitMapOperate.h"
#include "WndColorMgr.h"

extern CBitMapOperate gBitMapOperate;
extern CWndColorMgr   gWndColor;

// CColorWnd

IMPLEMENT_DYNCREATE(CColorWnd, CFrameWnd)

CColorWnd::CColorWnd()
{

}

CColorWnd::~CColorWnd()
{
	m_colBtnList_itor = m_colBtnList.begin();
	while( m_colBtnList_itor != m_colBtnList.end() )
	{
		delete (*m_colBtnList_itor);
		*m_colBtnList_itor = NULL;
		m_colBtnList_itor++;
	}
}

BEGIN_MESSAGE_MAP(CColorWnd, CFrameWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CColorWnd message handlers

void CColorWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	RECT lrect;
	::GetClientRect( this->m_hWnd, &lrect);
	//gBitMapOperate.FillRectangle(dc.m_hDC, lrect,  RGB(255,0,0)  , true ) ;
	gBitMapOperate.DrawLine(dc.m_hDC, RGB(170,170,170), 1, lrect.left+7, LINE_TOP, lrect.right-7, LINE_TOP, true);
	InitDrawText(&dc);
}

void CColorWnd::CreateClrButtons()
{

	CRect lrectClient;
	GetClientRect(&lrectClient);

	int lnBtnWidth  =  (lrectClient.Width() - LIFT_SPACE - RIGHT_SPACE)/ROW_COUNT;
	int lnBtnHeight =  (lrectClient.Height() - TOP_SPACE - BOTTOM_SPACE)/LINE_COUNT;
	int lnIndex = 0;

	m_colBtnList_itor = m_colBtnList.begin();
	while( m_colBtnList_itor != m_colBtnList.end() )
	{
		if( lnIndex != 0 )
		{
			(*m_colBtnList_itor)->Create
				(
				NULL,
				WS_CHILD | WS_VISIBLE|BS_OWNERDRAW,
				CRect(
				lrectClient.left + LIFT_SPACE + (lnIndex%ROW_COUNT)*lnBtnWidth, 
				lrectClient.top + TOP_SPACE + (int)(lnIndex/ROW_COUNT)*lnBtnHeight,
				lrectClient.left + LIFT_SPACE+lnBtnWidth + (lnIndex%ROW_COUNT)*lnBtnWidth, 
				lrectClient.top + TOP_SPACE + lnBtnHeight+(int)(lnIndex/ROW_COUNT)*lnBtnHeight
				),
				this,
				BOTTON_BEGAIN_ID+lnIndex
				);
			(*m_colBtnList_itor)->ShowWindow(SW_SHOW);
			(*m_colBtnList_itor)->SetListener(this);
		}
		else
		{
			(*m_colBtnList_itor)->Create
				(
				NULL,
				WS_CHILD | WS_VISIBLE|BS_OWNERDRAW,
				CRect(
				lrectClient.left + LIFT_SPACE, 
				lrectClient.top + TOP_SPACE,
				lrectClient.left + LIFT_SPACE+ lnBtnWidth+3 , 
				lrectClient.top + TOP_SPACE + lnBtnHeight
				),
				this,
				BOTTON_BEGAIN_ID+lnIndex
				);
			(*m_colBtnList_itor)->ShowWindow(SW_SHOW);
			(*m_colBtnList_itor)->SetListener(this);
		}
		lnIndex++;
		(*m_colBtnList_itor)->ShowWindow(SW_SHOW);
		m_colBtnList_itor++;
	}
}
int CColorWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);

	CRect lrectClient;
	GetClientRect(&lrectClient);

	m_defaultBotton.SetBottonType(m_defaultBotton.DEFAULT_BOTTON);
	m_defaultBotton.SetText( _T("默认颜色") );
	m_defaultBotton.SetColor( RGB( 64,64, 191), RGB( 64,64, 191) );
	m_defaultBotton.Create(NULL,WS_CHILD | WS_VISIBLE|BS_OWNERDRAW, CRect(lrectClient.left+25,
		LINE_TOP -DEFAULT_BOTTON_WIDTH -3, lrectClient.right -25, LINE_TOP-3),this, BOTTON_BEGAIN_ID-1);
	m_defaultBotton.ShowWindow(SW_SHOW);	
	m_defaultBotton.SetListener(this);

	this->CreateClrButtons();

	m_LightSlide.Create(  WS_VISIBLE, 
		CRect(lrectClient.left+SLIDE_LIFT_SPACE,lrectClient.top+LIGHT_SLIDE_TOP,lrectClient.right-SLIDE_RIGHT_SPACE,lrectClient.top+LIGHT_SLIDE_BUTTON),
		this, BOTTON_BEGAIN_ID-2);
	m_LightSlide.SetListener(this);
	m_LightSlide.SetSliderCtrlType(LIGHT_SLIDE);
	m_LightSlide.SetBKColor(RGB(0,0,0),RGB(255,255,255),70 );
	m_LightSlide.SetRange(0,240);
	m_LightSlide.SetButtonPos(120);

	m_HueSlide.Create(  WS_VISIBLE, 
		CRect(lrectClient.left+SLIDE_LIFT_SPACE,lrectClient.top+HUE_SLIDE_TOP,lrectClient.right-SLIDE_RIGHT_SPACE,lrectClient.top+HUE_SLIDE_BUTTON),
		this, BOTTON_BEGAIN_ID-3);
	m_HueSlide.SetListener(this);
	m_HueSlide.SetSliderCtrlType(HUE_SLIDE);
	//m_HueSlide.SetBKColor(RGB(255,0,0),RGB(255,0,6) );
	m_HueSlide.SetBKColor(gBitMapOperate.HLStoRGB(m_nBaseHue, 0.5, 1),
			gBitMapOperate.HLStoRGB(m_nBaseHue - 0.1, 0.5, 1));
	m_HueSlide.SetRange(0,239);
	m_HueSlide.SetButtonPos(0);

	m_SSlide.Create(  WS_VISIBLE, 
	CRect(lrectClient.left+SLIDE_LIFT_SPACE,lrectClient.top+S_SLIDE_TOP,lrectClient.right-SLIDE_RIGHT_SPACE,lrectClient.top+S_SLIDE_BUTTON),
		this, BOTTON_BEGAIN_ID-4);
	m_SSlide.SetListener(this);
	m_SSlide.SetSliderCtrlType(S_SLIDE);
	m_SSlide.SetBKColor(RGB(255,0,0),RGB(255,0,6) );
	m_SSlide.SetRange(0,239);
	m_SSlide.SetButtonPos(120);
	return 0;
}

BOOL CColorWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style=WS_CHILD;     
	//cs.dwExStyle|=WS_EX_TOOLWINDOW; 
	return CFrameWnd::PreCreateWindow(cs);
}

bool CColorWnd::RefreshColorButtons(_ChangeColor changecolor)
{
	m_colBtnList_itor = m_colBtnList.begin();

	while( m_colBtnList_itor != m_colBtnList.end() )
	{
		(*m_colBtnList_itor)->DestroyWindow();
		delete (*m_colBtnList_itor);
		*m_colBtnList_itor = NULL;
		m_colBtnList_itor++;
	}
	m_colBtnList.clear();
	
	for(int i = 0 ; i<changecolor.vecBaseColors.size(); i++)
	{
		basecolor b = changecolor.vecBaseColors[i];
		InsertBaseColTemplateNew(b.name, b.hue,b.sat,b.light, changecolor.baseHue);
	}

	this->CreateClrButtons();
	return true;
}

void CColorWnd::InsertBaseColTemplateNew(string &astrName, int hue, int sat, int light, int basehue)
{
	float fh = (float)hue / 240;
	if (0 == astrName.length() ) return ;

	m_nBaseHue = (float)basehue / 240;
	CSkinButton* lpBtnSkin = new CSkinButton();
	lpBtnSkin->light = (float)light/100;
	lpBtnSkin->sat = (float)sat/100;
	lpBtnSkin->SetText( CString(astrName.c_str() ) );
	lpBtnSkin->SetBottonType(1);
	lpBtnSkin->SetColor( gBitMapOperate.HLStoRGB(fh, 0.5, lpBtnSkin->sat),  gBitMapOperate.HLStoRGB(fh, 0.6, lpBtnSkin->sat));

	fh = fh - (float)basehue/ 240;
	if(fh > 1.0)
		fh = fh - 1.0;
	if(fh < 0)
		fh += 1.0;

	lpBtnSkin->hue = fh;

	m_colBtnList.push_back(lpBtnSkin);
}

void CColorWnd::InsertBaseColTemplate(string &astrName, COLORREF &acolBegain, COLORREF  &acolEnd )
{
	if (0 == astrName.length() ) return ;
	CSkinButton* lpBtnSkin = new CSkinButton();
	lpBtnSkin->SetText( CString(astrName.c_str() ) );
	lpBtnSkin->SetBottonType(1);
	lpBtnSkin->SetColor( acolBegain, acolEnd);
	m_colBtnList.push_back(lpBtnSkin);
}

void CColorWnd::InitDrawText( CPaintDC* aDC )
{
	SetBkMode(aDC->m_hDC, TRANSPARENT);
	LOGFONT  lf;
	ZeroMemory(&lf,sizeof(lf));
	lf.lfHeight=-MulDiv(9,GetDeviceCaps(aDC->m_hDC,LOGPIXELSY),72);
	lf.lfWeight=FW_MEDIUM;
	lf.lfCharSet=ANSI_CHARSET;
	lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
	lf.lfQuality=DEFAULT_QUALITY;
	lf.lfPitchAndFamily=FF_DONTCARE | DEFAULT_PITCH;
	CFont font;
	font.CreateFontIndirect(&lf);
	CFont * oldFont = aDC->SelectObject(&font);

	CRect lrectClient;
	GetClientRect(&lrectClient);
	CString lstr = _T("色 调");
	DrawText(
		aDC->m_hDC,
		lstr,
		lstr.GetLength(),
		CRect(
		lrectClient.left+4, 
		lrectClient.top+HUE_SLIDE_TOP-5, 
		lrectClient.left+SLIDE_LIFT_SPACE,
		lrectClient.top+HUE_SLIDE_BUTTON-5
		),
		DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS
		);

	lstr = _T("亮 度");
	DrawText(
		aDC->m_hDC,
		lstr,
		lstr.GetLength(),
		CRect(
		lrectClient.left+4, 
		lrectClient.top+LIGHT_SLIDE_TOP-5, 
		lrectClient.left+SLIDE_LIFT_SPACE,
		lrectClient.top+LIGHT_SLIDE_BUTTON-5
		),
		DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS
		);


	lstr = _T("饱和度");
	DrawText(
		aDC->m_hDC,
		lstr,
		lstr.GetLength(),
		CRect(
		lrectClient.left+4, 
		lrectClient.top+ S_SLIDE_TOP-5, 
		lrectClient.left+SLIDE_LIFT_SPACE,
		lrectClient.top+ S_SLIDE_BUTTON-5
		),
		DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS
		);
	aDC->SelectObject(oldFont);
}

void CColorWnd::OnDestroy()
{
	CFrameWnd::OnDestroy();
	m_colBtnList_itor = m_colBtnList.begin();
	while( m_colBtnList_itor != m_colBtnList.end() )
	{
		(*m_colBtnList_itor)->DestroyWindow();
		m_colBtnList_itor++;
	}
}
void CColorWnd::OnCliekButton(CButton* pBotton)
{
	COLORREF lColButton = RGB(0, 0, 0);
	if( pBotton->m_hWnd)
	{
		CSkinButton* pcolButton = (CSkinButton*)pBotton;
		this->SetColorSliderPos(pcolButton->hue, pcolButton->sat, pcolButton->light);
		//lColButton = pcolButton->GetCorlor();
		//double lH,lS,lL; 
		//gBitMapOperate.RGBtoHSL(lColButton,&lH,&lS,&lL);
		//COLORREF ddd = gBitMapOperate.HLStoRGB(lH, lS, lL);

		//COLORREF lcolBegain = gBitMapOperate.HLStoRGB(lH,0.3,0.5);
		//COLORREF lcolEnd =    gBitMapOperate.HLStoRGB(lH,0.8,0.5);
		//m_LightSlide.SetBKColor(lcolBegain,lcolEnd,(int)(pcolButton->light*239.0));
		//m_LightSlide.SetButtonPos(pcolButton->light * 239);
		//m_HueSlide.SetButtonPos( (int)(pcolButton->hue*239.0) );

		//lcolBegain = gBitMapOperate.HLStoRGB(lH,0.5,0.0);
		//lcolEnd =    gBitMapOperate.HLStoRGB(lH,0.5,1.0);
		//m_SSlide.SetBKColor(lcolBegain,lcolEnd,(int)(lH*239.0));
		//m_SSlide.SetButtonPos(pcolButton->sat * 239);
		//
		//int lnSlidePos = (int)lL*240;
	}  
	if( NULL != mp_EventListener)
	{
		if( pBotton->m_hWnd == m_defaultBotton.m_hWnd )
		{
			//this->mp_EventListener->OnSendChangeColor( lColButton,0);
			this->mp_EventListener->OnSendChangeSkin("");//使用初始颜色,保持皮肤模板不变
		}
		else
		{
			CSkinButton* pcolButton = (CSkinButton*)pBotton;
			this->mp_EventListener->OnSendChangeColor( gBitMapOperate.HLStoRGB(pcolButton->hue,pcolButton->light,pcolButton->sat),120);
		}
	}
}
void CColorWnd::OnCliekSlide(CSliderCtrl* pBotton)
{
	double lH = m_HueSlide.GetButtonPos()/240.0;
	//int llightPos =1;
	//if( pBotton->m_hWnd == m_HueSlide.m_hWnd)
	//{
	//	COLORREF lcolBegain = gBitMapOperate.HLStoRGB(lH,0.1,0.1);
	//	COLORREF lcolEnd =    gBitMapOperate.HLStoRGB(lH,0.9,0.1);
	//	m_LightSlide.SetBKColor(lcolBegain,lcolEnd,(int)(lH*239.0));
	//}
	//if( pBotton->m_hWnd == m_HueSlide.m_hWnd)
	//{
	//	COLORREF lcolBegain = gBitMapOperate.HLStoRGB(lH,0.5,0.0);
	//	COLORREF lcolEnd =    gBitMapOperate.HLStoRGB(lH,0.5,1.0);
	//	m_SSlide.SetBKColor(lcolBegain,lcolEnd,(int)(lH*239.0));
	//}

	int llightPos = m_LightSlide.GetButtonPos();
	//if( 0 == llightPos ) llightPos = 1;
	//if( 240 == llightPos) llightPos = 239;
	//m_LightSlide.SetButtonPos( llightPos );

	double lL = llightPos/240.0;
	if(lL == 0)
		lL += 0.01;
	if(lL == 1)
		lL -= 0.01;

	double lS = m_SSlide.GetButtonPos()/240.0;
	m_SSlide.SetButtonPos(m_SSlide.GetButtonPos());

	if( pBotton->m_hWnd == m_HueSlide.m_hWnd)
		SetColorSliderPos(lH, lS, lL);

	if( NULL != mp_EventListener)
	{
		llightPos = m_LightSlide.GetButtonPos();
		COLORREF test = gBitMapOperate.HLStoRGB(lH,lL,0.5);
		this->mp_EventListener->OnSendChangeColor( gBitMapOperate.HLStoRGB(lH,lL,lS),llightPos) ;
	}
}

void CColorWnd::SetColorSliderPos(float hue, float sat, float light)
{
	float fh = hue - this->m_nBaseHue;
	if(fh > 1.0)
		fh = fh - 1.0;
	if(fh < 0)
		fh += 1.0;

	COLORREF lcolBegain = gBitMapOperate.HLStoRGB(fh,0.5,0.0);
	COLORREF lcolEnd =    gBitMapOperate.HLStoRGB(fh,0.5,1.0);
	COLORREF test =    gBitMapOperate.HLStoRGB(0,0.5,0.5);

	m_SSlide.SetBKColor(lcolBegain,lcolEnd,(int)(fh*239.0));
	m_LightSlide.SetButtonPos(light * 240);
	m_SSlide.SetButtonPos(sat * 240);
	m_HueSlide.SetButtonPos(hue * 240);
}
void CColorWnd::SetSelectedColor(COLORREF aColor)
{
	double lH,lS,lL; 
	gBitMapOperate.RGBtoHSL(aColor,&lH,&lS,&lL);
	SetColorSliderPos(lH, lS, lL);
	COLORREF lcolBegain = gBitMapOperate.HLStoRGB(lH,0.3,0.5);
	COLORREF lcolEnd =    gBitMapOperate.HLStoRGB(lH,0.8,0.5);
	m_LightSlide.SetBKColor(lcolBegain,lcolEnd,(int)(lH*239.0));
	m_HueSlide.SetButtonPos( (int)(lH*239.0) );
}
void CColorWnd::SetSelectedLightPos(UINT anLightPos)
{
	m_LightSlide.SetButtonPos(anLightPos);
}