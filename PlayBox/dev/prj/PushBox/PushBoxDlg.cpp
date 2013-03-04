// PushBoxDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PushBox.h"
#include "PushBoxDlg.h"
#include "MyHtmlView.h"
#include ".\pushboxdlg.h"
#include "pushbox_macros.h"
#include "include\YL_HTTPRequest.h"
#include <hash_map>
#include <atlrx.h>

using namespace std;
using namespace stdext;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//Declare a function for ads thread.
UINT __cdecl adsPush( LPVOID pParam );
//Function to update the user behavior.
UINT __cdecl uploadUserBehavior(LPVOID pParam);

// CPushBoxDlg 对话框
CPushBoxDlg::CPushBoxDlg(CWnd* pParent /*=NULL*/)
: CDialog(CPushBoxDlg::IDD, pParent),
m_hMutexhandle(NULL),
ADS_TIME(60),
m_iWidth(0),
m_iHeight(0),
m_bTimerrun(false),
m_iTaskIndex(1),
m_iTask_Count(1),
m_pView(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//Determine whether there is any other process.
	SECURITY_DESCRIPTOR     sd_wideopen;
	InitializeSecurityDescriptor(&sd_wideopen, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(
		&sd_wideopen,	// addr of SD
		TRUE,		    // TRUE = DACL present
		NULL,		    // ... but it's empty (wide open)
		FALSE);		    // DACL explicitly set, not defaulted
	SECURITY_ATTRIBUTES sa;
	memset(&sa, 0, sizeof sa);
	sa.nLength              = sizeof sa;
	sa.lpSecurityDescriptor = &sd_wideopen;
	sa.bInheritHandle       = FALSE;
	m_hMutexhandle = ::CreateMutex(&sa, TRUE, "Lea_PushBox");
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle( m_hMutexhandle );
		exit (1);
	}

	memset(ADS_SECTION,0,sizeof(ADS_SECTION));
}

void CPushBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPushBoxDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_btnClose, OnBnClickedbtnclose)
	ON_WM_CLOSE()
	ON_MESSAGE(CUS_UPDATECONF,OnUpdateConf)
END_MESSAGE_MAP()


// CPushBoxDlg 消息处理程序

BOOL CPushBoxDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);
	if(!this->AddDynamicView(_T(""),RUNTIME_CLASS(CMyHtmlView)))
	{
		return FALSE;
	}
	this->GetConfigurations(true);


	//Set the position for the pop up window.
	RECT rectDesktopWithoutTaskbar,rectClient;
	long lLeft =0, lTop = 0,lWidth =0,lHeight = 0;
	this->GetClientRect(&rectClient);
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rectDesktopWithoutTaskbar, 0);

	lWidth = rectClient.right - rectClient.left;
	lHeight = rectClient.bottom - rectClient.top;

	lLeft = rectDesktopWithoutTaskbar.right - lWidth-20;
	lTop = rectDesktopWithoutTaskbar.bottom - lHeight-0;

	/*::SetWindowPos(this->m_hWnd,HWND_TOPMOST,lTop,lLeft,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);*/
	::MoveWindow(this->m_hWnd,lLeft,lTop,lWidth,lHeight,true );
	CRect rect;
	this->GetClientRect(&rect);
	m_pView->MoveWindow(rect,true);

	::SetWindowPos(this->GetDlgItem (IDC_btnClose)->GetSafeHwnd(),(HWND)CWnd::wndTopMost,   0,   0,   0,   0,   (SWP_NOSIZE   |   SWP_NOMOVE)   ); 



	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPushBoxDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CPushBoxDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


