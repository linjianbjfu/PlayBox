// CleanCacheDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PlayBox.h"
#include "CleanCacheDlg.h"
#include "CleanCacheDlgDef.h"
#include "src/GUI/CommonControl/xSkinButton.h"
#include ".\cleancachedlg.h"

#include <atlbase.h>
#include <Wininet.h>

// CCleanCacheDlg dialog

IMPLEMENT_DYNAMIC(CCleanCacheDlg, CDialog)
CCleanCacheDlg::CCleanCacheDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCleanCacheDlg::IDD, pParent)
{
	m_hCleanThread = NULL;

	m_pBkAskCleanCache= ::AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("bkAskCleanCache");
	m_pBkCleanedCache=	::AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("bkCleanedCache");
	m_pBkCleanningCache=::AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("bkCleanningCache");
	m_pBkShowing = m_pBkAskCleanCache;

	for (int i=0; i<8; i++)
	{
		m_LoadingBmp[i].LoadBitmap(IDB_BMP_LOADING1+i);
	}

	m_pBtnClose		= new CxSkinButton;
	m_pBtnCleanCache= new CxSkinButton;
	m_pBtnCancel	= new CxSkinButton;
	m_pBtnRestart	= new CxSkinButton;
	m_pBtnRestartUnable=new CxSkinButton;
}

CCleanCacheDlg::~CCleanCacheDlg()
{
	delete	m_pBtnClose;
	delete	m_pBtnCleanCache;
	delete	m_pBtnCancel;
	delete	m_pBtnRestart;
	delete m_pBtnRestartUnable;
}

void CCleanCacheDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_BMP_LOADING, m_staticBmpLoading);
}


BEGIN_MESSAGE_MAP(CCleanCacheDlg, CDialog)
	ON_BN_CLICKED(IDC_CLEANCACHE_BTN_CLEAN,		OnBnClickedCleanCache)
	ON_BN_CLICKED(IDC_CLEANCACHE_BTN_CANCEL,	OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CLEANCACHE_BTN_RESTART,	OnBnClickedRestart)
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_MESSAGE(WM_CLEAN_CACHE_DONE, OnCleanCacheDone)
END_MESSAGE_MAP()


// CCleanCacheDlg message handlers

BOOL CCleanCacheDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(NULL, 0, 0, 320, 180, SWP_NOMOVE);
	COLORREF m_color(RGB(255, 0, 255));
	SetWindowRgn(m_pBkAskCleanCache->CreateRgnFromBitmap(m_color,this), TRUE);
	CenterWindow();

	CRect rctNULL(0,0,0,0);

	m_pBtnCleanCache->Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CLEANCACHE_BTN_CLEAN);
	m_pBtnCancel->Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CLEANCACHE_BTN_CANCEL);
	m_pBtnClose->Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CLEANCACHE_BTN_CANCEL);
	m_pBtnRestart->Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CLEANCACHE_BTN_RESTART);
	m_pBtnRestartUnable->Create(NULL, 0, rctNULL, this, IDC_CLEANCACHE_BTN_RESTART);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();

	pLayoutMgr->RegisterCtrl( this, "CleanCacheDlg_btnClean", m_pBtnCleanCache);
	pLayoutMgr->RegisterCtrl( this, "CleanCacheDlg_btnCancel", m_pBtnCancel);
	pLayoutMgr->RegisterCtrl( this, "CleanCacheDlg_btnClose", m_pBtnClose);
	pLayoutMgr->RegisterCtrl( this, "CleanCacheDlg_btnReStart", m_pBtnRestart);
	pLayoutMgr->RegisterCtrl( this, "CleanCacheDlg_btnRestartUnable", m_pBtnRestartUnable);

	pLayoutMgr->CreateControlPane( this,"CleanCacheDlg","normal");	
	pLayoutMgr->CreateBmpPane( this,"CleanCacheDlg","normal" );

	m_pBtnRestart->ShowWindow(SW_HIDE);
	m_pBtnRestartUnable->ShowWindow(SW_HIDE);
	m_pBtnRestartUnable->EnableWindow(FALSE);

	m_staticBmpLoading.ShowWindow(SW_HIDE);
	m_staticBmpLoading.MoveWindow(92, 77, 22, 22, FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CCleanCacheDlg::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	rc.bottom=rc.top+29;
	return rc.PtInRect(point)?HTCAPTION:CDialog::OnNcHitTest(point);
}


