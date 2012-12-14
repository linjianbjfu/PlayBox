#ifndef _GLOBAL_SWF_PATH_H
#define _GLOBAL_SWF_PATH_H

#include "GlobalFunction.h"

class GlobalSwfPath
{
public:
	
	static CString Nothing()
	{
		return GetSwfPath() + "stop.swf";
	}
	static CString Net_Loading()
	{
		return GetSwfPath() + "loading.swf";
	}
	
	static CString GetSwfPath()
	{
		char szBuffer[512];
		if( CLhcImg::GetHomePath( szBuffer,512) )
		{
			return CString( szBuffer ) + "\\Resources\\StandardUI\\";
		}else
		{
			return "";
		}
	}
};

#endif