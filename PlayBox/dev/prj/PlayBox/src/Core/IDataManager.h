#ifndef _I_DATA_MANAGER_H
#define _I_DATA_MANAGER_H
#include "./src/core/AfxGlobals.h"

class IData;

//���ݹ�����
class IDataManager
{
public:
	virtual ~IDataManager(){};
	//��ϵͳ���ã�֪ͨ���ݹ������������
	virtual void	DataManAppStart() = 0;

	//��ϵͳ���ã�֪ͨ���ݹ���������˳�
	virtual void	DataManAppExit() = 0;	

	//������ͨ���˽ӿڻ�����������
	virtual IData*	GetDataObject( const DWORD& guidDataObject ) = 0;

	//���ݶ���ͨ���ú������Լ���ӵ����ݹ��������Ľӿ���
	virtual void	AddDataObject( const DWORD& guidDataObject,IData* pData ) = 0;

	//�����ཫ�Լ������ݹ��������Ľӿ����Ƴ�
	virtual void	RemoveDataObject( const DWORD& guidDataObject ) = 0;
};

#endif