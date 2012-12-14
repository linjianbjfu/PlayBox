#pragma once


// CMyList

#define BMP_NO     (-1)
#define BMP_NORMAL (0)
#define BMP_OVER   (1)
#define BMP_SELECT (2)

class CScrollControl;
class CHeadCtrol;
class CListData;
class CDrawList;
class CSortControl;
class CSelectControl;

#include<vector>
using namespace std;

enum BMP_SHOW_POS  { CENTER, TEXT_LEFT, TEXT_RIGHT,LEFT,RIGHT };
enum BMP_SHOW_TYPE { NORMAL=1,SINGLE_BMP=2,SELECT_DRAW=4,SINGLE_MULT_BMP=8,MULTI_BMP=16 }; // 图片显示类型：正常，3种状态；固定单图；选择后在显示
enum TEXT_SHOW_TYPE  { NORMAL_TEXT=1,LINK_TEXT=2 };

class CMyList : public CWnd,public CBasicSkinWnd
{
	DECLARE_DYNAMIC(CMyList)

public:
	CMyList();
	virtual ~CMyList();
	friend class CDrawList;
	friend class CSortControl;
	friend class CBitmapRectData;
	friend class CTextRectData;
	BOOL CreateList(CRect& rect, HWND parentWnd, int ID);
	int m_nhoverIndex;
	int m_nPrehoverIndex;
private:
	CScrollControl* m_pScrollInfo;
	CSortControl*   m_pSort;
	CHeadCtrol*     m_pHead;
	CListData*      m_pData;
	CDrawList*      m_pDraw;
	CSelectControl* m_pSelect;

	friend class CSelectControl;
public:
	CListData*      GetListData()const;
	CSelectControl* GetSelectControl()const;
	CDrawList*      GetDrawList()const;
	CHeadCtrol*     GetHeadWnd()const;
private:
	bool m_bHasHscrollBar;
	bool m_bPreHasHscrollBar;
	bool mbMouseOnList;

	CFont  m_textFont;
	int    m_headHiht;		    // 表头高度
	int    m_itemHeight;		// 每项目高
	int    m_itemBigHeight;	    // 大项高

	int    m_DisplayX;
	int    m_DisplayY;

	int				m_nPreSelected;
	vector<UINT>    m_SelectedVector;
	bool            m_buttonDown;
	bool            m_endMoveSel;

	struct   Struct_mouseMoveSel
	{
		vector<UINT> selectedVector;
		int         begainItem;
		int         endItem;
		CPoint      pointBegain;
		CPoint      pointEnd;
		CPoint		pointEndPre;

		Struct_mouseMoveSel()
		{
			begainItem = -1;
			endItem    = -1;
		}
		void SetEmpty()
		{
			begainItem = -1;
			endItem = -1;
		}
		void SetPointEnpty()
		{
			pointBegain.SetPoint(0,0);
			pointEnd.SetPoint(0,0);
		}
	};
	Struct_mouseMoveSel m_structMousemoveSel;
	int  PointToVirtualIndex(CPoint& point);
	int  PointToRow(CPoint& point);
	void PointInItem(CPoint& point,int& line, int& row);
	void ScrollToALine(UINT nLine, bool bDown);

