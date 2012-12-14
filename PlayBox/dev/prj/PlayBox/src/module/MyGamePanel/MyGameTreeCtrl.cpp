#include "StdAfx.h"
#include "MyGameTreeCtrl.h"
#include "YL_StringUtil.h"
#include "../../gui/util/ShowMenu.h"
#include "../../Core/CDataManager.h"
#include "../UserMan/IUserMan.h"
#include "../../Core/CDataManager.h"

MyGameTreeCtrl::MyGameTreeCtrl( CDibBitmap* pBit):m_bInsertData(false),m_iCurItem(0)
{
	m_bHideBtn = true;
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_QQ_ITEM_CHANGE,(IQQItemCountChangeObserver*)this );	
}

MyGameTreeCtrl::~MyGameTreeCtrl(void)
{
	ClearData();
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_QQ_ITEM_CHANGE,(IQQItemCountChangeObserver*)this );	
}

BEGIN_MESSAGE_MAP(MyGameTreeCtrl, CMyBtnTreeCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

HTREEITEM MyGameTreeCtrl::InsertItem( const TREENODE& treeNode ,HTREEITEM hParent,HTREEITEM hAfter)
{
	if( treeNode.bIsLeaf )
	{
		if( treeNode.strImage1 == "")
		//old item
		{
			HTREEITEM hTreeItem = CMyTreeCtrl::InsertItem( treeNode.strText,treeNode.strImage2,hParent,hAfter );
			SetItemData( hTreeItem,treeNode.iIndex );
			return hTreeItem;
		}
		else
		{
			HTREEITEM hTreeItem = CMyTreeCtrl::InsertItem( treeNode.strText,treeNode.strImage1,hParent,hAfter );
			if( hTreeItem != NULL )
			{
				m_mapNode[hTreeItem] = make_pair( treeNode.strImage1,treeNode.strImage2 );
			}
			SetItemData( hTreeItem,treeNode.iIndex );
			return hTreeItem;
		}
	}
	else
	{
		HTREEITEM hTreeItem = CMyTreeCtrl::InsertItem( treeNode.strText,treeNode.strImage1,hParent,hAfter );
		if( hTreeItem != NULL )
		{
			m_mapNode[hTreeItem] = make_pair( treeNode.strImage1,treeNode.strImage2 );
		}
		return hTreeItem;
	}
}

int MyGameTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMyBtnTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	LoadSkin();

	m_tootip.m_pClientWnd = this;
	m_tootip.CreateEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST,TOOLTIPS_CLASS,"",0,CRect(0,0,0,0),NULL,0);
	m_tootip.m_hParent = GetSafeHwnd();
	m_tootip.m_clrBK = RGB(255, 255, 225);	

	AfxGetUIManager()->UIGetSkinMgr()->AddSkinWnd( this );

	return 0;
}

void MyGameTreeCtrl::OnDestroy()
{
	m_tootip.DestroyWindow();
	__super::OnDestroy();
}

void MyGameTreeCtrl::LoadSkin()
{
	while(m_ImageList.GetImageCount() > 0)
	{
		m_ImageList.Remove(0);
	}

	for(vector<CBitmap*>::iterator iter=m_vecBitmap.begin(); iter!=m_vecBitmap.end(); iter++)
	{
		(*iter)->DeleteObject();
		delete (*iter);
	}
	m_vecBitmap.clear();
	m_mapBitmap2Index.clear();

	CDC*	pDC = GetDC();

	ISkinMgr*	pSkinMgr = AfxGetUIManager()->UIGetSkinMgr();

	SetScrollBitmap(pSkinMgr->GetDibBmp("ScrollVert"), pSkinMgr->GetDibBmp("ScrollHorz"));

	CDibBitmap* pDibBitmap = pSkinMgr->GetDibBmp( "IconTreeNodeCategory" );
	CBitmap* pBitmap1 = new CBitmap();
	pBitmap1->Attach( pDibBitmap->CreateBitmap( pDC ) );
	m_vecBitmap.push_back( pBitmap1);

	m_ImageList.Add( pBitmap1 ,RGB(0,0,0));

	m_mapBitmap2Index["IconTreeNodeCategory"] = 0;

	SetImageList( &m_ImageList,TVSIL_NORMAL );
	SetButtonBitmap( pSkinMgr->GetDibBmp("QQTreeBtnSCNormal"),pSkinMgr->GetDibBmp("QQTreeBtnSCOver"),pSkinMgr->GetDibBmp("QQTreeBtnSCDown") );
	ReleaseDC(pDC);

	m_colorNormalItemText = pSkinMgr->GetColor("QQTreeNorText");
	m_colorSelectItemBK   = pSkinMgr->GetColor("QQTreeSelBk");
	m_colorSelectItemText = pSkinMgr->GetColor("QQTreeSelText");
	SetBackColor( pSkinMgr->GetColor("QQTreeBk"));
}

void MyGameTreeCtrl::ClickTreeItem( HTREEITEM treeItem )
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);	

	m_iCurItem = (int)GetItemData(treeItem);

	if( m_BtnRect.PtInRect(pt) )
	{
		//显示右键
	}
	else
	{
		if( m_mapNode.find( treeItem) != m_mapNode.end() )
		{			
			//GLOBAL_QQDATA->IQQData_ChangeItem2Old( m_iCurItem );
			//SetItemImage( treeItem,m_mapBitmap2Index[m_mapNode[treeItem].second],m_mapBitmap2Index[m_mapNode[treeItem].second] );
			m_mapNode.erase( treeItem );
		}
		SearchTreeItem( treeItem );
	}
}

