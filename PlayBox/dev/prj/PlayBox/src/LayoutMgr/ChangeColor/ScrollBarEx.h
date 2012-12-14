#ifndef ScrollBarExH
#define ScrollBarExH
// zhou peng , build in sina

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//�������鳤��
#ifndef COUNT_OF
#define COUNT_OF(x)	(sizeof(x) / sizeof((x)[0]))
#endif

//������ż��
#ifndef IS_ODD
#define IS_ODD(x) (x & 0x1)
#endif

#ifndef WM_PDLG_MOUSEWHEEL_MSG 
#define WM_PDLG_MOUSEWHEEL_MSG WM_USER+1000
#endif 

enum ENUM_ScrollOrientation {esoHorizontal, esoVertical};	//����������

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
	int miScrollPos;		//������λ��
	int miScrollRange;		//��������������Χ
	SCROLLINFO mScrollInfo;			//��������Ϣ
	ENUM_ScrollOrientation mScrollOrientation;		//����������
	RECT mrcClient;			//��������
	int miButtonSize;		//��ͷ��ť��С
	bool mbEnabled;			//��Щ�ؼ���������ʱ��ʾ����������ʱӦ����Ϊ������ʾ״̬

	bool mbTrackLeave;		//����뿪״̬
	bool mbLButtonDown;		//����������״̬
	bool mbUpArrowDown, mbDownArrowDown, mbThumbDown, mbThumbSpaceDown;		//���ϰ��£����°��£����鰴�£����찴��
	UINT miLastHitTest;		//���һ�ε�����Ե�ֵ
	UINT miLButtonDownHitTest;		//����������ʱ����ֵ
	CPoint mPointDragOffset;	//����ڻ����ϰ���ʱ���뻬�����ĵ�λ��
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
	// �������ƣ�Draw
	// ����������hWnd	������
	//			 hDC	�滭�豸
	// �� �� ֵ��NULL
	// ����˵�������ƿ���
	// $_FUNCTION_END ********************************
	void Draw(HWND hWnd, HDC hDC);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�DrawLeftBtn
	// ����������hDC	�滭�豸
	//			 aRect	�滭����
	// �� �� ֵ��NULL
	// ����˵�������������ͷ��ť
	// $_FUNCTION_END ********************************
	void DrawLeftBtn(HDC hDC, RECT aRect);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�DrawRightBtn
	// ����������hDC	�滭�豸
	//			 aRect	�滭����
	// �� �� ֵ��NULL
	// ����˵�����������Ҽ�ͷ��ť
	// $_FUNCTION_END ********************************
	void DrawRightBtn(HDC hDC, RECT aRect);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�DrawUpBtn
	// ����������hDC	�滭�豸
	//			 aRect	�滭����
	// �� �� ֵ��NULL
	// ����˵�����������ϼ�ͷ��ť
	// $_FUNCTION_END ********************************
	void DrawUpBtn(HDC hDC, RECT aRect);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�DrawDownBtn
	// ����������hDC	�滭�豸
	//			 aRect	�滭����
	// �� �� ֵ��NULL
	// ����˵�����������¼�ͷ��ť
	// $_FUNCTION_END ********************************
	void DrawDownBtn(HDC hDC, RECT aRect);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�DrawThumb
	// ����������hDC	�滭�豸
	//			 aRect	�滭����
	// �� �� ֵ��NULL
	// ����˵�������ƻ���
	// $_FUNCTION_END ********************************
	void DrawThumb(HDC hDC, RECT aRect);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�DrawThumbSpace
	// ����������hDC	�滭�豸
	//			 aRect	�滭����
	// �� �� ֵ��NULL
	// ����˵�������ƻ���
	// $_FUNCTION_END ********************************
	void DrawThumbSpace(HDC hDC, RECT aRect);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�DrawHighlightFace
	// ����������hDC	�滭�豸
	//			 aRect	�滭����
	//			 aiType ����(0:���ͷ1:�Ҽ�ͷ2:�ϼ�ͷ3:�¼�ͷ4:����)
	// �� �� ֵ��NULL
	// ����˵�������Ƹ���ʱ����
	// $_FUNCTION_END ********************************
	void DrawHighlightFace(HDC aDC, RECT aRect, int aiType);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�DrawDownFace
	// ����������hDC	�滭�豸
	//			 aRect	�滭����
	//			 aiType ����(0:���ͷ1:�Ҽ�ͷ2:�ϼ�ͷ3:�¼�ͷ4:����)
	// �� �� ֵ��NULL
	// ����˵����������갴��ʱ����
	// $_FUNCTION_END ********************************
	void DrawDownFace(HDC aDC, RECT aRect, int aiType);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�DrawNormalFace
	// ����������hDC	�滭�豸
	//			 aRect	�滭����
	//			 aiType ����(0:���ͷ1:�Ҽ�ͷ2:�ϼ�ͷ3:�¼�ͷ4:����)
	// �� �� ֵ��NULL
	// ����˵��������Ĭ�ϱ���
	// $_FUNCTION_END ********************************
	void DrawNormalFace(HDC aDC, RECT aRect, int aiType);
	
	
	
	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�HitTest
	// ����������aPoint	��������
	// �� �� ֵ�����Խ��(�����������Ŀ�������)
	// ����˵�������Կ�������
	// $_FUNCTION_END ********************************
	UINT HitTest(CPoint aPoint);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�HitTest
	// ����������
	// �� �� ֵ�����Խ��(�����������Ŀ�������)
	// ����˵�������Ե�ǰ�������λ�õĿ�������
	// $_FUNCTION_END ********************************
	UINT HitTest();



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�GetRect
	// ����������aiHitTest
	// �� �� ֵ����������
	// ����˵��������ֵ��Ӧ�Ŀ�������
	// $_FUNCTION_END ********************************
	RECT GetRect(UINT aiHitTest);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�MoveThumb
	// ����������aPointOffset	�����뻬���ĵ�λ��
	// �� �� ֵ��NULL
	// ����˵��������϶������ƶ�����
	// $_FUNCTION_END ********************************
	void MoveThumb(CPoint aPointOffset);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�TrackSpace
	// ����������
	// �� �� ֵ��NULL
	// ����˵��������������ƶ�����Ŀ���
	// $_FUNCTION_END ********************************
	void TrackSpace();
	


	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�NotifyControlWnd
	// ����������aiMsg	��Ϣֵ
	//			 aiValue	��Ϣ����ֵ
	// �� �� ֵ��NULL
	// ����˵����֪ͨ���ƴ�����Ӧ����Ϣ
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
	static LRESULT CALLBACK SelfProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	//�Զ�����Ϣ�ص�����

	DECLARE_MESSAGE_MAP()

