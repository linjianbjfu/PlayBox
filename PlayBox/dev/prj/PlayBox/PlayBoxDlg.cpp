#include "stdafx.h"
#include "PlayBox.h"
#include "PlayBoxDlg.h"
#include "YL_Base64.h"
#include "tools.h"
#include "update.h"
#include "./src/AppConfig/config/ConfigLayoutDef.h"
#include "./src/AppConfig/config/ConfigAppDef.h"
#include "./src/AppConfig/config/ConfigSettingDef.h"
#include "./src/AppConfig/config/Dlg/DlgConfig.h"
#include "./src/DataInterface/IPanelChangeObserver.h"
#include "./src/module/IrregularBorder/IrregularBorderMgr.h"
#include "./src/module/HttpDown/HttpDownCtrl.h"
#include "./src/util/WebHandler.h"
#include "YL_StringUtil.h"
#include "../layoutmgr/changecolor/IChangeColorMgr.h"
#include "../../Core/CDataManager.h"
#include "./src/module/TabMan/TabWndFactory.h"
#include "./src/module/HTTPControl/HTTPControl.h"
#include "./src/datainterface/IConfUpdateObserver.h"
#include "./src/datainterface/IAdData.h"
//#include "ExceptionHandler.h"
#include "../module/TopPanel/TopPanel_Control.h"
#include "module/UserMan/WebWnd.h"
#include "module/UserMan/UserManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern HWND g_hSharedWnd;
extern BOOL	  g_bExitProgram ;
extern CPlayBoxApp theApp;

#define TIMER_TRIM_MEM		20
#define TIMER_APP_QUIT		22

CPlayBoxDlg::CPlayBoxDlg(bool bStartupRun, CWnd* pParent /*=NULL*/)
	: CDialog(CPlayBoxDlg::IDD, pParent)
{
	m_ptrayIcon = new CTrayIcon(IDI_ICON_BEAR);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_BEAR);
	
	m_bIsResizing = FALSE;
	m_ptStartResizing.x = 0;
	m_ptStartResizing.y = 0;
	m_rectStarResizing.left = 0;
	m_rectStarResizing.right = 0;
	m_rectStarResizing.top = 0;
	m_rectStarResizing.bottom = 0;
	m_iDirection = 0;
	m_dwFrontTime = 0;
	m_dwLastFrontTime = GetTickCount();
	m_bExit = false;
	m_bStartupRun = bStartupRun;
}

void CPlayBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPlayBoxDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()	
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_MEASUREITEM()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDCANCEL,OnCancel)
	ON_BN_CLICKED(IDOK,OnOK)	
	ON_MESSAGE(WM_MY_TRAY_NOTIFICATION, OnTrayNotification)		
	ON_MESSAGE(MSG_SHOW_TRAYICONBALLOON, OnShowTrayIconBalloon)		
	ON_MESSAGE(MSG_DELETE_TRAY, OnDeleteTray)
	ON_MESSAGE(MSG_AFTER_UICREATE,OnAfterUiCreate)
	ON_MESSAGE(MSG_CONF_UPDATE,OnConfUpdate)
	ON_MESSAGE(MSG_CHANGE_TEXT_DLG,OnChangeText)
	ON_MESSAGE(MSG_HTTP_DOWNLOAD,OnHTTPDonwload)
	ON_MESSAGE(MSG_NEW_BROWSER_WND, OnNewBrowserWnd)
	ON_MESSAGE(MSG_OPEN_REG_DIALOG, OnOpenRegDialog)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_WM_SYSCOMMAND()
	ON_WM_SHOWWINDOW()
	ON_WM_QUERYENDSESSION()
	ON_COMMAND(ID_MEMU_SETTING, OnTrayMenuSetting)
	ON_COMMAND(ID_APP_EXIT, OnTrayMenuExit)
	ON_COMMAND(ID_OPEN, OnTrayMenuOpen)
	ON_WM_COPYDATA()
	ON_WM_GETMINMAXINFO()
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
END_MESSAGE_MAP()

void CPlayBoxDlg::OnCancel()
{
}
void CPlayBoxDlg::OnOK()
{
}
void CPlayBoxDlg::DeleteTray()
{
	if(m_ptrayIcon != NULL)
		delete m_ptrayIcon;
	m_ptrayIcon = NULL;
}

void CPlayBoxDlg::OnConfigChange(const string& strKey )
{
	if( strKey == CONF_LAYOUT_NORMAL )
	{
		AfxGetUserConfig()->GetConfigBoolValue( CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_NORMAL,m_bNormal );
	}
}

