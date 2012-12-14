#include "stdafx.h"
#include "SubjectChildWnd.h"
#include "YL_FileInfo.h"
#include "../AppConfig/config/ConfigSettingDef.h"
#include "DownSubject.h"
#include "SubSkinButton.h"
#include "YL_DirInfo.h"
#include "Gui/CommonControl/DialogMsg.h"

#include "IChangeColorMgr.h"
extern CGdiEx gGdiExOperate;

IMPLEMENT_DYNCREATE(CSubjectChildWnd, CFrameWnd)

CSubjectChildWnd::CSubjectChildWnd()
{
	miScrollPos = 0;
	mbIsHasScroll = false;
	m_nMaxViewPos = 0;
	mp_EventListener = NULL;
}

CSubjectChildWnd::~CSubjectChildWnd()
{
}

BEGIN_MESSAGE_MAP(CSubjectChildWnd, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_NCPAINT()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

int CSubjectChildWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	//ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
	CreateSubBtn();
	miScrollPos = 0;
	return 0;
}

void CSubjectChildWnd::CreateSubBtn()
{
	GetWindowRect(mrcOriginalRect);
	mrcOriginalRect.bottom = mrcOriginalRect.top + m_nMaxViewPos;

	CRect lrectClient;
	GetClientRect(&lrectClient);

	UINT lnBmpWidth = 0;
	UINT lnBmpHight = 0;
	m_colBtnList_itor = m_colBtnList.begin() ;
	if( (0 != (int)( m_colBtnList.size())  ) && (NULL != (*m_colBtnList_itor)->GetBitmapEx()) ) 
	{
		BITMAP   lBitMap;
		(*m_colBtnList_itor)->GetBitmapEx()->GetBitmap( &lBitMap );
		lnBmpWidth = lBitMap.bmWidth+8;
		lnBmpHight = lBitMap.bmHeight;

		string  skinPath =AfxGetUIManager()->UIGetSkinMgr()->GetSkinPath();
		size_t idx = skinPath.find_last_of('\\');
		if( idx != string::npos )
		{
			skinPath = skinPath.substr(0,idx);
			skinPath = skinPath + "\\" + STR_DEFAULT_SKIN;
		}			

		list<CSubLabel *>::iterator subiter = m_SubLabelList.begin();
		int lnIndex = 0;
		while( m_colBtnList_itor != m_colBtnList.end() )
		{
			(*m_colBtnList_itor)->Create(NULL,WS_CHILD | WS_VISIBLE|BS_OWNERDRAW,CRect(0,0,0,0), this, BOTTON_BEGAIN_ID+lnIndex	);
			(*m_colBtnList_itor)->SetListener( this );
			(*m_colBtnList_itor)->GetBitmapEx()->SetBitmapDimension(lBitMap.bmWidth,lBitMap.bmHeight);
			(*m_colBtnList_itor)->SetPngSkin(CString(skinPath.c_str()) + CString("\\png\\") + "yes.png");
			(*m_colBtnList_itor)->SetPngHoverSkin(CString(skinPath.c_str()) + CString("\\png\\") + "delete.png");
			(*m_colBtnList_itor)->SetPngFrame(CString(skinPath.c_str()) + CString("\\png\\") + "SkinFrame.png");
			//skinlabel
			(*subiter)->Create(NULL,WS_CHILD | WS_VISIBLE|BS_OWNERDRAW,	CRect(0,0,0,0),	this,SUB_LABEL_ID+lnIndex);

			if( (*m_colBtnList_itor)->GetText() == AfxGetUIManager()->UIGetSkinMgr()->GetSubject()->strCurSubject.c_str()  )
			{
				(*m_colBtnList_itor)->SetSelected( true );
				(*subiter)->SetSelect( true );
			}
			(*m_colBtnList_itor)->SetErase( true );
			(*m_colBtnList_itor)->ShowWindow(SW_SHOW);

			lnIndex++;
			m_colBtnList_itor++;
			subiter++;		
		}
		m_SubBtnID = lnIndex;
	}
	
	MoveSubjectBtn();
	m_colBtnList_itor = m_colBtnList.begin();
}

