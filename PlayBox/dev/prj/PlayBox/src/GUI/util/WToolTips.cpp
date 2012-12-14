#include "stdafx.h"
#include "WToolTips.h"

#define LINE_HIGHT   15

IMPLEMENT_DYNAMIC(CCWToolTips, CWnd)
CCWToolTips::CCWToolTips()
{
	m_lastPoint = CPoint(0,0);
	m_pClientWnd = NULL;
	m_type = CLIENT_TOOLTIPS;

	m_clrFontText = RGB(0x00,0x00,0x00);
	m_clrBorder = RGB(0x00,0x00,0x00);
	m_clrBK=RGB(0xFF,0xFF,0xFF);
	m_hParent = NULL;
	m_nTimer = 800;
	m_bChangeColor = TRUE;
}

CCWToolTips::~CCWToolTips()
{
}

BEGIN_MESSAGE_MAP(CCWToolTips, CWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()

// CWToolTips 消息处理程序

void CCWToolTips::SetChangeColor(BOOL bchange)
{
	m_bChangeColor = bchange;
}
void CCWToolTips::OnPaint()
{
	CPaintDC dc(this); 
	if(m_bChangeColor)
	{
		m_clrFontText = ::AfxGetUIManager()->UIGetSkinMgr()->GetColor("TooltipTextColor");
		m_clrBorder = ::AfxGetUIManager()->UIGetSkinMgr()->GetColor("TooltipBorderColor");
		m_clrBK = ::AfxGetUIManager()->UIGetSkinMgr()->GetColor("TooltipBKColor");
	}
	CDC *pDC = &dc;
	dc.SetBkMode(TRANSPARENT);	
	CFont font;
	font.CreatePointFont(90,_T("宋体"));	
	dc.SelectObject(&font);	
	CRect rect;
	GetClientRect(rect);	
//	rect.InflateRect(1,1,1,1);
	CBrush brush(m_clrBK);
	CBrush* pOldBrush = dc.SelectObject(&brush);
	dc.FillRect(rect,&brush);
	dc.SelectObject(pOldBrush);

	CString tempStrInfo = m_strInfo;
	CRect textRect = CRect(rect.left+5,rect.top+3,rect.right,rect.top + 23);
	int nowindex = -1;
	dc.SetTextColor(m_clrFontText);
	while((nowindex = tempStrInfo.Find("$#",0))!= -1)
	{
		CString temp = tempStrInfo.Left(nowindex);		
		//dc.DrawText(temp,textRect,DT_NOPREFIX);
		int rows = DrawMultiLine(temp, textRect, dc );
		textRect.top+= rows * LINE_HIGHT;
		textRect.bottom += rows * LINE_HIGHT;	
		tempStrInfo = tempStrInfo.Right(tempStrInfo.GetLength() - 2 - nowindex);
	}

	DrawMultiLine(tempStrInfo, textRect, dc );
}

//返回行数
int CCWToolTips::DrawMultiLine(CString tempStrInfo, CRect textRect, CDC &dc )
{
	int rows = 0;
	CSize size = dc.GetTextExtent(tempStrInfo);
	CSize one = dc.GetTextExtent("a");

	textRect.right -= one.cx;
	int pos = textRect.Width()/one.cx - 1;
	LPTSTR p = tempStrInfo.GetBuffer();
	int total = 0;
	while(p[total] != 0)
	{
		total ++;
	}

	CString line;
	int lastpos = 0;
	int isMultiByte = 0;
	if(textRect.Width() < size.cx)
	{
		textRect.right += 2;
		while(lastpos < total)
		{
			if(pos > total)
				pos = total - 1;
			for(int i = lastpos ; i<pos+1; i++ )
			{
				line += p[i];
				if(p[i] & 0x80)
				{
					line += p[i+1];
					if(i == pos)
						pos ++;
					i++;
				}
			}

			lastpos = pos+1;
			dc.DrawText(line,textRect,DT_NOPREFIX);
			rows ++;

			line = "";
			textRect.top += LINE_HIGHT;
			textRect.bottom += LINE_HIGHT;
			pos += textRect.Width()/one.cx - 1;
		}
	}
	else
	{
		rows ++;
		dc.DrawText(tempStrInfo,textRect,DT_NOPREFIX);
	}

	return rows;
}
void CCWToolTips::ShowToolTips(CString info,CPoint point, CRect rect, int type)
{
	SetTimer(ID_TEMER_TOOLTIPS,m_nTimer,NULL);
	m_rcRange = rect;
	m_type = type;
	m_lastPoint = point;
	m_strInfo = info;
}

void CCWToolTips::ShowToolTips(CString info,CPoint point,int type)
{
	SetTimer(ID_TEMER_TOOLTIPS,m_nTimer,NULL);
	m_rcRange = CRect(0,0,0,0);
	m_type = type;
	m_lastPoint = point;
	m_strInfo = info;
}

void CCWToolTips::HideTooTips()
{
	if(IsWindow(m_hWnd))
	{
		this->ShowWindow(SW_HIDE);
		KillTimer(ID_TEMER_TOOLTIPS);
	}
}

void CCWToolTips::ShowToolTips(CString info, CRect rect, int timeout)
{
	m_strInfo = info;
	SetWindowPos(&wndTopMost,rect.left,rect.top,rect.Width(),rect.Height(), SWP_NOACTIVATE);
	ShowWindow(SW_SHOWNOACTIVATE);
	SetTimer(ID_TIMER_HIDE, timeout, NULL);
	OnPaint();
	//Invalidate();
}

void CCWToolTips::OnTimer(UINT nIDEvent)
{
	if(nIDEvent == ID_TIMER_HIDE)
	{
		HideTooTips();
		KillTimer(ID_TIMER_HIDE);
	}
	if(m_pClientWnd == NULL )
	{
		return;
	}

	if(nIDEvent == ID_TEMER_TOOLTIPS)
	{
		bool bShowToolTips = false;
		CPoint pt(0,0);
		::GetCursorPos(&pt);
		if(m_type == FRAME_TOOLTIPS )
		{
			this->ClientToScreen(&pt);
		}

		if(m_rcRange == CRect(0,0,0,0))
		{
			if(pt == m_lastPoint)
				bShowToolTips = true;
		}
		else
		{
			if(m_rcRange.PtInRect(pt))
				bShowToolTips = true;
		}

		if(bShowToolTips)
		{
			CPaintDC dc(this); 
			dc.SetBkMode(TRANSPARENT);	
			CFont font;
			font.CreatePointFont(90,_T("宋体"));	
			dc.SelectObject(&font);	
			int start = 0;
			int nowindex = -1;
			int maxxsize = 0;
			int maxysize = LINE_HIGHT;
			CString tempStrInfo = m_strInfo;
			while((nowindex = tempStrInfo.Find("$#",0))!= -1)
			{
				CString temp = tempStrInfo.Left(nowindex);
				CSize size = dc.GetTextExtent(temp);
				if(size.cx>maxxsize)
					maxxsize = size.cx;
				//最宽不能超过300像素
				if(maxxsize > 300)
				{
					maxysize += (maxxsize / 300) * LINE_HIGHT;
					maxxsize = 300;
				}
				tempStrInfo = tempStrInfo.Right(tempStrInfo.GetLength() - 2 - nowindex);
				maxysize+=LINE_HIGHT;
			}
			CSize size = dc.GetTextExtent(tempStrInfo);
			if(size.cx>maxxsize)
				maxxsize = size.cx;

			//最宽不能超过300像素
			if(maxxsize > 300)
			{
				maxysize += (maxxsize / 300) * LINE_HIGHT + 20;
				maxxsize = 326;
			}
			else 
			{
				maxxsize+=25;
				maxysize+=20;
			}
			
			CPoint opoint(m_lastPoint.x+10,m_lastPoint.y+20), point(m_lastPoint.x+maxxsize,m_lastPoint.y+maxysize+20-LINE_HIGHT);
			CRect rectNew, clientRect, mainRect;
			m_pClientWnd->GetClientRect(clientRect);
			m_pClientWnd->ClientToScreen(clientRect);			

			//将tooltip移到鼠标上面显示
			if(point.y > clientRect.bottom && pt.y-maxysize+LINE_HIGHT > 0)
			{
				opoint.y = pt.y-maxysize+LINE_HIGHT;
				point.y  = pt.y;
			}

			//将tooltip移到鼠标右面显示
			if(point.x > clientRect.right && pt.x - maxxsize > 0)
			{
				point.x = pt.x;
				opoint.x = pt.x - maxxsize + 10;
			}
			rectNew = CRect(opoint, point);

			if(GetParent() != NULL)
			{
				m_pClientWnd->ScreenToClient(rectNew);
			}
			SetWindowPos(&wndTopMost,rectNew.left,rectNew.top,rectNew.Width(),rectNew.Height(), SWP_NOACTIVATE);
		    ShowWindow(SW_SHOWNOACTIVATE);
		}
		this->Invalidate(TRUE);
		KillTimer(ID_TEMER_TOOLTIPS);
	}

	CWnd::OnTimer(nIDEvent);
}

BOOL CCWToolTips::PreTranslateMessage(MSG *pMsg)
{
	if(pMsg->message != WM_PAINT && pMsg->message != WM_SIZE && pMsg->message != WM_SIZING)
	{
		if(m_hParent != NULL)
			::PostMessage(m_hParent, pMsg->message, pMsg->wParam, pMsg->lParam);
	}
	return CWnd::PreTranslateMessage(pMsg);
}

void CCWToolTips::OnNcPaint()
{
	CRect rect;
	GetWindowRect(rect);

	m_clrBorder = ::AfxGetUIManager()->UIGetSkinMgr()->GetColor("TooltipBorderColor");
	rect.OffsetRect(-rect.left, -rect.top);

	CDC *pWinDC = GetWindowDC();	

	CPen pen(PS_SOLID,1,m_clrBorder);

	CPen* pOldPen = pWinDC->SelectObject(&pen);
	pWinDC->MoveTo(rect.TopLeft().x,rect.TopLeft().y);
	pWinDC->LineTo(rect.BottomRight().x-1,rect.TopLeft().y);
	pWinDC->LineTo(rect.BottomRight().x-1,rect.BottomRight().y-1);
	pWinDC->LineTo(rect.TopLeft().x,rect.BottomRight().y-1);
	pWinDC->LineTo(rect.TopLeft().x,rect.TopLeft().y);
	pWinDC->SelectObject(pOldPen);
	pen.DeleteObject();

	ReleaseDC(pWinDC);
}
