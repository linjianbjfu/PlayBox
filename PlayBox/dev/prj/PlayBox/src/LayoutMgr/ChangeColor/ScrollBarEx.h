#ifndef ScrollBarExH
#define ScrollBarExH
// zhou peng , build in sina

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//计算数组长度
#ifndef COUNT_OF
#define COUNT_OF(x)	(sizeof(x) / sizeof((x)[0]))
#endif

//计算奇偶数
#ifndef IS_ODD
#define IS_ODD(x) (x & 0x1)
#endif

#ifndef WM_PDLG_MOUSEWHEEL_MSG 
#define WM_PDLG_MOUSEWHEEL_MSG WM_USER+1000
#endif 

enum ENUM_ScrollOrientation {esoHorizontal, esoVertical};	//滚动条方向

class CScrollBarEx : public CStatic  
{
private:
#define HT_UPARROW		100
#define HT_DOWNARROW	101
#define HT_LEFTARROW	102
#define HT_RIGHTARROW	103
#define HT_THUMB		104
#define HT_THUMB_SPACE	105

	CWnd* mpWnd;
	
	WNDPROC mWndProc;;
	int miScrollPos;		//滚动条位置
	int miScrollRange;		//滚动条最大滚动范围
	SCROLLINFO mScrollInfo;			//滚动条信息
	ENUM_ScrollOrientation mScrollOrientation;		//滚动条方向
	RECT mrcClient;			//工作区域
	int miButtonSize;		//箭头近钮大小
	bool mbEnabled;			//有些控件在无数据时显示滚动条，此时应设置为禁用显示状态

	bool mbTrackLeave;		//鼠标离开状态
	bool mbLButtonDown;		//鼠标左键按下状态
	bool mbUpArrowDown, mbDownArrowDown, mbThumbDown, mbThumbSpaceDown;		//向上按下，向下按下，滑块按下，滑轨按下
	UINT miLastHitTest;		//最后一次点击测试的值
	UINT miLButtonDownHitTest;		//鼠标左键按下时测试值
	CPoint mPointDragOffset;	//鼠标在滑块上按下时距离滑块中心的位移
	//CPoint mPointMoveOffset;
	//int  mPointOffsetSum;

	COLORREF mcolNormal1;
	COLORREF mcolNormal2;
	COLORREF mcolEnter1;
	COLORREF mcolEnter2;
	COLORREF mcolDown1;
	COLORREF mcolDown2;
	COLORREF mcolFrame1;
	COLORREF mcolFrame2;
    COLORREF mcolBtn1;
	COLORREF mcolBtn2;
	COLORREF mcolFont;
	COLORREF mcolBK;

	BOOL     misDlg;