void  CSubjectChildWnd::MoveSubjectBtn()
{
	CRect lrectClient;
	GetClientRect(&lrectClient);

	UINT lnBmpWidth = 0;
	UINT lnBmpHight = 0;
	m_colBtnList_itor = m_colBtnList.begin() ;
	if( (0 != (int)( m_colBtnList.size())  ) && (NULL != (*m_colBtnList_itor)->GetBitmapEx()) ) 
	{
		BITMAP   lBitMap;
		(*m_colBtnList_itor)->GetBitmapEx()->GetBitmap( &lBitMap );
		lnBmpWidth = lBitMap.bmWidth+8;
		lnBmpHight = lBitMap.bmHeight;
	}
	list<CSkinButton *>::iterator SubBtniter = m_colBtnList.begin();
	list<CSubLabel *>::iterator Labeliter = m_SubLabelList.begin();
	int lnIndex =  0;
	int count = m_SubLabelList.size();

	int irow_count,irow_inter;
	irow_count = ROW_COUNT;
	irow_inter = 0;

	while (SubBtniter != m_colBtnList.end())
	{

		CRect btnrc = CRect(
			lrectClient.left + LIFT_SPACE + (lnIndex%irow_count)*(lnBmpWidth), 
			lrectClient.top  + TOP_SPACE + (int)(lnIndex/irow_count)*(lnBmpHight+2*TOP_SPACE) + (int)(lnIndex/irow_count)*INTER - miScrollPos,
			lrectClient.left + LIFT_SPACE+lnBmpWidth + (lnIndex%irow_count)*(lnBmpWidth) + irow_inter, 
			lrectClient.top  + 2*TOP_SPACE + lnBmpHight+(int)(lnIndex/irow_count)*(lnBmpHight+2*TOP_SPACE)+ (int)(lnIndex/irow_count)*INTER - miScrollPos
			);
		(*SubBtniter)->MoveWindow(&btnrc);

		CRect labelrc = CRect(
			lrectClient.left + LIFT_SPACE + (lnIndex%irow_count)*(lnBmpWidth), 
			lrectClient.top  + 3*TOP_SPACE + lnBmpHight+(int)(lnIndex/irow_count)*(lnBmpHight+2*TOP_SPACE)+ (int)(lnIndex/irow_count)*INTER- miScrollPos,
			lrectClient.left + LIFT_SPACE+lnBmpWidth + (lnIndex%irow_count)*(lnBmpWidth) + irow_inter, 
			lrectClient.top  + 3*TOP_SPACE + lnBmpHight+(int)(lnIndex/irow_count)*(lnBmpHight+2*TOP_SPACE)+ (int)(lnIndex/irow_count + 1)*INTER- miScrollPos
			);
		(*Labeliter)->MoveWindow(&labelrc);
		SubBtniter++;
		Labeliter++;
		lnIndex++;
	}
}

BOOL CSubjectChildWnd::IsSubExist(string strName)
{
	list<CSkinButton *>::iterator SubBtniter = m_colBtnList.begin();
	while(SubBtniter != m_colBtnList.end())
	{
		if(!strcmp(strName.c_str(),(*SubBtniter)->GetText()))
			return TRUE;
		SubBtniter++;
	}
	return FALSE;
}

