#include "stdafx.h"
#include "BottomAdPanel.h"
#include "tools.h"
#include "YL_StringUtil.h"
#include "../../gui/Bitmap/DibBitmap.h"
#include "YL_FileInfo.h"
#include "../gui/util/CBufferDC.h"

#define _USE_MATH_DEFINES
#include "math.h"
#include "../../AppConfig/config/ConfigAppDef.h"

IMPLEMENT_DYNAMIC(CBottomAdPanel, CWnd)
CBottomAdPanel::CBottomAdPanel() : m_bTrackMouse(false)
{}

CBottomAdPanel::~CBottomAdPanel()
{}

BEGIN_MESSAGE_MAP(CBottomAdPanel, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()	
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

void CBottomAdPanel::OnPaint()
{
	CPaintDC dc(this);
	DrawAd(&dc);
}
//绘制文字 内部函数
void CBottomAdPanel::DrawAd(CDC* dc)
{
}

void CBottomAdPanel::OnMouseMove(UINT nFlags, CPoint point)
{
	if( !m_bTrackMouse )
	{
		m_bTrackMouse = true;
       TRACKMOUSEEVENT tme;
       tme.cbSize = sizeof(tme);
       tme.hwndTrack = m_hWnd;
       tme.dwFlags = TME_LEAVE;
       _TrackMouseEvent(&tme);
	}
	bool bMouseOverAD = false;
	for (vector<InfoAd>::iterator iter = m_vInfoAd.begin(); iter != m_vInfoAd.end(); iter++)
	{
		if (iter->rect.PtInRect(point))
		{
			bMouseOverAD = true;
			break;
		}
	}
	::SetCursor(::LoadCursor(NULL, bMouseOverAD ? IDC_HAND : IDC_ARROW));
	__super::OnMouseMove(nFlags, point);
}

LRESULT CBottomAdPanel::OnMouseLeave(WPARAM w, LPARAM l)
{
	m_bTrackMouse = false;
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));	
	return 0;
}

void CBottomAdPanel::OnLButtonUp(UINT nFlags, CPoint point)
{
	__super::OnLButtonUp( nFlags, point );
	vector<InfoAd>::iterator iter = m_vInfoAd.begin();
	for (; iter != m_vInfoAd.end(); iter++)
	{
		if (iter->rect.PtInRect(point))
			break;
	}
	if(iter == m_vInfoAd.end())
		return;
	//open this web game
#pragma warning ("to do")
}

int CBottomAdPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	ReadADConf();
	m_Font.CreateFont(
			12,                        // nHeight
			0,                         // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			FW_NORMAL,                 // nWeight
			FALSE,                     // bItalic
			TRUE,                     // bUnderline
			0,                         // cStrikeOut
			ANSI_CHARSET,              // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			"宋体");                 // lpszFacename
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_CONFUPDATE, (IConfUpdateObserver*)this );
	return 0;
}

void CBottomAdPanel::OnDestroy()
{
	AfxGetMessageManager ()->DetachMessage (ID_MESSAGE_CONFUPDATE, (IConfUpdateObserver *) this);
}

void CBottomAdPanel::ReadADConf()
{
	vector<string> vec;
	string strADJson;
	AfxGetUserConfig()->GetConfigStringValue(CONF_APP_MODULE_NAME, BOTTOM_AD_JSON, strADJson);
	if (strADJson.empty())
		return;
}

void CBottomAdPanel::IConfUpdate_Update()
{
	ReadADConf();
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
}
