#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../LayoutMgr/ISkinMgr.h"

class CFlashGamePlay;
class CFlashGameDownloadWnd;
class MyWebBrowserWnd;
class COneFlashGameControl;

class CFlashGameStageLeft : public CBasicWnd
{
	DECLARE_DYNAMIC(CFlashGameStageLeft)
public:
	CFlashGameStageLeft(COneFlashGameControl* pCtrl);
	virtual ~CFlashGameStageLeft();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int	OnCreate(LPCREATESTRUCT lpCreateStruct);

private:
	COneFlashGameControl* m_pCtrl;
	CFlashGamePlay*   m_pPlay;
	MyWebBrowserWnd* m_pRecommand;
};