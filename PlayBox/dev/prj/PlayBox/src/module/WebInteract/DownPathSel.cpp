// MinOrExit.cpp : 实现文件
//

#include "stdafx.h"
#include "DownPathSel.h"
#include "../../Core/AfxGlobals.h"
#include "../../Global/GlobalVariable.h"
#include "../../AppConfig/config/ConfigSettingDef.h"
#include "../../AppConfig/config/ConfigAppDef.h"
#include "YL_DirInfo.h"
#include "YL_StringUtil.h"
#include "../../gui/util/CBufferDC.h"

#define ID_BTN_DOWNPATHSEL_BROWSE	10001
#define ID_BTN_DOWNPATHSEL_OK		10002
#define	ID_BTN_DOWNPATHSEL_CANCLE	10003
#define ID_EDIT_PATH				10004
//
// added by wang
//
#define ID_BTN_DOWNPATHSEL_POPUP_DIALOG		10012
#define ID_BTN_DOWNPATHSEL_UNPOPUP_DIALOG	10013

// CDownPathSel 对话框

IMPLEMENT_DYNAMIC(CDownPathSel, CDialog)
CDownPathSel::CDownPathSel(CWnd* pParent /*=NULL*/)
	: CDialog(CDownPathSel::IDD, pParent)
	,m_bChkIsPopUp(FALSE)
{
	m_bkg= ::AfxGetUIManager ()->UIGetSkinMgr ()->GetDibBmp ("DownPathSelBk");
	m_bSepLine = AfxGetUIManager ()->UIGetSkinMgr ()->GetDibBmp ("DownPathSelSeparator");
	
	m_strPath = CString ("");
}

CDownPathSel::~CDownPathSel()
{
	if (m_edit.m_hWnd)
		m_edit.DestroyWindow ();
}

void CDownPathSel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDownPathSel, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_NCHITTEST()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CDownPathSel 消息处理程序
BOOL CDownPathSel::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowRgn (m_bkg->CreateRgnFromBitmap (RGB(255, 255, 255), this), TRUE);

	string strDefaultPath;
	AfxGetUserConfig()->GetConfigStringValue (CONF_APP_MODULE_NAME, CONF_APP_SWF_PATH, strDefaultPath);

	//
	// added by wang
	//
	bool bIsPopUpDialogbox = false;
// 	AfxGetUserConfig()->GetConfigBoolValue(CONF_APP_MODULE_NAME, CONF_APP_IS_POPUP_BOX, bIsPopUpDialogbox);
	m_bChkIsPopUp = bIsPopUpDialogbox;

	if (!strDefaultPath.length ())
	{
		bool bRet = YL_DirInfo::GetLargestFreeBytesDisk (strDefaultPath);
		if (!bRet)
		{
			char szHomePath[MAX_PATH] = {0};
			CLhcImg::GetHomePath (szHomePath, MAX_PATH);
			string strTmp = szHomePath;
			strDefaultPath = strTmp.substr (0, strTmp.find_first_of ("\\"));
			strDefaultPath += "\\";
			strDefaultPath += SWF_SAVE_PATH;
		}
		else
		{
			strDefaultPath += SWF_SAVE_PATH;
		}
	}

	m_edit.SetWindowText (strDefaultPath.c_str ());

	m_Font.CreateFont(
		12,                        // nHeight
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
		"宋体");                 // lpszFacename

	m_edit.MoveWindow (20, 65, 198, 18);
	m_edit.m_bDrawBorderSelf = true;

	CRect winPos (0, 0, 296, 156);
	CRect winPosData (winPos);
	if (m_bkg != NULL)
	{
		winPos.right = 296;
		winPos.bottom = 156;
	}
	CRect wndRect;
	
	CWnd *pWnd = GetParent();
	if (pWnd == NULL)
		pWnd = AfxGetMainWindow ();

	pWnd->GetWindowRect(&wndRect);
	int x,y;
	
	x=wndRect.CenterPoint().x-winPosData.Width()/2;
	y=wndRect.CenterPoint().y-winPosData.Height()/2;
	int screenX=GetSystemMetrics(SM_CXSCREEN);
	int screenY=GetSystemMetrics(SM_CYSCREEN);
	if (x>=0)
	{
		winPos.left=x;
	}
	else
	{
		winPos.left=20;
	}
	winPos.right=winPos.left+winPosData.Width();
	
	if (winPos.right>screenX)
	{
		winPos.right=screenX-20;
		winPos.left=winPos.right-winPosData.Width();
	}
	if (y>0)
	{
		winPos.top=y;
	}
	else
	{
		winPos.top=20;
	}
	winPos.bottom=winPos.top+winPosData.Height();
	if (winPos.bottom>screenY)
	{
		winPos.bottom=screenY-20;
		winPos.top=winPos.bottom-winPosData.Height();
	}
	MoveWindow(winPos);

	return TRUE;
}

