#pragma once
#include "..\..\Global\GlobalVariable.h"
#include "..\..\datainterface\IGameData.h"

class MyWebBrowserWnd;

class COneFlashGameControl
{
public:
	COneFlashGameControl();
	~COneFlashGameControl();

	void SetBrowserIntro(CWnd* pWnd);
	void SetBrowserRecommand(CWnd* pWnd);
	void SetTabItem(TAB_ITEM ti);
private:
	void NavigateIntro(std::string strID);
	void NavigateRecommand(std::string strID);

	MyWebBrowserWnd* m_pBrowserIntro;
	MyWebBrowserWnd* m_pBrowserRecommand;
	TAB_ITEM m_tabItem;
	OneGame	m_olg;
};