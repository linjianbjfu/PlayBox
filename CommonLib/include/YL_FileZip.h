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
	//	压缩一些文件															//
	//																			//
	// 参数：																	//
	//		vecSrcFiles	: 需要压缩的文件										//
	//		strDesFile	: 压缩后的文件路径										//
	//																			//
	// 返回值：																	//
	//		true ：	压缩成功													//		
	//		false：	压缩失败													//
	//																			//
	//**************************************************************************//
	static bool Compress ( const vector<string>& vecSrcFiles,const string& strDesFile, string password="" );


	//**************************************************************************//
	//	解压缩一个文件															//
	//																			//
	// 参数：																	//
	//		strSrcFile	:　压缩文件路径											//
	//		strUnZipDir	:　解压文件放置的路径									//
	//		vecDesFiles	:  解压之后的文件名										//
	//		bAllSucc	:　true　表示全部解压缩正确，才拷贝文件到指定目录		//
	//					   false 表示部分解压缩正确，也拷贝文件到指定目录		//
	//																			//
	// 返回值：																	//
	//		true ：	解压缩全部成功，如果解压缩部分成功，则会返回false,			//
	//				但vecDesFiles会保存部分成功的文件名							//
	//		false：	解压缩失败													//
	//																			//
	//**************************************************************************//
	static bool UnCompress( const string& strSrcFile,const string& strUnZipDir,vector<string>& vecDesFiles,bool bAllSucc, string password="");

	static bool UnCompress( void* pContent,unsigned int unContentLen,const string& strUnZipDir,vector<string>& vecDesFiles,bool bAllSucc, string password="");
};

#endif
