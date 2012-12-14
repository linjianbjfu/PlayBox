#pragma once
#include "../../gui/CommonControl/MyBtnTreeCtrl.h"
#include "../../gui/CommonControl/BasicSkinWnd.h"
#include "../../DataInterface/ITabBarData.h"
#include "../../datainterface/IQQItemCountChangeObserver.h"
#include <utility>
using std::pair;

#define WM_MYGAME_TREE_EXPAND	WM_USER + 101

class MyGameTreeCtrl : public CMyBtnTreeCtrl, 
					   public CBasicSkinWnd,
					   public IQQItemCountChangeObserver
{
public:
	struct TREENODE
	{
		string	strImage1;
		string	strImage2;
		string	strText;
		bool	bIsLeaf;
		int		iIndex;
	};

public:
	MyGameTreeCtrl( CDibBitmap* pBit = NULL );
	~MyGameTreeCtrl(void);

	void	LoadSkin();
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT		OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void		OnDestroy();
	afx_msg int			OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void		OnMouseMove(UINT nFlags, CPoint point);	
public:
	HTREEITEM	InsertItem( const TREENODE&,HTREEITEM hParent,HTREEITEM hAfter = TVI_LAST);
	void		CreateTree();
	void		RefreshTree(string* pStr = NULL );
	void		ClearData();
	void		SelectTreeItem(int index );
	HTREEITEM	GetTreeItem(int index);

public:
	void	FindItem(int index,HTREEITEM treeRoot,HTREEITEM& treeItem);
	void	ClickTreeItem(HTREEITEM );
	void	DClickTreeItem( HTREEITEM );
	void	RClickTreeItem( HTREEITEM );
	void	SearchTreeItem( HTREEITEM );

private:	
	CCWToolTips		m_tootip;

	bool			m_bInsertData;
	typedef map<HTREEITEM,pair<string,string> > MAP_NODE;
	MAP_NODE		m_mapNode;
	int				m_iCurItem;
	vector<TAB_ITEM>	m_vecItem;
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	CString		m_strWindowName;	
	virtual void IQQItemChangeOb_CountChange( unsigned int iNewCount );
};
