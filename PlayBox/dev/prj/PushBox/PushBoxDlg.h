// PushBoxDlg.h : 头文件
//

#pragma once
#define CUS_UPDATECONF	 WM_USER+101


class CMyHtmlView;


typedef struct 
{
	HWND hWnd;
	TCHAR *pAds_URL;
	TCHAR szConfFile[1024];
	TCHAR szMacAddr[64];
	TCHAR szAdsSec[8];
} para_t;
// CPushBoxDlg 对话框
class CPushBoxDlg : public CDialog
{
// 构造
public:
	CPushBoxDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PUSHBOX_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	bool AddDynamicView(LPCTSTR lpszLabel, CRuntimeClass * pViewClass);
protected:
	CMyHtmlView *m_pView;
	HANDLE m_hMutexhandle;

	//Declare some variables for ads here.
	CString m_szAdsURL;
	int		 ADS_TIME;
	char		ADS_SECTION[8];
	int		m_iHeight;
	int		m_iWidth;
	int		m_iSleeptime; //By minutes.
	bool		m_bTimerrun;
	int		m_iTaskIndex;
	int		m_iTask_Count;
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedbtnclose();
	afx_msg void OnClose();
	// Read the configurations about ads.
	BOOL GetConfigurations(bool);
	bool GetMacAddress(char *);
	afx_msg LRESULT OnUpdateConf(WPARAM wParam, LPARAM lParam);
};
