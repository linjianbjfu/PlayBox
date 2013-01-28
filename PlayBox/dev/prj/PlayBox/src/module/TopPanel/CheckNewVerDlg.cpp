// CheckNewVerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CheckNewVerDlg.h"
#include "YL_HTTPRequest.h"
#include "LhcImg.h"
#include "YL_Base64.h"
#include <process.h>

#define STR_CHECK_NEW_VER_LOG   "checknewver.log"

BEGIN_MESSAGE_MAP( CHyperLinkClose, CHyperLink )
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CHyperLinkClose::OnLButtonUp(UINT nFlags, CPoint point)
{
	__super::OnLButtonUp( nFlags, point );
	::PostMessage(GetParent()->GetSafeHwnd(),WM_CLOSE,0,0);
}

// CCheckNewVerDlg dialog

IMPLEMENT_DYNAMIC(CCheckNewVerDlg, CDialog)
CCheckNewVerDlg::CCheckNewVerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckNewVerDlg::IDD, pParent)
{
}

CCheckNewVerDlg::~CCheckNewVerDlg()
{
}

void CCheckNewVerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_LINK_TO_NEW, m_linkNewVer);
}


BEGIN_MESSAGE_MAP(CCheckNewVerDlg, CDialog)
	ON_MESSAGE(WM_CHECK_RESULT, OnCheckResult)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

bool Encryption( char* szSrc, unsigned int iSrclength, char** szDest )
{
	bool bRet = false;
	try{
		YL_Log(STR_CHECK_NEW_VER_LOG, LOG_NOTICE, "UserReg", "Encryption in, src is %s.", szSrc);
		int i = BASE64_LENGTH(iSrclength)+1;
		*szDest = new char[BASE64_LENGTH(iSrclength)+1];//Base64::Encode((unsigned char*) pTemp,iSrclength);
		if(*szDest != NULL)
		{
			YL_Log(STR_CHECK_NEW_VER_LOG, LOG_NOTICE, "UserReg", "Encryption YL_Base64Encode in.");
			memset(*szDest, 0, BASE64_LENGTH(iSrclength)+1);
			if(YL_Base64Encode(*szDest, szSrc, iSrclength, "yeelion ")>0)
				bRet = true;
		}
		YL_Log(STR_CHECK_NEW_VER_LOG, LOG_NOTICE, "UserReg", "Encryption YL_Base64Encode out.");
	}
	catch(...)
	{
		return false;
	}
	return bRet;
}

bool CheckNewVersion(string &strConf)
{
	//获取服务器上的配置文件
	bool bret = false;
	YL_CHTTPRequest client;
	char len[NUM_LENGTH], szID[INT_LENGTH], szVersion[VERSION_LENGTH], szInstallSRC[SRC_LENGTH], szTransfer[1024];
	CLhcImg::GetUserID( szID, INT_LENGTH );
	CLhcImg::GetSoftwareVersion( szVersion, VERSION_LENGTH );
	CLhcImg::GetInstallSRC( szInstallSRC, SRC_LENGTH );

	char szsvr[SRC_LENGTH];
	memset(szsvr, 0, SRC_LENGTH);
	CLhcImg::GetConfigServer(szsvr, SRC_LENGTH);
	string strURL = string(szsvr) + "/uc/s?m=";    //经过压缩的日志
	char* pEncodeTransfer = NULL;
	memset(len, 0, NUM_LENGTH);
	memset( szTransfer, 0, 1024 );
	_snprintf(szTransfer, 1023, "%s;%s,%s,CHECK_UPDATE", szID, szVersion, szInstallSRC);

	//使用Base64加密字符串
	unsigned int length = (unsigned int)strlen(szTransfer);
	if(!Encryption(szTransfer, length, &pEncodeTransfer))
	{
		YL_Log(STR_CHECK_NEW_VER_LOG, LOG_WARNING, "UpdateUserConfig", "Encrypt string Error! string is %s, length is %u.", szTransfer, length);
	}
	YL_Log(STR_CHECK_NEW_VER_LOG, LOG_NOTICE, "UpdateUserConfig", "Encrypt string Success! string is %s, length is %u.", szTransfer, length);

	strURL = strURL + itoa((int)strlen(szTransfer), len, 10) + ";" + pEncodeTransfer;
	bret = client.Request(strURL.c_str(), YL_CHTTPRequest::REQUEST_POST, 10*1000, NULL, 0);
	if(pEncodeTransfer != NULL)
		delete[] pEncodeTransfer;

	if(!bret)
	{
		YL_Log( STR_CHECK_NEW_VER_LOG, LOG_WARNING, "UpdateUserConfig", "Get Config Request: HTTP Request is error!" );
		return false;
	}
	YL_Log( STR_CHECK_NEW_VER_LOG, LOG_NOTICE, "UpdateUserConfig", "Get Config Request Success." );

	long reslen = 0;
	unsigned char *pContent = client.GetContent(reslen);
	//得不到配置文件
	if(pContent == NULL || reslen <= 0)
	{
		YL_Log( STR_CHECK_NEW_VER_LOG, LOG_WARNING, "UpdateUserConfig", "Get Config Error: No Return Content!" );
		return false;
	}
	YL_Log(STR_CHECK_NEW_VER_LOG, LOG_NOTICE, "UpdateUserConfig", "result config content is %s", pContent);

	char *szContent = new char[reslen+1], *szConfig = new char[BASE64_LENGTH(reslen)+1];
	szContent[0] = szContent[reslen] = 0;
	memcpy(szContent, pContent, reslen);
	YL_Base64Decode(szConfig, szContent);
	if( strlen( szConfig ) > 0 )
	{
		strConf = szConfig;
		YL_Log(STR_CHECK_NEW_VER_LOG, LOG_NOTICE, "UpdateUserConfig", "Config file: content is %s", (char*)szConfig);
	}
	if(szConfig != NULL)
		delete[] szConfig;
	if(szContent != NULL)
		delete[] szContent;
	if(bret == false)
		return false;

	return true;
}

