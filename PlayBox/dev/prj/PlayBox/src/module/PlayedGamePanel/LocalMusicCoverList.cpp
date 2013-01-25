#include "stdafx.h"
#include "../../gui/CommonControl/AcceptDrag.h"
#include "LocalMusicCoverList.h"
#include "../../gui/CommonControl/SkinScroll/coolsb_detours.h"
#include "../../AppConfig/config/ConfigAppDef.h"
#include <algorithm>

extern LRESULT HandleCustomDraw(UINT ctrlid, NMCSBCUSTOMDRAW *nm,CDibBitmap * bmpV, CDibBitmap * bmpH);
extern const char* ChStringToPinyin(char *pinyin, const char *ch, int nMaxLenght);
#define DRAG_TIMER		103

IMPLEMENT_DYNAMIC(CLocalMusicCoverList, CWnd)

CLocalMusicCoverList::CLocalMusicCoverList()
: m_iItemWidth(0)
, m_iItemHeight(0)
, m_rect(0,0,0,0)
, m_pOldBitmap(NULL)
, m_rcRealRect(0,0,0,0)
, m_ImgState(LMCI_Middle)
, m_ptClientStart(0,0)
, m_blTracker(FALSE)
, m_ptDown(-1,-1)
, m_bmpoldTracker(NULL)
, m_rcTracker(0,0,0,0)
, m_iDistance(0)
, m_blDrag(FALSE)
, m_blMutil(FALSE)
, m_iPreSel(-1)
, m_blIsJudgeDrag(FALSE)
, m_pBitmapScrollHorz(NULL)
, m_pBitmapScrollVert(NULL)
, m_bInit(FALSE)
, m_PaintScrollBar(FALSE)
{
	m_pBitmapYes=new CBitmap();
	m_pBitmapNo=new CBitmap();
	ISkinMgr*	pSkinMgr = AfxGetUIManager()->UIGetSkinMgr();
	SetScrollBitmap(pSkinMgr->GetDibBmp("ScrollVert"), pSkinMgr->GetDibBmp("ScrollHorz"));
	m_gdires.colDetail = pSkinMgr->GetColor( "PlayedGameDetailTextColor" );
	m_gdires.colListBK = pSkinMgr->GetColor( "PlayedGameBkColor" );
	m_gdires.colName = pSkinMgr->GetColor( "PlayedGameNameColor" );
	
	int iValue = 0;
	AfxGetUserConfig()->GetConfigIntValue( CONF_APP_MODULE_NAME,CONF_APP_PLAYED_ICON_SIZE, iValue);
	switch( iValue )
	{
	case 0: //小图标
		SetItemWH(80,80);
		break;
	case 1:
		SetItemWH(100,100);
		break;
	default:
		SetItemWH(130,130);
		break;
	}
}

CLocalMusicCoverList::~CLocalMusicCoverList()
{
	delete m_pBitmapYes;
	delete m_pBitmapNo;
}

BEGIN_MESSAGE_MAP(CLocalMusicCoverList, CWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(LMC_MSG_TRACKERCHANGE,OnTrackerChange)
	ON_NOTIFY_REFLECT(NM_COOLSB_CUSTOMDRAW, OnCoolSBCustomdraw )
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
	ON_WM_SHOWWINDOW()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

int CLocalMusicCoverList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// 创建绘图相关资源
	CClientDC dc(this);
	m_dcMem.CreateCompatibleDC(&dc);
	m_dcTracker.CreateCompatibleDC(&dc);
	m_fName.CreateFont(12, 0, 0, 0, FW_BOLD , FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));
	m_fDeInfo.CreateFont(12, 0, 0, 0, FW_NORMAL , FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));
	// 竖直滚动控制
	m_vScrollTimer = SetTimer(100,1,NULL);
	// 透明度控制
	m_bf.BlendOp = AC_SRC_OVER; 
	m_bf.BlendFlags = 0; 
	m_bf.AlphaFormat = 0; 
	m_bf.SourceConstantAlpha = 50; 

	TCHAR path_tmp[MAX_PATH]={0};
	::GetModuleFileName(NULL,path_tmp,MAX_PATH);
	CString strFrameImg(path_tmp);
	strFrameImg=strFrameImg.Mid(0,strFrameImg.ReverseFind('\\'));
	strFrameImg+="\\Resources\\StandardUI\\IconBorder.png";
	USES_CONVERSION;
	Image imgFrame(A2W(strFrameImg));
	pImageFrame=imgFrame.Clone();
	return 0;
}

BOOL CLocalMusicCoverList::OnEraseBkgnd(CDC* pDC)
{
	return CWnd::OnEraseBkgnd(pDC);
}

