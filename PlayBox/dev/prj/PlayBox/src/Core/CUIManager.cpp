#include "stdafx.h"
#include "UIWindowID.h"
#include "CUIManager.h"
#include "LhcImg.h"
#include "YL_FileInfo.h"
#include "YL_StringUtil.h"
#include "../LayoutMgr/PanesMgr.h"
#include "../LayoutMgr/SkinMgr.h"
#include "../layoutMgr/ChangeColor/IChangeColorMgr.h"
#include "../Global/GlobalVariable.h"
#include "../gui/commoncontrol/skinscroll/coolsb_detours.h"
#include "../AppConfig/config/ConfigSettingDef.h"
#include "../AppConfig/config/ConfigSkinDef.h"
#include "../module/Toppanel/TopPanelWnd.h"
#include "../module/logopanel/LogoPanelWnd.h"
#include "../module/BottomPanel/BottomPanelWnd.h"
#include "../module/TabMan/tabbarWnd.h"
#include "../module/TabMan/TabPageParentWnd.h"
#include "../module/TabMan/TabPageControl.h"
#include "IAppExitObserver.h"

#define   DESTROY_WINDOW(x)   if(x){x->DestroyWindow();delete x;}

CUIManager::CUIManager()
{
}

CUIManager::~CUIManager()
{
}

//由系统调用，通知界面管理类程序启动
bool CUIManager::UIManAppStart()
{	
	//滚动条初始化
	CoolSB_InitializeApp();
	m_pAppSkin			= new CSkinMgr();
	m_pPanesMgr			= new CPanesMgr();
	m_pChangeColorMgr	= CSkinMgrV1::GetObject();

	string strSkinPath;
	GlobalFunc::GetSkinPath( strSkinPath );

	string strSkinConfPath;
	GlobalFunc::GetSkinConfPath( strSkinConfPath );

	if( !YL_FileInfo::IsValid( strSkinConfPath ) )
	{
		AfxGetUserConfig()->SetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_LIGHT, 120);
		AfxGetUserConfig()->SetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_COLOR, 0);
	}

	bool bSetXml = m_pPanesMgr->SetXmlPath( strSkinConfPath.c_str() );
	if( !bSetXml )
	{
		return false;
	}
	m_pAppSkin->SetSkinPath( strSkinPath.c_str() );
	m_pAppSkin->LoadSkinFromPath(strSkinPath.c_str());
	m_pPanesMgr->SetSkin( m_pAppSkin );
	m_pPanesMgr->LoadColors();
	m_pPanesMgr->LoadChangeColorData();
	//new 出各种panel
	CRect rectNull(0,0,0,0);
	AfxGetUIManager()->UIGetChangeColorMgr()->SetListener(AfxGetUIManager()->UIGetLayoutMgr());
	CWnd* pMainWnd = AfxGetMainWindow();
	
	m_pPanesMgr->CreateWndPane( pMainWnd,"playbox", true);
	m_pPanesMgr->UpdateLayout( pMainWnd->m_hWnd );
	m_pPanesMgr->CreateBmpPane(  pMainWnd, "mainBk", "11");
	string strPath = m_pAppSkin->GetSkinPath();
	string strSubPath = strPath + "\\SbujectSkin\\";	
	bool   bNotifySub = false;
	bool   bSub		  = false;
	bool   bSubError  = false;

	if( PathFileExists( strSubPath.c_str()) )
	{	
		string szCurSkin = "";
		AfxGetUserConfig()->GetConfigStringValue( CONF_SKINSUB_MODULE_NAME,CONF_SKINSUB_CURRSUB,szCurSkin);		

		if( szCurSkin != "" )
		{
			m_pPanesMgr->InsertSubjectData(szCurSkin.c_str());
			OneSubject* pSub = AfxGetUIManager()->UIGetSkinMgr()->GetOneSubject( szCurSkin.c_str() );

			if( pSub != NULL  && pSub->bNeedDown == false )
			{
				bSub = true;
				AfxGetUIManager()->UIGetChangeColorMgr()->NotifyChangeSubject( szCurSkin.c_str() );				

				int iColorIndex = 0;
				::AfxGetUserConfig()->GetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_COLOR_BTN_INDEX,iColorIndex);

				if( iColorIndex == 0 && szCurSkin != "" )
				{			
					bNotifySub = true;
				}
			}
			else
			{
				AfxGetUserConfig()->SetConfigStringValue( CONF_SKINSUB_MODULE_NAME,CONF_SKINSUB_CURRSUB,"",true);

				::AfxGetUserConfig()->SetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_LIGHT, 120);
				::AfxGetUserConfig()->SetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_COLOR, 0);

				bSubError = true;
			}
		}
		
	}
	int color, light;
	::AfxGetUserConfig()->GetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_LIGHT, light);
	::AfxGetUserConfig()->GetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_COLOR, color);	
	

	if( color == 0 && light == 120 && bSub )
	{
		OneSubject* pSub = AfxGetUIManager()->UIGetSkinMgr()->GetCurSubject();
		if( pSub )
		{
			color	 = pSub->subColor;
			light	 = pSub->iLight;
		

			::AfxGetUserConfig()->SetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_LIGHT, light);
			::AfxGetUserConfig()->SetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_COLOR, color);
		}
	}

	::AfxGetUIManager()->UIGetChangeColorMgr()->SetSelectedColor(color);

	

	if(!(color == 0 && light == 120) && !bSubError )
	{
		::AfxGetUIManager()->UIGetLayoutMgr()->OnchangeColor(color, light,!bNotifySub);
	}
	else
	{
		::AfxGetUIManager()->UIGetLayoutMgr()->LoadColors();
		::AfxGetUIManager()->UIGetSkinMgr()->LoadWndsSkin();
	}	

	m_pPanesMgr->CreateLoadSubjectTread();
	YL_Log( STR_LOG_FILE,LOG_NOTICE,"CUIManager","UIManAppStart-OUT" );

	// 打开游戏大厅首页
	CTabPageControl::GetInstance()->OpenHomePage();
	return true;
}

