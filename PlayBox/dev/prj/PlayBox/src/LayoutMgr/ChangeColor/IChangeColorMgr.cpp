#include "stdafx.h"
#include "../ISkinMgr.h"
#include ".\IChangeColorMgr.h"
#include "../AppConfig/config/ConfigSettingDef.h"
#include "YL_FileInfo.h"
#include "YL_P2PDllExport.h"
#include "YL_FileZip.h"
#include "DownSubject.h"
#include "./src/module/IrregularBorder/IrregularBorderMgr.h"
#include "../../gui/util/WToolTips.h"
#include "./Gui/CommonControl/DialogMsg.h"

#define SUB_WND_WIDTH				295
#define SUB_WND_HEIGHT				207

#define STR_SKIN_SMALL_PICTURE "\\small.jpg"
#define STR_SKIN_OLD_SMALL_BMP "\\small.bmp"


extern CBitMapOperate gBitMapOperate;
IChangeColorMgr* CSkinMgrV1::mp_IsHadObject = NULL;

CSkinMgrV1::CSkinMgrV1()
{
	mp_mainSkinWnd		= NULL;
	mp_skinWnd			= NULL;

	mp_downloadSkin		= NULL;
	mp_downloadSubject	= NULL;

	m_BaseColor			= RGB( 0,0,0);
	m_nLightPos			= 120;
	mb_HasPopWnd		= false;
	mb_isDownSkin		= false;
	mb_isDownSubject    = false;
	mb_isDownWebSub		= false;
	m_DefaultColor = RGB( 0,0,0);
	mp_SubjectWnd	= NULL;

	m_iCheckTab = 0;
	mb_isFirstPop = TRUE;
	InitializeCriticalSection( &m_csDownInfo );

	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_LAYOUTMGR,(ILayoutChangeObserver*)this);
}
CSkinMgrV1::~CSkinMgrV1()
{

	/*if( NULL != mp_mainSkinWnd ) delete mp_mainSkinWnd;
	if( NULL != mp_skinWnd)        delete mp_skinWnd;
	if( NULL != mp_tabCtrol)       delete mp_tabCtrol;
	if( NULL != mp_colorWnd )      delete mp_colorWnd;
	if( NULL != mp_CloseBotton )   delete mp_CloseBotton;*/
	if( mp_downloadSkin )
	{
		delete mp_downloadSkin;
		mp_downloadSkin = NULL;
	}
	if( mp_downloadSubject )
	{
		delete mp_downloadSubject;
		mp_downloadSubject = NULL;
	}

	DeleteCriticalSection( &m_csDownInfo );
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_LAYOUTMGR,(ILayoutChangeObserver*)this);
}
IChangeColorMgr* CSkinMgrV1::GetObject()
{
	if( NULL == mp_IsHadObject )
	{
		mp_IsHadObject = new CSkinMgrV1();
	}
	return mp_IsHadObject;
}

void CSkinMgrV1::OnClose()
{
	if(mb_HasPopWnd)
	{
		mp_mainSkinWnd->CloseWindows();
		mb_HasPopWnd = FALSE;
		mp_mainSkinWnd = NULL;
	}
}

void CSkinMgrV1::GetSkinWndRect(const POINT& aPoint,CRect& lrect)
{	
	if(AfxGetUIManager()->UIGetLayoutMgr()->GetDisplayMode() == DISPLAY_SEPERATE_MODE)
		mp_mainSkinWnd->m_hight = 211;

	lrect.top    = aPoint.y;
	lrect.bottom = lrect.top + mp_mainSkinWnd->m_hight;
	lrect.left   = aPoint.x;
	if(AfxGetUIManager()->UIGetLayoutMgr()->GetDisplayMode() == DISPLAY_SEPERATE_MODE)
		mp_mainSkinWnd->m_width = 240;
	lrect.right  = lrect.left +mp_mainSkinWnd->m_width;

	int lnScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int lnScreenHight = ::GetSystemMetrics(SM_CYSCREEN);

	if( lnScreenWidth < (int)(mp_mainSkinWnd->m_width + aPoint.x) )
	{
		lrect.left   =    aPoint.x - mp_mainSkinWnd->m_width;
		lrect.right  =    aPoint.x;
	}
	if( lnScreenHight < (int)mp_mainSkinWnd->m_hight + aPoint.y )
	{
		lrect.top =     aPoint.y - mp_mainSkinWnd->m_hight;
		lrect.bottom =  aPoint.y;
	}
}

