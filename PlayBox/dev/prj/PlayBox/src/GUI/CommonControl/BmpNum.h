#pragma once


// CBmpNum
#include "../Bitmap/DibBitmap.h"

class CBmpNum : public CWnd
{
	DECLARE_DYNAMIC(CBmpNum)
	
public:
	CBmpNum();
	virtual ~CBmpNum();
	enum CTROL_TYPE{LEFT,RIGHG,UNINT,ABITMAP=4,TIME=8}; 
public:

	CDibBitmap*  m_pNumBitmap;
	CDibBitmap*  m_pBKBitmap;

	
	void SetNum( int anum);
	void SetNum( CString strTime);
	void SetDrawType(UINT antype){m_ctrolType = antype;}  //数字左对齐显示、右对齐显示
	void SetShowStyle(int bTrans){ m_nShowStyle = bTrans; }
	void SetMaxDigit(UINT nDigit){ m_digit=nDigit; };

	void SetImage(const char* name);
	void SetBackImage(const char* name);

private:
	short m_armIntg[20]; // int类型数字范围
	UINT m_ctrolType;
	bool  m_nShowStyle;
	int   m_num;
	UINT  m_digit;  // 最大位数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


