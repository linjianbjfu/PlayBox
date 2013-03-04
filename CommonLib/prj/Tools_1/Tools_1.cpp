// Tools_1.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Tools_1.h"
#include "../../common/YL_Base64.h"
#include "../../common/YL_StringUtil.h"
#include "../../common/YL_EncFileReg.h"
#include "../../common/LhcImg.h"
#include "../../common/YL_UserId.h"
#include "../../common/tools.h"
#include "../../include/YL_HTTPRequest.h"
#include <shlobj.h>

#define PLAYBOX	"\\Play Box"
#define CONF_DB "\\cpinfo.db"
#define CHANNEL_SECTION "qudao"
#define CHANNEL_KEY			"Tyep"


//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#define _PLAY_TOOL_LOG	 

FILE * fp_log = NULL;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}


int __stdcall CallTools(int argc, _TCHAR* argv1 = NULL,_TCHAR *argv2 = NULL,_TCHAR *argv3 = NULL,_TCHAR *argv4 = NULL)
{
	FILE  *fp = NULL;
#ifdef _PLAY_TOOL_LOG	
	char szLogPath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,szLogPath);
	strcat(szLogPath,"\\Tools.log");
#ifndef DEBUG
	fp = fopen(szLogPath,"w+");
#else
	fp= fopen("D:\\Debug.txt","w+");
#endif
	if(fp != NULL)
	{
		fprintf(fp,"%s\n","CallTools Invoked");
		fp_log = fp;
#ifdef DEBUG
		fprintf(fp,"%s\n",szLogPath);
#endif
	}
#endif	
	if( argc == 2)
	{
		if(stricmp( argv1, "UNINSTALL" ) == 0 )
		{
			string msg = "UNINSTALL";
			//string strMsg = FormatMsg(2, msg);
			SendRealMsg( msg,2 );
		}
		else if(stricmp(argv1,"INSTALL")== 0 )
		{
			string msg = "INSTALL";
			SendRealMsg(msg,0);
		}
		else if(stricmp(argv1,"FIRST RUN")==0)
		{
			string msg = "FIRST RUN";
			SendRealMsg(msg,3);
		}
		else if(stricmp(argv1,"ONLINE") == 0)
		{
			string msg = string(argv1);
			SendRealMsg(msg,1);
		}
		else if(stricmp(argv1,"REINSTALL") == 0)
		{
			string msg = string(argv1);
			SendRealMsg(msg,4);
		}
#ifdef _PLAY_TOOL_LOG	
		fprintf(fp,"%s Invoked.\n",argv1);
		fclose(fp);
#endif
		return 0;
	}

	if( argc == 5 
		&& stricmp( argv1, "WRITE" ) == 0 )
	{
		string strSection = string( argv2 );
		string strKey     = string( argv3 );
		string strValue   = string( argv4 );
		YL_EncFileReg::WriteString( strSection.c_str(), strKey.c_str(), strValue.c_str() );
#ifdef _PLAY_TOOL_LOG	
		fprintf(fp,"%s----argv1:%s,argv2:%s,argv3:%s,argv4:%s\n","WRITE Invoked",\
			argv1,argv2,argv3,argv4);
		fclose(fp);
#endif
		return 0;
	}
	if( argc == 4 
		&& stricmp( argv1, "DEL" ) == 0 )
	{
		string strSection = string( argv2 );
		string strKey     = string( argv3 );
		YL_EncFileReg::DeleteValue( HKEY_LOCAL_MACHINE, strSection.c_str(), strKey.c_str() );
#ifdef _PLAY_TOOL_LOG	
		fprintf(fp,"%s----argv1:%s,argv2:%s,argv3:%s\n","Del Invoked",\
			argv1,argv2,argv3);
		fclose(fp);
#endif
		
		return 0;
	}
#ifdef _PLAY_TOOL_LOG	
	fclose(fp);
#endif
	//发送最后一次日志
	//Update4SendLog("");
	return 1;
}

