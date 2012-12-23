#include "stdafx.h"
#include "MyWebBrowserWnd.h"
#include "../../Core/AfxGlobals.h"
#include "../../Global/GlobalSwfPath.h"
#include "../../util/MarkupArchive.h"
#include "../../../../CommonLib/common/YL_Base64.h"
#include "../../../../CommonLib/common/LhcImg.h"
#include "../../LayoutMgr/ChangeColor/IChangeColorMgr.h"
#include "../../AppConfig/config/ConfigSettingDef.h"
#include "../../AppConfig/config/ConfigAppDef.h"
#include "../HTTPControl/JudgeFirewall.h"
#include "../HTTPControl/HTTPControl.h"
#include "WebManager.h"
#include "tools.h"
#include "YL_URLEncoder.h"
#include "YL_StringUtil.h"
#include "Wininet.h"
#include "../gui/util/CBufferDC.h"

#define TIMER_CHECK_NAVIGATE	WM_USER+108
#define ELAPSE_TIMER_CHECK_NAVIGATE	1000

#define TIMER_HIDE_LOADING		WM_USER+109
#define ELAPSE_TIMER_HIDE_LOADING	500

IMPLEMENT_DYNAMIC(MyWebBrowserWnd, CHtmlView)
MyWebBrowserWnd::MyWebBrowserWnd(BOOL bShowLoading/* = TRUE*/):m_strURL("")
{
	m_bSucceed = TRUE;
	m_bReady = FALSE;
	m_dwStartShowTime = 0;
	m_bHomePage = false; //默认不是主页
	m_bShowLoading = bShowLoading;
	AfxGetMessageManager()->AttachMessage(ID_MESSAGE_REFRESH, (IWebRefreshObserver*)this);
}

MyWebBrowserWnd::~MyWebBrowserWnd()
{
	AfxGetMessageManager()->DetachMessage(ID_MESSAGE_REFRESH, (IWebRefreshObserver*)this);
}

BEGIN_MESSAGE_MAP(MyWebBrowserWnd, CHtmlView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_CALL_JAVASCRIPT,OnCallJavaScript)	
END_MESSAGE_MAP()


BOOL MyWebBrowserWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,UINT nID, CCreateContext* pContext)
{	
	m_Mgr = new CCustomOccManager();

	m_pCreateContext = pContext;
	if (!CView::Create(lpszClassName, lpszWindowName,dwStyle, rect, pParentWnd,  nID, pContext))
	{
		return FALSE;
	}

	AfxEnableControlContainer(m_Mgr);

	RECT rectClient;
	GetClientRect(&rectClient);
	if (!m_wndBrowser.CreateControl(CLSID_WebBrowser, lpszWindowName,WS_VISIBLE | WS_CHILD, rectClient, this, AFX_IDW_PANE_FIRST))
	{
		DestroyWindow();
		return FALSE;
	}

	LPUNKNOWN lpUnk = m_wndBrowser.GetControlUnknown();
	HRESULT hr = lpUnk->QueryInterface(IID_IWebBrowser2, (void**) &m_pBrowserApp);
	if (!SUCCEEDED(hr))
	{
		m_pBrowserApp = NULL;
		m_wndBrowser.DestroyWindow();
		DestroyWindow();
		return FALSE;
	}
	return TRUE;
}

BOOL MyWebBrowserWnd::CreateControlSite(COleControlContainer* pContainer,COleControlSite** ppSite, UINT nID, REFCLSID clsid)
{
	return TRUE;
}

void MyWebBrowserWnd::Init()
{
	KillTimer( TIMER_CHECK_NAVIGATE );
	m_bSucceed = TRUE;
	m_bReady = FALSE;
	m_strURL = "";
	m_dwStartShowTime = 0;
	if (::IsWindow (m_flash.m_hWnd))
		m_flash.ShowWindow(SW_SHOW);
}

void MyWebBrowserWnd::Recycle()
{
	Init();
}

int MyWebBrowserWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( __super::OnCreate( lpCreateStruct) == -1 )
	{
		return -1;
	}

	if (m_bShowLoading)
	{
		m_flash.Create(NULL,NULL, WS_CHILD, CRect(0,0,0,0), this, ID_GAME_RIGHT_LOADING_FLASH);
		m_flash.ShowWindow(SW_SHOW);
	}

	char szBuffer[MAX_PATH];
	CLhcImg::GetHomePath(szBuffer,MAX_PATH);
	m_strErrorPagePath = szBuffer;
	m_strErrorPagePath += "\\Resources\\StandardUI\\error404.htm?r=";
	return 0;
}

BOOL MyWebBrowserWnd::OnEraseBkgnd(CDC* pDC)
{
// 	COLORREF clBak = RGB (41, 41, 41);
	COLORREF clBak = RGB (39, 39, 39);
	CRect rcClient;
	GetClientRect (&rcClient);

	pDC->FillSolidRect (&rcClient, clBak);
	
	return TRUE;
}

void MyWebBrowserWnd::Navigate( string strUrl )
{	
	Init();
	//RegisterLHProtocol();
	m_strURL = CString( strUrl.c_str() );

	m_dwStartShowTime = GetTickCount();
	//SetTimer( TIMER_CHECK_NAVIGATE, ELAPSE_TIMER_CHECK_NAVIGATE, NULL );	
	Navigate2( strUrl.c_str() );
	m_bSucceed = TRUE;
}

