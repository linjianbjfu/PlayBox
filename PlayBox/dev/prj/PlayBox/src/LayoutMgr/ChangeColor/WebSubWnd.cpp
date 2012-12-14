#include "stdafx.h"
#include "WebSubWnd.h"
#include "tools.h"
#include ".\module\HTTPControl\JudgeFirewall.h"
//²âÊÔurl
//http://resedit.kuwo.cn:8080/datax/MusicTopToday/export/skin/recSkin.html
//×îÖÕurl
//http://topmusic.kuwo.cn/today_recommend/skin/recSkin.html

#define  WEBSKIN_RECOMMEND_URL		"http://topmusic.kuwo.cn/today_recommend/skin/recSkin.html?name=none"	
CWebSubWnd::CWebSubWnd()
{
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_REFRESH,this);
}

CWebSubWnd::~CWebSubWnd()
{
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_REFRESH,this);
}
IMPLEMENT_DYNAMIC(CWebSubWnd, CHtmlView)

BEGIN_MESSAGE_MAP(CWebSubWnd, CHtmlView)
	ON_WM_CREATE()	
	ON_WM_SIZE()	
END_MESSAGE_MAP()

int CWebSubWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( __super::OnCreate( lpCreateStruct) == -1 )
	{
		return -1;
	}
	m_flash.Create(NULL,NULL, WS_CHILD, CRect(0,0,0,0), this, 11456);
	m_flash.ShowWindow(SW_SHOW);
	return 0;
}

void CWebSubWnd::OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel)
{
	m_flash.ShowWindow(SW_HIDE);
	if( strstr(lpszURL,WEBSKIN_RECOMMEND_URL) != 0 )
	{
		char szBuffer[MAX_PATH];
		CLhcImg::GetHomePath(szBuffer,MAX_PATH);
		string str = szBuffer;
		str += "\\html\\skinerror.htm?r=";		
		str += m_strURL;
		str += "&w=skin";

		Navigate2(str.c_str(), navNoReadFromCache, NULL);
		m_bSucceed = FALSE;

		LogUserActMsg("ERROR", "WEB_COMMEND_ERR");
		CJudgeFirewall::SetHTTPRes( HTTP_WEB_COMMAND,0 );
	}
}

HRESULT CWebSubWnd::OnGetHostInfo(DOCHOSTUIINFO *pInfo)
{
	pInfo->cbSize = sizeof(DOCHOSTUIINFO);
	pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER ;
	pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
	return S_OK;
}

void CWebSubWnd::OnNavigateComplete2( LPCTSTR strURL )
{
	if( m_strURL == strURL )
	{
		char szBuf[32];
		sprintf(szBuf,"T:%d",GetTickCount() - m_dwStartShowTime );
		LogUserActMsg("WEBLOADRECOM",szBuf);
		CJudgeFirewall::SetHTTPRes( HTTP_WEB_COMMAND,1 );
	}	

	m_dwStartShowTime = 0;
	if(strstr(strURL, "skinerror") != 0)
		Refresh2(REFRESH_NORMAL);
}

void CWebSubWnd::Navigate()
{
	char szurl[MAX_PATH] = WEBSKIN_RECOMMEND_URL;
	m_strURL = szurl;
	m_dwStartShowTime = GetTickCount();
	Navigate2(szurl, navNoReadFromCache, NULL);
}

void CWebSubWnd::IWebRefreshOb_Refresh(const char* psz)
{
	string strW = GetKeyValueStr(psz,"w");
	if( strW == "skin" )
	{
		Navigate2((LPCTSTR)m_strURL, navNoReadFromCache, NULL);
		m_bSucceed = TRUE;
	}
}

void CWebSubWnd::OnSize(UINT nType, int cx, int cy)
{
	m_flash.MoveWindow(0,0,cx,cy);
	__super::OnSize(nType,cx,cy);
}

void CWebSubWnd::OnDocumentComplete(LPCTSTR lpszURL)
{
	m_flash.ShowWindow(SW_HIDE);
	__super::OnDocumentComplete(lpszURL);
}
