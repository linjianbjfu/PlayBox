#pragma once

#include "../../gui/CommonControl/BasicSkinWnd.h"
#include "../../gui/CommonControl/shockwaveflash.h"
#include "../../datainterface/IAdData.h"
#include <vector>
#include <string>
using std::vector;
using std::string;

class CMyHtmlView;
class CDibBitmap;
class CxSkinButton;
class MyWebBrowserWnd;

class DownPercentWnd : public CWnd,
					   public CBasicSkinWnd
{
	DECLARE_DYNAMIC(DownPercentWnd)
public:
	DownPercentWnd();
	virtual ~DownPercentWnd();
	void	SetDownPercent( double dPercent );
	void	SetText( vector<string>& vecText );
	void	SetFailed( bool bFailed );
private:
	bool			m_isFailed;
	double			m_dDownPercent;
	CFont			m_font;
	vector<string>	m_vecText;
	//��ͼ��Ҫ��Ԫ��
	COLORREF		m_colBk;
	COLORREF		m_colText;
	CDibBitmap*		m_pBmpBgL;
	CDibBitmap*		m_pBmpBgM;
	CDibBitmap*		m_pBmpBgR;
	CDibBitmap*		m_pBmpM;
	CDibBitmap*		m_pBmpBg_Full;
	CxSkinButton*	m_pBtnClose;
	MyWebBrowserWnd* m_pAdWebWnd;
	std::string		m_strAdUrl;

	void	DrawProgress( CDC* pDc, CRect rc );
	void	DrawInfoText( CDC* pDc, CRect rc );
	void	DrawAd( CDC* pDc, CRect rc );
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnCloseTab();
	void	LoadSkin();
	DECLARE_MESSAGE_MAP()
};