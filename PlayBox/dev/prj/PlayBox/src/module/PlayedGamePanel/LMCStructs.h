#pragma once

#define ITEMTXTH		15			// 名称区域
#define ITEMDETAILTXTH	13			// 详细信息区域
#define	SPACEH			4			// 间距高度
#define SPACEW			16			// 水平间距
#define SPACEWS			10
#define	VSCROLLMIN		40			// 向下滚动幅度
#define VSCROLLMAX		100			// 向下滚动幅度

struct GdiRes
{
	GdiRes()
	{
		colListBK = RGB(240,247,252);
		colName   = RGB(77,77,77);
		colDetail = RGB(125,125,125);
	}
	
	COLORREF	colListBK;		// 列表背景颜色
	COLORREF	colName;		// 项名称的颜色
	COLORREF	colDetail;		// 详细名称的颜色
};

// list图片状态
enum LMCImgState
{
	LMCI_Small=0,	// 小图片
	LMCI_Middle,	// 中等图片
	LMCI_Big		// 大图片
};


// 描述每一项结构
typedef struct _tagLMCItemInfo
{
	string		strGID;				// 游戏ID
	int			iIndex;				// 项唯一标识
	BOOL		blSel;				// 是否选中
	CDibBitmap* pImg;				// 显示图片
	CString		strItemName;		// 项的名称
	CString		strItemdDetailinfo;	// 项的详细信息
	CString		strImgPath;			// 图片路径
	CRect		rc;					// 项图片的坐标，传出值
}LMC_ItemInfo;
