#pragma once

#include "windows.h"
#include "TKTime.h"

typedef BOOL ( __stdcall * pfnGetSystemTimes)( LPFILETIME lpIdleTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime );

class CPU
{
public:
    CPU( void );
    ~CPU( void );
    
    // return :
    // % of cpu usage for this process 
    // % cpu systemUsage 
    // uptime
    int GetUsage( int* pSystemUsage, TKTime* pUpTime );
	int GetUsage( HANDLE hProcess,int* pSystemUsage );
private:
    TKDelay s_delay;

    TKLong s_time;

    TKLong s_idleTime;
    TKLong s_kernelTime;
    TKLong s_userTime;
    int    s_lastCpu;
    int    s_cpu[5];

    TKLong s_kernelTimeProcess;
    TKLong s_userTimeProcess;
    int    s_lastCpuProcess;
    int    s_cpuProcess[5];

    int    s_count;
    int    s_index;

    TKLong s_lastUpTime;

    HINSTANCE s_hKernel;
    pfnGetSystemTimes s_pfnGetSystemTimes;

    CRITICAL_SECTION m_lock;
};