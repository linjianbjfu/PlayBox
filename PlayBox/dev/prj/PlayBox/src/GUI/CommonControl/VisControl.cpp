#include "StdAfx.h"
#include ".\viscontrol.h"
#include "../Player/KWPlayer/IMeidaPluginManagerFactory.h"


CVisControl::CVisControl(void)
: m_Top(0)
, m_Left(0)
, m_Width(0)
, m_Height(0)
, m_BmpName("")
{
}

CVisControl::~CVisControl(void)
{
}

void CVisControl::ILayoutChangeOb_ColorChanging()
{
	m_visDib.pDibBack = NULL;
	m_visDib.pDibBar = NULL;
}
void CVisControl::LoadSkin()
{
	ISkinMgr* pSkinMgr= AfxGetUIManager()->UIGetSkinMgr();
	CString path = pSkinMgr->GetSkinPath();

	path += "\\vis.cfg";
	IMediaPluginConfig* config = GetMeidaPluginManagerFactory()->GetMediaPluginConfig();
	config->SetCurrentVISPluginConfig(path);

	m_visDib.pDibBack = AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp(m_BmpName.c_str());
	m_visDib.pDibBar = AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("VisViewBar");
	SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)&m_visDib);
}

void CVisControl::SetConfig(int left, int top,int width, int height, const char* bmpName)
{
	m_Width = width;
	m_Height = height;
	m_BmpName = bmpName;

	SetWindowPos(NULL, 0 ,0, width, height, SWP_NOMOVE);
}

BOOL CVisControl::Create(LPCTSTR lpszText, DWORD dwStyle,const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if(CStatic::Create(lpszText, dwStyle,rect, pParentWnd, nID))
	{
		AfxGetMessageManager()->AttachMessage( KWID_MESSAGE_LAYOUTMGR,(ILayoutChangeObserver*) this);

		return true;
	}
	return false;
}

BEGIN_MESSAGE_MAP(CVisControl, CStatic)
ON_WM_PAINT()
END_MESSAGE_MAP()


void CVisControl::OnPaint()
{
	CPaintDC dc(this);

	CDibBitmap* dib = AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp(m_BmpName.c_str());

	if (dib)
	{
		dib->SetCDibRect(CRect(0,0,m_Width,m_Height));
		dib->Draw(&dc,FALSE);
	}
}


