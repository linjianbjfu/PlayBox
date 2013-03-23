#include "stdafx.h"
#include "WebWnd.h"
#include "../../Gui/CommonControl/xSkinButton.h"
#include "../WebInteract/MyWebBrowserWnd.h"

IMPLEMENT_DYNAMIC(CWebDlg, CDialog)
CWebDlg::CWebDlg(CWnd* pParent /*=NULL*/)
: CDialog(CWebDlg::IDD, pParent)
{
	m_pBtnClose = new CxSkinButton;
	m_pWebBrowser = new MyWebBrowserWnd(FALSE);
}
CWebDlg::~CWebDlg()
{
	delete m_pBtnClose;
	// delete m_pWebBrowser;
}

BEGIN_MESSAGE_MAP(CWebDlg, CDialog)
	ON_BN_CLICKED(ID_BNT_WEB_WND_CLOSE, OnBnClose)
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

void CWebDlg::OnBnClose()
{
	CDialog::OnCancel();
}
BOOL CWebDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rctNull(0, 0, 0, 0);
	m_pBtnClose->Create(NULL, NULL, rctNull, this, ID_BNT_WEB_WND_CLOSE);
	m_pWebBrowser->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN, rctNull,this,ID_WND_FLASHGAME_RIGHT );

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->RegisterCtrl(this,"btnClose",m_pBtnClose);
	pLayoutMgr->RegisterCtrl(this, "WebBrowser", m_pWebBrowser);

	SetWindowPos(NULL, 0, 0, m_sizeWnd.cx, m_sizeWnd.cy, SWP_NOMOVE);

	pLayoutMgr->CreateControlPane( this,"WebWnd","normal");
	pLayoutMgr->CreateBmpPane( this,"WebWnd","normal" );

	m_pWebBrowser->Navigate(m_strUrl.GetBuffer(0));
	m_strUrl.ReleaseBuffer();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CWebDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	AfxGetUIManager()->UIGetLayoutMgr()->PaintBkGround( m_hWnd ,&dc,false ) ;

	if (m_strTitle.GetLength() > 0)
	{
		CRect rctWnd;
		GetWindowRect(&rctWnd);

		ScreenToClient(&rctWnd);
		rctWnd.bottom = rctWnd.top + 30;
		rctWnd.left += 8;
		rctWnd.right-= 8;

		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(RGB(0xFF, 0xFF, 0xFF));
		dc.DrawText(m_strTitle, rctWnd, DT_SINGLELINE|DT_VCENTER);
	}
}

LRESULT CWebDlg::DoModal(CString strTitle, CString strUrl, int nPageWidth, int nPageHeight)
{
	m_strTitle = strTitle;
	m_strUrl = strUrl;

	m_sizeWnd.cx = nPageWidth+2;
	m_sizeWnd.cy = nPageHeight+2+30;

	return __super::DoModal();
}

LRESULT CWebDlg::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	rc.bottom=rc.top+30;
	return rc.PtInRect(point)?HTCAPTION:CDialog::OnNcHitTest(point);
}