void CLocalMusicCoverList::OnPaint()
{
	if(!m_bInit )
	{
		m_bInit = true;
		int w,h;
		w = h = 15;

		if( m_pBitmapScrollHorz && m_pBitmapScrollVert )
		{
			h = m_pBitmapScrollVert->GetHeight() / 3;
			w = m_pBitmapScrollVert->GetWidth() / 6;
		}

		InitializeCoolSB(m_hWnd);
		CoolSB_SetStyle(m_hWnd, SB_BOTH, CSBS_HOTTRACKED);
		CoolSB_SetSize(m_hWnd, SB_BOTH, h, w);
		::CoolSB_SetMinThumbSize(m_hWnd, SB_BOTH, 8);
		
	}
	if (m_PaintScrollBar)
	{
		int lenVbar = (float)m_rect.Height()/m_rcRealRect.Height()*m_rect.Height();
		int realhight = (float)(m_rect.Height()-lenVbar)/m_rect.Height()*m_rcRealRect.Height();
		float fper = (float)(0-m_ptClientStart.y)/(m_rcRealRect.Height()-m_rect.Height());
		int nPos = fper*realhight;
		CoolSB_SetScrollPos(m_hWnd,SB_VERT,nPos,TRUE);
		m_PaintScrollBar = FALSE;
	}
	CPaintDC dc(this); 
	CRect rcClient;
	GetClientRect(&rcClient);
	CSize sz = CSize(m_rect.Width(),m_rect.Height());
	
	dc.BitBlt(0,0,sz.cx,sz.cy,&m_dcMem,0,0,SRCCOPY);
}

void CLocalMusicCoverList::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	SizeChanged();
	
}

BOOL CLocalMusicCoverList::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	BOOL ret = CWnd::Create(AfxRegisterWndClass(CS_DBLCLKS| CS_CLASSDC | CS_SAVEBITS,AfxGetApp()->LoadStandardCursor(IDC_ARROW),0,0),
		_T(""), dwStyle, rect, pParentWnd, nID, pContext);

	return ret;	
}

void CLocalMusicCoverList::SetItemWH(int iItemWidth, int iItemHeight)
{
	m_iItemWidth = iItemWidth;
	m_iItemHeight = iItemHeight;
}

void CLocalMusicCoverList::SetImgState(LMCImgState is)
{
	m_ImgState=is;
	int iValue = 0;
	switch(m_ImgState)
	{
	case LMCI_Small:
		SetItemWH(80,80);
		iValue = 0;
		break;
	case LMCI_Middle:
		SetItemWH(100,100);
		iValue = 1;
		break;
	case LMCI_Big:
		SetItemWH(130,130);
		iValue = 2;
		break;
	}
	AfxGetUserConfig()->SetConfigIntValue( CONF_APP_MODULE_NAME,CONF_APP_PLAYED_ICON_SIZE, iValue);
	m_ptClientStart.SetPoint(0,0);
	UpdateList();
	OnMemoryDraw();
}

int CLocalMusicCoverList::InsertItem(CDibBitmap* pImg,
									 CString strItemName,
									 CString strdetail,
									 string strGID,
									 string strSrvID,
									 string strAddTime,
									 int nGameType,
									 BOOL blSel/* =FALSE */)
{
	int iHeightSpaceS = m_iItemHeight+SPACEH + ITEMTXTH + SPACEH;
	int iHeightSpaceMB = m_iItemHeight+SPACEH + ITEMTXTH + SPACEH + ITEMTXTH + SPACEH  ;	
	//if (m_ImgState==LMCI_Small)
	//{
	//}
	//else if (m_ImgState==LMCI_Middle||m_ImgState==LMCI_Big)
	//{
	//}
	return m_DataMgr.InsertItem(pImg,strItemName,strdetail, strGID, strSrvID, strAddTime, nGameType, blSel);
}

int CLocalMusicCoverList::InsertItem(LPCTSTR szImgPath, 
									 CString strItemName, 
									 CString strdetail,
									 string strGID,
									 string strSrvID, 
									 string strAddTime,
									 int nGameType,
									 BOOL blSel/* =FALSE */)
{
	return m_DataMgr.InsertItem(szImgPath,strItemName,strdetail,strGID,strSrvID,strAddTime,nGameType,blSel);
}

vector<int> CLocalMusicCoverList::GetSelectItem()
{
	vector<int> vIndex;
	CLMCDataMgr::iterLMCItem it = m_DataMgr.m_vItem.begin();
	CLMCDataMgr::iterLMCItem itend = m_DataMgr.m_vItem.end();
	for (;it!=itend;it++)
	{
		if (it->blSel)
		{
			vIndex.push_back(it->iIndex);
		}
	}
	return vIndex;
}

LMC_ItemInfo CLocalMusicCoverList::GetItemInfoByIndex(int iItem)
{
	return m_DataMgr.m_vItem[iItem];
}

