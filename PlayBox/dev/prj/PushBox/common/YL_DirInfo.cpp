#include "windows.h"
#include "shlobj.h"
#include "YL_DirInfo.h"
#include "YL_StringUtil.h"



bool YL_DirInfo::GetWindowsDir( char szDirBuffer[],unsigned int unBuffLen )
{
	string strDir;
	if( GetWindowsDir(strDir) && strDir.length() <= unBuffLen-1)
	{
		strcpy(szDirBuffer,strDir.c_str());
		return true;
	}
	return false;
}

bool YL_DirInfo::GetWindowsDir( string& strDir )
{
	char szPath[512];	
	if( SUCCEEDED(SHGetFolderPath(NULL, CSIDL_WINDOWS, NULL, 0, szPath)) ) 
	{
		strDir = szPath;
		return true;
	}
	return false;
}


bool YL_DirInfo::GetSystemDir( char szDirBuffer[],unsigned int unBuffLen )
{
	string strDir;
	if( GetSystemDir(strDir) && strDir.length() <= unBuffLen-1)
	{
		strcpy(szDirBuffer,strDir.c_str());
		return true;
	}
	return false;
}

bool YL_DirInfo::GetSystemDir( string& strDir )
{
	char szPath[512];	
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, 0, szPath))) 
	{
		strDir = szPath;
		return true;
	}
	return false;
}

bool YL_DirInfo::GetTempDir( char szDirBuffer[],unsigned int unBuffLen )
{
	string strDir;
	if( GetTempDir(strDir) && strDir.length() <= unBuffLen-1)
	{
		strcpy(szDirBuffer,strDir.c_str());
		return true;
	}
	return false;
}

bool YL_DirInfo::GetTempDir( string& strDir )
{
	char sz[MAX_PATH]={0};
	::GetModuleFileName(NULL, sz, MAX_PATH);
	string strExePath = sz;

	size_t n = strExePath.find_last_of("\\");
	if( n == string::npos )
	{
		return false;
	}
	string str = strExePath.substr(0,n);

	if(str.length() > 0 )		
	{
		if(str[str.length()-1] == '\\')
		{
			str = str.substr(0, str.length()-1);
		}
		str += "\\temp";
		
		strDir = str;
		return true;
	}

	return false;
}

bool YL_DirInfo::GetProgramFilesDir( char szDirBuffer[],unsigned int unBuffLen )
{
	string strDir;
	if( GetProgramFilesDir(strDir) && strDir.length() <= unBuffLen-1)
	{
		strcpy(szDirBuffer,strDir.c_str());
		return true;
	}
	return false;
}

bool YL_DirInfo::GetProgramFilesDir( string& strDir )
{
	char szPath[512];	
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, 0, szPath))) 
	{
		strDir = szPath;
		return true;
	}
	return false;
}


bool YL_DirInfo::MakeDir( const char* pszDirName )
{
	if( pszDirName == NULL )
	{
		return false;
	}
	return MakeDir(string(pszDirName));
}

bool YL_DirInfo::MakeDir( const string& strDirName )
{
	WIN32_FIND_DATA fileInfo;
	vector<string> vecDirs;

	if( strDirName == "" )
	{
		return false;
	}

	HANDLE hHandle = FindFirstFile(strDirName.c_str(), &fileInfo);
	if( hHandle != INVALID_HANDLE_VALUE) 
	{
		if( fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			FindClose(hHandle);
			return true;
		}
		FindClose( hHandle );

		DeleteFile( strDirName.c_str() );
	}	

	string strTemp;
	//scan the path string, get all sub dir path string in the path
	for(unsigned int i= 0; i < strDirName.length(); i++)
	{
		if(strDirName[i] != '\\' && strDirName[i] != '/')
		{
			strTemp += strDirName[i];
		}
		else
		{
			vecDirs.push_back(strTemp);
			strTemp += "\\";
		}

		//end of strPath
		if( strDirName[i] != '\\' && strDirName[i] != '/' && i == strDirName.length() - 1)
		{
			vecDirs.push_back(strTemp);
		}
	}

	bool ret = true;
	//create dirs in the vecDirs 
	for(int i = 1; i < (int)vecDirs.size(); i++)
	{
		if(::CreateDirectory(vecDirs[i].c_str(), NULL) == TRUE )
		{
			SetFileAttributes(vecDirs[i].c_str(), FILE_ATTRIBUTE_NORMAL);
		}
		else
		{
			DWORD dwError = ::GetLastError();
			if(dwError != ERROR_ALREADY_EXISTS)
			{
				ret = false;
				break;
			}
		}
	}

	return ret;
}


