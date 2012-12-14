#pragma once
# include <map>
using namespace std;
// 排序： 对应项位置信息更改； select项信息更改； 
class CMyList;
class CSortControl
{
	// 红黑树快速排序 
public:
	CSortControl(void);
	~CSortControl(void);
private:
	CMyList* mp_list;
	vector<int>* m_ppIndexData;
	vector<int> m_IndexDataBack;
	map<CString,UINT> m_sortMap;
public:
	void AttachList(HWND hwnd);
	void SortItem( int nRow, int nType );
	void SetSortIndex( vector<int>* pIndexData);
};
