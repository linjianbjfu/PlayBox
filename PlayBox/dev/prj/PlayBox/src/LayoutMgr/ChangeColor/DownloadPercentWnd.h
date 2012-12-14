#pragma once
#include "colorslide.h"
#include "StaticTextEx.h"
#include "IEvent.h"
#include "YL_HTTPDownFile.h"

class CDownloadPercentWnd : public CWnd
{
	DECLARE_DYNAMIC(CDownloadPercentWnd)
public:
	CDownloadPercentWnd();
	virtual ~CDownloadPercentWnd();
private:
	CColorSlide m_downLoadSlide;
	CStaticText   m_textPercent;
	CStaticText   m_textHit;

	IEventListener*		m_pEvnet;
	YL_CHTTPDownFile*	m_pDown;
	int				m_iDownPercent;
	BOOL			m_bNetConnect;
	
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnDownSubStart(WPARAM,LPARAM);
	afx_msg LRESULT OnDownSubClose(WPARAM,LPARAM);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();

	void SetDownloadHit(CString str);
	void SetDownloadPercent(int percentage);
	void SetEventListener(IEventListener* pEventListener){ m_pEvnet = pEventListener;}
};
