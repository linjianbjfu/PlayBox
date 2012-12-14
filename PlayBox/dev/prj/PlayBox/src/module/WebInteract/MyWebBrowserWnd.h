#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "../../gui/CommonControl/BasicWnd.h"
#include "../../DataInterface/IWebRefreshObserver.h"
#include "FlashLoadingWnd.h"
#include "afxhtml.h"
#include "Custsite.h"
#include "Idispimp.h"
#include "WebPage.h"
#include <string>
using std::string;

#define WM_CALL_JAVASCRIPT	WM_USER+1001

class MyWebBrowserWnd : public CHtmlView,
						public IWebRefreshObserver
{
	DECLARE_DYNAMIC(MyWebBrowserWnd)

public:
	MyWebBrowserWnd(BOOL bShowLoading = TRUE);
	virtual ~MyWebBrowserWnd();

	virtual BOOL CreateControlSite(COleControlContainer* pContainer, COleControlSite** ppSite, UINT nID, REFCLSID clsid);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,CCreateContext* pContext = NULL);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	virtual void OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel);
	virtual void OnNavigateComplete2( LPCTSTR strURL );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnDocumentComplete(LPCTSTR lpszURL);	
	virtual void IWebRefreshOb_Refresh(const char* psz);

	void	Navigate( string strUrl );
	void	Init();
	void	Recycle();
	void	SetHomePage( bool bHomePage );

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg	int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnTimer(UINT nIDEvent);

public:
	afx_msg LRESULT	OnCallJavaScript(WPARAM,LPARAM);	
	
	void			ShowErrorPage();
	//void			RegisterLHProtocol();

	BOOL			m_bSucceed;
	BOOL			m_bReady;	//ÍøÒ³documentComplete
	DWORD			m_dwStartShowTime;
	CString			m_strURL;
	CFlashWnd		m_flash;
	BOOL			m_bShowLoading;
private:
	bool			m_bHomePage;
	CCustomOccManager*		m_Mgr;	
	string			m_strErrorPagePath;
};