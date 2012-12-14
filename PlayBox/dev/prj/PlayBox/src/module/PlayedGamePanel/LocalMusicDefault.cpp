#include "stdafx.h"
#include "LocalMusicDefault.h"
#include ".\localmusicdefault.h"
#include "../Scan/ScanControl.h"
#include "../../../../InfoBarCtrl.h"
#include "KwcImg.h"
#include "YL_Ini.h"
#include "../LocalMusicUIMgr.h"

#define ID_BTN_CHOOSE	42780
#define ID_BTN_AUTO		42781

CLocalMusicDefault::CLocalMusicDefault()
{
	m_btnChoose = new CxSkinButton;
	m_btnAuto = new CxSkinButton;
	ScanControl::GetInstance()->AddScanObserver(this);
}

CLocalMusicDefault::~CLocalMusicDefault()
{
	delete m_btnChoose;
	delete m_btnAuto;
	ScanControl::GetInstance()->LeaveScanObserver(this);
}

BOOL CLocalMusicDefault::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext /* = NULL */)
{
	BOOL ret = CWnd::Create(AfxRegisterWndClass(CS_DBLCLKS| CS_CLASSDC | CS_SAVEBITS,AfxGetApp()->LoadStandardCursor(IDC_ARROW),0,0),
		_T(""), dwStyle, rect, pParentWnd, nID, pContext);
	m_font.CreateFont(
		12,						     // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet 
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"宋体"); 
	return ret;	
}

BEGIN_MESSAGE_MAP(CLocalMusicDefault, CWnd)
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_PAINT()
ON_BN_CLICKED(ID_BTN_CHOOSE,OnBtnClickChoose)
ON_BN_CLICKED(ID_BTN_AUTO,OnBtnClickAuto)
ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

int CLocalMusicDefault::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_btnChoose->SetSkin(
		AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("CPBtnChooseNormal"),
		AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("CPBtnChooseDown"),
		AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("CPBtnChooseOver"),AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("CPBtnChooseDisable"),NULL,NULL,0,0,0);
	m_btnChoose->Create(_T(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,ID_BTN_CHOOSE);

	m_btnAuto->SetSkin(
		AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("CPBtnAutoNormal"),
		AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("CPBtnAutoDown"),
		AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("CPBtnAutoOver"),AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("CPBtnAutoDisable"),NULL,NULL,0,0,0);
	m_btnAuto->Create(_T(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,ID_BTN_AUTO);

	return 0;
}

void CLocalMusicDefault::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if (m_btnChoose->GetSafeHwnd())
	{
		//93+30+118
		m_btnChoose->MoveWindow((cx-241)/2,240,93,20);		
	}
	if (m_btnAuto->GetSafeHwnd())
	{
		m_btnAuto->MoveWindow((cx-241)/2+123,240,118,20);
	}
}

void CLocalMusicDefault::OnPaint()
{
	CPaintDC dc(this); 
	CRect rc;
	GetClientRect(&rc);
	dc.FillSolidRect(&rc,RGB(255,255,255));
	int iW=300;
	CRect rcRound((rc.Width()-iW)/2, rc.top+120, (rc.Width()-iW)/2+iW,rc.top+280);
	CBrush br;
	CPen pen;
	pen.CreatePen(PS_SOLID,1,RGB(218,231,236));
	br.CreateSolidBrush(RGB(239,247,250));
	CBrush * pBrold = dc.SelectObject(&br);
	CPen* pPenold = dc.SelectObject(&pen);
	dc.RoundRect(&rcRound,CPoint(10,10));
	rcRound.left+=20;
	rcRound.top+=20;
	CString strText="\n请添加本地歌曲\n\n\n添加后，您就可以轻松管理这些歌曲了";
	int iBkMd = dc.SetBkMode(TRANSPARENT);
	CFont * pfold = dc.SelectObject(&m_font);
	dc.DrawText(strText,&rcRound,DT_LEFT);
	dc.SetBkMode(iBkMd);
	dc.SelectObject(pBrold);
	dc.SelectObject(pPenold);
	dc.SelectObject(pfold);
}

int CALLBACK CLocalMusicDefault::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED :
		{
			string path = CLocalMusicUIMgr::GetLastPath();
			::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)path.c_str()); 
		}
		break;
	default :
		break;
	}
	return 0;
}

void CLocalMusicDefault::OnBtnClickChoose()
{
	CString strDestFolder;
	LPMALLOC lpMalloc;
	if (::SHGetMalloc(&lpMalloc) != NOERROR) 
		return;
	char szDisplayName[_MAX_PATH];
	char szBuffer[_MAX_PATH];
	BROWSEINFO browseInfo;
	browseInfo.hwndOwner = m_hWnd;
	browseInfo.pidlRoot = NULL; 
	browseInfo.pszDisplayName = szDisplayName;
	browseInfo.lpszTitle = "请选择将要添加的文件夹";
	browseInfo.ulFlags = BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS;
	browseInfo.lpfn = CLocalMusicDefault::BrowseCallbackProc;
	browseInfo.lParam = 0;

	LPITEMIDLIST lpItemIDList;
	if ((lpItemIDList = ::SHBrowseForFolder(&browseInfo)) != NULL)
	{
		if (::SHGetPathFromIDList(lpItemIDList, szBuffer))
		{
			if (szBuffer[0] == '\0') return;

			PathAddBackslash(szBuffer);
			strDestFolder = szBuffer;
		}
		else if(strcmp(szBuffer,"")==0)
		{
			strDestFolder = szBuffer;
		}

		lpMalloc->Free(lpItemIDList);
		lpMalloc->Release();
	}
	else
	{
		return;
	}
	vector<string> vpath;
	if (strDestFolder!="")
		vpath.push_back(strDestFolder.GetBuffer());
	CLocalMusicUIMgr::SavePath(strDestFolder.GetBuffer());
	ScanControl::GetInstance()->StartScan(vpath);
}

void CLocalMusicDefault::OnBtnClickAuto()
{
	vector<string> v;
	ScanControl::GetInstance()->StartScan(v);
}


void CLocalMusicDefault::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (bShow)
	{
		CButton* pBtnLetterSwitch = (CButton*)AfxGetUIManager()->UIGetLayoutMgr()->GetCtrl(AfxGetMainWindow()->m_hWnd,"BtnLetterSwitch");
		if (pBtnLetterSwitch) pBtnLetterSwitch->SetCheck(FALSE);
		SetInfoText();
	}
	CWnd::OnShowWindow(bShow, nStatus);
}

void CLocalMusicDefault::SetInfoText()
{
	InfoBarCtrl::GetInstance()->ChangeInfo(string("酷我复制工具"));
}

void CLocalMusicDefault::IScanObserver_StartScan()
{
	m_btnChoose->EnableWindow(FALSE);
	m_btnAuto->EnableWindow(FALSE);
}

void CLocalMusicDefault::IScanObserver_EndScan()
{
	m_btnChoose->EnableWindow(TRUE);
	m_btnAuto->EnableWindow(TRUE);
}

