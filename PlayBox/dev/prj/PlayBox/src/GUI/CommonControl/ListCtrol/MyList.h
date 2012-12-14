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
enum BMP_SHOW_TYPE { NORMAL=1,SINGLE_BMP=2,SELECT_DRAW=4,SINGLE_MULT_BMP=8,MULTI_BMP=16 }; // ͼƬ��ʾ���ͣ�������3��״̬���̶���ͼ��ѡ�������ʾ
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
	int    m_headHiht;		    // ��ͷ�߶�
	int    m_itemHeight;		// ÿ��Ŀ��
	int    m_itemBigHeight;	    // �����

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
	// �����
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
	//ѡ�����
	virtual void GetSelected( vector<int>& selected, int& count );
	virtual int  GetPreSelected();
	virtual POSITION GetFirstSelectedItemPosition();   // ΪͬWINDOWS �ӿ���ͬ
	virtual int GetNextSelectedItem(POSITION& position);
	bool IsSelected(UINT nIndex);  // ĳ���Ƿ�ѡ�� 
	virtual void	SetColumnBitmapShowMode(UINT nColumn,int nShowPos, int nShowType);
	virtual void	SetColumnTextShowMode(UINT nColumn,int nShowPos, int nShowType);
	virtual void	SetColumnBitmap( int nRow, int nType, CDibBitmap* bmp);   // ����: �У� ����ͼƬ���ͱ�ţ� ͼƬ

	//��ʾ��ɫ
	virtual void	SetTextColor(COLORREF colText, COLORREF colSelText);
	virtual void	SetBKcolor( COLORREF bk1,COLORREF bk2,COLORREF select);
	virtual void	SetHeadTextColor( COLORREF col );
	
	//λ�����
	virtual bool	PointInItemRect(CPoint& point);
	virtual bool	PointInItemRect(CPoint& point,int& nLine, int& nRow);
	virtual bool    PointInItemBmpRect(CPoint& point, int& nLine, int& nRow, int& bmpIndex ); //bmpIndex ��ţ� ����ÿ��ֻһ��ͼƬ,��ʹ�� Ĭ��0
	virtual bool    PointInItemTextRect(CPoint& point, int& nLine, int& nRow, int& bmpIndex ); 
	virtual void	GetBmpRect( int nLine, int nRow,int nBmpPosIndex, CRect& rect );
	
	// ��ͷ���
	virtual void    GetColumnText(int index, CString& str);
	virtual void    SetColumnText(int index, CString str);
	virtual void    SetColumnData(int index, CString str,int width, int minWidth=5 );

	//����ͼƬ
	virtual void	SetScrollBitmap(CDibBitmap*,CDibBitmap*);
	virtual void    SetHeadBitmap( CDibBitmap* pBk,CDibBitmap* pMidLine,CDibBitmap* pAsc,CDibBitmap* pDesc);

	// ������������ָ��
	virtual void	SetSortIndex( vector<int>* pIndexData);

	// ��̬�ı����
	void ChangeItemShowBmpIndex( int line, int row, int nbmp =0, int nbmpIndex=0 ); //nbmp ��ͬλ�õ�ͼƬ, ���ֺ�ֻ��һ��λ��Ĭ��0
	void ChangeItmeBmpMouseTrack( int line, int row,bool bTrack );
	
	
    //����
	virtual void	LoadSkin(){};
};