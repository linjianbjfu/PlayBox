#ifndef _YL_FILE_INFO_H
#define _YL_FILE_INFO_H

#include <string>
using std::string;

class YL_FileInfo
{
public:

	//******************************************************************************//
	//																				//
	//	�ж��ļ��Ƿ���Ч��һ��ָ�Ƿ���ڱ���										//
	//																				//
	//  ����:																		//
	//																				//
	//	pszFilePath	:	�ļ�·��													//
	//	strFilePath	:	�ļ�·��													//
	//																				//
	//	����ֵ��																	//
	//																				//
	//		true	:	�ļ���Ч													//
	//		false	:	�ļ���Ч													//
	//																				//
	//******************************************************************************//
	static bool IsValid( const char* pszFilePath );
	static bool IsValid( const string& strFilePath );

	//******************************************************************************//
	//																				//
	//	�ж��ļ��Ƿ�ΪĿ¼															//
	//																				//
	//  ����:																		//
	//																				//
	//	pszFilePath	:	�ļ�·��													//
	//	strFilePath	:	�ļ�·��													//
	//																				//
	//	����ֵ��																	//
	//																				//
	//		true	:	�ļ�·��ΪĿ¼												//
	//		false	:	�ļ�·����ΪĿ¼											//
	//																				//
	//******************************************************************************//
	static bool IsDirectory( const char* pszFilePath );
	static bool IsDirectory( const string& strFilePath );

	//******************************************************************************//
	//																				//
	//	�ж��ļ������Ƿ���ϵͳ����												//
	//																				//
	//  ����:																		//
	//																				//
	//	pszFilePath	:	�ļ�·��													//
	//	strFilePath	:	�ļ�·��													//
	//																				//
	//	����ֵ��																	//
	//																				//
	//		true	:	�ļ�����ϵͳ����											//
	//		false	:	�ļ�������ϵͳ����											//
	//																				//
	//******************************************************************************//
	static bool IsAttrSystem( const char* pszFilePath );
	static bool IsAttrSystem( const string& strFilePath );

	//******************************************************************************//
	//																				//
	//	�ж��ļ������Ƿ���ֻ������												//
	//																				//
	//  ����:																		//
	//																				//
	//	pszFilePath	:	�ļ�·��													//
	//	strFilePath	:	�ļ�·��													//
	//																				//
	//	����ֵ��																	//
	//																				//
	//		true	:	�ļ�����ֻ������											//
	//		false	:	�ļ�������ֻ������											//
	//																				//
	//******************************************************************************//
	static bool IsAttrReadOnly( const char* pszFilePath );
	static bool IsAttrReadOnly( const string& strFilePath );

	//******************************************************************************//
	//																				//
	//	�ж��ļ������Ƿ�����������												//
	//																				//
	//  ����:																		//
	//																				//
	//	pszFilePath	:	�ļ�·��													//
	//	strFilePath	:	�ļ�·��													//
	//																				//
	//	����ֵ��																	//
	//																				//
	//		true	:	�ļ�������������											//
	//		false	:	�ļ���������������											//
	//																				//
	//******************************************************************************//
	static bool IsAttrHidden( const char* pszFilePath );
	static bool IsAttrHidden( const string& strFilePath );

	//******************************************************************************//
	//																				//
	//	�õ��ļ���С																//
	//																				//
	//  ����:																		//
	//																				//
	//	pszFilePath	:	�ļ�·��													//
	//	strFilePath	:	�ļ�·��													//
	//	unLowSize	:	�ļ���С�ĵ�32λ											//
	//	unHighSize	:	�ļ���С�ĸ�32λ											//
	//																				//
	//	����ֵ��																	//
	//																				//
	//		true	:	�ѵõ��ļ���С												//
	//		false	:	δ�õ��ļ���С												//
	//																				//
	//******************************************************************************//
	static bool GetFileSize( const char* pszFilePath,unsigned int* pUnLowSize,unsigned int* pUnHighSize = 0);
	static bool GetFileSize( const string& strFilePath,unsigned int* pUnLowSize,unsigned int* pUnHighSize = 0);

	//******************************************************************************//
	//																				//
	//	�õ��ļ�·���е��ļ���														//
	//																				//
	//  ����:																		//
	//																				//
	//	pszFilePath	:	�ļ�·��													//
	//	strFilePath	:	�ļ�·��													//
	//  szBufferName:	�ļ����ķ���Buffer											//
	//	unBufferLen	:	Buffer����													//
	//	strFileName :	���ص��ļ���												//
	//																				//
	//	����ֵ��																	//
	//																				//
	//		true	:	�ļ�����ȡ�ɹ�												//
	//		false	:	�ļ�����ȡʧ��												//
	//																				//
	//******************************************************************************//
	static bool GetFileName( const char* pszFilePath,char szBufferName[],unsigned int unBufferLen );
	static bool GetFileName( const string& strFilePath,string& strFileName );

	//******************************************************************************//
	//																				//
	//	�õ��ļ�·���е��ļ�����ǰ׺												//
	//	�� ����c:\windows\system32\ie.exe,����ie									//
	//																				//
	//  ����:																		//
	//																				//
	//	pszFilePath	:	�ļ�·��													//
	//	strFilePath	:	�ļ�·��													//
	//  szBufferName:	�ļ����ķ���Buffer											//
	//	unBufferLen	:	Buffer����													//
	//	strFileName :	���ص��ļ���												//
	//																				//
	//	����ֵ��																	//
	//																				//
	//		true	:	�ļ���ǰ׺��ȡ�ɹ�											//
	//		false	:	�ļ���ǰ׺��ȡʧ��											//
	//																				//
	//******************************************************************************//
	static bool GetFileNamePrefix( const char* pszFilePath,char szBufferName[],unsigned int unBufferLen );
	static bool GetFileNamePrefix( const string& strFilePath,string& strFileName );

	static bool GetFileNameSuffix( const char* pszFilePath,char szBufferName[],unsigned int unBufferLen );
	static bool GetFileNameSuffix( const string& strFilePath,string& strFileName );

	//******************************************************************************//
	//																				//
	//	�õ��ļ�·���е��ļ�����Ŀ¼												//
	//																				//
	//  ����:																		//
	//																				//
	//	pszFilePath	:	�ļ�·��													//
	//	strFilePath	:	�ļ�·��													//
	//  szBufferDirPath :	�ļ�Ŀ¼�ķ���Buffer									//
	//	unBufferLen	:	Buffer����													//
	//	strFileName :	���ص��ļ�����Ŀ¼											//
	//																				//
	//	����ֵ��																	//
	//																				//
	//		true	:	�ļ�����Ŀ¼��ȡ�ɹ�										//
	//		false	:	�ļ�����Ŀ¼��ȡʧ��										//
	//																				//
	//******************************************************************************//
	static bool GetFileDirPath( const char* pszFilePath,char szBufferDirPath[],unsigned int unBufferLen );
	static bool GetFileDirPath( const string& strFilePath,string& strFileDirPath );

		
	static string	GetFileContent( const string& strFileName );
	static bool SetFileContent( const string& strFileName, const string& strContent );
};

#endif