#include "StdAfx.h"
#include "drawlist.h"
#include "mylist.h"
#include "listdata.h"

CDrawList::CDrawList(void)
:mpList(NULL),
m_colorBK1( RGB(244,247,244) ),
m_colorBK2(	RGB(255,255,255) ),
m_colorSelect( RGB(213,224,213) ),
m_colorText( RGB(0, 0, 0 ) ),
m_colorSelText( RGB(0, 0, 0 ) ),
m_colorLyric1( RGB( 160,160, 162 ) ),
m_colorLyric2( RGB( 0xe0, 0xa1, 0x75 ) ),
m_maxX(0),
m_maxY(0)
{
}

CDrawList::~CDrawList(void)
{
	m_textFont.DeleteObject();
	m_textFontUnderLine.DeleteObject();
}
void CDrawList::AttachList(HWND hwnd)
{
	if( hwnd==NULL ) return;
	mpList = (CMyList*)CWnd::FromHandle( hwnd );
}
void CDrawList::DrawSelectFrame( CPoint& point1, CPoint& point2 )
{
	if( point1 == CPoint(0,0) || point2 == CPoint(0,0)  ) return;

	CRect lrect;
	lrect.SetRectEmpty();
	if(( point2.y > point1.y )&&( point2.x > point1.x) )   //3
	{
		lrect.SetRect( point1.x, point1.y ,point2.x, point2.y );
	}
	else if(( point2.y > point1.y )&&( point2.x < point1.x) ) //4
	{
		lrect.SetRect( point2.x, point1.y, point1.x, point2.y );
	}
	else if( ( point2.y < point1.y)&& ( point2.x < point1.x) ) // 1
	{
		lrect.SetRect( point2.x, point2.y ,point1.x, point1.y );
	}
	else if( ( point2.y < point1.y)&& ( point2.x > point1.x) )//2
	{
		lrect.SetRect( point1.x, point2.y, point2.x, point1.y );
	}
	CDC* pdc = mpList->GetDC();

	AfxGdiOperator()->DrawPointFrameXOR( pdc->m_hDC, lrect );
	ReleaseDC(mpList->m_hWnd, pdc->m_hDC);
}
void CDrawList::DrawARow(int index)
{
	if( index == -1 ) return ;
	CDC* pdc = mpList->GetDC();
	CRect rect(0,0, m_maxX,m_maxY );	
	DrawARow( pdc, index, m_maxX, m_maxY );
	ReleaseDC( mpList->m_hWnd, pdc->m_hDC );
}
void CDrawList::DrawARow( CDC* pdc, int& index, int& maxX, int& maxY)
{
	CRect lrect;
	lrect.SetRectEmpty();
	if( mpList->GetListData()->GetRowInfo(index) == NULL) return;
	lrect.top = ( index-mpList->GetListData()->GetRowInfo(index)->frontBigCout)* mpList->m_itemHeight
		+(mpList->GetListData()->GetRowInfo(index)->frontBigCout)* mpList->m_itemBigHeight;

	if(mpList->GetListData()->GetRowInfo(index)->strLineText.GetLength() == 0 )
	{
		lrect.bottom = lrect.top + mpList->m_itemHeight;
	}
	else
	{
		lrect.bottom = lrect.top + mpList->m_itemBigHeight;
	}
	lrect.right = mpList->GetItemRight(-1);
	lrect.top	 -= mpList->GetScrollPos2()->cy;	
	if( lrect.top < -mpList->m_itemBigHeight ) return;
	lrect.bottom -= mpList->GetScrollPos2()->cy;	

	CRect lrectClent;
	mpList->GetClientRect(&lrectClent);
	if( lrect.top > lrectClent.bottom+mpList->m_itemBigHeight ) return;
	lrectClent.top = lrect.top;
	lrectClent.bottom = lrect.bottom;

	CMemDC dcMem(pdc,lrectClent);


	DrawItemBK( &dcMem, lrectClent, index );
	if(mpList->GetListData()->GetRowInfo(index)->strLineText.GetLength() != 0 )
	{
		CRect lrectSub;
		lrectSub.left = mpList->GetItemLeft(  0  ) ;
		lrectSub.right = mpList->GetLastRowRight(); 
		lrectSub.top = lrect.top + mpList->m_itemHeight;
		lrectSub.bottom = lrect.top + mpList->m_itemBigHeight;
		DrawLyric(&dcMem, mpList->GetListData()->GetRowInfo(index)->strLineText, lrectSub );
	}

	int count = mpList->GetListData()->GetRowInfo(index)->rowData.size();		
	int i;
	for(  i=0; i<count; i++)
	{
		lrect.left = mpList->GetItemLeft(  i  ) ;
		lrect.right = mpList->GetItemRight( i );
		lrect.bottom = lrect.top + mpList->m_itemHeight;
		DrawItem( &dcMem,index, i, lrect, maxX, maxY );
	}
}