bool CPushBoxDlg::AddDynamicView(LPCTSTR lpszLabel, CRuntimeClass * pViewClass)
{
	TRY
	{
		m_pView = (CMyHtmlView *)pViewClass->CreateObject ();
		if (m_pView == NULL)
		{
			::AfxThrowMemoryException ();
		}
	}
	CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL

		//DWORD dwStyle = AFX_WS_DEFAULT_VIEW|SWP_FRAMECHANGED;
		DWORD dwStyle = WS_VISIBLE|SWP_NOSIZE |WS_CHILD;
	//Set the rect of dynamic created view.
	CRect rect;
	GetClientRect(&rect);

	if(!m_pView->Create (NULL,NULL,dwStyle,rect,this,CMyHtmlView::IDD ))
	{
		return FALSE;
	}
	m_pView->OnInitialUpdate ();
	m_pView->EnableWindow (TRUE);
	m_pView->ShowWindow (SW_SHOW);

	return TRUE;
}
void CPushBoxDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case ADS_TIMER_ID:
		{
			KillTimer(nIDEvent);
			this->ShowWindow(SW_HIDE);
		}
		break;
	case SLEEP_TIMER_ID:
		{
			KillTimer(nIDEvent);
			this->ShowWindow(SW_SHOWNOACTIVATE);
			m_bTimerrun = false;
			++m_iTaskIndex> m_iTask_Count ? 1:m_iTask_Count;
			this->GetConfigurations(false);
		}
		break;
	default:
		break;
	}


	CDialog::OnTimer(nIDEvent);
}

void CPushBoxDlg::OnBnClickedbtnclose()
{
	// TODO: Add your control notification handler code here
	::SendMessage(this->m_hWnd,WM_CLOSE,NULL,NULL);
}

void CPushBoxDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if(m_hMutexhandle != INVALID_HANDLE_VALUE)
		CloseHandle( m_hMutexhandle );
	CDialog::OnClose();
}