void CPlayBoxDlg::OnClose()
{
	YL_Log( STR_LOG_FILE ,LOG_NOTICE,"PlayBoxDlg","OnClose" );

	CIrregularBorderMgr::GetInstance()->ShowTransDlg(false);
	if(m_ptrayIcon != NULL)
	{
		delete m_ptrayIcon;
		m_ptrayIcon = NULL;
	}
	CloseHandle(theApp.m_hMutexhandle);
	EndDialog(0);
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_LAYOUTMGR,(ILayoutChangeObserver*) this);
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*)this);
	AfxGetUIManager()->UIManAppExit();
	KillTimer( TIMER_TRIM_MEM );
	if(m_uCloseTimer!=0)
		KillTimer(m_uCloseTimer);
	m_bExit = true;

	HttpDownCtrl::DelInstance();
	TabWndFactory::DelInstance();

	__super::OnClose();
}

LRESULT CPlayBoxDlg::OnDeleteTray(WPARAM wParam, LPARAM lParam)
{
	DeleteTray();
	return -1;
}

// CPlayBoxDlg ��Ϣ�������
HANDLE		g_hStartEvent;

DWORD WINAPI Log_StartMessage(void*)
{
	if( WaitForSingleObject(g_hStartEvent,10*1000) == WAIT_TIMEOUT )
	{
		LogRealMsg("START_TIMEOUT","");
	}
	return 0;
}

BOOL CPlayBoxDlg::OnInitDialog()
{
	DWORD dwStart = GetTickCount();
	AfxSetMainWindow( this );
	g_hStartEvent = CreateEvent(NULL,FALSE,FALSE,NULL );
	CloseHandle( CreateThread(0,0,Log_StartMessage,0,0,0) );

	HANDLE curMutexHandle=CreateMutex(NULL,false,"PBMutex");

	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_pUIManager = AfxGetUIManager();
	bool bUIStart = m_pUIManager->UIManAppStart();
	if( !bUIStart )
	{
		::SendMessage(::AfxGetMainWindow()->m_hWnd, WM_CLOSE, 0, 0);
		return TRUE;
	}
	
	::SetProcessWorkingSetSize(::GetCurrentProcess(), -1, -1);
	::SetTimer(m_hWnd,TIMER_TRIM_MEM,30*1000,NULL);

	GLOBAL_ADDATA;

	AfxUserConfig* pUserConfig = AfxGetUserConfig();
	AfxGetUserConfig()->GetConfigIntValue( CONF_LAYOUT_MODULE_NAME, CONF_LAYOUT_MIN_WIDTH, m_nMinCx );
	AfxGetUserConfig()->GetConfigIntValue( CONF_LAYOUT_MODULE_NAME, CONF_LAYOUT_MIN_HEIGHT, m_nMinCy );

	if(m_nMinCx < 300 || m_nMinCx > 1400)
		m_nMinCx = 650;
	if(m_nMinCy < 300 || m_nMinCy > 1024)
		m_nMinCy = 400;


	g_hSharedWnd = m_hWnd;
	m_ptrayIcon->SetNotificationWnd(this, WM_MY_TRAY_NOTIFICATION);
	SetWindowText("С������Ϸ��");
	_AdjustDlgSize();

	m_ptrayIcon->SetIcon(theApp.LoadIcon(IDI_BEAR),"С������Ϸ��");

	ReleaseMutex(curMutexHandle);

	char szBuffer[32];
	sprintf(szBuffer,"T:%d",GetTickCount() - dwStart);	
	LogUserActMsg( "AppInit",szBuffer );

	//���������ϵ�ϵͳ�˵���Ϊ�Լ�������
	ModifyStyle(0, WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX);

	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_LAYOUTMGR,(ILayoutChangeObserver*) this);
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*) this);
	AfxGetUserConfig()->AttachConfigMessage( CONF_LAYOUT_MODULE_NAME,this );
	YL_Log( STR_LOG_FILE ,LOG_NOTICE,"OnInitDialog","before post MSG_AFTER_UICREATE" );
	SetEvent(g_hStartEvent);

	PostMessage(MSG_AFTER_UICREATE);
	YL_Log( STR_LOG_FILE ,LOG_NOTICE,"OnInitDialog","after post MSG_AFTER_UICREATE" );

	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void CPlayBoxDlg::OnPaint() 
{
	CPaintDC dc(this); // ���ڻ��Ƶ��豸������
	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
	DWORD count = ::GetTickCount();
	m_pUIManager->UIOnPaint( &dc );
}

void CPlayBoxDlg::OnDestroy()
{
	AfxGetUserConfig()->DetachConfigMessage( CONF_LAYOUT_MODULE_NAME,this );
	__super::OnDestroy();
	EndDialog(IDOK);
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CPlayBoxDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
BOOL CPlayBoxDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CPlayBoxDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_bIsResizing )
	{
		DrawMoveFrame(m_rcMain);
		MoveWindow( m_rcMain,true );
		::InvalidateRect(::GetDesktopWindow(), &m_rcMain, true);
	}
	if( GetCapture() == this )
	{
		ReleaseCapture();
	}	
	m_bIsResizing = FALSE;
	return;
}

void CPlayBoxDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_iDirection = 0;

	if( _needChangeCursor(point))
	{
		m_bChangeCursor  = true;
		SetCapture();		
		m_bIsResizing = TRUE;
		m_ptStartResizing = point;
		ClientToScreen( &m_ptStartResizing );
		GetWindowRect( m_rectStarResizing );
		GetWindowRect(&m_rcMain);
		DrawMoveFrame(m_rcMain);
		return ;
	}		
	
	m_bIsResizing = FALSE;

	PostMessage( WM_NCLBUTTONDOWN, HTCAPTION, (LPARAM)&point );	
	CDialog::OnLButtonDown(nFlags, point);
}

void CPlayBoxDlg::OnMouseMove( UINT nFlags, CPoint point )
{
	if( !m_bIsResizing )
	{
		if( !_needChangeCursor( point ) )
		{
			if( m_bChangeCursor )
			{
				HCURSOR  m_hCursor = ::LoadCursor(NULL, IDC_ARROW);			
				::SetCursor(m_hCursor);			
				m_bChangeCursor = false;
			}
		}
		else
		{
			m_bChangeCursor = true;
		}
	}

	if( nFlags & MK_LBUTTON && m_bIsResizing )//���������ڵ��״̬��
	{
		CPoint tmpPT = point;
		ClientToScreen(&tmpPT);
		//�Ȼ�õ�ǰPt �� ��ʼ��� ��λ�õ� ��ֵ
		CPoint diff;
		diff.x = tmpPT.x - m_ptStartResizing.x;
		diff.y = tmpPT.y - m_ptStartResizing.y;

		CRect newRect;

		if( m_iDirection & DIRECTION_LEFT )
		{
			newRect.left = m_rectStarResizing.left + diff.x;
			newRect.right = m_rectStarResizing.right;
			newRect.bottom = m_rectStarResizing.bottom;
			newRect.top = m_rectStarResizing.top;

			if( m_iDirection & DIRECTION_TOP )
			{
				newRect.top += diff.y;
			}
			else if( m_iDirection & DIRECTION_BOTTOM )
			{
				newRect.bottom += diff.y;
			}				
		}
		else if( m_iDirection & DIRECTION_RIGHT )
		{		
			newRect.left = m_rectStarResizing.left;
			newRect.right = m_rectStarResizing.right + diff.x;
			newRect.bottom = m_rectStarResizing.bottom;
			newRect.top = m_rectStarResizing.top;

			if( m_iDirection & DIRECTION_TOP )
			{
				newRect.top += diff.y;
			}
			else if( m_iDirection & DIRECTION_BOTTOM )
			{
				newRect.bottom += diff.y;
			}		
		}
		else if( m_iDirection & DIRECTION_TOP )
		{
			newRect.left = m_rectStarResizing.left;
			newRect.right = m_rectStarResizing.right;
			newRect.bottom = m_rectStarResizing.bottom;
			newRect.top = m_rectStarResizing.top + diff.y;

			if( m_iDirection & DIRECTION_LEFT )
			{
				newRect.left += diff.x;
			}
			else if( m_iDirection & DIRECTION_RIGHT )
			{
				newRect.right += diff.x;
			}				
		}
		else if( m_iDirection & DIRECTION_BOTTOM )
		{
			newRect.left = m_rectStarResizing.left;
			newRect.right = m_rectStarResizing.right;
			newRect.bottom = m_rectStarResizing.bottom + diff.y;
			newRect.top = m_rectStarResizing.top;

			if( m_iDirection & DIRECTION_LEFT )
			{
				newRect.left += diff.x;
			}
			else if( m_iDirection & DIRECTION_RIGHT )
			{
				newRect.right += diff.x;
			}			
		}
		else
		{
			__super::OnMouseMove( nFlags,point );
			return;
		}

		if(newRect.Width() < m_nMinCx)
		{
			if(m_iDirection & DIRECTION_RIGHT)
				newRect.right = newRect.left + m_nMinCx;
			else
				newRect.left = newRect.right - m_nMinCx;
		}
		if(newRect.Height() < m_nMinCy)
		{
			if(m_iDirection & DIRECTION_BOTTOM)
				newRect.bottom = newRect.top + m_nMinCy;
			else
				newRect.top = newRect.bottom - m_nMinCy;
		}
		DrawMoveFrame(m_rcMain);
		m_rcMain = newRect;
		DrawMoveFrame(m_rcMain);
		
		
	}
	__super::OnMouseMove( nFlags,point );
}