//插入一个主题按钮
void CSubjectChildWnd::InsertSubjectBtn(string &astrName,CBitmap* apBmp, string strPath,bool bHasSkin)
{
	if (0 == astrName.length() ) return ;
	if(IsSubExist(astrName)) return;
	CSkinButton* lpBtnSkin = new CSubSkinButton();
	lpBtnSkin->SetText( CString(astrName.c_str() ) );
	lpBtnSkin->SetBottonType(lpBtnSkin->SKIN_BOTTON );
	lpBtnSkin->SetBitmap( apBmp);
	lpBtnSkin->SetSkinPath(strPath);
	m_colBtnList.insert(++m_colBtnList.begin(),lpBtnSkin);

	CSubLabel* pSkinLabel = new CSubLabel();
	m_SubLabelList.insert(++m_SubLabelList.begin(),pSkinLabel);
	//if( !bHasSkin )
	{
		pair<CString, CSkinButton*> apair;
		apair.first = astrName.c_str();
		apair.second = lpBtnSkin;
		m_noDownloadSkinMap.insert( apair );
	}

	BITMAP   lBitMap;
	apBmp->GetBitmap( &lBitMap );

	string  skinPath =AfxGetUIManager()->UIGetSkinMgr()->GetSkinPath();
	size_t idx = skinPath.find_last_of('\\');
	if( idx != string::npos )
	{
		skinPath = skinPath.substr(0,idx);
		skinPath = skinPath + "\\" + STR_DEFAULT_SKIN;
	}			

	lpBtnSkin->Create(NULL,WS_CHILD | WS_VISIBLE|BS_OWNERDRAW,CRect(0,0,0,0), this, BOTTON_BEGAIN_ID+m_SubBtnID	);
	pSkinLabel->Create(NULL,WS_CHILD | WS_VISIBLE|BS_OWNERDRAW,	CRect(0,0,0,0),	this,SUB_LABEL_ID+m_SubBtnID);

	lpBtnSkin->SetListener( this );
	lpBtnSkin->GetBitmapEx()->SetBitmapDimension(lBitMap.bmWidth,lBitMap.bmHeight);
	lpBtnSkin->SetPngSkin(CString(skinPath.c_str()) + CString("\\png\\") + "yes.png");
	lpBtnSkin->SetPngHoverSkin(CString(skinPath.c_str()) + CString("\\png\\") + "delete.png");
	if( lpBtnSkin->GetText() == AfxGetUIManager()->UIGetSkinMgr()->GetSubject()->strCurSubject.c_str()  )
	{
		lpBtnSkin->SetSelected( true );
		pSkinLabel->SetSelect(true);

	}
	lpBtnSkin->SetErase( true );
	lpBtnSkin->ShowWindow(SW_SHOW);


	MoveSubjectBtn();
	if(IsWindowVisible())
		ResetScroll();

	m_SubBtnID++;
	
}

void CSubjectChildWnd::ResetScroll()
{
	//miScrollPos = 0;
	SCROLLINFO si;
	mpVerticleScrollbar.GetScrollInfo(&si);
	InitScrollBar();
	CRect rc;
	GetClientRect(&rc);
	if(mrcOriginalRect.Height()<=0)
		return;
	miHeight = rc.Height();
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL; 
	si.nMin = 0;
	si.nMax = mrcOriginalRect.Height();
	si.nPage = miHeight + 1;
	si.nPos = si.nPos;	
	if(si.nPos > si.nMax- si.nPage)
	{
		si.nPos = si.nMax - si.nPage;
	}

	mpVerticleScrollbar.SetScrollInfo( &si);
	SetScrollInfo(SB_VERT, &si, TRUE);
}
void CSubjectChildWnd::OnPaint()
{
	CPaintDC dc(this);

	CRect rc;
	GetClientRect(&rc);
	dc.FillRect(rc,&CBrush(RGB(255,255,255)));

	if( m_colBtnList.size() > 0) 
	{
		list<CSkinButton*>::iterator itor = m_colBtnList.begin();
		if( !(*itor)->GetSafeHwnd() )
			CreateSubBtn();
	}
}

void CSubjectChildWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int liDelta;
	int liMaxPos = mrcOriginalRect.Height()- miHeight;

	switch (nSBCode)
	{
	case SB_LINEDOWN:
		if (miScrollPos >= liMaxPos)
			return;

		liDelta = min(max(liMaxPos/20,5),liMaxPos-miScrollPos);
		break;

	case SB_LINEUP:
		if (miScrollPos <= 0)
			return;
		liDelta = -min(max(liMaxPos/20,5),miScrollPos);
		break;
	case SB_PAGEDOWN:
		if (miScrollPos >= liMaxPos)
			return;

		liDelta = min(max(liMaxPos/20,5),liMaxPos-miScrollPos);
		break;
	case SB_THUMBTRACK:
		if (miScrollPos >= liMaxPos)
		{
			//moveToPos(IDC_ULUSERINFO_STATIC1);
		}		
		liDelta = nPos;
	case SB_THUMBPOSITION:
		liDelta = (int)nPos - miScrollPos;
		break;

	case SB_PAGEUP:
		if (miScrollPos <= 0)
			return;
		liDelta = -min(max(liMaxPos/20,5),miScrollPos);
		break;
	case WM_MOUSEWHEEL:

		if( 1==nPos)
		{
			if (miScrollPos <= 0) return;
			liDelta = -min(max(liMaxPos/20,5),miScrollPos);
		}

		if(-1 ==nPos)
		{
			if (miScrollPos >= liMaxPos) return;
			liDelta = min(max(liMaxPos/20,5),liMaxPos-miScrollPos);
		}
		break;
	default:
		return;
	}
	miScrollPos += liDelta;
	::SendMessage(mpVerticleScrollbar.m_hWnd,WM_PDLG_MOUSEWHEEL_MSG,SB_VERT,miScrollPos);
	ScrollWindow(0,-liDelta);

	CFrameWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CSubjectChildWnd::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
	if(mbIsHasScroll)
	{
		CFrameWnd::OnSize(nType, cx, cy);

		miHeight = cy;
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL; 
		si.nMin = 0;
		si.nMax = mrcOriginalRect.Height();
		si.nPage = cy+1;
		si.nPos = 0;
		SetScrollInfo(SB_VERT, &si, TRUE);
	}

	// TODO: Add your message handler code here
}

BOOL CSubjectChildWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int liUpOrDown;
	liUpOrDown = (zDelta >= 0) ?  1:-1;
	OnVScroll(WM_MOUSEWHEEL,liUpOrDown,NULL);
	return true;
	//return CFrameWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CSubjectChildWnd::OnNcPaint()
{
	// TODO: Add your message handler code here
	// Do not call CFrameWnd::OnNcPaint() for painting messages
}