BOOL CDownPathSel::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD lowWord	= LOWORD( wParam );
	switch (lowWord)
	{
	case ID_BTN_DOWNPATHSEL_CANCLE:
		CDialog::OnCancel();
		break;
	case ID_BTN_DOWNPATHSEL_OK:
		m_edit.GetWindowText (m_strPath);
		OnOK();
		break;
	case ID_BTN_DOWNPATHSEL_BROWSE:
		{
			BROWSEINFO bi;  
			ITEMIDLIST *pidl;  
			char Dir[MAX_PATH] = {0};

			bi.hwndOwner = NULL;  
			bi.pidlRoot = NULL;  
			bi.pszDisplayName = Dir;  
			bi.lpszTitle = "选择一个目录";  
			bi.ulFlags = BIF_RETURNONLYFSDIRS;  
			bi.lpfn = NULL;  
			bi.lParam = 0;  
			bi.iImage = 0;  

			pidl = SHBrowseForFolder( &bi );
			if (pidl == NULL)
				Dir[0] = 0;

			if (!SHGetPathFromIDList (pidl, Dir))
				Dir[0] = 0; 

			if (strlen (Dir))
				m_edit.SetWindowText (Dir);
		}
		break;
	}

	return CDialog::OnCommand(wParam,lParam);
}



void CDownPathSel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
// 	AfxGetUIManager()->UIGetLayoutMgr()->PaintBkGround( m_hWnd ,&dc,true );

	CBufferDC pDC(dc.m_hDC);

	m_oldFont = (CFont *)pDC.SelectObject (&m_Font);
	int nOldMode = pDC.SetBkMode (TRANSPARENT);

	CRect rcWindow;
	GetClientRect (&rcWindow);
	m_bkg->SetCDibRect (rcWindow);
	m_bkg->Draw (&pDC, FALSE);

	pDC.SetTextColor (RGB(255, 255,255));

	CRect rcTitle, rcName, rcSize;
	rcTitle = CRect (20, 15, 135, 30);
	pDC.DrawText ("下载保存路径", &rcTitle, DT_SINGLELINE | DT_VCENTER);


	
	rcName = CRect (20, 40, 200, 55);

	CSize szName = pDC.GetTextExtent (m_strName.c_str ());
	if (szName.cx < 180)
		rcName.right = rcName.left + szName.cx;

	pDC.DrawText (m_strName.c_str (), &rcName, DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);

	rcSize = CRect (rcName.right + 15, 40, rcName.right + 85, 55);
	pDC.DrawText (m_strSize.c_str (), &rcSize, DT_SINGLELINE | DT_VCENTER);

	CRect rcSepLine (2, 116, 292, 118);
	m_bSepLine->SetCDibRect (rcSepLine);
	m_bSepLine->Draw (&pDC, FALSE);

	m_edit.ShowSearchEditFrame (&pDC, false);
	
	//
	// added by wang
	//
	CRect rcCheckPath(45,155, 200, 50);
	/*pDC.Create(NULL,WS_VISIBLE, &rcCheckPath,this,ID_BTN_MINORCLOSE_BOX);*/
	pDC.DrawText("每次下载都弹出路径选择窗口", &rcCheckPath,  DT_SINGLELINE | DT_VCENTER );
	

	pDC.SetBkMode (nOldMode);
	pDC.SelectObject (m_oldFont);
}

void CDownPathSel::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值


	CRect boxRect(20,90,200,107);
	if(PtInRect(&boxRect,point))
	{
		if(m_bChkIsPopUp) {
			UnCheckBox();
		}else {
			CheckBox();
		}
	}


	CDialog::OnLButtonUp(nFlags, point);
}

UINT CDownPathSel::OnNcHitTest(CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	rc.bottom=rc.top+10;

	return rc.PtInRect(point)?HTCAPTION:CDialog::OnNcHitTest(point);
}

void CDownPathSel::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	//
	// added by wang
	//
	bool bRs = FALSE;
	bRs = m_bChkIsPopUp;
	ShowWindow(SW_HIDE);
	AfxGetUserConfig()->SetConfigBoolValue( CONF_APP_MODULE_NAME, CONF_APP_IS_POPUP_BOX, bRs, TRUE);
	CDialog::OnOK();
}