BOOL CPlayBoxDlg::_needChangeCursor(const CPoint& Pt )
{
	RECT fullRect;
	::GetWindowRect(m_hWnd,&fullRect);//��õ�ǰ���������������Ļ�ռ��rect			

	//���� cursor ���
	//�ڴ��ڵ� ���½ǵ� 6*6 �������������ͷ��re_size���ڣ�
	if( (Pt.x >= 0 && Pt.x <= 8 ) && (Pt.y > fullRect.bottom - fullRect.top - 8) )
	{
		//����������״�����б��ļ�ͷ
		HCURSOR  m_hCursor = ::LoadCursor(NULL, IDC_SIZENESW);
		::SetCursor(m_hCursor);
		m_iDirection |= DIRECTION_LEFT;
		m_iDirection |= DIRECTION_BOTTOM;

		return TRUE;
	}
	//�ڴ��ڵ� ���½ǵ� 6*6 �������������ͷ��re_size���ڣ�
	if( (Pt.x > fullRect.right - fullRect.left - 8) && (Pt.y > fullRect.bottom - fullRect.top - 8) )
	{
		//����������״�����б��ļ�ͷ
		HCURSOR  m_hCursor = ::LoadCursor(NULL, IDC_SIZENWSE);
		::SetCursor(m_hCursor);
		m_iDirection |= DIRECTION_RIGHT;
		m_iDirection |= DIRECTION_BOTTOM;

		return TRUE;
	}
	//�ڴ��ڵ� ���Ͻǵ� 6*6 �������������ͷ��re_size���ڣ�
	if( (Pt.x >= 0 && Pt.x <= 8 ) && (Pt.y >= 0 && Pt.y <= 8) )
	{
		//����������״�����б��ļ�ͷ
		HCURSOR  m_hCursor = ::LoadCursor(NULL, IDC_SIZENWSE);
		::SetCursor(m_hCursor);
		m_iDirection |= DIRECTION_LEFT;
		m_iDirection |= DIRECTION_TOP;

		return TRUE;
	}
	//�ڴ��ڵ� ���Ͻǵ� 6*6 �������������ͷ��re_size���ڣ�
	if( (Pt.x > fullRect.right - fullRect.left - 8) && (Pt.y >= 0 && Pt.y <= 8))
	{
		//����������״�����б��ļ�ͷ
		HCURSOR  m_hCursor = ::LoadCursor(NULL, IDC_SIZENESW);
		::SetCursor(m_hCursor);
		m_iDirection |= DIRECTION_RIGHT;
		m_iDirection |= DIRECTION_TOP;

		return TRUE;
	}
	//�ڴ��ڵ� �߿������� �� �� 3 �����أ��������ͷ��IDC_SIZEWE ������
	if(Pt.x >= 0 && Pt.x <= 8)
	{
		//����������״�����б��ļ�ͷ
		HCURSOR  m_hCursor = ::LoadCursor(NULL, IDC_SIZEWE);
		::SetCursor(m_hCursor);
		m_iDirection |= DIRECTION_LEFT;
		
		return TRUE;
	}
	else if(Pt.x >= fullRect.right - fullRect.left -8 && Pt.x <= fullRect.right - fullRect.left)
	{
		//����������״�����б��ļ�ͷ
		HCURSOR  m_hCursor = ::LoadCursor(NULL, IDC_SIZEWE);
		::SetCursor(m_hCursor);
		m_iDirection |= DIRECTION_RIGHT;		
		return TRUE;
	}
	//�ڴ��ڵ� �߿������� �� 3�����أ��������ͷ��IDC_SIZEWE ������
	if(Pt.y >= 0 && Pt.y <= 3)
	{	
		//����������״�����б��ļ�ͷ
		HCURSOR  m_hCursor = ::LoadCursor(NULL, IDC_SIZENS);
		::SetCursor(m_hCursor);
		m_iDirection |= DIRECTION_TOP;

		return TRUE;
	}
	else if(Pt.y >= fullRect.bottom - fullRect.top -8 && Pt.y <= fullRect.bottom - fullRect.top)
	{
		//����������״�����б��ļ�ͷ
		HCURSOR  m_hCursor = ::LoadCursor(NULL, IDC_SIZENS);
		::SetCursor(m_hCursor);
		m_iDirection |= DIRECTION_BOTTOM;
		return TRUE;
	}

	return FALSE;
}

