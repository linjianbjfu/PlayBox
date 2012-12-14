#include "stdafx.h"
#include "SortInfoWnd.h"
#include "SortInfoDef.h"

#include "PlayedGameWnd.h"
#include "../AppConfig/config/ConfigSettingDef.h"

#include "resource.h"
#include "../../gui/util/ShowMenu.h"
#include "MD5Checksum.h"
#include "../../util/MarkupArchive.h"
#include "YL_StringUtil.h"
#include "../../gui/util/CBufferDC.h"
#include <algorithm>
#include "../../core/CDataManager.h"

#include "../../../../CommonLib/common/tinyXML/tinyxml.h"
#include "../../../../CommonLib/common/version.h"

#include "YL_FileInfo.h"
#include "YL_RegInfo.h"

#pragma comment (lib, "Version.lib") // for GetFileVersionInfo

const char * GAMETOTALCNT = "共有%d款游戏";
const char * SORTTITLE	  = "排序：";
const char * SORTBYTIME   = "时间";
const char * SORTBYTYPE   = "类型";
const char * SORTBYTIMES  = "运行次数";

const COLORREF clrTitleSeparator = RGB (20, 20, 20);

static DWORD g_dwSearchGameCnt = 0;
static BOOL  g_bStopSearch = FALSE;
HANDLE SortInfoWnd::m_hFindThread = NULL;

IMPLEMENT_DYNAMIC(SortInfoWnd, CWnd)

SortInfoWnd * SortInfoWnd::m_pThis = NULL;

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) do { if ((x) != NULL) { delete (x); (x) = NULL; } }while (0)
#endif

struct GameVersion
{
	string strGameName;
	string strVersion;
	string strReleaseTime;
	string strExecuteFile;
	string strMD5Value;
	string strSize;

	GameVersion ()
	{
		strGameName = "";
		strVersion = "";
		strReleaseTime = "";
		strExecuteFile = "";
		strMD5Value = "";
		strSize = "";
	}
};

struct ThreadParameter
{
	LPVOID lpThis;
	string strPath;
};

static void SaveIcon(HICON hIconToSave, LPCTSTR sIconFileName)
{
	if(hIconToSave==NULL || sIconFileName==NULL)
		return;

	PICTDESC picdesc;
	picdesc.cbSizeofstruct = sizeof(PICTDESC);
	picdesc.picType = PICTYPE_ICON ;            
	picdesc.icon.hicon = hIconToSave;
	IPicture* pPicture=NULL;
	OleCreatePictureIndirect(&picdesc, IID_IPicture, TRUE,(VOID**)&pPicture);
	LPSTREAM pStream;
	CreateStreamOnHGlobal(NULL,TRUE,&pStream);
	LONG size;
	HRESULT hr=pPicture->SaveAsFile(pStream,TRUE,&size);
	char pathbuf[1024];
	strcpy(pathbuf,sIconFileName);
	CFile iconfile;
	iconfile.Open(pathbuf, CFile::modeCreate|CFile::modeWrite);
	LARGE_INTEGER li;
	li.HighPart =0;
	li.LowPart =0;
	ULARGE_INTEGER ulnewpos;
	pStream->Seek( li,STREAM_SEEK_SET,&ulnewpos);
	ULONG uReadCount = 1;
	while(uReadCount>0)
	{
		pStream->Read(pathbuf,sizeof(pathbuf),&uReadCount);
		if(uReadCount>0)
			iconfile.Write(pathbuf,uReadCount);
	}
	pStream->Release();
	iconfile.Close();
}

static void my1 (vector<string> & vStr)
{
	char szAppData[MAX_PATH] = {0};
	if (SHGetSpecialFolderPath (NULL, szAppData, CSIDL_COMMON_APPDATA, FALSE))
	{
		string strKWDataXml;
		YL_StringUtil::Format (strKWDataXml, "%s\\%s", szAppData, "kuaiwan\\data.xml");
		if (YL_FileInfo::IsValid (strKWDataXml))
		{
			CMarkupArchive xml;
			if( !xml.Open(strKWDataXml.c_str()) )
				return;

			xml.ResetPos();
			xml.FindElem();
			xml.IntoElem();

			while( xml.FindChildElem() )
			{
				string strTmp = xml.GetAttrib ("path");
				string strPath = strTmp.substr (0, strTmp.find ("\\games"));

				vector<string>::iterator itFind = find (vStr.begin (), vStr.end (), strPath);

				if (itFind == vStr.end ())
					vStr.push_back (strPath);
			}
		}
	}
}

static void my2 (vector<string> & vStr)
{
#define KKKEY	"SOFTWARE\\KuaiKuai\\ksvs\\apps"

	char szSubKey [MAX_PATH] = {0};
	DWORD dwIndex = 0;

	HKEY pKey;
	if (::RegOpenKey (HKEY_LOCAL_MACHINE, KKKEY, &pKey) == ERROR_SUCCESS)
	{
		while (::RegEnumKey (pKey, dwIndex++, szSubKey, MAX_PATH) == ERROR_SUCCESS)
		{
			string strSubKey;
			YL_StringUtil::Format (strSubKey, "%s\\%s", KKKEY, szSubKey);

			string strProgramDir;
			YL_RegInfo::ReadString (HKEY_LOCAL_MACHINE, strSubKey.c_str (),	
				"program_dir", strProgramDir);

			if (find (vStr.begin (), vStr.end (), strProgramDir) == vStr.end ())
				vStr.push_back (strProgramDir);
		}
	}
}

