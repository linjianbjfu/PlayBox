#include "stdafx.h"
#include "../Log/KwLogSvr.h"
#include "../../common/YL_RegInfo.h"
#include "../../common/YL_EncFileReg.h"
#include "../../common/YL_FileInfo.h"
#include "../../common/YL_DirInfo.h"
#include "../../common/YL_Ini.h"
#include "../../common/tools.h"
#include "../../common/MacroStr.h"
#include "../../common/LhcImg.h"
#include "CheckCmdLimit.h"
#include "Update.h"
#include <time.h>

bool CheckTimeStamp(const char* szRegValName, unsigned int nInterval)
{
	unsigned int dwTime = 0, dwCurrentTime = 0;
	time_t timer;
	time(&timer);
	dwCurrentTime = (DWORD)timer;
	//�ϴ�ִ��ʱ��
	if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, szRegValName, dwTime))
	{
		if(!YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, szRegValName, dwCurrentTime))
		{
			return false;
		}
		return true;
	}
	//�Ƚ�ʱ����
	if(dwCurrentTime - dwTime < nInterval*60)
	{
		if(dwCurrentTime < dwTime)
		{
			YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, szRegValName, dwCurrentTime);
		}

		return false;
	}
	else
	{
		if(!YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, szRegValName, dwCurrentTime))
		{
			return false;
		}
		return true;
	}
}

//ÿ�첻�ܳ���nTimesLimit��
int CheckDailyLimit(const char* szLastDateKey, const char* szTimesKey, unsigned int nTimesLimit)
{
	//��������
	__time64_t time;
	_time64(&time);
	unsigned int dwDate = (unsigned int)(time/(60 * 60 * 24)), dwTimes = 0, lastdate = 0;
	if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, szLastDateKey, lastdate))
	{
		lastdate = 0;
	}
	if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, szTimesKey, dwTimes))
	{
		dwTimes = 0;
	}
	//�ϴμ�¼���ǽ���,���¿�ʼ����
	if(dwDate != lastdate)
	{
		YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, szLastDateKey, dwDate);
		dwTimes = 0;
		YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, szTimesKey, dwTimes);
	}

	if(dwTimes < nTimesLimit)
	{
		if( !YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, szTimesKey, ++dwTimes) )
		{
			return CMD_CHECK_OK; // Ŀǰ��������, �Ժ�����޸�
		}
		return CMD_CHECK_OK;
	}
	else
	{
		YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "CheckDailyLimit", "Times error, times is %u, limit is %u.", dwTimes, nTimesLimit);
		return CMD_CHECK_FALSE;
	}
}

//�����ʹ��������ܳ�������
int CheckTimesLimit(unsigned int nIndex)
{
	//�Ѿ�ִ�е������ʹ���
	unsigned int  dwDays = 0, dwTimes = 0;
	if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_EXEC_DAYS_KEY, dwDays))
	{
		dwDays = 0;
	}

	if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_EXEC_TIMES_KEY, dwTimes))
	{
		dwTimes = 0;
	}

	//��������
	__time64_t time;
	unsigned int lastdate = 0, dwDate = 0;
	_time64(&time);
	dwDate = (DWORD)(time/(60 * 60 * 24));
	if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_LAST_DATE_KEY, lastdate))
	{
		lastdate = 0;
	}

	//������¼���ǽ���,���¿�ʼ����
	if(dwDate != lastdate)
	{
		YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_LAST_DATE_KEY, dwDate);
		dwTimes = 0;
		YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_EXEC_TIMES_KEY, dwTimes);
		dwDays++;
		YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_EXEC_DAYS_KEY, dwDays);
	}

	//������������,����FAIL
	if(dwDays > g_CmdsLimit[nIndex].nDaysLimit)
	{
		return CMD_CHECK_FAIL;
	}

	dwTimes++;
	//���������������, ����FALSE
	if(dwTimes > g_CmdsLimit[nIndex].nTimesLimit)
	{
		return CMD_CHECK_FALSE;
	}else
	{
		if( !YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_EXEC_TIMES_KEY, dwTimes))
		{
			return CMD_CHECK_FALSE;
		}
		return CMD_CHECK_OK;
	}
}

int CheckCmdLimit(unsigned int nIndex)
{
	if(nIndex == 1)
	{
		//���������ʱ�������		
		//if(!CheckTimeStamp(STR_CMD_LAST_TIME_KEY, g_CmdsLimit[nIndex-1].nInterval))
		return CheckDailyLimit(STR_CHECK_UPDATE_LAST_DATE_KEY, STR_CHECK_UPDATE_TIMES_KEY, g_CmdsLimit[nIndex-1].nTimesLimit);
	}

	unsigned int dwCmd;
	YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CURRENT_CMD, dwCmd);
	if(nIndex != dwCmd)
	{
		return CMD_CHECK_FAIL;
	}

	//�������ÿ���������������
	return CheckTimesLimit(nIndex-1);
}
