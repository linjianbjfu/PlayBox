#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../LayoutMgr/ISkinMgr.h"

class CFlashGamePlay;
class CFlashGameDownload;
class MyWebBrowserWnd;

class CFlashGameStageLeft : public CBasicWnd
{
	DECLARE_DYNAMIC(CFlashGameStageLeft)
public:
	CFlashGameStageLeft();
	virtual ~CFlashGameStageLeft();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int	OnCreate(LPCREATESTRUCT lpCreateStruct);

private:
	CFlashGamePlay*   m_pPlay;
	CFlashGameDownload* m_pDownload;
	MyWebBrowserWnd* m_pRecommand;
};