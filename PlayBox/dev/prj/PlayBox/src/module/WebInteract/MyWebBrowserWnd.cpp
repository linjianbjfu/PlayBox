#include "stdafx.h"
#include "MyWebBrowserWnd.h"
#include "WebManager.h"

IMPLEMENT_DYNAMIC(MyWebBrowserWnd, CHtmlView)
MyWebBrowserWnd::MyWebBrowserWnd(BOOL bShowLoading/* = TRUE*/)
{
}

MyWebBrowserWnd::~MyWebBrowserWnd()
{
}

BEGIN_MESSAGE_MAP(MyWebBrowserWnd, CHtmlView)
	ON_MESSAGE(WM_CALL_JAVASCRIPT,OnCallJavaScript)	
END_MESSAGE_MAP()


BOOL MyWebBrowserWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,UINT nID, CCreateContext* pContext)
{	
	m_Mgr = new CCustomOccManager();
	m_pCreateContext = pContext;
	if (!CView::Create(lpszClassName, lpszWindowName,dwStyle, rect, pParentWnd,  nID, pContext))
		return FALSE;

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
	m_pBrowserApp->put_Silent(VARIANT_TRUE);
	return TRUE;
}

BOOL MyWebBrowserWnd::CreateControlSite(COleControlContainer* pContainer,COleControlSite** ppSite, UINT nID, REFCLSID clsid)
{
	return TRUE;
}

void MyWebBrowserWnd::Navigate(const string& strUrl)
{
	Navigate2( strUrl.c_str() );
}

void MyWebBrowserWnd::NavigetePost(const string& strUrl, 
								   const string& strHeader, 
								   const string& strPostData)
{
	Navigate2(strUrl.c_str(), 0, NULL, strHeader.c_str(), 
		(LPVOID)(strPostData.c_str()), strPostData.length());
}

void MyWebBrowserWnd::OnNavigateComplete2(LPCTSTR strURL)
{
	GetParent()->SendMessage(WM_PAGE_CHANGED, (WPARAM)strURL, 0);
}

LRESULT MyWebBrowserWnd::OnCallJavaScript(WPARAM w,LPARAM l)
{
	CHtmlView* phtml = (CHtmlView*)w;
	CallJSParam * pParam = (CallJSParam*) l;
	CWebManager::GetInstance()->CallWebFromGBox( phtml, pParam/**pStr */);
	return 0;
}

void MyWebBrowserWnd::OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel)
{	
	AfxGetMainWindow()->SendMessage(MSG_NEW_BROWSER_WND, (WPARAM)ppDisp, 0);
}

void MyWebBrowserWnd::OnTitleChange(LPCTSTR lpszText)
{
	GetParent()->SendMessage(WM_PAGE_CHANGED, 0, (LPARAM)lpszText);
	__super::OnTitleChange(lpszText);
}