//由系统调用，通知界面管理类程序退出
void CUIManager::UIManAppExit()
{
	YL_Log( STR_LOG_FILE,LOG_NOTICE,"CUIManager","UIManAppExit======IN" );
	YL_Log(STR_LOG_FILE,LOG_DEBUG,"CUIManager","1");	
	this->UIGetLayoutMgr()->AppExit();
	YL_Log(STR_LOG_FILE,LOG_DEBUG,"CUIManager","2");

	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_APPEXIT,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IAppExitObserver* pOb = dynamic_cast<IAppExitObserver*>(*itOb);
		pOb->IAppExit( );
	}

	DESTROY_WINDOW(m_pWndTopPanel);
	DESTROY_WINDOW(m_pWndLogoPanel);
	DESTROY_WINDOW(m_pWndBottomPanel);
	DESTROY_WINDOW(m_pWndTabPageParent);
	DESTROY_WINDOW(m_pWndTabBar);

	delete m_pAppSkin;
	m_pAppSkin = NULL;
	YL_Log( STR_LOG_FILE,LOG_NOTICE,"CUIManager","Del AppSkin" );
	delete m_pPanesMgr;
	m_pPanesMgr = NULL;
	YL_Log( STR_LOG_FILE,LOG_NOTICE,"CUIManager","Del PanesMgr" );
	m_pChangeColorMgr->DeleteObject();	
	YL_Log( STR_LOG_FILE,LOG_NOTICE,"CUIManager","UIManAppExit" );
	CoolSB_UninitializeApp();
}

ILayoutMgr* CUIManager::UIGetLayoutMgr()
{
	return m_pPanesMgr;
}

ISkinMgr* CUIManager::UIGetSkinMgr()
{
	return m_pAppSkin;
}

IChangeColorMgr*CUIManager::UIGetChangeColorMgr()
{
	return m_pChangeColorMgr;
}
void CUIManager::UIOnPaint( CDC* pDc )
{
	DWORD count = ::GetTickCount();
	if(m_pPanesMgr != NULL)
		m_pPanesMgr->PaintBkGround( AfxGetMainWindow()->m_hWnd, pDc, false);
}

bool CUIManager::UICanExit()
{
	return m_vecDialog.size() == 0 ;
}

void CUIManager::UIAddDialog(CDialog* pDialog)
{	
	UIAddDialog((DWORD)pDialog);
}

void CUIManager::UIRemoveDialog(CDialog* pDialog)
{	
	UIRemoveDialog((DWORD)pDialog);
}

void CUIManager::UIAddDialog(DWORD dwPtr)
{
	for(size_t idx = 0;idx < m_vecDialog.size();idx++ )
	{
		if( m_vecDialog[idx] == dwPtr )
		{
			return;
		}
	}
	m_vecDialog.push_back( dwPtr );
}


