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
//������Ļ���
class IAdData: public IData
{

protected:
	virtual ~IAdData(){};

public:

	//��ϵͳ���ã�֪ͨ�����������������������ڴ˺����в�����������Ĳ���
	//virtual void	DataAppStart(){};

	//��ϵͳ���ã�֪ͨ����������˳�����������ڴ˺����в�������˳��Ĳ���
	//virtual void	DataAppExit(){};
	virtual bool GetLoadingPicAd( AD_STRUCT& ad ) = 0;
};

#endif