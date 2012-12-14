#include "stdafx.h"
#include "irregularBorderMgr.h"
#include "translucentdialog.h"
#include "resource.h"
#include "../common/YL_FileInfo.h"
#include "../common/YL_DirInfo.h"
#include "../LayoutMgr/ChangeColor/IChangeColorMgr.h"

CIrregularBorderMgr* CIrregularBorderMgr::m_pMgr = NULL;

CIrregularBorderMgr* CIrregularBorderMgr::GetInstance()
{
	if(CIrregularBorderMgr::m_pMgr == NULL)
		CIrregularBorderMgr::m_pMgr = new CIrregularBorderMgr();
	
	return CIrregularBorderMgr::m_pMgr;
}

void CIrregularBorderMgr::DelInstance(void)
{
	delete CIrregularBorderMgr::m_pMgr;
}

CIrregularBorderMgr::CIrregularBorderMgr()
{
	m_pMainWnd = AfxGetMainWindow();
	m_pBottomLeftWnd = new CTranslucentDialog(IDD_TRANS_DLG, "");
	m_pBottomRightWnd = new CTranslucentDialog(IDD_TRANS_DLG, "");
	m_pTopRightWnd = new CTranslucentDialog(IDD_TRANS_DLG, "");
	m_pTopLeftWnd = new CTranslucentDialog(IDD_TRANS_DLG, "");

	m_pTopLeftWnd->m_position = LEFT_TOP_CORNER;
	m_pTopRightWnd->m_position = RIGHT_TOP_CORNER;
	m_pBottomLeftWnd->m_position = LEFT_BOTTOM_CORNER;
	m_pBottomRightWnd->m_position = RIGHT_BOTTOM_CORNER;

	m_pBottomLeftWnd->Create(IDD_TRANS_DLG,::AfxGetMainWindow());
	m_pBottomRightWnd->Create(IDD_TRANS_DLG,::AfxGetMainWindow());
	m_pTopRightWnd->Create(IDD_TRANS_DLG,m_pBottomRightWnd);
	m_pTopLeftWnd->Create(IDD_TRANS_DLG,m_pBottomRightWnd);

	CWnd * pWnd = AfxGetMainWindow();
	m_pTopRightWnd->SetOwner(AfxGetMainWindow());
	m_curSubject = NULL;

	m_bUpdateView = false;
}

CIrregularBorderMgr::~CIrregularBorderMgr()
{
	delete m_pBottomLeftWnd;
	delete m_pBottomRightWnd;
	delete m_pTopRightWnd;
	delete m_pTopLeftWnd;
}

void CIrregularBorderMgr::LoadSubject()
{
}

void CIrregularBorderMgr::DeletePngImage()
{
	m_pBottomLeftWnd->DeleteImage();
	m_pBottomRightWnd->DeleteImage();
	m_pTopLeftWnd->DeleteImage();
	m_pTopRightWnd->DeleteImage();
}

void CIrregularBorderMgr::ShowTransDlg(bool bShow)
{
	int nShow = bShow? SW_SHOWNOACTIVATE: SW_HIDE;

	m_pTopLeftWnd->ShowWindow(m_pTopLeftWnd->m_pImage ? nShow: SW_HIDE);
	m_pTopRightWnd->ShowWindow(m_pTopRightWnd->m_pImage ? nShow: SW_HIDE);
	m_pBottomRightWnd->ShowWindow(m_pBottomRightWnd->m_pImage ? nShow: SW_HIDE);
	m_pBottomLeftWnd->ShowWindow(m_pBottomLeftWnd->m_pImage ? nShow: SW_HIDE);
}

void CIrregularBorderMgr::SetMainWnd(CWnd * pWnd)
{
}

Image * CloneImage(string path)
{
	WCHAR szWidePath[2048];   
	long lLen = MultiByteToWideChar(CP_ACP,0,path.c_str(),path.length(),szWidePath,sizeof(szWidePath)); 
	szWidePath[lLen] = '\0';     

	Image image(szWidePath);
	if(image.GetLastStatus() != S_OK)
	{
		return NULL;
	}
	int w = image.GetWidth();
	int h = image.GetHeight(); 
	TRACE("w: %d ,h: %d, %s\r\n", w,h,path.c_str());
	return image.Clone();
}

