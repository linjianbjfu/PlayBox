#ifndef _GLOBAL_SWF_PATH_H
#define _GLOBAL_SWF_PATH_H

#include "GlobalFunction.h"
#include "AppConfig\config\ConfigAppDef.h"
#include "AppConfig\config\ConfigParaDefault.h"
#include "YL_DirInfo.h"
#include "YL_FileInfo.h"

class GlobalSwfPath
{
public:
	static CString Net_Loading()
	{
		return GetSwfPath() + "loading.swf";
	}
	
	static CString GetSwfPath()
	{
		char szBuffer[512];
		if( CLhcImg::GetHomePath( szBuffer,512) )
			return CString( szBuffer ) + "\\Resources\\StandardUI\\";
		else
			return "";
	}

	static bool GetConfigSwfPath(std::string& strSwfSavePath)
	{
		AfxGetUserConfig()->GetConfigStringValue( CONF_APP_MODULE_NAME, CONF_APP_SWF_PATH, strSwfSavePath );
		if( !YL_FileInfo::IsDirectory( strSwfSavePath ) )
		{
			strSwfSavePath = ConfigParaDefault::GetDefaultAPP_SWF_PATH();
			AfxGetUserConfig()->SetConfigStringValue( CONF_APP_MODULE_NAME, CONF_APP_SWF_PATH, strSwfSavePath );
		}
		YL_DirInfo::MakeDir( strSwfSavePath ); //创建该目录
		return true;
	}
};

#endif