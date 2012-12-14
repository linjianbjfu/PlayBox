#include "StdAfx.h"
#include <tlhelp32.h>
#include "psapi.h"
#pragma comment(lib,"Psapi.lib")

#include "../../include/YL_CPUMem.h"
#include "cpu.h"

#include "SmartLock.h"

HANDLE GetProcessHanlde( const char* lpszProcessName )
{
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap == INVALID_HANDLE_VALUE )
	{
		return NULL;
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof( PROCESSENTRY32 );

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if( !Process32First( hProcessSnap, &pe32 ) )
	{
		//		printError( "Process32First" );  // Show cause of failure
		CloseHandle( hProcessSnap );     // Must clean up the snapshot object!
		return NULL;
	}

	// Now walk the snapshot of processes, and
	// display information about each process in turn
	do
	{

		// Retrieve the priority class.
		dwPriorityClass = 0;
		hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, pe32.th32ProcessID );
		if( hProcess == NULL )
			continue;

		if( stricmp(pe32.szExeFile,lpszProcessName) == 0 )
		{
			CloseHandle( hProcessSnap );
			return hProcess;
		}
		CloseHandle(hProcess);

	} while( Process32Next( hProcessSnap, &pe32 ) );

	CloseHandle( hProcessSnap );
	return NULL;
}


YL_CPUMem::YL_CPUMem()
{
	m_pCPU = new CPU();
}
YL_CPUMem::~YL_CPUMem()
{
	delete m_pCPU;
}


bool YL_CPUMem::GetCPUUsage( const char* pszProcessName,unsigned int& unUsage )
{
	if( pszProcessName == NULL )
	{
		return false;
	}

	HANDLE hHandle = GetProcessHanlde(pszProcessName);
	return GetCPUUsage( (unsigned int)hHandle,unUsage );
}

bool YL_CPUMem::GetCPUUsage( unsigned int hdHandle,unsigned int& unUsage )
{
	HANDLE hHandle = (HANDLE)hdHandle;
	if( hHandle == NULL )
	{
		return false;
	}

	int sys;	
	int iCpu = m_pCPU->GetUsage(hHandle,&sys);	
	CloseHandle(hHandle);
	if( iCpu < 100 && iCpu >= 0  )
	{
		unUsage = iCpu;
	}
	else
	{
		unUsage = 0;
	}
	return true;
}