void CLocalMusicCoverList::OnCalcTracker()
{
	if (m_blTracker)
	{
		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		m_ptDown.y = m_ptClientStart.y+m_iDistance;
		if (point.x>m_ptDown.x)
		{
			m_rcTracker.left = m_ptDown.x;
			m_rcTracker.right = point.x;
		}
		else
		{
			m_rcTracker.left = point.x;
			m_rcTracker.right = m_ptDown.x;
		}
		if (point.y>m_ptDown.y)
		{
			m_rcTracker.top = m_ptDown.y;
			m_rcTracker.bottom = point.y;
		}
		else
		{
			m_rcTracker.top = point.y;
			m_rcTracker.bottom = m_ptDown.y;
		}

		// 确定拖拽矩形左右边界不出界
		if (m_rcTracker.left<m_rect.left)
			m_rcTracker.left = m_rect.left;
		if (m_rcTracker.right>m_rect.right)
			m_rcTracker.right = m_rect.right;

	}
}

void CLocalMusicCoverList::OnDrawTracker()
{
	if (m_blTracker)
	{
		CBrush brNull;
		brNull.CreateStockObject(NULL_BRUSH);
		CBrush* brOld = m_dcMem.SelectObject(&brNull);
		CPen pen(PS_SOLID,1,RGB(125,125,125));
		CPen *penold = m_dcMem.SelectObject(&pen);
		m_dcMem.Rectangle(&m_rcTracker);
		CRect rcDraw;
		if (rcDraw.IntersectRect(&m_rcTracker,&m_rect))
			m_dcMem.AlphaBlend(rcDraw.left,rcDraw.top,rcDraw.Width(),rcDraw.Height(),&m_dcTracker,0,0,rcDraw.Width(),rcDraw.Height(),m_bf);
		m_dcMem.SelectObject(brOld);
		m_dcMem.SelectObject(penold);
		OnCalcSelectItem();
	}
}

void CLocalMusicCoverList::OnMemoryDraw(BOOL blRePaint/* =TRUE */)
{
	if (m_rect.Width()==0) return;
	m_dcMem.FillSolidRect(&m_rect,m_gdires.colListBK);
	
	if (m_rect.Width()<550&&m_rect.Height()>m_rcRealRect.Height())
	{// 右边与RmPanel区域的分割线
		CRect rcRightLine=m_rect;
		rcRightLine.left=rcRightLine.right-1;
		m_dcMem.FillSolidRect(&rcRightLine,RGB(165,186,214));
	}
	// 首先计算拖拽区域
	OnCalcTracker();
	int iHeightSpace=0;
	int iWidthSpace=0;
	if (m_ImgState==LMCI_Small)
		iHeightSpace = m_iItemHeight+SPACEH + ITEMTXTH + SPACEH*2;
	else
		iHeightSpace = m_iItemHeight+SPACEH + ITEMTXTH + SPACEH + ITEMTXTH + SPACEH*3;
	if (m_ImgState==LMCI_Small)
		iWidthSpace = SPACEWS;
	else
		iWidthSpace = SPACEW;
	CPoint ptS = m_ptClientStart;
	ptS.x = iWidthSpace+m_ptClientStart.x;
	ptS.y = SPACEH+m_ptClientStart.y;
	
	for(int i=0;i<m_DataMgr.m_vItem.size();i++)
	{
		if (ptS.x+m_iItemWidth>m_rect.right)
		{
			ptS.y = ptS.y + iHeightSpace;
			//ptS.x = m_ptClientStart.x+SPACEW;
			ptS.x = m_ptClientStart.x+iWidthSpace;
			i--;// 这个超屏幕了，下一个画的时候还要画这个，所以这里要减1
		}
		else
		{
			if (ptS.y>m_rect.bottom)
			{
				// 超出屏幕的区域只是计算Item所在的区域，并不绘制
				CalcHideItemRect(ptS,m_DataMgr.m_vItem[i]);
				//ptS.x = ptS.x+m_iItemWidth+SPACEW;
				ptS.x = ptS.x+m_iItemWidth+iWidthSpace;
			}
			else
			{
				if (ptS.y+iHeightSpace>=m_rect.top)
				{
					DrawItem(ptS,m_DataMgr.m_vItem[i]);
				}
				else
				{
					// 超出屏幕的区域只是计算Item所在的区域，并不绘制
					CalcHideItemRect(ptS,m_DataMgr.m_vItem[i]);
				}
				//ptS.x = ptS.x+m_iItemWidth+SPACEW;
				ptS.x = ptS.x+m_iItemWidth+iWidthSpace;
			}
		}
	}
	// 绘制拖拽区域
	OnDrawTracker();
	//Invalidate();
	RedrawWindow();

	int lenVbar = (float)m_rect.Height()/m_rcRealRect.Height()*m_rect.Height();
	int realhight = (float)(m_rect.Height()-lenVbar)/m_rect.Height()*m_rcRealRect.Height();
	float fper = (float)(0-m_ptClientStart.y)/(m_rcRealRect.Height()-m_rect.Height());
	int nPos = fper*realhight;
	CoolSB_SetScrollPos(m_hWnd,SB_VERT,nPos,TRUE);
}

