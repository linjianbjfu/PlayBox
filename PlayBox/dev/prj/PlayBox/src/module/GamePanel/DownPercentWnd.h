#pragma once
//#include <gdiplus.h>
#include "../../gui/CommonControl/BasicSkinWnd.h"
#include "../../gui/CommonControl/shockwaveflash.h"
#include "../../datainterface/IAdData.h"
#include <vector>
#include <string>
#include "MyHtmlView.h"

using std::vector;
using std::string;

class CMyHtmlView;
class CDibBitmap;
class CxSkinButton;

class DownPercentWnd : public CWnd,
					   public CBasicSkinWnd
{
	DECLARE_DYNAMIC(DownPercentWnd)
public:
	DownPercentWnd();
	virtual ~DownPercentWnd();
private:
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
	//广告
	Image*   		m_pImgAd;
	CRect		    m_rcFlash;
	AD_STRUCT		m_ad;

	void	DrawProgress( CDC* pDc, CRect rc );
	void	DrawInfoText( CDC* pDc, CRect rc );
	void	DrawAd( CDC* pDc, CRect rc );
	CxSkinButton*	m_pBtnClose;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnCloseTab();

	void	SetDownPercent( double dPercent );
	void	SetText( vector<string>& vecText );
	void	SetFailed( bool bFailed );
	void	LoadSkin();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	CMyHtmlView * m_pWnd;
	// Dynamic adding views(HtmlView here)
	BOOL AddDynamicView(LPCTSTR  lpszLabel, CRuntimeClass * pViewClass);
	bool m_bNavi;
};