bool YL_DirInfo::DeleteDir( const char* pszDirName)
{
	if( pszDirName == NULL )
	{
		return false;
	}
	return DeleteDir(string(pszDirName));
}

bool YL_DirInfo::DeleteDir( const string& strDirName)
{	
	vector<string> vecDir;
	vector<string> vecFile;

	if( YL_DirInfo::FindFileAndDir(strDirName,&vecDir,&vecFile,false) )
	{
		for(size_t idx = 0;idx <vecDir.size();idx++ )
		{
			if( !DeleteDir(vecDir[idx]) )
			{
				return false;
			}
		}

		if( ClearDir(strDirName) )
		{
			return true;
		}		
	}

	return false;
}

bool YL_DirInfo::ClearDir( const char* pszDirName,const char* pszFileExe )
{
	if( pszDirName == NULL )
	{
		return false;
	}
	if( pszFileExe == NULL )
	{
		return ClearDir(string(pszDirName));
	}
	else
	{
		return ClearDir(string(pszDirName),string(pszFileExe));
	}
}

bool YL_DirInfo::ClearDir( const string& strDirName ,const string& strExe)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	string dir;
	
	dir = strDirName + "\\*";	

	hFind = FindFirstFile(dir.c_str(), &FindFileData);

	if(hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	do{
		if(strcmp(FindFileData.cFileName, ".") != 0 &&	strcmp(FindFileData.cFileName, "..") != 0 )
		{
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				continue;
			}

			string strFileName = strDirName + "\\" + FindFileData.cFileName;

			DWORD dwFlags = GetFileAttributes (strFileName.c_str ());
			dwFlags &= ~FILE_ATTRIBUTE_READONLY;
			SetFileAttributes (strFileName.c_str (), dwFlags);

			if( strExe == "")
			{
				if( ::DeleteFile(strFileName.c_str()) == 0 )
				{
					FindClose( hFind );
					return false;
				}
			}
			else 
			{
				char* pTmp = strstr(FindFileData.cFileName,strExe.c_str());
				if( pTmp != NULL && 
					( strlen( FindFileData.cFileName) - (pTmp - FindFileData.cFileName) ) == strExe.length() )
				{
					if( ::DeleteFile(strFileName.c_str()) == 0 )
					{
						FindClose( hFind );
						return false;
					}
				}
			}
			
		}
	}while(FindNextFile(hFind, &FindFileData));
	
	FindClose( hFind );	

	if(::RemoveDirectory(strDirName.c_str()) == FALSE)
	{
		return false;
	}
	return true;
}

bool YL_DirInfo::FindFileAndDir( const string& strDirNameSrc,vector<string>* pvecDir,vector<string>* pvecFile,bool bRecursion,const vector<string>* vFileExe)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	string strDirName = strDirNameSrc;
	if( strDirName.find_last_of("\\") == strDirName.length()-1)
	{
		strDirName = strDirName.substr(0,strDirName.length()-1);
	}

	//find a file	
	hFind = FindFirstFile((strDirName + "\\*").c_str(), &FindFileData);	

	if(hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	//find all files and dirs
	do{
		if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && pvecFile  )
		{
			if( vFileExe == NULL )
			{
				(*pvecFile).push_back(strDirName + "\\" + FindFileData.cFileName);
			}
			else 
			{
				string strName = FindFileData.cFileName;
				YL_StringUtil::Str2Lower( strName );
				string strExe;

				for(size_t idx = 0;idx < vFileExe->size();idx++ )
				{
					strExe = (*vFileExe)[idx];
					char* pTmp = (char*)strstr( strName.c_str(),strExe.c_str());
					if( pTmp != NULL && 
						( strlen( FindFileData.cFileName) - (pTmp - strName.c_str()) ) == strExe.length() )
					{
						(*pvecFile).push_back(strDirName + "\\" + FindFileData.cFileName);
					}
				}
			}
		}
		else
		{
			if (strcmp(FindFileData.cFileName,".") != 0 && strcmp(FindFileData.cFileName,"..") != 0 && FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )			
			{
				if( pvecDir )
				{
					(*pvecDir).push_back(strDirName + "\\" + FindFileData.cFileName);				
				}

				if(bRecursion)
				{
					if( !FindFileAndDir(strDirName + "\\" + FindFileData.cFileName, pvecDir,pvecFile, bRecursion, vFileExe) )
					{
//						FindClose( hFind );
//						return false;
					}
				}
			}
		}
	}while( FindNextFile(hFind, &FindFileData) == TRUE);

	FindClose( hFind );
	return true;
}

