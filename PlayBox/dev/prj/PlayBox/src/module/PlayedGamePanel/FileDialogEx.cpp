// FileDialogEx.cpp : 实现文件
//

#include "stdafx.h"
#include "FileDialogEx.h"

#define _WIN32_WINNT 0x0501 

// CFileDialogEx

CString strPath;

UINT_PTR CALLBACK ComDlgPreviewProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
		{
			int iImgWH=96;
			PAINTSTRUCT ps;
			BeginPaint(hDlg,&ps);
			HDC dc = ::GetDC(hDlg);
			RECT rcClient;
			::GetClientRect(hDlg,&rcClient);
			
			if (strPath=="")
			{
				HBRUSH brbk=::CreateSolidBrush(::GetPixel(dc,rcClient.left+10,rcClient.bottom-10));
				RECT rcImg;
				rcImg.left = (rcClient.right-iImgWH)/2;
				rcImg.top = rcClient.bottom-115;
				rcImg.right = rcImg.left+iImgWH;
				rcImg.bottom = rcImg.top+iImgWH;
				::FillRect(dc,&rcImg,brbk);
			}
			else
			{
				static Image* pImageFrame=NULL;
				static Image* pImage=NULL;
				if (!pImageFrame)
				{
					TCHAR path_tmp[MAX_PATH]={0};
					::GetModuleFileName(NULL,path_tmp,MAX_PATH);
					CString strFrameImg(path_tmp);
					strFrameImg=strFrameImg.Mid(0,strFrameImg.ReverseFind('\\'));
					strFrameImg+="\\skin\\Common\\MLCoverFrame.png";
					USES_CONVERSION;
					Image imgFrame(A2W(strFrameImg) );
					pImageFrame=imgFrame.Clone();
				}
				USES_CONVERSION;
				Image img(A2W(strPath.GetBuffer()));
				pImage=img.Clone();
				Graphics gc(dc);
				Rect rcImgDesRect((rcClient.right-iImgWH)/2,rcClient.bottom-115,iImgWH,iImgWH);
				gc.DrawImage(pImage, rcImgDesRect);
				//gc.DrawImage(pImageFrame, rcImgDesRect);
			}
			
			EndPaint(hDlg,&ps);
		}
		break;
	case WM_NOTIFY:
		{
			LPOFNOTIFY pofn = (LPOFNOTIFY)lParam;
			if (pofn->hdr.code == CDN_SELCHANGE)
			{
				TCHAR szFile[MAX_PATH];
				if (CommDlg_OpenSave_GetFilePath(GetParent(hDlg), szFile, sizeof(szFile)) <= sizeof(szFile))
				{
					WIN32_FIND_DATA   wfd;
					HANDLE hFind = FindFirstFile(szFile, &wfd);
					if ((hFind != INVALID_HANDLE_VALUE) && (!(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)))
					{
						strPath=szFile;
					}
					else
					{
						strPath=_T("");
					}
					FindClose(hFind);
				}
			}
			SendMessage(hDlg,WM_PAINT,0,0);
		}
		break;
	default:
		return FALSE;
 	}
	return TRUE;
}


IMPLEMENT_DYNAMIC(CFileDialogEx, CFileDialog)

CFileDialogEx::CFileDialogEx(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	m_ofn.Flags |= OFN_EXPLORER|OFN_ENABLEHOOK|OFN_ENABLETEMPLATE|OFN_HIDEREADONLY;
	m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_DLG_SHOWIMG);
	m_ofn.hInstance = AfxGetInstanceHandle();
	m_ofn.lpfnHook = (LPOFNHOOKPROC)ComDlgPreviewProc;    
}

CFileDialogEx::~CFileDialogEx()
{
}


BEGIN_MESSAGE_MAP(CFileDialogEx, CFileDialog)
	ON_MESSAGE(WM_USER+300,OnSetImgPath)
	ON_WM_PAINT()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()


INT_PTR CFileDialogEx::DoModal()
{
	ASSERT_VALID(this);
	ASSERT(m_ofn.Flags & OFN_ENABLEHOOK);
	ASSERT(m_ofn.lpfnHook != NULL); // can still be a user hook

	// zero out the file buffer for consistent parsing later
	ASSERT(AfxIsValidAddress(m_ofn.lpstrFile, m_ofn.nMaxFile));
	DWORD nOffset = lstrlen(m_ofn.lpstrFile)+1;
	ASSERT(nOffset <= m_ofn.nMaxFile);
	memset(m_ofn.lpstrFile+nOffset, 0, (m_ofn.nMaxFile-nOffset)*sizeof(TCHAR));

	// WINBUG: This is a special case for the file open/save dialog,
	//  which sometimes pumps while it is coming up but before it has
	//  disabled the main window.
	HWND hWndFocus = ::GetFocus();
	BOOL bEnableParent = FALSE;
	m_ofn.hwndOwner = PreModal();
	AfxUnhookWindowCreate();
	if (m_ofn.hwndOwner != NULL && ::IsWindowEnabled(m_ofn.hwndOwner))
	{
		bEnableParent = TRUE;
		::EnableWindow(m_ofn.hwndOwner, FALSE);
	}

	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	ASSERT(pThreadState->m_pAlternateWndInit == NULL);

	if (m_ofn.Flags & OFN_EXPLORER)
		pThreadState->m_pAlternateWndInit = this;
	else
		AfxHookWindowCreate(this);

	INT_PTR nResult;
	if (m_bOpenFileDialog)
		nResult = ::GetOpenFileName(&m_ofn);
	else
		nResult = ::GetSaveFileName(&m_ofn);

	//if (nResult)
	//	ASSERT(pThreadState->m_pAlternateWndInit == NULL);

	pThreadState->m_pAlternateWndInit = NULL;

	// WINBUG: Second part of special case for file open/save dialog.
	if (bEnableParent)
		::EnableWindow(m_ofn.hwndOwner, TRUE);
	if (::IsWindow(hWndFocus))
		::SetFocus(hWndFocus);

	PostModal();
	return nResult ? nResult : IDCANCEL;
}

LRESULT CFileDialogEx::OnSetImgPath(WPARAM wParam, LPARAM lParam)
{
	m_ImgstrPath = (TCHAR*)wParam;
	return 0;
}

// CFileDialogEx 消息处理程序



void CFileDialogEx::OnPaint()
{
	CPaintDC dc(this);
}

void CFileDialogEx::OnNcPaint()
{
	int a=0;
}
