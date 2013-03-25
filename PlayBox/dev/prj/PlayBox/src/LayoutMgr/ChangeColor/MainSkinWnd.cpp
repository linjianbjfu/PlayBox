// MainSkinWnd.cpp : implementation file
//

#include "stdafx.h"
#include "MainSkinWnd.h"
#include "TabCtrlEx.h"

#include "BitMapOperate.h"
#include "WndColorMgr.h"
#include "IChangeColorMgr.h"

#include "../../AppConfig/config/ConfigSettingDef.h"
#include "SubjectChildWnd.h"

#include "../../gui/util/WToolTips.h"

#define MAINSKINWND_WIDTH		300
#define MAINSKINWND_HEIGHT		290

extern CBitMapOperate gBitMapOperate;
extern CWndColorMgr   gWndColor;

// CMainSkinWnd

IMPLEMENT_DYNCREATE(CMainSkinWnd, CFrameWnd)

CMainSkinWnd::CMainSkinWnd()
{
	m_hight = MAINSKINWND_HEIGHT;
	m_width = MAINSKINWND_WIDTH;
	m_pToolTip = new CCWToolTips();
	m_pToolTip->m_hParent	 = m_hWnd;
	m_pToolTip->CreateEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST,TOOLTIPS_CLASS,"",0,CRect(0,0,0,0),NULL,0);	
	m_pToolTip->m_clrBK = RGB(0xFF, 0xFF, 0xE1);
	m_pToolTip->SetChangeColor(0);
}

CMainSkinWnd::~CMainSkinWnd()
{
	//有时候关闭换肤窗口后，主窗口会跑到其他程序窗口的后面。用下面代码将它带到前台。
	::SetForegroundWindow(AfxGetMainWindow()->m_hWnd);
	if( m_pToolTip)
	{
		delete m_pToolTip;
		m_pToolTip = NULL;
	}
	vector<CSkinButton2*>::iterator itor = m_colbtnVec.begin();
	while(itor != m_colbtnVec.end())
	{
		delete (*itor);
		itor++;
	}
	m_colbtnVec.clear();
}

BEGIN_MESSAGE_MAP(CMainSkinWnd, CFrameWnd)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_MOVE()
	ON_COMMAND(ID_CLOSE_CHANGESKIN_BTN,OnCloseDlg)
	ON_COMMAND(ID_SUB_CHANGESKIN_BTN,OnBtnSub)
	ON_COMMAND(ID_SKIN_CHANGESKIN_BTN,OnBtnSkin)
	ON_COMMAND(ID_WEBSUB_CHANGESKIN_BTN,OnBtnWebSub)
	ON_COMMAND(ID_DOWNMORE_CHANGESKIN_BTN,OnClickDownMore)
	ON_COMMAND_RANGE(ID_CLO_BTN_BEGAIN,ID_CLO_BTN_BEGAIN+13, OnColor)
	ON_COMMAND(ID_MORECOLOR_CHANGESKIN_BTN, OnClickMoreCol)	
	ON_MESSAGE(MSG_CHANGE_DEF_BTN_COLOR,OnChangeBtnColor)
	ON_WM_LBUTTONDOWN()
//	ON_WM_SIZE()

//	ON_WM_TIMER()
END_MESSAGE_MAP()
void CMainSkinWnd::ShowToolTip(BOOL bShow)
{
	if(bShow)
	{
		CRect rc;
		GetWindowRect(&rc);
		m_pToolTip->ShowToolTips(m_ToolTipText, CRect(rc.left,rc.top +238,rc.left +300 ,rc.top + 260), 2000);
	}
	else 
		m_pToolTip->HideTooTips();
}

void CMainSkinWnd::SetToolTip(CString str)
{
	m_ToolTipText = str;
}
void CMainSkinWnd::OnCloseDlg()
{
	this->DestroyWindow();
	CSkinMgrV1::GetObject()->CloseSkinWnd();
}

void CMainSkinWnd::OnMove(int x, int y)
{
	CRect rctip;
	rctip.SetRect(x,y+240,x+300,y+260);
	m_pToolTip->MoveWindow(&rctip);
}

