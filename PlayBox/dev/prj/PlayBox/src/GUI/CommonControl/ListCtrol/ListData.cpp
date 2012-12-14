#include "StdAfx.h"
#include ".\listdata.h"
#include "myList.h"
ItemData::ItemDataRow& ItemData::ItemDataRow::operator =( const ItemDataRow& anItemDataRow)
{
	if( this == &anItemDataRow) return *this;
	else
	{
		strText = anItemDataRow.strText;

		bmpItem.first  = anItemDataRow.bmpItem.first;
		bmpItem.second  = anItemDataRow.bmpItem.second;
		bmpMouseTrack.first = anItemDataRow.bmpMouseTrack.first;
		bmpMouseTrack.second = anItemDataRow.bmpMouseTrack.second;
	}
	return *this;
}
ItemData::ItemData(UINT count)
:frontBigCout(0),
Selected(false),
strLineText("")
{
	for( UINT i=0; i<count; i++ )
	{
		ItemDataRow aRow;
		rowData.push_back( aRow );
	}
}
ItemData& ItemData::operator=(const ItemData& anItemData)
{
	if( this == &anItemData )return *this;
	else
	{
		frontBigCout = anItemData.frontBigCout;
		Selected	 = anItemData.Selected;
		strLineText =  anItemData.strLineText;
		rowData = anItemData.rowData;
	}
	return *this;
}
CListData::CListData(void)
:m_rowCount(0)
{
}

CListData::~CListData(void)
{
}
void CListData::AttachList(HWND hwnd)
{
	m_bmpRect.AttachList(hwnd);
	m_textRect.AttachList(hwnd);
}
void CListData::SetRow(UINT nCount)
{
	if( 0 == nCount) return;
	m_rowCount = nCount;
}
void CListData::SetRowBmpMode(int nRow, int nShowPos, int nShowType)
{
	BMP_MODE mode;
	mode.showPos = nShowPos;
	mode.showType = nShowType;
	pair<int,BMP_MODE> pairmode;
	pairmode.first = nRow;
	pairmode.second = mode;
	m_rowBmpMode.insert( pairmode);
}
void CListData::SetRowTextMode(int nRow, int nShowPos, int nShowType)
{
	TEXT_MODE mode;
	mode.showPos = nShowPos;
	mode.showType = nShowType;
	pair<int,TEXT_MODE> pairmode;
	pairmode.first = nRow;
	pairmode.second = mode;
	m_rowTextMode.insert( pairmode);
}

