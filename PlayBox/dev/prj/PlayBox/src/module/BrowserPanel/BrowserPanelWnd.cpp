#include "stdafx.h"
#include "resource.h"
#include "BrowserPanelWnd.h"
#include "../WebInteract/MyWebBrowserWnd.h"
#include "../WebInteract/WebManager.h"
#include "../../core/CDataManager.h"
#include "../../gui/CommonControl/xSkinButton.h"
#include "../../gui/CommonControl/xStaticText.h"
#include "../../Gui/CommonControl/EditEx.h"
#include ".\browserpanelwnd.h"
#include "src/GUI/CommonControl/LocalSearchEdit.h"
#include "src/GUI/util/ShowMenu.h"

IMPLEMENT_DYNAMIC(BrowserPanelWnd, CBasicWnd)
BrowserPanelWnd::BrowserPanelWnd()
{
	m_pBtnBack = new CxSkinButton();
	m_pBtnForward = new CxSkinButton();
	m_pBtnRefresh = new CxSkinButton();
	m_pBtnFav = new CxSkinButton();
	m_pEditAddress = new CLocalSearchEdit();
	m_pWndBrowser = new MyWebBrowserWnd();
}

BrowserPanelWnd::~BrowserPanelWnd()
{
	delete m_pBtnBack;
	delete m_pBtnForward;
	delete m_pBtnRefresh;
	delete m_pBtnFav;
	delete m_pEditAddress;
	//do not delete m_pWndBrowser
	//delete m_pWndBrowser;
}

BEGIN_MESSAGE_MAP(BrowserPanelWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_BROWSER_BACK,OnClickedBack)
	ON_BN_CLICKED(IDC_BTN_BROWSER_FORWARD,OnClickedForward)
	ON_BN_CLICKED(IDC_BTN_BROWSER_REFRESH,OnClickedRefresh)
	ON_BN_CLICKED(IDC_BTN_BROWSER_FAV,OnClickedFav)
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
	ON_MESSAGE(WM_PAGE_CHANGED, OnPageChanging)
	ON_COMMAND_RANGE(IDC_MENU_ITEM_BEGIN_ID, IDC_MENU_ITEM_END_ID, OnClickeFavUrlMenu)
END_MESSAGE_MAP()

int BrowserPanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectNull(0,0,0,0);
	m_pBtnBack->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_BROWSER_BACK);
	m_pBtnForward->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_BROWSER_FORWARD);
	m_pBtnRefresh->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_BROWSER_REFRESH);
	m_pBtnFav->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_BROWSER_FAV);
	m_pEditAddress->Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP,rectNull,this,IDC_BTN_BROWSER_ADDRESS_BAR);
	m_pWndBrowser->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,IDC_BROWSER_WEB );

	m_editFont.CreateFont(14,      // nHeight
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
	m_pEditAddress->SetFont(&m_editFont);
	m_pEditAddress->SetMargins(3, 0);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();	
	pLayoutMgr->RegisterCtrl( this, "BrowserBack", m_pBtnBack );
	pLayoutMgr->RegisterCtrl( this, "BrowserForward", m_pBtnForward );
	pLayoutMgr->RegisterCtrl( this, "BrowserRefresh", m_pBtnRefresh );
	pLayoutMgr->RegisterCtrl( this, "BrowserFav", m_pBtnFav );
	pLayoutMgr->RegisterCtrl( this, "BrowserAddressBar", m_pEditAddress );
	pLayoutMgr->RegisterCtrl( this, "BrowserWeb", m_pWndBrowser );

	pLayoutMgr->CreateControlPane( this, "browserpanel", "normal" );
	pLayoutMgr->CreateBmpPane( this,"browserpanel","normal" );

	return 0;
}

void BrowserPanelWnd::OnDestroy()
{
	m_pWndBrowser->DestroyWindow();
	__super::OnDestroy();
}

void BrowserPanelWnd::OnClickedBack()
{
	m_pWndBrowser->GoBack();
}

void BrowserPanelWnd::OnClickedForward()
{
	m_pWndBrowser->GoForward();
}

void BrowserPanelWnd::OnClickedRefresh()
{
	m_pWndBrowser->Refresh();
}

void BrowserPanelWnd::OnClickedFav()
{
	ShowFavUrlMenu();
}

void BrowserPanelWnd::Navigate(string strUrl)
{
	if (strUrl.empty())
	{
		strUrl = "about:blank";
	}
	m_pWndBrowser->Navigate(strUrl);
}

void BrowserPanelWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( ( GetKeyState(VK_CONTROL) & 0xF0000000) &&
		( nChar=='A' || nChar=='a') )
	{
		return;
	}
}
BOOL BrowserPanelWnd::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		{
			if (pMsg->hwnd == m_pEditAddress->GetSafeHwnd()
				&& pMsg->wParam == VK_RETURN)
			{
				CString str;
				m_pEditAddress->GetWindowText(str);
				Navigate(str.GetBuffer(0));
				str.ReleaseBuffer();
			}
		}
		break;
	}

	return CBasicWnd::PreTranslateMessage(pMsg);
}