BOOL CSkinMgrV1::PopSkinWnd(CWnd* apParent, UINT anID, POINT aPoint)
{
	BOOL lbReturn = false;
	BOOL bSepMode = false;
	if(AfxGetUIManager()->UIGetLayoutMgr()->GetDisplayMode() == DISPLAY_SEPERATE_MODE)
	{
		bSepMode = true;
		m_iCheckTab = 1;
	}

	if(!mb_HasPopWnd)
	{
		if(mb_isDownWebSub)
		{
			CDownSub::StopDownWebSubThread();
			mb_isDownWebSub = false;
			mb_isDownSubject = false;
		}

		CRect lrect;
		GetSkinWndRect( aPoint,lrect );
		// main win
		lbReturn = mp_mainSkinWnd->Create( NULL, NULL, NULL , lrect, apParent);
		mp_mainSkinWnd->SetCheckBtn(m_iCheckTab);
		mp_mainSkinWnd->SetListener(this);
		mp_mainSkinWnd->SetWndListener(this);

		mp_mainSkinWnd->ShowWindow(SW_SHOW);
		mp_mainSkinWnd->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
		RECT lClient;
		GetClientRect(mp_mainSkinWnd->m_hWnd, &lClient);
		
		int sub_wnd_w,sub_wnd_h;
		if(bSepMode)
		{
			sub_wnd_w = 234;
			sub_wnd_h = 132;
		}
		else
		{
			sub_wnd_w = SUB_WND_WIDTH;
			sub_wnd_h = SUB_WND_HEIGHT;
		}


		// skin win
		lbReturn =  mp_skinWnd->Create(NULL,NULL, WS_VISIBLE|WS_CHILD,CRect(3,27,3+sub_wnd_w,sub_wnd_h+27),mp_mainSkinWnd);
		mp_skinWnd->InitScrollBar(); 
		mp_skinWnd->SetSendEventListener(this);	
		mp_skinWnd->ShowWindow(bSepMode); 

		// subject win
		lbReturn =  mp_SubjectWnd->Create(NULL,NULL, WS_VISIBLE|WS_CHILD,CRect(3,27,3+sub_wnd_w,sub_wnd_h+27),mp_mainSkinWnd);
		mp_SubjectWnd->InitScrollBar(); 
		mp_SubjectWnd->SetSendEventListener(this);
		mp_SubjectWnd->ShowWindow(SW_HIDE); 

		//web subject win
		lbReturn =  mp_WebSubWnd->Create(NULL,NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,CRect(3,27,sub_wnd_w+3,sub_wnd_h+27),mp_mainSkinWnd,ID_WND_WEBSUBJECT);
		mp_WebSubWnd->Navigate();
		mp_WebSubWnd->ShowWindow(!bSepMode); 

		SubjectData*    pSub = AfxGetUIManager()->UIGetSkinMgr()->GetSubject();
		for(size_t idx = 0 ;idx < pSub->vecSubject.size();idx++ )
		{
			OneSubject& oneSub = pSub->vecSubject[idx];			

			if( oneSub.bNeedDown )
			{
				CString tips = CString("皮肤: ") + oneSub.strShowName.c_str() + "\r\n状态: 需点击下载";
				mp_SubjectWnd->SetTipText( oneSub.strSubName.c_str(),tips,oneSub.strShowName.c_str() );

			}
			else
			{
				CString tips = CString("皮肤: ") + oneSub.strShowName.c_str() + "\r\n状态: 已备好";
				mp_SubjectWnd->SetTipText( oneSub.strSubName.c_str(),tips,oneSub.strShowName.c_str());
			}
		}

	
		switch(m_iCheckTab)
		{
		case 0:
			mp_skinWnd->ShowWindow(SW_HIDE);
			mp_SubjectWnd->ShowWindow(SW_SHOW);
			mp_WebSubWnd->ShowWindow(SW_HIDE);
			m_bShowSkinWnd = FALSE;
			break;
		case 1:
			mp_skinWnd->ShowWindow(SW_SHOW);
			mp_SubjectWnd->ShowWindow(SW_HIDE);
			mp_WebSubWnd->ShowWindow(SW_HIDE);
			m_bShowSkinWnd = TRUE;
			break;
		case 2:
			mp_skinWnd->ShowWindow(SW_HIDE);
			mp_SubjectWnd->ShowWindow(SW_HIDE);
			mp_WebSubWnd->ShowWindow(SW_SHOW);
			m_bShowSkinWnd = FALSE;
			break;		
		}	


		mb_HasPopWnd	= true;
		//m_bShowSkinWnd	= (AfxGetUIManager()->UIGetLayoutMgr()->GetDisplayMode() == DISPLAY_SEPERATE_MODE );		
	}
	else
	{
		CRect lrect;
		GetSkinWndRect( aPoint,lrect );

		mp_mainSkinWnd->MoveWindow(&lrect,true);
		SetWindowPos(mp_mainSkinWnd->m_hWnd,HWND_TOP,0,0,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);
	}
	
	//下载皮肤，关闭换肤窗口后继续下载，若是下载主题关闭再打开则不再下载
	if(!mp_downloadSkin )
	{
		mp_downloadSkin = new CDownloadPercentWnd;
	}
	CRect lClient;
	lClient.SetRect(3,235,297,285);

	if( mp_downloadSkin->m_hWnd == NULL )
	{
		CString ClassName   = ::AfxRegisterWndClass(CS_CLASSDC,::LoadCursor(NULL,IDC_ARROW),NULL,NULL);   
		if( AfxGetUIManager()->UIGetLayoutMgr()->GetDisplayMode() == DISPLAY_SEPERATE_MODE )
		{
			mp_downloadSkin->CreateEx( 0,ClassName,NULL,  WS_CHILD,CRect( lClient.left,lClient.top,lClient.left+220,lClient.top+30),mp_mainSkinWnd,0);		
		}
		else
		{
			mp_downloadSkin->CreateEx( 0,ClassName,NULL,  WS_CHILD,lClient,mp_mainSkinWnd,0);		
		}
	}
		
	if( m_bShowSkinWnd )
	{
		if( mb_isDownSkin )
		{
			mp_downloadSkin->ShowWindow( SW_SHOW );
			//DownloadStart();
		}
		else
		{
			mp_downloadSkin->ShowWindow( SW_HIDE );
			mp_mainSkinWnd->ShowColorBtn( TRUE );
		}
	}	

	if(!mp_downloadSubject )
	{
		mp_downloadSubject = new CDownloadPercentWnd;
	}
	if( mp_downloadSubject->m_hWnd == NULL )
	{
		CString ClassName   = ::AfxRegisterWndClass(CS_CLASSDC,::LoadCursor(NULL,IDC_ARROW),NULL,NULL);   
		mp_downloadSubject->CreateEx( 0,ClassName,NULL,WS_CHILD,lClient,mp_mainSkinWnd,1);		
	}	

	return lbReturn;
}
bool CSkinMgrV1::InsertSmallSkin(string astrName,CBitmap* apBmp,string strPath,bool bHasSkin)
{
	if( !mb_HasPopWnd)
	{
		if(NULL == mp_mainSkinWnd)
		{
			mp_mainSkinWnd	= new CMainSkinWnd;
			mp_skinWnd		= new CSkinWnd;
			mp_SubjectWnd	= new CSubjectChildWnd;
			mp_WebSubWnd	= new CWebSubWnd;
//			m_pToolTip = new CCWToolTips();
		}

		mp_skinWnd->InsertSmallSkin(astrName,apBmp, strPath,bHasSkin);
		return true;
	}
	return false;
}
bool CSkinMgrV1::InsertSmallSubjectSkin(string astrName,CBitmap* apBmp,string strPath,bool bHasSkin)
{
	if( !mb_HasPopWnd)
	{	
		if(NULL == mp_mainSkinWnd)
		{
			mp_mainSkinWnd	= new CMainSkinWnd;
			mp_skinWnd		= new CSkinWnd;
			mp_SubjectWnd	= new CSubjectChildWnd;
			mp_WebSubWnd	= new CWebSubWnd;
//			m_pToolTip = new CCWToolTips();
		}

		mp_SubjectWnd->InsertSmallObjectBmp(astrName,apBmp, strPath,bHasSkin);
		return true;
	}
	return false;
}


