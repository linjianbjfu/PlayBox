#pragma once
#include <vector>
#include "ITabBarObserver.h"
using namespace std;

class PlayedGameWnd;

typedef pair< TAB_ITEM, CWnd* > ONE_TAB;
//����ʵ���ϸ�����ǶԴ��ڵĹ���
//���Ƕ��û�control�Ĺ���
//tab_item�Ĳ�����ֱ�ӵ���CTabBarData�Ľӿ�
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