void YL_DirInfo::GetSubDirNames(const string& dir, vector<string>& f)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	//find a file
	hFind = FindFirstFile((dir + "\\*").c_str(), &FindFileData);
	if(hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}
	//find all files and dirs
	do{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(FindFileData.cFileName,".") != 0 && strcmp(FindFileData.cFileName,"..") != 0)			
			{
				f.push_back(FindFileData.cFileName);
			}
		}
	}while(FindNextFile(hFind, &FindFileData) == TRUE);

	FindClose(hFind);
}

bool YL_DirInfo::GetDiskNames(vector<string>& vDisk)
{
    int DSLength = GetLogicalDriveStrings(0, NULL);
    //通过GetLogicalDriveStrings()函数获取所有驱动器字符串信息长度。

    char* DStr = new char[DSLength];//用获取的长度在堆区创建一个c风格的字符串数组
    GetLogicalDriveStrings(DSLength, (LPTSTR)DStr);
    //通过GetLogicalDriveStrings将字符串信息复制到堆区数组中,其中保存了所有驱动器的信息。
    int si=0;
    for (int i = 0; i < DSLength/4; ++i)
    //DStr内部保存的数据是A:\NULLB:\NULLC:\NULL，这样的信息，所以DSLength/4可以获得具体大循环范围
    {
        char dir[4]={DStr[si],':','\\','\0'};
        int DType = GetDriveType(DStr+i*4);
        //GetDriveType函数，可以获取驱动器类型，参数为驱动器的根目录
        if (DType == DRIVE_FIXED)//硬盘
        {
			vDisk.push_back(dir);
        }
        else if (DType == DRIVE_CDROM)//光驱
        {
        }
        else if (DType == DRIVE_REMOVABLE)//可移动式磁盘
        {
        }
        else if (DType == DRIVE_REMOTE)//网络磁盘
        {
        }
        else if (DType == DRIVE_RAMDISK)//虚拟RAM磁盘
        {
        }
        else if (DType == DRIVE_UNKNOWN)//未知设备
        {
        }
        si += 4;
    }
	delete DStr;
	return true;
}

bool YL_DirInfo::GetLargestFreeBytesDisk(string& strDisk)
{
	vector<string> vDisk;
	if( !GetDiskNames(vDisk) )
		return false;

	ULONGLONG i64LargestFreeBytes = 0;
	for( vector<string>::iterator it1 = vDisk.begin(); it1 != vDisk.end(); it1++ )
	{
		ULONGLONG i64FreeBytesToCaller, i64TotalBytes, i64FreeBytes;
		BOOL fResult = GetDiskFreeSpaceEx (
						it1->c_str(),
						(PULARGE_INTEGER)&i64FreeBytesToCaller,
						(PULARGE_INTEGER)&i64TotalBytes,
						(PULARGE_INTEGER)&i64FreeBytes);
		//GetDiskFreeSpaceEx函数，可以获取驱动器磁盘的空间状态,函数返回的是个BOOL类型数据
		if (fResult)//通过返回的BOOL数据判断驱动器是否在工作状态
		{
			if( i64LargestFreeBytes < i64FreeBytes )
			{
				i64LargestFreeBytes = i64FreeBytes;
				strDisk = *it1;
			}
		}
		else
		{
			//设备未准备好
		}
	}
	if( i64LargestFreeBytes == 0 )
	{
		return false;
	}else
	{
		return true;
	}
}