void CDrawList::DrawItem( CDC* pdc,int& index, int& nRow, CRect& lrect, int& maxX, int& maxY )
{
	if( mpList->GetListData()->GetRowInfo(index) == NULL) return;
	lrect.left += 2;
	if( !(( mpList->GetListData()->GetRowBmpShowType(nRow) == SINGLE_MULT_BMP )|| ( mpList->GetListData()->GetRowBmpShowType(nRow) == MULTI_BMP ))  )
	{
		if( mpList->GetListData()->GetRowInfo(index)->Selected )
		{
			AfxGdiOperator()->DrawTextQuick( pdc, mpList->GetListData()->GetRowInfo(index)->rowData[nRow].strText,m_colorSelText, lrect, m_textFont, maxX, maxY );
		}
		else
		{
			AfxGdiOperator()->DrawTextQuick( pdc, mpList->GetListData()->GetRowInfo(index)->rowData[nRow].strText,m_colorText, lrect, m_textFont, maxX, maxY );
		}
	}
	CRect bmpRect;
	switch( mpList->GetListData()->GetRowBmpShowType(nRow) )
	{
	case NORMAL:
		{
			mpList->GetListData()->GetbmpRectData()->GetBitmapRect(bmpRect,index, nRow );
			bmpRect.top	 -= mpList->GetScrollPos2()->cy;	
			bmpRect.bottom -= mpList->GetScrollPos2()->cy;
			if( !(( bmpRect.top>maxY)  || ( bmpRect.bottom<0 ) || ( bmpRect.left >maxX ) || ( bmpRect.right<0 )))
			{
				//  画 pair<int , int> bmpItem; 
				//  pair<int, bool> bmpMouseTrack;
				if(  mpList->GetListData()->GetRowInfo(index)->rowData[nRow].bmpItem.second != -1 )
				{
					if( nRow >1000 )return ;
					CDibBitmap* pDibBitmap;
					if( mpList->GetListData()->GetRowInfo(index)->Selected )
					{
						pDibBitmap = mpList->GetListData()->GetbmpRectData()->GetBitmap(nRow,mpList->GetListData()->GetRowInfo(index)->rowData[nRow].bmpItem.second+2 );
					}
					else
					{
						pDibBitmap = mpList->GetListData()->GetbmpRectData()->GetBitmap(nRow,mpList->GetListData()->GetRowInfo(index)->rowData[nRow].bmpItem.second );
					}
					if( pDibBitmap )
					{
						pDibBitmap->SetCDibRect( bmpRect );
						pDibBitmap->Draw(pdc, true );
					}
				}
			}
			break;
		}
	case SINGLE_BMP:
		{
			mpList->GetListData()->GetbmpRectData()->GetBitmapRect(bmpRect,index, nRow );
			bmpRect.top	 -= mpList->GetScrollPos2()->cy;	
			bmpRect.bottom -= mpList->GetScrollPos2()->cy;
			if(! (( bmpRect.top>maxY)  || ( bmpRect.bottom<0 ) || ( bmpRect.left >maxX ) || ( bmpRect.right<0 )))
			{
				CDibBitmap* pDibBitmap;

				if( mpList->GetListData()->GetRowInfo(index)->Selected )
				{
					pDibBitmap = mpList->GetListData()->GetbmpRectData()->GetBitmap(nRow,2/* 因为音乐盒的控件每列图片相同所以 写死 GetRowBmpShowMode */ );
				}
				else
				{
					pDibBitmap = mpList->GetListData()->GetbmpRectData()->GetBitmap(nRow,0/* 因为音乐盒的控件每列图片相同所以 写死 GetRowBmpShowMode */ );
				}
				if( pDibBitmap )
				{
					pDibBitmap->SetCDibRect( bmpRect );
					pDibBitmap->Draw(pdc, true );
				}
			}
			break;
		}
	case SINGLE_MULT_BMP:
		{
			mpList->GetListData()->GetbmpRectData()->GetBitmapRect(bmpRect,index, nRow );
			bmpRect.top	 -= mpList->GetScrollPos2()->cy;	
			bmpRect.bottom -= mpList->GetScrollPos2()->cy;
			if(! (( bmpRect.top>maxY)  || ( bmpRect.bottom<0 ) || ( bmpRect.left >maxX ) || ( bmpRect.right<0 )))
			{
				int ngrade = atoi( mpList->GetListData()->GetRowInfo(index)->rowData[nRow].strText );
				CDibBitmap* pDibBitmap = mpList->GetListData()->GetbmpRectData()->GetBitmap(nRow,0 );
				if( pDibBitmap )
				{
					pDibBitmap->SetCDibRect( bmpRect );
					pDibBitmap->Draw(pdc, true );
				}
								
				CDibBitmap* pDibBitmap2 = mpList->GetListData()->GetbmpRectData()->GetBitmap(nRow,1 );
				
				if( pDibBitmap2 )
				{
					for( int i=0; i<ngrade; i++ )
					{
						CRect bmpRect2 = bmpRect;
						bmpRect2.left += i*4;
						bmpRect2.right = bmpRect2.left +3;
						pDibBitmap2->SetCDibRect( bmpRect2 );
						pDibBitmap2->Draw( pdc, true );
					}
				}
			}
			break;
		}

	case SELECT_DRAW:
		{
			if( mpList->GetListData()->GetRowInfo(index)->Selected)
			{
				mpList->GetListData()->GetbmpRectData()->GetBitmapRect(bmpRect,index, nRow );
				bmpRect.top	 -= mpList->GetScrollPos2()->cy;	
				bmpRect.bottom -= mpList->GetScrollPos2()->cy;
				if( !(( bmpRect.top>maxY)  || ( bmpRect.bottom<0 ) || ( bmpRect.left >maxX ) || ( bmpRect.right<0 )))
				{
					CDibBitmap* pDibBitmap;
					if( mpList->GetListData()->GetRowInfo(index)->Selected )
					{
						pDibBitmap = mpList->GetListData()->GetbmpRectData()->GetBitmap(nRow,2/* 因为音乐盒的控件每列图片相同所以 写死 GetRowBmpShowMode */ );
					}
					else
					{
						pDibBitmap = mpList->GetListData()->GetbmpRectData()->GetBitmap(nRow,0/* 因为音乐盒的控件每列图片相同所以 写死 GetRowBmpShowMode */ );
					}
					if( pDibBitmap )
					{
						pDibBitmap->SetCDibRect( bmpRect );
						pDibBitmap->Draw(pdc, true );
					}
				}
			}
			break;
		}
	case MULTI_BMP:
		{
			mpList->GetListData()->GetbmpRectData()->GetBitmapRect(bmpRect,index, nRow );
			bmpRect.top	 -= mpList->GetScrollPos2()->cy;	
			bmpRect.bottom -= mpList->GetScrollPos2()->cy;
			if(! (( bmpRect.top>maxY)  || ( bmpRect.bottom<0 ) || ( bmpRect.left >maxX ) || ( bmpRect.right<0 )))
			{				
				if(  mpList->GetListData()->GetRowInfo(index)->rowData[nRow].bmpItem.second != -1 )
				{
					CDibBitmap* pDibBitmap;
					if( mpList->GetListData()->GetRowInfo(index)->Selected )
					{
						pDibBitmap = mpList->GetListData()->GetbmpRectData()->GetBitmap(nRow,mpList->GetListData()->GetRowInfo(index)->rowData[nRow].bmpItem.second+2 );
					}
					else
					{

						pDibBitmap = mpList->GetListData()->GetbmpRectData()->GetBitmap(nRow,mpList->GetListData()->GetRowInfo(index)->rowData[nRow].bmpItem.second );

					}
					if( pDibBitmap )
					{
						pDibBitmap->SetCDibRect( bmpRect );
						pDibBitmap->Draw(pdc, true );
					}
				}
			}
			break;
		}
	default: break;
	}
}
void CDrawList::DrawItemBmp( int& index, int& nRow,int ntype)
{
	if( mpList->GetListData()->GetRowInfo(index) == NULL) return;
	CDC* pdc = mpList->GetDC();
	CRect bmpRect;

	switch( mpList->GetListData()->GetRowBmpShowType(nRow) )
	{
	case NORMAL:
		{
			mpList->GetListData()->GetbmpRectData()->GetBitmapRect(bmpRect,index, nRow );
			bmpRect.top	  -= mpList->GetScrollPos2()->cy;	
			bmpRect.bottom -= mpList->GetScrollPos2()->cy;
			DrawItemBK( pdc, bmpRect,index );
			//  画 pair<int , int> bmpItem; 
			//  pair<int, bool> bmpMouseTrack;
			if(  mpList->GetListData()->GetRowInfo(index)->rowData[nRow].bmpItem.second != -1 )
			{
				CDibBitmap* pDibBitmap = mpList->GetListData()->GetbmpRectData()->GetBitmap(nRow,ntype/* 因为音乐盒的控件每列图片相同所以 写死 GetRowBmpShowMode */ );
				if( pDibBitmap )
				{
					pDibBitmap->SetCDibRect( bmpRect );
					pDibBitmap->Draw(pdc, true );
				}
			}
			break;
		}
	case SINGLE_BMP:
		{
			mpList->GetListData()->GetbmpRectData()->GetBitmapRect(bmpRect,index, nRow );
			bmpRect.top	 -= mpList->GetScrollPos2()->cy;	
			bmpRect.bottom -= mpList->GetScrollPos2()->cy;
			DrawItemBK( pdc, bmpRect,index );
			CDibBitmap* pDibBitmap = mpList->GetListData()->GetbmpRectData()->GetBitmap(nRow,0/* 因为音乐盒的控件每列图片相同所以 写死 GetRowBmpShowMode */ );
			if( pDibBitmap )
			{
				pDibBitmap->SetCDibRect( bmpRect );
				pDibBitmap->Draw(pdc, true );
			}
			break;
		}
	case SINGLE_MULT_BMP:
		{
			mpList->GetListData()->GetbmpRectData()->GetBitmapRect(bmpRect,index, nRow );
			bmpRect.top	 -= mpList->GetScrollPos2()->cy;	
			bmpRect.bottom -= mpList->GetScrollPos2()->cy;
			DrawItemBK( pdc, bmpRect,index );
			int ngrade = atoi( mpList->GetListData()->GetRowInfo(index)->rowData[nRow].strText );
			CDibBitmap* pDibBitmap = mpList->GetListData()->GetbmpRectData()->GetBitmap(nRow,0 );
			if( pDibBitmap )
			{
				pDibBitmap->SetCDibRect( bmpRect );
				pDibBitmap->Draw(pdc, true );
			}
			CDibBitmap* pDibBitmap2 = mpList->GetListData()->GetbmpRectData()->GetBitmap(nRow,1 );
			if( pDibBitmap2 )
			{
				for( int i=0; i<ngrade; i++ )
				{
					CRect bmpRect2 = bmpRect;
					bmpRect2.left += i*4;
					bmpRect2.right = bmpRect2.left +3;
					pDibBitmap2->SetCDibRect( bmpRect2 );
					pDibBitmap2->Draw( pdc, true );
				}
			}
			break;
		}

	case SELECT_DRAW:
		{
			if( mpList->GetListData()->GetRowInfo(index)->Selected)
			{
				mpList->GetListData()->GetbmpRectData()->GetBitmapRect( bmpRect,index, nRow );
				bmpRect.top	 -= mpList->GetScrollPos2()->cy;	
				bmpRect.bottom -= mpList->GetScrollPos2()->cy;
				DrawItemBK( pdc, bmpRect,index );
				CDibBitmap* pDibBitmap = mpList->GetListData()->GetbmpRectData()->GetBitmap(nRow,ntype/* 因为音乐盒的控件每列图片相同所以 写死 GetRowBmpShowMode */ );
				if( pDibBitmap )
				{
					pDibBitmap->SetCDibRect( bmpRect );
					pDibBitmap->Draw(pdc, true );
				}
			}
			break;
		}
	case MULTI_BMP:
		{
			mpList->GetListData()->GetbmpRectData()->GetBitmapRect(bmpRect,index, nRow );

			bmpRect.top	 -= mpList->GetScrollPos2()->cy;	
			bmpRect.bottom -= mpList->GetScrollPos2()->cy;

			DrawItemBK( pdc, bmpRect,index );


			CDibBitmap* pDibBitmap = mpList->GetListData()->GetbmpRectData()->GetBitmap(nRow,ntype );

			if( pDibBitmap )
			{
				pDibBitmap->SetCDibRect( bmpRect );
				pDibBitmap->Draw(pdc, true );
			}

			break;
		}
	default: break;
	}
	ReleaseDC(mpList->m_hWnd, pdc->m_hDC);
}
void CDrawList::DrawAItemText( int& index, int& nRow,int ntype)
{
	if( mpList->GetListData()->GetRowInfo(index) == NULL) return;
	CDC* pdc = mpList->GetDC();

	CRect lrect;
	lrect.SetRectEmpty();

	lrect.top = ( index-mpList->GetListData()->GetRowInfo(index)->frontBigCout)* mpList->m_itemHeight
		+(mpList->GetListData()->GetRowInfo(index)->frontBigCout)* mpList->m_itemBigHeight;	
	lrect.bottom = lrect.top + mpList->m_itemHeight;
	
	lrect.top	  -= mpList->GetScrollPos2()->cy;	
	lrect.bottom  -= mpList->GetScrollPos2()->cy;
	lrect.left = mpList->GetItemLeft( nRow ) +2;
	lrect.right = mpList->GetItemRight( nRow );

	if( mpList->GetListData()->GetRowInfo(index)->Selected )
	{
		if( ntype == LINK_TEXT)
		{
			AfxGdiOperator()->DrawTextQuick( pdc, mpList->GetListData()->GetRowInfo(index)->rowData[nRow].strText,m_colorSelText, lrect, m_textFontUnderLine );
		}
		else if( ntype == NORMAL_TEXT)
		{
			lrect.left -= 2;
			DrawItemBK(pdc, lrect, index );
			DrawItem( pdc,index, nRow, lrect, m_maxX, m_maxY );
			//AfxGdiOperator()->DrawTextQuick( pdc, mpList->GetListData()->GetRowInfo(index)->rowData[nRow].strText,m_colorSelText, lrect, m_textFont );
		}
	}
	else
	{
		if( ntype == LINK_TEXT)
		{
			AfxGdiOperator()->DrawTextQuick( pdc, mpList->GetListData()->GetRowInfo(index)->rowData[nRow].strText,m_colorText, lrect, m_textFontUnderLine );
		}
		else if( ntype == NORMAL_TEXT)
		{
			lrect.left -= 2;
			DrawItemBK(pdc, lrect, index );
			DrawItem( pdc,index, nRow, lrect, m_maxX, m_maxY );
			//AfxGdiOperator()->DrawTextQuick( pdc, mpList->GetListData()->GetRowInfo(index)->rowData[nRow].strText,m_colorText, lrect, m_textFont );
		}
	}

	ReleaseDC(mpList->m_hWnd, pdc->m_hDC);
}
void CDrawList::DrawList( CDC* pdc,int& maxX, int& maxY)
{
	m_maxX = maxX;
	m_maxY = maxY;
	int count = mpList->GetListData()->GetSize();
	if( !m_textFont.m_hObject )
		AfxGdiOperator()->SetDrawTextFont(pdc, m_textFont);
	if( !m_textFontUnderLine.m_hObject )
		AfxGdiOperator()->SetDrawTextFont(pdc, m_textFontUnderLine, true);

	/////////////////////////////////// 优化 在客户区的项的最大可能性
	int Scrolltop = mpList->GetScrollPos2()->cy;
	CRect rect;
	mpList->GetClientRect( &rect );

	int startIndex = 0;
	int endIndex = 0;
	if( Scrolltop != 0 )
	{
		startIndex = Scrolltop /mpList->m_itemBigHeight;
		endIndex   = (Scrolltop + rect.Height())/mpList->m_itemHeight;
	}
	else
	{
		startIndex = 0;
		if( rect.Height() == 0 )
		{
			endIndex   = 0;
		}
		else
		{
			endIndex = rect.Height()/mpList->m_itemHeight;
		}
	}
	endIndex += 1;
	endIndex = endIndex<count ? endIndex:count ;
	for( int i=startIndex; i<endIndex; i++ )
	{
		DrawARow( pdc,  i, maxX, maxY);
	}
}
void CDrawList::DrawLyric(CDC* pdc, CString& str,CRect& rect)
{	

	if( rect.bottom < 0) return;
	if( rect.right < 0)  return;
	if( rect.left>m_maxX)  return;
	if( rect.top> m_maxY)  return;
	if( str.GetLength()== 0 ) return ;

	CFont* oldFont = (CFont*)pdc->SelectObject(&m_textFont);

	rect.left += 30;  //前面留空白部分

	CString aGroupText = "";
	CString endText = "";
	int startGroup = 0;
	int endGroup   = 0;



	while(1)
	{
		endGroup = str.Find( '<',startGroup );
		if( endGroup>-1)
		{
			strncpy(aGroupText.GetBufferSetLength( endGroup-startGroup ), (LPCTSTR)str + startGroup, (endGroup-startGroup) );
			AfxGdiOperator()->DrawTextQuick( pdc,aGroupText,m_colorLyric1, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS );
			rect.left = rect.left + pdc->GetTextExtent( aGroupText ).cx;
			aGroupText.ReleaseBuffer(endGroup-startGroup);
			startGroup = endGroup+1;
		}
		else
		{
			break;
		}
		endGroup = str.Find( '>',startGroup );
		if( endGroup>-1 )
		{
			strncpy(aGroupText.GetBufferSetLength( endGroup-startGroup ) , (LPCTSTR)str + startGroup, (endGroup-startGroup) );	
			AfxGdiOperator()->DrawTextQuick( pdc,aGroupText,m_colorLyric2, rect,  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS );
			rect.left = rect.left + pdc->GetTextExtent( aGroupText ).cx;
			aGroupText.ReleaseBuffer(endGroup-startGroup);
			startGroup = endGroup+1;
		}
		else
		{
			break;
		}
	}
	endText = str.Right( str.GetLength() - startGroup );
	AfxGdiOperator()->DrawTextQuick( pdc,endText,m_colorLyric1, rect,m_textFont,m_maxX,m_maxY,  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS );

	pdc->SelectObject(oldFont);
}
void CDrawList::DrawItemBK(CDC* pdc,CRect& rect,int& nIndex )
{
	if( mpList->GetListData()->GetRowInfo(nIndex) == NULL) return;
	if(mpList->GetListData()->GetRowInfo(nIndex)->Selected )
	{
		AfxGdiOperator()->FillRectangle( pdc->m_hDC, rect,m_colorSelect, false ,m_maxX, m_maxY );
	}
	else
	{			
		if(  nIndex%2 == 0 )
		{
			AfxGdiOperator()->FillRectangle( pdc->m_hDC, rect,m_colorBK1, false ,m_maxX, m_maxY );

		}
		else
		{
			AfxGdiOperator()->FillRectangle( pdc->m_hDC, rect,m_colorBK2, false ,m_maxX, m_maxY );
		}

		//Item上画一条分割线
		CRect rcTopLine = rect;
		rcTopLine.bottom = rcTopLine.top + 1;
		AfxGdiOperator()->FillRectangle( pdc->m_hDC, rcTopLine,RGB(241,246,249), false ,m_maxX, m_maxY );

		//高朝华编辑
		if( mpList->m_nhoverIndex == nIndex )
		{
			CDibBitmap *pDib = ::AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("ListHoverBK");
			pDib->SetCDibRect(rect);
			pDib->Draw(pdc, true);
		}
	}
}
void CDrawList::DrawEndBK( CDC* pdc,int& maxX, int& maxY, int top,  int lineWidth)
{
	CRect lrectClent, lrectDraw;
	mpList->GetClientRect(&lrectClent);
	lrectDraw = lrectClent;
	lrectDraw.top = top;
	lrectDraw.bottom = lrectDraw.top + lineWidth;

	UINT index = mpList->GetListData()->GetSize();
	while( lrectDraw.top < lrectClent.bottom )
	{
		if(  index%2 == 0 )
		{
			AfxGdiOperator()->FillRectangle( pdc->m_hDC, lrectDraw,m_colorBK1, false ,maxX, maxY );
		}
		else
		{
			AfxGdiOperator()->FillRectangle( pdc->m_hDC, lrectDraw,m_colorBK2, false ,maxX, maxY );
		}

		lrectDraw.top += lineWidth;
		lrectDraw.bottom = lrectDraw.top + lineWidth;
		index ++;
	}
}
void CDrawList::DrawListFromIndex()
{

}
void CDrawList::SetTextCorlor(COLORREF& col,COLORREF& selCol)
{
	m_colorText = col;
	m_colorSelText = selCol;
}
void CDrawList::SetBkCorlor( COLORREF& bk1,COLORREF& bk2, COLORREF& select )
{
	m_colorBK1 = bk1;
	m_colorBK2 = bk2;
	m_colorSelect = select;
}