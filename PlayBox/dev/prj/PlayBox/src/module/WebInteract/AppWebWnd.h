
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "../gui/CommonControl/BasicWnd.h"
#include "../DataInterface/IPanelChangeObserver.h"
#include "../DataInterface/IWebRefreshObserver.h"
#include "afxhtml.h"
#include "./webwnd/Custsite.h"
#include "./webwnd/Idispimp.h"
#include "./webwnd/WebPage.h"

#define WM_SHOWPAGE		WM_USER + 100

class CAppWebWnd : public CHtmlView,public IPanelChangeObserver,public IWebRefreshObserver
{
	DECLARE_DYNAMIC(CAppWebWnd)

public:
	CAppWebWnd();
	virtual ~CAppWebWnd();

	virtual void OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel);

	void		IPanelChangeOb_Change( int iPos );
	void		IWebRefreshOb_Refresh(const char* psz);

	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,CCreateContext* pContext = NULL);
	virtual BOOL CreateControlSite(COleControlContainer* pContainer, COleControlSite** ppSite, UINT nID, REFCLSID clsid);

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg	int				OnCreate(LPCREATESTRUCT lpCreateStruct);	
	afx_msg LRESULT			OnCallJavaScript(WPARAM,LPARAM);	
public:
	LRESULT		 OnShowPage(WPARAM,LPARAM);		

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	
private:
	CCustomOccManager*		m_Mgr;	
};