void CLocalMusicCoverList::SizeChanged(BOOL blFouce)
{
	CRect rs;
	GetClientRect(&rs);
	if (!blFouce)
	{
		if(rs.IsRectNull() || rs.IsRectEmpty() || rs == m_rect)
			return;
	}

	m_rect = rs;
	CClientDC dc(this);
	CSize sz = CSize(m_rect.Width(),m_rect.Height());
	if(m_pOldBitmap)
	{
		if(m_dcMem.GetSafeHdc())
			m_dcMem.SelectObject(m_pOldBitmap);
		m_pOldBitmap = NULL;
	}
	if (m_bmpoldTracker)
	{
		if (m_dcTracker.GetSafeHdc())
			m_dcTracker.SelectObject(m_pOldBitmap);
		m_bmpoldTracker = NULL;
	}
	if(m_memBitmap.GetSafeHandle())
		m_memBitmap.DeleteObject();
	if(m_memBitmap.CreateCompatibleBitmap(&dc,sz.cx,sz.cy))
		m_pOldBitmap = m_dcMem.SelectObject(&m_memBitmap);
	
	UpdateList();
	m_ptClientStart.x = 0;
	m_ptClientStart.y = 0;
	OnMemoryDraw();
}

void CLocalMusicCoverList::UpdateList()
{
	m_ptClientStart.SetPoint(0,0);
	CRect rs;
	GetClientRect(&rs);
	if (rs.IsRectNull() || rs.IsRectEmpty()) return;
	CClientDC dc(this);
	if (m_bmpTracker.GetSafeHandle())
		m_bmpTracker.DeleteObject();
	int iHeightSpace=0;
	if (m_ImgState==LMCI_Small)
		iHeightSpace = m_iItemHeight+SPACEH + ITEMTXTH + SPACEH;
	else
		iHeightSpace = m_iItemHeight+SPACEH + ITEMTXTH + SPACEH + ITEMTXTH + SPACEH*3;
	int iW=0;
	m_ImgState==LMCI_Small?iW = m_iItemWidth+SPACEWS:iW = m_iItemWidth+SPACEW;
	
	int iCount = m_rect.Width()/iW;
	int iLine = m_DataMgr.m_vItem.size()/iCount;
	if (m_DataMgr.m_vItem.size()%iCount)
		iLine++;
	m_rcRealRect.bottom = iLine* iHeightSpace;
	m_rcRealRect.right = m_rect.right;

	if (m_bmpTracker.CreateCompatibleBitmap(&dc,m_rect.Width(),m_rect.Height()))
 	{
 		m_bmpoldTracker = m_dcTracker.SelectObject(&m_bmpTracker);
 		//m_dcTracker.FillSolidRect(0,0,m_rcRealRect.Width(),m_rcRealRect.Height(),RGB(180,180,180));
		m_dcTracker.FillSolidRect(0,0,m_rect.Width(),m_rect.Height(),RGB(180,180,180));
 	}
	SCROLLINFO si;
	si.cbSize	= sizeof(si);
	si.fMask	= SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;
	si.nMin		= 0;
	si.nMax		= m_rcRealRect.Height();
	si.nPos		= 0;
	si.nPage	= rs.Height();
	si.nMax<m_rect.Height()?CoolSB_ShowScrollBar(m_hWnd,SB_VERT,FALSE):CoolSB_SetScrollInfo (m_hWnd, SB_VERT, &si, TRUE);
	//CoolSB_SetScrollRange(m_hWnd,SB_VERT,0,m_rcRealRect.Height(),TRUE);
	// 保存滚动条位置
	int iPos = CoolSB_GetScrollPos(m_hWnd,SB_VERT);
	CoolSB_SetScrollPos(m_hWnd,SB_VERT,iPos,TRUE);
}

void CLocalMusicCoverList::SetScrollBitmap(CDibBitmap* pBitmapVert,CDibBitmap* pBitmapHorz)
{
	m_pBitmapScrollHorz = pBitmapHorz;
	m_pBitmapScrollVert = pBitmapVert;

	int w,h;
	w = h = 15;
	if( m_pBitmapScrollHorz && m_pBitmapScrollVert )
	{
		h = m_pBitmapScrollVert->GetHeight() / 3;
		w = m_pBitmapScrollVert->GetWidth() / 6;
	}
	CoolSB_SetSize(m_hWnd, SB_BOTH, h, w);
	::CoolSB_SetMinThumbSize(m_hWnd, SB_BOTH, 8);

}