// Read the configurations about ads.
BOOL CPushBoxDlg::GetConfigurations(bool bRun)
{
	//static int iTaskIndex = 0;
	TCHAR szCurPath[MAX_PATH],szINIPath[MAX_PATH],szDefaultAds[MAX_PATH],szTemp[1024];
	::GetCurrentDirectory(MAX_PATH,szCurPath);
	strcpy (szINIPath,szCurPath);
	strcpy(szDefaultAds,szCurPath);
	strcat(szINIPath,CONF_FILE);
	strcat(szDefaultAds,ADS_DEFAULT_PAGE);

	m_iTask_Count = ::GetPrivateProfileInt(ADS_NUM_SEC,ADS_NUM,1,szINIPath);

	_snprintf(ADS_SECTION,7,"Task_%d",m_iTaskIndex);
	//If the Action is zero, kill the process.
	if(!::GetPrivateProfileInt(ADS_NUM_SEC,KILLER_ACTION,1,szINIPath))
	{
		this->SendMessage(WM_CLOSE);
	}
	//If the ads type is zero, this is not an ads, do nothing.
	else if(::GetPrivateProfileInt(ADS_SECTION,ADS_TYPE,0,szINIPath))
	{
		YL_CHTTPRequest http;
		//Get the real ads url.
		char tempPath[MAX_PATH],szTemp[1024];
		DWORD dwLen = GetTempPathA(MAX_PATH,tempPath);
		strcat(tempPath,CONF_REAL_ADS);
		::GetPrivateProfileString(ADS_SECTION,ADS_SEC_URL,NULL,szTemp,MAX_PATH,szINIPath);
		if(szTemp != NULL)
		{
			char szUrl[1024];
			memset(szUrl,0,1024);
			_snprintf(szUrl,1024,"%s&mac=%s&type=inldt&action=1",szTemp,/*szMacAddr*/"0026c75acde4");
			//MessageBox(NULL,szUrl,NULL,MB_OK|MB_ICONINFORMATION);
			if( http.Request( szUrl ,YL_CHTTPRequest::REQUEST_GET,30*1000 ) )
			{
				FILE *fp = NULL;
				if((fp = fopen(tempPath,"w+"))  != NULL)
				{
					long		   lConLen = 0;
					unsigned char* pContent = http.GetContent(lConLen);
					//sprintf(szUrl,"%d",lConLen);
					//MessageBox(NULL,szUrl,NULL,MB_OK|MB_ICONINFORMATION);
					fprintf(fp,"%s",pContent);
					fclose(fp);
				}
			}
		}
		//Get the ads url.
		char tempFile[MAX_PATH];
		dwLen = GetTempPathA(MAX_PATH,tempFile);
		strcat(tempFile,CONF_REAL_ADS);
		if(m_szAdsURL.IsEmpty())
			::GetPrivateProfileString("OUT","u",szDefaultAds,szTemp,MAX_PATH,tempFile);
		else
			::GetPrivateProfileString("OUT","u",m_szAdsURL,szTemp,MAX_PATH,tempFile);
		m_szAdsURL.Format("%s",szTemp);
		if(m_pView != NULL)
			m_pView->Navigate2(m_szAdsURL);
		//Get the time for ads to display.
		ADS_TIME = ::GetPrivateProfileInt(ADS_SECTION,ADS_SEC_TIME,75,szINIPath);
		SetTimer(ADS_TIMER_ID,ADS_TIME*1000,NULL);
		m_iWidth = ::GetPrivateProfileInt(ADS_SECTION,ADS_WIDTH,500,szINIPath);
		m_iHeight = ::GetPrivateProfileInt(ADS_SECTION,ADS_HEIGHT,600,szINIPath);
		m_iSleeptime = ::GetPrivateProfileInt(ADS_SECTION,ADS_SLEEP_TIME,2,szINIPath);
		if(!m_bTimerrun)
		{
			SetTimer(SLEEP_TIMER_ID,m_iSleeptime*60*1000,NULL);
			m_bTimerrun = true;
		}
	}
	if(bRun)
	{
		para_t * pPara = new para_t;
		memset (pPara,0,sizeof(pPara));
		pPara->pAds_URL = new TCHAR[1024];
		pPara->hWnd = this->m_hWnd;
		strcpy(pPara->szConfFile,szINIPath);
		strcpy(pPara->szAdsSec,ADS_SECTION);

		char szMacAddr[64];
		memset(szMacAddr,0,64);
		this->GetMacAddress(szMacAddr);
		strcpy(pPara->szMacAddr,szMacAddr);
		_snprintf(pPara->pAds_URL,1023,"%smac=%s&type=unldt&qudao=ldt007&ver=1.40&untime=2012-06-22&action=1",\
			ADS_PUSH_DEFAULT_URL,
			szMacAddr);
		//::GetPrivateProfileString(ADS_SECTION,ADS_PUSH_URL,ADS_PUSH_DEFAULT_URL,pPara->pAds_URL,1024,szINIPath);
		::AfxBeginThread(adsPush,
			(LPVOID)pPara);
		::AfxBeginThread(uploadUserBehavior,
			(LPVOID)NULL);
	}

	return 0;
}