static void UTF8ToGB(string &szOut)
{
	WCHAR *strSrc;
	TCHAR *szRes;
	//长度
	int i = MultiByteToWideChar(CP_UTF8, 0, szOut.c_str (), -1, NULL, 0);
	strSrc = new WCHAR[i+1];
	MultiByteToWideChar(CP_UTF8, 0, szOut.c_str (), -1, strSrc, i);

	//长度
	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new TCHAR[i+1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

	szOut = szRes;

	delete []strSrc;
	delete []szRes;
}

static void my3 (vector<string> & vStr)
{
#define DJPath "DianJi\\LavaNet DNA\\AppData\\dgs\\dgs.xml"

	char szCommonFiles[MAX_PATH] = {0};
	SHGetSpecialFolderPath (NULL, szCommonFiles, CSIDL_PROGRAM_FILES_COMMON, FALSE);

	string strPath;
	YL_StringUtil::Format (strPath, "%s\\%s", szCommonFiles, DJPath);

	TiXmlDocument doc;
	bool loadOkay = doc.LoadFile(strPath.c_str ());

	if (loadOkay )
	{
		TiXmlNode* node = 0;
		TiXmlElement* filesElement = 0;
		TiXmlElement* itemElement = 0;

		node = doc.FirstChild ("files");
		if (node != NULL)
		{
			filesElement = node->ToElement();
			if (filesElement != NULL)
			{
				node = filesElement->FirstChildElement();
				while (node != NULL)
				{
                    itemElement = node->ToElement();
					if (itemElement != NULL)
					{
						const char * pPath = itemElement->Attribute ("path");
						string strValue (pPath);

						UTF8ToGB (strValue);

						if (find (vStr.begin (), vStr.end (), strValue) == vStr.end ())
							vStr.push_back (strValue);
					}

					node = itemElement->NextSiblingElement ();
				}
			}
		}
	}
}

static void my4 (vector<string> & vStr)
{
#define FTNN "SOFTWARE\\4399\\DownCore\\Downloads"

	string strDownPathTmp;
	if (YL_RegInfo::ReadString (HKEY_CURRENT_USER, FTNN, "CompletePath", strDownPathTmp))
	{
		string strDownPath = strDownPathTmp.substr (0, strDownPathTmp.rfind ("\\"));

		if (find (vStr.begin (), vStr.end (), strDownPath) == vStr.end ())
			vStr.push_back (strDownPath);
	}
}

int GetFL (vector<string> &vStr)
{
	vStr.clear ();

	my1 (vStr);
	my2 (vStr);
	my3 (vStr);
	my4 (vStr);

	return 1;
}

void InitFL2 (vector<string> & vStr)
{
	vStr.clear ();

	vStr.push_back ("windows");
	vStr.push_back ("winddk");
	vStr.push_back ("inetpub");
	vStr.push_back ("Documents and Settings");
	vStr.push_back ("4399");
	vStr.push_back ("kuaikuai");
	vStr.push_back ("kuaiwan");
	vStr.push_back ("dianji");
	vStr.push_back ("java");
	vStr.push_back ("Internet explorer");
	vStr.push_back ("adobe");
	vStr.push_back ("common files");
	vStr.push_back ("ksafe");
	vStr.push_back ("kwmusic");
	vStr.push_back ("microsoft");
	vStr.push_back ("msdn");
	vStr.push_back ("tencent");
	vStr.push_back ("thunder");
}

static vector<string> g_flString;
static vector<string> g_flString2;

static void FindGameR (LPVOID pThis, string strPath)
{
	SortInfoWnd * pSortInfoWnd = (SortInfoWnd*)pThis;

	CFileFind tempFind;
	char tempFileFind[MAX_PATH] = {0};

	if (strPath[strPath.length () - 1] == '\\')
		_snprintf (tempFileFind, MAX_PATH, "%s*.*", strPath.c_str ());
	else
		_snprintf (tempFileFind, MAX_PATH, "%s\\*.*", strPath.c_str ());

	BOOL IsFinded=(BOOL)tempFind.FindFile(tempFileFind);
	while(IsFinded && !g_bStopSearch)
	{
		IsFinded=(BOOL)tempFind.FindNextFile();
		if(!tempFind.IsDots())
		{
			if (tempFind.IsDirectory ())
			{
				if (find (g_flString.begin (), g_flString.end (), tempFind.GetFilePath ().GetBuffer ()) 
								!= g_flString.end ())
				{
#ifdef _DEBUG
					string strOutputInfo;
					YL_StringUtil::Format (strOutputInfo, "Skip dir: %s\n", tempFind.GetFilePath ().GetBuffer ());
					OutputDebugString (strOutputInfo.c_str ());
#endif
					continue;
				}

				vector<string>::iterator it = g_flString2.begin ();
				for (; it != g_flString2.end (); it++)
				{
					string strTmp = tempFind.GetFilePath ().GetBuffer ();
					if (StrStrI (strTmp.c_str (), (*it).c_str ()))
						break;
				}

				if (it != g_flString2.end ())
				{
#ifdef _DEBUG
					string strSkipFolder;
					YL_StringUtil::Format (strSkipFolder, "Skip dir: %s\n", tempFind.GetFilePath ().GetBuffer ());
					OutputDebugString (strSkipFolder.c_str ());
#endif
					continue;
				}

				FindGameR (pSortInfoWnd, tempFind.GetFilePath ().GetBuffer ());
			}
			else
			{
				char foundFileName[MAX_PATH] = {0};
				_snprintf (foundFileName, MAX_PATH, "%s", tempFind.GetFilePath().GetBuffer(MAX_PATH));

				pSortInfoWnd->DrawScanningText (foundFileName);

				if (tempFind.GetFileName ().Right (4) != ".exe")
					continue;

				GameVersion * pGameVersion = pSortInfoWnd->CheckMD5 (foundFileName);
				if (pGameVersion != NULL)
				{
					SHFILEINFO stFileInfo;
					SHGetFileInfo(foundFileName, 0, &stFileInfo,sizeof(stFileInfo), SHGFI_ICON | SHGFI_LARGEICON );
					if (stFileInfo.hIcon != NULL)
					{
						string strFilePath = tempFind.GetFilePath ().GetBuffer ();
						strFilePath = strFilePath.substr (0, strFilePath.rfind ("."));
						string strIconPath;
						YL_StringUtil::Format (strIconPath, "%s.ico", strFilePath.c_str ());
						SaveIcon (stFileInfo.hIcon, strIconPath.c_str ());
						DestroyIcon (stFileInfo.hIcon);

						if (GLOBAL_LOCALGAME->ILocalGameData_AddGame (pGameVersion->strMD5Value, pGameVersion->strGameName,
								strIconPath, tempFind.GetFilePath ().GetBuffer (), "", 1, pGameVersion->strMD5Value))
						{
							g_dwSearchGameCnt++;
						}
					}
				}
			}
		}
	}
}

static GetWindowsGameXP ()
{
	vector<pair<string, string> > vGame;
	vGame.push_back (make_pair("mshearts.exe", "网上红心大战"));
	vGame.push_back (make_pair("freecell.exe", "空当接龙"));
	vGame.push_back (make_pair("winmine.exe", "扫雷"));
	vGame.push_back (make_pair("spider.exe", "蜘蛛纸牌"));
	vGame.push_back (make_pair("sol.exe", "纸牌"));

	char szDir[MAX_PATH] = {0};
	SHGetSpecialFolderPath (NULL, szDir, CSIDL_SYSTEM, FALSE);

	for (vector<pair<string, string> >::iterator it = vGame.begin (); it != vGame.end (); it++)
	{
		string strGamePath;
		YL_StringUtil::Format (strGamePath, "%s\\%s", szDir, (*it).first.c_str ());

		if (YL_FileInfo::IsValid (strGamePath))
		{
			CString strMD5 = CMD5Checksum::GetMD5 (strGamePath.c_str ());

			SHFILEINFO stFileInfo;
			SHGetFileInfo(strGamePath.c_str (), 0, &stFileInfo,sizeof(stFileInfo), SHGFI_ICON | SHGFI_LARGEICON );
			if (stFileInfo.hIcon != NULL)
			{
				string strFilePath = strGamePath;
				strFilePath = strFilePath.substr (0, strFilePath.rfind ("."));
				string strIconPath;
				YL_StringUtil::Format (strIconPath, "%s.ico", strFilePath.c_str ());
				SaveIcon (stFileInfo.hIcon, strIconPath.c_str ());
				DestroyIcon (stFileInfo.hIcon);

				if (GLOBAL_LOCALGAME->ILocalGameData_AddGame (strMD5.GetBuffer (), (*it).second,
					strIconPath, strGamePath.c_str (), "", 1, strMD5.GetBuffer ()))
				{
					g_dwSearchGameCnt++;
				}
			}
		}
	}
}

static GetWindowsGameWin7 ()
{
	vector<pair<string, string> > vGame;

	vGame.push_back (make_pair("Chess\\Chess.exe", "中国象棋"));
	vGame.push_back (make_pair("FreeCell\\FreeCell.exe", "空当接龙"));
	vGame.push_back (make_pair("Hearts\\Hearts.exe", "红心大战"));
	vGame.push_back (make_pair("Mahjong\\Mahjong.exe", "麻将高手"));
	vGame.push_back (make_pair("Minesweeper\\MineSweeper.exe", "扫雷"));
	vGame.push_back (make_pair("Multiplayer\\Backgammon\bckgzm.exe", "双陆棋"));
	vGame.push_back (make_pair("Multiplayer\\Checkers\chkrzm.exe", " 跳棋"));
	vGame.push_back (make_pair("Multiplayer\\Spades\shvlzm.exe", "黑桃王"));
	vGame.push_back (make_pair("Purble Place\\PurblePlace.exe", "厨房大赛"));
	vGame.push_back (make_pair("Solitaire\\Solitaire.exe", "纸牌"));
	vGame.push_back (make_pair("SpiderSolitaire\\SpiderSolitaire.exe", "蜘蛛纸牌"));

	char szDir[MAX_PATH] = {0};
	SHGetSpecialFolderPath (NULL, szDir, CSIDL_PROGRAM_FILES, FALSE);

	for (vector<pair<string, string> >::iterator it = vGame.begin (); it != vGame.end (); it++)
	{
		string strGamePath;
		YL_StringUtil::Format (strGamePath, "%s\\%s", szDir, (*it).first.c_str ());

		if (YL_FileInfo::IsValid (strGamePath))
		{
			CString strMD5 = CMD5Checksum::GetMD5 (strGamePath.c_str ());

			SHFILEINFO stFileInfo;
			SHGetFileInfo(strGamePath.c_str (), 0, &stFileInfo,sizeof(stFileInfo), SHGFI_ICON | SHGFI_LARGEICON );
			if (stFileInfo.hIcon != NULL)
			{
				string strFilePath = strGamePath;
				strFilePath = strFilePath.substr (0, strFilePath.rfind ("."));
				string strIconPath;
				YL_StringUtil::Format (strIconPath, "%s.ico", strFilePath.c_str ());
				SaveIcon (stFileInfo.hIcon, strIconPath.c_str ());
				DestroyIcon (stFileInfo.hIcon);

				if (GLOBAL_LOCALGAME->ILocalGameData_AddGame (strMD5.GetBuffer (), (*it).second,
					strIconPath, strGamePath.c_str (), "", 1, strMD5.GetBuffer ()))
				{
					g_dwSearchGameCnt++;
				}
			}
		}
	}
}

DWORD WINAPI SortInfoWnd::FindGame (ThreadParameter *lpParameter)
{
	g_dwSearchGameCnt = 0;
	g_bStopSearch = FALSE;

	GetFL (g_flString);
	InitFL2 (g_flString2);

#ifdef _DEBUG
	string strOutput;
	OutputDebugString ("Filter dir is :\n");
	for (vector<string>::iterator it = g_flString.begin (); it != g_flString.end (); it++)
	{
		YL_StringUtil::Format (strOutput, "%s\n", (*it).c_str ());
		OutputDebugString (strOutput.c_str ());
	}
	OutputDebugString ("\n");
#endif

	SortInfoWnd * pSortInfoWnd = (SortInfoWnd *)(lpParameter->lpThis);

	if (lpParameter->strPath == "")
	{
		pSortInfoWnd->DrawScanningText ("正在为您搜索本地游戏...");

		GetWindowsGameXP ();
		GetWindowsGameWin7 ();

		char buf[MAX_PATH]; 
		DWORD len = GetLogicalDriveStrings(sizeof(buf)/sizeof(TCHAR),buf); 
		string strDisks;
		for (char * s = buf; *s && !g_bStopSearch; s += strlen (s) + 1) 
		{ 
			char *sDrivePath = s;
			strDisks = string(sDrivePath);

			UINT uType = GetDriveType (strDisks.c_str ());
			if (uType == DRIVE_UNKNOWN ||
				uType == DRIVE_NO_ROOT_DIR || uType == DRIVE_CDROM)
				continue;

			FindGameR (lpParameter->lpThis, strDisks);
		} 
	}
	else
	{
		char szWindows[MAX_PATH] = {0};
		SHGetSpecialFolderPath (NULL, szWindows, CSIDL_WINDOWS, FALSE);

		char szSystem[MAX_PATH] = {0};
		SHGetSpecialFolderPath (NULL, szSystem, CSIDL_SYSTEM, FALSE);

		if (lpParameter->strPath == szWindows ||
			lpParameter->strPath == szSystem)
			GetWindowsGameXP ();

		FindGameR (lpParameter->lpThis, lpParameter->strPath);
	}

	string strResult;
	YL_StringUtil::Format (strResult, "共搜索到%d个游戏", g_dwSearchGameCnt);

	pSortInfoWnd->DrawScanningText (strResult);
	pSortInfoWnd->PostMessage (WM_COMMAND, MAKEWPARAM (ID_BTN_CANCELADDGAME, BN_CLICKED), 0);

	SAFE_DELETE (lpParameter);

	return 1;
}

SortInfoWnd::SortInfoWnd ()
{
	m_txtColorHilight = RGB(228,108,10);
	m_txtColorOrig    = RGB(255, 255, 255);
	m_oldFont = NULL;
	m_bAutoAddLocalGame = FALSE;
	m_bScanning = FALSE;
	m_strScanningText = "";

	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_APPEXIT, (IAppExitObserver*)this );
}

