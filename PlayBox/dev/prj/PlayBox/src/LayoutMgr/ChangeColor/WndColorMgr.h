#pragma once

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef enum FaceType
{
  BITMAP_WND,
  COLOR_WND,
  TABLE_CTROL,
  SCROLLBAR_CTROL,
  BITMAP_CTROL,
  COLOL_CTROL,
  SLIDER_CTROL,
  WND_TYPE_NUM
}ENUM_FACE_TYPE;
// 全局颜色
typedef struct BaseColor
{
	BaseColor()	{};
	~BaseColor() {};
  
  COLORREF colFrame1;
  COLORREF colFrame2;
  COLORREF colBody;
  COLORREF colFont;
  COLORREF colBK;
}STRU_BASE_COLOR,* PTR_STRU_BASE_COLOR;

// 这里定义哪些控件需要扩展的颜色
typedef COLORREF SCROLL_COLORREF_EX;
//某个需要其他颜色的控件扩展颜色的联合
typedef union CtrColType
{
	SCROLL_COLORREF_EX colScrollEX;
//  其他类型
}UNION_COL_EX;
//需要其他颜色的控件扩展颜色
typedef struct CtrColExtend
{
	CtrColExtend() {};
	~CtrColExtend()	{};
	UNION_COL_EX corEX1;
}STRU_CTR_COL_EXTEND,* PTR_STRU_CTR_COL_EXTEND;

// 控件基本颜色
typedef struct CtrColBase
{
  PTR_STRU_BASE_COLOR structColBase;
  PTR_STRU_CTR_COL_EXTEND structColEX;
  COLORREF colNormal1;
  COLORREF colNormal2;
  COLORREF colEnter1;
  COLORREF colEnter2;
  COLORREF colDown1;
  COLORREF colDown2;
  COLORREF colDisable1;
  COLORREF colDisable2;
	CtrColBase()
	{
		structColBase = new STRU_BASE_COLOR;
	};
	~CtrColBase()
	{
		delete structColBase;
	};
} STRU_CTR_COL_BASE,* PTR_STRU_CTR_COL_BASE;
 
class CWndColorMgr
{	
public:
	~CWndColorMgr(void);
	CWndColorMgr(void);
	//获得颜色
	 PTR_STRU_CTR_COL_BASE getColor(UINT aiFaceType);
	//设置颜色
	int setColor(UINT aiFaceNum/*filename*/);
private:
	PTR_STRU_CTR_COL_BASE mpColor;
	void inIt();
};