void CMainSkinWnd::OnColor(UINT nID)
{
	//if( nID-ID_CLO_BTN_BEGAIN == 0 ) return;
	if( m_colbtnVec.size() > 0 )
	{
		for( int i=0; i<m_colbtnVec.size(); i++ )
		{
			if( m_colbtnVec[i]->GetCheck() == 1 )
				m_colbtnVec[i]->SetCheck(0);
		}
		if( m_colBtnMore.GetCheck() ) m_colBtnMore.SetCheck(0);

		m_colbtnVec[nID-ID_CLO_BTN_BEGAIN]->SetCheck(1);
		COLORREF col = m_colbtnVec[nID-ID_CLO_BTN_BEGAIN]->GetBKColor();

		if( nID == ID_CLO_BTN_BEGAIN )
		{
			OneSubject* s = AfxGetUIManager()->UIGetSkinMgr()->GetCurSubject();

			if( AfxGetUIManager()->UIGetSkinMgr()->GetCurSubject() == NULL )
			{					 
				mp_EventListener->OnSendChangeSkin(AfxGetUIManager()->UIGetSkinMgr()->GetSkinPath());
			}
			else if( s != NULL && s->subColor == 0 )
			{
				mp_EventListener->OnSendChangeSubject(AfxGetUIManager()->UIGetSkinMgr()->GetSkinPath(),s->strSubName.c_str());
				AfxGetUIManager()->UIGetChangeColorMgr()->OnSendChangeColor( s->subColor,s->iLight ,false );
			}
			else
			{
				mp_EventListener->OnSendChangeColor(col,120,false );
			}
		}
		else
		{
			mp_EventListener->OnSendChangeColor(col,120,true );
		}

		::AfxGetUserConfig()->SetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_COLOR_BTN_INDEX,nID-ID_CLO_BTN_BEGAIN,true);
	}
}

// CMainSkinWnd message handlers

BOOL CMainSkinWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style=WS_POPUP;     
	cs.dwExStyle|=WS_EX_TOOLWINDOW; 	

	COLORREF col1;
	OneSubject* pCurSub = AfxGetUIManager()->UIGetSkinMgr()->GetCurSubject();
	if( pCurSub != NULL )
	{
		col1 = pCurSub->subColor;
	}
	else
	{
		col1 = AfxGetUIManager()->UIGetLayoutMgr()->GetSkinColor();
	}


	if( col1 == 0 )
	{
		col1 = gBitMapOperate.HLStoRGB( ((double)145)/240  ,((double)50)/100,((double)45)/100 );
	}

	COLORREF col2 = gBitMapOperate.HLStoRGB( ((double)201)/240 ,((double)60)/100,((double)44)/100 );
	COLORREF col3 = gBitMapOperate.HLStoRGB( ((double)225)/240   ,((double)50)/100,((double)50)/100 );
	COLORREF col4 = gBitMapOperate.HLStoRGB( ((double)89)/240  ,((double)60)/100,((double)30)/100 );
	COLORREF col5 = gBitMapOperate.HLStoRGB( ((double)6)/240   ,((double)60)/100,((double)45)/100 );
	COLORREF col6 = gBitMapOperate.HLStoRGB( ((double)30)/240   ,((double)50)/100,((double)50)/100 );
	COLORREF col7 = gBitMapOperate.HLStoRGB( ((double)110)/240  ,((double)60)/100,((double)35)/100 );
	COLORREF col8 = gBitMapOperate.HLStoRGB( ((double)145)/240  ,((double)50)/100,((double)45)/100 );
	COLORREF col9  = gBitMapOperate.HLStoRGB(((double)18)/240   ,((double)50)/100,((double)50)/100 );
//	COLORREF col10 = gBitMapOperate.HLStoRGB(((double)225)/240   ,((double)50)/100,((double)50)/100 );
//	COLORREF col11 = gBitMapOperate.HLStoRGB(((double)98)/240    ,((double)68)/100,((double)44)/100 );


	vector<COLORREF> colV;
	colV.push_back(col1);
	colV.push_back(col2);
	colV.push_back(col3);
	colV.push_back(col4);
	colV.push_back(col5);
	colV.push_back(col6);
	colV.push_back(col7);
	colV.push_back(col8);
	colV.push_back(col9);
//	colV.push_back(col10);
//	colV.push_back(col11);	


	for( int i=0; i<9; i++ )
	{
		CSkinButton2* pBtn = new CSkinButton2;
		pBtn->SetBKColor(255,GetRValue( colV[i] ),GetGValue( colV[i] ),GetBValue( colV[i] ) );
		pBtn->m_enmuDrawType = CSkinButton2::NO_FRAME_SELECT;

		m_colbtnVec.push_back( pBtn );		
	}

	return CFrameWnd::PreCreateWindow(cs);
}

BOOL CMainSkinWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return false;
//	return CFrameWnd::OnEraseBkgnd(pDC);
	//return true;
}

int CMainSkinWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
	SetWindowLong(m_hWnd,GWL_EXSTYLE,GetWindowLong(m_hWnd,GWL_EXSTYLE)^WS_EX_LAYERED);
	SetLayeredWindowAttributes(RGB(255,0,0),0,LWA_COLORKEY);

	AfxGetUIManager()->UIGetLayoutMgr()->CreateBmpPane( this,"ChangeSkin","normal" );


	CRect lrectClient;
	GetClientRect(&lrectClient);
	m_BtnClose.Create(NULL,WS_VISIBLE ,CRect(0,0,0,0), this,ID_CLOSE_CHANGESKIN_BTN);
	m_btnSub.Create(NULL,WS_VISIBLE|BS_AUTOCHECKBOX ,CRect(0,0,0,0), this,ID_SUB_CHANGESKIN_BTN);
	m_btnSkin.Create(NULL,WS_VISIBLE|BS_AUTOCHECKBOX,CRect(0,0,0,0), this,ID_SKIN_CHANGESKIN_BTN);

	m_btnWebSub.Create(NULL,WS_VISIBLE|BS_AUTOCHECKBOX,CRect(0,0,0,0), this,ID_WEBSUB_CHANGESKIN_BTN);

	m_btnMoreColr.Create(NULL,WS_VISIBLE,CRect(0,0,0,0), this,ID_MORECOLOR_CHANGESKIN_BTN);
	m_btnDownMore.Create(NULL,WS_VISIBLE,CRect(0,0,0,0), this,ID_DOWNMORE_CHANGESKIN_BTN);

	ILayoutMgr*	pLayoutMgr = AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->RegisterCtrl( this,"ChangeSkin_Close",&m_BtnClose );
	pLayoutMgr->RegisterCtrl( this,"Sub",&m_btnSub);
	pLayoutMgr->RegisterCtrl( this,"Skin",&m_btnSkin);
	pLayoutMgr->RegisterCtrl( this,"WebSub",&m_btnWebSub);
	pLayoutMgr->RegisterCtrl( this,"MoreColor",&m_btnMoreColr);
	pLayoutMgr->RegisterCtrl( this,"DownMore",&m_btnDownMore);
	pLayoutMgr->CreateControlPane( this,"ChangeSkin","normal");	

	//m_btnWebSub.SetCheck(1);
	//m_btnSkin.SetCheck(0);
	//m_btnSub.SetCheck(0);
	//去掉web主题和皮肤，只剩下我的主题
	m_btnWebSub.SetCheck(0);
	m_btnSkin.SetCheck(0);
	m_btnSub.SetCheck(1);

	vector<CSkinButton2*>::iterator itor = m_colbtnVec.begin();

	string  skinPath =AfxGetUIManager()->UIGetSkinMgr()->GetSkinPath();
	size_t idx = skinPath.find_last_of('\\');
	if( idx != string::npos )
	{
		skinPath = skinPath.substr(0,idx);
		skinPath += "\\";
		skinPath += STR_DEFAULT_SKIN;
	}	

	int iColorSelect = 0;
	::AfxGetUserConfig()->GetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_COLOR_BTN_INDEX,iColorSelect);
	int nIndex=0;
	int col_btn_t;

	col_btn_t = COL_BTN_TOP;

	while( itor != m_colbtnVec.end() )
	{
		(*itor)->Create( NULL, WS_CHILD | WS_VISIBLE|BS_OWNERDRAW,
			CRect( lrectClient.left+ COL_BTN_LEFT + nIndex*(18+COL_BTN_INTER),
			col_btn_t,
			lrectClient.left+ COL_BTN_LEFT + nIndex*(18+COL_BTN_INTER)+ 18,
			col_btn_t+18
			),
			this,
			ID_CLO_BTN_BEGAIN + nIndex
			);
		
		(*itor)->SetSkin(CString(skinPath.c_str()) + CString("\\png\\") + "right.png");
		if( nIndex == iColorSelect )
			(*itor)->SetCheck(1);
		(*itor)->ShowWindow(SW_SHOW);			
		itor++;
		nIndex++;
	}

	m_colBtnMore.Create(  NULL, WS_CHILD | WS_VISIBLE|BS_OWNERDRAW,
		CRect(lrectClient.left+ COL_BTN_LEFT + nIndex*(18+COL_BTN_INTER),// lrectClient.left+ COL_BTN_LEFT ,
		col_btn_t,
		lrectClient.left+ COL_BTN_LEFT + nIndex*(18+COL_BTN_INTER)+ 18,//lrectClient.left+ COL_BTN_LEFT + 18,
		col_btn_t+18
		),
		this,
		ID_MORE_COLOR
		);
	m_colBtnMore.ShowWindow(SW_HIDE);


	m_colBtnMore.m_enmuDrawType = CSkinButton2::NO_FRAME_SELECT;
	m_colBtnMore.m_bHandMouse = FALSE;
	int iUserColor = 0;
	::AfxGetUserConfig()->GetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_COLOR_USER,iUserColor);
	if( iUserColor == 0 )
	{
		m_colBtnMore.SetBKColor(200,200,0,0 );
	}
	else
	{
		m_colBtnMore.SetBKColor(255,GetRValue(iUserColor),GetGValue(iUserColor),GetBValue(iUserColor) );
	}

	m_colBtnMore.SetSkin(CString(skinPath.c_str()) + CString("\\png\\") + "right.png");

	if( iColorSelect == -1 )
	{
		m_colBtnMore.SetCheck(1);
	}
	SetTimer(111, 50, NULL);
	return 0;
}
void CMainSkinWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	AfxGetUIManager()->UIGetLayoutMgr()->PaintBkGround( m_hWnd,&dc,true );
}
void CMainSkinWnd::CloseWindows()
{
	OnClose();
}

