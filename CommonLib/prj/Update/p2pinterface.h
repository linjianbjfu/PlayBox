#pragma once

#include "../P2PDll/P2PInt.h"

//p2p������Ϣ������
class P2PDownFile : public P2PObserver
{
public:
	virtual ~P2PDownFile(){
		CloseHandle(m_hEvent);
	};

public:

	P2PDownFile()
	{
		m_hEvent = CreateEvent(NULL, 0, 0, "");
	}

	virtual void P2POb_DownStart(const Sign& sign, const char* lpszFilePath, unsigned int unFileSize)
	{
		m_strFilePath = lpszFilePath;
	};

	//�������
	virtual void P2POb_DownFinish(const Sign& sign)
	{
		SetEvent(m_hEvent);
	};

	HANDLE m_hEvent;
	string m_strFilePath;
};