void CUIManager::UIRemoveDialog(DWORD dwPtr)
{
	vector<DWORD>::iterator it = m_vecDialog.begin();

	while( it != m_vecDialog.end() )
	{
		if( (*it) == dwPtr )
		{
			m_vecDialog.erase( it );
			return;
		}
		it++;
	}
}

int CUIManager::UIDlgSize()
{
	return m_vecDialog.size();
}

void CUIManager::CreateUIWindow(const char* pszWindowName )
{
	CRect	rectNull(0,0,0,0);
	CWnd*	pMainWnd = AfxGetMainWindow();
	int		id = ID_WND_TOPPANEL;

	if( strcmp( pszWindowName,"toppanel") == 0 )
	{
		YL_Log( "createWindow.txt",LOG_NOTICE,"CreateUIWindow","%s--in",pszWindowName);

		m_pWndTopPanel			= new CTopPanelWnd();
		m_pWndTopPanel->Create(NULL,NULL,WS_CHILD,rectNull,pMainWnd,id );
		m_pPanesMgr->RegisterWnd( "toppanel", m_pWndTopPanel );

		YL_Log( "createWindow.txt",LOG_NOTICE,"CreateUIWindow","%s",pszWindowName);
		return;
	}
	else if( strcmp( pszWindowName,"logopanel") == 0 )
	{
		id++;
		YL_Log( "createWindow.txt",LOG_NOTICE,"CreateUIWindow","%s--in",pszWindowName);

		m_pWndLogoPanel		= new CLogoPanelWnd();
		m_pWndLogoPanel->Create(NULL,NULL,WS_CHILD,rectNull,pMainWnd,id );
		m_pPanesMgr->RegisterWnd( "logopanel", m_pWndLogoPanel );

		YL_Log( "createWindow.txt",LOG_NOTICE,"CreateUIWindow","%s",pszWindowName);
		return;
	}
	else if( strcmp( pszWindowName,"bottompanel") == 0 )
	{
		id++;
		YL_Log( "createWindow.txt",LOG_NOTICE,"CreateUIWindow","%s--in",pszWindowName);

		m_pWndBottomPanel		= new CBottomPanelWnd();
		m_pWndBottomPanel->Create(NULL,NULL,WS_CHILD,rectNull,pMainWnd,id );
		m_pPanesMgr->RegisterWnd( "bottompanel", m_pWndBottomPanel );

		YL_Log( "createWindow.txt",LOG_NOTICE,"CreateUIWindow","%s",pszWindowName);
		return;
	}
	else if( strcmp( pszWindowName,"tabbarWnd") == 0 )
	{
		id ++ ;
		YL_Log( "createWindow.txt",LOG_NOTICE,"CreateUIWindow","%s--in",pszWindowName);

		m_pWndTabBar			= new CTabBarWnd();
		m_pWndTabBar->Create(NULL,NULL,WS_CHILD,rectNull,pMainWnd,id );
		m_pPanesMgr->RegisterWnd( "tabbarWnd", m_pWndTabBar );

		YL_Log( "createWindow.txt",LOG_NOTICE,"CreateUIWindow","%s",pszWindowName);
		return;
	}
	else if( strcmp( pszWindowName,"midpanel") == 0 )
	{
		id++;
		YL_Log( "createWindow.txt",LOG_NOTICE,"CreateUIWindow","%s--in",pszWindowName);

		m_SplitterMid.Create("", WS_CHILD|WS_CLIPCHILDREN, rectNull,pMainWnd, id );
		m_pPanesMgr->RegisterWnd( pszWindowName ,&m_SplitterMid);

		YL_Log( "createWindow.txt",LOG_NOTICE,"CreateUIWindow","%s",pszWindowName);
		return;
	}

	else if( strcmp( pszWindowName,"tabpanelparent") == 0 )
	{
		id++;
		YL_Log( "createWindow.txt",LOG_NOTICE,"CreateUIWindow","%s--in",pszWindowName);
		m_pWndTabPageParent			= new CTabPageParentWnd();
		m_pWndTabPageParent->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,pMainWnd,id );
		m_pPanesMgr->RegisterWnd( pszWindowName ,m_pWndTabPageParent);

		YL_Log( "createWindow.txt",LOG_NOTICE,"CreateUIWindow","%s",pszWindowName);
		return;
	}
}

void CUIManager::UIOnMove(int xPos,int yPos )
{
}