GraphicsPath* CLocalMusicCoverList::MakeRoundRect(Point topLeft, Point bottomRight, INT percentageRounded)
{
	ASSERT (percentageRounded >= 1 && percentageRounded <= 100);

	INT left  = min(topLeft.X, bottomRight.X);
	INT right = max(topLeft.X, bottomRight.X);

	INT top    = min(topLeft.Y, bottomRight.Y);
	INT bottom = max(topLeft.Y, bottomRight.Y);

	INT offsetX = (right-left)*percentageRounded/100;  
	INT offsetY = (bottom-top)*percentageRounded/100;

	GraphicsPath pt;
	GraphicsPath * path = pt.Clone();

	path->AddArc(right-offsetX, top, offsetX, offsetY, 270, 90);
	path->AddArc(right-offsetX, bottom-offsetY, offsetX, offsetY, 0, 90);
	path->AddArc(left, bottom - offsetY, offsetX, offsetY, 90, 90);
	path->AddArc(left, top, offsetX, offsetY, 180, 90);

	path->AddLine(left + offsetX, top, right - offsetX/2, top);

	return path;
}

void CLocalMusicCoverList::UnSelectAll()
{
	for(int i=0;i<m_DataMgr.m_vItem.size();i++)
		m_DataMgr.m_vItem[i].blSel = FALSE;
}

void CLocalMusicCoverList::CalcHideItemRect(CPoint ptS, LMC_ItemInfo& lmci)
{
	int iSpaceforline = 6;
	lmci.rc.left = ptS.x+iSpaceforline/2;
	lmci.rc.top = ptS.y+iSpaceforline/2;
	lmci.rc.right = ptS.x+m_iItemWidth-iSpaceforline/2;
	lmci.rc.bottom = ptS.y+m_iItemHeight-iSpaceforline/2;
}

void CLocalMusicCoverList::DrawItem(CPoint ptS, LMC_ItemInfo& lmci)
{
	int iSpaceforline = 6;
	// 绘制图片
	USES_CONVERSION;
	Image img(A2W(lmci.strImgPath.GetBuffer()));
	Graphics gc(m_dcMem.GetSafeHdc());
	Rect rcImgDesRect(ptS.x, ptS.y,m_iItemWidth-3,m_iItemHeight-2);
	Rect rcImgCover(rcImgDesRect.X+2,rcImgDesRect.Y+2,rcImgDesRect.Width-5,rcImgDesRect.Height-5);
	gc.DrawImage(&img, rcImgCover);
	gc.DrawImage(pImageFrame, rcImgDesRect);
	// 设置Item区域大小
	lmci.rc.left = ptS.x+iSpaceforline/2;
	lmci.rc.top = ptS.y+iSpaceforline/2;
	lmci.rc.right = ptS.x+m_iItemWidth-iSpaceforline/2;
	lmci.rc.bottom = ptS.y+m_iItemHeight-iSpaceforline/2;
	// 绘制选中框
	if (lmci.blSel)
	{
		Graphics   graphics(m_dcMem.GetSafeHdc()); 
		graphics.SetSmoothingMode(SmoothingModeHighQuality );
		int dis = 10;
		int ipy=8;
		//Pen grpen(Color(57,137,194),3);
		COLORREF col = AfxGetUIManager()->UIGetSkinMgr()->GetColor("PlayedGameFrameBorder");
		Color colPen;
		colPen.SetFromCOLORREF( col );
		Pen grpen(colPen,3);
		GraphicsPath * pPath = MakeRoundRect(Point(ptS.x+2,ptS.y+2),Point(ptS.x+m_iItemWidth-6, ptS.y+m_iItemHeight-5), 10);
		graphics.DrawPath(&grpen,pPath);
	}
	// 绘制文本
	CFont *pOldFont = m_dcMem.GetCurrentFont();
	COLORREF oldText = m_dcMem.GetTextColor();
	int iBK = m_dcMem.GetBkMode();
	m_dcMem.SelectObject(m_fName);
	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.SetTextColor(m_gdires.colName);
	CRect rcText;
	rcText.left = ptS.x-10;
	rcText.top = ptS.y+(m_iItemHeight-iSpaceforline/2)+SPACEH;
	rcText.right = ptS.x+m_iItemWidth+10;
	rcText.bottom = rcText.top+ITEMTXTH;
	if (lmci.strItemName.GetBuffer())// err
		m_dcMem.DrawText(lmci.strItemName,&rcText,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);
	if (m_ImgState==LMCI_Big||m_ImgState==LMCI_Middle)
	{
		CRect rcdetailText;
		rcdetailText.left = ptS.x;
		rcdetailText.top = rcText.bottom+SPACEH;
		rcdetailText.right = ptS.x+m_iItemWidth;
		rcdetailText.bottom = rcdetailText.top+ITEMDETAILTXTH;
		m_dcMem.SelectObject(&m_fDeInfo);
		m_dcMem.SetTextColor(m_gdires.colDetail);
		m_dcMem.DrawText(lmci.strItemdDetailinfo,&rcdetailText,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);
	}
	m_dcMem.SelectObject(pOldFont);
	m_dcMem.SetTextColor(oldText);
	m_dcMem.SetBkMode(iBK);
}

