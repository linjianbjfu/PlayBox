#ifndef _I_MESSAGE_OBSERVER_H
#define _I_MESSAGE_OBSERVER_H

enum MESSAGE_OBSERVER_PRIORITY
{
	TOP_PRIORITY = 0,
	HIGH_PRIORITY,
	NORMAL_PRIORITY,
	BOTTOM_PRIORITY
};

//��Ϣ�����߽ӿ�
class IMessageObserver
{
public:
	virtual ~IMessageObserver(){};
};


#endif