
#include "stdafx.h"
#include ".\tabctrlEx.h"
#include "BitMapOperate.h"
#include "WndColorMgr.h"

extern CBitMapOperate gBitMapOperate;
extern CWndColorMgr   gWndColor;

#pragma warning(disable:4311)
#pragma warning(disable:4312)

CTabCtrlEx::CTabCtrlEx()
{
}

CTabCtrlEx::~CTabCtrlEx()
{
}

BEGIN_MESSAGE_MAP(CTabCtrlEx, CTabCtrl)
	//{{AFX_MSG_MAP(CTabCtrlEx)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	//	ON_WM_LBUTTONDBLCLK()
	//ON_WM_LBUTTONDOWN()
	//ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
END_MESSAGE_MAP()

void CTabCtrlEx::SetSelTipOrient(ENUM_ORIENT aiOrient)
{
	miOrient = aiOrient;
}
void CTabCtrlEx::PreSubclassWindow() 
{	
	if(TOP == miOrient)
	{
		ModifyStyle(TCS_BOTTOM,TCS_VERTICAL);
	}
	mWndProc = (WNDPROC)SetWindowLong(this->GetSafeHwnd(), GWL_WNDPROC, (LONG)SelfProc);
	//SetItemSize(CSize(0, 23));
	CTabCtrl::PreSubclassWindow();
}

LRESULT CALLBACK CTabCtrlEx::SelfProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{ 
	LONG lReturn = 0;
	CTabCtrlEx* lpThis = NULL;
	lpThis = (CTabCtrlEx*)CWnd::FromHandle(hWnd);
	if (lpThis == NULL)
		return lReturn;

	PAINTSTRUCT ps; 
	HDC hDC; 

	switch (uMsg)
	{
	case WM_PAINT:
		{
			hDC = ::BeginPaint(hWnd, &ps);
			lpThis->DrawBorder(hWnd, hDC);
			lpThis->DrawTabs(hWnd, hDC);		
			::EndPaint(hWnd, &ps);
			return 0L;
		}
		break;
	}
	// 调用原来的回调函数
	lReturn = (LONG)CallWindowProc(lpThis->mWndProc, hWnd, uMsg, wParam, lParam);
	return lReturn;
}

BOOL CTabCtrlEx::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CTabCtrlEx::DrawBorder(HWND hWnd, HDC hDC)
{

	RECT lrcClient;
	RECT lrcButton;
	RECT lrcDlg;
	//获取窗口大小
	GetClientRect( &lrcClient);
	this->GetItemRect(0,&lrcButton);
	lrcDlg = lrcClient;
	lrcDlg.top = lrcClient.top  + lrcButton.bottom;

	if( miOrient == TOP )
	{
		gBitMapOperate.FillRectangle(hDC, lrcDlg,  gWndColor.getColor(TABLE_CTROL)->structColBase->colBK  , true );
		gBitMapOperate.DrawLine(hDC, gWndColor.getColor(TABLE_CTROL)->structColBase->colFrame1, 1,lrcDlg.left, lrcDlg.top,lrcDlg.left, lrcDlg.bottom,  true);
		gBitMapOperate.DrawLine(hDC, gWndColor.getColor(TABLE_CTROL)->structColBase->colFrame1, 1,lrcDlg.right-1, lrcDlg.top,lrcDlg.right-1, lrcDlg.bottom,  true);
		gBitMapOperate.DrawLine(hDC, gWndColor.getColor(TABLE_CTROL)->structColBase->colFrame1, 1,lrcDlg.left, lrcDlg.bottom-1,lrcDlg.right-1, lrcDlg.bottom-1,  true);

	}
	int liTabCount = GetItemCount();
	this->GetItemRect(liTabCount,&lrcButton);
	gBitMapOperate.DrawLine(hDC, gWndColor.getColor(TABLE_CTROL)->structColBase->colFrame1, 1,lrcButton.left, lrcButton.bottom-1,lrcButton.right-1, lrcButton.bottom-1,  true);

}

