#include "stdafx.h"
#include "MyGamePanelWnd.h"
#include "MyGameTreeCtrl.h"
#include "YL_StringUtil.h"
#include "../QQPanel/QQPanelDef.h"
#include "tools.h"
#include "../../gui/util/BalloonHelp.h"
#include "../../Core/CDataManager.h"

#define MY_GAME_TITLE_TEXT "我的游戏"

IMPLEMENT_DYNAMIC(MyGamePanelWnd, CBasicWnd)
MyGamePanelWnd::MyGamePanelWnd()
{	
	m_pQQTreeCtrl = NULL;
	m_iTreeSize = 1;
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_QQ_ITEM_CHANGE,(IQQItemCountChangeObserver*)this );	
}

MyGamePanelWnd::~MyGamePanelWnd()
{
	if( m_pQQTreeCtrl != NULL) 
		delete []m_pQQTreeCtrl;
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_QQ_ITEM_CHANGE,(IQQItemCountChangeObserver*)this );	
}


BEGIN_MESSAGE_MAP(MyGamePanelWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_QQSHIFT,OnWMQQShift)
END_MESSAGE_MAP()

int MyGamePanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
 	m_QQPanelWnd.Create( NULL,NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,ID_WND_MYGAME_VIEW);
	m_QQPanelWnd.m_pWndParent = this;

	CreateControls();
	AfxGetUIManager()->UIGetSkinMgr()->AddSkinWnd( this );
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_LAYOUTMGR,(ILayoutChangeObserver*) this);

	return 0;
}

void MyGamePanelWnd::OnSize(UINT nType, int cx, int cy)
{
	m_QQPanelWnd.MoveWindow(0,0,cx,cy);
	//m_QQPanelWnd.MoveWindow(2,2,cx-4,cy-4);
}

void MyGamePanelWnd::LoadSkin()
{
	ISkinMgr*	pSkinMgr = AfxGetUIManager()->UIGetSkinMgr();

	WND_SLIDER_DATA wsd;
	wsd.pBitmapIcon		= pSkinMgr->GetDibBmp( "QQPaneIconMyGame" );
	wsd.pBitmapIcon->is_ChangeColor = false;
	wsd.pBitmapIconRight= pSkinMgr->GetDibBmp( "QQPaneIconRight" );
	wsd.pBitmapIconRight->is_ChangeColor = false;

	wsd.pBitmapLeftNormal	= pSkinMgr->GetDibBmp( "QQPaneLeftNormal" );
	wsd.pBitmapLeftOver		= pSkinMgr->GetDibBmp( "QQPaneLeftOver" );
	wsd.pBitmapLeftDown		= pSkinMgr->GetDibBmp( "QQPaneLeft" );

	wsd.pBitmapMidNormal = pSkinMgr->GetDibBmp( "QQPaneMiddleNormal" );
	wsd.pBitmapMidOver	= pSkinMgr->GetDibBmp( "QQPaneMiddleOver" );
	wsd.pBitmapMidDown	= pSkinMgr->GetDibBmp( "QQPaneMiddleDown" );

	wsd.pBitmapRightNormal	= pSkinMgr->GetDibBmp( "QQPaneRightNormal" );
	wsd.pBitmapRightOver	= pSkinMgr->GetDibBmp( "QQPaneRightOver" );
	wsd.pBitmapRightDown	= pSkinMgr->GetDibBmp( "QQPaneRightDown" );

	wsd.colorFont		= pSkinMgr->GetColor( "QQBarFont");

	m_QQPanelWnd.LoadSkin(wsd, MY_GAME_TITLE_TEXT);

	for(int idx = 0;idx < m_iTreeSize;idx++ )
	{
		m_pQQTreeCtrl[idx].LoadSkin();
	}
}

