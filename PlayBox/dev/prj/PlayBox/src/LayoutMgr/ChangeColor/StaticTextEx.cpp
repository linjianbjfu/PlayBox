#include "stdafx.h"
#include "StaticTextEx.h"
#include "BitMapOperate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CBitMapOperate gBitMapOperate;
CStaticText::CStaticText()
{
	mPaintStyle = epsNormal;
	mbSetText = false;
	//mTextColor = RGB(20, 84, 156);
	mTextColor = RGB(0, 0, 0);
	mstrText = "";
	miTextTop = 0;
	mbCaptured = false;
	mbUrl = FALSE;
	mFormat = DT_LEFT | DT_VCENTER ;
	mWidth  =FW_NORMAL;
	mSize = 9;
	mTextSet = GB2312_CHARSET;
	mLinkText = "";
	m_fontName = "";
}

CStaticText::~CStaticText()
{
	if(mhBrush)
		DeleteObject((HGDIOBJ)mhBrush);
}

BEGIN_MESSAGE_MAP(CStaticText, CStatic) 
	//{{AFX_MSG_MAP(CStaticText)
	ON_WM_CTLCOLOR_REFLECT()
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP

	ON_WM_MOUSEMOVE()

	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//	ON_WM_SHOWWINDOW()
	//	ON_WM_PAINT()
	//ON_WM_PAINT()
END_MESSAGE_MAP()