LRESULT BrowserPanelWnd::OnPageChanging(WPARAM wParam, LPARAM lParam)
{
	// TRACE(TEXT("\r\n%s\r\n"), (LPCTSTR)wParam);
	if (wParam != 0)
	{
		m_pEditAddress->SetWindowText((LPCTSTR)wParam);
	}
	if (lParam != 0)
	{
		TAB_ITEM iItem;
		GLOBAL_TABBARDATA->ITabBar_GetCurItem(iItem);
		iItem.strTitle = (LPCTSTR)lParam;
		GLOBAL_TABBARDATA->ITabBar_ChangeTab(iItem);
	}
	// GLOBAL_TABBARDATA->ITabBar_ChangeTab(iItem);
	return 0;
}

int BrowserPanelWnd::AddFavUrlToMenu( int &nStartItemID, int nStartPos, const char* szPath, CMenu* pMenu )
{
	vector<string>	m_arrSubDir;
	CString strPath = szPath;
	int curCnt = 0;

	if(strPath[strPath.GetLength() - 1] != _T('\\'))
		strPath += _T('\\');
	CString strFind = strPath + "*.*";

	WIN32_FIND_DATA	findData;
	HANDLE	hFile = NULL;

	hFile = FindFirstFile(strFind, &findData);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if ( strcmp(".", findData.cFileName )==0
				|| strcmp("..", findData.cFileName)==0)
			{
				continue;
			}

			// 略过隐藏文件和系统文件
			if ( (findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
				|| (findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM))
			{
				continue;
			}

			// 目录
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				string strSubDir = strPath;
				strSubDir += findData.cFileName;
				m_arrSubDir.push_back(strSubDir);
			}

			// 文件
			CString strFileName = findData.cFileName;
			strFileName.MakeLower();
			if (strFileName.Right(4) == ".url")
			{
				char szUrl[MAX_PATH] = {0};
				::GetPrivateProfileString(_T("InternetShortcut"), _T("URL"),
					"", szUrl, 400,
					strPath+strFileName);

				strFileName = strFileName.Left(strFileName.GetLength() - 4);

				m_arrFavMenuItemParam.push_back(szUrl);
				pMenu->AppendMenu(MF_STRING|MF_ENABLED, nStartItemID++, strFileName.GetBuffer(0));
				strFileName.ReleaseBuffer();
				curCnt++;
			}

		} while (FindNextFile(hFile, &findData));
	}
	FindClose(hFile);

	vector<string>::iterator it = m_arrSubDir.begin();
	for (; it != m_arrSubDir.end(); it++)
	{
		CMenu *pSubMenu = new CMenu;
		pSubMenu->CreatePopupMenu();

		int n = AddFavUrlToMenu(nStartItemID, 0, it->c_str(), pSubMenu);
		if (n > 0)
		{
			string dir = *it;
			size_t pos = dir.rfind('\\');
			dir = dir.substr(pos+1, dir.length()-pos);
			pMenu->InsertMenu(nStartItemID++, MF_BYPOSITION | MF_POPUP | MF_STRING, (UINT)pSubMenu->m_hMenu, dir.c_str());
			pSubMenu->Detach();
			m_arrFavMenuItemParam.push_back(it->c_str());
			curCnt++;
		}
		delete pSubMenu;
	}

	return curCnt;
}

void BrowserPanelWnd::ShowFavUrlMenu()
{
	char szPath[MAX_PATH] = {0};

	if ( SHGetSpecialFolderPath(NULL, szPath, CSIDL_FAVORITES, FALSE) )
	{
		CMenu menu;
		menu.CreatePopupMenu();
		if ( 1 )
		{
			int nBeginID = IDC_MENU_ITEM_BEGIN_ID;
			m_arrFavMenuItemParam.clear();

			menu.AppendMenu(MF_STRING|MF_ENABLED, nBeginID++, "添加到收藏夹");
			m_arrFavMenuItemParam.push_back("Favorate");

			menu.AppendMenu(MF_SEPARATOR, nBeginID++);
			m_arrFavMenuItemParam.push_back("Separator");

			AddFavUrlToMenu(nBeginID, 2, szPath, &menu);

			CRect rct;
			m_pBtnFav->GetWindowRect(rct);
			menu.TrackPopupMenu(0, rct.right, rct.bottom, this);
			menu.DestroyMenu();
		}
	}
}

void BrowserPanelWnd::OnClickeFavUrlMenu(UINT nID)
{
	if (nID == IDC_MENU_ITEM_BEGIN_ID)
	{
		// 添加到收藏夹
		CString strURL;
		TAB_ITEM item;
		char szPath[MAX_PATH] = {0};
		m_pEditAddress->GetWindowText(strURL);	// 获取地址栏的地址
		GLOBAL_TABBARDATA->ITabBar_GetCurItem(item);	// 获取tab页标题

		if ( SHGetSpecialFolderPath(NULL, szPath, CSIDL_FAVORITES, FALSE) )
		{
			CString strFile = szPath;
			if(strFile[strFile.GetLength() - 1] != _T('\\'))
				strFile += _T('\\');
			strFile += item.strTitle.c_str();
			strFile += ".url";
			
			// 写入快捷方式到收藏夹
			::WritePrivateProfileString("InternetShortcut", "URL",
				strURL.GetBuffer(0),
				strFile.GetBuffer(0));
			strURL.ReleaseBuffer();
			strFile.ReleaseBuffer();
		}
	}
	else
	{
		string url = m_arrFavMenuItemParam[nID-IDC_MENU_ITEM_BEGIN_ID];
		Navigate(url);
	}
}