BOOL CSubjectChildWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	int lnCount = (int)( m_colBtnList.size() );
	int lnLine  = lnCount/ROW_COUNT+ (lnCount%ROW_COUNT == 0? 0:1);
	BITMAP   lBitMap;
	m_colBtnList_itor = m_colBtnList.begin() ;
	if( (0 != lnCount) && (NULL != (*m_colBtnList_itor)->GetBitmapEx())  )
	{
		CBitmap *bmp = (*m_colBtnList_itor)->GetBitmapEx();
		bmp->GetBitmap( &lBitMap );
		int lnBmpHight = lBitMap.bmHeight;
		int lnCy = lnLine* (lnBmpHight + 2*TOP_SPACE) + TOP_SPACE;
		if( lnCy> cs.cy )
		{
			mbIsHasScroll = true;
			m_nMaxViewPos = lnCy;
		}
	}
	// TODO: Add your specialized code here and/or call the base class
	return CFrameWnd::PreCreateWindow(cs);
}
void CSubjectChildWnd::UpdateScrollBar()
{

	//更新滑块信息
	mpVerticleScrollbar.UpdateScrollBar();
	mpHorizontalScrollbar.UpdateScrollBar();
}
void CSubjectChildWnd::UpdateScrollBarPosition()
{

	//设置滚动条位置
	mpVerticleScrollbar.SetPosition();
	mpHorizontalScrollbar.SetPosition();
}
void CSubjectChildWnd::UpdateControlsEnable(BOOL abEnable)
{
	if (abEnable)
	{
		if (mpVerticleScrollbar)
		{
			mpVerticleScrollbar.EnableWindow(TRUE);
			mpVerticleScrollbar.ShowWindow(SW_SHOW);
		}
	}
	else
	{
		if (mpVerticleScrollbar)
		{
			mpVerticleScrollbar.EnableWindow(FALSE);
			mpVerticleScrollbar.ShowWindow(SW_HIDE);
		}
	}
}
void CSubjectChildWnd::InitScrollBar()
{	
//	miScrollPos = 0;

	CRect rc;
	this->GetClientRect( &rc );
	int lnCount = (int)( m_colBtnList.size() );
	int lnLine  = lnCount/ROW_COUNT+ (lnCount%ROW_COUNT == 0? 0:1);
	BITMAP   lBitMap;
	m_colBtnList_itor = m_colBtnList.begin() ;
	if( (0 != lnCount) && (NULL != (*m_colBtnList_itor)->GetBitmapEx())  )
	{
		CBitmap *bmp = (*m_colBtnList_itor)->GetBitmapEx();
		bmp->GetBitmap( &lBitMap );
		int lnBmpHight = lBitMap.bmHeight;
		int lnCy = lnLine* (lnBmpHight + 2*TOP_SPACE + INTER) + TOP_SPACE;
		if( lnCy> rc.Height() )
		{
			mbIsHasScroll = true;
			m_nMaxViewPos = lnCy;
		}
	}

	GetWindowRect(mrcOriginalRect);
	mrcOriginalRect.bottom = mrcOriginalRect.top + m_nMaxViewPos;

	if(mbIsHasScroll)
	{
		CWnd* lpParent = GetParent();

		//运行时建立自定义滚动条
		if( ! mpVerticleScrollbar.m_hWnd )
		{
			mpVerticleScrollbar.Create(  NULL, WS_CHILD | SS_LEFT | SS_NOTIFY | WS_VISIBLE | WS_GROUP, CRect(0 ,0, 0, 0),   lpParent);
			CRect rect;
			gGdiExOperate.GetWindowRectInParentCoordinates( this->m_hWnd, &rect );
			rect.bottom-=1;
			this->MoveWindow( &rect, 0 );
			rect.bottom+=1;
			this->MoveWindow( &rect, 1 );
		}
		if( ! mpHorizontalScrollbar.m_hWnd )
			mpHorizontalScrollbar.Create(NULL, WS_CHILD | SS_LEFT | SS_NOTIFY | WS_VISIBLE | WS_GROUP, CRect(0 ,0, 0, 0),   lpParent);

		//设置滚动条方向
		mpVerticleScrollbar.SetScrollOrientation(esoVertical);
		mpHorizontalScrollbar.SetScrollOrientation(esoHorizontal);
		//设置滚动条控制控件
		mpVerticleScrollbar.SetControlWnd(this);
		mpHorizontalScrollbar.SetControlWnd(this);
		//设置滚动条位置
		UpdateScrollBarPosition();
		if (IsWindowEnabled())
			UpdateControlsEnable(TRUE);
		else
			UpdateControlsEnable(FALSE);
		UpdateScrollBar();
	}
}
void CSubjectChildWnd::InsertSmallObjectBmp(string &astrName,CBitmap* apBmp, string strPath,bool bHasSkin)
{
	if (0 == astrName.length() ) return ;
	CSkinButton* lpBtnSkin = new CSubSkinButton();
	lpBtnSkin->SetText( CString(astrName.c_str() ) );
	lpBtnSkin->SetBottonType(lpBtnSkin->SKIN_BOTTON );
	lpBtnSkin->SetBitmap( apBmp);
	lpBtnSkin->SetSkinPath(strPath);
	m_colBtnList.push_back(lpBtnSkin);

	CSubLabel* pSkinLabel = new CSubLabel();
	m_SubLabelList.push_back(pSkinLabel);
	//if( !bHasSkin )
	{
		pair<CString, CSkinButton*> apair;
		apair.first = astrName.c_str();
		apair.second = lpBtnSkin;
		m_noDownloadSkinMap.insert( apair );
	}
}
void CSubjectChildWnd::OnDestroy()
{
	CFrameWnd::OnDestroy();
	DelAllSubjectBtn();
}