SortInfoWnd::~SortInfoWnd()
{
	SAFE_DELETE (m_pbtnSortByTime);
	SAFE_DELETE (m_pbtnSortByType);
	SAFE_DELETE (m_pbtnSortByTimes);
	SAFE_DELETE (m_pbtnAutoAddLocalGame);
	SAFE_DELETE (m_pbtnManualAddLocalGame);
	SAFE_DELETE (m_pbtnAddGameTriangle);
	SAFE_DELETE (m_pbtnCancelAddGame);

	std::vector<GameVersion*>::iterator it = m_vGameVersion.begin ();
	for (; it != m_vGameVersion.end (); it++)
	{
		if (*it != NULL)
		{
			delete *it;
			*it = NULL;
		}
	}

	m_vGameVersion.clear ();

	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_APPEXIT, (IAppExitObserver*)this );
}

BEGIN_MESSAGE_MAP(SortInfoWnd, CWnd)
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_COMMAND (ID_MENU_MANUALADDLOCALGAME, OnManualScan)
	ON_COMMAND (ID_MENU_AUTOADDLOCALGAME, OnAutoScan)
END_MESSAGE_MAP()

void SortInfoWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);
	if (bShow) 
	{
		int nSortType = 0;
		AfxGetUserConfig()->GetConfigIntValue (
			CONF_SETTING_MODULE_NAME,CONF_SETTING_PLAYED_GAME_SORT_TYPE, nSortType);

		switch (nSortType)
		{
		case 0:
			m_pbtnSortByTime->SetCheck (BST_CHECKED);
			m_pbtnSortByType->SetCheck (BST_UNCHECKED);
			m_pbtnSortByTimes->SetCheck (BST_UNCHECKED);
			Invalidate ();
			break;
		case 1:
			m_pbtnSortByTime->SetCheck (BST_UNCHECKED);
			m_pbtnSortByType->SetCheck (BST_CHECKED);
			m_pbtnSortByTimes->SetCheck (BST_UNCHECKED);
			Invalidate ();
			break;
		case 2:
			m_pbtnSortByTime->SetCheck (BST_UNCHECKED);
			m_pbtnSortByType->SetCheck (BST_UNCHECKED);
			m_pbtnSortByTimes->SetCheck (BST_CHECKED);
			Invalidate ();
			break;
		}
	}
}