UINT __cdecl adsPush( LPVOID pParam )
{
	para_t *pPara = ((para_t *)pParam);
	TCHAR szAdsPushURL[1024] = {0},szConfFile[MAX_PATH] ={0},szMacAddr[64] = {0},ADS_SECTION[8] ={0};
	HWND hWnd = pPara->hWnd;
	strcpy(szAdsPushURL, pPara->pAds_URL);
	strcpy(szConfFile,pPara->szConfFile);
	strcpy(szMacAddr,pPara->szMacAddr);
	strcpy(ADS_SECTION,pPara->szAdsSec);

	delete pPara;

	YL_CHTTPRequest http;
	while (1)
	{
		Sleep(PUSH_TIME*60*1000);
		if( http.Request( szAdsPushURL ,YL_CHTTPRequest::REQUEST_GET,30*1000 ) )
		{
			FILE *fp = NULL;
			if((fp = fopen(szConfFile,"w+"))  != NULL)
			{
				long		   lConLen = 0;
				unsigned char* pContent = http.GetContent(lConLen);
				fprintf(fp,"%s",pContent);
				fclose(fp);
				//Get the real ads url.
				//char tempPath[MAX_PATH],szTemp[1024];
				//DWORD dwLen = GetTempPathA(MAX_PATH,tempPath);
				//strcat(tempPath,CONF_REAL_ADS);
				//::GetPrivateProfileString(ADS_SECTION,ADS_SEC_URL,NULL,szTemp,MAX_PATH,szConfFile);
				//if(szTemp != NULL)
				//{
				//	char szUrl[1024];
				//	memset(szUrl,0,1024);
				//	_snprintf(szUrl,1024,"%s&mac=%s&type=inldt&action=1",szTemp,/*szMacAddr*/"0026c75acde4");
				//	//MessageBox(NULL,szUrl,NULL,MB_OK|MB_ICONINFORMATION);
				//	if( http.Request( szUrl ,YL_CHTTPRequest::REQUEST_GET,30*1000 ) )
				//	{
				//		FILE *fp = NULL;
				//		if((fp = fopen(tempPath,"w+"))  != NULL)
				//		{
				//			long		   lConLen = 0;
				//			unsigned char* pContent = http.GetContent(lConLen);
				//			sprintf(szUrl,"%d",lConLen);
				//			//MessageBox(NULL,szUrl,NULL,MB_OK|MB_ICONINFORMATION);
				//			fprintf(fp,"%s",pContent);
				//			fclose(fp);
				//		}
				//	}
				//}

			}
			SendMessage(hWnd,CUS_UPDATECONF,NULL,NULL);
		}
	}
	return (0);
}
struct string_less:public binary_function<const string, const string, bool>
{
public:
	result_type operator()(const first_argument_type& _Left, const second_argument_type& _Right) const
	{
		return (_Left.compare(_Right)<0? true:false);
	}
};

UINT __cdecl uploadUserBehavior(LPVOID pParam)
{
	char userBehaviors[MAX_PATH];
	DWORD dwLen = GetTempPathA(MAX_PATH,userBehaviors);
	strcat(userBehaviors,FILE_USER_BEHAVIOR);

	vector<string> vecUserBehavior;
	hash_map<string, int, hash_compare<string,string_less> > strUrlHash;

	FILE *fp = fopen(userBehaviors,"r+");
	if(fp != NULL)
	{
		char *temp = new char[1024];
		while(!feof(fp))
		{
			fgets(temp,1024,fp);
			vecUserBehavior.push_back(string(temp));
		}
		delete temp;
		fclose(fp);
	}

	//When all has been read, clear it .
	fp = fopen(userBehaviors,"w+");
	fclose(fp);

	CAtlRegExp<> reUrl;
	REParseError status = reUrl.Parse(
		"({[^:/?#]+}:)?(//{[^/?#]*})?{[^?#]*}(?{[^#]*})?(#{.*})?" );
	CAtlREMatchContext<> mcUrl;

	vector<string>::iterator vecIter;
	for(vecIter = vecUserBehavior.begin(); vecIter != vecUserBehavior.end(); vecIter++)
	{
		string strTemp = ((string)*vecIter);
		if(!reUrl.Match(strTemp.c_str(),&mcUrl))
		{
			continue;
		}
		else 
		{
			char szUploadUrl[1024];
			for (UINT nGroupIndex = 0; nGroupIndex < mcUrl.m_uNumGroups;
				++nGroupIndex)
			{
				const CAtlREMatchContext<>::RECHAR* szStart = 0;
				const CAtlREMatchContext<>::RECHAR* szEnd = 0;
				mcUrl.GetMatch(nGroupIndex, &szStart, &szEnd);

				ptrdiff_t nLength = szEnd - szStart;
				switch(nGroupIndex)
				{
				case 1:
					{
						memset(szUploadUrl,0,sizeof(szUploadUrl));
						_snprintf(szUploadUrl,1024,"%.*s\n", nLength, szStart);
						//MessageBox(NULL,szUploadUrl,"Info",MB_ICONINFORMATION|MB_OK);
						if(strUrlHash.find(szUploadUrl) == strUrlHash.end())
						{
							strUrlHash[szUploadUrl] = 1;
						}
						else
						{
							++strUrlHash[szUploadUrl] ;
						}
					}
					break;
				default:
					break;
				}

			}
		}
	}
	hash_map<string, int, hash_compare<string,string_less> >::iterator hashIter;
	for(hashIter = strUrlHash.begin(); hashIter != strUrlHash.end(); hashIter++)
	{
		//MessageBox(NULL,hashIter->first.c_str(),"Info2",MB_ICONINFORMATION|MB_OK);
	}

	return (0);
}