void CSubjectChildWnd::OnCliekButton(const CString& strText)
{
	YL_Log("SkinB.txt",LOG_DEBUG,"Click-In","1");
	if( NULL != mp_EventListener)
	{		
		mp_EventListener->OnSendChangeSubject( "",strText) ;

		COLORREF colorref = AfxGetUIManager()->UIGetSkinMgr()->GetSubjectColor( strText );
		::SendMessage( this->GetParent()->GetSafeHwnd(), MSG_CHANGE_DEF_BTN_COLOR,colorref,0 );

		OneSubject* pCurSub = AfxGetUIManager()->UIGetSkinMgr()->GetCurSubject();
		if( pCurSub != NULL )
		{
			YL_Log("SkinB.txt",LOG_DEBUG,"Click-In","1.2");
			mp_EventListener->OnSendChangeColor( pCurSub->subColor,pCurSub->iLight ,false );
			::AfxGetUserConfig()->SetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_COLOR_BTN_INDEX,0,true);
			YL_Log("SkinB.txt",LOG_DEBUG,"Click-In","1.3");


			m_colBtnList_itor = m_colBtnList.begin();
			list<CSubLabel*>::iterator label_itor = m_SubLabelList.begin();
			while( m_colBtnList_itor != m_colBtnList.end() )
			{
				if( (*m_colBtnList_itor)->GetSelected() == 1 )
				{
					(*m_colBtnList_itor)->SetSelected( false );
					(*m_colBtnList_itor)->Invalidate( TRUE );
					(*label_itor)->SetSelect(false);
					(*label_itor)->Invalidate(TRUE);
				}

				if( (*m_colBtnList_itor)->GetText() == strText )
				{
					(*m_colBtnList_itor)->SetSelected( true );
					(*m_colBtnList_itor)->Invalidate( TRUE );
					(*label_itor)->SetSelect(true);
					(*label_itor)->Invalidate(TRUE);
				}
				m_colBtnList_itor ++ ;
				label_itor++;
			}
			m_colBtnList_itor = m_colBtnList.end();			
		}

		//		::AfxGetMainWindow()->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);				
	}
	YL_Log("SkinB.txt",LOG_DEBUG,"Click-In","2");
}

void CSubjectChildWnd::OnCliekButton(CButton* pBotton)
{
	YL_Log("SkinB.txt",LOG_DEBUG,"Click-In","1");
	char szBuf[128];
	sprintf(szBuf,"主题-%s",((CSkinButton*)pBotton)->GetText() );

	CString SkinPath = ((CSkinButton*)pBotton)->GetSkinPath();
	string temppathstr = SkinPath;
	CString SkinName = ((CSkinButton*)pBotton)->GetText();

	if(((CSkinButton*)pBotton)->IsClickPng())
	{
		((CSkinButton*)pBotton)->SetClickPng(0);
		CDialogMsg dlgmsg;
		dlgmsg.SetMsg("确定要删除这套皮肤吗？");
		dlgmsg.SetBtnText(1,"确定");
		dlgmsg.SetBtnText(2,"取消");
		
		if(dlgmsg.DoModal()==1)
		{		
			LogUserActMsg(STR_USER_CLICK, string("BTN:DELETE_REAL_"+SkinName));
			
			DelSubjectBtn(pBotton);
			SkinPath += "\\SbujectSkin\\";
			SkinPath += SkinName;
			OneSubject* pCurSub = AfxGetUIManager()->UIGetSkinMgr()->GetCurSubject();
			if (!strcmp(pCurSub->strSubName.c_str(),SkinName))
			{
				this->RedrawWindow();
				CString NextSkinPath = (*m_BtnNextList_itor)->GetSkinPath();
				CString NextSkinName = (*m_BtnNextList_itor)->GetText();
			if( AfxGetUIManager()->UIGetSkinMgr()->IsNeedDownSub( NextSkinName) )
			{
 				ClickSubBtn(NextSkinPath,"1");
			}
			else
			{
				ClickSubBtn( NextSkinPath,NextSkinName);
			}
			
			}
			
			YL_DirInfo::DeleteDir(SkinPath);
			AfxGetUIManager()->UIGetLayoutMgr()->DeleteSubjectData(SkinName);
		}

	}
	else if( AfxGetUIManager()->UIGetSkinMgr()->IsNeedDownSub( SkinName) )
	{
		if( NULL != mp_EventListener)
		{
			YL_Log("SkinB.txt",LOG_DEBUG,"Click-In","2");
			mp_EventListener->OnStartDownSubject( SkinName );
		}
	}
	else
	{
		ClickSubBtn( SkinPath,SkinName);
	}
	
}

