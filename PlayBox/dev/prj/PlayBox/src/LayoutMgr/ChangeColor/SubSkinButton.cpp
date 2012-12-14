#include "stdafx.h"
#include "SubSkinButton.h"
#include "WndColorMgr.h"
#include "../../Gui/CommonControl/SkinButton2.h"
#include "../../gui/util/ShowMenu.h"

#define		TIMER_MOUSE_IN_BTN			200

extern CWndColorMgr   gWndColor;
BEGIN_MESSAGE_MAP(CSubSkinButton,CSkinButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()	
	ON_COMMAND(ID_MENU_SKIN_APPLY,OnApplySkin)
	ON_COMMAND(ID_MENU_SKIN_DELETE,OnDeleteSkin)
	ON_COMMAND(ID_MENU_MORE_SKIN,OnMoreSkin)
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

CSubSkinButton::CSubSkinButton()
{
	//m_pPngHoverImage = NULL;
	m_pPngDeleteDown = NULL;
	m_pPngDeleteOver = NULL;
	m_pPngDeleteNormal = NULL;

	m_pPngFrame = NULL;
	m_bClickPng = 0;
	m_bShowDel = FALSE;
}

CSubSkinButton::~CSubSkinButton()
{
	//if(m_pPngHoverImage) delete m_pPngHoverImage;
	if(m_pPngDeleteDown) delete m_pPngDeleteDown;
	if(m_pPngDeleteOver) delete m_pPngDeleteOver;
	if(m_pPngDeleteNormal) delete m_pPngDeleteNormal;
	if(m_pPngFrame) delete m_pPngFrame;
}

void CSubSkinButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rc;
	GetClientRect(&rc);
	CRect delrc;

	if(m_pPngDeleteNormal)
	{
		delrc.SetRect(rc.right-m_pPngDeleteNormal->m_pBitmap->GetWidth()-5,2,rc.right+2,2+m_pPngDeleteNormal->m_pBitmap->GetHeight());
		if(PtInRect(&delrc,point)&& strcmp(m_string,"1"))//&&m_bShowDel
		{
			m_bClickPng = 1;
			LogUserActMsg(STR_USER_CLICK, string("BTN:DELETE_BTN_"+m_string));
		}
	}
	CSkinButton::OnLButtonDown(nFlags, point);
}

void CSubSkinButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct != NULL);
	CString lstrTitle = GetText();
	lstrTitle.Trim();
	m_ButtonDraw.DrawButton(this, lpDrawItemStruct,m_bottonType, mbMouseOnButton,lstrTitle, m_colBegain, m_colEnd,m_BitMap);

	Gdiplus::Graphics graphics(lpDrawItemStruct->hDC);

	if(!mbMouseOnButton && !m_bSelected)
		DrawBtnFrame(graphics);

	if(mbMouseOnButton && strcmp(m_string,"1")) //&&m_bShowDel
	{
		DrawHoverImage(graphics);
	}

	if( m_pPngImage && m_pPngImage->m_pBitmap && m_bSelected ) 
	{
		CSize lsize = m_BitMap->GetBitmapDimension();

		RECT aRect = lpDrawItemStruct->rcItem;

		int lisrcHight = aRect.bottom - aRect.top;
		int lisrcWidth = aRect.right  - aRect.left; 

		int nX = (lisrcWidth-lsize.cx)>4? (lisrcWidth-lsize.cx)/2:2;
		int nY = (lisrcHight-lsize.cy)>4? (lisrcHight-lsize.cy)/2:2;
		CRect lrcFrame(nX,nY,nX+lsize.cx, nY+lsize.cy);
		HBRUSH hHandle = 0;
		lrcFrame.InflateRect(1,1,1,1);
		hHandle = CreateSolidBrush(RGB(255,106,0));//(gWndColor.getColor(COLOL_CTROL)->structColBase->colFrame2);
		//::InflateRect(&lrcFrame,1,1);
		FrameRect(lpDrawItemStruct->hDC, &lrcFrame,(HBRUSH)hHandle );
		::InflateRect(&lrcFrame,-1,-1);
		FrameRect(lpDrawItemStruct->hDC, &lrcFrame,(HBRUSH)hHandle );
		::InflateRect(&lrcFrame,-2,-2);
		HBRUSH hbru2 = CreateSolidBrush(RGB(255,255,255));
		FrameRect(lpDrawItemStruct->hDC, &lrcFrame,hbru2 );
		DeleteObject((HGDIOBJ)hHandle);
		DeleteObject(hbru2);
	}	
	graphics.ReleaseHDC(lpDrawItemStruct->hDC);
}