void CheckThread(void *pparam)
{
	string strConf;
	CheckNewVersion(strConf);
	if(pparam != NULL)
	{
		// parse config string, url as msg param
		if(strConf.find("NewestSoft") != string::npos)
		{
			string::size_type nIdx = strConf.find("url=");
			if(nIdx != string::npos)
			{
				string::size_type nPost = strConf.find(BOX_DELIMITERS, nIdx);
				if(nPost != string::npos)
				{
					string str = strConf.substr(nIdx+4, nPost-nIdx-4);
					SendMessage((HWND)pparam, WM_CHECK_RESULT, (WPARAM)str.c_str(), 1);
					return;
				}
			}
		}

		SendMessage((HWND)pparam, WM_CHECK_RESULT, 0, 0);// 没有更新版本
	}
}

// CCheckNewVerDlg message handlers
LRESULT CCheckNewVerDlg::OnCheckResult(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default
	CWnd *pOK_Wnd = GetDlgItem(IDOK), *pCANCEL_Wnd = GetDlgItem(IDCANCEL);

	// 
	if(lParam == 1)
	{
		m_linkNewVer.SetURL((char*)wParam);
		m_linkNewVer.SetLinkCursor( AfxGetApp()->LoadCursor(IDC_CURSOR1) );
		m_linkNewVer.ShowWindow(SW_SHOW);

		if(pOK_Wnd != NULL)
		{
			pOK_Wnd->ShowWindow(SW_SHOW);
		}

		if(pCANCEL_Wnd != NULL)
		{
			pCANCEL_Wnd->ShowWindow(SW_SHOW);
		}

		CWnd *pMSG_Wnd = GetDlgItem(IDC_STATIC_TEXT_MSG);
		if(pMSG_Wnd != NULL)
		{
			pMSG_Wnd->SetWindowText("7k7k游戏盒有最新版本, 详情请点击:");
		}
	}
	else
	{
		CWnd *pMSG_Wnd = GetDlgItem(IDC_STATIC_TEXT_MSG);
		if(pMSG_Wnd != NULL)
		{
			pMSG_Wnd->SetWindowText("您使用的已经是7k7k游戏盒最新版本,谢谢关注.");
		}

		if(pOK_Wnd != NULL)
		{
			CRect rect;
			pOK_Wnd->GetWindowRect(&rect);
			rect.left += 50;
			rect.right += 50;
			ScreenToClient(&rect);
			pOK_Wnd->MoveWindow(&rect);
			pOK_Wnd->ShowWindow(SW_SHOW);
		}
	}
	return 0;
}

BOOL CCheckNewVerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CWnd *pOK_Wnd = GetDlgItem(IDOK), *pCANCEL_Wnd = GetDlgItem(IDCANCEL);
	if(pOK_Wnd != NULL)
	{
		pOK_Wnd->ShowWindow(SW_HIDE);
	}
	
	if(pCANCEL_Wnd != NULL)
	{
		pCANCEL_Wnd->ShowWindow(SW_HIDE);
	}

	m_linkNewVer.SetURL("");
	m_linkNewVer.ShowWindow(SW_HIDE);

	CWnd *pMSG_Wnd = GetDlgItem(IDC_STATIC_TEXT_MSG);
	if(pMSG_Wnd != NULL)
	{
		pMSG_Wnd->SetWindowText("正在检测新版信息...");
	}

	_beginthread(CheckThread , 0, m_hWnd);

	return TRUE; 
}

void CCheckNewVerDlg::OnBnClickedOk()
{
	if(m_linkNewVer.GetURL() != "")
		ShellExecute(NULL, "open", "iexplore.exe", 	(LPCSTR)m_linkNewVer.GetURL(), "", SW_SHOWMAXIMIZED);

	OnOK();
}