void CPlayBoxDlg::ILayoutChangeOb_SkinChanged(string oldSkinPath, string newSkinPath)
{
	int cx, cy;

	CRect rc;

	this->GetWindowRect(&rc);
	cx = rc.Width(); 
	cy = rc.Height();

	if( AfxGetUIManager()->UIGetLayoutMgr() != NULL )
	{
		DWORD count = ::GetTickCount();
		CDC* dc = GetDC();
		HRGN hrgn = AfxGetUIManager()->UIGetSkinMgr()->GetFrameRGN(cx, cy , dc);
		ReleaseDC(dc);
		::SetWindowRgn(this->m_hWnd, hrgn, true);
		AfxGetUIManager()->UIGetLayoutMgr()->UpdateLayout(this->m_hWnd);
		//RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
		// ɾ���������toppanle��ť������������
	}
}


void CPlayBoxDlg::OnSize(UINT nType, int cx, int cy)
{
	if(AfxGetUIManager()->UIGetLayoutMgr() != NULL &&AfxGetUIManager()->UIGetLayoutMgr()->GetDisplayMode() == DISPLAY_SEPERATE_MODE)
	{
		cx = 0;
		cy = 0;
	}
	__super::OnSize(nType, cx, cy);
	if( AfxGetUIManager()->UIGetLayoutMgr() != NULL )
	{
		DWORD count = ::GetTickCount();
		CDC* dc = GetDC();
		HRGN hrgn = AfxGetUIManager()->UIGetSkinMgr()->GetFrameRGN(cx, cy , dc);
		ReleaseDC(dc);
		::SetWindowRgn(this->m_hWnd, hrgn, true);
		AfxGetUIManager()->UIGetLayoutMgr()->UpdateLayout(this->m_hWnd);
		RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
		//TRACE("update layout time: %d\n", ::GetTickCount() - count);
	}
	if(AfxGetMainWindow() != NULL && IsWindow(AfxGetMainWindow()->m_hWnd) && cx != 0 && cy != 0)
		CIrregularBorderMgr::GetInstance()->OnSize();
}

BOOL CPlayBoxDlg::PreTranslateMessage(MSG* pMsg )
{
	UINT uMsg = pMsg->message;
	WPARAM wParam = pMsg->wParam;
	LPARAM lParam = pMsg->lParam;

	if( pMsg->message == WM_KEYDOWN
		&& pMsg->wParam == VK_ESCAPE
		&& GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen())
		GLOBAL_PANELCHANGEDATA->IPanelChange_ExitFullScreen();

	if(pMsg->message == WM_SYSCOMMAND 
		&& pMsg->wParam == SC_CLOSE 
		&& AfxGetUIManager()->UICanExit())
		pMsg->hwnd = m_hWnd;
	return __super::PreTranslateMessage( pMsg );
}

LRESULT CPlayBoxDlg::OnTrayNotification(WPARAM uID, LPARAM lEvent)
{
	return 0;
}

void CPlayBoxDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if(lpMeasureItemStruct->CtlType==ODT_MENU 
		&& IsMenu((HMENU)lpMeasureItemStruct->itemID)
		&& m_ptrayIcon)
		m_ptrayIcon->MeasureItem( lpMeasureItemStruct);

	__super::OnMeasureItem( nIDCtl,lpMeasureItemStruct);
}

void CPlayBoxDlg::OnTimer(UINT nIDEvent)
{
	if(nIDEvent == TIMER_FOR_TRAY)
	{
		if(m_ptrayIcon->m_bRClick)
			m_ptrayIcon->CreatePopup(m_hWnd);
		else
			::SendMessage(m_hWnd, WM_COMMAND, ID_OPEN, 0);
		KillTimer(nIDEvent);
	}
	if( nIDEvent == TIMER_TRIM_MEM )
		::SetProcessWorkingSetSize(::GetCurrentProcess(), -1, -1);

	if( nIDEvent == TIMER_APP_QUIT )
	{
		KillTimer(m_uCloseTimer);
		m_uCloseTimer = 0;
		PostMessage(WM_CLOSE,0,0);
	}
	__super::OnTimer(nIDEvent);
}

void CPlayBoxDlg::DrawMoveFrame(CRect rc)
{
	CWnd* pParentWnd = GetParent();
	CWindowDC dc(pParentWnd);
	CBitmap bmp;

	bmp.LoadBitmap(IDB_FRAME_LINE);
	CBrush brush(&bmp);
	CBrush* pOldBrush = dc.SelectObject(&brush);

	dc.PatBlt(
		rc.left, 
		rc.top, 
		4, 
		rc.Height(), 
		PATINVERT);

	dc.PatBlt(
		rc.left, 
		rc.top, 
		rc.Width(), 
		4, 
		PATINVERT);

	dc.PatBlt(
		rc.left, 
		rc.bottom - 4, 
		rc.Width(), 
		4, 
		PATINVERT);

	dc.PatBlt(
		rc.right, 
		rc.top, 
		4, 
		rc.Height(), 
		PATINVERT);

	dc.SelectObject(pOldBrush);
}