BOOL SortInfoWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD lowWord	= LOWORD( wParam );
	WORD hiWord		= HIWORD( wParam );

	if (hiWord == BN_CLICKED)
	{
		switch( lowWord )
		{
		case ID_BTN_SORTTIME:
			m_playedWnd->UpdateSortType ((SortType) 0);
			m_pbtnSortByTime->SetCheck (BST_CHECKED);
			m_pbtnSortByType->SetCheck (BST_UNCHECKED);
			m_pbtnSortByTimes->SetCheck (BST_UNCHECKED);
// 			Invalidate ();
			break;
		case ID_BTN_SORTTYPE:
			m_playedWnd->UpdateSortType ((SortType) 1);
			m_pbtnSortByTime->SetCheck (BST_UNCHECKED);
			m_pbtnSortByType->SetCheck (BST_CHECKED);
			m_pbtnSortByTimes->SetCheck (BST_UNCHECKED);
// 			Invalidate ();
			break;
		case ID_BTN_SORTTIMES:
			m_playedWnd->UpdateSortType ((SortType) 2);
			m_pbtnSortByTime->SetCheck (BST_UNCHECKED);
			m_pbtnSortByType->SetCheck (BST_UNCHECKED);
			m_pbtnSortByTimes->SetCheck (BST_CHECKED);
// 			Invalidate ();
			break;
		case ID_BTN_ADDGAMETRIANGLE:
			ShowMenu ();
			break;
		case ID_BTN_AUTOADDLOCALGAME:
			_AutoScan ();
			break;
		case ID_BTN_MANUALADDLOCALGAME:
			_ManualScan ();
			break;
		case ID_BTN_CANCELADDGAME:
			{
				g_bStopSearch = TRUE;

				m_pbtnCancelAddGame->ShowWindow (SW_HIDE);
				m_pbtnAddGameTriangle->ShowWindow (SW_SHOW);
				if (m_bAutoAddLocalGame)
					m_pbtnAutoAddLocalGame->ShowWindow (SW_SHOW);
				else
					m_pbtnManualAddLocalGame->ShowWindow (SW_SHOW);
			}
			break;
		}
	}

	return __super::OnCommand(wParam, lParam);
}

int SortInfoWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_font.CreateFont(
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

	int nTop, nBottom;
	nTop = (lpCreateStruct->cy - 23) / 2 + 1;
	nBottom = nTop + 23;

	m_rcTotalGameCnt = CRect (20,  1, 230, lpCreateStruct->cy - 1);
	m_rcSortTitle    = CRect (250, 1, 280, lpCreateStruct->cy - 1);

	m_rcSortTime     = CRect (140, nTop, 190, nBottom);
	m_rcSortGameType = CRect (205, nTop, 255, nBottom);
	m_rcSortPlayTimes= CRect (270, nTop, 350, nBottom);

	m_pbtnSortByTime = new CxSkinButton ();
	m_pbtnSortByTimes= new CxSkinButton ();
	m_pbtnSortByType = new CxSkinButton ();

	m_pbtnAutoAddLocalGame = new CxSkinButton ();
	m_pbtnManualAddLocalGame = new CxSkinButton ();
	m_pbtnAddGameTriangle = new CxSkinButton ();
	m_pbtnCancelAddGame = new CxSkinButton ();

	m_pbtnSortByTime->Create (NULL, WS_VISIBLE | WS_CHILD, m_rcSortTime, this, ID_BTN_SORTTIME);
	m_pbtnSortByType->Create (NULL, WS_VISIBLE | WS_CHILD, m_rcSortGameType, this, ID_BTN_SORTTYPE);
	m_pbtnSortByTimes->Create (NULL, WS_VISIBLE | WS_CHILD,m_rcSortPlayTimes, this, ID_BTN_SORTTIMES);

	CRect rcNull (0, 0, 0, 0);
	m_pbtnAutoAddLocalGame->Create (NULL, WS_VISIBLE | WS_CHILD, rcNull, this, ID_BTN_AUTOADDLOCALGAME);
	m_pbtnManualAddLocalGame->Create (NULL, WS_VISIBLE | WS_CHILD, rcNull, this, ID_BTN_MANUALADDLOCALGAME);
	m_pbtnAddGameTriangle->Create (NULL, WS_VISIBLE | WS_CHILD, rcNull, this, ID_BTN_ADDGAMETRIANGLE);
	m_pbtnCancelAddGame->Create (NULL, WS_VISIBLE | WS_CHILD, rcNull, this, ID_BTN_CANCELADDGAME);

	ISkinMgr * pSkinMgr = AfxGetUIManager ()->UIGetSkinMgr ();

	m_pbtnAutoAddLocalGame->SetSkin (
		pSkinMgr->GetDibBmp ("AutoAddNormal"),
		pSkinMgr->GetDibBmp ("AutoAddDown"),
		pSkinMgr->GetDibBmp ("AutoAddHover"), NULL, NULL, NULL, 1, 0, 0);

	m_pbtnManualAddLocalGame->SetSkin (
		pSkinMgr->GetDibBmp ("ManualAddNormal"),
		pSkinMgr->GetDibBmp ("ManualAddDown"),
		pSkinMgr->GetDibBmp ("ManualAddHover"), NULL, NULL, NULL, 1, 0, 0);

	m_pbtnAddGameTriangle->SetSkin (
		pSkinMgr->GetDibBmp ("AddGameTriangleNormal"),
		pSkinMgr->GetDibBmp ("AddGameTriangleDown"),
		pSkinMgr->GetDibBmp ("AddGameTriangleHover"), NULL, NULL, NULL, 1, 0, 0);

	m_pbtnCancelAddGame->SetSkin (
		pSkinMgr->GetDibBmp ("CancelAddNormal"),
		pSkinMgr->GetDibBmp ("CancelAddDown"),
		pSkinMgr->GetDibBmp ("CancelAddHover"), NULL, NULL, NULL, 1, 0, 0);

	m_pbtnSortByTime->SetWindowText ("时间");
	m_pbtnSortByType->SetWindowText ("类型");
	m_pbtnSortByTimes->SetWindowText ("运行次数");

	COLORREF rgbWhite = RGB (255, 255, 255);

	m_pbtnSortByTime->SetNormalTextColor (rgbWhite);
	m_pbtnSortByTime->SetDownTextColor (rgbWhite);
	m_pbtnSortByTime->SetOverTextColor (rgbWhite);

	m_pbtnSortByType->SetNormalTextColor (rgbWhite);
	m_pbtnSortByType->SetDownTextColor (rgbWhite);
	m_pbtnSortByType->SetOverTextColor (rgbWhite);
	
	m_pbtnSortByTimes->SetNormalTextColor (rgbWhite);
	m_pbtnSortByTimes->SetDownTextColor (rgbWhite);
	m_pbtnSortByTimes->SetOverTextColor (rgbWhite);

	m_pbtnSortByTime->SetSkin (
		pSkinMgr->GetDibBmp ("SortByTimeNormal"), 
		pSkinMgr->GetDibBmp ("SortByTimeDown"),
		pSkinMgr->GetDibBmp ("SortByTimeHover"), NULL, NULL, NULL, 1, 0, 0);

	m_pbtnSortByType->SetSkin (
		pSkinMgr->GetDibBmp ("SortByTypeNormal"), 
		pSkinMgr->GetDibBmp ("SortByTypeDown"),
		pSkinMgr->GetDibBmp ("SortByTypeHover"), NULL, NULL, NULL, 1, 0, 0);

	m_pbtnSortByTimes->SetSkin (
		pSkinMgr->GetDibBmp ("SortByTimesNormal"), 
		pSkinMgr->GetDibBmp ("SortByTimesDown"),
		pSkinMgr->GetDibBmp ("SortByTimesHover"), NULL, NULL, NULL, 1, 0, 0);

	m_pBack = pSkinMgr->GetDibBmp ("PlayedGameSortTitleBar");

	InitGameVersion ();

	bool bIsSan = FALSE;
	AfxGetUserConfig ()->GetConfigBoolValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_IS_SCAN_LOCAL_GAME, bIsSan);
	if (bIsSan)
		OnAutoScan ();

	return 0;
}