void CSubjectChildWnd::ClickSubBtn(const CString& strSkinPath,const CString& strSubName)
{
	
	if( NULL != mp_EventListener)
	{
		string strSkinPathTemp = AfxGetUIManager()->UIGetSkinMgr()->GetSkinPath();
		string strSkinName;		
		YL_FileInfo::GetFileName(strSkinPathTemp,strSkinName);				

		if( strSkinName != STR_DEFAULT_SKIN )
		{
			string strDefSkinPath;			
			YL_FileInfo::GetFileDirPath(strSkinPathTemp,strDefSkinPath);
			strDefSkinPath += "\\";
			strDefSkinPath += STR_DEFAULT_SKIN;
			
			if( NULL != mp_EventListener)
			{
				this->mp_EventListener->OnChangeSkinAndSub( strDefSkinPath.c_str(),strSubName) ;
			}
		}
		else
		{		
			mp_EventListener->OnSendChangeSubject( strSkinPath,strSubName ) ;
		}

	
		if(!IsWindow(m_hWnd))
			return;

		COLORREF colorref = AfxGetUIManager()->UIGetSkinMgr()->GetSubjectColor( strSubName );
		::SendMessage( this->GetParent()->GetSafeHwnd(), MSG_CHANGE_DEF_BTN_COLOR,colorref,0 );

		OneSubject* pCurSub = AfxGetUIManager()->UIGetSkinMgr()->GetCurSubject();
		if( pCurSub != NULL )
		{
			YL_Log("SkinB.txt",LOG_DEBUG,"Click-In","1.2");
			mp_EventListener->OnSendChangeColor( pCurSub->subColor,pCurSub->iLight ,false );
			::AfxGetUserConfig()->SetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_COLOR_BTN_INDEX,0,true);
			YL_Log("SkinB.txt",LOG_DEBUG,"Click-In","1.3");

			m_colBtnList_itor = m_colBtnList.begin();
			list<CSubLabel*>::iterator label_iter = m_SubLabelList.begin();
			while( m_colBtnList_itor != m_colBtnList.end() )
			{				
				if( (*m_colBtnList_itor)->GetSelected() == 1 )
				{
					(*m_colBtnList_itor)->SetSelected( false );
					(*m_colBtnList_itor)->Invalidate( TRUE );	
					(*label_iter)->SetSelect(false);
					(*label_iter)->Invalidate(TRUE);
				}

				if( (*m_colBtnList_itor)->GetText() == strSubName )
				{
					(*m_colBtnList_itor)->SetSelected( true );
					(*m_colBtnList_itor)->Invalidate( TRUE );
					(*label_iter)->SetSelect(true);
					(*label_iter)->Invalidate(TRUE);
				}
				m_colBtnList_itor ++ ;
				label_iter ++;
			}
			m_colBtnList_itor = m_colBtnList.end();
		}

	}
	YL_Log("SkinB.txt",LOG_DEBUG,"Click-In","2");
}

void CSubjectChildWnd::DelAllSubjectBtn()
{
	if( !(m_colBtnList.size() >0) ) return;
	m_colBtnList_itor =m_colBtnList.begin();
	list<CSubLabel *>::iterator subiter = m_SubLabelList.begin();
	while(  m_colBtnList_itor != m_colBtnList.end() )
	{
		CSkinButton* pBtn = *m_colBtnList_itor;
		if( pBtn->m_BitMap != NULL && pBtn->m_BitMap->m_hObject )
			pBtn->m_BitMap->DeleteObject();
			//delete pBtn->m_BitMap;
		pBtn->m_BitMap=NULL;
		delete pBtn;

		*m_colBtnList_itor = NULL;

		delete (*subiter);
		*subiter = NULL;

		m_colBtnList_itor++;
		subiter++;
	}
	m_colBtnList.clear();
	m_noDownloadSkinMap.clear();
}

