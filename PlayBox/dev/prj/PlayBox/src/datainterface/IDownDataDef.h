#pragma once
#include "../Global/GlobalVariable.h"
#include <string>
#include <vector>
using namespace std;

//��������
typedef vector<string>		RidVector;

//����֪ͨ����״̬��
enum DownloadNotifyCode
{						//״̬��˵��						��;
	DNC_ERROR = 0,		//P2P����						֪ͨ��/״̬��
	DNC_DOWNLOAD,		//���ύ����						֪ͨ��
	DNC_PAUSE,			//��ͣ����						֪ͨ��/״̬��
	DNC_WAITING,		//��������������δ�ύP2P		֪ͨ��/״̬��
	DNC_RESTART,		//���¿�ʼ����						֪ͨ��				
	DNC_START,			//���ύP2P						֪ͨ��/״̬��
	DNC_DOWNING,		//������							״̬��
	DNC_DELETE,			//ɾ������							֪ͨ��
	DNC_DECOMPRESS,		// ��ѹ
	DNC_FINISHED,		//�������							֪ͨ��
	DNC_ADD,
	DNC_EXISTED,
	DNC_DATACHANGE
};

//ÿ�������ļ�������״̬
struct DownState
{
	DownloadNotifyCode	dnc;//��ǰ״̬
};
