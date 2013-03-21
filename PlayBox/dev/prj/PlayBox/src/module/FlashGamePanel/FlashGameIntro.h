#pragma once
#include "../../gui/CommonControl/BasicWnd.h"

class CxSkinButton;
class COneFlashGameControl;
class MyWebBrowserWnd;

class CFlashGameIntro : public CBasicWnd
{
	DECLARE_DYNAMIC(CFlashGameIntro)
public:
	CFlashGameIntro(COneFlashGameControl* pCtrl);
	virtual ~CFlashGameIntro();
	void Navigate(const std::string& strUrl);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int	OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnIntroHideClicked();
private:
	COneFlashGameControl* m_pCtrl;
	CxSkinButton* m_pBtnIntroCtrlHide;
	MyWebBrowserWnd* m_pWebIntro;
};