BOOL CCleanCacheDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rct;
	CDC tmpDC;
	
	GetWindowRect(&rct);
	tmpDC.CreateCompatibleDC(pDC);
	HBITMAP bmp = m_pBkShowing->CreateBitmap(pDC);
	tmpDC.SelectObject(bmp);
	pDC->BitBlt(0, 0, rct.Width(), rct.Height(), &tmpDC, 0, 0, SRCCOPY);
	ReleaseDC(&tmpDC);

	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

void CCleanCacheDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ID_TIMER_LOADING)
	{
		CRect rct;
		m_staticBmpLoading.GetWindowRect(&rct);
		rct.right = rct.left+22;
		rct.bottom = rct.top+22;

		static int i = 0;
		CDC *pDC = GetDC();
		m_staticBmpLoading.SetBitmap(m_LoadingBmp[i++]);
		i = i>7 ? 0:i;
		ReleaseDC(pDC);
	}
}

void CCleanCacheDlg::OnBnClickedCleanCache()	// 开始清理缓存
{

	m_hCleanThread = CreateThread(0, 0, CCleanCacheDlg::CleanCacheThread, (LPVOID)m_hWnd, 0, 0);
	if (m_hCleanThread)
	{
		SetTimer(ID_TIMER_LOADING, 100, NULL);
		m_staticBmpLoading.ShowWindow(SW_SHOW);
		m_pBkShowing = m_pBkCleanningCache;
		Invalidate(TRUE);	// 重绘窗口
		m_pBtnRestartUnable->ShowWindow(SW_SHOW);
		m_pBtnCleanCache->ShowWindow(SW_HIDE);
	}
}

void CCleanCacheDlg::OnBnClickedCancel()	// 取消或者关闭
{
	if (m_hCleanThread)
	{
		KillTimer(ID_TIMER_LOADING);
		m_staticBmpLoading.ShowWindow(SW_HIDE);
		// 终止线程
		TerminateThread(m_hCleanThread, 0);
		CloseHandle(m_hCleanThread);
		m_hCleanThread = NULL;
	}
	return __super::OnCancel();
}

void CCleanCacheDlg::OnBnClickedRestart()
{
	return __super::OnOK();
}

LRESULT CCleanCacheDlg::OnCleanCacheDone(WPARAM wParam, LPARAM lParam)
{
	KillTimer(ID_TIMER_LOADING);
	m_staticBmpLoading.ShowWindow(SW_HIDE);
	m_pBkShowing = m_pBkCleanedCache;
	Invalidate(TRUE);	// 重绘窗口
	m_pBtnRestart->ShowWindow(SW_SHOW);
	m_pBtnRestartUnable->ShowWindow(SW_HIDE);
	return 0;
}

DWORD CCleanCacheDlg::CleanCacheThread(LPVOID lpParam)
{
	HWND hWnd = (HWND)lpParam;
	Sleep(3000);

	char szPath[MAX_PATH] = {0};
	if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_INTERNET_CACHE, FALSE))
	{ //得到临时目录，并清空它.
		EmptyDirectory(szPath);
	}

	// DelTempFiles();
	::PostMessage(hWnd, WM_CLEAN_CACHE_DONE, 0, 0);
	return 0;
}

BOOL CCleanCacheDlg::EmptyDirectory(char* szPath)
{
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	CString sFullPath;
	CString sFindFilter;
	DWORD dwAttributes=0;

	sFindFilter=szPath;
	sFindFilter+="\\*.*";
	hFind = FindFirstFile(sFindFilter,&wfd);

	if(hFind == INVALID_HANDLE_VALUE)
		return FALSE;
	do
	{
		if(memcmp(wfd.cFileName,".",1)==0||memcmp(wfd.cFileName,"..",1)==0)
			continue;
		sFullPath=szPath;
		sFullPath+="\\";
		sFullPath+=wfd.cFileName;

		dwAttributes = GetFileAttributes(sFullPath);
		if (dwAttributes & FILE_ATTRIBUTE_READONLY)
		{
			dwAttributes &= ~FILE_ATTRIBUTE_READONLY;
			SetFileAttributes(sFullPath, dwAttributes);
		}

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			EmptyDirectory(sFullPath.GetBuffer(0));
			RemoveDirectory(sFullPath);
		}
		else
		{
			if (strnicmp(wfd.cFileName, "index.dat", 9) == 0) {
				continue;
			}
			else if (strnicmp(wfd.cFileName, "desktop.ini", 11)) {
				continue;
			}
			else {
				DeleteFile(sFullPath);
			}
		}
	}while (FindNextFile(hFind, &wfd));
	FindClose(hFind);
	return TRUE;
}