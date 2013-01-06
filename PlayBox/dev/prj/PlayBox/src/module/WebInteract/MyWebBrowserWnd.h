#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "../../gui/CommonControl/BasicWnd.h"
#include "FlashLoadingWnd.h"
#include "afxhtml.h"
#include "Custsite.h"
#include "Idispimp.h"
#include "WebPage.h"
#include <string>
using std::string;

#define WM_CALL_JAVASCRIPT	WM_USER+1001

// 页面跳转 
// 
// WPARAM : (LPCTSTR) string of url		(可以为NULL)
// LPARAM : (LPCTSTR) string of title	(可以为NULL)
#define WM_PAGE_CHANGED	(WM_USER+501)

class MyWebBrowserWnd : public CHtmlView 
{
	DECLARE_DYNAMIC(MyWebBrowserWnd)
public:
	MyWebBrowserWnd(BOOL bShowLoading = TRUE);
	virtual ~MyWebBrowserWnd();

	virtual BOOL CreateControlSite(COleControlContainer* pContainer, COleControlSite** ppSite, UINT nID, REFCLSID clsid);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,CCreateContext* pContext = NULL);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	virtual void DocumentComplete(LPDISPATCH pDisp, VARIANT* URL);
	virtual void OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel);
	virtual void OnNavigateComplete2( LPCTSTR strURL );
	virtual void OnTitleChange(LPCTSTR lpszText);
	virtual void OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel);
	virtual void OnDownloadBegin();
	void	Navigate( string strUrl );
	void	Init();
	void	Recycle();
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg	int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnTimer(UINT nIDEvent);

public:
	afx_msg LRESULT	OnCallJavaScript(WPARAM,LPARAM);	
	void			ShowErrorPage();

	BOOL			m_bSucceed;
	BOOL			m_bReady;	//网页documentComplete
	CString			m_strURL;
	CFlashWnd		m_flash;
	BOOL			m_bShowLoading;
private:
	bool			m_bHomePage;
	CCustomOccManager*		m_Mgr;	
	string			m_strErrorPagePath;

	LPDISPATCH		m_lpDisp;//用于判断网页加载是否真正完毕
};