void CSkinMgrV1::SetTipText( CString astrName,CString strTip)
{
	if( mp_skinWnd )
		mp_skinWnd->SetTipText( astrName,strTip );
}

DIBINFO* CSkinMgrV1::ChangeBmp(DIBINFO ahSourceDib,UINT aColorGroup, COLORREF aMaskColor) 
{
	return gBitMapOperate.ChangeBmpSkin(ahSourceDib,aColorGroup,m_BaseColor,aMaskColor,m_nLightPos);
}

void  CSkinMgrV1::ChangeColor(COLORREF &aColor)
{
	gBitMapOperate.changeColor(aColor,m_BaseColor,m_nLightPos);
}
void CSkinMgrV1::SetListener( IChangeSkinEventListener* apListtener )
{
	for(size_t idx = 0;idx < mp_changeSkinEven.size();idx++ )
	{
		if( mp_changeSkinEven[idx] == apListtener )
		{
			return;
		}
	}

	mp_changeSkinEven.push_back( apListtener );	
}

void CSkinMgrV1::RemoveListener( IChangeSkinEventListener* apListtener )
{
	for( vector<IChangeSkinEventListener*>::iterator it1 = mp_changeSkinEven.begin(); 
		it1 != mp_changeSkinEven.end(); it1++ )
	{
		if( (*it1) == apListtener )
		{
			mp_changeSkinEven.erase( it1 );
			return;
		}
	}
}

