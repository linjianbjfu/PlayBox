#pragma  once

#include <vector>
#include "HostWnd.h"

using namespace std;

class ILyricPopupWndShowObserver;

struct CWndData{
	CWnd *pWnd;
	CWnd *pParentWnd;
};

struct SepWindowData
{
	CWnd		*pWnd;
	BOOL		isMain;
	BOOL		isResizable;
	CRect		rc;
	CString		strName;
	CString		strContent;
	int			nMinX;
	int			nMinY;
	BOOL		bHide;
};

class CHostWnd;
class CHostWndMgr{
private:

	map<HWND, CWndData> m_mapContentWnd;
	static CHostWndMgr*	m_pHostMgr;
    vector<CHostWnd*>	m_vecHostWnds;
	vector<CHostWnd*>	m_vecVisibleWnds;
	vector<ILyricPopupWndShowObserver*>		m_vecPopupWnds;
	CWnd *				m_pMainWnd;

	CHostWndMgr();
	~CHostWndMgr();

public:
	CWnd*				GetMainWnd(){
		return m_pMainWnd;
	};
	static CHostWndMgr* CHostWndMgr::GetInstance();
	static void			CHostWndMgr::DelInstance();

	void				AttachMovingWnd(SepWindowData data);//CWnd* pWnd, bool isMain, CRect rc, string strName, bool resiezable, CString strBorder);
	void				AttachMovingWnds(vector<SepWindowData> & vecSepData);
	void				KeepOriParent(CWnd *pWndContent);
//	void				DetachMovingWnd(CHostWnd* pWnd);
	void				DetachAllMovingWnd();
	vector<CHostWnd*>*	GetHostWnds();
	CHostWnd*			GetHostWnd(CWnd *pWnd);
	CHostWnd*			GetHostWnd(string strName);
	void				MoveMainWindow(CRect rcNew);
	void				ShowHostWindows(BOOL bShow);	
	void				AddPopupMsgWnds(ILyricPopupWndShowObserver *pWnd){m_vecPopupWnds.push_back(pWnd);};
	void				ShowPopupWndsBeforeMainChange(BOOL bShow);
	void				ShowPopupWndsAfterMainChange(BOOL bShow);
	void				PopupWndsChangeTo1x();
	void				PopupWndsChangeTo2x();
	void				test();
	bool				m_bHideWindow;
};