int CListData::InsertLine()
{
	P_ITEMDATA_INFO pItemData;
	pItemData = new ItemData(m_rowCount);
	m_vectorItemData.push_back( pItemData );
	return (int)(m_vectorItemData.size() -1);
}
int CListData::InsertLine(UINT nIndex)
{
	size_t nSize = m_vectorItemData.size();
	if( nIndex > nSize ) return -1;

	P_ITEMDATA_INFO pItemData;
	pItemData = new ItemData(m_rowCount);
	if( nIndex == nSize ) m_vectorItemData.push_back( pItemData );
	m_vectorItemData.insert( m_vectorItemData.begin()+nIndex, pItemData  );
	return nIndex;
}
void CListData::DeleteLine(UINT nIndex)
{
	size_t nSize = m_vectorItemData.size();
	if( nIndex > nSize-1 ) return ;
	vector<P_ITEMDATA_INFO>::iterator itor;
	itor = m_vectorItemData.begin()+nIndex;
	(*itor)->rowData.clear();

	bool isWidth = false;
	if( (*itor)->strLineText.GetLength() > 0)
	{
		isWidth = true;
	}
	if( (*itor) ) delete (*itor);
	itor = m_vectorItemData.erase(itor);	 

	// 信息重置
	for( int i = nIndex; i<GetSize(); i++ )
	{
		if( isWidth )
		{
			m_vectorItemData[nIndex]->frontBigCout -= 1;
		}
	}
}
void CListData::DeleteAllLine()
{
	vector<P_ITEMDATA_INFO>::iterator itor;
	for( itor = m_vectorItemData.begin(); itor != m_vectorItemData.end(); itor++ )
	{
		(*itor)->rowData.clear();
		if( (*itor) ) delete (*itor);
	}
	m_vectorItemData.clear();
	if( m_index.size( ) > 0 )
	{
		m_index.clear();
	}
}
void CListData::DeleteAllLine(vector<P_ITEMDATA_INFO>* dataVector)
{
	if( NULL == dataVector ) return ;
	vector<P_ITEMDATA_INFO>::iterator itor;
	for( itor = dataVector->begin(); itor != dataVector->end(); itor++ )
	{
		(*itor)->rowData.clear();
		if( (*itor) ) delete (*itor);
	}
	dataVector->clear();
}
bool CListData::SetItemData(UINT nLine, UINT nRow,CString& str )
{
	if( 0 == str.GetLength() ) return false;
	if( nLine >m_vectorItemData.size()-1 ) return false;
	if( nRow > m_rowCount-1 ) return false;

	if( nLine == 0)
	{
		m_vectorItemData[nLine]->rowData[nRow].strText = str;
		m_vectorItemData[nLine]->frontBigCout = 0;
	}
	else
	{
		if(  0 == m_vectorItemData[nLine-1]->strLineText.GetLength() )
		{
			m_vectorItemData[nLine]->frontBigCout = m_vectorItemData[nLine-1]->frontBigCout;
		}
		else
		{
			m_vectorItemData[nLine]->frontBigCout = m_vectorItemData[nLine-1]->frontBigCout + 1;
		}
		m_vectorItemData[nLine]->rowData[nRow].strText = str;
	}

	return true;
}
bool CListData::SetItemData(UINT nLine, UINT nRow,CString& str, int nbmpType,int nbmpTypeIndex, bool isMouseTrack  )
{
	if( nLine >m_vectorItemData.size()-1 ) return false;
	if( nRow > m_rowCount-1 ) return false;

	if( nLine == 0)
	{
		m_vectorItemData[nLine]->frontBigCout = 0;
		m_vectorItemData[nLine]->rowData[nRow].strText = str;
		m_vectorItemData[nLine]->rowData[nRow].bmpItem.first = nbmpType;
		m_vectorItemData[nLine]->rowData[nRow].bmpItem.second = nbmpTypeIndex;
		m_vectorItemData[nLine]->rowData[nRow].bmpMouseTrack.first = nbmpType;
		m_vectorItemData[nLine]->rowData[nRow].bmpMouseTrack.second = isMouseTrack;
	}
	else
	{
		if(  0 == m_vectorItemData[nLine-1]->strLineText.GetLength() )
		{
			m_vectorItemData[nLine]->frontBigCout = m_vectorItemData[nLine-1]->frontBigCout;
		}
		else
		{
			m_vectorItemData[nLine]->frontBigCout = m_vectorItemData[nLine-1]->frontBigCout + 1;
		}
		m_vectorItemData[nLine]->rowData[nRow].strText = str;
		m_vectorItemData[nLine]->rowData[nRow].bmpItem.first = nbmpType;
		m_vectorItemData[nLine]->rowData[nRow].bmpItem.second = nbmpTypeIndex;
		m_vectorItemData[nLine]->rowData[nRow].bmpMouseTrack.first = nbmpType;
		m_vectorItemData[nLine]->rowData[nRow].bmpMouseTrack.second = isMouseTrack;
	}
	return true;
}
bool CListData::SetItemData(UINT nLine, UINT nRow, int nbmpType,int nbmpTypeIndex, bool isMouseTrack  )
{
	if( nLine >m_vectorItemData.size()-1 ) return false;
	if( nRow > m_rowCount-1 ) return false;

	if( nLine == 0)
	{
		m_vectorItemData[nLine]->frontBigCout = 0;
		m_vectorItemData[nLine]->rowData[nRow].bmpItem.first = nbmpType;
		m_vectorItemData[nLine]->rowData[nRow].bmpItem.second = nbmpTypeIndex;
		m_vectorItemData[nLine]->rowData[nRow].bmpMouseTrack.first = nbmpType;
		m_vectorItemData[nLine]->rowData[nRow].bmpMouseTrack.second = isMouseTrack;
	}
	else
	{
		if(  0 == m_vectorItemData[nLine-1]->strLineText.GetLength() )
		{
			m_vectorItemData[nLine]->frontBigCout = m_vectorItemData[nLine-1]->frontBigCout;
		}
		else
		{
			m_vectorItemData[nLine]->frontBigCout = m_vectorItemData[nLine-1]->frontBigCout + 1;
		}
		m_vectorItemData[nLine]->rowData[nRow].bmpItem.first = nbmpType;
		m_vectorItemData[nLine]->rowData[nRow].bmpItem.second = nbmpTypeIndex;
		m_vectorItemData[nLine]->rowData[nRow].bmpMouseTrack.first = nbmpType;
		m_vectorItemData[nLine]->rowData[nRow].bmpMouseTrack.second = isMouseTrack;
	}
	return true;
}
bool CListData::SetItemData(UINT nLine, UINT nRow,CString& str,CString& strEx, int nbmpType,int nbmpTypeIndex, bool isMouseTrack  )
{
	if( 0 == str.GetLength() ) return false;
	if( nLine >m_vectorItemData.size()-1 ) return false;
	if( nRow > m_rowCount-1 ) return false;

	if( nLine == 0)
	{
		m_vectorItemData[nLine]->frontBigCout = 0;
		m_vectorItemData[nLine]->strLineText = strEx;
		m_vectorItemData[nLine]->rowData[nRow].strText = str;
		m_vectorItemData[nLine]->rowData[nRow].bmpItem.first = nbmpType;
		m_vectorItemData[nLine]->rowData[nRow].bmpItem.second = nbmpTypeIndex;
		m_vectorItemData[nLine]->rowData[nRow].bmpMouseTrack.first = nbmpType;
		m_vectorItemData[nLine]->rowData[nRow].bmpMouseTrack.second = isMouseTrack;
	}
	else
	{
		if(  0 == m_vectorItemData[nLine-1]->strLineText.GetLength() )
		{
			m_vectorItemData[nLine]->frontBigCout = m_vectorItemData[nLine-1]->frontBigCout;
		}
		else
		{
			m_vectorItemData[nLine]->frontBigCout = m_vectorItemData[nLine-1]->frontBigCout + 1;
		}
		m_vectorItemData[nLine]->strLineText = strEx;
		m_vectorItemData[nLine]->rowData[nRow].strText = str;
		m_vectorItemData[nLine]->rowData[nRow].bmpItem.first = nbmpType;
		m_vectorItemData[nLine]->rowData[nRow].bmpItem.second = nbmpTypeIndex;
		m_vectorItemData[nLine]->rowData[nRow].bmpMouseTrack.first = nbmpType;
		m_vectorItemData[nLine]->rowData[nRow].bmpMouseTrack.second = isMouseTrack;
	}
	for( UINT i=nLine+1; i<m_vectorItemData.size(); i++ )
	{
		m_vectorItemData[i]->frontBigCout += 1;
	}
	return true;
}
bool CListData::SetItemData(UINT nLine, UINT nRow,INSERT_DATASTRUCT& DateStruct  )
{
	if( DateStruct.strIextLine.GetLength() == 0 )
	{
		if( DateStruct.strText.GetLength() == 0 )
		{
			SetItemData(  nLine, nRow,DateStruct.nbmpType,DateStruct.nbmpTypeIndex, DateStruct.bmpMouseTrack );
		}
		else
		{
			SetItemData(  nLine, nRow,DateStruct.strText, DateStruct.nbmpType,DateStruct.nbmpTypeIndex, DateStruct.bmpMouseTrack );
		}
	}
	else
	{
		SetItemData(  nLine, nRow,DateStruct.strText,DateStruct.strIextLine, DateStruct.nbmpType,DateStruct.nbmpTypeIndex, DateStruct.bmpMouseTrack );
	}
	return true;
}
bool CListData::GetItemData(UINT& nLine, UINT& nRow, CString& str )const
{
	if( nLine > GetSize() ) return false;
	if( nRow > m_rowCount-1 ) return false;
	str = m_vectorItemData[nLine]->rowData[nRow].strText;
	return true;
}
bool CListData::GetItemData(UINT& nLine, UINT& nRow, CString& str,CString& strEx)const
{
	if( nLine > GetSize() ) return false;
	if( nRow > m_rowCount-1 ) return false;
	str = m_vectorItemData[nLine]->rowData[nRow].strText;
	strEx = m_vectorItemData[nLine]->strLineText;
	return true;
}
const char* CListData::GetItemStrPtr(UINT& nLine, UINT& nRow)
{
	if( nLine > GetSize() ) return NULL;
	if( nRow > m_rowCount-1 ) return NULL;
	return m_vectorItemData[nLine]->rowData[nRow].strText;
}
bool CListData::UpdateIndex(int nIndex)
{
	if( ( (int)(m_index.size())-1> nIndex) && (nIndex !=0) ) return false;
	if( (0 == nIndex) )
	{
		m_index.clear();
		int count = 0;
		if( GetSize() != 0 )
		{
			count = GetSize()  + m_vectorItemData[GetSize()-1]->frontBigCout;
			if ( m_vectorItemData[GetSize()-1]->strLineText.GetLength() > 0 )
			{
				count	+= 1;
			}
		}

		//for( int i=0; i<count; i++ )
		//{
		m_index.resize( count );//push_back( 0 ); 
		//}
		int index = 0;
		for( int i=0; i<count; i++  )
		{
			m_index[i] = index ;
			if( m_vectorItemData[index]->strLineText.GetLength() > 0 )
			{
				i+=1;
				m_index[i] = index;
			}
			index ++;
		}
	}
	else 
	{
		int indexCount = (int)(m_index.size());
		int count = m_vectorItemData[ GetSize() -1 ]->frontBigCout+ GetSize();
		if ( m_vectorItemData[GetSize()-1]->strLineText.GetLength() > 0 )
		{
			count	+= 1;
		}
		//for( int i=0; i< count-indexCount; i++ )
		//{
		m_index.resize( count );
		//}
		int index = nIndex;
		for( int i= nIndex+ m_vectorItemData[nIndex]->frontBigCout; i<count; i++  )
		{
			m_index[i] = index ;
			if( m_vectorItemData[index]->strLineText.GetLength() > 0 )
			{
				i+=1;
				m_index[i] = index;
			}
			index ++;
		}
	}
	return true;
}
void CListData::UpdateBigCount()
{
	for( int i=0; i<GetSize(); i++ )
	{
		if( i == 0 )
		{
			m_vectorItemData[i]->frontBigCout = 0;
		}
		else
		{
			if( 0 != m_vectorItemData[i-1]->strLineText.GetLength() )
			{
				m_vectorItemData[i]->frontBigCout = m_vectorItemData[i-1]->frontBigCout +1;
			}
			else
			{
				m_vectorItemData[i]->frontBigCout = m_vectorItemData[i-1]->frontBigCout;
			}
		}
	}
}
int CListData::GetSize()const
{
	return (int)m_vectorItemData.size();
}
bool CListData::GetRealIndex(int Index, int& RealIndex)const
{
	if( m_index.size() == 0 ) return false;
	if( Index>m_index.size()-1 ) return false;
	RealIndex = m_index[Index];
	return true;
}
void CListData::SelectItem(int RealIndex)
{
	if( RealIndex<0 ) return;
	m_vectorItemData[RealIndex]->Selected ^= 1; 
}
const P_ITEMDATA_INFO CListData::GetRowInfo(int index)const
{
	if( GetSize() == 0 ) return NULL;
	if( index > GetSize() - 1  ) return NULL;
	if(index < 0 ) return NULL;
	return m_vectorItemData[index];
}
void  CListData::SetRowInfo(int index, P_ITEMDATA_INFO info)
{
	if( index > GetSize() - 1  ) return ;
	m_vectorItemData[index] = info;
}
const vector<P_ITEMDATA_INFO>* CListData::GetDataVector()const
{
	return &m_vectorItemData;
}
bool CListData::IsSelected( int index )
{
	if( index < 0 ) return false;
	if( index> GetSize()-1 ) return false;
	return m_vectorItemData[index]->Selected;
}	
int  CListData::GetRowBmpShowMode(int nRow)
{
	map<int,BMP_MODE>::iterator itor;
	itor = m_rowBmpMode.find( nRow);
	if( m_rowBmpMode.end() == itor ) return -1;
	return (*itor).second.showPos; 
}
int  CListData::GetRowTextShowMode(int nRow)
{
	map<int,TEXT_MODE>::iterator itor;
	itor = m_rowTextMode.find( nRow);
	if( m_rowTextMode.end() == itor ) return -1;
	return (*itor).second.showPos; 
}
int  CListData::GetRowBmpShowType(int nRow)
{
	map<int,BMP_MODE>::iterator itor;
	itor = m_rowBmpMode.find( nRow);
	if( m_rowBmpMode.end() == itor ) return -1;
	return (*itor).second.showType; 
}
int  CListData::GetRowTextShowType(int nRow)
{
	map<int,TEXT_MODE>::iterator itor;
	itor = m_rowTextMode.find( nRow);
	if( m_rowTextMode.end() == itor ) return -1;
	return (*itor).second.showType; 
}
void CListData::CtrolMouseLive()
{
	m_bmpRect.CtrolMouseLive();
	m_textRect.CtrolMouseLive();
}

