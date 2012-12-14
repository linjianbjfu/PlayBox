#include "StdAfx.h"
#include ".\selectcontrol.h"
#include "MyList.h"
#include "DrawList.h"
#include "ListData.h"
#include "HeadCtrol.h"
#include "..\HanziToPinyin.h"

#include <algorithm>

CSelectControl::CSelectControl(void):
m_nPreSelected(-1),
m_endMoveSel(false)
{
}

CSelectControl::~CSelectControl(void)
{
}
void CSelectControl::AttachList(HWND hwnd)
{
	if( hwnd==NULL ) return;
	mp_list = (CMyList*)CWnd::FromHandle( hwnd );
}
void CSelectControl::CtrolMouseMove(UINT& nFlags,CPoint& point)
{
	mp_list->GetDrawList()->DrawSelectFrame( m_structMousemoveSel.pointBegain, m_structMousemoveSel.pointEndPre );
	m_structMousemoveSel.pointEnd = point;
	mp_list->GetDrawList()->DrawSelectFrame( m_structMousemoveSel.pointBegain, m_structMousemoveSel.pointEnd );
	m_structMousemoveSel.pointEndPre = point;
}
void CSelectControl::CtrolRButtonDown(UINT& nFlags,CPoint& point)
{
	int lnSelect = -1;
	mp_list->GetListData()->GetRealIndex( mp_list->PointToVirtualIndex(point), lnSelect );
	if( m_SelectedVector.size() <1 || !mp_list->GetListData()->IsSelected(lnSelect) )
	{
		if( point.x< mp_list->GetHeadWnd()->GetHeadRight() )
		{
			CleanSelected(true);

			int lnSelect = -1;
			mp_list->GetListData()->GetRealIndex( mp_list->PointToVirtualIndex(point), lnSelect );
			if( ( lnSelect >=0 )&& (lnSelect< mp_list->GetListData()->GetSize()) )
			{
				UpDataItem(lnSelect);
				m_nPreSelected = lnSelect;
			}
		}
		else
		{
			CleanSelected(true);
		}
	}
}

