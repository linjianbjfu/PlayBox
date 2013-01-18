// CheckUpdate.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PlayBox.h"
#include "CheckUpdate.h"
#include <cassert>
#include <string>
#include "src/GUI/CommonControl/xSkinButton.h"
#include ".\checkupdate.h"
#include "YL_HTTPDownFile.h"
#include "YL_HTTPRequest.h"
#include "LhcImg.h"
#include "YL_DirInfo.h"

#define UPDATE_CONF	"\\Update.ini"
#define UPDATE_SECTION "Update"
#define UPDATE_SEC_URL "URL" 
#define UPDATE_SEC_VER "Ver"
#define UPDATE_SEC_ISRUN "Run"
#define UPDATE_PATH	"\\Update"
#define TIMER_ID_CHKDOWN	1002
#define PROGRESS_LEN	0.10

// CCheckUpdate dialog

IMPLEMENT_DYNAMIC(CCheckUpdate, CDialog)
CCheckUpdate::CCheckUpdate(CWnd* pParent /*=NULL*/)
: CDialog(CCheckUpdate::IDD, pParent),
m_dDownPercent(0.00),
m_pBmpBkChkCheckingUpdate(NULL),
m_pBmpBkChkDownloading(NULL),
m_pBmpBkChkNewer(NULL),
m_pBmpBkChkNoUp(NULL),
m_hMutexhandle(NULL),
m_hDownMutexhandle(NULL),
m_bDown(false)
{
	m_pBtnChkCancel = new CxSkinButton;
	m_pBtnChkDone	= new CxSkinButton;
	m_pBtnChkInstall	= new CxSkinButton;
	m_pBtnChkIsDown = new CxSkinButton;
	m_pBtnChkClose	= new CxSkinButton;
	//m_pHTTPDownFile = new YL_CHTTPDownFile;
	memset(m_szUpdateServ,0,sizeof(m_szUpdateServ));
	memset(m_szUpdateUrl,0,sizeof(m_szUpdateUrl));
	m_bRun = false;
}

CCheckUpdate::~CCheckUpdate()
{
	if(m_pBtnChkInstall != NULL)
	{
		delete m_pBtnChkInstall;
	}
	if(m_pBtnChkDone != NULL)
	{
		delete m_pBtnChkDone;
	}
	m_pBtnChkCancel == NULL ? NULL:(delete m_pBtnChkCancel);
	m_pBtnChkIsDown == NULL ?NULL:(delete m_pBtnChkIsDown);
	m_pBtnChkClose	== NULL ? NULL:(delete m_pBtnChkClose);
	m_hMutexhandle	== NULL ? NULL:(CloseHandle(m_hMutexhandle));
	m_hDownMutexhandle == NULL ? NULL:(CloseHandle(m_hDownMutexhandle));
}

void CCheckUpdate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCheckUpdate, CDialog)
	ON_BN_CLICKED(IDC_BTN_CHK_INSTALL,OnBnClickedInstall)
	ON_BN_CLICKED(IDC_BTN_CHK_CANCEL,OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_CHK_DONE,OnBnClickedDone)
	ON_BN_CLICKED(IDC_BTN_CHK_ISDOWN,OnBnClickedIsDown)
	ON_BN_CLICKED(IDC_BTN_CHK_CLOSE,OnBnClickedClose)
	ON_MESSAGE(USER_MSG_CHECKUPDATE,OnUserMsg)
	ON_MESSAGE(USER_MSG_DOWNLOAD,OnUserMsgDownload)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CCheckUpdate message handlers