///
CBitmapRectData::CBitmapRectData()
:m_preLineBmpPos(-1),
m_preRowBmpPos(-1),
m_leaveLineBmpPos(-1),
m_leaveRowBmpPos(-1),
m_leaveLinePos(-1),
m_leaveRowPos(-1),
m_preLinePos( -1),
m_preRowPos(-1)
{
}
void CBitmapRectData::SendMsgMouseLeave(int& nLine, int& nRow)
{
	if( ( nLine<0) && ( nRow<0 ) ) return; 
	::SendMessage( mpList->m_hWnd,  MSG_MOUSELEAVE, nLine, nRow );
}
void CBitmapRectData::SendMsgMouseInBmp(int& nLine, int& nRow)
{
	if( ( nLine<0) && ( nRow<0 ) ) return; 
	::SendMessage( mpList->m_hWnd,  MSG_MOUSEHOVER, nLine, nRow );
}
void CBitmapRectData::SendMsgMouseLeaveItem(int& nLine, int& nRow)
{
	if( ( nLine<0) && ( nRow<0 ) ) return; 
	::SendMessage( mpList->m_hWnd,  MSG_MOUSELEAVE_ITEM, nLine, nRow );
}
void CBitmapRectData::SendMsgMouseInItem(int& nLine, int& nRow)
{
	if( ( nLine<0) && ( nRow<0 ) ) return; 
	::SendMessage( mpList->m_hWnd,  MSG_MOUSEHOVER_ITEM, nLine, nRow );
}
void CBitmapRectData::AttachList(HWND hwnd)
{
	mpList = (CMyList*)CWnd::FromHandle( hwnd );
}
void CBitmapRectData::CtrolMouseMove(UINT nFlags,CPoint point)
{
	int line = -1;
	int row = -1;
	mpList->PointInItem( point, line, row );
	if(( line >=0 ) && (row>= 0) )
	{
		if( (m_preLinePos != line)||(m_preRowPos != row) ) 
		{
			SendMsgMouseInItem( line, row );
			m_preLinePos = line;
			m_preRowPos = row;
		}

		CRect bmprect;
		GetBitmapRect( bmprect,line,row );
		/*point.x -= mpList->GetScrollPos()->cx;	
		point.y -= mpList->GetScrollPos()->cy;*/

		//point.x += mpList->GetScrollPos2()->cx;	
		point.y += mpList->GetScrollPos2()->cy;
		if( mpList->GetListData()->GetRowInfo(line) == NULL) return;
		if( PtInRect( &bmprect, point ) )
		{
			if( (m_preLineBmpPos != line)||(m_preRowBmpPos != row) ) 
			{				
				if( (row != 5)|| ((row == 5)&&( mpList->GetListData()->GetRowInfo(line)->rowData[row].bmpItem.second != 4))  )// 特殊处理，下载完成， 没有状态变化 ( 因为控件不没支持 单独 行的 mousetrack 更改支持, 这个判断不属于控件本身的乱代码 )

				{
					SendMsgMouseLeave(m_preLineBmpPos, m_preRowBmpPos );
					SendMsgMouseInBmp(line, row );				
				}
				m_preLineBmpPos = line;
				m_preRowBmpPos =  row;
			}
			if( !(nFlags&MK_LBUTTON) )
			{
				if((mpList->GetListData()->GetRowInfo(line)->rowData[row].bmpMouseTrack.second) &&( !((SELECT_DRAW == mpList->GetListData()->GetRowBmpShowType( row ))&& !(mpList->GetListData()->GetRowInfo(line)->Selected ) ) ) )
				{				
					if( (row != 5)|| ((row == 5)&&( mpList->GetListData()->GetRowInfo(line)->rowData[row].bmpItem.second != 4))  )// 特殊处理，下载完成， 没有状态变化 ( 因为控件不没支持 单独 行的 mousetrack 更改支持, 这个判断不属于控件本身的乱代码 )
					{
#ifndef IDC_HAND
#define IDC_HAND    MAKEINTRESOURCE(32649)
#endif
					::SetCursor(::LoadCursor(NULL, IDC_HAND));					
					}
				}
				if( mpList->GetListData()->GetRowInfo(line)->rowData[row].bmpMouseTrack.second && !(SELECT_DRAW == mpList->GetListData()->GetRowBmpShowType( row )) )
				{										
					if( (row != 5)|| ((row == 5)&&( mpList->GetListData()->GetRowInfo(line)->rowData[row].bmpItem.second != 4))  )// 特殊处理，下载完成， 没有状态变化 ( 因为控件不没支持 单独 行的 mousetrack 更改支持, 这个判断不属于控件本身的乱代码 )
					{
#ifndef IDC_HAND
#define IDC_HAND    MAKEINTRESOURCE(32649)
#endif
					::SetCursor(::LoadCursor(NULL, IDC_HAND));				
					}
				}
			}
		}
		else
		{
			if( ( m_leaveLinePos != m_preLinePos)||( m_preRowPos != m_leaveRowPos ) ) 
			{
				SendMsgMouseLeaveItem( m_preLinePos,m_preRowPos );
				m_leaveLinePos = m_preLinePos;
				m_leaveRowPos =  m_preRowPos;
				m_preLinePos = -1;
				m_preRowPos  = -1;
			}
			if( ( m_leaveLineBmpPos != m_preLineBmpPos)||( m_preRowBmpPos != m_leaveRowBmpPos ) ) 
			{
				if( (row != 5)|| ((row == 5)&&( mpList->GetListData()->GetRowInfo(line)->rowData[row].bmpItem.second != 4))  )// 特殊处理，下载完成， 没有状态变化 ( 因为控件不没支持 单独 行的 mousetrack 更改支持, 这个判断不属于控件本身的乱代码 )
				{
					SendMsgMouseLeave(m_preLineBmpPos,m_preRowBmpPos );
				}
				m_leaveLineBmpPos = m_preLineBmpPos;
				m_leaveRowBmpPos  = m_preRowBmpPos;

				m_preLineBmpPos = -1;
				m_preRowBmpPos  = -1;
			}
		}
	}
}

