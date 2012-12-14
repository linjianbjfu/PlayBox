// DialogMsg.cpp : implementation file
//

#include "stdafx.h"
#include "DialogMsg.h"
#include "../../gui/util/CBufferDC.h"

#define		BTN_WIDTH	67
#define		BTN_HEIGHT	23
#define		BTN_WID_SPACE		10
#define		BTN_HEIGHT_SPACE	10

// CDialogMsg dialog

IMPLEMENT_DYNAMIC(CDialogMsg, CDialog)
CDialogMsg::CDialogMsg(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogMsg::IDD, pParent)
{
	m_strTitle = "";

	SetSize(254,131);
	SetBtnCount(2);
	SetTitle("提示");
	SetBtnText(1,"确 定");
	SetBtnText(2,"取 消");	

	m_bShowCheckBox = FALSE;

	m_titleFont.CreateFont(
		12,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_BOLD,				   // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"宋体");				   // lpszFacename

	m_msgFont.CreateFont(
		12,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"宋体");				   // lpszFacename
}

CDialogMsg::~CDialogMsg()
{
}

void CDialogMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
}


BEGIN_MESSAGE_MAP(CDialogMsg, CDialog)
	ON_BN_CLICKED(IDC_MSGD_BTN_CLOSE, OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_MSGD_BTN_1, OnBnClickedBtn1)
	ON_BN_CLICKED(IDC_MSGD_BTN_2, OnBnClickedBtn2)
	ON_BN_CLICKED(IDC_MSGD_BTN_CHECK,OnBnClickedCheck)
	ON_BN_CLICKED(IDC_MSGD_BTN_UNCHECK,OnBnClickedUnCheck)
	ON_BN_CLICKED(IDOK,OnOK)
	ON_BN_CLICKED(IDCANCEL,OnCancel)
	ON_WM_PAINT()	
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CDialogMsg::SetBtnCount(int iBtnCount)
{
	if( iBtnCount <= 0 )
	{
		m_iBtnCount = 0;
	}
	else if( iBtnCount >= 2 )
	{
		m_iBtnCount = 2;
	}
	else
	{
		m_iBtnCount =1;
	}
}

void CDialogMsg::SetBtnText(int iBtnIdx,const CString& strText)
{		
	if( iBtnIdx == 1 )
	{
		m_strBtnText1 = strText;
	}
	if( iBtnIdx == 2 )
	{
		m_strBtnText2 = strText;
	}
}


// CDialogMsg message handlers

void CDialogMsg::OnBnClickedBtnClose()
{
	OnClickClose();
}

void CDialogMsg::OnBnClickedBtn1()
{
	OnClickBtn1();
}

void CDialogMsg::OnBnClickedBtn2()
{
	OnClickBtn2();
}

void CDialogMsg::OnClickBtn1()
{
	if( ::IsWindowVisible( m_btnCheck.m_hWnd) || ::IsWindowVisible(m_btnUnCheck.m_hWnd))
	{
		CheckBox_Check();
	}
	EndDialog(1);
}

void CDialogMsg::OnClickBtn2()
{
	if( ::IsWindowVisible( m_btnCheck.m_hWnd) || ::IsWindowVisible(m_btnUnCheck.m_hWnd))
	{
		CheckBox_Check();
	}
	EndDialog(0);
}

void CDialogMsg::OnClickClose()
{
	if( ::IsWindowVisible( m_btnCheck.m_hWnd) || ::IsWindowVisible(m_btnUnCheck.m_hWnd))
	{
		CheckBox_Check();
	}
	EndDialog(0);
}

void CDialogMsg::OnOK()
{
	OnClickBtn1();
}

void CDialogMsg::OnCancel()
{
	OnClickClose();
}

