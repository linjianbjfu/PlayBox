
#include "stdafx.h"

#include "VariableButtonBar.h"
#include ".\variablebuttonbar.h"

VButtonBar::VButtonBar()
{
	m_iBigShow=0;
	m_iBigShowBefore=-1;
	m_iMenuShow=0;
	m_iBigButtonTotal=0;
	m_iAverLength=0;
	m_iDrawMode=0;
	m_iMouseOn=-1;
	m_iMouseBefore=-1;
	m_iMouseDown=0;
	m_bIsMouseInClient=false;
	m_bIsMouseMove=false;
	m_bIsMenuDown=false;
	m_iPlaySignShow=-1;
}

VButtonBar::~VButtonBar()
{

}

BEGIN_MESSAGE_MAP(VButtonBar, CWnd)

	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_MESSAGE(MSG_CLICKED_BUTTON,OnClickButton)
	ON_MESSAGE(MSG_CLICKED_MENU,OnClickMenu)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void VButtonBar::OnPaint()
{
	CPaintDC dc(this);
	AfxGetUIManager()->UIGetLayoutMgr()->PaintBkGround( m_hWnd,&dc,true );
	CDC* cdc=dc.FromHandle(dc.m_hDC);
	DrawButtons(cdc);
}

void VButtonBar::OnSize(UINT nType, int cx, int cy)
{
	AfxGetUIManager()->UIGetLayoutMgr()->UpdateLayout( m_hWnd );
}
void f1(vector<string>&vec)
{
	for ( int i=0;i<vec.size();++i)
	{
		TRACE("%s\n",vec.at(i).c_str());
	}
}
void f2(vector<CRect>&vec)
{
	for (int i=0;i<vec.size();++i)
	{
		CRect rect=vec.at(i);
		TRACE("VEC:L%d,R%d,T%d,B%d\n",rect.left,rect.right,rect.top,rect.bottom);
	}
}
void VButtonBar::SetButtons(vector<string> &vecBmpBtnName
							,vector<string> &vecBmpNamePlaySign
							,vector<string> &vecBmpNameMenu
							,vector<CRect> &vecRectBig
							,vector<CRect> &vecRectSmall
							,vector<CRect> &vecRectPlaySignBig
							,vector<CRect> &vecRectPlaySignSmall
							,vector<CRect> &vecRectMenu
							)
{
	f1(vecBmpBtnName);
	f1(vecBmpNamePlaySign);
	f1(vecBmpNameMenu);
	f2(vecRectBig);
	f2(vecRectSmall);
	f2(vecRectPlaySignBig);
	f2(vecRectPlaySignSmall);
	f2(vecRectMenu);

	m_vecBmpBtnName=vecBmpBtnName;
	m_vecBmpNamePlaySign=vecBmpNamePlaySign;
	m_vecBmpNameMenu=vecBmpNameMenu;
	m_vecRectBig=vecRectBig;
	m_vecRectSmall=vecRectSmall;
	m_vecRectMenu=vecRectMenu;
	m_vecRectPlaySignBig=vecRectPlaySignBig;
	m_vecRectPlaySignSmall=vecRectPlaySignSmall;
	m_iBigButtonTotal=(int)vecBmpBtnName.size();
	m_iAverLength=vecRectSmall[0].right;
	LoadSkin();
	InitDrawVec();
}


void VButtonBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int iSel=PtInWhickRect(point);
	if (iSel>-1 && iSel<m_iBigButtonTotal*2)
	{
		SelectButton(iSel);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void VButtonBar::SelectButton(int iIdx)
{
	if(iIdx>=m_iBigButtonTotal)
	{
		m_bIsMenuDown=true;
		InvalidateRect(m_vecRectMenu[iIdx-m_iBigButtonTotal]);
		SendMessage(MSG_CLICKED_MENU,(WPARAM)(iIdx-m_iBigButtonTotal),0);
	}
	else
	{
		m_iMouseDown=iIdx;
		if(iIdx!=m_iBigShow)
		{
			m_iBigShowBefore=m_iBigShow;
			m_iBigShow=iIdx;
			m_iMenuShow=m_iBigShow;
			UpdateDrawVec();
			Invalidate(true);
			SendMessage(MSG_CLICKED_BUTTON,(WPARAM)iIdx,0);
		}
	}
}

void VButtonBar::ShowPlaySign(int iIdx)
{
	m_iPlaySignShow=iIdx;
	UpdateDrawVec();
	Invalidate(true);
}

int VButtonBar::PtInWhickRect(CPoint pt)
{
	int x=pt.x;
	int y=pt.y;

	if(m_iAverLength ==0)
		return -1;
	if(PtInRect(&m_vecRectBig[m_iBigShow],pt))
	{
		if(PtInRect(&m_vecRectMenu[m_iBigShow],pt))
		{
			return m_iBigButtonTotal+m_iBigShow;
		}else
		{
			return m_iBigShow;
		}
	}

	if (x>m_vecRectBig[m_iBigButtonTotal-1].right)
	{
		return -1;
	}
	else if(x<m_vecRectBig[m_iBigShow].left)
	{
		return x/m_iAverLength;
	}
	else if(x>m_vecRectBig[m_iBigShow].right)
	{
		return m_iBigShow+((x-m_vecRectBig[m_iBigShow].right)/m_iAverLength)+1;
	}
	
}

void VButtonBar::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_bIsMouseInClient==false)
	{
		TRACKMOUSEEVENT t = { sizeof(TRACKMOUSEEVENT), TME_LEAVE|TME_HOVER, m_hWnd, 0};
		if(TrackMouseEvent(&t))
		{
			m_bIsMouseInClient=true;
		}
	}
	int iSel=PtInWhickRect(point);
	if (iSel>-1 && iSel<m_iBigButtonTotal*2)
	{
		m_iMouseOn=iSel;
		if(m_iMouseOn!=m_iMouseBefore)
		{
			m_bIsMouseMove=true;
			if(m_iMouseOn<m_iBigButtonTotal)
			{
				InvalidateRect(&(m_vecCurRect[m_iMouseOn]));
				if(m_iMouseBefore>=0 && m_iMouseBefore<m_iBigButtonTotal*2)
				{
					//InvalidateRect(&(m_vecCurRect[m_iMouseBefore]));
					Invalidate();
			}
			
		}else
			{
				InvalidateRect(&(m_vecRectMenu[m_iMouseOn-m_iBigButtonTotal]));
			}
			m_iMouseBefore=m_iMouseOn;
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void VButtonBar::DrawBitmap(CDC* dc, CDibBitmap* hbmp, RECT r)
{

	if(!hbmp) return;	//safe check

	int cx=r.right  - r.left;
	int cy=r.bottom - r.top;

	CDC dcBmp,dcMask;
	dcBmp.CreateCompatibleDC(dc);
	int iSaveDC = dcBmp.SaveDC();

	CBitmap hBitmapBmp;                     //需要删除
	hBitmapBmp.CreateCompatibleBitmap(dc,hbmp->m_lpBMIH->biWidth,hbmp->m_lpBMIH->biHeight);
	dcBmp.SelectObject(&hBitmapBmp);
	hbmp->SetCDibRect(CRect(0,0,hbmp->m_lpBMIH->biWidth,hbmp->m_lpBMIH->biHeight));
	hbmp->Draw(&dcBmp,FALSE);

	int bx=hbmp->GetWidth();
	int by=hbmp->GetHeight();
	dc->StretchBlt(r.left,r.top,cx,cy,&dcBmp,0,0,bx,by,SRCCOPY);

	dcBmp.RestoreDC( iSaveDC );

	DeleteDC(dcBmp);
	hBitmapBmp.DeleteObject();
}

void VButtonBar::LoadSkin()
{
	m_vecCDBSmallNormal.clear();
	m_vecCDBSmallOver.clear();
	m_vecCDBBigNormal.clear();
	m_vecCDBPlaySignNormal.clear();
	m_vecCDBPlaySignOver.clear();
	m_vecCDBPlaySignBig.clear();
	m_vecCDBMenuNormal.clear();
	m_vecCDBMenuOver.clear();
	m_vecCDBMenuDown.clear();
	ISkinMgr* skinMgr=AfxGetUIManager()->UIGetSkinMgr();
	for(int i=0;i<(int)m_vecBmpBtnName.size();i++)
	{
		CDibBitmap* cdbNormal;
		CDibBitmap* cdbOver;
		cdbNormal=skinMgr->GetDibBmp((m_vecBmpBtnName[i]+"SmallNormal").c_str());
		cdbOver=skinMgr->GetDibBmp((m_vecBmpBtnName[i]+"SmallOver").c_str());
		m_vecCDBSmallNormal.push_back(cdbNormal);
		m_vecCDBSmallOver.push_back(cdbOver);

		CDibBitmap* cdbBigNormal;
		cdbBigNormal=skinMgr->GetDibBmp((m_vecBmpBtnName[i]+"Big").c_str());
		m_vecCDBBigNormal.push_back(cdbBigNormal);
	}

	for (int i=0;i<m_vecBmpNamePlaySign.size();++i)
	{
		CDibBitmap *cdbPlaySign;
		cdbPlaySign=skinMgr->GetDibBmp((m_vecBmpNamePlaySign[i]+"Normal").c_str());
		m_vecCDBPlaySignNormal.push_back(cdbPlaySign);
		cdbPlaySign=skinMgr->GetDibBmp((m_vecBmpNamePlaySign[i]+"Over").c_str());
		m_vecCDBPlaySignOver.push_back(cdbPlaySign);
		cdbPlaySign=skinMgr->GetDibBmp((m_vecBmpNamePlaySign[i]+"Big").c_str());
		m_vecCDBPlaySignBig.push_back(cdbPlaySign);
	}
	for(int i=0;i<(int)m_vecBmpNameMenu.size();i++)
	{
		CDibBitmap* cdbNormal;
		CDibBitmap* cdbOver;
		CDibBitmap* cdbDown;
		cdbNormal=skinMgr->GetDibBmp((m_vecBmpNameMenu[i]+"Normal").c_str());
		cdbOver=skinMgr->GetDibBmp((m_vecBmpNameMenu[i]+"Over").c_str());
		cdbDown=skinMgr->GetDibBmp((m_vecBmpNameMenu[i]+"Down").c_str());
		m_vecCDBMenuNormal.push_back(cdbNormal);
		m_vecCDBMenuOver.push_back(cdbOver);
		m_vecCDBMenuDown.push_back(cdbDown);
	}
	
}

BOOL VButtonBar::OnEraseBkgnd(CDC* pDC)
{
	return 1;
}

void VButtonBar::DrawButtons(CDC *cdc)
{

	for(int i=0;i<m_iBigButtonTotal;i++)
	{
		CRect rect=m_vecCurRect[i];
		DrawBitmap(cdc,m_vecCurCDB[i],m_vecCurRect[i]);
	}

	DrawBitmap(cdc,m_vecCDBMenuNormal[m_iMenuShow],m_vecRectMenu[m_iMenuShow]);

	if(m_bIsMouseMove && m_iMouseOn!=-1)
	{
		DrawCurButtonOver(cdc);
		m_bIsMouseMove=false;
	}

/*
	if(m_bIsMenuDown)
	{
		DrawCurSmallButtonDown(cdc);
	}
*/

	if (m_iPlaySignShow>=0 && m_iPlaySignShow<m_vecBmpNamePlaySign.size())
	{
		if (m_iPlaySignShow==m_iBigShow)
		{
			DrawBitmap(cdc,m_vecCDBPlaySignBig[m_iPlaySignShow],m_vecRectPlaySignBig[m_iPlaySignShow]);
		}
		else
		{
			CRect rect(m_vecRectPlaySignSmall[m_iPlaySignShow]);
			if (m_iPlaySignShow>m_iBigShow)
			{
				CRect sRect=m_vecRectSmall[m_iBigShow];
				CRect bRect=m_vecRectBig[m_iBigShow];
				int iMinus=bRect.Width()-sRect.Width();
				rect.left+=iMinus;
				rect.right+=iMinus;
			}

			if (m_iPlaySignShow==m_iMouseOn)
			{
				DrawBitmap(cdc,m_vecCDBPlaySignOver[m_iPlaySignShow],rect);
			}
			else
			{
				DrawBitmap(cdc,m_vecCDBPlaySignNormal[m_iPlaySignShow],rect);
			}
		}
	}
}

void VButtonBar::UpdateDrawVec()
{
	m_vecCurCDB.clear();
	m_vecCurRect.clear();
	for(int i=0;i<m_iBigShow;i++)
	{
		m_vecCurCDB.push_back(m_vecCDBSmallNormal[i]);
		m_vecCurRect.push_back(m_vecRectSmall[i]);
	}
	m_vecCurCDB.push_back(m_vecCDBBigNormal[m_iBigShow]);
	m_vecCurRect.push_back(m_vecRectBig[m_iBigShow]);

	CRect sRect=m_vecRectSmall[m_iBigShow];
	CRect bRect=m_vecRectBig[m_iBigShow];
	int iMinus=bRect.Width()-sRect.Width();

	for(int i=m_iBigShow+1;i<m_iBigButtonTotal;i++)
	{
		CRect rect=m_vecRectSmall[i];
		rect.left+=iMinus;
		rect.right+=iMinus;
		m_vecCurCDB.push_back(m_vecCDBSmallNormal[i]);
		m_vecCurRect.push_back(rect);
	}
}

void VButtonBar::DrawCurButtonOver(CDC *cdc)
{
	if(m_iMouseOn==m_iBigShow)
	{
		return;
	}
	if(m_iMouseOn>-1 && m_iMouseOn<m_iBigButtonTotal)
	{
		DrawBitmap(cdc,m_vecCDBSmallOver[m_iMouseOn],m_vecCurRect[m_iMouseOn]);
	}
	else if(m_iMouseOn>=m_iBigButtonTotal && m_iMouseOn<m_iBigButtonTotal*2)
	{
		DrawBitmap(cdc,m_vecCDBMenuOver[m_iMouseOn-m_iBigButtonTotal],m_vecRectMenu[m_iMouseOn-m_iBigButtonTotal]);
	}
}

/*
void VButtonBar::DrawCurSmallButtonDown(CDC *cdc)
{
	if(m_iMouseDown>=m_iBigButtonTotal)
	{
		DrawBitmap(cdc,m_vecCDBMenuDown[m_iMouseDown-m_iBigButtonTotal],m_vecRectMenu[m_iMouseDown-m_iBigButtonTotal]);
	}
}
*/

LRESULT VButtonBar::OnMouseLeave(WPARAM,LPARAM)
{
	m_bIsMouseMove=false;
	m_bIsMenuDown=false;
	m_iMouseOn=-1;
	m_iMouseBefore=-1;
	m_bIsMouseInClient=false;
	Invalidate();
	return 0;
}

void VButtonBar::InitDrawVec()
{
	m_vecCurCDB.push_back(m_vecCDBBigNormal[0]);
	m_vecCurRect.push_back(m_vecRectBig[0]);

	CRect sRect=m_vecRectSmall[m_iBigShow];
	CRect bRect=m_vecRectBig[m_iBigShow];
	int iMinus=bRect.Width()-sRect.Width();

	for(int i=1;i<(int)m_vecCDBSmallNormal.size();i++)
	{
		CRect rect=m_vecRectSmall[i];
		rect.left+=iMinus;
		rect.right+=iMinus;
		m_vecCurCDB.push_back(m_vecCDBSmallNormal[i]);
		m_vecCurRect.push_back(rect);
	}
}
void VButtonBar::OnLButtonUp(UINT nFlags, CPoint point)

{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	m_bIsMenuDown=false;
	m_bIsMouseMove=true;
	//InvalidateRect(m_vecRectMenu[m_iMouseDown]);
	CWnd::OnLButtonUp(nFlags, point);
}

LRESULT VButtonBar::OnMouseHover(WPARAM,LPARAM)
{
	Invalidate();
	return 0;
}
int  VButtonBar::GetSelectedButtonId()
{
	return m_iMouseDown;
}

CRect VButtonBar::GetNormalButtonRect(int iIdx)
{
	return m_vecRectSmall.at(iIdx);
}