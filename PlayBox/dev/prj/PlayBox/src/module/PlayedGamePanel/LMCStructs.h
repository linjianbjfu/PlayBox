#pragma once

#define ITEMTXTH		15			// ��������
#define ITEMDETAILTXTH	13			// ��ϸ��Ϣ����
#define	SPACEH			4			// ���߶�
#define SPACEW			30			// ˮƽ���
#define SPACEWS			10
#define	VSCROLLMIN		40			// ���¹�������
#define VSCROLLMAX		100			// ���¹�������
#define START_X_LEFT	45			//��������Ŀհ�����
#define END_X_RIGHT		45			//�ұ������Ŀհ�����

struct GdiRes
{
	GdiRes()
	{
		colListBK = RGB(240,247,252);
		colName   = RGB(77,77,77);
		colDetail = RGB(125,125,125);
	}
	
	COLORREF	colListBK;		// �б�����ɫ
	COLORREF	colName;		// �����Ƶ���ɫ
	COLORREF	colDetail;		// ��ϸ���Ƶ���ɫ
};

// listͼƬ״̬
enum LMCImgState
{
	LMCI_Small=0,	// СͼƬ
	LMCI_Middle,	// �е�ͼƬ
	LMCI_Big		// ��ͼƬ
};


// ����ÿһ��ṹ
typedef struct _tagLMCItemInfo
{
	string		strGID;				// ��ϷID
	string		strSvrID;			
	int			iIndex;				// ��Ψһ��ʶ
	BOOL		blSel;				// �Ƿ�ѡ��
	CDibBitmap* pImg;				// ��ʾͼƬ
	CString		strItemName;		// �������
	CString		strItemdDetailinfo;	// �����ϸ��Ϣ
	CString		strImgPath;			// ͼƬ·��
	CRect		rc;					// ��ͼƬ�����꣬����ֵ
	int			nGameType;			// ��Ϸ����
	string		strAddTime;			// ���ʱ�� eg: "2013-1-13 12:50"
}LMC_ItemInfo;