bool YL_CPUMem::GetMemUsage( const char* lpszProcessName,unsigned int& unUsage )
{
	if( lpszProcessName == NULL )
	{
		return false;
	}
	HANDLE hHandle = GetProcessHanlde(lpszProcessName);
	return GetMemUsage( (unsigned int)hHandle,unUsage );
}
bool YL_CPUMem::GetMemUsage( unsigned int hdProcess,unsigned int& unUsage )
{
	PROCESS_MEMORY_COUNTERS pmc;
	HANDLE hProcess = (HANDLE)hdProcess;

	if ( hProcess != NULL && GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
	{
		CloseHandle(hProcess);
//		unUsage = (DWORD)pmc.WorkingSetSize + (DWORD)pmc.PagefileUsage;
		unUsage = (DWORD)pmc.WorkingSetSize;
		return true;
	}
	if( hProcess != NULL )
	{
		CloseHandle(hProcess);
	}
	return false;
}




CPU::CPU():s_count(0),s_index(0),s_lastCpu(0),s_lastUpTime(0),s_pfnGetSystemTimes(NULL),s_hKernel(NULL)
{
    ::InitializeCriticalSection( &m_lock );
    if( s_hKernel == NULL )
    {   
        s_hKernel = LoadLibrary( "Kernel32.dll" );
        if( s_hKernel != NULL )
        {
            s_pfnGetSystemTimes = (pfnGetSystemTimes)GetProcAddress( s_hKernel, "GetSystemTimes" );
            if( s_pfnGetSystemTimes == NULL )
            {
                FreeLibrary( s_hKernel ); s_hKernel = NULL;
            }
        }
    }

    s_delay.Mark();
}

CPU::~CPU()
{
    if( s_hKernel == NULL )
    {
        FreeLibrary( s_hKernel ); s_hKernel = NULL;
    }

    ::DeleteCriticalSection( &m_lock );
}

int CPU::GetUsage( int* pSystemUsage, TKTime* pUpTime )
{
    TKLong sTime;
    int sLastCpu;
    int sLastCpuProcess;
    TKTime sLastUpTime;

    // lock
    { 
        SmartLock lock( &m_lock );

        sTime           = s_time;
        sLastCpu        = s_lastCpu;
        sLastCpuProcess = s_lastCpuProcess;
        sLastUpTime     = s_lastUpTime;
    }

    if( s_delay.MSec() <= 200 )
    {
        if( pSystemUsage != NULL )
            *pSystemUsage = sLastCpu;

        if( pUpTime != NULL )
            *pUpTime = sLastUpTime;

        return sLastCpuProcess;
    }

    TKLong time;
    
    TKLong idleTime;
    TKLong kernelTime;
    TKLong userTime;
    TKLong kernelTimeProcess;
    TKLong userTimeProcess;

    GetSystemTimeAsFileTime( (LPFILETIME)&time );

    if( sTime == 0 )
    {
        // for the system
        if( s_pfnGetSystemTimes != NULL )
        {
            /*BOOL res = */s_pfnGetSystemTimes( (LPFILETIME)&idleTime, (LPFILETIME)&kernelTime, (LPFILETIME)&userTime );
        }
        else
        {
            idleTime    = 0;
            kernelTime  = 0;
            userTime    = 0;
        }

        // for this process
        {
            FILETIME createTime;
            FILETIME exitTime;
            GetProcessTimes( GetCurrentProcess(), &createTime, &exitTime, 
                             (LPFILETIME)&kernelTimeProcess, 
                             (LPFILETIME)&userTimeProcess );
        }

        // LOCK
        {
            SmartLock lock( &m_lock );

            s_time              = time;
            
            s_idleTime          = idleTime;
            s_kernelTime        = kernelTime;
            s_userTime          = userTime;

            s_kernelTimeProcess = kernelTimeProcess;
            s_userTimeProcess   = userTimeProcess;

            s_lastCpu           = 0;
            s_lastCpuProcess    = 0;

            s_lastUpTime        = kernelTime + userTime;

            sLastCpu        = s_lastCpu;
            sLastCpuProcess = s_lastCpuProcess;
            sLastUpTime     = s_lastUpTime;
        }

        if( pSystemUsage != NULL )
            *pSystemUsage = sLastCpu;

        if( pUpTime != NULL )
            *pUpTime = sLastUpTime;

        s_delay.Mark();
        return sLastCpuProcess;
    }
    /////////////////////////////////////////////////////
    // sTime != 0

    TKLong div = ( time - sTime );
        
    // for the system
    if( s_pfnGetSystemTimes != NULL )
    {
        /*BOOL res = */s_pfnGetSystemTimes( (LPFILETIME)&idleTime, (LPFILETIME)&kernelTime, (LPFILETIME)&userTime );
    }
    else
    {
        idleTime    = 0;
        kernelTime  = 0;
        userTime    = 0;
    }

    // for this process
    {
        FILETIME createTime;
        FILETIME exitTime;
        GetProcessTimes( GetCurrentProcess(), &createTime, &exitTime, 
                         (LPFILETIME)&kernelTimeProcess, 
                         (LPFILETIME)&userTimeProcess );
    }

    int cpu;
    int cpuProcess;
    // LOCK
    {
        SmartLock lock( &m_lock );

        TKLong usr = userTime   - s_userTime;
        TKLong ker = kernelTime - s_kernelTime;
        TKLong idl = idleTime   - s_idleTime;

        TKLong sys = (usr + ker);

        if( sys == 0 )
            cpu = 0;
        else
            cpu = int( (sys - idl) *100 / sys ); // System Idle take 100 % of cpu :-((
        
        cpuProcess = int( ( ( ( userTimeProcess - s_userTimeProcess ) + ( kernelTimeProcess - s_kernelTimeProcess ) ) *100 ) / div );
        
        s_time              = time;

        s_idleTime          = idleTime;
        s_kernelTime        = kernelTime;
        s_userTime          = userTime;

        s_kernelTimeProcess = kernelTimeProcess;
        s_userTimeProcess   = userTimeProcess;
        
        s_cpu[(s_index++) %5] = cpu;
        s_cpuProcess[(s_index++) %5] = cpuProcess;
        s_count ++;
        if( s_count > 5 ) 
            s_count = 5;
        
        int i;
        cpu = 0;
        for( i = 0; i < s_count; i++ )
            cpu += s_cpu[i];
        
        cpuProcess = 0;
        for( i = 0; i < s_count; i++ )
            cpuProcess += s_cpuProcess[i];

        cpu         /= s_count;
        cpuProcess  /= s_count;
        
        s_lastCpu        = cpu;
        s_lastCpuProcess = cpuProcess;

        s_lastUpTime     = kernelTime + userTime;

        sLastCpu        = s_lastCpu;
        sLastCpuProcess = s_lastCpuProcess;
        sLastUpTime     = s_lastUpTime;
    }

    //DBGOUT( _T("CPU:%d  sys:%d div %d"), cpuProcess, cpu, div );
    
    if( pSystemUsage != NULL )
        *pSystemUsage = sLastCpu;

    if( pUpTime != NULL )
        *pUpTime = sLastUpTime;

    s_delay.Mark();
    return sLastCpuProcess;
}

int CPU::GetUsage( HANDLE hProcess,int* pSystemUsage )
{
	TKLong sTime;
	int sLastCpu;
	int sLastCpuProcess;
	TKTime sLastUpTime;

	// lock
	{ 
		SmartLock lock( &m_lock );

		sTime           = s_time;
		sLastCpu        = s_lastCpu;
		sLastCpuProcess = s_lastCpuProcess;
		sLastUpTime     = s_lastUpTime;
	}

	if( s_delay.MSec() <= 200 )
	{
		if( pSystemUsage != NULL )
			*pSystemUsage = sLastCpu;

		return sLastCpuProcess;
	}

	TKLong time;

	TKLong idleTime;
	TKLong kernelTime;
	TKLong userTime;
	TKLong kernelTimeProcess;
	TKLong userTimeProcess;

	GetSystemTimeAsFileTime( (LPFILETIME)&time );

	if( sTime == 0 )
	{
		// for the system
		if( s_pfnGetSystemTimes != NULL )
		{
			/*BOOL res = */s_pfnGetSystemTimes( (LPFILETIME)&idleTime, (LPFILETIME)&kernelTime, (LPFILETIME)&userTime );
		}
		else
		{
			idleTime    = 0;
			kernelTime  = 0;
			userTime    = 0;
		}

		// for this process
		{
			FILETIME createTime;
			FILETIME exitTime;
			GetProcessTimes( hProcess, &createTime, &exitTime, 
				(LPFILETIME)&kernelTimeProcess, 
				(LPFILETIME)&userTimeProcess );
		}

		// LOCK
		{
			SmartLock lock( &m_lock );

			s_time              = time;

			s_idleTime          = idleTime;
			s_kernelTime        = kernelTime;
			s_userTime          = userTime;

			s_kernelTimeProcess = kernelTimeProcess;
			s_userTimeProcess   = userTimeProcess;

			s_lastCpu           = 0;
			s_lastCpuProcess    = 0;

			s_lastUpTime        = kernelTime + userTime;

			sLastCpu        = s_lastCpu;
			sLastCpuProcess = s_lastCpuProcess;
			sLastUpTime     = s_lastUpTime;
		}

		if( pSystemUsage != NULL )
			*pSystemUsage = sLastCpu;

		s_delay.Mark();
		return sLastCpuProcess;
	}
	/////////////////////////////////////////////////////
	// sTime != 0

	TKLong div = ( time - sTime );

	// for the system
	if( s_pfnGetSystemTimes != NULL )
	{
		/*BOOL res = */s_pfnGetSystemTimes( (LPFILETIME)&idleTime, (LPFILETIME)&kernelTime, (LPFILETIME)&userTime );
	}
	else
	{
		idleTime    = 0;
		kernelTime  = 0;
		userTime    = 0;
	}

	// for this process
	{
		FILETIME createTime;
		FILETIME exitTime;
		GetProcessTimes( hProcess, &createTime, &exitTime, 
			(LPFILETIME)&kernelTimeProcess, 
			(LPFILETIME)&userTimeProcess );
	}

	int cpu;
	int cpuProcess;
	// LOCK
	{
		SmartLock lock( &m_lock );

		TKLong usr = userTime   - s_userTime;
		TKLong ker = kernelTime - s_kernelTime;
		TKLong idl = idleTime   - s_idleTime;

		TKLong sys = (usr + ker);

		if( sys == 0 )
			cpu = 0;
		else
			cpu = int( (sys - idl) *100 / sys ); // System Idle take 100 % of cpu :-((

		cpuProcess = int( ( ( ( userTimeProcess - s_userTimeProcess ) + ( kernelTimeProcess - s_kernelTimeProcess ) ) *100 ) / div );

		s_time              = time;

		s_idleTime          = idleTime;
		s_kernelTime        = kernelTime;
		s_userTime          = userTime;

		s_kernelTimeProcess = kernelTimeProcess;
		s_userTimeProcess   = userTimeProcess;

		s_cpu[(s_index++) %5] = cpu;
		s_cpuProcess[(s_index++) %5] = cpuProcess;
		s_count ++;
		if( s_count > 5 ) 
			s_count = 5;

		int i;
		cpu = 0;
		for( i = 0; i < s_count; i++ )
			cpu += s_cpu[i];

		cpuProcess = 0;
		for( i = 0; i < s_count; i++ )
			cpuProcess += s_cpuProcess[i];

		cpu         /= s_count;
		cpuProcess  /= s_count;

		s_lastCpu        = cpu;
		s_lastCpuProcess = cpuProcess;

		s_lastUpTime     = kernelTime + userTime;

		sLastCpu        = s_lastCpu;
		sLastCpuProcess = s_lastCpuProcess;
		sLastUpTime     = s_lastUpTime;
	}

	//DBGOUT( _T("CPU:%d  sys:%d div %d"), cpuProcess, cpu, div );

	if( pSystemUsage != NULL )
		*pSystemUsage = sLastCpu;

	s_delay.Mark();
	return sLastCpuProcess;
}