void CLocalMusicCoverList::MoveVWnd(BOOL blIsup)
{
	CPoint pt = m_ptClientStart;
	if (m_blTracker)// Tracker状态下向下滚动的速度大一些
		blIsup?pt.y+=VSCROLLMAX:pt.y-=VSCROLLMAX;
	else
		blIsup?pt.y+=VSCROLLMIN:pt.y-=VSCROLLMIN;
	
	if (pt.y>=m_rect.top)
		pt.y=m_rect.top;
	else if(pt.y+m_rcRealRect.Height()<= m_rect.bottom)
		pt.y = m_rect.Height() - m_rcRealRect.Height()-SPACEH;

	m_csLockV.Lock();
	m_vPtQueue.push_back(pt);
	m_csLockV.Unlock();
	
}

void CLocalMusicCoverList::OnCalcSelectItem()
{
	for (int i=0;i<m_DataMgr.m_vItem.size();i++)
	{	
		CRect rectInter;
		m_DataMgr.m_vItem[i].blSel = rectInter.IntersectRect(&m_DataMgr.m_vItem[i].rc, &m_rcTracker);
	}
}

BOOL CLocalMusicCoverList::HitTest(int& itemIndex,CPoint pt)
{
	for (int i=0;i<m_DataMgr.m_vItem.size();i++)
	{
		if (m_DataMgr.m_vItem[i].rc.PtInRect(pt))
		{
			itemIndex = m_DataMgr.m_vItem[i].iIndex;
			return TRUE;
		}
	}
	return FALSE;
}

void CLocalMusicCoverList::OnTimer(UINT_PTR nIDEvent)
{
	if (m_vScrollTimer == nIDEvent)
	{
		m_csLockV.Lock();
		if(m_vPtQueue.size()>0)
		{
			m_ptClientStart = m_vPtQueue.front();
			m_vPtQueue.pop_front();
			OnMemoryDraw();
		}
		m_csLockV.Unlock();
	}
	if (m_vScrDownTimer == nIDEvent)
		MoveVWnd(FALSE);

	if (m_vScrUpTimer == nIDEvent)
		MoveVWnd(TRUE);

	CWnd::OnTimer(nIDEvent);
}

BOOL CLocalMusicCoverList::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_rcRealRect.Height()>m_rect.Height())
		MoveVWnd(zDelta >= 0);
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

LRESULT CLocalMusicCoverList::OnTrackerChange(WPARAM wParam, LPARAM lParam)
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	if (pt.y>0 && pt.y<10)
		MoveVWnd(TRUE);
	else if (pt.y>m_rect.bottom-10 && pt.y<m_rect.bottom)
		MoveVWnd(FALSE);	
	return 0;
}

void CLocalMusicCoverList::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_rcRealRect.Height()>m_rect.Height() && m_rcTracker.bottom>m_rect.bottom && point.y>m_rect.bottom)
	{
		MoveVWnd(FALSE);
		return;
	}
	else
		KillTimer(m_vScrDownTimer);

	if (m_rcRealRect.Height()>m_rect.Height() &&m_rcTracker.top<m_rect.top&&point.y<m_rect.top)
	{
		MoveVWnd(TRUE);
		return;
	}
	else
		KillTimer(m_vScrUpTimer);
	if (m_blTracker)
	{
		static long g_idrag=0;
		g_idrag++;
		if (g_idrag%2==0)
			OnMemoryDraw();
	}
	// 开始拖拽
	if ((nFlags&MK_LBUTTON) && m_blIsJudgeDrag)
	{
		int iItem;
		if (HitTest(iItem,point))
		{
			m_blIsJudgeDrag = FALSE;
			m_blDrag = TRUE;
		}
	}
	if (m_blDrag)
		OnMoveDraging();
	CWnd::OnMouseMove(nFlags, point);
}

void CLocalMusicCoverList::OnMoveDraging()
{
}

void CLocalMusicCoverList::OnEndDrag()
{
}

void CLocalMusicCoverList::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	int iIndex;
	if (HitTest(iIndex,point))
	{// 点击区域有数据
		m_blIsJudgeDrag = TRUE;
		if (nFlags & MK_CONTROL)
		{
			BOOL blsel = m_DataMgr.m_vItem[iIndex].blSel;
			m_iPreSel = -1;
			m_DataMgr.m_vItem[iIndex].blSel = !blsel;
		}
		else
		{
			vector<int> vSel = GetSelectItem();
			if (find(vSel.begin(),vSel.end(),iIndex)!=vSel.end())
			{
				CPoint pt = point;
			}
			else
			{
				CLMCDataMgr::iterLMCItem it = m_DataMgr.m_vItem.begin();
				CLMCDataMgr::iterLMCItem itend = m_DataMgr.m_vItem.end();
				for (;it!=itend;it++)
					it->iIndex==iIndex?it->blSel=TRUE:it->blSel=FALSE;
			}
		}
	}
	else
	{// 点击区域无数据
		if (nFlags & MK_CONTROL)
		{
		}
		else
		{
			SetCapture();
			m_blTracker = TRUE;
			m_iDistance = point.y-m_ptClientStart.y;
			m_ptDown = point;
		}
	}
	OnMemoryDraw();
	CWnd::OnLButtonDown(nFlags, point);
}

