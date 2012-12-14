#pragma once
#include "afxhtml.h"
#include "../../DataInterface/IWebRefreshObserver.h"
#include "../../gui/CommonControl/FlashLoadingWnd.h"

class CWebSubWnd : public CHtmlView , public IWebRefreshObserver
{
	DECLARE_DYNAMIC(CWebSubWnd)
protected:
	DECLARE_MESSAGE_MAP()
public:
		
	CWebSubWnd();
	virtual ~CWebSubWnd();
	virtual void OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel);
	virtual void OnNavigateComplete2( LPCTSTR strURL );
	HRESULT	OnGetHostInfo(DOCHOSTUIINFO *pInfo);
	afx_msg	int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	void		Navigate();
	void		 IWebRefreshOb_Refresh(const char* psz);
	virtual void OnDocumentComplete(LPCTSTR lpszURL);

	BOOL m_bSucceed;
	DWORD m_dwStartShowTime;
	CString	m_strURL;

	CFlashWnd m_flash;
};