LRESULT CPlayBoxDlg::OnShowTrayIconBalloon(WPARAM wp, LPARAM lp)
{
	CString strTitle = *((CString*)wp);
	CString strContent = *((CString*)lp);
	m_ptrayIcon->ShowTrayIconBalloon( strTitle, strContent, 3000 );
	return 0;
}

void CPlayBoxDlg::_AdjustDlgSize()
{
	AfxGetUserConfig()->GetConfigIntValue( CONF_LAYOUT_MODULE_NAME, CONF_LAYOUT_MIN_WIDTH, m_nMinCx );
	AfxGetUserConfig()->GetConfigIntValue( CONF_LAYOUT_MODULE_NAME, CONF_LAYOUT_MIN_HEIGHT, m_nMinCy );

	if(m_nMinCx < 300 || m_nMinCx > 1400)
		m_nMinCx = 650;
	if(m_nMinCy < 300 || m_nMinCy > 1024)
		m_nMinCy = 400;

	RECT   r;   
	SystemParametersInfo(   SPI_GETWORKAREA,   sizeof(RECT),   &r,   0   );   

	int iDesktopWidth	= r.right - r.left;
	int iDesktopHeight	= r.bottom - r.top;

	int iWndNormalHeight,iWndNormalWidth,iXPos,iYPos;

	AfxGetUserConfig()->GetConfigIntValue( CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_HEIGHT,iWndNormalHeight);
	AfxGetUserConfig()->GetConfigIntValue( CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_WIDTH,iWndNormalWidth);
	AfxGetUserConfig()->GetConfigIntValue( CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_X_POS,iXPos);
	AfxGetUserConfig()->GetConfigIntValue( CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_Y_POS,iYPos);

	iWndNormalHeight	= iWndNormalHeight < m_nMinCy ? m_nMinCy : iWndNormalHeight;
	iWndNormalWidth		= iWndNormalWidth  < m_nMinCx ? m_nMinCx : iWndNormalWidth;

	if( iXPos < 0 || iXPos > iDesktopWidth )
		iXPos = 100;

	if( iYPos < 0 || iYPos > iDesktopHeight )
		iYPos = 100;

	AfxGetUserConfig()->GetConfigBoolValue( CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_NORMAL,m_bNormal );	
	if( m_bNormal)
		::MoveWindow( AfxGetMainWindow()->m_hWnd,iXPos,iYPos,iWndNormalWidth,iWndNormalHeight,TRUE);
	else
		::MoveWindow( AfxGetMainWindow()->m_hWnd,0,0,iDesktopWidth,iDesktopHeight,TRUE);

	bool bHold;
	AfxGetUserConfig()->GetConfigBoolValue( CONF_APP_MODULE_NAME,CONF_APP_MAINWND_HOLD,bHold );
	if( bHold )
		SetWindowPos(&wndTopMost,-1,-1,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
}


void CPlayBoxDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch( nID )
	{
	case SC_MAXIMIZE:
		GLOBAL_PANELCHANGEDATA->IPanelChange_Max();
		break;
	case SC_MINIMIZE:
		GLOBAL_PANELCHANGEDATA->IPanelChange_Min();
		break;
	case SC_RESTORE:
		GLOBAL_PANELCHANGEDATA->IPanelChange_Restore();
		break;
	case SC_CLOSE:
		GLOBAL_PANELCHANGEDATA->IPanelChange_Close();
		break;
	default:
		__super::OnSysCommand(nID, lParam);	
		break;
	}
}

void CPlayBoxDlg::IPanelChangeOb_WindowMax()
{
	ModifyStyle(WS_MAXIMIZEBOX,WS_MINIMIZEBOX);
	CMenu* pMenu = GetSystemMenu(FALSE);
	if(pMenu)
	{
		pMenu->EnableMenuItem(SC_MAXIMIZE,MF_BYCOMMAND|MF_DISABLED);
		pMenu->EnableMenuItem(SC_MINIMIZE,MF_BYCOMMAND|MF_ENABLED);	
	}
}

void CPlayBoxDlg::IPanelChangeOb_WindowMin()
{
	ModifyStyle(WS_MINIMIZEBOX,WS_MAXIMIZEBOX);
	CMenu* pMenu = GetSystemMenu(FALSE);
	if(pMenu)
	{
		pMenu->EnableMenuItem(SC_MAXIMIZE,MF_BYCOMMAND|MF_ENABLED);
		pMenu->EnableMenuItem(SC_MINIMIZE,MF_BYCOMMAND|MF_DISABLED);
	}
}