LRESULT CPushBoxDlg::OnUpdateConf(WPARAM wParam, LPARAM lParam)
{
	//this->GetConfigurations(false);
	if(!::IsWindowVisible(this->m_hWnd))
	{
		RECT rectDesktopWithoutTaskbar;
		long lLeft =0, lTop = 0;
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &rectDesktopWithoutTaskbar, 0);
		lLeft = rectDesktopWithoutTaskbar.right - m_iWidth -5;
		lTop = rectDesktopWithoutTaskbar.bottom - m_iHeight -1;
		::MoveWindow(this->m_hWnd,lLeft,lTop,m_iWidth,m_iHeight,true );
		CRect rect;
		this->GetClientRect(&rect);
		m_pView->MoveWindow(rect,true);
		//this->ShowWindow(SW_SHOWNOACTIVATE); //Be controlled by SLEEP_TIMER now.
	}
	return NULL;
}

bool CPushBoxDlg::GetMacAddress(char *szMac)
{
	HKEY hKey, hKey2;
	char m_SubKey[] = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards", szName[MAX_PATH], szValue[MAX_PATH];
	DWORD cbNameLen = MAX_PATH, cbValueLen = MAX_PATH, dwIndex = 0, dwAttr = 0;
	if(RegOpenKeyEx(HKEY_PLAYBOX_ROOT, m_SubKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return false;
	}

	BYTE mac[6];
	BOOL result = FALSE;
	while(RegEnumKeyEx(hKey, dwIndex, szName, &cbNameLen, 0, NULL, NULL, 0) != ERROR_NO_MORE_ITEMS)
	{
		if(strcmp(szName, "") != NULL)
		{
			//如果找到了，分别在枚举下面有没有ServiceName
			char szCard[MAX_PATH+1];
			_snprintf(szCard, MAX_PATH, "%s\\%s", m_SubKey, szName);
			//再打开，查找
			if(RegOpenKeyEx(HKEY_PLAYBOX_ROOT, szCard, 0, KEY_READ, &hKey2) == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey2,"ServiceName", 0, &dwAttr, LPBYTE(szValue), &cbValueLen);
				RegCloseKey(hKey2);
				szValue[cbValueLen] = 0;

				string strDriver = string("\\\\.\\") + szValue;
				HANDLE hICDriver = CreateFile(strDriver.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);   
				if( hICDriver != INVALID_HANDLE_VALUE )
				{
					ULONG32 oid = OID_802_3_PERMANENT_ADDRESS;       // Defined in Ntddpack.h
					DWORD dwByteCount = 0;
					result = DeviceIoControl(hICDriver, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, 4, mac, 6, &dwByteCount, NULL);
					CloseHandle(hICDriver);
					if(result != FALSE)
						break;
				}
			}
		}
		dwIndex++;
		cbNameLen = MAX_PATH;
		cbValueLen = MAX_PATH;
	}
	RegCloseKey(hKey);

	if(result == FALSE)
		return false;
	else
	{
		_snprintf( szMac, 12, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
		return true;
	}
}
