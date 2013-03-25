#include "stdafx.h"
#include "../../common/YL_Base64.h"
#include "../../common/YL_StringUtil.h"
#include "../../common/YL_EncFileReg.h"
#include "../../common/LhcImg.h"
#include "../../common/tools.h"
#include "../../include/YL_HTTPRequest.h"

#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

typedef void (__stdcall *LPCREATERECOGNISE)(const char* szSoft, const char* szModule, HWND hWnd);
void Update4SendLog(void *pparam);
void SendRealMsg( string& strMsg );
string FormatMsg(int nMsgType, const string& msg);
string Trim( string str ); //去掉str前后的双引号或者单引号

int _tmain(int argc, _TCHAR* argv[])
{
	if( argc == 2
		&& stricmp( argv[1], "UNINSTALL" ) == 0 )
	{
		string msg = "UNINSTALL";
		string strMsg = FormatMsg(2, msg);
		SendRealMsg( strMsg );
		return 0;
	}

	if( argc == 2
		&& stricmp( argv[1], "TASKBARPIN" ) == 0 )
	{
		TaskBarPin();
		return 0;
	}

	if( argc == 2
		&& stricmp( argv[1], "TASKBARUNPIN" ) == 0 )
	{
		TaskBarUnPin();
		return 0;
	}

	if( argc == 5 
		&& stricmp( argv[1], "WRITE" ) == 0 )
	{
		string strSection = string( argv[2] );
		string strKey     = string( argv[3] );
		string strValue   = string( argv[4] );
		YL_EncFileReg::WriteString( strSection.c_str(), strKey.c_str(), strValue.c_str() );
		return 0;
	}
	if( argc == 4 
		&& stricmp( argv[1], "DEL" ) == 0 )
	{
		string strSection = string( argv[2] );
		string strKey     = string( argv[3] );
		YL_EncFileReg::DeleteValue( HKEY_LOCAL_MACHINE, strSection.c_str(), strKey.c_str() );
		return 0;
	}
	//发送最后一次日志
	//Update4SendLog("");
}

string FormatMsg(int nMsgType, const string& msg)
{
	//format <SRC:$ver|ACT:msg{$src}|U:$id>
	string strFMsg, str;
	char szID[VERSION_LENGTH], szVersion[VERSION_LENGTH], szInstallSRC[SRC_LENGTH];
	//CLhcImg::GetUserID(szID, VERSION_LENGTH);
	//CLhcImg::GetSoftwareVersion(szVersion, VERSION_LENGTH);
	//CLhcImg::GetInstallSRC(szInstallSRC, SRC_LENGTH);
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
}
void SendRealMsg( string& strMsg )
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
	//CLhcImg::GetUserID( szID, INT_LENGTH );
	//CLhcImg::GetSoftwareVersion( szVersion, VERSION_LENGTH );
	//CLhcImg::GetInstallSRC( szInstallSRC, SRC_LENGTH );

	string strURL;
	YL_StringUtil::Format( strURL, "%s/ulog?type=ilog&ver=%s&id=%s&source=%s",
		szLogServer,
		szVersion,
		szID,
		szInstallSRC );

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