void CIrregularBorderMgr::InitTransDlg(BOOL bMustInit)
{
	ISkinMgr *pSkin = AfxGetUIManager()->UIGetSkinMgr();
	OneSubject *pCurSubject = pSkin->GetCurSubject();

	if(pCurSubject != m_curSubject || bMustInit)
	{
		LogUserActMsg(STR_USER_CLICK, "Enter InitTransDlg");
		m_curSubject = pCurSubject;

		string path = pSkin->GetSkinPath();

		DeletePngImage();
		if(m_curSubject == NULL)
		{
			m_pTopLeftWnd->ShowWindow(SW_HIDE);
			m_pTopRightWnd->ShowWindow(SW_HIDE);
			m_pBottomRightWnd->ShowWindow(SW_HIDE);
			m_pBottomLeftWnd->ShowWindow(SW_HIDE);
			return;
		}
		string pngpath = path + "\\sbujectskin\\" + m_curSubject->strSubName + "\\topleft.png";
		LogUserActMsg(STR_USER_CLICK, pngpath.c_str());
		if( YL_FileInfo::IsValid(pngpath) && pCurSubject->ptLeftTop.x != -1000)
		{
			m_pTopLeftWnd->ShowWindow(SW_SHOWNOACTIVATE);
			m_pTopLeftWnd->m_pImage = CloneImage(pngpath.c_str());
			CString strLog;
			strLog.Format("cloneTopLeft: %d ,h: %d, %s\r\n", m_pTopLeftWnd->m_pImage->GetWidth(),m_pTopLeftWnd->m_pImage->GetHeight(),path.c_str());
			LogUserActMsg(STR_USER_CLICK, string(strLog));
			m_pTopLeftWnd->UpdatePosition( pCurSubject->ptLeftTop);
		}else
		{
			m_pTopLeftWnd->ShowWindow(SW_HIDE);
			LogUserActMsg(STR_USER_CLICK, "TopLeft hide");
		}

		pngpath = path + "\\sbujectskin\\" + m_curSubject->strSubName + "\\topright.png";
		LogUserActMsg(STR_USER_CLICK, pngpath.c_str());
		if( YL_FileInfo::IsValid(pngpath)&& pCurSubject->ptRightTop.x != -1000)
		{
			m_pTopRightWnd->ShowWindow(SW_SHOWNOACTIVATE);
			m_pTopRightWnd->m_pImage = CloneImage(pngpath.c_str());

			CString strLog;
			strLog.Format("cloneTopRight: %d ,h: %d, %s\r\n", m_pTopRightWnd->m_pImage->GetWidth(),m_pTopRightWnd->m_pImage->GetHeight(),path.c_str());
			LogUserActMsg(STR_USER_CLICK, string(strLog));

			m_pTopRightWnd->UpdatePosition(pCurSubject->ptRightTop);
		}
		else
		{
			m_pTopRightWnd->ShowWindow(SW_HIDE);
			LogUserActMsg(STR_USER_CLICK, "TopRight Hide");
		}

		pngpath = path + "\\sbujectskin\\" + m_curSubject->strSubName + "\\bottomright.png";
		if( YL_FileInfo::IsValid(pngpath)&& pCurSubject->ptRightBottom.x != -1000)
		{
			m_pBottomRightWnd->ShowWindow(SW_SHOWNOACTIVATE);
			m_pBottomRightWnd->m_pImage = CloneImage(pngpath.c_str());
			m_pBottomRightWnd->UpdatePosition(pCurSubject->ptRightBottom);
		}
		else
		{
			m_pBottomRightWnd->ShowWindow(SW_HIDE);
		}

		pngpath = path + "\\sbujectskin\\" + m_curSubject->strSubName + "\\bottomleft.png";
		if( YL_FileInfo::IsValid(pngpath)&& pCurSubject->ptLeftBottom.x != -1000)
		{
			m_pBottomLeftWnd->ShowWindow(SW_SHOWNOACTIVATE);
			m_pBottomLeftWnd->m_pImage = CloneImage(pngpath.c_str());
			m_pBottomLeftWnd->UpdatePosition(pCurSubject->ptLeftBottom);
		}
		else
		{
			m_pBottomLeftWnd->ShowWindow(SW_HIDE);
		}

		m_bUpdateView = true;
		OnMove(0,0);
		m_bUpdateView = false;
	}


}
void CIrregularBorderMgr::DrawToppanelPng(CDC *pDC, CRect rc)
{
	InitTransDlg();
	if(m_curSubject == NULL)
		return ;

	Gdiplus::Image* pImgRight = m_pTopRightWnd->m_pImage;
	Gdiplus::Image* pImgLeft = m_pTopLeftWnd->m_pImage;

	if(pImgRight == NULL && pImgLeft == NULL)
		return;

	Gdiplus::Graphics graph(pDC->m_hDC);
	graph.SetSmoothingMode(Gdiplus::SmoothingModeNone);

	if(pImgLeft)
	{
		Rect rr(0,0, pImgLeft->GetWidth() - (m_curSubject->ptLeftTop.x + 2), pImgLeft->GetHeight() - (m_curSubject->ptLeftTop.y + 2));
		graph.DrawImage(pImgLeft,
			rr,
			(int)m_curSubject->ptLeftTop.x + 2, 
			(int)m_curSubject->ptLeftTop.y + 2,
			(int)pImgLeft->GetWidth() - (m_curSubject->ptLeftTop.x + 2), 
			(int)pImgLeft->GetHeight() - (m_curSubject->ptLeftTop.y + 2),
			UnitPixel );
	}
	if(pImgRight)
	{
		int h = pImgRight->GetHeight();
		Rect rr(rc.right - m_curSubject->ptRightTop.x + 2,0,m_curSubject->ptRightTop.x - 2, pImgRight->GetHeight() - m_curSubject->ptRightTop.y - 2);
		graph.DrawImage(pImgRight, 
			rr,
			(int)0, 
			(int)m_curSubject->ptRightTop.y + 2,
			(int)m_curSubject->ptRightTop.x - 2, 
			(int)pImgRight->GetHeight() - m_curSubject->ptRightTop.y - 2,
			UnitPixel);
	}
}