void MyGameTreeCtrl::SelectTreeItem(int index )
{
	HTREEITEM temp = GetRootItem();
	HTREEITEM hTree = NULL;

	while( temp != NULL)
	{
		FindItem( index,temp,hTree);
		if( hTree != NULL )
		{
			break;
		}
		temp = GetNextItem(temp,TVGN_NEXT);
	}

	if( hTree != NULL )
	{
		SelectItem( hTree );
		EnsureVisible(hTree);

		if( m_mapNode.find( hTree) != m_mapNode.end() )
		{			
			//GLOBAL_QQDATA->IQQData_ChangeItem2Old( m_iCurItem );
			//SetItemImage( hTree,m_mapBitmap2Index[m_mapNode[hTree].second],m_mapBitmap2Index[m_mapNode[hTree].second] );
			m_mapNode.erase( hTree );
		}

	}
}

void MyGameTreeCtrl::FindItem(int index,HTREEITEM treeRoot,HTREEITEM& treeItem)
{
	DWORD i = GetItemData(treeRoot);
	if( i == index )
	{
		treeItem = treeRoot;
	}
	HTREEITEM temp = GetNextItem(treeRoot,TVGN_CHILD);
	while( temp != NULL )
	{		
		FindItem(index, temp, treeItem);
		if( treeItem != NULL )
		{
			break;
		}
		temp = GetNextItem(temp,TVGN_NEXT);
	}
}

void MyGameTreeCtrl::SearchTreeItem( HTREEITEM htree)
{	
	int iItemIndex = (int)GetItemData(htree);

	if( iItemIndex >= 0 && iItemIndex < m_vecItem.size() )
	{
		//如果是收藏的游戏，要看是否已登陆
		if( m_vecItem[iItemIndex].eumType == TAB_COLLECTED_GAME )
		{
			if( GLOBAL_USERDATA->User_IsLogin() == FALSE )
			{
				IUserMan::User_Login();
				return;
			}
		}
		GLOBAL_TABBARDATA->ITabBar_ChangeTab(m_vecItem[iItemIndex]);
	}	
}
void MyGameTreeCtrl::DClickTreeItem( HTREEITEM htree)
{
	if(ItemHasChildren(htree))
		return ;
	m_iCurItem = GetItemData(htree);
	SearchTreeItem( htree );
}

void MyGameTreeCtrl::RClickTreeItem( HTREEITEM htree)
{
	//右键不处理
}

void MyGameTreeCtrl::ClearData()
{
	CMyTreeCtrl::ClearData();
	m_mapNode.clear();
}

void MyGameTreeCtrl::CreateTree()
{
	DeleteAllItems();
	m_vecItem.clear();
	//获取数据
	//TAB_ITEM ti;
	//ti.strName = "我的游戏";
	//ti.strParam = "";
	//ti.eumType  = TAB_PLAYED_GAME;
	//m_vecItem.push_back( ti );

	//ti.strName  = "我收藏的游戏";
	//ti.strParam = "";
	//ti.eumType  = TAB_COLLECTED_GAME;
	//m_vecItem.push_back( ti );
	//////////
	//TREENODE	treeNode;

	//treeNode.iIndex		= 0;
	//treeNode.bIsLeaf	= true;

	//string strItemName;
	//unsigned int iCount = GLOBAL_LOCALGAME->ILocalGameData_GetGameCount();
	//YL_StringUtil::Format( strItemName, "%s(%d)", "我的游戏", iCount );

	//treeNode.strText = strItemName;
	//treeNode.strImage1 = "IconTreeNodeCategory";
	//treeNode.strImage2 = "IconTreeNodeCategory";
	//InsertItem( treeNode,NULL);
}

void MyGameTreeCtrl::RefreshTree(string* pstrPanelName)
{
	m_mapNode.clear();
	CreateTree();
	m_bInsertData = true;
	EnsureVisible(GetRootItem());
}

void MyGameTreeCtrl::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if( !m_bInsertData )
	{
		RefreshTree();		
	}

	CMyBtnTreeCtrl::OnShowWindow(bShow, nStatus);		
}

HTREEITEM MyGameTreeCtrl::GetTreeItem(int index)
{
	HTREEITEM temp = GetRootItem();
	HTREEITEM hTree = NULL;

	while( temp != NULL)
	{
		FindItem( index,temp,hTree);
		if( hTree != NULL )
		{
			break;
		}
		temp = GetNextItem(temp,TVGN_NEXT);
	}

	return hTree;
}

void MyGameTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove(nFlags,point);

	HTREEITEM htree = HitTest(point);	
	if( ItemHasChildren(htree) )
	{
		return;
	}
	int iIndex = GetItemData(htree);

	if( iIndex != 0 )//目前只有一个“我的游戏”
		return;
	CString info;
	info.Format("名称: 我的游戏$#这里的游戏无需下载，断网照样能玩。" );
	
	CPoint pt(0,0);
	GetCursorPos(&pt);
	if( m_tootip.m_lastPoint != pt )
	{
		m_tootip.HideTooTips();
	}
	m_tootip.ShowToolTips( info,pt);
}

LPARAM MyGameTreeCtrl::OnMouseLeave(WPARAM wp, LPARAM lp)
{
	m_bTrackMouse = FALSE;
	m_tootip.HideTooTips();	
	return __super::OnMouseLeave(wp,lp);
}

void MyGameTreeCtrl::IQQItemChangeOb_CountChange( unsigned int iNewCount )
{
	RefreshTree();
}