void CPlayBoxDlg::IPanelChangeOb_WindowRestore()
{
	ModifyStyle(0,WS_MAXIMIZEBOX|WS_MINIMIZEBOX);
	CMenu* pMenu = GetSystemMenu(FALSE);
	if(pMenu)
	{
		pMenu->EnableMenuItem(SC_MAXIMIZE,MF_BYCOMMAND|MF_ENABLED);
		pMenu->EnableMenuItem(SC_MINIMIZE,MF_BYCOMMAND|MF_ENABLED);
	}
}

void CPlayBoxDlg::OnTrayMenuSetting()
{
	CConfigDialog configDlg(0, AfxGetMainWindow());
	configDlg.DoModal();
}

void CPlayBoxDlg::OnTrayMenuOpen()
{
	HWND hMainWnd = m_hWnd;
	if(!m_ptrayIcon->m_bDbClicked)//����
	{
		//if( !IsWindowVisible() )//������ûͼ��ʱʹ��
// 		if( IsWindowVisible() && IsIconic() )//��������ͼ��ʱʹ�ã���ʱ����С��
		if (!IsWindowVisible () ||
			IsWindowVisible () && IsIconic ())
		{
			GLOBAL_PANELCHANGEDATA->IPanelChange_Restore();
			::SetForegroundWindow( GetSafeHwnd() );
		}else
		{
			GLOBAL_PANELCHANGEDATA->IPanelChange_Min();
		}
		//ֻҪ������½����̣��͵���������
		GLOBAL_PANELCHANGEDATA->IPanelChange_Restore();
		::SetForegroundWindow( GetSafeHwnd() );

		m_bStartupRun = false;
	}
}

void CPlayBoxDlg::OnTrayMenuExit()
{
	::PostMessage(m_hWnd,WM_CLOSE,0,0 );
}

void CPlayBoxDlg::OnMove(int x,int y)
{
	if( AfxGetUIManager()->UIGetLayoutMgr() != NULL )
	{
		AfxGetUIManager()->UIOnMove( x,y );
	}
	
	CRect rc;
	GetWindowRect(&rc);
	if(m_hWnd  != NULL && rc.Width() != 0 && rc.Height() != 0)
	{
		AfxSetMainWindow(this);
		CIrregularBorderMgr::GetInstance()->OnMove(x,y);
	}
	__super::OnMove(x,y);
}

void CPlayBoxDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CRect rc;
	GetWindowRect(&rc);

	if(rc.Width() != 0 && rc.Height() != 0)
		CIrregularBorderMgr::GetInstance()->ShowTransDlg(bShow);
	__super::OnShowWindow(bShow, nStatus);
}
BOOL CPlayBoxDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	if (!pCopyDataStruct)
		return FALSE;

	switch(pCopyDataStruct->dwData)
	{
	case WM_PLAYBOX_PROTOCOL:
		OnKooWoProtocol(pCopyDataStruct);
		break;
	}
	return TRUE;
}

void CPlayBoxDlg::OnKooWoProtocol(const COPYDATASTRUCT* pCopyDataStruct)
{
	char* szBuf = new char[strlen( (LPCTSTR)pCopyDataStruct->lpData)+1];
	strcpy(szBuf,(LPCTSTR)pCopyDataStruct->lpData);

	for(int i = 0;i < strlen(szBuf);i++)
	{
		if( szBuf[i] == '\\')
		{
			szBuf[i] = '/';
		}
	}
	char* tmp = (char*)strstr(szBuf, "://");	

	if (tmp == NULL)
		return;

	tmp += 3;
	OnExecWebCmd( tmp );
}


void CPlayBoxDlg::OnExecWebCmd(const char *tmp)
{
	string strParam;
	if (strncmp(tmp, "add", 3) == 0)
	{	
	}
	else if(strncmp(tmp, "search", 6) == 0)
	{
	}
	else if(strncmp(tmp,"refresh",7) == 0 )
	{
		NotifyWebRefresh(tmp);
	}
}

void CPlayBoxDlg::NotifyWebRefresh( const  char* psz)
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_REFRESH,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IWebRefreshObserver* pOb = dynamic_cast<IWebRefreshObserver*>(*itOb);
		pOb->IWebRefreshOb_Refresh( psz);
	}
}


LRESULT CPlayBoxDlg::OnAfterUiCreate(WPARAM,LPARAM)
{
	YL_Log( STR_LOG_FILE ,LOG_NOTICE,"PlayBoxDlg","OnAfterUiCreate==IN" );
	checkPopUpUpdate(&g_hSharedWnd);
	checkUpdate("PlayBox");
	if (m_bStartupRun)
	{
		CTopPanelControl::GetInstance()->DoMin();
	}

	YL_Log( STR_LOG_FILE ,LOG_NOTICE,"PlayBoxDlg","OnAfterUiCreate==OUT" );
	return 0;
}

