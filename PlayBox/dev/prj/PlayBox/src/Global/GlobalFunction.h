#ifndef _GLOBAL_FUNCTION_H
#define _GLOBAL_FUNCTION_H

#include <string>
using namespace std;

class GlobalFunc
{
public:
	static bool GetTempFilePath( const string& strSrcFile,string& strDesFile );
	static void	CreateGBoxRunInStartUp( bool bAutoRun );
	static bool IsAutoRun();
	static bool GetSkinConfPath( string& strPath );
	static bool DelSkinXml();
	static bool GetSkinPath( string& strPath );
};

#endif