void CDialogMsg::OnPaint()
{
	CPaintDC dc(this); 
	CBufferDC* pDC = new CBufferDC(dc);

	ISkinMgr* pSkinMgr = AfxGetUIManager()->UIGetSkinMgr();
	CDibBitmap* pTopLeft		= pSkinMgr->GetDibBmp("MSGDialogLeftTop");
	CDibBitmap* pTopRight		= pSkinMgr->GetDibBmp("MSGDialogRightTop");
	CDibBitmap* pTopMid			= pSkinMgr->GetDibBmp("MSGDialogMidTop");
	CDibBitmap* pCenter			= pSkinMgr->GetDibBmp("MSGDialogMid");
	CDibBitmap* pCenterLeft		= pSkinMgr->GetDibBmp("MSGDialogMidLeft");
	CDibBitmap* pCenterRight	= pSkinMgr->GetDibBmp("MSGDialogMidRight");
	CDibBitmap* pBottomLeft		= pSkinMgr->GetDibBmp("MSGDialogLeftBottom");
	CDibBitmap* pBottomRight	= pSkinMgr->GetDibBmp("MSGDialogRightBottom");
	CDibBitmap* pBottomMid		= pSkinMgr->GetDibBmp("MSGDialogMidBottom");

	CRect rect;
	rect.left	= 0;
	rect.top	= 0;
	rect.right	= pTopLeft->GetWidth();
	rect.bottom = pTopLeft->GetHeight();
	pTopLeft->SetCDibRect(rect);
	pTopLeft->Draw( pDC,TRUE );

	rect.right	= m_iXSize;
	rect.left	= m_iXSize - pTopRight->GetWidth();
	rect.top	= 0;	
	rect.bottom = pTopLeft->GetHeight();
	pTopRight->SetCDibRect(rect);
	pTopRight->Draw( pDC,TRUE );

	rect.right	= m_iXSize-pTopRight->GetWidth();
	rect.left	= pTopLeft->GetWidth();;
	rect.top	= 0;	
	rect.bottom = pTopLeft->GetHeight();
	pTopMid->SetCDibRect(rect);
	pTopMid->Draw( pDC,TRUE );

	rect.left	= 0;
	rect.right  = pBottomLeft->GetWidth();
	rect.bottom	= m_iYSize;
	rect.top	= rect.bottom - pBottomLeft->GetHeight();
	pBottomLeft->SetCDibRect(rect);
	pBottomLeft->Draw( pDC,TRUE );

	rect.left	= m_iXSize-pBottomRight->GetWidth();
	rect.right  = m_iXSize;
	rect.bottom	= m_iYSize;
	rect.top	= rect.bottom - pBottomLeft->GetHeight();
	pBottomRight->SetCDibRect(rect);
	pBottomRight->Draw( pDC,TRUE );

	rect.right	= m_iXSize-pBottomRight->GetWidth();
	rect.left	= pBottomLeft->GetWidth();;
	rect.top	= m_iYSize - pBottomMid->GetHeight();	
	rect.bottom = m_iYSize;
	pBottomMid->SetCDibRect(rect);
	pBottomMid->Draw( pDC,TRUE );

	rect.left	= 0;
	rect.right  = pCenterLeft->GetWidth();
	rect.bottom	= m_iYSize-pBottomLeft->GetHeight();
	rect.top	= pTopLeft->GetHeight();
	pCenterLeft->SetCDibRect(rect);
	pCenterLeft->Draw( pDC,TRUE );

	rect.left	= m_iXSize-pCenterRight->GetWidth();
	rect.right  = m_iXSize;
	rect.bottom	= m_iYSize-pBottomRight->GetHeight();
	rect.top	= pTopRight->GetHeight();
	pCenterRight->SetCDibRect(rect);
	pCenterRight->Draw( pDC,TRUE );

	rect.left	= pCenterLeft->GetWidth();
	rect.right  = m_iXSize-pCenterRight->GetWidth();
	rect.bottom	= m_iYSize-pBottomRight->GetHeight();
	rect.top	= pTopRight->GetHeight();
	pCenter->SetCDibRect(rect);
	pCenter->Draw( pDC,TRUE );

	CFont* pFont = (CFont*)pDC->SelectObject( &m_titleFont );
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(28,68,91));	
	pDC->DrawText(m_strTitle,m_titleRect,DT_LEFT);
	pDC->SelectObject(pFont);


	pFont = (CFont*)pDC->SelectObject( &m_msgFont );
	pDC->SetTextColor(RGB(48,54,54));

	CString str;
	int		iStartPos = 0;
	int		iLastPos  = m_strMsg.Find("\r\n",iStartPos);

	CRect   rect2 = m_msgRect;

	while(  iLastPos != -1)
	{
		str = m_strMsg.Mid(iStartPos,iLastPos-iStartPos);
		pDC->DrawText(str,rect2,DT_LEFT);
		rect2.top += 15;
		iStartPos = iLastPos + 2;
		iLastPos  = m_strMsg.Find("\r\n",iStartPos);
	}

	str = m_strMsg.Right(m_strMsg.GetLength()-iStartPos);

	pDC->DrawText(str,rect2,DT_LEFT);	

	if( m_bShowCheckBox )
	{
		rect2.left = 25+2;
		rect2.top  = 107+2;
		rect2.bottom = 107+30;
		rect2.right  = 25+100;

		SIZE s;
		GetTextExtentPoint(pDC->m_hDC,m_strCheckBox,m_strCheckBox.GetLength(),&s);
		pDC->DrawText(m_strCheckBox,rect2,DT_LEFT);

		m_NoMoreHintRect = rect2;
		m_NoMoreHintRect.right = m_NoMoreHintRect.left + s.cx;
	}

	pDC->SelectObject(pFont);
	PaintSomething( pDC );

	delete pDC;
}

