#include "stdafx.h"
#include "GameInfoWnd.h"
#include "../../gui/util/CBufferDC.h"

IMPLEMENT_DYNAMIC(CGameInfoWnd, CBasicWnd)
CGameInfoWnd::CGameInfoWnd()
{
	m_font.CreatePointFont(90,_T("����"));	
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
	m_strGameInfo = "x ָ����СCRect������ұߵĵ�λ��";
	m_strGameInfo += "y ָ����СCRect���ϡ��±ߵĵ�λ��";
	m_strGameInfo += "size ָ����СCRect�ĵ�λ����SIZE��CSize��";
	m_strGameInfo += "cx ֵָ����С���ұߵĵ�λ����";
	m_strGameInfo += "cy ָ����С�ϡ��±ߵĵ�λ����";
	m_strGameInfo += "lpRect ָ��һ��RECT�ṹ��CRect��ָ����Сÿһ�ߵĵ�λ����";
	m_strGameInfo += "l ָ����СCRect��ߵĵ�λ����";
	m_strGameInfo += "t ָ����СCRect�ϱߵĵ�λ����";
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