void CCheckUpdate::LoadSkin()
{
	ISkinMgr * pSkinMgr = ::AfxGetUIManager()->UIGetSkinMgr();
	m_pBmpBkChkCheckingUpdate = pSkinMgr->GetDibBmp("bkChkCheckingUpdate");
	m_pBmpBkChkDownloading = pSkinMgr->GetDibBmp("bkChkDonwloading");
	m_pBmpBkChkNewer	= pSkinMgr->GetDibBmp("bkChkNewer");
	m_pBmpBkChkNoUp = pSkinMgr->GetDibBmp("bkChkNoUp");
	m_pBmpBkChkInstall = pSkinMgr->GetDibBmp("bkChkIntall");
	m_pBmpBkChkCur = m_pBmpBkChkCheckingUpdate;

	m_pBmpBg_Full  = pSkinMgr->GetDibBmp( "Progress_Bg_Full" );
	m_pBmpBgM  = pSkinMgr->GetDibBmp( "Progress_Bg_M" );
	m_pBmpBgR  = pSkinMgr->GetDibBmp( "Progress_Bg_R" );
}

void CCheckUpdate::OnBnClickedCancel()
{
	SendMessage(WM_CLOSE);
	CDialog::OnCancel();
}
void CCheckUpdate::OnBnClickedDone()
{
	this->OnBnClickedCancel();
}
void CCheckUpdate::OnBnClickedInstall()
{
	char szCurPath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,szCurPath);
	strcat(szCurPath,UPDATE_PATH);
	strcat(szCurPath,"\\UpdateInstall.exe");

	ShellExecute(NULL,"Open",szCurPath,NULL,NULL,SW_SHOW);
	CDialog::OnOK();
}
int CCheckUpdate::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	LoadSkin();
	SetWindowPos(NULL, 0, 0, 320, 180, SWP_NOMOVE);
	COLORREF color(RGB(255, 0, 255));
	SetWindowRgn(m_pBmpBkChkCur->CreateRgnFromBitmap(color,this), TRUE);
	CenterWindow();

	CRect rectNULL(0,0,0,0);
	m_pBtnChkCancel->Create (NULL,WS_VISIBLE,rectNULL,this,IDC_BTN_CHK_CANCEL);
	m_pBtnChkDone->Create(NULL,WS_VISIBLE,rectNULL,this,IDC_BTN_CHK_DONE);
	m_pBtnChkInstall->Create(NULL,WS_VISIBLE,rectNULL,this,IDC_BTN_CHK_INSTALL);
	m_pBtnChkIsDown->Create(NULL,WS_VISIBLE,rectNULL,this,IDC_BTN_CHK_ISDOWN);
	m_pBtnChkClose->Create(NULL,WS_VISIBLE,rectNULL,this,IDC_BTN_CHK_CLOSE);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->RegisterCtrl(this,"CheckUpdateDlg_btnInstall",m_pBtnChkInstall);
	pLayoutMgr->RegisterCtrl(this,"CheckUpdateDlg_btnCancel",m_pBtnChkCancel);
	pLayoutMgr->RegisterCtrl(this,"CheckUpdateDlg_btnDone",m_pBtnChkDone);
	pLayoutMgr->RegisterCtrl(this,"CheckUpdateDlg_btnIsDown",m_pBtnChkIsDown);
	pLayoutMgr->RegisterCtrl(this,"CheckUpdateDlg_btnClose",m_pBtnChkClose);

	pLayoutMgr->CreateControlPane( this,"CheckUpdateDlg","normal");	
	pLayoutMgr->CreateBmpPane( this,"CheckUpdateDlg","normal" );		
	return 0;
}

