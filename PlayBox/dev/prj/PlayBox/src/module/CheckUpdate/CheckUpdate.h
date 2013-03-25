#pragma once

#define INT_SERV_LEN 128
#define INT_USER_LEN 64
#define USER_MSG_CHECKUPDATE WM_USER+31
#define USER_MSG_DOWNLOAD	WM_USER+32
// CCheckUpdate dialog
class CxSkinButton;
class CDibBitmap;
class YL_CHTTPDownFile;

typedef struct _checkupdate{
	bool isNewer;
	bool isRun;
	char szUpdateUrl[1024];
} update_param_t;

typedef struct _download{
	HWND hWnd;
	char szDownloadUrl[1024];
}down_para_t;


class CCheckUpdate : public CDialog
{
	DECLARE_DYNAMIC(CCheckUpdate)

public:
	CCheckUpdate(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCheckUpdate();

// Dialog Data
	enum { IDD = IDD_DIALOG_CHKUPDATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	//Dialogs background.
	CDibBitmap* m_pBmpBkChkDownloading;
	CDibBitmap* m_pBmpBkChkInstall;
	CDibBitmap* m_pBmpBkChkNewer;
	CDibBitmap* m_pBmpBkChkNoUp;
	CDibBitmap* m_pBmpBkChkCheckingUpdate;
	CDibBitmap* m_pBmpBkChkCur;

	//Downloading Progress.
	CDibBitmap* m_pBmpBg_Full;
	CDibBitmap* m_pBmpBgM;
	CDibBitmap* m_pBmpBgR;

	//Buttons.
	CxSkinButton* m_pBtnChkInstall;
	CxSkinButton* m_pBtnChkCancel;
	CxSkinButton* m_pBtnChkDone;
	CxSkinButton* m_pBtnChkIsDown;
	CxSkinButton* m_pBtnChkClose;
	
	double		m_dDownPercent;
	YL_CHTTPDownFile* m_pHTTPDownFile;
	char			m_szUpdateServ[ INT_SERV_LEN];
	char			m_szUpdateUrl[1024];
	bool			m_bRun;
	bool			m_bDown;
	HANDLE m_hMutexhandle;
	HANDLE m_hDownMutexhandle;
public:
	void CheckUpdate();//Package function
	//void checkUpdate();
	void drawProgress(CDC *,CRect);
	void DownloadPackage();//Package function
	//void downloadPackage();

private:
	void LoadSkin();
	afx_msg void OnBnClickedInstall();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedDone();
	afx_msg void OnBnClickedIsDown();
	afx_msg void OnBnClickedClose();
	afx_msg LRESULT OnUserMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserMsgDownload(WPARAM,LPARAM);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
};
UINT __cdecl checkUpdate(LPVOID);
UINT __cdecl downloadPackage(LPVOID);
