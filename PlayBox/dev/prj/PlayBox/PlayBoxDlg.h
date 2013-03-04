#pragma once

#include "./src/AppConfig/config/AfxUserConfig.h"
#include "./src/GUI/tray/TrayIcon.h"
#include "./src/DataInterface/ILayoutChangeObserver.h"
#include "./src/DataInterface/IPanelChangeObserver.h"
#include "IUserMsgObserver.h"

class CPlayBoxDlg : public CDialog, 
					public AfxUserConfig_Observer,
					public ILayoutChangeObserver,
					public IPanelChangeObserver,
					public IUserMsgObserver
{
// 构造
public:
	CPlayBoxDlg(bool bStartupRun, CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PLAYBOX_DIALOG };
	void		DeleteTray();
	DWORD		m_dwFrontTime;
	BOOL		m_bExit;
	bool	m_bStartupRun;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	void OnConfigChange(const string& strKey );
	void OnKooWoProtocol(const COPYDATASTRUCT* pCopyDataStruct);
	void OnExecWebCmd(const char *tmp);
	void NotifyWebRefresh(const char* psz);

protected:
	HICON m_hIcon;
	CTrayIcon*	m_ptrayIcon;		// my tray icon

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x,int y);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCancel();
	afx_msg void OnOK();
	afx_msg void OnClose();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg LRESULT OnTrayNotification(WPARAM wp, LPARAM lp);	
	afx_msg LRESULT OnShowTrayIconBalloon(WPARAM wp, LPARAM lp);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnAfterUiCreate(WPARAM,LPARAM);
	afx_msg LRESULT OnChangeText(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnHTTPDonwload(WPARAM,LPARAM);
	afx_msg LRESULT OnNewBrowserWnd(WPARAM w,LPARAM l);
	afx_msg LRESULT OnOpenRegDialog(WPARAM w,LPARAM l);
	afx_msg LRESULT OnDoTask(WPARAM w, LPARAM l);
	afx_msg LRESULT OnConfUpdate(WPARAM,LPARAM);
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	BOOL PreTranslateMessage(MSG* pMsg );

	DECLARE_MESSAGE_MAP()

private:
	void	_AdjustDlgSize();	
	void	SetFormCmdLine(bool bValue);
	BOOL	_needChangeCursor(const CPoint&);
	void	DrawMoveFrame(CRect rc);
	CRect	m_rcMain;

#define DIRECTION_LEFT		1
#define DIRECTION_RIGHT		2
#define DIRECTION_TOP		4
#define DIRECTION_BOTTOM	8

	int			m_iDirection;
	BOOL		m_bIsResizing;
	CPoint		m_ptStartResizing;
	CRect		m_rectStarResizing;
	bool		m_bNormal;
	bool		m_bChangeCursor;
	
	IUIManager* m_pUIManager;

	int			m_nMinCx;
	int			m_nMinCy;

	BOOL		m_bNeedStartPlay;
	UINT        m_uCloseTimer;

	DWORD		m_dwLastFrontTime;
	BOOL		m_bNotAutoStart;
	string		m_strLastP2PMsgContent;
	string		m_strLastP2PMsgTitle;
	string		m_strLastP2PMsgLink;

public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnDeleteTray(WPARAM wParam, LPARAM lParam);

	virtual void	ILayoutChangeOb_SkinChanged(string oldSkinPath, string newSkinPath);
	virtual void	IPanelChangeOb_WindowMax();
	virtual void	IPanelChangeOb_WindowMin();
	virtual void	IPanelChangeOb_WindowRestore();

	void UserMsg_Login();
	void UserMsg_LogOut();
	void UserMsg_LogFaild();
	void UserMsg_BeginLogin();
	
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	afx_msg void OnTrayMenuSetting();
	afx_msg void OnTrayMenuOpen();
	afx_msg void OnTrayMenuExit();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	void SetStartupRun (bool bStartupRun) { m_bStartupRun = bStartupRun; }
};