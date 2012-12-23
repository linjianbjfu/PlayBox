#include "stdafx.h"
#include "resource.h"
#include "BrowserPanelWnd.h"
#include "../WebInteract/MyWebBrowserWnd.h"
#include "../WebInteract/WebManager.h"
#include "../../core/CDataManager.h"
#include "../../gui/CommonControl/xSkinButton.h"
#include "../../gui/CommonControl/xStaticText.h"
#include "../../Gui/CommonControl/EditEx.h"

IMPLEMENT_DYNAMIC(BrowserPanelWnd, CBasicWnd)
BrowserPanelWnd::BrowserPanelWnd()
{
	m_pBtnBack = new CxSkinButton();
	m_pBtnForward = new CxSkinButton();
	m_pBtnRefresh = new CxSkinButton();
	m_pBtnFav = new CxSkinButton();
	m_pEditAddress = new CEditEx();
	m_pWndBrowser = new MyWebBrowserWnd();
}

BrowserPanelWnd::~BrowserPanelWnd()
{
	delete m_pBtnBack;
	delete m_pBtnForward;
	delete m_pBtnRefresh;
	delete m_pBtnFav;
	delete m_pEditAddress;
	//do not delete m_pWndBrowser
	//delete m_pWndBrowser;
}

BEGIN_MESSAGE_MAP(BrowserPanelWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_BROWSER_BACK,OnClickedBack)
	ON_BN_CLICKED(IDC_BTN_BROWSER_FORWARD,OnClickedForward)
	ON_BN_CLICKED(IDC_BTN_BROWSER_REFRESH,OnClickedRefresh)
	ON_BN_CLICKED(IDC_BTN_BROWSER_FAV,OnClickedFav)
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()

int BrowserPanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectNull(0,0,0,0);
	m_pBtnBack->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_BROWSER_BACK);
	m_pBtnForward->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_BROWSER_FORWARD);
	m_pBtnRefresh->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_BROWSER_REFRESH);
	m_pBtnFav->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_BROWSER_FAV);
	m_pEditAddress->Create(WS_VISIBLE,rectNull,this,IDC_BTN_BROWSER_ADDRESS_BAR);
	m_pWndBrowser->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,IDC_BROWSER_WEB );

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();	
	pLayoutMgr->RegisterCtrl( this, "BrowserBack", m_pBtnBack );
	pLayoutMgr->RegisterCtrl( this, "BrowserForward", m_pBtnForward );
	pLayoutMgr->RegisterCtrl( this, "BrowserRefresh", m_pBtnRefresh );
	pLayoutMgr->RegisterCtrl( this, "BrowserFav", m_pBtnFav );
	pLayoutMgr->RegisterCtrl( this, "BrowserAddressBar", m_pEditAddress );
	pLayoutMgr->RegisterCtrl( this, "BrowserWeb", m_pWndBrowser );

	pLayoutMgr->CreateControlPane( this, "browserpanel", "normal" );
	pLayoutMgr->CreateBmpPane( this,"browserpanel","normal" );

	return 0;
}

void BrowserPanelWnd::OnDestroy()
{
	m_pWndBrowser->DestroyWindow();
	__super::OnDestroy();
}

void BrowserPanelWnd::OnClickedBack()
{
	m_pWndBrowser->GoBack();
}

void BrowserPanelWnd::OnClickedForward()
{
	m_pWndBrowser->GoForward();
}

void BrowserPanelWnd::OnClickedRefresh()
{
	m_pWndBrowser->Refresh();
}

void BrowserPanelWnd::OnClickedFav()
{
	//
}

void BrowserPanelWnd::Navigate(string strUrl)
{
	if (strUrl.empty())
	{
		strUrl = "about:blank";
	}
	m_pWndBrowser->Navigate(strUrl);
}

void BrowserPanelWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( ( GetKeyState(VK_CONTROL) & 0xF0000000) &&
		( nChar=='A' || nChar=='a') )
	{
		return;
	}
}