	// $_FUNCTION_BEGIN ******************************
	// 函数名称：Draw
	// 函数参数：hWnd	窗体句柄
	//			 hDC	绘画设备
	// 返 回 值：NULL
	// 函数说明：绘制控制
	// $_FUNCTION_END ********************************
	void Draw(HWND hWnd, HDC hDC);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：DrawLeftBtn
	// 函数参数：hDC	绘画设备
	//			 aRect	绘画区域
	// 返 回 值：NULL
	// 函数说明：绘制向左箭头按钮
	// $_FUNCTION_END ********************************
	void DrawLeftBtn(HDC hDC, RECT aRect);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：DrawRightBtn
	// 函数参数：hDC	绘画设备
	//			 aRect	绘画区域
	// 返 回 值：NULL
	// 函数说明：绘制向右箭头按钮
	// $_FUNCTION_END ********************************
	void DrawRightBtn(HDC hDC, RECT aRect);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：DrawUpBtn
	// 函数参数：hDC	绘画设备
	//			 aRect	绘画区域
	// 返 回 值：NULL
	// 函数说明：绘制向上箭头按钮
	// $_FUNCTION_END ********************************
	void DrawUpBtn(HDC hDC, RECT aRect);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：DrawDownBtn
	// 函数参数：hDC	绘画设备
	//			 aRect	绘画区域
	// 返 回 值：NULL
	// 函数说明：绘制向下箭头按钮
	// $_FUNCTION_END ********************************
	void DrawDownBtn(HDC hDC, RECT aRect);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：DrawThumb
	// 函数参数：hDC	绘画设备
	//			 aRect	绘画区域
	// 返 回 值：NULL
	// 函数说明：绘制滑块
	// $_FUNCTION_END ********************************
	void DrawThumb(HDC hDC, RECT aRect);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：DrawThumbSpace
	// 函数参数：hDC	绘画设备
	//			 aRect	绘画区域
	// 返 回 值：NULL
	// 函数说明：绘制滑轨
	// $_FUNCTION_END ********************************
	void DrawThumbSpace(HDC hDC, RECT aRect);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：DrawHighlightFace
	// 函数参数：hDC	绘画设备
	//			 aRect	绘画区域
	//			 aiType 类型(0:左箭头1:右箭头2:上箭头3:下箭头4:滑块)
	// 返 回 值：NULL
	// 函数说明：绘制高亮时背景
	// $_FUNCTION_END ********************************
	void DrawHighlightFace(HDC aDC, RECT aRect, int aiType);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：DrawDownFace
	// 函数参数：hDC	绘画设备
	//			 aRect	绘画区域
	//			 aiType 类型(0:左箭头1:右箭头2:上箭头3:下箭头4:滑块)
	// 返 回 值：NULL
	// 函数说明：绘制鼠标按下时背景
	// $_FUNCTION_END ********************************
	void DrawDownFace(HDC aDC, RECT aRect, int aiType);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：DrawNormalFace
	// 函数参数：hDC	绘画设备
	//			 aRect	绘画区域
	//			 aiType 类型(0:左箭头1:右箭头2:上箭头3:下箭头4:滑块)
	// 返 回 值：NULL
	// 函数说明：绘制默认背景
	// $_FUNCTION_END ********************************
	void DrawNormalFace(HDC aDC, RECT aRect, int aiType);
	
	
	
	// $_FUNCTION_BEGIN ******************************
	// 函数名称：HitTest
	// 函数参数：aPoint	测试坐标
	// 返 回 值：测试结果(鼠标所在区域的控制类型)
	// 函数说明：测试控制类型
	// $_FUNCTION_END ********************************
	UINT HitTest(CPoint aPoint);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：HitTest
	// 函数参数：
	// 返 回 值：测试结果(鼠标所在区域的控制类型)
	// 函数说明：测试当前鼠标所在位置的控制类型
	// $_FUNCTION_END ********************************
	UINT HitTest();



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：GetRect
	// 函数参数：aiHitTest
	// 返 回 值：控制区域
	// 函数说明：测试值对应的控制区域
	// $_FUNCTION_END ********************************
	RECT GetRect(UINT aiHitTest);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：MoveThumb
	// 函数参数：aPointOffset	鼠标距离滑中心的位移
	// 返 回 值：NULL
	// 函数说明：鼠标拖动滑块移动控制
	// $_FUNCTION_END ********************************
	void MoveThumb(CPoint aPointOffset);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：TrackSpace
	// 函数参数：
	// 返 回 值：NULL
	// 函数说明：鼠标点击滑轨移动滑块的控制
	// $_FUNCTION_END ********************************
	void TrackSpace();
	


	// $_FUNCTION_BEGIN ******************************
	// 函数名称：NotifyControlWnd
	// 函数参数：aiMsg	消息值
	//			 aiValue	消息参数值
	// 返 回 值：NULL
	// 函数说明：通知控制窗体相应的消息
	// $_FUNCTION_END ********************************
	void NotifyControlWnd(UINT aiMsg, int aiValue);

protected:
	afx_msg void OnLButtonDown(UINT aiFlags, CPoint aPoint);
	afx_msg void OnLButtonDblClk(UINT aiFlags, CPoint aPoint);
	afx_msg void OnLButtonUp(UINT aiFlags, CPoint aPoint);
	afx_msg void OnMouseMove(UINT aiFlags, CPoint aPoint);
	afx_msg void OnTimer(UINT aiIDEvent);
	afx_msg LRESULT OnDlgMouseWheel(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);