void CSkinMgrV1::OnTabSelChange( UINT anIndex)
{
	if( 1 ==anIndex )
	{
		m_iCheckTab = 1;
		m_bShowSkinWnd = true;
		mp_SubjectWnd->ShowWindow(FALSE);
		mp_skinWnd->ShowWindow(TRUE);
		mp_WebSubWnd->ShowWindow(FALSE);
		mp_skinWnd->UpdateControlsEnable(true);

		mp_downloadSubject->ShowWindow( SW_HIDE );
		if( (mp_downloadSkin) && (mp_downloadSkin->m_hWnd) && mb_isDownSkin)
		{
			if(mp_skinWnd != NULL)
			{
				mp_skinWnd->EnableSkinBtn( FALSE );
				mp_skinWnd->SetUseTip(false);
			}

			mp_downloadSkin->ShowWindow( SW_SHOW );
			mp_mainSkinWnd->ShowColorBtn( FALSE );
		}
		else
		{
			mp_downloadSkin->ShowWindow( SW_HIDE );
			mp_mainSkinWnd->ShowColorBtn( TRUE );
		}
	}
	else if( 0 == anIndex )
	{
		m_bShowSkinWnd = false;
		m_iCheckTab = 0;
		mp_SubjectWnd->ResetScroll();

		mp_SubjectWnd->ShowWindow(TRUE);
		mp_skinWnd->ShowWindow(FALSE);	
		mp_WebSubWnd->ShowWindow(FALSE);
		mp_skinWnd->UpdateControlsEnable(false);
		
		if( ::IsWindowVisible(mp_downloadSkin->m_hWnd ) )
		{
			mp_downloadSkin->ShowWindow( SW_HIDE );
			mp_downloadSkin->Invalidate();
		}
		
		if( (mp_downloadSubject) && (mp_downloadSubject->m_hWnd) && mb_isDownSubject)
		{
			mp_downloadSubject->ShowWindow( SW_SHOW );
			mp_mainSkinWnd->ShowColorBtn( FALSE );
		}
		else
		{
			mp_downloadSubject->ShowWindow( SW_HIDE );
			mp_mainSkinWnd->ShowColorBtn( TRUE );
		}
	}
	else if( 2 == anIndex )//websubwnd
	{
		m_iCheckTab = 2;
		m_bShowSkinWnd = false;
		mp_SubjectWnd->ShowWindow(FALSE);
		mp_skinWnd->ShowWindow(FALSE);	
		mp_WebSubWnd->ShowWindow(TRUE);

		if( ::IsWindowVisible(mp_downloadSkin->m_hWnd ) )
		{
			mp_downloadSkin->ShowWindow( SW_HIDE );
			mp_downloadSkin->Invalidate();
		}

		if( (mp_downloadSubject) && (mp_downloadSubject->m_hWnd) && mb_isDownWebSub)
		{
			mp_downloadSubject->ShowWindow( SW_SHOW );
			mp_mainSkinWnd->ShowColorBtn( FALSE );
		}
		else
		{
			mp_downloadSubject->ShowWindow( SW_HIDE );
			mp_mainSkinWnd->ShowColorBtn( TRUE );
		}
	}
}

void CSkinMgrV1::SetTopMost(bool bTopMost)
{
	if(mp_mainSkinWnd != NULL && ::IsWindow(mp_mainSkinWnd->m_hWnd))
	{
		if(bTopMost)
			SetWindowPos(mp_mainSkinWnd->m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);
		else
			SetWindowPos(mp_mainSkinWnd->m_hWnd,HWND_TOP,0,0,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);
	}
}
void  CSkinMgrV1::DeleteObject()
{
	if( NULL != mp_mainSkinWnd ) 
	{
		mp_mainSkinWnd->DestroyWindow();
	}

	delete this;
}
void CSkinMgrV1::OnSendChangeColor( COLORREF acolCol, UINT anLightValue,bool bChange)
{
	if( mp_changeSkinEven.size() > 0 )
	{
		if(( 0== anLightValue)&& (0!= m_DefaultColor)) 
		{
			m_BaseColor = m_DefaultColor;
			m_nLightPos = 120;

			//if( mb_HasPopWnd )
			//mp_colorWnd->SetSelectedColor(m_DefaultColor);

			for(size_t idx = 0;idx < mp_changeSkinEven.size();idx++ )
			{
				mp_changeSkinEven[idx]->OnchangeColor( m_DefaultColor, 120,bChange );
			}

		}
		else
		{
			m_BaseColor = acolCol;
			m_nLightPos = anLightValue;

			for(size_t idx = 0;idx < mp_changeSkinEven.size();idx++ )
				mp_changeSkinEven[idx]->OnchangeColor( acolCol, anLightValue,bChange );
		}
	}
}

