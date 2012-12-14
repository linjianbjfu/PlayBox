#pragma once

#include<vector>
using namespace std;

class CMyList;
class CSelectControl
{
public:
	CSelectControl(void);
	~CSelectControl(void);
	void AttachList(HWND hwnd);

private:

	CMyList* mp_list;

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

public:
	Struct_mouseMoveSel m_structMousemoveSel;

	int				m_nPreSelected;
	bool            m_endMoveSel;
	vector<UINT>    m_SelectedVector;

public:
	void CtrolMouseMove(UINT& nFlags,CPoint& point);
	void CtrolRButtonDown(UINT& nFlags,CPoint& point);
	void CtrolLButtonDown(UINT& nFlags, CPoint& point);
	void CtrolLButtonUp(UINT& nFlags, CPoint& point);
	void CtrolAKeyDown();
	void CtrolDownKeyDown();
	void CtrolUpKeyDown();
	int  Ctrol_a_z_KeyDown(short nKey);
private:
	void CleanSelected( bool bHadSel=false);
	void UpDataSelected();
	void UpDataItem(int nItem);
};
