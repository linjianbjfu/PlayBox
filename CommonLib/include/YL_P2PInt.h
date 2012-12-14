#ifndef _P2P_INT_H
#define _P2P_INT_H

#pragma once

#include <string>

#include "YL_P2PDef.h"

//p2p������Ϣ������
class P2PObserver
{
public:
	virtual ~P2PObserver(){};

public:

	//���ؿ�ʼ
	virtual void P2POb_DownStart(const string& strID, const char* lpszFilePath, unsigned int unFileSize){};

	//�������
	virtual void P2POb_DownFinish(const string& strID){};

	//����ʧ��
	virtual void P2POb_DownFailed(const string& strID, P2P_DOWN_FAILED_REASON pdfr){};

	//���ؽ���
	virtual void P2POb_DownProgress(const string& strID, unsigned int unContinue, unsigned int unFinished, unsigned int unSpeed){};

	//������ͣ
	virtual void P2POb_DownPause(const string& strID){};

	//���ر��
	virtual void P2POb_SigChange(const string& strID, const string& strIDNew, const char* szRid){};
};

#endif