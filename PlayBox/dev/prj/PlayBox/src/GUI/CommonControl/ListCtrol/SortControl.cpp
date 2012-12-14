#include "StdAfx.h"
#include ".\sortcontrol.h"
#include "listdata.h"
#include ".\mylist.h"

CSortControl::CSortControl():
m_ppIndexData(NULL)
{
}

CSortControl::~CSortControl(void)
{
}
void CSortControl::SetSortIndex( vector<int>* pIndexData)
{
	m_ppIndexData = pIndexData;
}
void CSortControl::AttachList(HWND hwnd)
{
	if( hwnd==NULL ) return;
	mp_list = (CMyList*)CWnd::FromHandle( hwnd );
}

void CSortControl::SortItem( int nRow, int nType )
{
	for( int i=0; i< mp_list->GetListData()->GetSize(); i++ )
	{
		// mpList->GetListData()->GetRowInfo(index)->rowData[nRow].bmpItem.second != -1
		CString str = mp_list->GetListData()->GetRowInfo(i)->rowData[nRow].strText;
		str.TrimLeft(" ");
		if( str.GetLength() == 0 )
		{
			if( (mp_list->GetListData()->GetRowInfo(i)->rowData[nRow].bmpItem.second == -1) && (mp_list->GetListData()->GetRowInfo(i)->rowData[nRow].bmpItem.first != -1) )
			{
				str+= ".";
			}
		}
		CString str2;
		str2.Format( ".%d", i);
		str += str2;
		pair<CString, int> lpair;
		lpair.first = str;
		lpair.second = i;
		m_sortMap.insert( lpair );	
	}
    
	vector<P_ITEMDATA_INFO> lVectorItemDataTemp;

	
	lVectorItemDataTemp = *(mp_list->GetListData()->GetDataVector());
	
	
	map<CString,UINT>::iterator mapItor =  m_sortMap.begin();
	m_IndexDataBack.clear();
	m_IndexDataBack = *m_ppIndexData;


	for( int i=0; i< mp_list->GetListData()->GetSize(); i++ )
	{
		UINT nNewpos = (*mapItor).second;
		if( 2 == nType )
		{
			//排序后的列指针数组
			mp_list->GetListData()->SetRowInfo( i, lVectorItemDataTemp[nNewpos] );


			// 外部索引于排序后对应编号
			if( NULL != m_ppIndexData )
			{
				vector<int>::iterator itorIndex;
				itorIndex = (*m_ppIndexData).begin() + i;
				if( itorIndex != (*m_ppIndexData).end() )
				{
					(*m_ppIndexData)[i] = m_IndexDataBack[nNewpos];
				}	
			}
		}
		else if( 1 == nType )
		{
			//排序后的列指针数组
			mp_list->GetListData()->SetRowInfo( mp_list->GetListData()->GetSize()-1-i, lVectorItemDataTemp[nNewpos] );

			// 外部索引于排序后对应编号
			if( NULL != m_ppIndexData )
			{
				vector<int>::iterator itorIndex;
				itorIndex = (*m_ppIndexData).begin() + i;
				if( itorIndex != (*m_ppIndexData).end() )
				{
					(*m_ppIndexData)[(*m_ppIndexData).size()-1-i]= m_IndexDataBack[nNewpos];
				}			
			}
			if( mp_list->GetPreSelected() == i )
			{
				mp_list->SetSelected( nNewpos );
			}	
		}

		mapItor++;
	}	
	
	// 选择了的项编号更改
	int selCount = 0;
	vector<UINT>* pSelItem = mp_list->GetCtrlKeySelected(selCount );
	pSelItem->clear();

	for( int i=0; i< mp_list->GetListData()->GetSize(); i++ )
	{
			if( selCount == 0 )
			{
				if( mp_list->GetListData()->GetRowInfo(i)->Selected )
				{
					mp_list->SetSelected( i );
				}
			}
			else
			{
				if( mp_list->GetPreSelected() == -1 )
				{
					if( mp_list->GetListData()->GetRowInfo(i)->Selected )
					{
						pSelItem->push_back(i);
					}
				}
				else
				{
					if( mp_list->GetPreSelected() != -1 ) mp_list->SetSelected( -1 );
					if( mp_list->GetListData()->GetRowInfo(i)->Selected )
					{
						pSelItem->push_back(i);
					}
				}
			}
	}
	mp_list->GetListData()->UpdateBigCount();
	mp_list->GetListData()->UpdateIndex();
	m_sortMap.clear();
}