LRESULT CPlayBoxDlg::OnConfUpdate(WPARAM,LPARAM)
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_CONFUPDATE,listOb);
	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IConfUpdateObserver* pOb = dynamic_cast<IConfUpdateObserver*>(*itOb);
		pOb->IConfUpdate_Update();
	}
	return 0;
}

LRESULT CPlayBoxDlg::OnChangeText(WPARAM wp, LPARAM lp)
{
	if(!IsWindow(m_hWnd))
		return 0;
	try
	{
		CString str = (LPCSTR)wp;
		if(str != "" && str[0] == '-')
			str = str.Mid(1);
		SetWindowText(str);
		m_ptrayIcon->SetIcon(theApp.LoadIcon(IDR_MAINFRAME),(LPCSTR)wp);
	}catch(...){}
	return 0;
}

LRESULT CPlayBoxDlg::OnHTTPDonwload(WPARAM w,LPARAM)
{
	//���Ҳ�ʵ�ָ�����������������
	if( w == 1 )
	{
		CHTTPControl::HTTPFinish_RegID();
	}
	if( w == 2)
	{
		CHTTPControl::HTTPFinish_ConfigFile();
		//ȡ��������֮���پ����Ƿ񵯿�,3.0���Ϊ����ȡ��������֮��,��������Ƶ�OnHTTPDonwload��
		//CMainMsgControl::GetInstance()->ResetPopedMsgNum();
		//::SetTimer(m_hWnd,TIMER_POP_MESSAGE,3000,NULL);
		//���ع��
		//list<IMessageObserver*> listOb;
		//AfxGetMessageManager()->QueryObservers( ID_MESSAGE_CONFUPDATE,listOb);
		//for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
		//{
		//	IConfUpdateObserver* pOb = dynamic_cast<IConfUpdateObserver*>(*itOb);
		//	pOb->IConfUpdate_ConfigFile();
		//}
	}
	if( w == 3)
	{
		CHTTPControl::HTTPFinish_UpClientLog();
	}
	return 0;
}

LRESULT CPlayBoxDlg::OnNewBrowserWnd(WPARAM w,LPARAM l)
{
	TAB_ITEM tabItem;
	tabItem.strTitle = TAB_BROWSER_DEFAULT_TITLE;
	tabItem.enumType = TAB_BROWSER;
	tabItem.iLPDISPATCHOnlyForBrowser = static_cast<int>(w);
	GLOBAL_TABBARDATA->ITabBar_ChangeTab( tabItem );
	return 0L;
}

LRESULT CPlayBoxDlg::OnOpenRegDialog(WPARAM w,LPARAM l)
{
	// ��ȡע���ַ
	string strUrl;
	AfxGetUserConfig()->GetConfigStringValue(CONF_APP_MODULE_NAME, 
		CONF_APP_REGIST_URL, strUrl);
	// ��ȡע��ҳ���С
	CRect rctPageSize(0, 0, 518, 298);
	CWebDlg dlg(AfxGetMainWindow());
	AfxGetUIManager()->UIAddDialog(&dlg);
	CUserManager::GetInstance()->SetRegisterWnd(&dlg);
	dlg.DoModal(_T("ע��"), strUrl.c_str(), rctPageSize.Width(), rctPageSize.Height());
	AfxGetUIManager()->UIRemoveDialog(&dlg);
	return 0L;
}

void CPlayBoxDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: Add your message handler code here and/or call default

	RECT rcWorkArea;
	SystemParametersInfo (SPI_GETWORKAREA, 0, (PVOID)&rcWorkArea, 0);

	if (lpMMI->ptMaxSize.x > rcWorkArea.right)
		lpMMI->ptMaxSize.x = rcWorkArea.right;

	if (lpMMI->ptMaxSize.y > rcWorkArea.bottom)
		lpMMI->ptMaxSize.y = rcWorkArea.bottom;

	__super::OnGetMinMaxInfo(lpMMI);
}

LRESULT CPlayBoxDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_HOTKEY_BOSSKEY:
		{
			BOOL bShowingWnd = IsWindowVisible();
			if (bShowingWnd)
			{
				ShowWindow(SW_HIDE);
				delete m_ptrayIcon;
				m_ptrayIcon = NULL;
			}
			else
			{
				ShowWindow(SW_NORMAL);
				m_ptrayIcon = new CTrayIcon(IDI_BEAR);
				m_ptrayIcon->SetNotificationWnd(this, WM_MY_TRAY_NOTIFICATION);
				m_ptrayIcon->SetIcon(theApp.LoadIcon(IDI_BEAR),"С������Ϸ��");
			}
			
		}
		break;
	}
	return 0;
}