	const CSize* GetScrollPos2();
	int   GetBotton()const;
	int   GetRight()const;
	void  SetSelected(int sel);
	vector<UINT>* GetCtrlKeySelected( int& count );
	void  CtrolMouseMove(UINT& nFlags,CPoint& point);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg LRESULT OnSort( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnChangeScrool( WPARAM wParam, LPARAM lParam );
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCoolSBCustomdraw(NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcMButtonDblClk(UINT nHitTest, CPoint point);

	afx_msg LRESULT OnMouseLeaveList(WPARAM   wParam,   LPARAM   lParam) ;
	afx_msg LRESULT OnMouseHoverList(WPARAM   wParam,   LPARAM   lParam) ;

	afx_msg LRESULT OnMouseLeaveButton(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHoverButton(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnMouseLeaveItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHoverItem(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnMouseLeaveText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHoverText(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual BOOL OnMouseLeaveButton( int line ,int row ){return TRUE;};
	virtual BOOL OnMouseHoverButton( int line ,int row ){return TRUE;};

	virtual void OnMouseHoverItem( int line , int row){};
	virtual void OnMouseLeaveList( ){};


protected:
	CDibBitmap*	m_pBitmapScrollVert;
	CDibBitmap*	m_pBitmapScrollHorz;

public:
	BOOL	 m_bInit;
	// 项相关
	virtual int  InsertColumn( CString strName, int width, int lessWidth=0, bool sort= false );
	virtual void DeleteAllColumn();
	virtual HWND GetHeadHwnd()const;
	virtual void SetItemHight(UINT hight);
	virtual UINT GetItemRight(int pos)const;
	virtual UINT GetLastRowRight()const;
	virtual UINT GetItemLeft(int pos)const;
	virtual void InvalidateList();
	virtual UINT GetLineSize();
	virtual int  GetItemBmpType(int nLine, int nRow);
	virtual void DrawItemBmp(int nLine, int nRow, int bmpType);

	virtual int	 InsertItem();
	virtual int  InsertItem(UINT	index);
	virtual void DeleteAllItem();
	virtual bool SetItemText( UINT index,UINT nSubItem, CString lpszText); 
	virtual bool SetItemText( UINT index,UINT nSubItem, CString lpszText,CString subRowText ); 
	virtual bool SetItemData( UINT index,UINT nSubItem, CString lpszText,CString subRowText,int nbmpType, bool bmpmouseTrack=false );
	virtual bool SetItemData( UINT index,UINT nSubItem, CString lpszText,int nbmpType, bool bmpmouseTrack=false );
	virtual bool GetItemData(UINT nLine, UINT nRow, CString& str )const;
	virtual bool GetItemData(UINT nLine, UINT nRow, CString& str,CString& strEx)const; 
	const char*  GetItemStrPtr(UINT& nLine, UINT& nRow);

	virtual void    SetItemCount( int count){;}
	virtual bool UpdataIndex(UINT nIndex=0);
	//选择相关
	virtual void GetSelected( vector<int>& selected, int& count );
	virtual int  GetPreSelected();
	virtual POSITION GetFirstSelectedItemPosition();   // 为同WINDOWS 接口相同
	virtual int GetNextSelectedItem(POSITION& position);
	bool IsSelected(UINT nIndex);  // 某列是否选择 
	virtual void	SetColumnBitmapShowMode(UINT nColumn,int nShowPos, int nShowType);
	virtual void	SetColumnTextShowMode(UINT nColumn,int nShowPos, int nShowType);
	virtual void	SetColumnBitmap( int nRow, int nType, CDibBitmap* bmp);   // 参数: 列， 列内图片类型编号， 图片

	//显示颜色
	virtual void	SetTextColor(COLORREF colText, COLORREF colSelText);
	virtual void	SetBKcolor( COLORREF bk1,COLORREF bk2,COLORREF select);
	virtual void	SetHeadTextColor( COLORREF col );
	
	//位置相关
	virtual bool	PointInItemRect(CPoint& point);
	virtual bool	PointInItemRect(CPoint& point,int& nLine, int& nRow);
	virtual bool    PointInItemBmpRect(CPoint& point, int& nLine, int& nRow, int& bmpIndex ); //bmpIndex 编号， 现在每项只一个图片,不使用 默认0
	virtual bool    PointInItemTextRect(CPoint& point, int& nLine, int& nRow, int& bmpIndex ); 
	virtual void	GetBmpRect( int nLine, int nRow,int nBmpPosIndex, CRect& rect );
	
	// 表头相关
	virtual void    GetColumnText(int index, CString& str);
	virtual void    SetColumnText(int index, CString str);
	virtual void    SetColumnData(int index, CString str,int width, int minWidth=5 );

	//设置图片
	virtual void	SetScrollBitmap(CDibBitmap*,CDibBitmap*);
	virtual void    SetHeadBitmap( CDibBitmap* pBk,CDibBitmap* pMidLine,CDibBitmap* pAsc,CDibBitmap* pDesc);

	// 设置排序数组指针
	virtual void	SetSortIndex( vector<int>* pIndexData);

	// 动态改变相关
	void ChangeItemShowBmpIndex( int line, int row, int nbmp =0, int nbmpIndex=0 ); //nbmp 不同位置的图片, 音乐盒只有一个位置默认0
	void ChangeItmeBmpMouseTrack( int line, int row,bool bTrack );
	
	
    //换肤
	virtual void	LoadSkin(){};
};