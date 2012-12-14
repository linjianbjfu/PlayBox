#if !defined USLabelH
#define USLabelH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "GdiPlusEx.h"



//���¶���Ϊ��Ϣ
#ifndef MSG_LABEL_MOUSEDOWN
#define MSG_LABEL_MOUSEDOWN		(20001)
#endif  
#ifndef MSG_LABEL_MOUSEUP
#define MSG_LABEL_MOUSEUP		(20002)
#endif 

enum ENUM_PAINT_STYLE {epsNormal, epsOpaque, epsHGradient, epsVGradient, espToolBar,epsInfoHeader};

class CStaticText : public CStatic
{
private:
	//��Ա����
	WNDPROC mWndProc;
	COLORREF mBackColor;
	COLORREF mBeginColor;
	COLORREF mEndColor;
	COLORREF mBorderColor;
	ENUM_PAINT_STYLE mPaintStyle;
	bool mbSetText;
	CString mstrText;
	CString mLinkText;
	COLORREF mTextColor;
	UINT mTextSet;
	HBRUSH mhBrush;		//���ƿͻ���������ɫ�Ļ�ˢ
	int miTextTop;
	LONG mlBold;
	bool mbCaptured;
	bool mbUrl;
	bool mbMouseOver;
	void MouseLeave();
	void MouseEnter();
	void InvalidateBack();
	UINT mFormat;
	UINT mWidth;
	UINT mSize;
	CString m_fontName;

protected:

	//{{AFX_VIRTUAL(CULLabel)
	virtual void PreSubclassWindow();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_VIRTUAL
	static LRESULT CALLBACK SelfProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	//�Զ�����Ϣ�ص�����
	afx_msg HBRUSH CtlColor (CDC* pDC, UINT nCtlColor);
	void DrawFace(HWND hwnd,HDC hDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

public:
	//���죬����
	CStaticText();
	virtual ~CStaticText();

	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�  SetGradient
	// ����������  aBeginColor	���俪ʼ��ɫ
	//             aEndColor    ���������ɫ
	// �� �� ֵ��  ��
	// ����˵����  ���ƽ�����(ˮƽ����)
	// $_FUNCTION_END ********************************
	void SetGradient(COLORREF aBeginColor, COLORREF aEndColor);


	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�  SetVerticalGradient
	// ����������  aBeginColor	���俪ʼ��ɫ
	//             aEndColor    ���������ɫ
	// �� �� ֵ��  ��
	// ����˵����  ���ƽ�����(��ֱ����)
	// $_FUNCTION_END ********************************
	void SetVerticalGradient(COLORREF aBeginColor, COLORREF aEndColor);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�  SetOpaque
	// ����������  aBackColor  ������ɫ
	// �� �� ֵ��  ��
	// ����˵����  ���Ʋ�͸�����
	// $_FUNCTION_END ********************************
	void SetOpaque(COLORREF aBackColor);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�  SetToolBarBK
	// ����������  aBorderColor  �߿���ɫ
	// �� �� ֵ��  ��
	// ����˵����  ���Ʋ�͸�����
	// $_FUNCTION_END ********************************
	void SetToolBarBK(COLORREF aBorderColor);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�  SetNormal
	// ����������  ��
	// �� �� ֵ��  ��
	// ����˵����  ����Ĭ�Ϸ��(͸��)
	// $_FUNCTION_END ********************************
	void SetNormal();

	void SetInfoHeader(/*COLORREF aBorderColor,COLORREF aBackColor*/);


	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�  OnSetText
	// ����������  wParam, lParam  ��Ϣ����
	// �� �� ֵ��  LRESULT
	// ����˵����  �ػ������ı���ϢWM_SETTEXT�¼�
	// $_FUNCTION_END ********************************
	LRESULT OnSetText(WPARAM wParam, LPARAM lParam);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�  SetTextColor
	// ����������  acolTextColor  ��ɫ
	// �� �� ֵ��  ��
	// ����˵���� �����ı���ɫ
	// $_FUNCTION_END ********************************
	void SetTextColor(COLORREF acolTextColor);

	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�  SetTextColor
	// ����������  acolTextColor  ��ɫ
	// �� �� ֵ��  ��
	// ����˵���� �����ı���ɫ
	// $_FUNCTION_END ********************************
	

	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�  SetTextColor
	// ����������  acolTextColor  ��ɫ
	// �� �� ֵ��  ��
	// ����˵���� �����ı���ɫ
	// $_FUNCTION_END ********************************
	void SetFontBold(LONG alBold = FW_BOLD);



	// $_FUNCTION_BEGIN ******************************
	// �������ƣ�  SetTextTop
	// ����������  aiValue  �ı����ϱ߾�
	// �� �� ֵ��  ��
	// ����˵���� �����ı��ϱ߾�
	// $_FUNCTION_END ********************************
	void SetTextTop(int aiValue );

	BOOL SetLinkAddress(CString linkaddress);
	BOOL EnableWindow(BOOL aValue = TRUE);

	void SetTextFormat(UINT anFormat);
	void SetTextWidth(UINT anWidth);
	void SetTextSize(int aiFontSize);
	void SetTextSet( UINT anTextSet );
	void SetFontName( CString fontName){m_fontName = fontName;}
};
#endif