BOOL CSubSkinButton::SetPngFrame(CString str)
{
	if(m_pPngImage != NULL)
		delete m_pPngFrame;
	m_pPngFrame = new CGdiPlusBitmap;

	WCHAR   wFile[MAX_PATH];   
#ifndef   UNICODE   
	MultiByteToWideChar(CP_ACP,   0,   LPCTSTR(str),   -1,   wFile,   MAX_PATH);   
#else   
	lstrcpy(wFile,   szFile);   
#endif   
	bool bReturn = m_pPngFrame->Load( wFile );
	if( bReturn ) this->Invalidate();

	return bReturn;
}

CSkinButton::CGdiPlusBitmap * CSubSkinButton::CreatePngBitmap(CString str)
{
	WCHAR   wFile[MAX_PATH];  
	CGdiPlusBitmap *pPng = new CGdiPlusBitmap;
#ifndef   UNICODE   
	MultiByteToWideChar(CP_ACP,   0,   LPCTSTR(str),   -1,   wFile,   MAX_PATH);   
#else   
	lstrcpy(wFile,   szFile);   
#endif   
	bool bReturn = pPng->Load( wFile );
	if( bReturn ) 
	{
		Invalidate();
		return pPng;
	}else
	{
		delete pPng;
		return NULL;
	}
}

BOOL CSubSkinButton::SetPngHoverSkin(CString str)
{
	if( m_pPngDeleteNormal != NULL )
		delete m_pPngDeleteNormal;
	if( m_pPngDeleteOver != NULL )
		delete m_pPngDeleteOver;
	if( m_pPngDeleteDown != NULL )
		delete m_pPngDeleteDown;

	CString path = str.Mid(0, str.GetLength() - 4) + "Normal.png";
	m_pPngDeleteNormal = CreatePngBitmap(path);

	path = str.Mid(0, str.GetLength() - 4) + "Over.png";
	m_pPngDeleteOver = CreatePngBitmap(path);

	path = str.Mid(0, str.GetLength() - 4) + "Down.png";
	m_pPngDeleteDown = CreatePngBitmap(path);
	return true;
}

void CSubSkinButton::DrawBtnFrame(Gdiplus::Graphics& gdi)
{
	if( m_pPngFrame && m_pPngFrame->m_pBitmap )
	{
		CRect rect;
		this->GetClientRect(&rect);
		int w = m_pPngFrame->m_pBitmap->GetWidth();
		int h = m_pPngFrame->m_pBitmap->GetHeight();
		RectF grect;
		int nX = ((rect.Width()-w)>4? (rect.Width()-w)/2:2);
		int nY = 0;//(rect.Height()-h)>4? (rect.Height()-h)/2:2;

		Rect rr(nX, nY, w,h+nY-1);
		gdi.DrawImage(*m_pPngFrame,
			rr,
			(int)0, 
			(int)1,
			(int)w, 
			(int)h-1,
			UnitPixel );
	}
}

