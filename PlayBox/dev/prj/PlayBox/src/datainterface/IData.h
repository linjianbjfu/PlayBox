#ifndef _I_DATA_H
#define _I_DATA_H


//数据类的基类
class IData
{

protected:
	virtual ~IData(){};

public:

	//由系统调用，通知数据类程序启动，数据类可在此函数中插入程序启动的操作
	virtual void	DataAppStart(){};

	//由系统调用，通知数据类程序退出，数据类可在此函数中插入程序退出的操作
	virtual void	DataAppExit(){};

};

#endif