void CSkinMgrV1::OnSendChangeSkin( CString strPath)
{
	if(strPath != "")
		m_strSkinPath = strPath;
	else
	{
		AfxUserConfig* pUserConfig = AfxGetUserConfig();
		string path;
		pUserConfig->GetConfigStringValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_SKIN_IDX, path);
		m_strSkinPath = path.c_str();
		char szbuffer[512];
		CLhcImg::GetHomePath(szbuffer,512);
		strcat(szbuffer,"\\skin\\");
		m_strSkinPath = szbuffer + m_strSkinPath;
		//	pUserConfig->SetConfigStringValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_SKIN_IDX, string(skinPath), true);
	}
	int	nPos = m_strSkinPath.TrimRight("\\").ReverseFind('\\');
	CString name = m_strSkinPath.Tokenize("\\", nPos);
	if(!YL_FileInfo::IsValid(m_strSkinPath.Left(nPos) + "\\skin.dat"))
	{
		//
	}
	else
	{
		m_BaseColor = 0;
		if( mp_changeSkinEven.size() > 0 )
		{
			for(size_t idx = 0;idx < mp_changeSkinEven.size();idx++ )
			{
				mp_changeSkinEven[idx]->OnChangeSkinTemplate( m_strSkinPath );
			}
		}

		if( mp_SubjectWnd && IsWindow(mp_SubjectWnd->GetSafeHwnd()))
		{
			mp_SubjectWnd->OnSendChangeSkin("");
		}
	}
}

void CSkinMgrV1::OnChangeSkinAndSub(const CString& strPath,const CString& strSub)
{
	if(strPath != "")
		m_strSkinPath = strPath;
	else
	{
		AfxUserConfig* pUserConfig = AfxGetUserConfig();
		string path;
		pUserConfig->GetConfigStringValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_SKIN_IDX, path);
		m_strSkinPath = path.c_str();
		char szbuffer[512];
		CLhcImg::GetHomePath(szbuffer,512);
		strcat(szbuffer,"\\skin\\");
		m_strSkinPath = szbuffer + m_strSkinPath;
		//	pUserConfig->SetConfigStringValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_SKIN_IDX, string(skinPath), true);
	}

	int	nPos = m_strSkinPath.TrimRight("\\").ReverseFind('\\');
	CString name = m_strSkinPath.Tokenize("\\", nPos);
	if(!YL_FileInfo::IsValid(m_strSkinPath.Left(nPos) + "\\skin.dat"))
	{

	}
	else
	{
		m_BaseColor = 0;
		if( mp_changeSkinEven.size() > 0 )
		{
			for(size_t idx = 0;idx < mp_changeSkinEven.size();idx++ )
			{
				mp_changeSkinEven[idx]->OnChangeSkinTemplate( m_strSkinPath,strSub);
			}
		}
	}
}

void CSkinMgrV1::BeforeChangeColor()
{
	gBitMapOperate.BeforeChangeColor();
}
void CSkinMgrV1::AfterChangeColor()
{
	gBitMapOperate.AfterChangeColor();
	gBitMapOperate.SetAdjustValue(m_nLightPos);
}
void CSkinMgrV1::DownloadStart()
{	
	if( !mp_skinWnd ) return;
	

	if(mp_skinWnd != NULL)
	{
		mp_skinWnd->EnableSkinBtn( FALSE );
		mp_skinWnd->SetUseTip(false);
	}
	SetDownloadPercentage(0, m_strDownSkin);

	if( m_bShowSkinWnd )
	{
		CRect lClient;

		lClient.SetRect(3,235,297,285);

		if( AfxGetUIManager()->UIGetLayoutMgr()->GetDisplayMode() == DISPLAY_SEPERATE_MODE )
		{			
			mp_downloadSkin->MoveWindow( CRect( lClient.left,lClient.top,lClient.left+220,lClient.top+30) );
		}
		else
		{		
			mp_downloadSkin->MoveWindow( lClient );
		}
		mp_downloadSkin->ShowWindow( SW_SHOW );
		mb_isDownSkin = true;

		mp_mainSkinWnd->ShowColorBtn( FALSE );
	}
}
void CSkinMgrV1::SetDownloadPercentage(int percentage,CString strSkinName)
{
	if(mp_skinWnd == NULL || !::IsWindow(mp_skinWnd->m_hWnd))
		return;

	if( m_bShowSkinWnd )
	{
		if( !mp_downloadSkin->IsWindowVisible() && mp_skinWnd->IsWindowVisible())
			mp_downloadSkin->ShowWindow( SW_SHOW );
		if( strSkinName.GetLength() != 0 )
		{
			CString strHit = "";
			strHit.Format( "正在下载 \"%s\" 皮肤",strSkinName );
			mp_downloadSkin->SetDownloadHit( strHit );
		}
		mp_downloadSkin->SetDownloadPercent( percentage );
	}
}
void CSkinMgrV1::DownloadFinish(CString strSkinName)
{
	if(mp_skinWnd == NULL || !::IsWindow(mp_skinWnd->m_hWnd))
		return;

	mp_skinWnd->EnableSkinBtn( TRUE );
	mp_skinWnd->DownloadSkinFinish( strSkinName );
	mp_downloadSkin->ShowWindow( SW_HIDE );
	mp_mainSkinWnd->ShowColorBtn( TRUE );

	// 更新按钮的 tooltips
	char szAuthor[SRC_LENGTH];
	YL_Ini ini((LPCTSTR)(m_strSkinPath + "\\skin.ini"));
	ini.GetString("skin", "author", szAuthor, SRC_LENGTH-1);
	CString tips = "皮肤: " + strSkinName + "\r\n作者: " + szAuthor + "\r\n状态: 已备好";
	SetTipText(strSkinName, tips);
}