/*string FormatMsg(int nMsgType, const string& msg)
{
	//format <SRC:$ver|ACT:msg{$src}|U:$id>
	string strFMsg, str;
	char szID[VERSION_LENGTH], szVersion[VERSION_LENGTH], szInstallSRC[SRC_LENGTH];
	CLhcImg::GetUserID(szID, VERSION_LENGTH);
	CLhcImg::GetSoftwareVersion(szVersion, VERSION_LENGTH);
	CLhcImg::GetInstallSRC(szInstallSRC, SRC_LENGTH);
	str = string("SRC:") + szVersion + "|ACT:" + msg + "{" + szInstallSRC + "}" + "|U:" + szID;
	//str = string("|ACT:") + msg + "{" + szInstallSRC + "}" + "|U:" + szID;
	switch(nMsgType)
	{
	case 0:
		strFMsg = "|ACT:" + msg;// + "|SRC:" + szVersion;
		break;
	case 2:
		strFMsg = string("2%09<") + str + ">";
		break;
	case 9:
		strFMsg = string("9%09<") + str + ">";
		break;
	}
	return strFMsg;
}*/
void SendRealMsg( string& strMsg,int iMsgType )
{
	//Base64 加密
	char* chInput = new char[BASE64_LENGTH(strMsg.length())+1];
	YL_Base64Encode(chInput, strMsg.c_str(), (int)strMsg.length(), "langhua ");
	//send to log server
	YL_CHTTPRequest client;
	char szLogServer[SRC_LENGTH];
	memset(szLogServer, 0, SRC_LENGTH);

	if(!CLhcImg::GetLogServer(szLogServer, SRC_LENGTH))
	{
		_snprintf(szLogServer, SRC_LENGTH-1, "%s", "http://115.182.59.18");
	}

	char szID[INT_LENGTH], szVersion[VERSION_LENGTH], szInstallSRC[SRC_LENGTH];
	TCHAR szConfigFile[MAX_PATH];
	CLhcImg::GetUserID( szID, INT_LENGTH );
	CLhcImg::GetSoftwareVersion( szVersion, VERSION_LENGTH );
	CLhcImg::GetInstallSRC( szInstallSRC, SRC_LENGTH );

	//Get the market channel.
	TCHAR szMarkCh[MAX_PATH];
	GetMyDoucuments(szConfigFile);
	strcat(szConfigFile,PLAYBOX);
	strcat(szConfigFile,CONF_DB);
	if(!GetPrivateProfileString(CHANNEL_SECTION,CHANNEL_KEY,NULL,szMarkCh,MAX_PATH,szConfigFile))
	{

	}

	//Format the string.
	string strURL;
	YL_StringUtil::Format( strURL, "%sget_info.php?a=%d&id=%s&qd=%s&v=%s",
		szLogServer,
		iMsgType,
		szID,
		szMarkCh,
		szVersion);
	//Post the URL.
#ifdef DEBUG
	if(fp_log != NULL)
	{
		fprintf(fp_log,"The formated URL is \n %s \n",strURL.c_str());
	}
#endif

	bool bRet = client.Request(strURL.c_str(), YL_CHTTPRequest::REQUEST_POST, 10*1000, (unsigned char*)chInput, strlen(chInput));
	delete[] chInput;
	return ;
}

void Update4SendLog(void *pparam)
{
	char szHomePath[MAX_PATH], szUpdate[MAX_PATH], szPlayer[64];
	memset(szHomePath, 0 , MAX_PATH);
	memset(szUpdate, 0, MAX_PATH);
	memset(szPlayer, 0, 64);
	_snprintf(szPlayer, 64-1, "%s", (char*)pparam);
	if(!CLhcImg::GetHomePath(szHomePath, MAX_PATH))
	{
		return;
	}
	_snprintf(szUpdate, MAX_PATH-1, "%s\\Update.dll", szHomePath);

	HMODULE hLib = LoadLibrary(szUpdate);
	if(hLib != NULL)
	{
		LPCREATERECOGNISE lpfn = (LPCREATERECOGNISE)GetProcAddress(hLib, "BeginUpdate2");	
		if(lpfn != NULL)
		{
			lpfn(g_szSoftName, szPlayer, NULL);
		}
		FreeLibrary(hLib);
	}
}
BOOL GetMyDoucuments(TCHAR *szPath)
{
	memset(szPath,0,MAX_PATH);
	return (SHGetSpecialFolderPath(NULL,(LPSTR)szPath,CSIDL_PERSONAL,0));
}