void CSelectControl::CtrolLButtonDown(UINT& nFlags, CPoint& point)
{
	if( nFlags&MK_CONTROL )
	{
		/////
		if( point.x< mp_list->GetHeadWnd()->GetHeadRight() )
		{
			int lnSelect = -1;
			mp_list->GetListData()->GetRealIndex( mp_list->PointToVirtualIndex(point), lnSelect );
			if( ( lnSelect >=0 )&& (lnSelect< mp_list->GetListData()->GetSize()) )
			{
				UpDataItem(lnSelect);
				if( mp_list->GetListData()->IsSelected( lnSelect ) )
				{
					m_SelectedVector.push_back( lnSelect );
				}
			}
		}
		else
		{
			if( mp_list->GetListData()->IsSelected( m_nPreSelected) )
			{
				UpDataItem(m_nPreSelected);
			}
			m_nPreSelected = -1;

			for( UINT i=0; i< m_SelectedVector.size(); i++ )
			{
				if( mp_list->GetListData()->IsSelected( m_SelectedVector[i]) )
				{
					UpDataItem(m_SelectedVector[i]);
				}
			}
			if( m_SelectedVector.size() > 0 ) 
			{
				m_SelectedVector.clear();
			}
		}
	}
	else if(  nFlags&MK_SHIFT )
	{
		if( m_nPreSelected != -1 )
		{
			if( point.x< mp_list->GetHeadWnd()->GetHeadRight() )
			{
				int lnSelect = -1;
				mp_list->GetListData()->GetRealIndex( mp_list->PointToVirtualIndex(point), lnSelect );
				if( lnSelect != -1 )
				{
					CleanSelected();
					if( lnSelect> m_nPreSelected )
					{
						for( int i=m_nPreSelected; i<=lnSelect; i++ )
						{
							if( !mp_list->GetListData()->IsSelected( i ) )
							{
								UpDataItem(i);
								m_SelectedVector.push_back( i );
							}
						}
					}
					else if( lnSelect< m_nPreSelected )
					{
						for( int i=lnSelect; i<=m_nPreSelected; i++ )
						{
							if( !mp_list->GetListData()->IsSelected( i ) )
							{
								UpDataItem(i);
								m_SelectedVector.push_back( i );
							}
						}
					}
				}
			}
			else
			{
				CleanSelected(true);
			}
		}
		else
		{
			int lnSelect = -1;
			mp_list->GetListData()->GetRealIndex( mp_list->PointToVirtualIndex(point), lnSelect );
			if( ( lnSelect >=0 )&& (lnSelect< mp_list->GetListData()->GetSize()) )
			{
				UpDataItem(lnSelect);
				m_nPreSelected = lnSelect;
				if( mp_list->GetListData()->IsSelected( lnSelect ) )
				{
					m_SelectedVector.push_back( lnSelect );
				}
			}
		}
	}
	else
	{
		if( point.x< mp_list->GetHeadWnd()->GetHeadRight() )
		{
			bool bSelect = false;
			int lnSelect = -1;
			mp_list->GetListData()->GetRealIndex( mp_list->PointToVirtualIndex(point), lnSelect );
			if( ( lnSelect >=0 )&& (lnSelect< mp_list->GetListData()->GetSize()) )
			{
				bSelect = mp_list->GetListData()->IsSelected(lnSelect);
			}
			if( !bSelect )
			{
				CleanSelected(true);

				if( ( lnSelect >=0 )&& (lnSelect< mp_list->GetListData()->GetSize()) )
				{
					UpDataItem(lnSelect);
					m_nPreSelected = lnSelect;
				}
			}
		}
		else
		{
			CleanSelected(true);
		}
	}
	// 按ctrl 多选择
	if (!(nFlags&MK_CONTROL)) 
	{
		bool bSelect = false;
		int lnSelect = -1;
		mp_list->GetListData()->GetRealIndex( mp_list->PointToVirtualIndex(point), lnSelect );
		if( ( lnSelect >=0 )&& (lnSelect< mp_list->GetListData()->GetSize()) )
		{
			bSelect = mp_list->GetListData()->IsSelected(lnSelect);
		}
		if( !bSelect )
		{
			while( m_structMousemoveSel.selectedVector.size() != 0 )
			{
				int lindex = m_structMousemoveSel.selectedVector[ m_structMousemoveSel.selectedVector.size()-1 ];
				if(  mp_list->GetListData()->IsSelected( lindex ) )
				{
					UpDataItem(lindex);
				}
				m_structMousemoveSel.selectedVector.pop_back(  );
			}
			m_structMousemoveSel.selectedVector.clear();
		}
	}

	// 鼠标移动区域选择开始
	if( mp_list->m_buttonDown && (point.x > mp_list->GetHeadWnd()->GetHeadRight() ) )
	{
		int lnSelect = -1;
		mp_list->GetListData()->GetRealIndex( mp_list->PointToVirtualIndex(point), lnSelect );


		m_structMousemoveSel.begainItem = lnSelect;

		m_structMousemoveSel.pointBegain = point;
		m_structMousemoveSel.pointEndPre = m_structMousemoveSel.pointBegain;
	}
	if( mp_list->m_buttonDown && (point.y>mp_list->GetBotton() )  )
	{
		m_structMousemoveSel.begainItem = mp_list->GetLineSize()>0? mp_list->GetLineSize()-1: -1;
		m_structMousemoveSel.pointBegain = point;
		m_structMousemoveSel.pointEndPre = m_structMousemoveSel.pointBegain;
		m_endMoveSel = true;
	}

	if( mp_list->m_buttonDown && (point.x <mp_list->GetHeadWnd()->GetHeadRight()) &&  point.y< mp_list->GetBotton())
	{
		m_structMousemoveSel.SetPointEnpty();
	}
}
void CSelectControl::CtrolLButtonUp(UINT& nFlags, CPoint& point)
{
	if(( nFlags != MK_SHIFT )&& (nFlags != MK_CONTROL) )
	{
		if(point.y < 0)
			point.y = 0;
		if( point.x< mp_list->GetHeadWnd()->GetHeadRight() )
		{
			bool bSelect = false;
			int lnSelect = -1;
			mp_list->GetListData()->GetRealIndex( mp_list->PointToVirtualIndex(point), lnSelect );
			if( ( lnSelect >=0 )&& (lnSelect< mp_list->GetListData()->GetSize()) )
			{
				bSelect = mp_list->GetListData()->IsSelected(lnSelect);
			}
			CRect wndRect;
			CPoint tempPoint = point;
			mp_list->GetWindowRect( &wndRect );
			mp_list->ClientToScreen(  &tempPoint );

			if( bSelect && m_SelectedVector.size()>0 && ::PtInRect(&wndRect,tempPoint ) )
			{
				CleanSelected(true);
				m_nPreSelected = lnSelect;
				UpDataItem(m_nPreSelected);

			}
		}
	}

	// 鼠标移动区域选择结束
	if( mp_list->m_buttonDown && (point.x > mp_list->GetHeadWnd()->GetHeadRight() && !m_endMoveSel ) )
	{
		m_structMousemoveSel.SetEmpty();
		m_structMousemoveSel.pointEnd = point;

	}
	m_endMoveSel = false;
	if( mp_list->m_buttonDown )
	{
		mp_list->GetDrawList()->DrawSelectFrame( m_structMousemoveSel.pointBegain, m_structMousemoveSel.pointEndPre );
	}
	if( mp_list->m_buttonDown /*&& (point.x < mp_list->GetHeadWnd()->GetHeadRight())*/ )
	{
		int lnSelect = -1;
		mp_list->GetListData()->GetRealIndex( mp_list->PointToVirtualIndex(point), lnSelect );
		m_structMousemoveSel.endItem = lnSelect;

		for( int i= m_structMousemoveSel.begainItem<=m_structMousemoveSel.endItem? m_structMousemoveSel.begainItem:m_structMousemoveSel.endItem; 
			i<= (m_structMousemoveSel.begainItem>=m_structMousemoveSel.endItem? m_structMousemoveSel.begainItem:m_structMousemoveSel.endItem);
			i++)
		{
			if( i == -1 ) break;
			if( ! mp_list->GetListData()->IsSelected( i ) )
			{
				UpDataItem(i);
			}
			m_structMousemoveSel.selectedVector.push_back( i );

			vector<UINT>::iterator itor = find(m_SelectedVector.begin(), m_SelectedVector.end(),  i );
			if( itor == m_SelectedVector.end() )
			{
				m_SelectedVector.push_back( i );
			}
		}
	}
	mp_list->m_buttonDown =false;
	m_structMousemoveSel.SetEmpty();
}
void CSelectControl::CtrolAKeyDown()
{
	m_SelectedVector.clear();
	for( UINT i=0; i< mp_list->GetLineSize(); i++ )
	{
		m_SelectedVector.push_back( i );
		if( !mp_list->GetListData()->IsSelected( i ) )
		{
			UpDataItem(i);
		}
	}
}
void CSelectControl::CtrolDownKeyDown()
{
	if( ( m_nPreSelected == -1)&&(m_SelectedVector.size() == 0 ) )
	{
		UpDataItem(0);
		m_nPreSelected = 0;
	}
	else if( (m_nPreSelected != -1)&&(m_SelectedVector.size() == 0 ) )
	{
		int nextSelect = m_nPreSelected;
		
		if( m_nPreSelected+1 < mp_list->GetLineSize() )
		{
			UpDataSelected();
			nextSelect += 1;
			UpDataItem(nextSelect);
			m_nPreSelected = nextSelect;
		}
	}
	else if( (m_nPreSelected != -1)&&(m_SelectedVector.size() != 0 ) )
	{
		int nextSelect = m_nPreSelected;
		CleanSelected();
		if( nextSelect+1 < mp_list->GetLineSize() )
		{
			UpDataSelected();
			nextSelect += 1;
			UpDataItem(nextSelect);
			m_nPreSelected = nextSelect;
		}
	}
	else if( (m_nPreSelected == -1)&&(m_SelectedVector.size() != 0 ) )
	{
		int nextSelect = m_SelectedVector[0];
		CleanSelected();
		if( nextSelect+1 < mp_list->GetLineSize() )
		{
			nextSelect += 1;
			UpDataItem(nextSelect);
			m_nPreSelected = nextSelect;
		}
	}
}
void CSelectControl::CtrolUpKeyDown()
{
	if( ( m_nPreSelected == -1)&&(m_SelectedVector.size() == 0 ) )
	{
		UpDataItem( mp_list->GetLineSize()-1);
		m_nPreSelected = mp_list->GetLineSize()-1;
	}
	else if( (m_nPreSelected != -1)&&(m_SelectedVector.size() == 0 ) )
	{
		int nextSelect = m_nPreSelected;
		if( m_nPreSelected-1 >= 0 )
		{
			UpDataSelected();
			nextSelect -= 1;
			UpDataItem( nextSelect );
			m_nPreSelected = nextSelect;
		}
	}
	else if( (m_nPreSelected != -1)&&(m_SelectedVector.size() != 0 ) )
	{
		int nextSelect = m_nPreSelected;
		CleanSelected();
		if( nextSelect-1 >= 0 )
		{
			UpDataSelected();
			nextSelect -= 1;
			UpDataItem( nextSelect );
			m_nPreSelected = nextSelect;
		}
	}
	else if( (m_nPreSelected == -1)&&(m_SelectedVector.size() != 0 ) )
	{
		int nextSelect = m_SelectedVector[0];
		CleanSelected();
		if( nextSelect-1 >= 0  )
		{
			nextSelect -= 1;
			UpDataItem( nextSelect );
			m_nPreSelected = nextSelect;
		}
	}	
}
void CSelectControl::CleanSelected( bool bHadSel)
{
	if( bHadSel )
	{
		UpDataSelected();
		m_nPreSelected = -1;
	}
	for( UINT i=0; i<m_SelectedVector.size(); i++ )
	{
		if( mp_list->GetListData()->IsSelected( m_SelectedVector[i]) )
		{
			UpDataItem( m_SelectedVector[i] );
		}
	}
	if( m_SelectedVector.size() > 0 ) 
	{
		m_SelectedVector.clear();
	}
}
void CSelectControl::UpDataSelected()
{
	if( mp_list->GetListData()->IsSelected(m_nPreSelected) )
	{
		UpDataItem( m_nPreSelected );
	}
}
void CSelectControl::UpDataItem(int nItem)
{
		mp_list->GetListData()->SelectItem( nItem );
		mp_list->GetDrawList()->DrawARow( nItem );
}
int  CSelectControl::Ctrol_a_z_KeyDown(short nKey)
{

	int nReturn = -1; // 返回为1 可能向下滚动窗口(如果选择在可是范围以外)； 
					  // 返回为0 向上滚动窗口(如果选择在可是范围以外)； 
					  // 返回为 -1; 不变
	int nSize = mp_list->GetLineSize();
	if(nSize<=0) return -1;
	if(m_nPreSelected>=nSize )  return -1;


	UINT nRow = 0;
	char cA[3];
	::ZeroMemory( cA, 3);
	cA[2] = '\0'; // 原始2字符 KEY
	char cCA[7];
	::ZeroMemory( cCA, 6);
	cCA[6] = '\0'; // 中文对应汉字拼音; 或者英文字符

	bool bFind = false;
	UINT  index;


	index=m_nPreSelected+1;   // 从当前选择的下一个开始搜索
	

	int  nFindTimes = 2;
	while(nFindTimes>0)
	{
		for(; index<nSize; index++ )
		{
			const char* pStr = mp_list->GetItemStrPtr( index, nRow );
			::memcpy( cA, pStr,2 );
			if( cA[0] & 0x80 )
			{

				ChStringToPinyin(cCA, cA, 7);
				
			}
			else
			{
				::memcpy( cCA, pStr,1);
				cCA[1] = '\0';
			}	
			char test = char(nKey);
			if( (char)nKey == cCA[0] || (char)(nKey+0x20) == cCA[0] )
			{
				bFind = true;
				break;
			}
		}
		if( bFind )
		{
			CleanSelected(true);
			m_nPreSelected = index;
			UpDataItem(index);

			nFindTimes = 0;
			if( nReturn != 0 )
			nReturn = 1;
		}
		else
		{
			if( (index == nSize-1)&&(m_nPreSelected == -1) )
			{
				nFindTimes = 0;   // 从第一项位置也没有找到， 不转到表头
			}
			else
			{
				index = 0;
				nFindTimes--;
				nReturn = 0;
			}
		}
	}


	return nReturn;
}