void MyWebBrowserWnd::OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel)
{
	if( strstr(lpszURL,"http://") != 0 )
	{
		ShowErrorPage( );
	}
	SetTimer( TIMER_HIDE_LOADING, ELAPSE_TIMER_HIDE_LOADING, NULL );
	if( m_bHomePage )
	{
		CHTTPControl::HTTPFinish_TodayRecommend();
	}
}

void MyWebBrowserWnd::ShowErrorPage()
{
	string str = m_strErrorPagePath + string(m_strURL);
	Navigate2(str.c_str(), 0, NULL);
	m_bSucceed = FALSE;

	LogUserActMsg("ERROR", "WEB_COMMEND_ERR");
	CJudgeFirewall::SetHTTPRes( HTTP_WEB_COMMAND,0 );
}

void MyWebBrowserWnd::OnNavigateComplete2( LPCTSTR strURL )
{
	if( m_strURL == strURL || strstr(strURL, "SwfGamePlaying.htm") != NULL )
	{
		char szBuf[32];
		sprintf(szBuf,"T:%d",GetTickCount() - m_dwStartShowTime );
		LogUserActMsg("WEBLOADRECOM",szBuf);
		CJudgeFirewall::SetHTTPRes( HTTP_WEB_COMMAND,1 );

		m_bReady = TRUE;
		SetTimer( TIMER_HIDE_LOADING, ELAPSE_TIMER_HIDE_LOADING, NULL );
	}else	
	if(strstr(strURL, "error404") != 0)
	{
		Refresh2(REFRESH_NORMAL);
	}
}

BOOL MyWebBrowserWnd::PreTranslateMessage(MSG* pMsg)
{
	if(WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
    {
        if(pMsg->wParam==VK_RETURN)
		{
			TranslateMessage(pMsg);
			DispatchMessage(pMsg);
			return 0;
		}
    }
	return __super::PreTranslateMessage(pMsg);
}

void MyWebBrowserWnd::OnSize(UINT nType, int cx, int cy)
{
	if (IsWindow (m_flash.m_hWnd))
		m_flash.MoveWindow(0,0,cx,cy);

	__super::OnSize(nType,cx,cy);
}

void MyWebBrowserWnd::OnDocumentComplete(LPCTSTR lpszURL)
{
	YL_Log("log_webrec.txt",LOG_DEBUG,"OnDocumentComplete: ", "%s", lpszURL);
	if( lpszURL == m_strURL || strstr(lpszURL, "SwfGamePlaying.htm") != NULL) 
	{
		m_bReady = TRUE;
		SetTimer( TIMER_HIDE_LOADING, ELAPSE_TIMER_HIDE_LOADING, NULL );
		if( m_bHomePage )
		{
			CHTTPControl::HTTPFinish_TodayRecommend();
		}
	}
	__super::OnDocumentComplete(lpszURL);
}

void MyWebBrowserWnd::OnTimer(UINT nIDEvent)
{
	if( nIDEvent == TIMER_CHECK_NAVIGATE )
	{
		DWORD dwCurTime = GetTickCount();
		if( dwCurTime - m_dwStartShowTime > 4000 )
		{
			KillTimer( nIDEvent );
			if( m_bReady == FALSE )
			{
				ShowErrorPage();
				SetTimer( TIMER_HIDE_LOADING, ELAPSE_TIMER_HIDE_LOADING, NULL );
			}
		}
	}else
	if( nIDEvent == TIMER_HIDE_LOADING )
	{
		KillTimer( nIDEvent );

		CRect rcClient;
		GetClientRect (&rcClient);
		::AdjustWindowRectEx (&rcClient, m_wndBrowser.GetStyle (), FALSE, WS_EX_CLIENTEDGE);
		m_wndBrowser.MoveWindow (&rcClient, FALSE);
		m_wndBrowser.ShowWindow (SW_SHOW);

		if (IsWindow (m_flash.m_hWnd))
		{
			CRect rcNull (0, 0, 0, 0);
			m_flash.MoveWindow (&rcNull, FALSE);
			m_flash.ShowWindow(SW_HIDE);
		}

		Refresh2 (REFRESH_IFEXPIRED);
	}
	__super::OnTimer(nIDEvent);
}

LRESULT MyWebBrowserWnd::OnCallJavaScript(WPARAM w,LPARAM l)
{
	CHtmlView* phtml = (CHtmlView*)w;
// 	string* pStr = (string*)l;
	CallJSParam * pParam = (CallJSParam*) l;

	CWebManager::GetInstance()->CallWebFromGBox( phtml, pParam/**pStr */);
	/*delete pStr;*/

	return 0;
}

void MyWebBrowserWnd::IWebRefreshOb_Refresh( const char* psz )
{
	if( m_bSucceed == FALSE )
	{
		if( strstr( psz, m_strURL.GetString() ) != NULL )
		{
			Navigate( m_strURL.GetString() );
		}
	}
}

void MyWebBrowserWnd::SetHomePage( bool bHomePage )
{
	m_bHomePage = bHomePage;
}

