/* 
�ú����������Ƿ�����ʾ��������֮����ɸ��·������б������������ȹ���
*/

#include "windows.h"
#include <string>
using namespace std;

void checkUpdate(const char* player);//���·������б�������������
void checkPopUpUpdate(HWND *phWnd = NULL);	//����Ƿ�����ʾ������
//�ϱߵ�����������ͷ����������������������һ����������˳���
//�±ߵĺ����Ǳ��ϱ������������õ�
void realCheckLastWrongExit(LPVOID pparam); //��鲥�����ϴ��Ƿ������˳�
void runPopUpUpdate(LPVOID pparam);//����Ƿ�����ʾ���������������а�װ�������˳�������
void realUpdate(LPVOID pparam);	//����������·������б������������ȹ���
void stopCheckUpdate();