void CDialogMsg::PaintSomething(CDC* pDC )
{
	
}

BOOL CDialogMsg::OnInitDialog()
{
	CDialog::OnInitDialog();	

	SetWindowLong(m_hWnd,GWL_EXSTYLE,GetWindowLong(m_hWnd,GWL_EXSTYLE)^WS_EX_LAYERED);
	SetWindowPos(NULL,0,0,m_iXSize,m_iYSize,SWP_NOMOVE);

	SetLayeredWindowAttributes(RGB(255,0,0),0,LWA_COLORKEY);

	m_titleRect.left = 9;
	m_titleRect.top  = 8;
	m_titleRect.right	= m_iXSize;
	m_titleRect.bottom	= m_iYSize;

	m_msgRect.left	= 25;
	m_msgRect.right	= m_iXSize - 25;
	m_msgRect.top	= 48;
	m_msgRect.bottom = m_iYSize - 35;

	ISkinMgr* pSkinMgr = AfxGetUIManager()->UIGetSkinMgr();

	m_btnClose.Create(NULL,WS_VISIBLE,CRect(m_iXSize-20,5,m_iXSize-5,20),this,IDC_MSGD_BTN_CLOSE );	
	m_btnClose.SetSkin( pSkinMgr->GetDibBmp("MSGDialogCloseNormal"),pSkinMgr->GetDibBmp("MSGDialogCloseDown"),pSkinMgr->GetDibBmp("MSGDialogCloseOver"),NULL,NULL,NULL,0,0,0);

	if( m_iBtnCount == 1)
	{
		if( !m_bShowCheckBox )
		{
			m_btn1.Create(NULL,WS_VISIBLE,CRect(m_iXSize/2-34,m_iYSize-6-23,m_iXSize/2+33,m_iYSize-6),this,IDC_MSGD_BTN_1);
		}
		else
		{
			m_btn1.Create(NULL,WS_VISIBLE,CRect(m_iXSize-67-10,m_iYSize-6-23,m_iXSize-10,m_iYSize-6),this,IDC_MSGD_BTN_1);
		}
		m_btn1.SetWindowText( m_strBtnText1 );		

		m_btn1.SetSkin( pSkinMgr->GetDibBmp("MSGDialogBtn1Normal"),pSkinMgr->GetDibBmp("MSGDialogBtn1Down"),pSkinMgr->GetDibBmp("MSGDialogBtn1Over"),NULL,NULL,NULL,0,0,0);		

	}
	else
	{	
		if( !m_bShowCheckBox )
		{
			m_btn1.Create(NULL,WS_VISIBLE,CRect(m_iXSize/2-3-67,m_iYSize-6-23,m_iXSize/2-3,m_iYSize-6),this,IDC_MSGD_BTN_1);
			m_btn2.Create(NULL,WS_VISIBLE,CRect(m_iXSize/2+3,m_iYSize-6-23,m_iXSize/2+3+67,m_iYSize-6),this,IDC_MSGD_BTN_2);
		}
		else
		{
			m_btn1.Create(NULL,WS_VISIBLE,CRect(m_iXSize-67-67-10-3,m_iYSize-6-23,m_iXSize-67-3-10,m_iYSize-6),this,IDC_MSGD_BTN_1);
			m_btn2.Create(NULL,WS_VISIBLE,CRect(m_iXSize-67-10,m_iYSize-6-23,m_iXSize-10,m_iYSize-6),this,IDC_MSGD_BTN_2);
		}

		m_btn1.SetWindowText( m_strBtnText1 );
		m_btn2.SetWindowText( m_strBtnText2 );

		m_btn1.SetSkin( pSkinMgr->GetDibBmp("MSGDialogBtn1Normal"),pSkinMgr->GetDibBmp("MSGDialogBtn1Down"),pSkinMgr->GetDibBmp("MSGDialogBtn1Over"),NULL,NULL,NULL,0,0,0);
		m_btn2.SetSkin( pSkinMgr->GetDibBmp("MSGDialogBtn2Normal"),pSkinMgr->GetDibBmp("MSGDialogBtn2Down"),pSkinMgr->GetDibBmp("MSGDialogBtn2Over"),NULL,NULL,NULL,0,0,0);		
	}

	if( m_bShowCheckBox )
	{
		m_btnCheck.Create(NULL,NULL,CRect(9,107,9+16,107+16),this,IDC_MSGD_BTN_CHECK);
		m_btnUnCheck.Create(NULL,WS_VISIBLE,CRect(9,107,9+16,107+16),this,IDC_MSGD_BTN_UNCHECK);

		m_btnCheck.SetSkin( pSkinMgr->GetDibBmp("msgminorexitboxchkNormal"),pSkinMgr->GetDibBmp("msgminorexitboxchkNormal"),pSkinMgr->GetDibBmp("msgminorexitboxchkOver"),NULL,NULL,NULL,0,0,0);
		m_btnUnCheck.SetSkin( pSkinMgr->GetDibBmp("msgminorexitboxuchkNormal"),pSkinMgr->GetDibBmp("msgminorexitboxuchkNormal"),pSkinMgr->GetDibBmp("msgminorexitboxuchkOver"),NULL,NULL,NULL,0,0,0);		
	}
		
	CenterWindow();

	return TRUE;  
}