void CSkinMgrV1::P2POb_DownStart(const Sign& sign, const char* lpszFilePath, unsigned int unFileSize)
{
EnterCriticalSection( &m_csDownInfo );
	if( !mb_HasPopWnd )
	{
		LeaveCriticalSection( &m_csDownInfo );
		return;
	}
	m_strDownPath = lpszFilePath;
	SetDownloadPercentage(0, m_strDownSkin);
	mb_isDownSkin = true;
	if(mp_skinWnd != NULL)
		mp_skinWnd->SetUseTip(false);
LeaveCriticalSection( &m_csDownInfo );

}
void CSkinMgrV1::P2POb_DownFinish(const Sign& sign)
{
EnterCriticalSection( &m_csDownInfo );
	mb_isDownSkin = false;
	if( mb_HasPopWnd )
	{
		SetDownloadPercentage(100, m_strDownSkin);
		DownloadFinish(m_strDownSkin);	
		if(mp_skinWnd != NULL)
			mp_skinWnd->SetUseTip(true);
	}

	if( mp_changeSkinEven.size() > 0 )
	{
		char szbuffer[512];
		CLhcImg::GetHomePath(szbuffer,512);
		strcat(szbuffer,"\\skin\\");

		vector<string> vec;
		if( YL_CFileZip::UnCompress( (LPCTSTR)m_strDownPath, szbuffer,vec,true) )
		{
			m_BaseColor = 0;
			if( mb_HasPopWnd )
				::SendMessage(::AfxGetApp()->m_pMainWnd->m_hWnd, WM_SKIN_DOWNLOAD_FINISHED, (WPARAM)(LPCTSTR)m_strDownSkin, 0);
		}
	}

	
LeaveCriticalSection( &m_csDownInfo );
}
void CSkinMgrV1::P2POb_DownFailed(const Sign& sign, P2P_DOWN_FAILED_REASON pdfr)
{
EnterCriticalSection( &m_csDownInfo );
	if(!mb_HasPopWnd || mp_skinWnd == NULL || !::IsWindow(mp_skinWnd->m_hWnd))
	{
		LeaveCriticalSection( &m_csDownInfo );
		return;
	}
	::PostMessage(mp_skinWnd->m_hWnd, WM_SKIN_DOWNLOAD_FAILED, 0, 0);
	mb_isDownSkin = false;
	if(mp_skinWnd != NULL)
	{
		mp_skinWnd->SetUseTip(true);
		mp_skinWnd->EnableSkinBtn( TRUE );
	}
	if( mp_downloadSkin != NULL )
		mp_downloadSkin->ShowWindow( SW_HIDE );
	mp_mainSkinWnd->ShowColorBtn( TRUE );
	mp_mainSkinWnd->Invalidate();
	mp_mainSkinWnd->UpdateWindow();

LeaveCriticalSection( &m_csDownInfo );
}
void CSkinMgrV1::P2POb_DownProgress(const Sign& sign, unsigned int unContinue, unsigned int unFinished, unsigned int unSpeed)
{
EnterCriticalSection( &m_csDownInfo );
	if( !mb_HasPopWnd )
	{
		LeaveCriticalSection( &m_csDownInfo );
		return;
	}
	SetDownloadPercentage((UINT)unFinished, m_strDownSkin);
LeaveCriticalSection( &m_csDownInfo );
}


void CSkinMgrV1::OnSendChangeSubject(CString,CString subjectName)
{
	NotifyChangeSubject( subjectName );
	CIrregularBorderMgr::GetInstance()->InitTransDlg(true);
}

