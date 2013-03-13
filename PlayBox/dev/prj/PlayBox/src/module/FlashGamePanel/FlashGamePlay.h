#pragma once
#include "../../gui/CommonControl/BasicWnd.h"
#include "../../LayoutMgr/ISkinMgr.h"

class CShockwaveFlash;
class CxSkinButton;

class CFlashGamePlay : public CBasicWnd
{
	DECLARE_DYNAMIC(CFlashGamePlay)
public:
	CFlashGamePlay();
	virtual ~CFlashGamePlay();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int	OnCreate(LPCREATESTRUCT lpCreateStruct);

private:
	CShockwaveFlash*   m_pShockwave;
	CxSkinButton* m_pBtnIntroCtrl;
	CxSkinButton* m_pBtnRecommandCtrl;
};