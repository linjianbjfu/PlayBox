#pragma once
# include <map>
using namespace std;
// ���� ��Ӧ��λ����Ϣ���ģ� select����Ϣ���ģ� 
class CMyList;
class CSortControl
{
	// ������������� 
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
