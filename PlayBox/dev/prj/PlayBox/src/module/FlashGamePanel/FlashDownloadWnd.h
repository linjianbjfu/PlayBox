#pragma once
#include "../../gui/CommonControl/BasicSkinWnd.h"
#include <vector>
#include <string>
using std::vector;
using std::string;

class CDibBitmap;
class CxSkinButton;
class MyWebBrowserWnd;
class COneFlashGameControl;

class CFlashGameDownloadWnd : public CWnd, public CBasicSkinWnd
{
	DECLARE_DYNAMIC(CFlashGameDownloadWnd)
public:
	CFlashGameDownloadWnd(COneFlashGameControl* pCtrl);
	virtual ~CFlashGameDownloadWnd();
	void	SetDownPercent( double dPercent );
	void	SetText( vector<string>& vecText );
	void	SetFailed( bool bFailed );
private:
	COneFlashGameControl* m_pCtrl;
	bool			m_isFailed;
	double			m_dDownPercent;
	CFont			m_font;
	vector<string>	m_vecText;
	//画图需要的元素
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