void SortInfoWnd::OnDestroy()
{
	m_pbtnSortByTime->DestroyWindow ();
	m_pbtnSortByType->DestroyWindow ();
	m_pbtnSortByTimes->DestroyWindow ();
	m_pbtnAutoAddLocalGame->DestroyWindow ();
	m_pbtnManualAddLocalGame->DestroyWindow ();
	m_pbtnAddGameTriangle->DestroyWindow ();
	m_pbtnCancelAddGame->DestroyWindow ();
}

void SortInfoWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	DrawSortInfo (&dc);
}

BOOL SortInfoWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return CWnd::OnEraseBkgnd(pDC);
}

void SortInfoWnd::DrawSortInfo (CDC * pDC)
{
	CRect rcClient;
	GetClientRect (&rcClient);

	CDC MemDC; //首先定义一个显示设备对象
	CBitmap MemBitmap;//定义一个位图对象

	MemDC.CreateCompatibleDC(pDC);
	MemBitmap.CreateCompatibleBitmap(pDC, rcClient.Width (), rcClient.Height ());

	m_oldFont = (CFont*)MemDC.SelectObject (&m_font);
	CBitmap *pOldBit=MemDC.SelectObject(&MemBitmap);

	m_pBack->SetCDibRect (rcClient);
	m_pBack->Draw (&MemDC, FALSE);

	DrawSortTypeText (&MemDC);

	pDC->BitBlt (0, 0, rcClient.Width (), rcClient.Height (), &MemDC, 0, 0, SRCCOPY);

	MemDC.SelectObject (pOldBit);
	if( m_oldFont )
	{
		MemDC.SelectObject (m_oldFont);
		m_oldFont = NULL;
	}

	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
}

void SortInfoWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	CRect rcWindow;
	GetWindowRect (&rcWindow);
	int width = rcWindow.Width ();
	int height = rcWindow.Height ();

	int nTop, nBottom;
	nTop = (height - 20) / 2 + 1;
	nBottom = nTop + 20;

	m_rcCancelAddGame = CRect (width - 20 - 76, nTop, width - 20, nBottom);
	m_rcAddGameButton = CRect (width - 20 - 76, nTop, width - 20 - 16, nBottom);
	m_rcAddGameTriangle = CRect (width - 20 - 16, nTop, width - 20, nBottom);
	m_rcScaningText = CRect (m_rcAddGameButton.left - 200 - 20, 0, m_rcAddGameButton.left - 20, height);

	m_pbtnAutoAddLocalGame->MoveWindow (&m_rcAddGameButton);
	m_pbtnManualAddLocalGame->MoveWindow (&m_rcAddGameButton);
	m_pbtnCancelAddGame->MoveWindow (&m_rcCancelAddGame);
	m_pbtnAddGameTriangle->MoveWindow (&m_rcAddGameTriangle);

	if (m_bScanning)
	{
		m_pbtnAutoAddLocalGame->ShowWindow (SW_HIDE);
		m_pbtnManualAddLocalGame->ShowWindow (SW_HIDE);
		m_pbtnAddGameTriangle->ShowWindow (SW_HIDE);
		m_pbtnCancelAddGame->ShowWindow (SW_SHOW);
	}
	else
	{
		if (m_bAutoAddLocalGame)
		{
			m_pbtnAutoAddLocalGame->ShowWindow (SW_SHOW);
			m_pbtnAddGameTriangle->ShowWindow (SW_SHOW);
			m_pbtnManualAddLocalGame->ShowWindow (SW_HIDE);
			m_pbtnCancelAddGame->ShowWindow (SW_HIDE);
		}
		else
		{
			m_pbtnAutoAddLocalGame->ShowWindow (SW_SHOW);
			m_pbtnAddGameTriangle->ShowWindow (SW_SHOW);
			m_pbtnManualAddLocalGame->ShowWindow (SW_HIDE);
			m_pbtnCancelAddGame->ShowWindow (SW_HIDE);
		}
	}
}

void SortInfoWnd::SetPlayedGameCount (int nCnt)
{
	memset (m_strTotalGameCnt, 0, sizeof (MAX_PATH));
	if (nCnt >= 0)
		_snprintf (m_strTotalGameCnt, MAX_PATH, GAMETOTALCNT, nCnt);

	if (::IsWindowVisible (m_hWnd))
		Invalidate ();
}

void SortInfoWnd::DrawSortTypeText (CDC * pDC)
{
	int nSortType = 0;

	if (m_playedWnd != NULL)
		nSortType = m_playedWnd->GetSortType ();

	pDC->SetTextColor (m_txtColorOrig);
	pDC->SetBkMode (TRANSPARENT);

	if (strlen (m_strTotalGameCnt))
		pDC->DrawText (m_strTotalGameCnt, &m_rcTotalGameCnt, DT_SINGLELINE | DT_VCENTER);

	pDC->DrawText (m_strScanningText.c_str (), &m_rcScaningText, 
				DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_PATH_ELLIPSIS);
}

void SortInfoWnd::SetPlayedGameWnd (PlayedGameWnd * pPlayedWnd)
{
	m_playedWnd = pPlayedWnd;
}

void SortInfoWnd::ShowMenu ()
{
	CShowMenu *pMenu = new CShowMenu(this, IDR_MENU_LOCALGAME, 0);

	CPoint p (m_rcAddGameTriangle.left, m_rcAddGameTriangle.bottom);

	ClientToScreen ( &p );

	pMenu->ShowMenu( p );	
	delete pMenu;
}

void SortInfoWnd::OnAutoScan ()
{
	_AutoScan ();
}

void SortInfoWnd::OnManualScan ()
{
	_ManualScan ();
}

