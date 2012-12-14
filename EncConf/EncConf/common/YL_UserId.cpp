#include "YL_UserId.h"
#include "MacroInt.h"
#include "LhcImg.h"
#include <stdio.h>
#include <string>

using namespace std;

bool YL_UserId::GetUserFileName(char szFileName[], int namelen) //g7kk.dat
{
	char path[MAX_PATH];
	memset(szFileName, 0, namelen);
	if(!CLhcImg::GetConfigPath(path, MAX_PATH))
		return false;

	if(GetPrivateProfileString("Config", "userfile", NULL, szFileName, namelen, path) == 0)
	{
		//g7kk.dat
		return false;
	}

	if(strlen(szFileName) <= 0)
		return false;
	szFileName[namelen-1] = '\0';
	return true;
}

bool YL_UserId::GetUserFilePath(char szFilePath[], int pathlen) //g7kk.dat
{
	CHAR szSystemPath[MAX_PATH], szUserFileName[MAX_PATH];
	memset(szFilePath, 0, pathlen);
	memset(szSystemPath, 0, MAX_PATH);
	memset(szUserFileName, 0, MAX_PATH);
	if( GetSystemDirectory(szSystemPath, MAX_PATH) == 0 )
	{				
		return false;
	}

	if(!GetUserFileName(szUserFileName, MAX_PATH))
	{
		return false;
	}

	strcpy(szFilePath, szSystemPath);
	strcat(szFilePath, "\\");
	strcat(szFilePath, szUserFileName);
	return true;
}

bool YL_UserId::GetUserID( char szID[], int idlen)
{
	memset(szID, 0, idlen);
	CHAR szTemp[MAX_PATH], szDesFilePath[MAX_PATH];
	if(!GetUserFilePath(szDesFilePath, MAX_PATH)) //g7kk.dat
	{
		return false;
	}

	FILE* file;
	if( (file = fopen( szDesFilePath, "r")) == NULL )
	{		
		return false;
	}

	memset(szTemp, 0, MAX_PATH);
	fgets( szTemp, MAX_PATH, file );
	fclose( file );
	string temp(szTemp);
	string::size_type index = temp.find_first_of('\r');
	string id = temp.substr(0, index);
	_snprintf(szID, idlen-1, "%s", id.c_str());
	szID[idlen-1] = '\0';

	if( atoi( szID ) <= 0 )
	{
		memset(szID, 0, idlen);
		return false;
	}

	return true;
}

bool YL_UserId::SetUserID(const char *szID)
{
	char szUserFilePath[MAX_PATH], szSRC[SRC_LENGTH];
	if(!GetUserFilePath(szUserFilePath, MAX_PATH))
	{
		return false;
	}
	GetInstallSRC(szSRC, SRC_LENGTH);

	FILE *file;
	if( (file = fopen( szUserFilePath, "w")) == NULL )
	{
		return false;
	}	
	fprintf(file, "%s\r", szID);
	fprintf(file, "%s\r", szSRC);
	fclose( file );
	return true;
}

bool YL_UserId::GetInstallSRC(char szSRC[], int srclen)
{
	memset(szSRC, 0, srclen);
	CHAR szTemp[MAX_PATH], szDesFilePath[MAX_PATH];
	if(!GetUserFilePath(szDesFilePath, MAX_PATH))
	{
		return false;
	}

	FILE* file;
	if( (file = fopen( szDesFilePath,"r")) == NULL )
	{		
		return false;
	}
	//读取第一行 可能是ID, 也可能是安装源
	memset(szTemp, 0, MAX_PATH);
	fgets( szTemp, MAX_PATH, file );
	fclose( file );

	string temp(szTemp);
	string::size_type index = temp.find_first_of('\r');
	string src = temp.substr(index+1, temp.length()-index-1);
	if(src.length() <= 0 || (src.length() == 1 && src[0] == '\r'))
		return false;
	
	_snprintf(szSRC, srclen-1, "%s", src.c_str());
	szSRC[srclen-1] = '\0';
	if(src.length() < (unsigned)srclen)
	{
		if(szSRC[src.length()-1] == '\r')
			szSRC[src.length()-1] = '\0';
	}

	return true;
}

bool YL_UserId::SetInstallSRC(const char* szSRC)
{
	char szUserFilePath[MAX_PATH], szID[INT_LENGTH];
	if(!GetUserFilePath(szUserFilePath, MAX_PATH))
	{
		return false;
	}
	GetUserID(szID, INT_LENGTH);

	FILE *file;
	if( (file = fopen( szUserFilePath, "w" )) == NULL )
	{
		return false;
	}
	fprintf( file, "%s\r", szID );
	fprintf( file, "%s\r", szSRC);
	fclose( file );
	return true;
}