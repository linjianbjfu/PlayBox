#pragma once

#include "MyListCtrlData.h"
#include "../../gui/util/WToolTips.h"

#include <map>
#include <vector>
using std::map;
using std::vector;


class CMyListCtrl :
	public CListCtrl,
	public CBasicSkinWnd
{

public:
	COLORREF	m_clrOddItemBK;		
	COLORREF	m_clrEvenItemBK;		
	COLORREF	m_clrNormalItemText;
	COLORREF	m_clrSelItemBK;	
	COLORREF	m_clrSelItemText;	

	CDibBitmap*	m_pBitmapScrollVert;
	CDibBitmap*	m_pBitmapScrollHorz;

	CFont	m_font;
	int		m_iOffset;

	int		m_iCurrentCursorItem;
	int		m_iCurrentCursorSubItem;
	int		m_iOverImageItem;
	int		m_iOverImageSubItem;
	BOOL	m_bTrackMouseEvent;

	map<int,MYLIST_COLUMN_INFO>	m_mapColumnInfo;	
	CCWToolTips		m_tooltip;

public:
	bool	 m_bAsc;	
	int		 m_iSortItem;
	bool	 m_bInit;	
	vector<int> m_vecSearchedItems;
	COLORREF m_colorSearchHightLight;
	COLORREF m_clrPlayItem;

public:
	CMyListCtrl(void);
	virtual ~CMyListCtrl(void);
	DECLARE_MESSAGE_MAP()

	afx_msg void	OnDestroy();
	afx_msg int		OnCreate(LPCREATESTRUCT);
	afx_msg BOOL	OnEraseBkgnd(CDC*);
	afx_msg void	MeasureItem(LPMEASUREITEMSTRUCT lpmis);
	afx_msg void	DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct); 
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT	OnMouseLeave(WPARAM,LPARAM);
	afx_msg void	OnPaint();
	afx_msg void	OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult);

	void	Init();			
	afx_msg void	OnCoolSBCustomdraw(NMHDR* pNMHDR, LRESULT* pResult );

public:
	void	SetColor( COLORREF,COLORREF,COLORREF,COLORREF,COLORREF);
	void	SetScrollBitmap(CDibBitmap*,CDibBitmap*);
	void	SetColumnInfo(const vector<MYLIST_COLUMN_INFO>& vecColumn );
	void	HideScrollBars(int Which);
	void	SearchHighLight(vector<int> &vecItems,COLORREF &colorHighLight);

	
	static int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	
	virtual void	LoadSkin(){};
	virtual void	CreateControl( int iID,CWnd* pParent);
	virtual void	DrawTextColumn(CDC* pDC,int iItem,int iColumn,const CRect& rect );
	virtual void	DrawImageColumn(CDC* pDC,int iItem,int iColumn,const CRect& rect,BOOL bSelected );
	virtual BOOL	NeedChangeCursor(CPoint& pt);
	virtual void	InitColumnInfo() = 0;
	virtual void	SetItemTextColor( CDC* pDc,BOOL bHighlight,int iItem ) ;
	virtual void	SetItemBkColor( CDC* pDC,BOOL bHighlight,int iItem,const CRect& rect );

	virtual void	ShowItemToolTip( int iItem,int iSubItem ){};
	virtual void	ShowSubItemToolTip(int iItem,int iSubItem ){};
	virtual void	HideToolTip( );
	virtual int		SortFunction(int iItem1,int iItem2){return 0;}; 
	virtual void    SetKeySubItem(int nIndex);
	virtual void	GetItemIdxByKey(string strKey,vector<int> &vecIdx){};

protected:
	void	DrawImage( CDibBitmap* pDibBitmap, const CRect &r, CDC* memDC );
	BOOL	PointInSubitemImage(CPoint& pt);
	BOOL	PointInOperatorBtn(CPoint& pt);
	bool	m_bDrawHoverRow;	//是否有鼠标hover状态
// 	bool	m_bDrawItemTopLine;	//true就画横线，否则不画
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
	//int  Ctrol_a_z_KeyDown(short nKey);
	
	int m_nKeySubItem;

};
