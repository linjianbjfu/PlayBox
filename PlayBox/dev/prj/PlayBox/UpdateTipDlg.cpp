#include "stdafx.h"
#include "UpdateTipDlg.h"
#include "YL_Ini.h"
#include "LhcImg.h"

#define WM_VIEW_UPDATE_INFO WM_USER + 11225
#define UPDATE_INFO_HEIGHT  100          

BEGIN_MESSAGE_MAP( CHyperLinkExpand, CHyperLink )
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CHyperLinkExpand::OnLButtonDown(UINT nFlags, CPoint point)
{

}

void CHyperLinkExpand::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect rect;
	GetWindowRect(rect);
	ClientToScreen(&point);
	if (PtInRect(rect, point))	// If the mouse is over us follow the link.
	{
		::PostMessage(GetParent()->GetSafeHwnd(),WM_VIEW_UPDATE_INFO,0,0);
	}
}

IMPLEMENT_DYNAMIC(CUpdateTipDlg, CDialog)
CUpdateTipDlg::CUpdateTipDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUpdateTipDlg::IDD, pParent)
{
}

CUpdateTipDlg::~CUpdateTipDlg()
{
}

void CUpdateTipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_UPDATE_INFO, m_linkUpdateInfo);
	DDX_Text(pDX, IDC_EDIT_UPDATE_INFO, m_strUpdateInfo);
}


BEGIN_MESSAGE_MAP(CUpdateTipDlg, CDialog)
	ON_MESSAGE(WM_VIEW_UPDATE_INFO, OnViewUpdateInfo)
END_MESSAGE_MAP()

void CUpdateTipDlg::SetForceUpdate(int nForce)
{
	m_nForceUpdate = nForce;
}

BOOL CUpdateTipDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CMenu* pMenu = this->GetSystemMenu(FALSE);
	pMenu->EnableMenuItem( SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);

	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	wp.rcNormalPosition.bottom -= UPDATE_INFO_HEIGHT;
	SetWindowPlacement(&wp);

	if(m_hWnd != NULL)
	{
		::SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	}

	if(m_nForceUpdate == MB_OK)
	{
		CWnd *pWnd = GetDlgItem(IDCANCEL);
		if(pWnd != NULL)
		{
			pWnd->ShowWindow(SW_HIDE);
		}

		pWnd = GetDlgItem(IDOK);
		if(pWnd != NULL)
		{
			CRect rect;
			pWnd->GetWindowRect(&rect);
			rect.left += 50;
			rect.right += 50;
			ScreenToClient(&rect);
			pWnd->MoveWindow(&rect);
		}
	}

	m_linkUpdateInfo.SetLinkCursor( AfxGetApp()->LoadCursor(IDC_CURSOR1) );

	char szImgPath[MAX_PATH*2], szInfo[1024*10];
	CLhcImg::GetHomePath(szImgPath, MAX_PATH*2-1);
	strcat(szImgPath, "\\res\\pic\\lhupd.img");

	YL_Ini ini(szImgPath);
	ini.GetString("NewestSoft", "intro", szInfo, 1024*10-1);
	m_strUpdateInfo = szInfo;
	m_strUpdateInfo.Replace("||", "\r\n");
	UpdateData(FALSE);
 
	return TRUE;
}

LRESULT CUpdateTipDlg::OnViewUpdateInfo(WPARAM wParam, LPARAM lParam)
{
	static bool bViewInfo = false;

	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	if(bViewInfo)
	{
		wp.rcNormalPosition.bottom -= UPDATE_INFO_HEIGHT;
	}
	else
	{
		wp.rcNormalPosition.bottom += UPDATE_INFO_HEIGHT;
	}
	SetWindowPlacement(&wp);

	bViewInfo = !bViewInfo;
	return 0;
}