void CCheckUpdate::CheckUpdate()
{
	//Change to the checkingUpdate background.
	{
		m_pBmpBkChkCur = m_pBmpBkChkCheckingUpdate;
		//Invalidate();
		m_pBtnChkCancel->ShowWindow(SW_HIDE);
		m_pBtnChkDone->ShowWindow(SW_HIDE);
		m_pBtnChkIsDown->ShowWindow(SW_HIDE);
		m_pBtnChkInstall->ShowWindow(SW_HIDE);
		
	}
	//Detect whether there is another update process.
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
	m_hMutexhandle = ::CreateMutex(&sa, TRUE, "Lea_CheckUpdate");
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle( m_hMutexhandle );
		m_hMutexhandle = NULL;
		return;
	}
	
	::AfxBeginThread(checkUpdate,(LPVOID)this->m_hWnd);
}
UINT __cdecl checkUpdate(LPVOID pParam)
{
	//http://g.najiuwan.com/up/up.php?type=box&qudao=all&ver=1.10&instime=2013-01-12&action=1
	YL_CHTTPRequest http;
	char szUpdateServUrl[1024]={0},szUpdateServ[1024]={0};
	char szUserID[INT_SERV_LEN]={0},szVer[16]={0},szInstTime[]={"2013-01-12"},szMarketChannel[]={"all"};
	HWND hWnd = (HWND)pParam;
	//CLhcImg::GetUpdateServer(szUpdateServ,INT_SERV_LEN);
	//assert(std::strlen(m_szUpdateServ )!= 0);
	///*if(std::strlen(m_szUpdateServ) == 0)
	//{
	//	MessageBox("Update server not found.","Warning!",MB_OK|MB_ICONEXCLAMATION);
	//	SendMessage(WM_CLOSE);
	//	return ;
	//}*/
	//CLhcImg::GetUserID(szUserID,INT_USER_LEN);
	//assert(std::strlen(szUserID) != 0);
	//CLhcImg::GetSoftwareVersion(szVer,16);
	//assert(std::strlen(szVer) != 0);
	_snprintf(szUpdateServ,1024,"%s","http://g.najiuwan.com/up");
	_snprintf(szVer,16,"%s","1.10");
	
	
	
	_snprintf(szUpdateServUrl,1024,"%s/up.php?type=box&qudao=%s&instime=%s&action=1",szUpdateServ,szMarketChannel,szInstTime);
	//_snprintf(szUpdateUrl,1024,"%s/up.php?type=box&id=%s&qd=%s&v=%s&instime=%s",\
		m_szUpdateServ,szUserID,szMarketChannel,szVer,szInstTime);
	if( http.Request( szUpdateServUrl ,YL_CHTTPRequest::REQUEST_GET,30*1000 ) )
	{
		FILE *fp = NULL;
		char tempFile[MAX_PATH];
		DWORD dwLen = GetTempPathA(MAX_PATH,tempFile);
		strcat(tempFile,UPDATE_CONF);
		if((fp = fopen(tempFile,"w+"))  != NULL)
		{
			long		   lConLen = 0;
			unsigned char* pContent = http.GetContent(lConLen);
			fprintf(fp,"%s",pContent);
			fclose(fp);
		}
		float fOldVer = atof(szVer);
		float fNewVer=0;
		char szNewVer[16]={0}; 
		update_param_t *pPara = new update_param_t;
		memset(pPara,0,sizeof(update_param_t));
		::GetPrivateProfileString(UPDATE_SECTION,UPDATE_SEC_VER,NULL,szNewVer,16,tempFile);
		fNewVer = (szNewVer == NULL ? 0:atof(szNewVer));
		if(fOldVer < fNewVer)
		{
			int iIsRun = ::GetPrivateProfileInt(UPDATE_SECTION,UPDATE_SEC_ISRUN,0,tempFile);
			::GetPrivateProfileString(UPDATE_SECTION,UPDATE_SEC_URL,NULL,szUpdateServ,1024,tempFile);
			pPara->isNewer=true;
			pPara->isRun = iIsRun == 0 ?false:true;
			strcpy(pPara->szUpdateUrl,szUpdateServ);
		}
		else
		{
			pPara->isNewer=false;
			pPara->isRun = false;
		}
		SendMessage(hWnd,USER_MSG_CHECKUPDATE,(WPARAM)pPara,NULL);
	}
	return 0;
}