void CDialogMsg::OnLButtonDown(UINT nFlags, CPoint point)
{	
	if( m_NoMoreHintRect.PtInRect( point) )
	{
		if( m_btnCheck.IsWindowVisible() ) 
		{
			m_btnCheck.ShowWindow(SW_HIDE);
			m_btnUnCheck.ShowWindow(SW_SHOW);

			CheckBox_UnChecked();
		}
		else
		{
			m_btnCheck.ShowWindow(SW_SHOW);
			m_btnUnCheck.ShowWindow(SW_HIDE);
			CheckBox_Checked();
		}
	}
	else
	{
		PostMessage( WM_NCLBUTTONDOWN, HTCAPTION, (LPARAM)&point );	
		CDialog::OnLButtonDown(nFlags, point);
	}
}


void CDialogMsg::OnBnClickedCheck()
{
	m_btnCheck.ShowWindow(SW_HIDE);
	m_btnUnCheck.ShowWindow(SW_SHOW);
	CheckBox_UnChecked();
}

void CDialogMsg::OnBnClickedUnCheck()
{
	m_btnCheck.ShowWindow(SW_SHOW);
	m_btnUnCheck.ShowWindow(SW_HIDE);
	CheckBox_Checked();
}

void CDialogMsg::ShowCheckBox(BOOL bShow)
{
	m_bShowCheckBox = bShow;
	if( m_bShowCheckBox )
	{
		SetSize(260,131);
	}
}

void CDialogMsg::SetCheckBoxText(const CString& str)
{
	m_strCheckBox = str;
}