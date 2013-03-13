#pragma once
#include <vector>
#include "ITabBarObserver.h"
using namespace std;

class PlayedGameWnd;

typedef pair< TAB_ITEM, CWnd* > ONE_TAB;
//该类实际上更多的是对窗口的管理
//而非对用户control的管理
//tab_item的操作可直接调用CTabBarData的接口
class CTabPageControl : public ITabBarObserver
{
private:
	vector<ONE_TAB>		m_mapTab;
	CWnd*				m_pWndParent;
	CWnd*				m_pWndCurShow;
public:
	static CTabPageControl*	m_pTabPageControl;
	static CTabPageControl*	GetInstance();
	static void				DelInstance();
	
	CTabPageControl();
	~CTabPageControl();

public:
	void SetParentWnd(CWnd* pWnd);
	HWND GetParentWnd();
	CWnd* GetCurShowWnd();

	void ITabBarOb_CreateNewTab(TAB_ITEM & item);
	void ITabBarOb_OpenExistTab(TAB_ITEM & item);
	void ITabBarOb_DelTab(TAB_ITEM & item);
	void ITabBarOb_OpenTabError(int iErrorCode);
	void ITabBarOb_TabItemDataChanged(TAB_ITEM & item);

	void ResizePage();

	void OpenHomePage();
	void CallJS (LPVOID lpVoid);
};