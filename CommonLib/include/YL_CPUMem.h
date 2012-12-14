#ifndef _YL_CPU_MEM_H
#define _YL_CPU_MEM_H

#ifdef	YL_CPUMEM_EXPORT
#define YL_CPUMEM_DLL __declspec(dllexport)
#else
#define YL_CPUMEM_DLL __declspec(dllimport)
#endif

class CPU;

class YL_CPUMEM_DLL YL_CPUMem
{
public:
	YL_CPUMem();
	~YL_CPUMem();

	//**************************************************************************//
	//																			//
	//	获得CPU使用率,如果是想获得多个进程的ＣＰＵ使用率，请分别使用多个		//
	//	YL_CPUMem对象.															//
	//	该函数前几次获得的ＣＰＵ率不准											//
	// 参数：																	//
	//		hHandle			:	进程句柄										//
	//		pszProcessName	:	可执行程序文件名								//
	//							当同一个文件有多个进程时，需要传递进程句柄		//
	//		unUsage			:	cpu使用率										//
	//																			//
	// 返回值：																	//
	//		true	:	获得成功												//		
	//		false	:	获得失败，有可能进程不存在								//
	//																			//
	//**************************************************************************//
	bool GetCPUUsage( unsigned int hHandle,unsigned int& unUsage );
	bool GetCPUUsage( const char* pszProcessName,unsigned int& unUsage );

	//**************************************************************************//
	//																			//
	//	获得内存使用数															//
	// 参数：																	//
	//		hHandle			:	进程句柄										//
	//		pszProcessName	:	可执行程序文件名								//
	//							当同一个文件有多个进程时，需要传递进程句柄		//
	//		unUsage			:	内存使用数，单位为字节							//
	//																			//
	// 返回值：																	//
	//		true	:	获得成功												//		
	//		false	:	获得失败，有可能进程不存在								//
	//																			//
	//**************************************************************************//
	bool GetMemUsage( unsigned int hHandle,unsigned int& unUsage );
	bool GetMemUsage( const char* lpszProcessName,unsigned int& unUsage );

private:
	CPU* m_pCPU;

};

#endif;