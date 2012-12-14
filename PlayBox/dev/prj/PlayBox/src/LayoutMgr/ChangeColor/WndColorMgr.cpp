#include "StdAfx.h"
#include "WndColorMgr.h"
// $_FUNCTION_BEGIN ******************************
// �������ƣ����캯��
// ����������
// �� �� ֵ��
// ����˵����  
// $_FUNCTION_END ********************************
CWndColorMgr::CWndColorMgr(void)
{
	mpColor = new STRU_CTR_COL_BASE[WND_TYPE_NUM];
	inIt();

}
// $_FUNCTION_BEGIN ******************************
// �������ƣ������캯��
// ����������
// �� �� ֵ��
// ����˵����  
// $_FUNCTION_END ********************************
CWndColorMgr::~CWndColorMgr(void)
{
	if(mpColor)
	{
		delete[] mpColor;
	}
	mpColor = NULL;
}

// $_FUNCTION_BEGIN ******************************
// �������ƣ�getColor
// ����������ENUM_FACE_TYPE �ؼ�����
// �� �� ֵ��PTR_STRU_CTR_COL_BASE ��ɫ�ṹָ��
// ����˵�����õ���ɫ�ṹָ�� 
// $_FUNCTION_END *******************************
PTR_STRU_CTR_COL_BASE CWndColorMgr::getColor(UINT aiFaceType)
{
	return &mpColor[aiFaceType];
}
// $_FUNCTION_BEGIN ******************************
// �������ƣ�setColor
// ����������
// �� �� ֵ��1�ɹ���-1 ʧ��
// ����˵����
// $_FUNCTION_END ********************************
int CWndColorMgr::setColor(UINT aiFaceNum/*�ⲿ�ļ�*/)
{
	return 1;
}
void CWndColorMgr::inIt()
{
	mpColor[BITMAP_WND].structColEX = 0;
	mpColor[BITMAP_WND].structColBase->colBK =  RGB(250,250,250);
	//mpColor[BITMAP_WND].colNormal2 =  RGB(255,255,255);
	mpColor[BITMAP_WND].structColBase->colFrame1 = RGB(109,109,109);

	mpColor[COLOR_WND].structColEX = 0;
	mpColor[COLOR_WND].structColBase->colBK =  RGB(250,250,250);
	//mpColor[COLOR_WND].colNormal2 =  RGB(255,255,255);
	mpColor[COLOR_WND].structColBase->colFrame1 = RGB(109,109,109);

	mpColor[TABLE_CTROL].structColEX = 0;
	mpColor[TABLE_CTROL].structColBase->colFont = RGB(0,0,0);
	mpColor[TABLE_CTROL].structColBase->colBK = RGB(250,250,250);
	mpColor[TABLE_CTROL].colNormal1 =  RGB(222,222,222);
	mpColor[TABLE_CTROL].colNormal2 =  RGB(235,235,235);
	mpColor[TABLE_CTROL].colDown1 = RGB(230,230,230);
	mpColor[TABLE_CTROL].colDown2 =  RGB(250,250,250);
	mpColor[TABLE_CTROL].structColBase->colFrame1 = RGB(109,109,109);

	mpColor[SCROLLBAR_CTROL].colNormal1 = 0xfbfbfb;
	mpColor[SCROLLBAR_CTROL].colNormal2 = 0xe9e9e9;
	mpColor[SCROLLBAR_CTROL].colEnter1 = 0xfcfbf6;
	mpColor[SCROLLBAR_CTROL].colEnter2 = 0xebeae4;
	mpColor[SCROLLBAR_CTROL].colDown1 = 0xebeae4;
	mpColor[SCROLLBAR_CTROL].colDown2 = 0xfcfbf6;
	mpColor[SCROLLBAR_CTROL].structColBase->colFrame1 =0xb6b6b6;
	mpColor[SCROLLBAR_CTROL].structColBase->colFrame2 =0xf8f8f8;
	mpColor[SCROLLBAR_CTROL].colDisable1 = 0xf7f3f3; // ������Ť��ɫ
	mpColor[SCROLLBAR_CTROL].colDisable2 = 0xd7d2c5;
	mpColor[SCROLLBAR_CTROL].structColBase->colFont =0x5a5956;
	mpColor[SCROLLBAR_CTROL].structColBase->colBK = 0x837654;

	mpColor[BITMAP_CTROL].structColEX  = 0;
	mpColor[BITMAP_CTROL].structColBase->colFont =   RGB(4,4,4);
	mpColor[BITMAP_CTROL].structColBase->colFrame1 = RGB(250,250,250);
	mpColor[BITMAP_CTROL].structColBase->colFrame2 = RGB(225,150,0);

	mpColor[COLOL_CTROL].structColEX  = 0;
	mpColor[COLOL_CTROL].structColBase->colFont   = RGB(4,4,4);
	mpColor[COLOL_CTROL].structColBase->colFrame1 = RGB(250,250,250);
	mpColor[COLOL_CTROL].structColBase->colFrame2 = RGB(255,106,0);//RGB(225,150,0);

	mpColor[SLIDER_CTROL].structColBase->colFrame1 = RGB(0,0,0); 
	mpColor[SLIDER_CTROL].structColBase->colFont    = RGB(109,109,109);


}
 CWndColorMgr   gWndColor;