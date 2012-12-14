#ifndef _I_AD_DATA_H
#define _I_AD_DATA_H
#include "IData.h"
#include <string>
using std::string;

struct AD_STRUCT
{
	string strLinkUrl;
	string strPicPath;
};
//数据类的基类
class IAdData: public IData
{

protected:
	virtual ~IAdData(){};

public:

	//由系统调用，通知数据类程序启动，数据类可在此函数中插入程序启动的操作
	//virtual void	DataAppStart(){};

	//由系统调用，通知数据类程序退出，数据类可在此函数中插入程序退出的操作
	//virtual void	DataAppExit(){};
	virtual bool GetLoadingPicAd( AD_STRUCT& ad ) = 0;
};

#endif