void CSkinMgrV1::NotifyChangeSubject(const CString& strSbName)
{	
	if( !AfxGetUIManager()->UIGetSkinMgr()->SubjectToSkin( string(strSbName) ) )
		return;

	AfxGetUIManager()->UIGetSkinMgr()->GetSubject()->strCurSubject = strSbName;
	AfxGetUIManager()->UIGetLayoutMgr()->OnChangeSubject( strSbName );

	for(size_t idx = 0;idx < mp_changeSkinEven.size();idx++ )
	{
		mp_changeSkinEven[idx]->OnChangeSubject();
	}		

	if( mp_skinWnd && ::IsWindow(mp_skinWnd->m_hWnd))
	{
		mp_skinWnd->OnChangeSubject();
	}
}

bool CSkinMgrV1::CloseSkinWnd()
{
	EnterCriticalSection( &m_csDownInfo );
	mb_HasPopWnd = false;	
	mp_mainSkinWnd = NULL;

//	mb_isDownSubject = false;

	LeaveCriticalSection( &m_csDownInfo );
	return true;
}


void  CSkinMgrV1::SetSelectedColor(COLORREF aColor)
{
	m_BaseColor = aColor;
}

void CSkinMgrV1::ClickSub(const CString& str)
{
	if( mp_SubjectWnd && IsWindow(mp_SubjectWnd->GetSafeHwnd()))
	{
		mp_SubjectWnd->OnCliekButton( str );
	}
}

void CSkinMgrV1::ILayoutChangeOb_SkinChanged(string oldSkinPath, string newSkinPath)
{
	if( mb_HasPopWnd )
	{
		if( mp_skinWnd )
		{
			mp_skinWnd->SkinChanged( newSkinPath );
		}
	}
}

void CSkinMgrV1::OnStartDownSubject(const CString& strSubName)
{
	DownloadSubStart(strSubName);
	CDownSub::ReqDownSub( strSubName,mp_downloadSubject->m_hWnd );
	m_strDownSubName = strSubName;
}

void CSkinMgrV1::OnFinishDownSubject(BOOL bSuc)
{
	mb_isDownSubject = false;
	if(mb_isDownWebSub)
	{
		string strName = m_strDownSubName;
		InsertSubject(strName);
		InstallWebSubject(strName);
		DownloadWebSubFinish( m_strDownSubName,bSuc );
		mb_isDownWebSub	 = false;
	}
	else
		DownloadSubFinish( m_strDownSubName,bSuc );
}

void CSkinMgrV1::InsertSubject(string strSubName)
{
}

void CSkinMgrV1::InstallWebSubject(string strSubName)
{
}
void CSkinMgrV1::DownloadWebSub(const CString& strUrl,const CString& strid)
{
//	EnterCriticalSection( &m_csDownInfo );
	if( !mp_skinWnd || !mp_mainSkinWnd) return;
	if(mb_isDownWebSub&&CDownSub::IsDownWebSub())
	{
		mp_mainSkinWnd->SetToolTip("已有下载任务了，请稍候...");
		mp_mainSkinWnd->ShowToolTip(1);
		return;
	}
	OneSubject * onesub = AfxGetUIManager()->UIGetSkinMgr()->GetOneSubject(strid);
	if(onesub!=NULL)
	{
		mb_isDownWebSub = false;
		if(mp_SubjectWnd&&IsWindow(mp_SubjectWnd->m_hWnd))
			mp_SubjectWnd->ClickSubBtn("",strid);
		return;
	}

	mp_mainSkinWnd->ShowToolTip(0);
	DownloadSubStart(strid);
	mb_isDownWebSub = true;
	CDownSub::ReqDownWebSub( strUrl,strid,mp_downloadSubject->m_hWnd );
	m_strDownSubName = strid;
//	LeaveCriticalSection(&m_csDownInfo);
}

void CSkinMgrV1::DownloadSubStart(const CString& strSub)
{
	if( !mp_skinWnd ) return;
	CRect lClient;

	lClient.SetRect(3,235,297,285);

	if(mp_SubjectWnd != NULL)
	{
		mp_SubjectWnd->EnableSkinBtn( FALSE );
	}
	if(mp_skinWnd != NULL )
	{
		mp_skinWnd->EnableSkinBtn( FALSE );
	}

	if( !m_bShowSkinWnd )
	{
		mp_downloadSubject->MoveWindow( lClient );
		mp_downloadSubject->ShowWindow( SW_SHOW );	

		mp_downloadSubject->SetEventListener( this );

		OneSubject* pOneSub = AfxGetUIManager()->UIGetSkinMgr()->GetOneSubject( strSub );
		

		CString strHit = "";
		if( pOneSub != NULL )
		{
			strHit.Format( "正在下载 \"%s\" 皮肤",pOneSub->strShowName.c_str()  );
		}
		else
		{
			strHit.Format( "正在下载皮肤");
		}

		mp_downloadSubject->SetDownloadHit( strHit );		
		mp_downloadSubject->SetDownloadPercent(0);

		mb_isDownSubject = true;

		mp_mainSkinWnd->ShowColorBtn( FALSE );
	}
}

