#pragma once
#include <vector>
#include <map>
using namespace std;

#define MSG_MOUSELEAVE (WM_USER+1000)
#define MSG_MOUSEHOVER (WM_USER+1001)
#define MSG_MOUSELEAVE_ITEM (WM_USER+1003)
#define MSG_MOUSEHOVER_ITEM (WM_USER+1004)
#define MSG_MOUSELEAVE_TEXT (WM_USER+1005)
#define MSG_MOUSEHOVER_TEXT (WM_USER+1006)
// 插入数据项参数


typedef struct InsertDataStruct     
{
public:
	CString strText;
	CString strIextLine;
	int     nbmpType;
	int     nbmpTypeIndex;
	bool    bmpMouseTrack;
	bool    textMouseTrack;
	bool    IsExLine;
	InsertDataStruct()
		: strText(""),
		strIextLine(""),
		nbmpType(0),
		nbmpTypeIndex(0),
		bmpMouseTrack(false),
		IsExLine( false )
	{
	}
}INSERT_DATASTRUCT;

//感应鼠标事件的图片
class CMyList;
class CBitmapRectData
{
#ifndef BMP_INTERVAL
#define BMP_INTERVAL (4)
#endif
public:
	CBitmapRectData();
private:
	void SendMsgMouseLeave(int& nLine, int& nRow);
	void SendMsgMouseInBmp(int& nLine, int& nRow);

	void SendMsgMouseLeaveItem(int& nLine, int& nRow);
	void SendMsgMouseInItem(int& nLine, int& nRow);
	
	int  m_preLineBmpPos;
	int  m_preRowBmpPos;


	int  m_leaveLineBmpPos;
	int  m_leaveRowBmpPos;


	int  m_preLinePos;
	int  m_preRowPos;

	int  m_leaveLinePos;
	int  m_leaveRowPos;

	map<int,SIZE> m_bmpSize;
	map<int, map<int,CDibBitmap*> > m_bmp;  // < 列编号， <图片编号， 图片指针> > 图片编号： 正常 0，高亮1，按下2， 同列其他形状图片 累加 3...;特殊情况特殊指定
	CMyList* mpList;
public:
	void AttachList(HWND hwnd);
	void CtrolMouseMove(UINT nFlags,CPoint point);
	void CtrolMouseLive();
	void GetBitmapRect(CRect& bmpRect,int& nLine,int& nRow);
	int GetBmpSize( int nRow,CSize& size);
	void SetBitmap( int nRow,int nBmpType, CDibBitmap* pBmp);
	CDibBitmap* GetBitmap( int nRow, int nType );
	int GetRowBmpCount(int row);
	int GetRowTextCount(int row);
};
class CTextRectData
{
public:
	CTextRectData();
private:
	void SendMsgMouseInText(int& nLine, int& nRow);
	void SendMsgMouseLeaveText(int& nLine, int& nRow);
	int  m_preLineTextPos;
	int  m_preRowTextPos;

	int  m_leaveLineTextPos;
	int  m_leaveRowTextPos;
	CMyList* mpList;
	public:
	void AttachList(HWND hwnd);
	void CtrolMouseMove(UINT nFlags,CPoint& point);
	void CtrolMouseLive();
	void GetTextRect(CRect& textRect,int& nLine,int& nRow);

};
class CListData;

//项数据结构
typedef class ItemData 
{
	friend class CListData;
	friend class CDrawList;
	friend class CSortControl;
	friend class CBitmapRectData;
	friend class CTextRectData;
	friend class CMyList;
private:
	class ItemDataRow
	{
	public:
		CString strText;                //每项文字
		pair<int , int> bmpItem;        //图片属性 <类型编号,每个类型图片不同图片> 无图片<-1,-1>
		pair<int, bool> bmpMouseTrack;  //是否有鼠标状态<类型编号,bool>
		pair<int, bool> textMouseTrack;  //是否有鼠标状态<类型编号,bool>
		ItemDataRow()
			:strText("")
		{
			bmpItem.first = -1;
			bmpItem.second = -1;
			bmpMouseTrack.first = -1;
			bmpMouseTrack.second = false;
			textMouseTrack.first = -1;
			textMouseTrack.second = false;
		}
		ItemDataRow& operator=(const ItemDataRow& anItemDataRow );
	};
	
	vector<ItemDataRow>  rowData;
	CString strLineText;
	UINT    frontBigCout;              //前面宽项目个数， 计算鼠标点击移动定项计算
	bool    Selected;
public:
	ItemData(UINT count);
	ItemData& operator=(const ItemData& anItemData);
}ITEMDATA_INFO,*P_ITEMDATA_INFO;

