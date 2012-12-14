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
	//上次执行时间
	if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, szRegValName, dwTime))
	{
		if(!YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, szRegValName, dwCurrentTime))
		{
			return false;
		}
		return true;
	}
	//比较时间间隔
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

//每天不能超过nTimesLimit次
int CheckDailyLimit(const char* szLastDateKey, const char* szTimesKey, unsigned int nTimesLimit)
{
	//更新天数
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
	//上次记录不是今天,重新开始计数
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
			return CMD_CHECK_OK; // 目前不做限制, 以后可能修改
		}
		return CMD_CHECK_OK;
	}
	else
	{
		YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "CheckDailyLimit", "Times error, times is %u, limit is %u.", dwTimes, nTimesLimit);
		return CMD_CHECK_FALSE;
	}
}

//天数和次数都不能超过限制
int CheckTimesLimit(unsigned int nIndex)
{
	//已经执行的天数和次数
	unsigned int  dwDays = 0, dwTimes = 0;
	if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_EXEC_DAYS_KEY, dwDays))
	{
		dwDays = 0;
	}

	if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_EXEC_TIMES_KEY, dwTimes))
	{
		dwTimes = 0;
	}

	//更新天数
	__time64_t time;
	unsigned int lastdate = 0, dwDate = 0;
	_time64(&time);
	dwDate = (DWORD)(time/(60 * 60 * 24));
	if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_LAST_DATE_KEY, lastdate))
	{
		lastdate = 0;
	}

	//升级记录不是今天,重新开始计数
	if(dwDate != lastdate)
	{
		YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_LAST_DATE_KEY, dwDate);
		dwTimes = 0;
		YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_EXEC_TIMES_KEY, dwTimes);
		dwDays++;
		YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CMD_EXEC_DAYS_KEY, dwDays);
	}

	//天数超过限制,返回FAIL
	if(dwDays > g_CmdsLimit[nIndex].nDaysLimit)
	{
		return CMD_CHECK_FAIL;
	}

	dwTimes++;
	//当天次数超过限制, 返回FALSE
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
		//检测升级按时间戳控制		
		//if(!CheckTimeStamp(STR_CMD_LAST_TIME_KEY, g_CmdsLimit[nIndex-1].nInterval))
		return CheckDailyLimit(STR_CHECK_UPDATE_LAST_DATE_KEY, STR_CHECK_UPDATE_TIMES_KEY, g_CmdsLimit[nIndex-1].nTimesLimit);
	}

	unsigned int dwCmd;
	YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_CURRENT_CMD, dwCmd);
	if(nIndex != dwCmd)
	{
		return CMD_CHECK_FAIL;
	}

	//其他命令按每天次数和天数控制
	return CheckTimesLimit(nIndex-1);
}