void CLocalMusicCoverList::OnLButtonUp(UINT nFlags, CPoint point)
{

	//==================框选处理==============
	if (m_blTracker)
	{
		ReleaseCapture();
		m_rcTracker.SetRect(0,0,0,0);
		m_iDistance = 0;
		m_blTracker = FALSE;
		OnMemoryDraw();
		KillTimer(m_vScrDownTimer);
		return;
	}
	//End框选处理

	//==================拖拽处理==============
	if (m_blDrag)
	{
		m_blDrag = FALSE;
		CImageList::DragLeave(NULL);
		CImageList::EndDrag();
		ReleaseCapture();
		OnMemoryDraw();
		OnEndDrag();
	}
	else
	{
		if (nFlags&MK_CONTROL)
		{
		}
		else
		{
			UnSelectAll();
			int iIndex;
			if (HitTest(iIndex,point))
				m_DataMgr.m_vItem[iIndex].blSel = TRUE;	
			OnMemoryDraw();
		}
	}
	m_blIsJudgeDrag = FALSE;
	//End拖拽处理

	//===============全部取消选择=============
	int iItem;
	if (!HitTest(iItem,point)&&!m_blDrag)
	{
		CLMCDataMgr::iterLMCItem it = m_DataMgr.m_vItem.begin();
		CLMCDataMgr::iterLMCItem itend = m_DataMgr.m_vItem.end();
		for (;it!=itend;it++)
			it->blSel=FALSE;


	}
	//End全部取消选择
	CWnd::OnLButtonUp(nFlags, point);
}

void CLocalMusicCoverList::OnRButtonUp(UINT nFlags, CPoint point)
{	
	int iItem;
	if (HitTest(iItem,point))
	{
		// 右键区域有内容
		vector<int> vSel = GetSelectItem();
		if (find(vSel.begin(),vSel.end(),iItem)!=vSel.end())
			return;
		else
		{
			UnSelectAll();
			m_DataMgr.m_vItem[iItem].blSel = TRUE;
		}
	}
	else		
		UnSelectAll();// 右键区域无内容
	OnMemoryDraw();
	CWnd::OnRButtonUp(nFlags, point);
}

void CLocalMusicCoverList::OnCoolSBCustomdraw(NMHDR* pNMHDR, LRESULT* pResult )
{
	NMCSBCUSTOMDRAW* pNMCD = reinterpret_cast<NMCSBCUSTOMDRAW*>(pNMHDR);	
	if( m_pBitmapScrollHorz != NULL && m_pBitmapScrollVert != NULL )
		*pResult = HandleCustomDraw(0, (NMCSBCUSTOMDRAW*)pNMHDR, m_pBitmapScrollVert,m_pBitmapScrollHorz );
}

void CLocalMusicCoverList::OnDestroy()
{
	CWnd::OnDestroy();
	if( m_bInit )
		UninitializeCoolSB( m_hWnd );
}

void CLocalMusicCoverList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nCurrpos = CoolSB_GetScrollPos(m_hWnd,SB_VERT);
	int lenVbar = (float)m_rect.Height()/m_rcRealRect.Height()*m_rect.Height();// 滚动条长度
	int realhight = (float)(m_rect.Height()-lenVbar)/m_rect.Height()*m_rcRealRect.Height();// 真实长度-滚动条长度
	switch(nSBCode)
	{
	case SB_LINEUP:
		nCurrpos-=realhight/10;
		if (nCurrpos<0) nCurrpos=0;
		break;
	case SB_LINEDOWN:
		nCurrpos+=realhight/10;
		if(nCurrpos>realhight) nCurrpos=realhight;
		break;
	case SB_PAGEUP:
		nCurrpos-=realhight/10;
		if (nCurrpos<0) nCurrpos=0;
		break;
		break;
	case SB_PAGEDOWN:
		nCurrpos+=realhight/10;
		if(nCurrpos>realhight) nCurrpos=realhight;
		break;
		break;
	case SB_TOP:
		break;
	case SB_BOTTOM:
		break;
	case SB_THUMBTRACK:
		nCurrpos=nPos;
		break;
	}
	
	CPoint pt=m_ptClientStart;
	pt.y = nCurrpos*(m_rcRealRect.Height()-m_rect.Height())/realhight;
	pt.y=0-pt.y;
	m_csLockV.Lock();
	m_vPtQueue.push_back(pt);
	m_csLockV.Unlock();
	CoolSB_SetScrollPos(m_hWnd,SB_VERT,nCurrpos,TRUE);
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CLocalMusicCoverList::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);
	m_PaintScrollBar = bShow;
}