void MyGamePanelWnd::CreateTreePanels( MyGameTreeCtrl* pTree,int iIndex )
{
	pTree->m_strWindowName = MY_GAME_TITLE_TEXT;
	pTree->Create( WS_VISIBLE | WS_TABSTOP | WS_CHILD | TVS_FULLROWSELECT    
		| TVS_SHOWSELALWAYS  |  TVS_HASBUTTONS | TVS_NOTOOLTIPS
		| TVS_DISABLEDRAGDROP | 0x8000,CRect(0,0,0,0),&m_QQPanelWnd, ID_TREE_QQ + iIndex );
	pTree->SetItemHeight(18);
	
	ISkinMgr*	pSkinMgr = AfxGetUIManager()->UIGetSkinMgr();

	WND_SLIDER_DATA wsd;


	wsd.strTitle.Format( "%s(%d)", MY_GAME_TITLE_TEXT, GLOBAL_LOCALGAME->ILocalGameData_GetGameCount() );
	wsd.pWnd = pTree;
	wsd.pBitmapIcon		= pSkinMgr->GetDibBmp( "QQPaneIconMyGame" );
	wsd.pBitmapIcon->is_ChangeColor = false;
	wsd.pBitmapIconRight= pSkinMgr->GetDibBmp( "QQPaneIconRight" );
	wsd.pBitmapIconRight->is_ChangeColor = false;
	wsd.pBitmapLeftNormal	= pSkinMgr->GetDibBmp( "QQPaneLeftNormal" );
	wsd.pBitmapLeftOver		= pSkinMgr->GetDibBmp( "QQPaneLeftOver" );
	wsd.pBitmapLeftDown		= pSkinMgr->GetDibBmp( "QQPaneLeftDown" );

	wsd.pBitmapMidNormal	= pSkinMgr->GetDibBmp( "QQPaneMiddleNormal" );
	wsd.pBitmapMidOver		= pSkinMgr->GetDibBmp( "QQPaneMiddleOver" );
	wsd.pBitmapMidDown		= pSkinMgr->GetDibBmp( "QQPaneMiddleDown" );

	wsd.pBitmapRightNormal	= pSkinMgr->GetDibBmp( "QQPaneRightNormal" );
	wsd.pBitmapRightOver	= pSkinMgr->GetDibBmp( "QQPaneRightOver" );
	wsd.pBitmapRightDown	= pSkinMgr->GetDibBmp( "QQPaneRightDown" );

	wsd.colorFont		= pSkinMgr->GetColor( "QQBarFont");

	m_QQPanelWnd.CreateView(wsd);	
	m_vecQQHwnd.push_back( pTree->m_hWnd );
}

void MyGamePanelWnd::CreateControls()
{
	m_vecQQHwnd.clear();

	YL_Log( "qq.txt",LOG_DEBUG,"QQPanelWnd::OnCreate","1");

	m_pQQTreeCtrl = new MyGameTreeCtrl[m_iTreeSize];
	for( int idx = 0 ;idx < m_iTreeSize; idx++ )
	{
		CreateTreePanels( &m_pQQTreeCtrl[idx], idx);
	}
}

void MyGamePanelWnd::AddLayers()
{	
	LoadSkin();
}

void MyGamePanelWnd::SelectQQView(const string&  str)
{
	int iView = m_QQPanelWnd.GetViewCount();
	int iLen  = (int)str.size();
	CString strBtnTitle;

	for(int i = 0;i < iView;i++ )
	{
		m_QQPanelWnd.GetButtonTitle(i,strBtnTitle);
		if( str == string(strBtnTitle.Left(iLen)) )
		{
			m_QQPanelWnd.MySetActiveView( i );
			break;
		}
	}
}

void MyGamePanelWnd::ILayoutChangeOb_SkinChanged(string oldSkinPath, string newSkinPath)
{
}

LRESULT MyGamePanelWnd::OnWMQQShift( WPARAM wParam,LPARAM lParam )
{
	if( lParam == 1 )
	{
	}
	else
	{
	}
	TAB_ITEM ti;
	ti.strName = "我的游戏";
	ti.strParam = "";
	ti.eumType  = TAB_PLAYED_GAME;
	GLOBAL_TABBARDATA->ITabBar_ChangeTab( ti );
	return 0;
}

void MyGamePanelWnd::IQQItemChangeOb_CountChange( unsigned int iNewCount )
{
	CString strTitle;

	int iViewCount = m_QQPanelWnd.GetViewCount();
	for(int i = 0;i < iViewCount;i++ )
	{
		m_QQPanelWnd.GetButtonTitle( i,strTitle );
		if( strTitle.Find( MY_GAME_TITLE_TEXT ) != -1 )
		{
			strTitle.Format( "%s(%d)", MY_GAME_TITLE_TEXT, iNewCount );
			m_QQPanelWnd.SetButtonTitle( i,strTitle );
			break;
		}
		
	}
}