HBRUSH CStaticText::CtlColor(CDC* pDC, UINT nCtlColor)
{
	if (mhBrush != NULL)
		DeleteObject((HGDIOBJ)mhBrush);

	mhBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	pDC->SetBkMode(TRANSPARENT);
	return mhBrush;

}
BOOL CStaticText::SetLinkAddress(CString linkaddress)
{   
	mbUrl = TRUE;
	if (linkaddress.Find("http") != -1)								//��֤��ַ�ĺϷ���
		mLinkText = linkaddress;
	else if (linkaddress.Find("@") != -1)
		mLinkText = "mailto:"+linkaddress;
	else
	{
		mLinkText = "";
		return FALSE;
	}
	return TRUE;
}
// $_FUNCTION_BEGIN ******************************
// �������ƣ�  OnSetText
// ����������  wParam, lParam  ��Ϣ����
// �� �� ֵ��  LRESULT
// ����˵����  �ػ������ı���ϢWM_SETTEXT�¼�
// $_FUNCTION_END ********************************
LRESULT CStaticText::OnSetText(WPARAM wParam, LPARAM lParam)
{
	CString lstrText = mstrText;
	mstrText = (LPCTSTR)lParam;
	mbSetText = true;

	if (lstrText != mstrText)
	{
		CRect lRect;
		GetClientRect(lRect);
		ClientToScreen(&lRect);

		CWnd* lpWnd = NULL;
		lpWnd = GetParent();
		lpWnd->ScreenToClient(&lRect);
		lpWnd->InvalidateRect(lRect, FALSE);
	}

	return NULL;
}
void CStaticText::InvalidateBack()
{
	CWnd* lpWnd = NULL;
	lpWnd = GetParent();
	/*if (lpWnd && ::IsWindow(lpWnd->m_hWnd)  )
	{
	CRect lRect;
	GetClientRect(lRect);
	ClientToScreen(&lRect);

	lpWnd->ScreenToClient(&lRect);
	lpWnd->InvalidateRect(lRect, FALSE);
	this->Invalidate(FALSE);
	}*/

}
// $_FUNCTION_BEGIN ******************************
// �������ƣ�  SetGradient
// ����������  aBeginColor	���俪ʼ��ɫ
//             aEndColor    ���������ɫ
// �� �� ֵ��  ��
// ����˵����  ���ƽ�����
// $_FUNCTION_END ********************************
void CStaticText::SetGradient(COLORREF aBeginColor, COLORREF aEndColor)
{
	mPaintStyle = epsHGradient;
	mBeginColor = aBeginColor;
	mEndColor = aEndColor;
	InvalidateBack();
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�  SetVerticalGradient
// ����������  aBeginColor	���俪ʼ��ɫ
//             aEndColor    ���������ɫ
// �� �� ֵ��  ��
// ����˵����  ���ƽ�����(��ֱ����)
// $_FUNCTION_END ********************************
void CStaticText::SetVerticalGradient(COLORREF aBeginColor, COLORREF aEndColor)
{
	mPaintStyle = epsVGradient;
	mBeginColor = aBeginColor;
	mEndColor = aEndColor;
	InvalidateBack();
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�  SetOpaque
// ����������  aBackColor  ������ɫ
// �� �� ֵ��  ��
// ����˵����  ���Ʋ�͸�����
// $_FUNCTION_END ********************************
void CStaticText::SetOpaque(COLORREF aBackColor)
{
	mPaintStyle = epsOpaque;
	mBackColor = aBackColor;
	InvalidateBack();
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�  SetToolBarBK
// ����������  aBorderColor  �߿���ɫ
// �� �� ֵ��  ��
// ����˵����  ���Ʋ�͸�����
// $_FUNCTION_END ********************************
void CStaticText::SetToolBarBK(COLORREF aBorderColor)
{
	mPaintStyle = espToolBar;
	mBorderColor = aBorderColor;
	InvalidateBack();
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�  SetNormal
// ����������  ��
// �� �� ֵ��  ��
// ����˵����  ����Ĭ�Ϸ��(͸��)
// $_FUNCTION_END ********************************
void CStaticText::SetNormal()
{
	mPaintStyle = epsNormal;
	InvalidateBack();
}
void CStaticText::SetInfoHeader(/*COLORREF aBorderColor,COLORREF aBackColor*/)
{
	mPaintStyle = epsInfoHeader;
	//mBorderColor = aBorderColor;
	//mBackColor = aBackColor;
	InvalidateBack();
}
void CStaticText::SetTextColor(COLORREF acolTextColor)
{
	mTextColor = acolTextColor;
	InvalidateBack();
}

void CStaticText::SetTextSize(int aiFontSize)
{
	mSize = aiFontSize;
	InvalidateBack();
}
void CStaticText::SetTextSet( UINT anTextSet )
{
	mTextSet = anTextSet;
	InvalidateBack();
}
void CStaticText::SetFontBold(LONG alBold)
{
	mlBold = alBold;
	InvalidateBack();
}

LRESULT CALLBACK CStaticText::SelfProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LONG lReturn = 0;
	CStaticText* lpThis = NULL;
	lpThis = (CStaticText*)CWnd::FromHandle(hWnd);
	if (lpThis == NULL)
		return lReturn;

	PAINTSTRUCT ps; 
	HDC hDC; 

	switch (uMsg)
	{

	case WM_PAINT:
		{
			hDC = ::BeginPaint(hWnd, &ps);
			lReturn = (LONG)::DefWindowProc(hWnd, uMsg, wParam, lParam);
			lpThis->DrawFace(hWnd,hDC);
			::EndPaint(hWnd,&ps);
			return lReturn;
		}
	case WM_NCPAINT:
		{
			return 0L;
		}
	}

	// ����ԭ���Ļص�����
	lReturn = (LONG)CallWindowProc(lpThis->mWndProc, hWnd, uMsg, wParam, lParam);
	return lReturn;
}

void CStaticText::PreSubclassWindow() 
{
	mWndProc = (WNDPROC)SetWindowLong(this->GetSafeHwnd(), GWL_WNDPROC, (LONG)SelfProc);
	ModifyStyle(0,SS_NOTIFY);										//ʹstatic�ؼ�����Ӧ�����Ϣ
	CStatic::PreSubclassWindow();
}

void CStaticText::DrawFace(HWND hwnd, HDC hDC)
{
	CRect lrcClient;                               
	GetClientRect(lrcClient);

	//��ȡ�ı�
	CString lstrText;
	if (mbSetText)
		lstrText = mstrText;

	else
		GetWindowText(lstrText);

	//ת��Static�ķ���Text
#define MAP_STYLE(src, dest) if (dwStyle & (src)) dwText |= (dest)
#define NMAP_STYLE(src, dest) if (!(dwStyle & (src))) dwText |= (dest)

	DWORD dwStyle = GetStyle(), dwText = 0;

	MAP_STYLE(SS_RIGHT,			DT_RIGHT				  );
	MAP_STYLE(SS_CENTER,		DT_CENTER				  );
	MAP_STYLE(SS_CENTERIMAGE,	DT_VCENTER | DT_SINGLELINE);
	MAP_STYLE(SS_NOPREFIX,		DT_NOPREFIX				  );
	MAP_STYLE(SS_WORDELLIPSIS,	DT_WORD_ELLIPSIS		  );
	MAP_STYLE(SS_ENDELLIPSIS,	DT_END_ELLIPSIS			  );
	MAP_STYLE(SS_PATHELLIPSIS,	DT_PATH_ELLIPSIS		  );

	NMAP_STYLE(	SS_LEFTNOWORDWRAP |
		SS_CENTERIMAGE    |
		SS_WORDELLIPSIS   |
		SS_ENDELLIPSIS    |
		SS_PATHELLIPSIS, DT_WORDBREAK);

	switch(mPaintStyle)
	{
	case epsNormal:
		break;
	case epsOpaque:
		gBitMapOperate.FillRectangle(hDC, lrcClient, mBackColor, FALSE);
		break;
	case epsHGradient:
		gBitMapOperate.FillRectangle(hDC, lrcClient, mBeginColor, mEndColor, gBitMapOperate.edoHorizontal, FALSE);
		break;
	case epsVGradient:
		gBitMapOperate.FillRectangle(hDC, lrcClient, mBeginColor, mEndColor, gBitMapOperate.edoVertical, FALSE);
		break;
	case espToolBar:
		{
			//�ϱ�
			gBitMapOperate.DrawLine(hDC, mBorderColor, 1, lrcClient.left, lrcClient.top, lrcClient.right - 1, lrcClient.top, FALSE);
			gBitMapOperate.DrawLine(hDC, RGB(255, 255, 255), 1, lrcClient.left, lrcClient.top + 1, lrcClient.right - 1, lrcClient.top + 1, FALSE);
			//�м�3�����
			gBitMapOperate.DrawLine(hDC, mBorderColor, 1, lrcClient.left + 56, lrcClient.top + 10, lrcClient.left + 56, lrcClient.top + 42, FALSE);
			gBitMapOperate.DrawLine(hDC, RGB(255, 255, 255), 1, lrcClient.left + 57, lrcClient.top + 10, lrcClient.left + 57, lrcClient.top + 42, FALSE);
			gBitMapOperate.DrawLine(hDC, mBorderColor, 1, lrcClient.left + 124, lrcClient.top + 10, lrcClient.left + 124, lrcClient.top + 42, FALSE);
			gBitMapOperate.DrawLine(hDC, RGB(255, 255, 255), 1, lrcClient.left + 125, lrcClient.top + 10, lrcClient.left + 125, lrcClient.top + 42, FALSE);
			gBitMapOperate.DrawLine(hDC, mBorderColor, 1, lrcClient.left + 192, lrcClient.top + 10, lrcClient.left + 192, lrcClient.top + 42, FALSE);
			gBitMapOperate.DrawLine(hDC, RGB(255, 255, 255), 1, lrcClient.left + 193, lrcClient.top + 10, lrcClient.left + 193, lrcClient.top + 42, FALSE);
			return;
		}
		break;
	case epsInfoHeader:
		{
			gBitMapOperate.FillRectangle(hDC,lrcClient,RGB(245,249,255),RGB(255,255,255),gBitMapOperate.edoVertical,false);
		}
		break;
	}
	DWORD dwExStyle = GetExStyle();
	if (dwExStyle & WS_EX_CLIENTEDGE)
	{
		CRect lRect = lrcClient;
		lRect.bottom -= 1;

		//if (IsWindowEnabled())
		//	gGdiExOperate.FillRectangle(hDC, lRect, RGB(125, 189, 237), FALSE);
		//else
		//	gGdiExOperate.FillRectangle(hDC, lRect, BrightColor(0xc0c0c0, 0.36), FALSE);
	}

	CDC* dc = CDC::FromHandle(hDC);

	//��ȡ��ǰ����
	/*CFont *lpFont, *lpOldFont;
	CFont loUseFont;
	LOGFONT lolf;
	lpFont = GetFont();

	COLORREF clTextColor = 0;

	if (lpFont == NULL)
	{
	lpFont = GetParent()->GetFont();
	}*/
	//if (lpFont != NULL)
	//{

	//	lpFont->GetLogFont(&lolf);
	//	if (miFontSize != 0)
	//	{
	//		lolf.lfHeight = miFontSize/* * 20*/;
	//	}
	//	if (mlBold != 0)
	//	{
	//		lolf.lfWeight = mlBold;
	//	}
	/*if(mbMouseOver == true&&mbUrl == true)
	{
	lolf.lfUnderline = true;
	clTextColor = RGB(0,44,133);
	}
	else
	{
	lolf.lfUnderline = false;
	clTextColor = mTextColor;
	}*/
	/*	loUseFont.CreateFontIndirect(&lolf);
	lpFont = &loUseFont;
	lpOldFont = dc->SelectObject(lpFont);*/
	//}
	COLORREF clTextColor = 0;
	dc->SetBkMode(TRANSPARENT);					//���ñ���͸��
	if (IsWindowEnabled())
	{
		dc->SetTextColor(clTextColor);				//����������ɫ
	}
	else
	{
		dc->SetTextColor(gBitMapOperate.BrightColor( RGB( 192,192,192), 0.40 ) );
	}


	if(mbUrl)
	{
		gBitMapOperate.DrawText( dc,lstrText,mTextColor,lrcClient,mFormat,true,mWidth,mSize,mTextSet,(string)m_fontName );
	}
	else
	{
		gBitMapOperate.DrawText( dc,lstrText,mTextColor,lrcClient,mFormat,false,mWidth,mSize,mTextSet,(string)m_fontName );
	}

}

BOOL CStaticText::EnableWindow(BOOL aValue)
{
	InvalidateBack();
	return CStatic::EnableWindow(aValue);

}

BOOL CStaticText::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CStaticText::SetTextTop(int aiValue )
{
	miTextTop = aiValue;
	InvalidateBack();
}

void CStaticText::OnMouseMove(UINT nFlags, CPoint point)
{
	SetCapture();													//��׽���
#ifndef IDC_HAND
#define IDC_HAND            MAKEINTRESOURCE(32649)
#endif
	if(mbUrl)
		::SetCursor(::LoadCursor(NULL, IDC_HAND));
	POINT mpoint;   
	GetCursorPos(&mpoint);											//ȡ�õ�ǰ���λ��
	CRect rect;
	GetWindowRect(&rect);											//ȡ�ÿؼ��Ĵ��ڴ�С

	if (!rect.PtInRect(mpoint))										//�ж�����Ƿ��ڿؼ��Ĵ�����
	{																//��겻�ڿؼ��Ĵ�����
		mbCaptured = FALSE;
		ReleaseCapture();											//�ͷ���겶׽
		MouseLeave();												//����mouseleave()����
		return;
	}

	//�������ڴ��������Ѿ�����׽�򷵻�
	if (!mbCaptured)
	{
		mbCaptured = TRUE;
		MouseEnter();												//����MouseEnter()����
		ReleaseCapture();											//�ͷ���겶׽
		CStatic::OnMouseMove(nFlags, point);						//���û�����Ӧ����
	}
}
void CStaticText::MouseLeave()
{
	mbMouseOver = false;
	//InvalidateBack();
}
void CStaticText::MouseEnter()
{
	mbMouseOver = true;												//����MouseEnter()����
	InvalidateBack();
}



void CStaticText::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	HWND lhParent = NULL;
	if (GetParent() != NULL)
	{
		lhParent = GetParent()->GetSafeHwnd();
	}
	::PostMessage(lhParent, MSG_LABEL_MOUSEDOWN,::GetWindowLong(this->m_hWnd,GWL_ID ),0);

	if (mLinkText.GetLength() != 0 )											
		ShellExecute(NULL, "open", mLinkText,NULL, NULL, SW_SHOWNORMAL);  
	CStatic::OnLButtonDown(nFlags, point);
}


void CStaticText::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	HWND lhParent = NULL;
	if (GetParent() != NULL)
	{
		lhParent = GetParent()->GetSafeHwnd();
	}
	::PostMessage(lhParent, MSG_LABEL_MOUSEUP,0,0);

	

	CStatic::OnLButtonUp(nFlags, point);
}
void CStaticText::SetTextFormat(UINT anFormat)
{
	mFormat = anFormat;
}
void CStaticText::SetTextWidth(UINT anWidth)
{
	mWidth = anWidth;
}