void CCheckUpdate::OnBnClickedIsDown()
{
	{
		m_pBmpBkChkCur = m_pBmpBkChkDownloading;
		Invalidate();
		m_pBtnChkInstall->ShowWindow(SW_SHOW);
		m_pBtnChkInstall->EnableWindow(FALSE);
		m_pBtnChkCancel->ShowWindow(SW_SHOW);
		m_pBtnChkCancel->EnableWindow(TRUE);
		m_pBtnChkDone->ShowWindow(SW_HIDE);
		m_pBtnChkIsDown->ShowWindow(SW_HIDE);         
	}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
	m_bDown = true;
	DownloadPackage();
}
UINT __cdecl downloadPackage(LPVOID lPvoid)
{
	char szUpdateUrl[1024] ;
	down_para_t *pPara = (down_para_t *)lPvoid;
	HWND hWnd = pPara->hWnd;
	strcpy(szUpdateUrl,pPara->szDownloadUrl);

	if(szUpdateUrl == NULL )
	{
		return 1;
	}

	char szCurPath[MAX_PATH] = {0};
	DWORD dwSize = ::GetCurrentDirectory(MAX_PATH,szCurPath);
	strcat(szCurPath,UPDATE_PATH);
	YL_DirInfo::MakeDir(string(szCurPath));
	strcat(szCurPath,"\\UpdateInstall.exe");
	YL_CHTTPDownFile* pHTTPDownFile = new YL_CHTTPDownFile;
	//AsyncDownloadFile
	SendMessage(hWnd,USER_MSG_DOWNLOAD,(WPARAM)pHTTPDownFile,NULL);
	pHTTPDownFile->DownloadFile(string(szUpdateUrl),string(szCurPath),0);
	return 0;
}

void CCheckUpdate::drawProgress(CDC *pDc,CRect rc)
{
	if( m_pBmpBg_Full == NULL || m_pBmpBgM == NULL 
		|| m_pBmpBgR == NULL )
	{
		return;
	}
	//确定进度条范围：中心点向下30像素
	CPoint ptCenter = rc.CenterPoint();
	CRect rcProgressBarBg( rc );
	rcProgressBarBg.left += 30;
	rcProgressBarBg.right -= 30;
	rcProgressBarBg.top = ptCenter.y + 5;
	rcProgressBarBg.bottom = rcProgressBarBg.top + m_pBmpBg_Full->GetHeight();

	//Paint the full progress.
	CRect rcBgL( rcProgressBarBg );
	m_pBmpBg_Full->SetCDibRect( rcBgL );
	m_pBmpBg_Full->Draw( pDc, TRUE, RGB(255,0,0) );
	//Paint the right background.
	CRect rcBgR( rcProgressBarBg );
	rcBgR.left = rcBgR.right - m_pBmpBgR->GetWidth();
	m_pBmpBgR->SetCDibRect( rcBgR );
	m_pBmpBgR->Draw( pDc, TRUE, RGB(255,0,0) );
	//Paint the middle parts.
	CRect rcBgM( rcProgressBarBg );
	rcBgM.left  = rcProgressBarBg.left +m_dDownPercent*rcProgressBarBg.Width() ;
	rcBgM.right = rcBgR.left;
	m_pBmpBgM->SetCDibRect( rcBgM );
	m_pBmpBgM->Draw( pDc, TRUE, RGB(255,0,0) );

}
void CCheckUpdate::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	CRect rect;
	GetClientRect(&rect);

	CDC MemDC;
	CBitmap MemBitmap;
	MemDC.CreateCompatibleDC(&dc);
	int iSavedDC = MemDC.SaveDC();
	MemBitmap.CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());
	MemDC.SelectObject(&MemBitmap);
	MemDC.SetBkMode(TRANSPARENT);
	MemDC.FillSolidRect(&rect,RGB(125,125,125));

	//Paint the background.
	m_pBmpBkChkCur->SetCDibRect(rect);
	assert(m_pBmpBkChkCur != NULL);
	m_pBmpBkChkCur->Draw(&MemDC,FALSE,RGB(255,255,255));

	if(m_bDown == true)
	{
		drawProgress(&MemDC,rect);
	}
	dc.BitBlt( 0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY );
	MemDC.RestoreDC(iSavedDC);
	MemDC.DeleteDC();	
	MemBitmap.DeleteObject();

}

UINT CCheckUpdate::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect rc;
	GetClientRect(&rc);
	rc.bottom = rc.top + 40;
	ClientToScreen(&rc);
	return rc.PtInRect(point) ? HTCAPTION : CDialog::OnNcHitTest(point);
}

