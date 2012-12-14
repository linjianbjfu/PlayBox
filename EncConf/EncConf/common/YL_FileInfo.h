#ifndef _YL_FILE_INFO_H
#define _YL_FILE_INFO_H

#include <string>
using std::string;

class YL_FileInfo
{
public:

	//******************************************************************************//
	//																				//
	//	判断文件是否有效，一般指是否存在本地										//
	//																				//
	//  参数:																		//
	//																				//
	//	pszFilePath	:	文件路径													//
	//	strFilePath	:	文件路径													//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	文件有效													//
	//		false	:	文件无效													//
	//																				//
	//******************************************************************************//
	static bool IsValid( const char* pszFilePath );
	static bool IsValid( const string& strFilePath );

	//******************************************************************************//
	//																				//
	//	判断文件是否为目录															//
	//																				//
	//  参数:																		//
	//																				//
	//	pszFilePath	:	文件路径													//
	//	strFilePath	:	文件路径													//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	文件路径为目录												//
	//		false	:	文件路径不为目录											//
	//																				//
	//******************************************************************************//
	static bool IsDirectory( const char* pszFilePath );
	static bool IsDirectory( const string& strFilePath );

	//******************************************************************************//
	//																				//
	//	判断文件属性是否含有系统属性												//
	//																				//
	//  参数:																		//
	//																				//
	//	pszFilePath	:	文件路径													//
	//	strFilePath	:	文件路径													//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	文件含有系统属性											//
	//		false	:	文件不含有系统属性											//
	//																				//
	//******************************************************************************//
	static bool IsAttrSystem( const char* pszFilePath );
	static bool IsAttrSystem( const string& strFilePath );

	//******************************************************************************//
	//																				//
	//	判断文件属性是否含有只读属性												//
	//																				//
	//  参数:																		//
	//																				//
	//	pszFilePath	:	文件路径													//
	//	strFilePath	:	文件路径													//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	文件含有只读属性											//
	//		false	:	文件不含有只读属性											//
	//																				//
	//******************************************************************************//
	static bool IsAttrReadOnly( const char* pszFilePath );
	static bool IsAttrReadOnly( const string& strFilePath );

	//******************************************************************************//
	//																				//
	//	判断文件属性是否含有隐藏属性												//
	//																				//
	//  参数:																		//
	//																				//
	//	pszFilePath	:	文件路径													//
	//	strFilePath	:	文件路径													//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	文件含有隐藏属性											//
	//		false	:	文件不含有隐藏属性											//
	//																				//
	//******************************************************************************//
	static bool IsAttrHidden( const char* pszFilePath );
	static bool IsAttrHidden( const string& strFilePath );

	//******************************************************************************//
	//																				//
	//	得到文件大小																//
	//																				//
	//  参数:																		//
	//																				//
	//	pszFilePath	:	文件路径													//
	//	strFilePath	:	文件路径													//
	//	unLowSize	:	文件大小的低32位											//
	//	unHighSize	:	文件大小的高32位											//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	已得到文件大小												//
	//		false	:	未得到文件大小												//
	//																				//
	//******************************************************************************//
	static bool GetFileSize( const char* pszFilePath,unsigned int* pUnLowSize,unsigned int* pUnHighSize = 0);
	static bool GetFileSize( const string& strFilePath,unsigned int* pUnLowSize,unsigned int* pUnHighSize = 0);

	//******************************************************************************//
	//																				//
	//	得到文件路径中的文件名														//
	//																				//
	//  参数:																		//
	//																				//
	//	pszFilePath	:	文件路径													//
	//	strFilePath	:	文件路径													//
	//  szBufferName:	文件名的返回Buffer											//
	//	unBufferLen	:	Buffer长度													//
	//	strFileName :	返回的文件名												//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	文件名获取成功												//
	//		false	:	文件名获取失败												//
	//																				//
	//******************************************************************************//
	static bool GetFileName( const char* pszFilePath,char szBufferName[],unsigned int unBufferLen );
	static bool GetFileName( const string& strFilePath,string& strFileName );

	//******************************************************************************//
	//																				//
	//	得到文件路径中的文件名的前缀												//
	//	即 传入c:\windows\system32\ie.exe,返回ie									//
	//																				//
	//  参数:																		//
	//																				//
	//	pszFilePath	:	文件路径													//
	//	strFilePath	:	文件路径													//
	//  szBufferName:	文件名的返回Buffer											//
	//	unBufferLen	:	Buffer长度													//
	//	strFileName :	返回的文件名												//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	文件名前缀获取成功											//
	//		false	:	文件名前缀获取失败											//
	//																				//
	//******************************************************************************//
	static bool GetFileNamePrefix( const char* pszFilePath,char szBufferName[],unsigned int unBufferLen );
	static bool GetFileNamePrefix( const string& strFilePath,string& strFileName );

	static bool GetFileNameSuffix( const char* pszFilePath,char szBufferName[],unsigned int unBufferLen );
	static bool GetFileNameSuffix( const string& strFilePath,string& strFileName );

	//******************************************************************************//
	//																				//
	//	得到文件路径中的文件所在目录												//
	//																				//
	//  参数:																		//
	//																				//
	//	pszFilePath	:	文件路径													//
	//	strFilePath	:	文件路径													//
	//  szBufferDirPath :	文件目录的返回Buffer									//
	//	unBufferLen	:	Buffer长度													//
	//	strFileName :	返回的文件所在目录											//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	文件所在目录获取成功										//
	//		false	:	文件所在目录获取失败										//
	//																				//
	//******************************************************************************//
	static bool GetFileDirPath( const char* pszFilePath,char szBufferDirPath[],unsigned int unBufferLen );
	static bool GetFileDirPath( const string& strFilePath,string& strFileDirPath );

		
	static string	GetFileContent( const string& strFileName );
	static bool SetFileContent( const string& strFileName, const string& strContent );
};

#endif