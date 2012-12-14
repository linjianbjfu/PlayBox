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
// ȫ����ɫ
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

// ���ﶨ����Щ�ؼ���Ҫ��չ����ɫ
typedef COLORREF SCROLL_COLORREF_EX;
//ĳ����Ҫ������ɫ�Ŀؼ���չ��ɫ������
typedef union CtrColType
{
	SCROLL_COLORREF_EX colScrollEX;
//  ��������
}UNION_COL_EX;
//��Ҫ������ɫ�Ŀؼ���չ��ɫ
typedef struct CtrColExtend
{
	CtrColExtend() {};
	~CtrColExtend()	{};
	UNION_COL_EX corEX1;
}STRU_CTR_COL_EXTEND,* PTR_STRU_CTR_COL_EXTEND;

// �ؼ�������ɫ
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
	//�����ɫ
	 PTR_STRU_CTR_COL_BASE getColor(UINT aiFaceType);
	//������ɫ
	int setColor(UINT aiFaceNum/*filename*/);
private:
	PTR_STRU_CTR_COL_BASE mpColor;
	void inIt();
};