void CTabCtrlEx::DrawTabs(HWND hWnd, HDC hDC)
{



	//获取当前字体
	HFONT hOldFont = (HFONT)SelectObject(hDC, (HFONT)SendMessage(WM_GETFONT, 0, 0));
	TCITEM tcItem;

	CRect lrcClient;
	::GetClientRect(hWnd, lrcClient);

	RECT lrcButtonRect; // Tab的BUTTON 区域


	for( int i = 0; i < GetItemCount(); i ++ )
	{

		this->GetItemRect(i,&lrcButtonRect);
		lrcButtonRect.top -= 2;
		lrcButtonRect.left -= 2;
		lrcButtonRect.right -= 1;
	
		if( i == GetCurSel() )
		{
			CDC::FromHandle(hDC)->FrameRect( &lrcButtonRect, &CBrush( gWndColor.getColor(TABLE_CTROL)->structColBase->colFrame1) );
			InflateRect(&lrcButtonRect,-1,-1);
			gBitMapOperate.FillRectangle(hDC,lrcButtonRect,gWndColor.getColor(TABLE_CTROL)->colDown1,gWndColor.getColor(TABLE_CTROL)->colDown2,
				gBitMapOperate.edoVertical,true, lrcButtonRect.bottom - lrcButtonRect.top);
			gBitMapOperate.DrawLine(hDC, gWndColor.getColor(TABLE_CTROL)->colDown2,1,lrcButtonRect.left,lrcButtonRect.bottom,lrcButtonRect.right,
				lrcButtonRect.bottom, true);
			memset(&tcItem, 0, sizeof(TCITEM));
			tcItem.mask = TCIF_TEXT;
			tcItem.pszText = new TCHAR[256 + 1];	//初始化
			tcItem.cchTextMax = 256;				//文本长度
			CString lstrText;
			GetItem(i, &tcItem);
			lstrText = tcItem.pszText;
			SetBkMode(hDC, TRANSPARENT);

            LOGFONT  lf;
			ZeroMemory(&lf,sizeof(lf));
			lf.lfHeight=-MulDiv(9,GetDeviceCaps(CDC::FromHandle(hDC)->m_hDC,LOGPIXELSY),72);
			lf.lfWeight=FW_MEDIUM;
			lf.lfCharSet=ANSI_CHARSET;
			lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
			lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
			lf.lfQuality=DEFAULT_QUALITY;
			lf.lfPitchAndFamily=FF_DONTCARE | DEFAULT_PITCH;
			CFont font;
			font.CreateFontIndirect(&lf);
			CFont * oldFont = CDC::FromHandle(hDC)->SelectObject(&font);

			SetTextColor(hDC,gWndColor.getColor(TABLE_CTROL)->structColBase->colFont);
			DrawText(hDC, lstrText, lstrText.GetLength(), &lrcButtonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);


			if (tcItem.pszText != NULL)
			{
				delete [] tcItem.pszText;
				tcItem.pszText = NULL;
			}
			CDC::FromHandle(hDC)->SelectObject(oldFont);
		}
		else
		{
			CDC::FromHandle(hDC)->FrameRect( &lrcButtonRect, &CBrush( gWndColor.getColor(TABLE_CTROL)->structColBase->colFrame1) );
			InflateRect(&lrcButtonRect,-1,-1);
			gBitMapOperate.FillRectangle(hDC,lrcButtonRect,gWndColor.getColor(TABLE_CTROL)->colNormal1,gWndColor.getColor(TABLE_CTROL)->colNormal2,
				gBitMapOperate.edoVertical,true, lrcButtonRect.bottom - lrcButtonRect.top);
			memset(&tcItem, 0, sizeof(TCITEM));
			tcItem.mask = TCIF_TEXT;
			tcItem.pszText = new TCHAR[256 + 1];	//初始化
			tcItem.cchTextMax = 256;				//文本长度
			CString lstrText;
			GetItem(i, &tcItem);
			lstrText = tcItem.pszText;
			SetBkMode(hDC, TRANSPARENT);

			LOGFONT  lf;
			ZeroMemory(&lf,sizeof(lf));
			lf.lfHeight=-MulDiv(9,GetDeviceCaps(CDC::FromHandle(hDC)->m_hDC,LOGPIXELSY),72);
			lf.lfWeight=FW_MEDIUM;
			lf.lfCharSet=ANSI_CHARSET;
			lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
			lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
			lf.lfQuality=DEFAULT_QUALITY;
			lf.lfPitchAndFamily=FF_DONTCARE | DEFAULT_PITCH;
			CFont font;
			font.CreateFontIndirect(&lf);
			CFont * oldFont = CDC::FromHandle(hDC)->SelectObject(&font);

			SetTextColor(hDC,gWndColor.getColor(TABLE_CTROL)->structColBase->colFrame1);
			DrawText(hDC, lstrText, lstrText.GetLength(), &lrcButtonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            

			if (tcItem.pszText != NULL)
			{
				delete [] tcItem.pszText;
				tcItem.pszText = NULL;
			}
			CDC::FromHandle(hDC)->SelectObject(oldFont);
		}

		int lcount = GetItemCount();
		this->GetItemRect(i,&lrcButtonRect);
		gBitMapOperate.DrawLine(hDC,gWndColor.getColor(TABLE_CTROL)->structColBase->colFrame1,1, lrcButtonRect.right, lrcButtonRect.bottom-1,lrcClient.right,lrcButtonRect.bottom-1, true );
	}



	//选回旧字体
	SelectObject(hDC, hOldFont);
	DeleteObject(hOldFont);
}

void CTabCtrlEx::RepaintClientRect()
{
	RECT lrcClient ;
	GetClientRect(&lrcClient);

	this->ClientToScreen(&lrcClient);
	GetParent()->ScreenToClient(&lrcClient);
	GetParent()->InvalidateRect(&lrcClient,FALSE);
}
