// PushBoxDlg.h : ͷ�ļ�
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
// CPushBoxDlg �Ի���
class CPushBoxDlg : public CDialog
{
// ����
public:
	CPushBoxDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PUSHBOX_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