void CCheckUpdate::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case TIMER_ID_CHKDOWN:
		{
			m_dDownPercent = m_pHTTPDownFile->GetCurrentDownloadStatus();
			m_dDownPercent = m_dDownPercent < 0.0 ? 0.0: m_dDownPercent;
			//Download complete, change to the Install interface.
			if(m_dDownPercent >= 1.0)
			{
				KillTimer(nIDEvent);
				m_pBmpBkChkCur = m_pBmpBkChkInstall;
				Invalidate();
				m_pBtnChkInstall->ShowWindow(SW_SHOW);
				m_pBtnChkInstall->EnableWindow(TRUE);
				m_pBtnChkCancel->ShowWindow(SW_SHOW);
				m_pBtnChkCancel->EnableWindow(TRUE);
				m_pBtnChkDone->ShowWindow(SW_HIDE);
				m_pBtnChkIsDown->ShowWindow(SW_HIDE);   
			}
			else
			{
				Invalidate();
			}
		}
		break;
	default:
		break;
	}
	CDialog::OnTimer(nIDEvent);
}

void CCheckUpdate::OnBnClickedClose()
{
	SendMessage(WM_CLOSE);
}
BOOL CCheckUpdate::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CheckUpdate();		

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CCheckUpdate::OnUserMsg(WPARAM wParam, LPARAM lParam)
{
	update_param_t* para = (update_param_t* )wParam;
	
	if(para->isNewer == true)
	{
		StrCpy(m_szUpdateUrl ,para->szUpdateUrl);
		//StrCpy(m_szUpdateUrl,"http://static.laidiantuan.com/static/download/gov/laidiantuan_V1.40.exe");//Debug
		if(para->isRun != true)
		{
			m_bRun = false;
			//Change to newer version available background.
			{
				m_pBmpBkChkCur = m_pBmpBkChkNewer;
				m_bDown = false;
				Invalidate();
				m_pBtnChkInstall->ShowWindow(SW_HIDE);
				m_pBtnChkInstall->EnableWindow(FALSE);
				m_pBtnChkCancel->ShowWindow(SW_SHOW);
				m_pBtnChkCancel->EnableWindow(TRUE);
				m_pBtnChkDone->ShowWindow(SW_HIDE);
				m_pBtnChkIsDown->ShowWindow(SW_SHOW);
			}

			
		}
		else
		{
			m_bRun = true;
			//Directly into Downloading interface.
			{
				m_pBmpBkChkCur = m_pBmpBkChkDownloading;
				m_bDown = true;
				
				m_pBtnChkInstall->ShowWindow(SW_SHOW);
				m_pBtnChkInstall->EnableWindow(FALSE);
				m_pBtnChkCancel->ShowWindow(SW_SHOW);
				m_pBtnChkCancel->EnableWindow(TRUE);
				m_pBtnChkDone->ShowWindow(SW_HIDE);
				m_pBtnChkIsDown->ShowWindow(SW_HIDE);
			}					
			Invalidate();
			DownloadPackage();
		}
	}
	else
	{
		//Change into Current Version is newest interface.
		m_pBmpBkChkCur = m_pBmpBkChkNoUp;
		Invalidate();
		m_pBtnChkDone->ShowWindow(SW_SHOW);
		m_pBtnChkCancel->ShowWindow(SW_HIDE);
		m_pBtnChkInstall->ShowWindow(SW_HIDE);
		m_pBtnChkIsDown->ShowWindow(SW_HIDE);
		
	}
	delete para;
	CloseHandle(m_hMutexhandle);
	return NULL;
}
LRESULT CCheckUpdate::OnUserMsgDownload(WPARAM wParam,LPARAM lParam)
{
	m_pHTTPDownFile = (YL_CHTTPDownFile* )wParam;
	SetTimer(TIMER_ID_CHKDOWN,1000,NULL);
	return NULL;
}
void CCheckUpdate::DownloadPackage()
{
	down_para_t *pDownpara = new down_para_t;
	pDownpara->hWnd = this->m_hWnd;
	StrCpy(pDownpara->szDownloadUrl , m_szUpdateUrl);
	AfxBeginThread(downloadPackage,(LPVOID)pDownpara);
}