void CLocalMusicCoverList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( ( GetKeyState(VK_CONTROL) & 0xF0000000) &&
		( nChar=='A' || nChar=='a') )
	{
		for (int i=0;i<m_DataMgr.m_vItem.size();i++)
			m_DataMgr.m_vItem[i].blSel = TRUE;
		OnMemoryDraw();
		return;
	}
}

void CLocalMusicCoverList::EnsureVisible(int iIndex)
{
	CPoint pt=m_ptClientStart;
	if (m_DataMgr.m_vItem[iIndex].rc.top>m_rect.bottom)
		pt.y = pt.y-(m_DataMgr.m_vItem[iIndex].rc.top-m_rect.bottom+m_iItemHeight)-20;//20 temp
	if(m_DataMgr.m_vItem[iIndex].rc.bottom<m_rect.top)
		pt.y = pt.y+(m_rect.top-m_DataMgr.m_vItem[iIndex].rc.bottom)+m_iItemHeight;
	m_DataMgr.m_vItem[iIndex].blSel = TRUE;
	m_csLockV.Lock();
	m_vPtQueue.push_back(pt);
	m_csLockV.Unlock();
}
BOOL CLocalMusicCoverList::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
	{
		if (pMsg->wParam==VK_LEFT)
		{
			vector<int> vIndex = GetSelectItem();
			if(vIndex.size()==0) 
			{
				m_DataMgr.m_vItem[0].blSel = TRUE;
				EnsureVisible(0);
				return TRUE;
			}
			if (vIndex.size()>1)
			{
				UnSelectAll();
				m_DataMgr.m_vItem[vIndex[0]].blSel = TRUE;
				EnsureVisible(vIndex[0]);
			}
			else
			{
				if (vIndex[0]>0)
				{
					UnSelectAll();
					int isel=--vIndex[0];
					m_DataMgr.m_vItem[isel].blSel=TRUE;
					EnsureVisible(isel);
				}
			}
			//SetFocus();
			return 1;
		}
		if (pMsg->wParam==VK_RIGHT)
		{
			vector<int> vIndex = GetSelectItem();
			if(vIndex.size()==0) 
			{
				m_DataMgr.m_vItem[0].blSel = TRUE;
				EnsureVisible(0);
				return TRUE;
			}
			if (vIndex.size()>1)
			{
				UnSelectAll();
				int isel=*(--vIndex.end());
				m_DataMgr.m_vItem[isel].blSel=TRUE;
				EnsureVisible(isel);
			}
			else if(vIndex.size()==1)
			{
				if (*(--vIndex.end())<m_DataMgr.m_vItem.size()-1)
				{
					UnSelectAll();
					int isel=++*(--vIndex.end());
					m_DataMgr.m_vItem[isel].blSel=TRUE;
					EnsureVisible(isel);
				}
			}
			//SetFocus();
			return TRUE;
		}
		else if (pMsg->wParam==VK_UP)
		{
			vector<int> vIndex = GetSelectItem();
			if(vIndex.size()==0) 
			{
				m_DataMgr.m_vItem[0].blSel = TRUE;
				EnsureVisible(0);
				return CWnd::PreTranslateMessage(pMsg);
			}
			if (vIndex.size()>1)
			{
				UnSelectAll();
				m_DataMgr.m_vItem[vIndex[0]].blSel=TRUE;
				EnsureVisible(vIndex[0]);
			}
			else if(vIndex.size()==1)
			{
				UnSelectAll();
				int iSel=vIndex[0]-m_rect.Width()/(m_iItemWidth+SPACEW);
				if (iSel<0) iSel=0;
				m_DataMgr.m_vItem[iSel].blSel = TRUE;
				EnsureVisible(iSel);
			}
			//SetFocus();
			return TRUE;
		}
		else if (pMsg->wParam==VK_DOWN)
		{
			vector<int> vIndex = GetSelectItem();
			if(vIndex.size()==0) 
			{
				m_DataMgr.m_vItem[0].blSel = TRUE;
				EnsureVisible(0);
				return CWnd::PreTranslateMessage(pMsg);
			}
			if (vIndex.size()>1)
			{
				UnSelectAll();
				m_DataMgr.m_vItem[vIndex[0]].blSel=TRUE;
				EnsureVisible(vIndex[0]);
			}
			else if(vIndex.size()==1)
			{
				UnSelectAll();
				int iSel=vIndex[0]+m_rect.Width()/(m_iItemWidth+SPACEW);
				if (iSel>=m_DataMgr.m_vItem.size())
				{
					iSel=m_DataMgr.m_vItem.size()-1;
				}
				m_DataMgr.m_vItem[iSel].blSel = TRUE;
				EnsureVisible(iSel);
			}
			//SetFocus();
			return TRUE;
		}
	}	
	return CWnd::PreTranslateMessage(pMsg);
}

void CLocalMusicCoverList::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
}

void CLocalMusicCoverList::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);
}

void CLocalMusicCoverList::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnRButtonDown(nFlags, point);
}