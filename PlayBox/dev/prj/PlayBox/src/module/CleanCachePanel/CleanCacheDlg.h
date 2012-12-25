#pragma once
#include "afxwin.h"


// CCleanCacheDlg dialog

class CxSkinButton;

class CCleanCacheDlg : public CDialog
{
	DECLARE_DYNAMIC(CCleanCacheDlg)

public:
	CCleanCacheDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCleanCacheDlg();

// Dialog Data
	enum { IDD = IDD_DLG_CLEANCHACHE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CxSkinButton*	m_pBtnClose;
	CxSkinButton*	m_pBtnCleanCache;
	CxSkinButton*	m_pBtnCancel;
	CxSkinButton*	m_pBtnRestart;
	CxSkinButton*	m_pBtnRestartUnable;

	CDibBitmap*		m_pBkAskCleanCache;
	CDibBitmap*		m_pBkCleanedCache;
	CDibBitmap*		m_pBkCleanningCache;
	CDibBitmap*		m_pBkShowing;

	CBitmap		m_LoadingBmp[8];

	HANDLE	m_hCleanThread;
	static DWORD WINAPI CleanCacheThread(LPVOID lpParam);
	static BOOL EmptyDirectory(char *szPath);

public:
	virtual BOOL OnInitDialog();
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnBnClickedCleanCache();
	afx_msg void OnBnClickedCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnCleanCacheDone(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	// // 清理缓存时的动态图
	CStatic m_staticBmpLoading;
};