void CMainSkinWnd::LoadSkin()
{
}
void CMainSkinWnd::OnClickMoreCol()
{
	int iColor;
	::AfxGetUserConfig()->GetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_COLOR_USER,iColor);
	m_colDefault = iColor;

	CColorDialog dlg(m_colDefault,0, this);
	if(dlg.DoModal() == IDOK)
	{
		m_colDefault = dlg.m_cc.rgbResult;
		m_colBtnMore.SetBKColor(255,GetRValue( m_colDefault),GetGValue( m_colDefault ),GetBValue( m_colDefault ) );
		m_colBtnMore.SetCheck(1);
		for( int i=0; i<m_colbtnVec.size(); i++ )
		{
			if( m_colbtnVec[i]->GetCheck() == 1 )
				m_colbtnVec[i]->SetCheck(0);
		}
		mp_EventListener->OnSendChangeColor(m_colDefault,120);
		::AfxGetUserConfig()->SetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_COLOR_BTN_INDEX,-1,true);
		::AfxGetUserConfig()->SetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_COLOR_USER,m_colDefault,true);
	}
}

void CMainSkinWnd::OnClickDownMore()
{
	ShellExecute(NULL, "open", _T("iexplore"), "http://www.kuwo.cn/skin/", NULL, SW_SHOWNORMAL);
}

LRESULT CMainSkinWnd::OnChangeBtnColor(WPARAM wParam, LPARAM lParam)
{	
	COLORREF colorref = (COLORREF)wParam;

	if( colorref == 0 )
	{
		colorref = gBitMapOperate.HLStoRGB( ((double)145)/240  ,((double)50)/100,((double)45)/100 );
	}
	m_colBtnMore.SetCheck(0);
	for( int i=0; i<m_colbtnVec.size(); i++ )
	{		
		m_colbtnVec[i]->SetCheck(0);
	}
	m_colbtnVec[0]->SetCheck(1);
	m_colbtnVec[0]->SetBKColor(255,GetRValue( colorref ),GetGValue( colorref ),GetBValue( colorref ) );

	return 1;
}

void CMainSkinWnd::OnBtnSub()
{
	m_btnSub.SetCheck(1);
	m_btnSkin.SetCheck(0);
	m_btnWebSub.SetCheck(0);

	if( mp_WndListener )
	{
		mp_WndListener->OnTabSelChange(0);
	}
}

void CMainSkinWnd::OnBtnSkin()
{
	m_btnSub.SetCheck(0);
	m_btnSkin.SetCheck(1);
	m_btnWebSub.SetCheck(0);

	if( mp_WndListener )
	{
		mp_WndListener->OnTabSelChange(1);
	}
}
void CMainSkinWnd::OnBtnWebSub()
{
	m_btnSub.SetCheck(0);
	m_btnSkin.SetCheck(0);
	m_btnWebSub.SetCheck(1);

	if( mp_WndListener )
	{
		mp_WndListener->OnTabSelChange(2);
	}
}

void CMainSkinWnd::SetCheckBtn(int ibtn)
{
	switch (ibtn)
	{
		case 0:
			m_btnSkin.SetCheck(0);
			m_btnSub.SetCheck(1);
			m_btnWebSub.SetCheck(0);
			break;
		case 1:
			m_btnSkin.SetCheck(1);
			m_btnSub.SetCheck(0);
			m_btnWebSub.SetCheck(0);
			break;
		case 2:
			m_btnSkin.SetCheck(0);
			m_btnSub.SetCheck(0);
			m_btnWebSub.SetCheck(1);
			break;
	}
}
void CMainSkinWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage( WM_NCLBUTTONDOWN, HTCAPTION, (LPARAM)&point );	
	CFrameWnd::OnLButtonDown(nFlags, point);
}


void CMainSkinWnd::ShowColorBtn(BOOL bShow)
{
	for( int i=0; i<m_colbtnVec.size(); i++ )
	{		
		m_colbtnVec[i]->ShowWindow( bShow ? SW_SHOW:SW_HIDE );
	}

//	m_colBtnMore.ShowWindow( bShow ? SW_SHOW : SW_HIDE );
	m_btnMoreColr.ShowWindow( bShow ? SW_SHOW : SW_HIDE );
	m_btnDownMore.ShowWindow( bShow ? SW_SHOW : SW_HIDE );
}