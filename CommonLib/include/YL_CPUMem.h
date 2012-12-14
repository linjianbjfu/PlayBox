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
	//	���CPUʹ����,��������ö�����̵ģãУ�ʹ���ʣ���ֱ�ʹ�ö��		//
	//	YL_CPUMem����.															//
	//	�ú���ǰ���λ�õģãУ��ʲ�׼											//
	// ������																	//
	//		hHandle			:	���̾��										//
	//		pszProcessName	:	��ִ�г����ļ���								//
	//							��ͬһ���ļ��ж������ʱ����Ҫ���ݽ��̾��		//
	//		unUsage			:	cpuʹ����										//
	//																			//
	// ����ֵ��																	//
	//		true	:	��óɹ�												//		
	//		false	:	���ʧ�ܣ��п��ܽ��̲�����								//
	//																			//
	//**************************************************************************//
	bool GetCPUUsage( unsigned int hHandle,unsigned int& unUsage );
	bool GetCPUUsage( const char* pszProcessName,unsigned int& unUsage );

	//**************************************************************************//
	//																			//
	//	����ڴ�ʹ����															//
	// ������																	//
	//		hHandle			:	���̾��										//
	//		pszProcessName	:	��ִ�г����ļ���								//
	//							��ͬһ���ļ��ж������ʱ����Ҫ���ݽ��̾��		//
	//		unUsage			:	�ڴ�ʹ��������λΪ�ֽ�							//
	//																			//
	// ����ֵ��																	//
	//		true	:	��óɹ�												//		
	//		false	:	���ʧ�ܣ��п��ܽ��̲�����								//
	//																			//
	//**************************************************************************//
	bool GetMemUsage( unsigned int hHandle,unsigned int& unUsage );
	bool GetMemUsage( const char* lpszProcessName,unsigned int& unUsage );

private:
	CPU* m_pCPU;

};

#endif;