BOOL CSubSkinButton::IsMouseInDeleteButton()
{
	CPoint	lPoint;
	CRect	rect;

	if(!m_pPngDeleteNormal)
		return false;

	GetCursorPos(&lPoint);
	GetClientRect(&rect);

	ScreenToClient(&lPoint);

	CRect rc;
	rc.left = rect.right - m_pPngDeleteNormal->m_pBitmap->GetWidth()-5;
	rc.top = 2;
	rc.right = rc.left + m_pPngDeleteNormal->m_pBitmap->GetWidth();
	rc.bottom = 2 + m_pPngDeleteNormal->m_pBitmap->GetHeight();

	if(rc.PtInRect(lPoint))
		return true;
	return false;
}
void CSubSkinButton::DrawHoverImage(Gdiplus::Graphics& gdi)
{
	CRect rect;
	GetClientRect(&rect);
	if(IsMouseInDeleteButton())
	{
		m_pCurDelete = m_pPngDeleteOver;
	}
	else 
	{
		m_pCurDelete = m_pPngDeleteNormal;
	}

	if( m_pCurDelete && m_pCurDelete->m_pBitmap )
	{

		RectF grect;
		grect.X=rect.right - m_pCurDelete->m_pBitmap->GetWidth()-5, grect.Y=2; 
		grect.Width = m_pCurDelete->m_pBitmap->GetWidth(); 
		grect.Height = m_pCurDelete->m_pBitmap->GetHeight();
		gdi.DrawImage(*m_pCurDelete,grect );
	}
}

BOOL CSubSkinButton::IsClickPng()
{
	return m_bClickPng;
}

 void CSubSkinButton::SetClickPng(BOOL bclick)
 {
	m_bClickPng = bclick;	 
 }

 void CSubSkinButton::OnMouseMove(UINT nFlags, CPoint point)
 {
// 	 if(!m_bShowDel)
// 		 SetTimer(TIMER_MOUSE_IN_BTN,400,NULL);

	 if(IsMouseInDeleteButton() && m_strToolTip != "É¾³ý")
	 {
		 m_strLastToolTip = m_strToolTip;
		 SetToolTipText("É¾³ý");
	 }
	 else if(!IsMouseInDeleteButton() && m_strToolTip == "É¾³ý")
	 {
		 SetToolTipText(m_strLastToolTip);
	 }
	 if(IsMouseInDeleteButton() && m_pCurDelete != m_pPngDeleteOver)
	 {


		 m_pCurDelete = m_pPngDeleteOver;
		 Invalidate();
	 }
	 if(!IsMouseInDeleteButton() && m_pCurDelete != m_pPngDeleteNormal)
	 {
		 m_pCurDelete = m_pPngDeleteNormal;
		Invalidate();
	 }
	 CSkinButton::OnMouseMove(nFlags, point);
 }

 void CSubSkinButton::OnTimer(UINT aiIDEvent)
 {
	 if(aiIDEvent == TIMER_MOUSE_IN_BTN)
	 {
		 m_bShowDel = TRUE;
		 Invalidate();
	 }
 }

 LRESULT CSubSkinButton::OnMouseLeave(WPARAM   wParam,   LPARAM   lParam)
 {
	 KillTimer(TIMER_MOUSE_IN_BTN);
	 m_bShowDel = FALSE;
	 return (CSkinButton::OnMouseLeave(wParam,lParam));
 }


 void CSubSkinButton::OnRButtonDown(UINT nFlags, CPoint point)
 {
	CSkinButton::OnRButtonDown(nFlags, point);
 }

 void CSubSkinButton::OnApplySkin()
 {
	 CSkinButton::OnLButtonDown(0, 0);
 }

 void CSubSkinButton::OnDeleteSkin()
 {
	 LogUserActMsg(STR_USER_CLICK, string("BTN:DELETE_MENU_"+m_string));
	 m_bClickPng = 1;
	 CSkinButton::OnLButtonDown(0, 0);
 }

 void CSubSkinButton::OnMoreSkin()
 {
	 LogUserActMsg(STR_USER_CLICK, "BTN:DOWN_MORE_MENU");
	 ShellExecute(NULL, "open", _T("iexplore"), "http://www.kuwo.cn/skin/", NULL, SW_SHOWNORMAL);
 }
