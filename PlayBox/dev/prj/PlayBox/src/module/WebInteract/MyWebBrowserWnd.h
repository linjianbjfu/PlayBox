#pragma once
#include "../../gui/CommonControl/BasicWnd.h"
#include "afxhtml.h"
#include "Custsite.h"
#include "Idispimp.h"
#include <string>
using std::string;

#define WM_CALL_JAVASCRIPT	WM_USER+1001

// 页面跳转 
// 
// WPARAM : (LPCTSTR) string of url		(可以为NULL)
// LPARAM : (LPCTSTR) string of title	(可以为NULL)
#define WM_PAGE_CHANGED	(WM_USER+501)

// WPARAM : (LPSTR) string of url
#define WM_NEWPAGE	(WM_USER+502)

class MyWebBrowserWnd : public CHtmlView 
{
	DECLARE_DYNAMIC(MyWebBrowserWnd)
public:
	MyWebBrowserWnd(BOOL bShowLoading = TRUE);
	virtual ~MyWebBrowserWnd();
	virtual BOOL CreateControlSite(COleControlContainer* pContainer, COleControlSite** ppSite, UINT nID, REFCLSID clsid);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,CCreateContext* pContext = NULL);
	virtual void OnTitleChange(LPCTSTR lpszText);
	virtual void OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel);
	virtual void OnDownloadBegin();
	void	Navigate( string strUrl );
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT	OnCallJavaScript(WPARAM,LPARAM);	
	BOOL			m_bSucceed;
	BOOL			m_bReady;	//网页documentComplete
private:
	CCustomOccManager*		m_Mgr;	
};