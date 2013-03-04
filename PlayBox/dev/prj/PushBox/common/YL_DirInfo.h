#ifndef _YL_DIR_INFO_H
#define _YL_DIR_INFO_H

#include <string>
#include <vector>

using std::string;
using std::vector;

class YL_DirInfo
{
public:

	//******************************************************************************//
	//																				//
	//	获得Windows安装路径															//
	//	路径最后不带斜杠 \															//
	//																				//
	//  参数:																		//
	//		szDirBuffer	:	路径Buffer												//
	//		strDir		:	返回的路径												//
	//		unBuffLen	:	Buffer长度												//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	操作成功													//
	//		false	:	操作失败													//
	//																				//
	//******************************************************************************//
	static bool GetWindowsDir( char szDirBuffer[],unsigned int unBuffLen );
	static bool GetWindowsDir( string& strDir );
	
	//******************************************************************************//
	//																				//
	//	获得System32路径															//
	//	路径最后不带斜杠 \															//
	//																				//
	//  参数:																		//
	//		szDirBuffer	:	路径Buffer												//
	//		strDir		:	返回的路径												//
	//		unBuffLen	:	Buffer长度												//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	操作成功													//
	//		false	:	操作失败													//
	//																				//
	//******************************************************************************//
	static bool GetSystemDir( char szDirBuffer[],unsigned int unBuffLen );
	static bool GetSystemDir( string& strDir );

	//******************************************************************************//
	//																				//
	//	获得临时文件夹安装路径														//
	//	路径最后不带斜杠 \															//
	//																				//
	//  参数:																		//
	//		szDirBuffer	:	路径Buffer												//
	//		strDir		:	返回的路径												//
	//		unBuffLen	:	Buffer长度												//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	操作成功													//
	//		false	:	操作失败													//
	//																				//
	//******************************************************************************//
	static bool GetTempDir( char szDirBuffer[],unsigned int unBuffLen );
	static bool GetTempDir( string& strDir );

	//******************************************************************************//
	//																				//
	//	获得Program Files路径														//
	//	路径最后不带斜杠 \															//
	//																				//
	//  参数:																		//
	//		szDirBuffer	:	路径Buffer												//
	//		strDir		:	返回的路径												//
	//		unBuffLen	:	Buffer长度												//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	操作成功													//
	//		false	:	操作失败													//
	//																				//
	//******************************************************************************//
	static bool GetProgramFilesDir( char szDirBuffer[],unsigned int unBuffLen );
	static bool GetProgramFilesDir( string& strDir );

	//******************************************************************************//
	//																				//
	//	创建一个目录																//
	//	例如创建 c:\windows\system32，会首先检查c:\windows是否存在，如果不存在，	//
	//	会先创建 c:\windows，接着再创建 c:\windows\system32							//
	//	目录路径支持以斜杠结尾或不以斜杠结尾，即c:\a 和 c:\a\均可					//
	//																				//
	//  参数:																		//
	//		pszDirName	:	目录名称												//
	//		strDirName	:	目录名称												//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	操作成功													//
	//		false	:	操作失败													//
	//																				//
	//******************************************************************************//
	static bool MakeDir( const char* pszDirName );
	static bool MakeDir( const string& strDirName );

	//******************************************************************************//
	//																				//
	//	删除一个目录																//
	//	会删除该目录下的所有文件及目录												//
	//	如果目录或者目录内的某个文件被占用，则会删除失败，函数马上返回，但已删除	//
	//	的文件不会回滚。															//
	//																				//
	//  参数:																		//
	//		pszDirName	:	目录名称												//
	//		strDirName	:	目录名称												//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	操作成功													//
	//		false	:	操作失败													//
	//																				//
	//******************************************************************************//
	static bool DeleteDir( const char* pszDirName);
	static bool DeleteDir( const string& strDirName);

	//******************************************************************************//
	//																				//
	//	删除一个目录下的所有文件													//
	//	不会删除子目录下的文件，可以指定删除文件的后缀								//
	//	如果目录下的文件被占用，则删除会失败，函数马上返回。但已删除的文件不会		//
	//	回滚。																		//
	//																				//
	//  参数:																		//
	//		pszDirName	:	目录名称												//
	//		strDirName	:	目录名称												//
	//		pszFileExe	:	需要删除的文件后缀，如果为空，则默认全部删除			//
	//		strExe		:	需要删除的文件后缀，如果为空，则默认全部删除			//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	操作成功													//
	//		false	:	操作失败													//
	//																				//
	//******************************************************************************//
	static bool ClearDir( const char* pszDirName,const char* pszFileExe = 0 );
	static bool ClearDir( const string& strDirName ,const string& strExe = "");
	

	//******************************************************************************//
	//																				//
	//	查找目录下的所有文件和文件夹												//
	//	目录路径支持以斜杠结尾或不以斜杠结尾，即c:\a 和 c:\a\均可					//
	//																				//
	//  参数:																		//
	//		strDirName	:	目录名称												//
	//		pvecDir		:	找到的目录,如果传入为NULL,则不返回目录					//
	//		pvecFile	:	找到的文件,如果传入为NULL,则不返回文件					//
	//		bRecursion	:	是否需要递归查找										//
	//		vFileExe	:	需要查找的文件后缀，如果为空，则默认查找所有文件		//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	操作成功													//
	//		false	:	操作失败													//
	//																				//
	//******************************************************************************//
	static bool FindFileAndDir( const string& strDirNameSrc,vector<string>* pvecDir,vector<string>* pvecFile,bool bRecursion = true,const vector<string>* vFileExe = NULL );	
	//******************************************************************************//
	//																				//
	//	查找目录下的所有文件夹														//
	//	目录路径支持以斜杠结尾或不以斜杠结尾，即c:\a 和 c:\a\均可					//
	//																				//
	//  参数:																		//
	//		strDirName	:	目录名称												//
	//		f			:	找到的目录												//
	//																				//
	//	返回值：无																	//
	//******************************************************************************//
	static void GetSubDirNames(const string& dir, vector<string>& f);
	//******************************************************************************//
	//																				//
	//	查找本机所有硬盘    														//
	//																				//
	//  参数:																		//
	//		vDisk		:	找到的硬盘盘符											//
	//																				//
	//		true	:	操作成功													//
	//		false	:	操作失败													//
	//******************************************************************************//
	static bool GetDiskNames(vector<string>& vDisk);
	//获取剩余空间最大的盘符
	static bool GetLargestFreeBytesDisk(string& strDisk);
};


#endif