public:
	CScrollBarEx();
	virtual ~CScrollBarEx();

	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�GetScrollInfo
	// ����������aScrollInfo	��������Ϣ
	//			 aiMask		��ȡ���ѡ��
	// �� �� ֵ���Ƿ�ɹ�
	// ����˵������ȡ��������Ϣ
	// $_FUNCTION_END ********************************
	BOOL GetScrollInfo(LPSCROLLINFO aScrollInfo, UINT aiMask = SIF_ALL);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�GetScrollLimit
	// ����������
	// �� �� ֵ��������λ��
	// ����˵������ȡ������λ��
	// $_FUNCTION_END ********************************
	int GetScrollLimit();



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�GetScrollPos
	// ����������
	// �� �� ֵ������λ��
	// ����˵������ȡ����λ��
	// $_FUNCTION_END ********************************
	int GetScrollPos();



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�GetScrollRange
	// ����������aiMinPos	������Сֵ
	//			 aiMaxPos	�������ֵ
	// �� �� ֵ��
	// ����˵������ȡ������Χ
	// $_FUNCTION_END ********************************
	void GetScrollRange(LPINT aiMinPos, LPINT aiMaxPos);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�GetScrollOrientation
	// ����������
	// �� �� ֵ������������
	// ����˵������ȡ����������
	// $_FUNCTION_END ********************************
	ENUM_ScrollOrientation GetScrollOrientation();



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�SetControlWnd
	// ����������apWnd	������
	// �� �� ֵ��
	// ����˵�������ÿ��ƴ�����
	// $_FUNCTION_END ********************************
	void SetControlWnd(CWnd* apWnd);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�SetScrollInfo
	// ����������aScrollInfo	��������Ϣ
	//			 abRedraw	�Ƿ��ػ�
	// �� �� ֵ���Ƿ�ɹ�
	// ����˵�������ù�������Ϣ
	// $_FUNCTION_END ********************************
	BOOL SetScrollInfo(LPSCROLLINFO aScrollInfo, BOOL abRedraw = TRUE);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�SetScrollPos
	// ����������aiPos	������λ��
	//			 abRedraw	�Ƿ��ػ�
	// �� �� ֵ�����óɹ���λ��
	// ����˵�������ù�����λ��
	// $_FUNCTION_END ********************************
	int SetScrollPos(int aiPos, BOOL abRedraw = TRUE);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�SetScrollRange
	// ����������aiMinPos	������Сֵ
	//			 aiMaxPos	�������ֵ
	//			 abRedraw	�Ƿ��ػ�
	// �� �� ֵ��
	// ����˵�������ù�������Χ
	// $_FUNCTION_END ********************************
	void SetScrollRange(int aiMinPos, int aiMaxPos, BOOL abRedraw = TRUE);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�SetScrollOrientation
	// ����������aScrollOrientation		��������
	// �� �� ֵ��
	// ����˵�������ù���������
	// $_FUNCTION_END ********************************
	void SetScrollOrientation(ENUM_ScrollOrientation aScrollOrientation);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�SetPosition
	// ����������
	// �� �� ֵ��NULL
	// ����˵�������ù�������λ��
	// $_FUNCTION_END ********************************
	void SetPosition();


	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�UpdateScrollBar
	// ����������
	// �� �� ֵ��NULL
	// ����˵�������¹�������Ϣ���ػ�
	// $_FUNCTION_END ********************************
	void UpdateScrollBar();


public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
#endif