void CIrregularBorderMgr::OnMove(int x , int y)
{
	CRect rc;

	ISkinMgr *pSkin = AfxGetUIManager()->UIGetSkinMgr();
	if(pSkin == NULL)
		return;
	OneSubject *pCurSubject = pSkin->GetCurSubject();
	m_pMainWnd = AfxGetMainWindow();

	if(pCurSubject == NULL)
		return;
	m_pMainWnd->GetWindowRect(&rc);
	rc.top -= 162;
	rc.bottom = rc.top + 164;
	rc.right = rc.left + 209;

	if(m_pTopLeftWnd != NULL && IsWindowVisible(m_pTopLeftWnd->m_hWnd))
	{
		m_pMainWnd->GetWindowRect(&rc);
		rc.left = rc.left - pCurSubject->ptLeftTop.x;
		rc.top = rc.top - pCurSubject->ptLeftTop.y;
		rc.bottom = rc.top + m_pTopLeftWnd->GetImageHeight();
		rc.right = rc.left + m_pTopLeftWnd->GetImageWidth();
		m_pTopLeftWnd->MoveWindow(&rc);
		m_pTopLeftWnd->UpdateView(m_bUpdateView);
	}
	if(m_pTopRightWnd != NULL  && IsWindowVisible(m_pTopRightWnd->m_hWnd))
	{
		m_pMainWnd->GetWindowRect(&rc);
		rc.left = rc.right - pCurSubject->ptRightTop.x;
		rc.top = rc.top - pCurSubject->ptRightTop.y;
		rc.bottom = rc.top + m_pTopRightWnd->GetImageHeight();
		rc.right = rc.left + m_pTopRightWnd->GetImageWidth();
	//	m_pTopRightWnd->MoveWindow(&rc);
		CWnd *pWnd = AfxGetUIManager()->UIGetChangeColorMgr()->GetSkinWnd();
		m_pTopRightWnd->SetWindowPos(pWnd, rc.left, rc.top, rc.Width(),rc.Height(), SWP_NOACTIVATE);
		m_pTopRightWnd->UpdateView(m_bUpdateView);
	}

	if(m_pBottomLeftWnd != NULL && IsWindowVisible(m_pBottomLeftWnd->m_hWnd))
	{
		m_pMainWnd->GetWindowRect(&rc);
		rc.left = rc.left - pCurSubject->ptLeftBottom.x;
		rc.top = rc.bottom - pCurSubject->ptLeftBottom.y;
		rc.bottom = rc.top + m_pBottomLeftWnd->GetImageHeight();
		rc.right = rc.left + m_pBottomLeftWnd->GetImageWidth();
		m_pBottomLeftWnd->MoveWindow(&rc);
		m_pBottomLeftWnd->UpdateView(m_bUpdateView);
	}
	if(m_pBottomRightWnd != NULL  && IsWindowVisible(m_pBottomRightWnd->m_hWnd))
	{
		m_pMainWnd->GetWindowRect(&rc);
		rc.left = rc.right - pCurSubject->ptRightBottom.x;
		rc.top = rc.bottom - pCurSubject->ptRightBottom.y;
		rc.bottom = rc.top + m_pBottomRightWnd->GetImageHeight();
		rc.right = rc.left + m_pBottomRightWnd->GetImageWidth();
		//m_pBottomRightWnd->MoveWindow(&rc);
		CWnd *pWnd = AfxGetUIManager()->UIGetChangeColorMgr()->GetSkinWnd();
		m_pBottomRightWnd->SetWindowPos(AfxGetMainWindow(), rc.left, rc.top, rc.Width(),rc.Height(), SWP_NOACTIVATE);
		m_pBottomRightWnd->UpdateView(m_bUpdateView);

	}
	if(m_bUpdateView)
	{
		CWnd *pWnd = AfxGetUIManager()->UIGetLayoutMgr()->GetWnd("toppanel");
		if(pWnd)
			pWnd->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
	}
}

void CIrregularBorderMgr::OnSize()
{
	OnMove(0,0);
}

void CIrregularBorderMgr::LoadChangeSubjectData()
{
	ISkinMgr *pSkin = AfxGetUIManager()->UIGetSkinMgr();
	OneSubject *pCurSubject = pSkin->GetCurSubject();
}