void CSkinMgrV1::DownloadSubFinish( const CString& strSub,BOOL bSuc)
{
	if( mp_SubjectWnd != NULL )
		mp_SubjectWnd->EnableSkinBtn( TRUE );

	if(mp_skinWnd != NULL )
		mp_skinWnd->EnableSkinBtn( TRUE );

	mp_downloadSubject->ShowWindow( SW_HIDE );
	mp_mainSkinWnd->ShowColorBtn( TRUE );
	mp_mainSkinWnd->Invalidate();
	mp_mainSkinWnd->UpdateWindow();

	if( bSuc )
	{	
		AfxGetUIManager()->UIGetSkinMgr()->SubDownloaded( strSub );
		if( mb_HasPopWnd )
		{
			OneSubject* pOneSub = AfxGetUIManager()->UIGetSkinMgr()->GetOneSubject( strSub );

			CString tips = "";
			if( pOneSub != NULL )
				tips.Format( "皮肤:  %s\r\n状态: 已备好",pOneSub->strShowName.c_str()  );
			else
				tips.Format( "状态: 已备好");
			if( mp_SubjectWnd && pOneSub )
				mp_SubjectWnd->SetTipText( strSub,tips ,pOneSub->strShowName.c_str());
			mp_SubjectWnd->ClickSubBtn("",strSub );
		}
	}
	else
	{
		if( mb_HasPopWnd )
		{
			CDialogMsg dlgmsg;
			dlgmsg.SetBtnCount(1);
			dlgmsg.SetMsg("暂时无法下载该皮肤，请稍候再试");
			dlgmsg.SetBtnText(1,"确定");
			dlgmsg.DoModal();
		}
	}
}
void CSkinMgrV1::DownloadWebSubFinish( const CString& strSub,BOOL bSuc)
{
	if( mp_SubjectWnd != NULL )
		mp_SubjectWnd->EnableSkinBtn( TRUE );

	if(mp_skinWnd != NULL )
		mp_skinWnd->EnableSkinBtn( TRUE );

	mp_downloadSubject->ShowWindow( SW_HIDE );
	mp_mainSkinWnd->ShowToolTip(0);
	mp_mainSkinWnd->ShowColorBtn( TRUE );

	CRect rc;
	mp_downloadSubject->GetWindowRect(rc);
	mp_mainSkinWnd->ScreenToClient(rc);
 	mp_mainSkinWnd->InvalidateRect(&rc,true);
 	mp_mainSkinWnd->UpdateWindow();

	if( bSuc )
	{	
		AfxGetUIManager()->UIGetSkinMgr()->SubDownloaded( strSub );
		if( mb_HasPopWnd )
		{
			OneSubject* pOneSub = AfxGetUIManager()->UIGetSkinMgr()->GetOneSubject( strSub );
			bool nFirstRun;
			AfxGetUserConfig()->GetConfigBoolValue( CONF_SETTING_MODULE_NAME,CONF_SETTING_FIRST_TIME_CHANGESKIN,nFirstRun);
			if(nFirstRun)
			{
				mp_mainSkinWnd->OnBtnSub();
				nFirstRun = 0;
				AfxGetUserConfig()->SetConfigBoolValue( CONF_SETTING_MODULE_NAME,CONF_SETTING_FIRST_TIME_CHANGESKIN,nFirstRun);
			}
			else
			{
				mp_mainSkinWnd->SetToolTip("皮肤已保存到“我的皮肤”");
				mp_mainSkinWnd->ShowToolTip(1);
			}

			CString tips = "";
			if( pOneSub != NULL )
				tips.Format( "皮肤:  %s\r\n状态: 已备好",pOneSub->strShowName.c_str()  );
			else
				tips.Format( "状态: 已备好");

			if(  pOneSub != NULL&&mp_SubjectWnd )
				mp_SubjectWnd->SetTipText( strSub,tips ,pOneSub->strShowName.c_str());

			mp_SubjectWnd->ClickSubBtn("",strSub );
		}
	}
	else
	{
		if( mb_HasPopWnd )
		{
			CDialogMsg dlgmsg;
			dlgmsg.SetBtnCount(1);
			dlgmsg.SetMsg("暂时无法下载该皮肤，请稍候再试");
			dlgmsg.SetBtnText(1,"确定");
			dlgmsg.DoModal();
		}
		//	AfxMessageBox("皮肤下载失败!");
	}
}