void CBitmapRectData::CtrolMouseLive()
{
	SendMsgMouseLeave(m_preLineBmpPos,m_preRowBmpPos);
	m_preLineBmpPos = -1;
	m_preRowBmpPos  = -1;

	m_leaveLineBmpPos = -1;
	m_leaveRowBmpPos  = -1;

	m_leaveLineBmpPos = -1;
	m_leaveRowBmpPos = -1;
	m_preLinePos = -1;
	m_preRowPos = -1;
}
void CBitmapRectData::GetBitmapRect(CRect& bmpRect,int& nLine,int& nRow)
{	
	bmpRect.SetRectEmpty();
	CRect lrect; // item rect
	lrect.SetRectEmpty();
	if( mpList->GetListData()->GetRowInfo(nLine) == NULL) return;
	lrect.top = ( nLine-mpList->GetListData()->GetRowInfo(nLine)->frontBigCout)* mpList->m_itemHeight
		+(mpList->GetListData()->GetRowInfo(nLine)->frontBigCout)* mpList->m_itemBigHeight;
	lrect.bottom = lrect.top + mpList->m_itemHeight;
	lrect.left = mpList->GetItemLeft( nRow ) ;
	lrect.right = mpList->GetItemRight( nRow );

	CSize size;
	size.SetSize(0,0);
	if( -1 == GetBmpSize( nRow,size ) )
	{
		bmpRect.InflateRect( -1,-1 );
	}
	switch( mpList->GetListData()->GetRowBmpShowMode(nRow) )
	{
	case RIGHT:
		{
			bmpRect.left = lrect.right - BMP_INTERVAL - size.cx;
			if( bmpRect.left< lrect.left ) bmpRect.left= lrect.left;
			bmpRect.right = bmpRect.left+size.cx;
			if( bmpRect.right>lrect.right ) bmpRect.right = lrect.right;
			bmpRect.top = lrect.top+ BMP_INTERVAL;
			bmpRect.bottom = bmpRect.top+size.cy;

			break;
		}
	case CENTER:
		{
			bmpRect.left = lrect.left + lrect.Width()/2 - size.cx/2;
			if( bmpRect.left< lrect.left ) bmpRect.left= lrect.left;
			bmpRect.right = bmpRect.left+size.cx;
			if( bmpRect.right>lrect.right ) bmpRect.right = lrect.right;
			int nInterval = (mpList->m_itemHeight- size.cy)/2;
			bmpRect.top = lrect.top+ nInterval;
			bmpRect.bottom = bmpRect.top+size.cy;
			break;
		}
	default: break;
	}
}
int CBitmapRectData::GetBmpSize( int nRow,CSize& size )
{
	map<int,SIZE>::iterator itor; 
	itor = m_bmpSize.find( nRow );
	if( itor == m_bmpSize.end() ) return -1;
	size = (*itor).second;
	return 1;
}
int CBitmapRectData::GetRowBmpCount(int row)
{
	map<int, map<int,CDibBitmap*> >::iterator itor;
	itor  = m_bmp.find( row );
	if( itor != m_bmp.end() )
	{
		return (int)((*itor).second.size() );
	}
	return 0;
}
void CBitmapRectData::SetBitmap( int nRow,int nBmpType, CDibBitmap* pBmp)
{
	if( !pBmp ) return ; 
	pair<int,CDibBitmap*> apair;
	apair.first = nBmpType;
	apair.second = pBmp;

	map<int, map<int,CDibBitmap*> >::iterator itor;
	itor = m_bmp.find( nRow );

	if( itor == m_bmp.end() )
	{
		map<int,CDibBitmap*>  rowbmpMap;
		rowbmpMap.insert( apair );

		pair<int, map<int,CDibBitmap*> > arowPair;
		arowPair.first = nRow;
		arowPair.second = rowbmpMap;
		m_bmp.insert( arowPair );

		if(nBmpType == 0 )
		{
			pair<int , SIZE> apair;
			apair.first = nRow;
			apair.second = CSize(pBmp->GetWidth(), pBmp->GetHeight() );
			m_bmpSize.insert( apair );
		}
	}
	else
	{
		map<int,CDibBitmap*>::iterator itorRow;
		itorRow = (*itor).second.find( nBmpType );
		if( itorRow == (*itor).second.end() )
		{
			(*itor).second.insert( apair );
		}
		else
		{
			(*itorRow).second = pBmp;
		}

		if(nBmpType == 0 )
		{
			pair<int , SIZE> apair;
			apair.first = nRow;
			apair.second = CSize(pBmp->GetWidth(), pBmp->GetHeight() );
			map<int,SIZE>::iterator itorSize;
			itorSize = m_bmpSize.find( nRow );
			if( itorSize == m_bmpSize.end() )
			{
				m_bmpSize.insert( apair );
			}
			else
			{
				//(*itorSize).first = nRow;
				(*itorSize).second =CSize (pBmp->GetWidth(), pBmp->GetHeight() );
			}
		}
	}
}
CDibBitmap* CBitmapRectData::GetBitmap( int nRow, int nType )
{
	map<int, map<int,CDibBitmap*> >::iterator itor;
	itor = m_bmp.find( nRow );
	if( itor == m_bmp.end() ) return NULL;

	map<int,CDibBitmap*>::iterator itorRow;
	itorRow = (*itor).second.find( nType );
	if( itorRow == (*itor).second.end() ) return NULL;
	return (*itorRow).second;
}
CTextRectData::CTextRectData():
m_preLineTextPos(-1),
m_preRowTextPos(-1),
m_leaveLineTextPos(-1),
m_leaveRowTextPos(-1),
mpList(NULL)
{

}
void CTextRectData::AttachList(HWND hwnd)
{
	mpList = (CMyList*)CWnd::FromHandle( hwnd );
}
void CTextRectData::CtrolMouseMove(UINT nFlags,CPoint& point)
{
	int line = -1;
	int row = -1;
	mpList->PointInItem( point, line, row );
	CRect textRect;
	GetTextRect(textRect,line,row );

	//if( (m_preLineTextPos != line)||(m_preRowTextPos != row) ) 
	{
		if(LINK_TEXT == mpList->GetListData()->GetRowTextShowType(m_preRowTextPos) )
		{
			SendMsgMouseLeaveText(m_preLineTextPos,m_preRowTextPos );
		}
		//鼠标在LINK_TEXT的文字上，才显示手型
		if(LINK_TEXT == mpList->GetListData()->GetRowTextShowType(row) )
		{
			//TRACE("textRect:左%d上%d右%d下%d--point<%d,%d>\n", textRect.left, textRect.top, textRect.right, textRect.bottom,
			//	point.x, point.y );
			//下面的代码是朝华添加的，为了解决bug
			//D001470	低	鼠标在歌手、专辑列的空白区域呈小手状，点击无反应。	陈创	
			if( point.x > textRect.left && point.x < textRect.right )
			{
				SendMsgMouseInText(line, row );
			}			
		}

		m_preLineTextPos = line;
		m_preRowTextPos =  row;
	}
	if( (!(nFlags&MK_LBUTTON))&&(LINK_TEXT == mpList->GetListData()->GetRowTextShowType(row)) )
	{
#ifndef IDC_HAND
#define IDC_HAND            MAKEINTRESOURCE(32649)
#endif
		if( point.x > textRect.left && point.x < textRect.right )
		{
			::SetCursor(::LoadCursor(NULL, IDC_HAND));
		}
	}

}
void CTextRectData::CtrolMouseLive()
{
	SendMsgMouseLeaveText(m_preLineTextPos,m_preRowTextPos);
	m_preLineTextPos = -1;
	m_preRowTextPos  = -1;

	m_leaveLineTextPos = -1;
	m_leaveRowTextPos  = -1;

}
void CTextRectData::GetTextRect(CRect& textRect,int& nLine,int& nRow)
{	
	textRect.SetRectEmpty();
	CRect lrect; // item rect
	lrect.SetRectEmpty();
	if( mpList->GetListData()->GetRowInfo(nLine) == NULL) return;
	lrect.top = ( nLine-mpList->GetListData()->GetRowInfo(nLine)->frontBigCout)* mpList->m_itemHeight
		+(mpList->GetListData()->GetRowInfo(nLine)->frontBigCout)* mpList->m_itemBigHeight;
	lrect.bottom = lrect.top + mpList->m_itemHeight;
	lrect.left = mpList->GetItemLeft( nRow ) ;
	lrect.right = mpList->GetItemRight( nRow );
	lrect.left += 2;

	CDC* pdc = mpList->GetDC();
	UINT uline = nLine;
	UINT uRow  = nRow;
	CString str = mpList->GetListData()->GetItemStrPtr(uline, uRow);
	CSize size = pdc->GetTextExtent( str );
	textRect = lrect;
	textRect.top  = lrect.top + (lrect.Height()-size.cy)/2;
	textRect.bottom = lrect.top + size.cy;
	if( lrect.Width()> size.cx )
	{
		textRect.right = lrect.left + size.cx;
	}

	ReleaseDC(  mpList->m_hWnd, pdc->m_hDC );

}
void CTextRectData::SendMsgMouseInText(int& nLine, int& nRow)
{
	if( ( nLine<0) && ( nRow<0 ) ) return; 
	::SendMessage( mpList->m_hWnd,  MSG_MOUSEHOVER_TEXT, nLine, nRow );
}
void CTextRectData::SendMsgMouseLeaveText(int& nLine, int& nRow)
{
	if( ( nLine<0) && ( nRow<0 ) ) return; 
	::SendMessage( mpList->m_hWnd,  MSG_MOUSELEAVE_TEXT, nLine, nRow );
}