//list数据
class CListData
{
public:
	CListData(void);
	~CListData(void);
	
private:
	vector<P_ITEMDATA_INFO> m_vectorItemData;     //list数据结构
	UINT                    m_rowCount;           //项列数
	vector<int>             m_index;              //等高项目的虚拟编号和 list的项对应索引
//图片显示模式
	typedef struct bmp_mode
	{
		int showPos;
		int showType;
		bmp_mode(  )
		{
			showPos =  -1;
			showType = -1;
		}
	}BMP_MODE;
	map<int,BMP_MODE>  m_rowBmpMode;              //列图片显示位置和模式

	typedef struct text_mode
	{
		int showPos;
		int showType;
		text_mode(  )
		{
			showPos =  -1;
			showType = -1;
		}
	}TEXT_MODE;
	map<int,TEXT_MODE>  m_rowTextMode;              //列图片显示位置和模式

	CBitmapRectData m_bmpRect;
	CTextRectData   m_textRect;
public:
	void AttachList(HWND hwnd);
	void SetRow(UINT nCount);
	int  InsertLine();
	int  InsertLine(UINT nIndex);
	void DeleteLine(UINT nIndex);
	void DeleteAllLine();
	void DeleteAllLine(vector<P_ITEMDATA_INFO>* dataVector);
	bool SetItemData(UINT nLine, UINT nRow,CString& str );
	bool SetItemData(UINT nLine, UINT nRow,CString& str, int nbmpType,int nbmpTypeIndex, bool isMouseTrack= false  );
	bool SetItemData(UINT nLine, UINT nRow, int nbmpType,int nbmpTypeIndex, bool isMouseTrack= false  );
	bool SetItemData(UINT nLine, UINT nRow,CString& str,CString& strEx, int nbmpType,int nbmpTypeIndex, bool isMouseTrack= false  );
	bool SetItemData(UINT nLine, UINT nRow,INSERT_DATASTRUCT& DateStruct  );
	bool GetItemData(UINT& nLine, UINT& nRow, CString& str )const;
	bool GetItemData(UINT& nLine, UINT& nRow, CString& str,CString& strEx)const; 
	const char* GetItemStrPtr(UINT& nLine, UINT& nRow);
	int GetSize()const;
	bool UpdateIndex(int nIndex=0);               // 更新索引
	// 索引帮助鼠标移动，点击事件快速查找到对应的项
	// 多次SetItemData之后 UpdateIndex
	void UpdateBigCount();
	bool GetRealIndex(int Index, int& RealIndex)const;
	void SelectItem(int RealIndex);
	const P_ITEMDATA_INFO GetRowInfo(int index)const;
	void  SetRowInfo(int index, P_ITEMDATA_INFO info);
	const vector<P_ITEMDATA_INFO>* GetDataVector()const;
	bool IsSelected( int index );
	void SetRowBmpMode(int nRow, int nShowPos=-1, int nShowType=-1);
	void SetRowTextMode(int nRow, int nShowPos=-1, int nShowType=-1);
	int  GetRowBmpShowMode(int nRow);
	int  GetRowTextShowMode(int nRow);
	int  GetRowBmpShowType(int nRow);
	int  GetRowTextShowType(int nRow);
	 CBitmapRectData*  GetbmpRectData(){ return &m_bmpRect;}
	CTextRectData*  GetTextRectData(){ return &m_textRect;}
	void CtrolMouseLive();

};