void CDownPathSel::CheckBox()
{
	m_bChkIsPopUp = TRUE;
	m_chkPopUpDialog.ShowWindow(SW_SHOW);
	m_ChkUnPopUpDialog.ShowWindow(SW_HIDE);
}
void CDownPathSel::UnCheckBox()
{
	m_bChkIsPopUp = FALSE;
	m_chkPopUpDialog.ShowWindow(SW_HIDE);
	m_ChkUnPopUpDialog.ShowWindow(SW_SHOW);
}

int CDownPathSel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rectNULL (0, 0, 0, 0);

	m_btnBrowse.Create (NULL, WS_VISIBLE, rectNULL, this, ID_BTN_DOWNPATHSEL_BROWSE);
	m_btnOk.Create (NULL, WS_VISIBLE, rectNULL, this, ID_BTN_DOWNPATHSEL_OK);
	m_btnCancel.Create (NULL, WS_VISIBLE, rectNULL, this, ID_BTN_DOWNPATHSEL_CANCLE);
	// added by wang
	m_chkPopUpDialog.Create(NULL,WS_VISIBLE,rectNULL,this,ID_BTN_DOWNPATHSEL_POPUP_DIALOG);
	m_ChkUnPopUpDialog.Create(NULL,WS_VISIBLE,rectNULL,this,ID_BTN_DOWNPATHSEL_UNPOPUP_DIALOG);



	ISkinMgr * pSkinMgr = AfxGetUIManager ()->UIGetSkinMgr ();
	m_btnBrowse.SetSkin (pSkinMgr->GetDibBmp ("DownPathSelBrowseNormal"), 
		pSkinMgr->GetDibBmp ("DownPathSelBrowseNormal"), 
		pSkinMgr->GetDibBmp ("DownPathSelBrowseHover"), 
		NULL, NULL, NULL, 0, 0, 0);

	m_btnOk.SetSkin (pSkinMgr->GetDibBmp ("DownPathSelOkNormal"), 
		pSkinMgr->GetDibBmp ("DownpathSelOkNormal"), 
		pSkinMgr->GetDibBmp ("DownPathSelOkHover"), NULL, NULL, NULL, 0, 0, 0);

	m_btnCancel.SetSkin (pSkinMgr->GetDibBmp ("DownPathSelCancelNormal"), 
		pSkinMgr->GetDibBmp ("DownPathSelCancelNormal"), 
		pSkinMgr->GetDibBmp ("DownPathSelCancelHover"), NULL, NULL, NULL, 0, 0, 0);
	//
	// added by wang
	//
	m_chkPopUpDialog.SetSkin (pSkinMgr->GetDibBmp ("DownPathSel_PopUpDialog"), 
		pSkinMgr->GetDibBmp ("DownPathSel_PopUpDialog"), NULL, NULL, NULL, NULL, 0, 0, 0);
	m_ChkUnPopUpDialog.SetSkin (pSkinMgr->GetDibBmp ("DownPathSel_UnPopUpDialog"), 
		pSkinMgr->GetDibBmp ("DownPathSel_PopUpDialog"), NULL, NULL, NULL, NULL, 0, 0, 0);

	ILayoutMgr* pLayoutMgr = AfxGetUIManager()->UIGetLayoutMgr();

	pLayoutMgr->RegisterCtrl (this, "DownPathSel_browse", &m_btnBrowse);
	pLayoutMgr->RegisterCtrl (this, "DownPathSel_ok", &m_btnOk);
	pLayoutMgr->RegisterCtrl (this, "DownPathSel_cancel", &m_btnCancel);

	//added by wang
	pLayoutMgr->RegisterCtrl(this, "DownPathSel_PopUpDialog", &m_chkPopUpDialog);
	pLayoutMgr->RegisterCtrl( this,"DownPathSel_UnPopUpDialog",&m_ChkUnPopUpDialog);
	

	pLayoutMgr->CreateControlPane (this, "DownPathSel", "normal");
	pLayoutMgr->CreateBmpPane (this, "DownPathSel", "normal");





	

	m_edit.Create (WS_CHILD | WS_VISIBLE | ES_READONLY, rectNULL, this, ID_EDIT_PATH);

	CDibBitmap *pBk = AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("DownPathSelBk");
	CString path = pBk->GetPath();
	int pos = path.ReverseFind('\\');
	path = path.Mid(0,pos);
	m_edit.SetPngBorder (
		string(path + "\\DownPathSelEditBk.bmp"),
		string(path + "\\DownPathSelEditBk.bmp"));

	return 0;
}

#define KB 1024
#define MB (1024*KB)

void CDownPathSel::SetParam (string strName, string strSize)
{
	m_strName = strName;
	YL_StringUtil::Format (m_strSize, "%0.2fMB", (double)atol (strSize.c_str ()) / KB);
}
