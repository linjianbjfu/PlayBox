#ifndef _I_DATA_H
#define _I_DATA_H


//������Ļ���
class IData
{

protected:
	virtual ~IData(){};

public:

	//��ϵͳ���ã�֪ͨ�����������������������ڴ˺����в�����������Ĳ���
	virtual void	DataAppStart(){};

	//��ϵͳ���ã�֪ͨ����������˳�����������ڴ˺����в�������˳��Ĳ���
	virtual void	DataAppExit(){};

};

#endif