void CSubjectChildWnd::DelSubjectBtn(CButton * pdelbtn)
{
	if( !(m_colBtnList.size() >0) ) return;
	list<CSkinButton*>::iterator btniter = m_colBtnList.begin();
	CSkinButton* pBtn;
	CRect btnrc;
	CSubLabel* psublabel;
	CRect labelrc;
	list<CSubLabel *>::iterator subiter = m_SubLabelList.begin();
	while(  btniter != m_colBtnList.end() )
	{	
		pBtn = *btniter;
		if(((CSkinButton*)pdelbtn)->GetText() == pBtn->GetText())
		{
			pBtn->GetWindowRect(&btnrc);
			ScreenToClient(&btnrc);
			m_colBtnList.erase(btniter++);

			psublabel = (*subiter);
			psublabel->GetWindowRect(&labelrc);
			ScreenToClient(&labelrc);
			m_SubLabelList.erase(subiter++);
			break;
		}
		btniter++;
		subiter++;
	}
	
	m_BtnNextList_itor = btniter;
	if(m_BtnNextList_itor == m_colBtnList.end())
		m_BtnNextList_itor = m_colBtnList.begin();
	while(  btniter != m_colBtnList.end() )
	{	
		pBtn = *btniter;
		CRect temprc;
		pBtn->GetWindowRect(&temprc);
		ScreenToClient(&temprc);
		pBtn->MoveWindow(&btnrc);
		btnrc = temprc;
		
		CRect labeltemprc;
		(*subiter)->GetWindowRect(&labeltemprc);
		ScreenToClient(&labeltemprc);
		(*subiter)->MoveWindow(&labelrc);
		labelrc = labeltemprc;

		btniter++;
		subiter++;
	}
	
	pBtn = (CSkinButton*)pdelbtn;
	if( pBtn->m_BitMap != NULL && pBtn->m_BitMap->m_hObject )
		delete pBtn->m_BitMap;
	pBtn->m_BitMap=NULL;
	delete pBtn;

	delete psublabel;

	//重设滚动条
	ResetScroll();
}

BOOL CSubjectChildWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	/*CRect rc;
	GetClientRect(&rc);
	pDC->FillRect(rc,&CBrush(RGB(255,255,255)));*/

	return __super::OnEraseBkgnd(pDC);
	//return TRUE;
}


void CSubjectChildWnd::ShowWindow(BOOL bShow)
{
	if( bShow )
	{
		::ShowWindow(m_hWnd,SW_SHOW);
		if( mbIsHasScroll )
		{
			::ShowWindow(mpVerticleScrollbar.m_hWnd,SW_SHOW);
			::ShowWindow(mpHorizontalScrollbar.m_hWnd,SW_SHOW);
		}
	}
	else
	{
		::ShowWindow(m_hWnd,SW_HIDE);
		if( mbIsHasScroll )
		{
			::ShowWindow(mpVerticleScrollbar.m_hWnd,SW_HIDE);
			::ShowWindow(mpHorizontalScrollbar.m_hWnd,SW_HIDE);
		}
	}
}

void CSubjectChildWnd::OnSendChangeSkin ( CString strPath)
{
	m_colBtnList_itor = m_colBtnList.begin();
	list<CSubLabel *>::iterator labelitor = m_SubLabelList.begin();
	while( m_colBtnList_itor != m_colBtnList.end() )
	{
		if( (*m_colBtnList_itor)->GetSelected() == 1 )
		{
			(*m_colBtnList_itor)->SetSelected( false );
			(*m_colBtnList_itor)->Invalidate( TRUE );
			(*labelitor)->SetSelect(false);
			break;
		}
		m_colBtnList_itor ++ ;
		labelitor++;
	}
	m_colBtnList_itor = m_colBtnList.end();
}

void CSubjectChildWnd::EnableSkinBtn( BOOL enable )
{
	m_colBtnList_itor = m_colBtnList.begin();
	while( m_colBtnList_itor != m_colBtnList.end() )
	{
		(*m_colBtnList_itor)->EnableWindow(enable);
		m_colBtnList_itor++;
	}
	m_colBtnList_itor = m_colBtnList.begin();
}

void CSubjectChildWnd::SetTipText( CString astrName,CString strTip,CString strShowName)
{
	m_colBtnList_itor = m_colBtnList.begin();
	list<CSubLabel *>::iterator subiter = m_SubLabelList.begin();
	while( m_colBtnList_itor != m_colBtnList.end() )
	{
		CString nameText = (*m_colBtnList_itor)->GetText();
		if( nameText == astrName )
		{
			(*m_colBtnList_itor)->SetToolTipText( strTip );
			if(strShowName)
			{
				(*subiter)->SetText(strShowName);
				(*subiter)->Invalidate();
			}
		}
		m_colBtnList_itor++;
		subiter++;
	}
	m_colBtnList_itor = m_colBtnList.begin();
}