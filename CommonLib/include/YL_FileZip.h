#ifndef _YL_FILE_ZIP_H
#define _YL_FILE_ZIP_H


#ifdef	YL_FILEZIP_EXPORT
#define YL_FILEZIP_DLL __declspec(dllexport)
#else
#define YL_FILEZIP_DLL __declspec(dllimport)
#endif

#include <string>
#include <vector>

using std::string;
using std::vector;

class YL_FILEZIP_DLL YL_CFileZip
{
public:

	//**************************************************************************//
	//	ѹ��һЩ�ļ�															//
	//																			//
	// ������																	//
	//		vecSrcFiles	: ��Ҫѹ�����ļ�										//
	//		strDesFile	: ѹ������ļ�·��										//
	//																			//
	// ����ֵ��																	//
	//		true ��	ѹ���ɹ�													//		
	//		false��	ѹ��ʧ��													//
	//																			//
	//**************************************************************************//
	static bool Compress ( const vector<string>& vecSrcFiles,const string& strDesFile, string password="" );


	//**************************************************************************//
	//	��ѹ��һ���ļ�															//
	//																			//
	// ������																	//
	//		strSrcFile	:��ѹ���ļ�·��											//
	//		strUnZipDir	:����ѹ�ļ����õ�·��									//
	//		vecDesFiles	:  ��ѹ֮����ļ���										//
	//		bAllSucc	:��true����ʾȫ����ѹ����ȷ���ſ����ļ���ָ��Ŀ¼		//
	//					   false ��ʾ���ֽ�ѹ����ȷ��Ҳ�����ļ���ָ��Ŀ¼		//
	//																			//
	// ����ֵ��																	//
	//		true ��	��ѹ��ȫ���ɹ��������ѹ�����ֳɹ�����᷵��false,			//
	//				��vecDesFiles�ᱣ�沿�ֳɹ����ļ���							//
	//		false��	��ѹ��ʧ��													//
	//																			//
	//**************************************************************************//
	static bool UnCompress( const string& strSrcFile,const string& strUnZipDir,vector<string>& vecDesFiles,bool bAllSucc, string password="");

	static bool UnCompress( void* pContent,unsigned int unContentLen,const string& strUnZipDir,vector<string>& vecDesFiles,bool bAllSucc, string password="");
};

#endif
