//

#include "stdafx.h"
#include "AppWebWnd.h"
#include "YL_StringUtil.h"
#include "PluginDef.h"
#include "tools.h"
#include "..\gui\CommonControl\DialogMsg.h"

#define WM_CALL_JAVASCRIPT	WM_USER+1001


IMPLEMENT_DYNAMIC(CAppWebWnd, CHtmlView)

CAppWebWnd::CAppWebWnd():m_Mgr(NULL)
{	
	AfxGetMessageManager()->AttachMessage(ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*)this);
	AfxGetMessageManager()->AttachMessage(KWID_MESSAGE_REFRESH, (IWebRefreshObserver*)this);
}

CAppWebWnd::~CAppWebWnd()
{
	AfxGetMessageManager()->DetachMessage(ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*)this);
	AfxGetMessageManager()->DetachMessage(KWID_MESSAGE_REFRESH, (IWebRefreshObserver*)this);

	/*if( m_Mgr )
	{
		delete m_Mgr;
	}*/
}

BEGIN_MESSAGE_MAP(CAppWebWnd, CHtmlView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()	
	ON_MESSAGE(WM_SHOWPAGE,OnShowPage)	
	ON_MESSAGE(WM_CALL_JAVASCRIPT,OnCallJavaScript)	
END_MESSAGE_MAP()

int CAppWebWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( __super::OnCreate( lpCreateStruct) == -1 )
	{
		return -1;
	}
			
	AfxGetPluginManager()->SetWebWnd( this );
	return 0;
}

BOOL CAppWebWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;	
}


void CAppWebWnd::OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel)
{
	
	char szBuffer[MAX_PATH];
	CKwcImg::GetHomePath(szBuffer,MAX_PATH);
	string str = szBuffer;
	str += "\\html\\weberror.htm?r=1&w=app";	

	Navigate2(str.c_str(), 0, NULL);
		
}

void CAppWebWnd::IPanelChangeOb_Change( int iPos )
{
	if( iPos == 4 )
	{
		static BOOL s_bPostMessage = FALSE;

		if( s_bPostMessage )
			return;

		PostMessage(WM_SHOWPAGE);
		s_bPostMessage = TRUE;
	}
}

LRESULT CAppWebWnd::OnShowPage(WPARAM,LPARAM)
{
	string strURL = "";
	AfxGetPluginManager()->GetPluginWebPage( strURL );		

	Navigate2(strURL.c_str());	
	return 0;
}


void CAppWebWnd::IWebRefreshOb_Refresh(const char* psz)
{
	string strW = GetKeyValueStr(psz,"w");
	if( strW == "app" )
	{
		PostMessage(WM_SHOWPAGE);
	}
}

BOOL CAppWebWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,UINT nID, CCreateContext* pContext)
{	
	m_Mgr = new CCustomOccManager;

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

BOOL CAppWebWnd::CreateControlSite(COleControlContainer* pContainer,COleControlSite** ppSite, UINT nID, REFCLSID clsid)
{
	return TRUE;
}

LRESULT CAppWebWnd::OnCallJavaScript(WPARAM w,LPARAM)
{
	string* pStr = (string*)w;

	AfxGetPluginManager()->CallWebFromKwMusic( *pStr );
	delete pStr;

	return 0;
}