void SortInfoWnd::InitGameVersion ()
{
	CMarkupArchive xml;

	char szHomePath[MAX_PATH];
	CLhcImg::GetHomePath( szHomePath, MAX_PATH );

	string strGameVersionpath;
	YL_StringUtil::Format (strGameVersionpath, "%s\\res\\GameVersion.xml", szHomePath);

	if( !xml.Open(strGameVersionpath.c_str()) )
		return;

	xml.ResetPos();
	xml.FindElem();
	xml.IntoElem();

	while( xml.FindChildElem() )
	{
		string strTagName = xml.GetTagName ();
		if (strTagName != "GameVersion")
			continue;

		GameVersion * pGame = new GameVersion ();

		pGame->strGameName		= xml.GetAttrib ("GameName");
		pGame->strVersion		= xml.GetAttrib ("Version");
		pGame->strReleaseTime	= xml.GetAttrib ("ReleaseTime");
		pGame->strExecuteFile	= xml.GetAttrib ("ExecuteFile");
		pGame->strMD5Value		= xml.GetAttrib ("MD5Value");
		pGame->strSize			= xml.GetAttrib ("Size");

		m_vGameVersion.push_back (pGame);
	}

	xml.Close();
}

void SortInfoWnd::DrawScanningText (string strText)
{
	if (g_bStopSearch)
		return;

	m_strScanningText = strText;
	Invalidate ();
}

void SortInfoWnd::_ManualScan ()
{
	BROWSEINFO bi;
	ITEMIDLIST *pidl;
	char Dir [MAX_PATH] = {0};

	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = Dir;
	bi.lpszTitle = "请选择一个目录";
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
	{
		DWORD dwThreadID = 0;

		ThreadParameter * lpParameter = new ThreadParameter;
		lpParameter->lpThis = this;
		lpParameter->strPath = Dir;

		m_hFindThread = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)FindGame, lpParameter, 0, &dwThreadID);

		m_bAutoAddLocalGame = FALSE;
		m_pbtnCancelAddGame->ShowWindow (SW_SHOW);
		m_pbtnAutoAddLocalGame->ShowWindow (SW_HIDE);
		m_pbtnManualAddLocalGame->ShowWindow (SW_HIDE);
		m_pbtnAddGameTriangle->ShowWindow (SW_HIDE);

		m_bAutoAddLocalGame = FALSE;
	}
}

void SortInfoWnd::_AutoScan ()
{
	DWORD dwThreadID = 0;

	ThreadParameter * lpParameter = new ThreadParameter;
	lpParameter->lpThis = this;
	lpParameter->strPath = "";

	m_hFindThread = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)FindGame, lpParameter, 0, &dwThreadID);

	m_pbtnCancelAddGame->ShowWindow (SW_SHOW);
	m_pbtnAutoAddLocalGame->ShowWindow (SW_HIDE);
	m_pbtnManualAddLocalGame->ShowWindow (SW_HIDE);
	m_pbtnAddGameTriangle->ShowWindow (SW_HIDE);

	m_bAutoAddLocalGame = TRUE;
}

class FindByMD5
{
public:
	FindByMD5 (string strMD5) : m_strMD5 (strMD5) {}

	bool operator()(GameVersion * gameVersion)
	{
		return (gameVersion->strMD5Value == m_strMD5);
	}
private:
	string m_strMD5;
};

class FindByExecuteName
{
public:
	FindByExecuteName (string strName) : m_strName (strName) {}

	bool operator () (GameVersion * gameVersion)
	{
		return (gameVersion->strExecuteFile == m_strName);
	}

private:
	string m_strName;
};

GameVersion* SortInfoWnd::CheckMD5 (string strFilePath)
{
	string strExeName = strFilePath.substr (strFilePath.rfind ("\\") + 1);
	vector<GameVersion *>::iterator itFind = 
		std::find_if (m_vGameVersion.begin (), m_vGameVersion.end (), FindByExecuteName (strExeName));

	if (itFind != m_vGameVersion.end ())
	{
		try
		{
			version ver (strFilePath);

			string strVer1 = ver.get_product_version ();
			YL_StringUtil::ReplaceAll (strVer1, ",", ".");
			string strVer2 = (*itFind)->strVersion;

			if (strVer1 == strVer2)
			{
				CString strMD5 = CMD5Checksum::GetMD5 (strFilePath.c_str ());
				if (!StrCmpI ((*itFind)->strMD5Value.c_str (), strMD5.GetBuffer ()))
					return *itFind;
			}
		}
		catch (std::exception & e)
		{
#ifdef _DEBUG
			string strOutput;
			YL_StringUtil::Format (strOutput, "%s : %s\n", e.what (), strFilePath.c_str ());
			OutputDebugString (strOutput.c_str ());
#endif
		}
		catch (...)
		{

		}
	}

	return NULL;
}

SortInfoWnd * SortInfoWnd::GetInstance ()
{
	if (m_pThis == NULL)
	{
		m_pThis = new SortInfoWnd ();
	}

	return m_pThis;
}

void SortInfoWnd::DelInstance ()
{
	g_bStopSearch = TRUE;

	m_pThis->DestroyWindow ();

	if (m_pThis != NULL)
	{
		delete m_pThis;
		m_pThis = NULL;
	}
}

void SortInfoWnd::IAppExit ()
{
	g_bStopSearch = TRUE;
	if (WaitForSingleObject (m_hFindThread, 300) != WAIT_OBJECT_0)
	{
		TerminateThread (m_hFindThread, 0);
		CloseHandle (m_hFindThread);
		m_hFindThread = NULL;
	}
}