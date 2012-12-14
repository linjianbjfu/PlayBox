#pragma once

class CMyList;
class CDrawList
{
public:
	CDrawList(void);
	~CDrawList(void);
private:
	CMyList* mpList;

	COLORREF m_colorBK1;
	COLORREF m_colorBK2;
	COLORREF m_colorSelect;
	COLORREF m_colorText;
	COLORREF m_colorSelText;
	
	COLORREF m_colorLyric1;
	COLORREF m_colorLyric2;


	CFont  m_textFont;
	CFont  m_textFontUnderLine;// 有下划线的字体

	int m_maxX;  
	int m_maxY;  // x,y最大坐标， 用于draw 优化
public:
	void AttachList(HWND hwnd);
	virtual void DrawARow( int index);
	virtual void DrawARow(  CDC* pdc, int& index, int& maxX, int& maxY);
	virtual void DrawAItemText( int& index, int& nRow,int ntype=0);
	virtual void DrawSelectFrame( CPoint& point1, CPoint& point2 );
	virtual void DrawItem( CDC* pdc,int&index, int& nRow, CRect& lrect, int& maxX, int& maxY );
	virtual void DrawItemBmp( int& index, int& nRow,int ntype=0);
	virtual void DrawList( CDC* pdc,int& maxX, int& maxY);
	virtual void DrawItemBK(CDC* pdc,CRect& rect,int& nIndex );
	virtual void DrawEndBK( CDC* pdc,int& maxX, int& maxY, int top, int lineWidth);
	virtual void DrawListFromIndex();

	virtual void SetTextCorlor(COLORREF& col,COLORREF& selCol);
	virtual void SetBkCorlor( COLORREF& bk1,COLORREF& bk2, COLORREF& select );
	virtual void DrawLyric(CDC* pdc, CString& str,	CRect& rect );
};
