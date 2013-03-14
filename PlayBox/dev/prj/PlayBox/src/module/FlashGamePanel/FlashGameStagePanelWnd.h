#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../LayoutMgr/ISkinMgr.h"

class CFlashGameStageLeft;
class MyWebBrowserWnd;

class CFlashGameStagePanelWnd : public CBasicWnd
{
	DECLARE_DYNAMIC(CFlashGameStagePanelWnd)
public:
	CFlashGameStagePanelWnd();
	virtual ~CFlashGameStagePanelWnd();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int	OnCreate(LPCREATESTRUCT lpCreateStruct);

private:
	CFlashGameStageLeft*   m_pStageLeft;
	MyWebBrowserWnd* m_pIntro;
};