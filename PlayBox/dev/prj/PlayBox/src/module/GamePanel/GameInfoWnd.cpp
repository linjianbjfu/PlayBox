#include "stdafx.h"
#include "GameInfoWnd.h"
#include "../../gui/util/CBufferDC.h"

IMPLEMENT_DYNAMIC(CGameInfoWnd, CBasicWnd)
CGameInfoWnd::CGameInfoWnd()
{
	m_font.CreatePointFont(90,_T("宋体"));	
}

CGameInfoWnd::~CGameInfoWnd()
{
}

BEGIN_MESSAGE_MAP(CGameInfoWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


int CGameInfoWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->CreateControlPane( this,"GameInfo","normal");
	pLayoutMgr->CreateBmpPane( this,"GameInfo","normal" );
	return 0;
}

void CGameInfoWnd::LoadSkin()
{
}

void CGameInfoWnd::SetGameInfo( string& str )
{
	m_strGameInfo = str;
	m_strGameInfo = "x 指定缩小CRect的左和右边的单位数";
	m_strGameInfo += "y 指定缩小CRect的上、下边的单位数";
	m_strGameInfo += "size 指定缩小CRect的单位数的SIZE或CSize；";
	m_strGameInfo += "cx 值指定缩小左、右边的单位数；";
	m_strGameInfo += "cy 指定缩小上、下边的单位数；";
	m_strGameInfo += "lpRect 指向一个RECT结构或CRect，指定缩小每一边的单位数；";
	m_strGameInfo += "l 指定缩小CRect左边的单位数；";
	m_strGameInfo += "t 指定缩小CRect上边的单位数；";
	Invalidate();
}
void CGameInfoWnd::OnPaint()
{
	CPaintDC dc(this);
	AfxGetUIManager()->UIGetLayoutMgr()->PaintBkGround( m_hWnd,&dc,true );

	CRect rc;
	GetClientRect( &rc );
	rc.DeflateRect( 5, 5 );
	dc.SetBkMode( TRANSPARENT );
	dc.SetTextColor( RGB(255,255,255) );
	dc.SelectObject( &m_font );
	dc.DrawText( m_strGameInfo.c_str(), -1, &rc, DT_LEFT|DT_VCENTER );
}