	virtual void PreSubclassWindow();
	static LRESULT CALLBACK SelfProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	//自定义消息回调函数

	DECLARE_MESSAGE_MAP()

public:
	CScrollBarEx();
	virtual ~CScrollBarEx();

	// $_FUNCTION_BEGIN ******************************
	// 函数名称：GetScrollInfo
	// 函数参数：aScrollInfo	滚动条信息
	//			 aiMask		获取类别选择
	// 返 回 值：是否成功
	// 函数说明：获取滚动条信息
	// $_FUNCTION_END ********************************
	BOOL GetScrollInfo(LPSCROLLINFO aScrollInfo, UINT aiMask = SIF_ALL);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：GetScrollLimit
	// 函数参数：
	// 返 回 值：最大滚动位置
	// 函数说明：获取最大滚动位置
	// $_FUNCTION_END ********************************
	int GetScrollLimit();



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：GetScrollPos
	// 函数参数：
	// 返 回 值：滚动位置
	// 函数说明：获取滚动位置
	// $_FUNCTION_END ********************************
	int GetScrollPos();



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：GetScrollRange
	// 函数参数：aiMinPos	滚动最小值
	//			 aiMaxPos	滚动最大值
	// 返 回 值：
	// 函数说明：获取滚动范围
	// $_FUNCTION_END ********************************
	void GetScrollRange(LPINT aiMinPos, LPINT aiMaxPos);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：GetScrollOrientation
	// 函数参数：
	// 返 回 值：滚动条方向
	// 函数说明：获取滚动条方向
	// $_FUNCTION_END ********************************
	ENUM_ScrollOrientation GetScrollOrientation();



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：SetControlWnd
	// 函数参数：apWnd	窗体句柄
	// 返 回 值：
	// 函数说明：设置控制窗体句柄
	// $_FUNCTION_END ********************************
	void SetControlWnd(CWnd* apWnd);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：SetScrollInfo
	// 函数参数：aScrollInfo	滚动条信息
	//			 abRedraw	是否重绘
	// 返 回 值：是否成功
	// 函数说明：设置滚动条信息
	// $_FUNCTION_END ********************************
	BOOL SetScrollInfo(LPSCROLLINFO aScrollInfo, BOOL abRedraw = TRUE);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：SetScrollPos
	// 函数参数：aiPos	滚动条位置
	//			 abRedraw	是否重绘
	// 返 回 值：设置成功的位置
	// 函数说明：设置滚动条位置
	// $_FUNCTION_END ********************************
	int SetScrollPos(int aiPos, BOOL abRedraw = TRUE);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：SetScrollRange
	// 函数参数：aiMinPos	滚动最小值
	//			 aiMaxPos	滚动最大值
	//			 abRedraw	是否重绘
	// 返 回 值：
	// 函数说明：设置滚动条范围
	// $_FUNCTION_END ********************************
	void SetScrollRange(int aiMinPos, int aiMaxPos, BOOL abRedraw = TRUE);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：SetScrollOrientation
	// 函数参数：aScrollOrientation		滚动方向
	// 返 回 值：
	// 函数说明：设置滚动条方向
	// $_FUNCTION_END ********************************
	void SetScrollOrientation(ENUM_ScrollOrientation aScrollOrientation);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：SetPosition
	// 函数参数：
	// 返 回 值：NULL
	// 函数说明：放置滚动条的位置
	// $_FUNCTION_END ********************************
	void SetPosition();


	// $_FUNCTION_BEGIN ******************************
	// 函数名称：UpdateScrollBar
	// 函数参数：
	// 返 回 值：NULL
	// 函数说明：更新滚动条信息并重绘
	// $_FUNCTION_END ********************************
	void UpdateScrollBar();


public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
#endif