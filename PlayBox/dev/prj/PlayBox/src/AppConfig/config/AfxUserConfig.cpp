#include "stdafx.h"
#include "AfxUserConfig.h"
#include "ConfigLayoutDef.h"
#include "ConfigSettingDef.h"
#include "AfxConfigType.h"
#include "ConfigAppDef.h"
#include "ConfigSkinDef.h"
#include "YL_FileInfo.h"
#include "YL_EncFileReg.h"

AfxUserConfig::AfxUserConfig():
m_bIsSepMode(FALSE)
{
	InitializeCriticalSection(&m_cs);
	InitializeCriticalSection(&m_Configcs);
}

AfxUserConfig::~AfxUserConfig()
{
	DeleteCriticalSection(&m_cs);
	DeleteCriticalSection(&m_Configcs);

	map<string,CConfigObject*>::iterator it = m_mapUserConfig.begin();
	for( ;it != m_mapUserConfig.end();it++ )
	{
		delete it->second;
	}
	m_mapUserConfig.clear();
}

void AfxUserConfig::Config_AppStart()
{
	m_bIsSepMode=CheckIsSepMode();
	AddConfigObject( CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT, (sizeof( CONF_LAYOUT) / sizeof(char*))/2 );
	AddConfigObject( CONF_SETTING_MODULE_NAME,CONF_SETTING,( sizeof( CONF_SETTING) / sizeof( char*))/2);
	AddConfigObject( CONF_APP_MODULE_NAME,CONF_APP,( sizeof( CONF_APP) / sizeof( char*))/2);
	AddConfigObject( CONF_SKINSUB_MODULE_NAME,CONF_SKINSUB,( sizeof( CONF_SKINSUB) / sizeof( char*))/2);
	Load();
}

void AfxUserConfig::Config_AppExit()
{
	Save();
}

void AfxUserConfig::AddConfigObject( const char* strModule,const char** ppConfig,int iConfigItemNum)
{
	CConfigObject*	pConfigObject = new CConfigObject;

	for( int idx = 0;idx <iConfigItemNum;idx++ )
	{
		if( strcmp( ppConfig[idx*2+1],TYPE_CONFIG_STR ) == 0 )
		{
			pConfigObject->m_mapString2String[ ppConfig[idx*2] ] = "";
		}
		if( strcmp( ppConfig[idx*2+1],TYPE_CONFIG_STR_HASSEP ) == 0 )
		{
			pConfigObject->m_mapString2String_sep[ ppConfig[idx*2] ] = "";
		}
		else if( strcmp( ppConfig[idx*2+1],TYPE_CONFIG_INT ) == 0 )
		{
			pConfigObject->m_mapString2Int[ ppConfig[idx*2] ] = 0;
		}
		else if( strcmp( ppConfig[idx*2+1],TYPE_CONFIG_INT_HASSEP ) == 0 )
		{
			pConfigObject->m_mapString2Int_sep[ ppConfig[idx*2] ] = 0;
		}
		else if( strcmp( ppConfig[idx*2+1],TYPE_CONFIG_BOOL) == 0 )
		{
			pConfigObject->m_mapString2Bool[ ppConfig[idx*2] ] = 0;
		}
		else if( strcmp( ppConfig[idx*2+1],TYPE_CONFIG_BOOL_HASSEP) == 0 )
		{
			pConfigObject->m_mapString2Bool_sep[ ppConfig[idx*2] ] = 0;
		}
	}
	m_mapUserConfig[string(strModule)] = pConfigObject;
}

void AfxUserConfig::Load()
{
	map<string,CConfigObject*>::iterator it = m_mapUserConfig.begin();
	
	string strModule;
	for( ;it != m_mapUserConfig.end();it++ )
	{
		strModule = it->first;

		map<string,string>::iterator itStr = it->second->m_mapString2String.begin();
		while( itStr != it->second->m_mapString2String.end() )
		{
			GetStringFromFile( strModule,itStr->first );
			itStr++;
		}

		map<string,string>::iterator itStr_sep = it->second->m_mapString2String_sep.begin();
		while( itStr_sep != it->second->m_mapString2String_sep.end() )
		{
			GetStringFromFile_HasSep( strModule,itStr_sep->first );
			itStr_sep++;
		}

		map<string,int>::iterator itInt = it->second->m_mapString2Int.begin();
		while( itInt != it->second->m_mapString2Int.end() )
		{
			GetIntFromFile( strModule,itInt->first );
			itInt++;
		}

		map<string,int>::iterator itInt_sep = it->second->m_mapString2Int_sep.begin();
		while( itInt_sep != it->second->m_mapString2Int_sep.end() )
		{
			GetIntFromFile_HasSep( strModule,itInt_sep->first );
			itInt_sep++;
		}

		map<string,bool>::iterator itBool = it->second->m_mapString2Bool.begin();
		while( itBool != it->second->m_mapString2Bool.end() )
		{
			GetBoolFromFile( strModule,itBool->first );
			itBool++;
		}	

		map<string,bool>::iterator itBool_sep = it->second->m_mapString2Bool_sep.begin();
		while( itBool_sep != it->second->m_mapString2Bool_sep.end() )
		{
			GetBoolFromFile_HasSep( strModule,itBool_sep->first );
			itBool_sep++;
		}		
	}
}

void AfxUserConfig::Save()
{
	map<string,CConfigObject*>::iterator it = m_mapUserConfig.begin();
	
	string strModule;
	for( ;it != m_mapUserConfig.end();it++ )
	{
		strModule = it->first;

		map<string,string>::iterator itStr = it->second->m_mapString2String.begin();
		while( itStr != it->second->m_mapString2String.end() )
		{
			WriteString2File( strModule,itStr->first );
			itStr++;
		}

		map<string,string>::iterator itStr_sep = it->second->m_mapString2String_sep.begin();
		while( itStr_sep != it->second->m_mapString2String_sep.end() )
		{
			WriteString2File_HasSep( strModule,itStr_sep->first );
			itStr_sep++;
		}

		map<string,int>::iterator itInt = it->second->m_mapString2Int.begin();
		while( itInt != it->second->m_mapString2Int.end() )
		{
			WriteInt2File( strModule,itInt->first );
			itInt++;
		}

		map<string,int>::iterator itInt_sep = it->second->m_mapString2Int_sep.begin();
		while( itInt_sep != it->second->m_mapString2Int_sep.end() )
		{
			WriteInt2File_HasSep( strModule,itInt_sep->first );
			itInt_sep++;
		}

		map<string,bool>::iterator itBool = it->second->m_mapString2Bool.begin();
		while( itBool != it->second->m_mapString2Bool.end() )
		{
			WriteBool2File( strModule,itBool->first );
			itBool++;
		}

		map<string,bool>::iterator itBool_sep = it->second->m_mapString2Bool_sep.begin();
		while( itBool_sep != it->second->m_mapString2Bool_sep.end() )
		{
			WriteBool2File_HasSep( strModule,itBool_sep->first );
			itBool_sep++;
		}
	}

}


void AfxUserConfig::AttachConfigMessage( const string& strModule,AfxUserConfig_Observer* pObserver )
{
EnterCriticalSection(&m_cs);
	if( m_mapObservers.find( strModule) != m_mapObservers.end() )
	{
		list<AfxUserConfig_Observer*>& listOb = m_mapObservers[strModule];		
		listOb.push_back(pObserver);
	}
	else
	{
		list<AfxUserConfig_Observer*> listob;
		listob.push_back(pObserver);
		m_mapObservers[strModule] = listob;		
	}	
LeaveCriticalSection(&m_cs);
}

void AfxUserConfig::DetachConfigMessage( const string& strMolude,AfxUserConfig_Observer* pObserver )
{
EnterCriticalSection(&m_cs);
	if( m_mapObservers.find(strMolude) == m_mapObservers.end() )
	{
		LeaveCriticalSection(&m_cs);
		return;
	}

	list<AfxUserConfig_Observer*>& listOb = m_mapObservers[strMolude];

	for( list<AfxUserConfig_Observer*>::iterator it = listOb.begin(); it != listOb.end(); it++ )
	{
		if( (*it) == pObserver )
		{
			listOb.erase(it);
			break;
		}
	}
LeaveCriticalSection(&m_cs);
}

void AfxUserConfig::GetConfigStringValue( const string& strModule,const string& strKey,string& strValue)
{
	if( m_mapUserConfig.find(strModule) == m_mapUserConfig.end())
	{
		return;
	}

EnterCriticalSection(&m_Configcs);

if(m_bIsSepMode &&
		m_mapUserConfig[strModule]->m_mapString2String_sep.find(strKey) != m_mapUserConfig[strModule]->m_mapString2String_sep.end())
	{
			strValue = m_mapUserConfig[strModule]->m_mapString2String_sep[strKey];
	}
	else
	{
		strValue = m_mapUserConfig[strModule]->m_mapString2String[strKey];
	}

LeaveCriticalSection(&m_Configcs);
}

void AfxUserConfig::SetConfigStringValue(const string& strModule,const string& strKey,const string& strValue,bool bWrite)
{
	if( m_mapUserConfig.find(strModule) == m_mapUserConfig.end())
	{
		return;
	}

EnterCriticalSection(&m_Configcs);
	string strOldValue;
	if(m_bIsSepMode &&
		m_mapUserConfig[strModule]->m_mapString2String_sep.find(strKey) != m_mapUserConfig[strModule]->m_mapString2String_sep.end())
	{
		strOldValue=m_mapUserConfig[strModule]->m_mapString2String_sep[strKey];
		m_mapUserConfig[strModule]->m_mapString2String_sep[strKey] = strValue;
		if( bWrite )
		{
			WriteString2File_HasSep( strModule,strKey );
		}
	}
	else
	{
		strOldValue = m_mapUserConfig[strModule]->m_mapString2String[strKey];
		m_mapUserConfig[strModule]->m_mapString2String[strKey] = strValue;
		if( bWrite )
		{
			WriteString2File( strModule,strKey );
		}
	}

LeaveCriticalSection(&m_Configcs);
	

	if( strOldValue != strValue )
	{
		NotifyObserver( strModule,strKey);
	}
}

void AfxUserConfig::GetConfigIntValue( const string& strModule,const string& strKey,int& iValue)
{
	if( m_mapUserConfig.find(strModule) == m_mapUserConfig.end())
	{
		return;
	}

EnterCriticalSection(&m_Configcs);
	if(m_bIsSepMode &&
	m_mapUserConfig[strModule]->m_mapString2Int_sep.find(strKey) != m_mapUserConfig[strModule]->m_mapString2Int_sep.end())
	{
		iValue = m_mapUserConfig[strModule]->m_mapString2Int_sep[strKey];
	}
	else
	{
		iValue = m_mapUserConfig[strModule]->m_mapString2Int[strKey];
	}
LeaveCriticalSection(&m_Configcs);
}

void AfxUserConfig::SetConfigIntValue(const string& strModule,const string& strKey,int iValue,bool bWrite)
{

	if( m_mapUserConfig.find(strModule) == m_mapUserConfig.end())
	{
		return;
	}

EnterCriticalSection(&m_Configcs);
	int iOldValue;
	if(m_bIsSepMode &&
	m_mapUserConfig[strModule]->m_mapString2Int_sep.find(strKey) != m_mapUserConfig[strModule]->m_mapString2Int_sep.end())
	{
		iOldValue=m_mapUserConfig[strModule]->m_mapString2Int_sep[strKey];
		m_mapUserConfig[strModule]->m_mapString2Int_sep[strKey] = iValue;
		if( bWrite )
		{
			WriteInt2File_HasSep( strModule,strKey );
		}
	}
	else
	{

		iOldValue = m_mapUserConfig[strModule]->m_mapString2Int[strKey];
		m_mapUserConfig[strModule]->m_mapString2Int[strKey] = iValue;
		if( bWrite )
		{
			WriteInt2File( strModule,strKey );
		}
	}

LeaveCriticalSection(&m_Configcs);
	if( iOldValue != iValue )
	{
		NotifyObserver( strModule,strKey);
	}

}

void AfxUserConfig::GetConfigBoolValue( const string& strModule,const string& strKey,bool& bValue)
{
	if( m_mapUserConfig.find(strModule) == m_mapUserConfig.end())
	{
		return;
	}

EnterCriticalSection(&m_Configcs);
	if(m_bIsSepMode &&
	m_mapUserConfig[strModule]->m_mapString2Bool_sep.find(strKey) != m_mapUserConfig[strModule]->m_mapString2Bool_sep.end())
	{
		bValue = m_mapUserConfig[strModule]->m_mapString2Bool_sep[strKey];
	}
	else
	{
		bValue = m_mapUserConfig[strModule]->m_mapString2Bool[strKey];
	}

LeaveCriticalSection(&m_Configcs);
}

void AfxUserConfig::SetConfigBoolValue(const string& strModule,const string& strKey,bool bValue,bool bWrite)
{

	if( m_mapUserConfig.find(strModule) == m_mapUserConfig.end())
	{
		return;
	}

EnterCriticalSection(&m_Configcs);
	bool bOldValue;
	if(m_bIsSepMode &&
	m_mapUserConfig[strModule]->m_mapString2Bool_sep.find(strKey) != m_mapUserConfig[strModule]->m_mapString2Bool_sep.end())
	{
		bOldValue=m_mapUserConfig[strModule]->m_mapString2Bool_sep[strKey];
		m_mapUserConfig[strModule]->m_mapString2Bool_sep[strKey] = bValue;
		if( bWrite )
		{
			WriteBool2File_HasSep( strModule,strKey );
		}
	}
	else
	{
		bOldValue = m_mapUserConfig[strModule]->m_mapString2Bool[strKey];
		m_mapUserConfig[strModule]->m_mapString2Bool[strKey] = bValue;
		if( bWrite )
		{
			WriteBool2File( strModule,strKey );
		}
	}

LeaveCriticalSection(&m_Configcs);

	if( bOldValue != bValue )
	{
		NotifyObserver( strModule,strKey);
	}
}


void AfxUserConfig::NotifyObserver(const string& strModule,const string& strKey )
{
EnterCriticalSection(&m_cs);
	if( m_mapObservers.find(strModule) == m_mapObservers.end() )
	{
LeaveCriticalSection(&m_cs);
		return;
	}
	else
	{
		list<AfxUserConfig_Observer*>& pList = m_mapObservers[strModule];
		list<AfxUserConfig_Observer*>::iterator it = pList.begin();
		for(;it!=pList.end();it++)
		{
			(*it)->OnConfigChange(strKey);
		}
	}	
LeaveCriticalSection(&m_cs);
}

void AfxUserConfig::GetStringFromFile(const string& strModule,const string& strKey)
{
	char* pszBuffer = new char[1024];
	YL_EncFileReg::GetString( strModule.c_str(),strKey.c_str(),pszBuffer,1024 );
	if(strlen(pszBuffer) <= 0)
	{
		char szHome[MAX_PATH];
		if(CLhcImg::GetHomePath(szHome, MAX_PATH))
		{
			if(strnicmp(strKey.c_str(), "songpath", 8) == 0)
			{
				m_mapUserConfig[strModule]->m_mapString2String[strKey] = string(szHome).substr(0, 2) + "\\KwDownload\\song";
			}else if(strnicmp(strKey.c_str(), "lrcsavepath", 11) == 0)
			{
				m_mapUserConfig[strModule]->m_mapString2String[strKey] = string(szHome).substr(0, 2) + "\\KwDownload\\lyric";
			}

			delete[] pszBuffer;
			return;
		}
	}
	
	m_mapUserConfig[strModule]->m_mapString2String[strKey] = pszBuffer;

	delete[] pszBuffer;
}
void AfxUserConfig::GetStringFromFile_HasSep(const string& strModule,const string& strKey)
{
	if(m_bIsSepMode)
	{
		string strSepKey="sep"+strKey;
		char* pszBuffer = new char[1024];
		YL_EncFileReg::GetString( strModule.c_str(),strSepKey.c_str(),pszBuffer,1024 );
		if(strlen(pszBuffer) <= 0)
		{
			char szHome[MAX_PATH];
			if(CLhcImg::GetHomePath(szHome, MAX_PATH))
			{
				if(strnicmp(strSepKey.c_str(), "sepsongpath", 8) == 0)
				{
					m_mapUserConfig[strModule]->m_mapString2String_sep[strKey] = string(szHome).substr(0, 2) + "\\KwDownload\\song";
				}else if(strnicmp(strSepKey.c_str(), "seplrcsavepath", 11) == 0)
				{
					m_mapUserConfig[strModule]->m_mapString2String_sep[strKey] = string(szHome).substr(0, 2) + "\\KwDownload\\lyric";
				}

				delete[] pszBuffer;
				return;
			}
		}

		m_mapUserConfig[strModule]->m_mapString2String_sep[strKey] = pszBuffer;

		delete[] pszBuffer;
	}
	else
	{
		GetStringFromFile(strModule,strKey);
	}
}

void AfxUserConfig::WriteString2File(const string& strModule,const string& strKey)
{
	YL_EncFileReg::WriteString( strModule.c_str(),strKey.c_str(),m_mapUserConfig[ strModule ]->m_mapString2String[strKey] .c_str() );
}
void AfxUserConfig::WriteString2File_HasSep(const string& strModule,const string& strKey)
{
	if(m_bIsSepMode)
	{
		string strSepKey="sep"+strKey;
		YL_EncFileReg::WriteString( strModule.c_str(),strSepKey.c_str(),m_mapUserConfig[ strModule ]->m_mapString2String_sep[strKey] .c_str() );
	}
	else
	{
		WriteString2File(strModule,strKey);
	}
}

void AfxUserConfig::GetIntFromFile(const string& strModule,const string& strKey)
{
	m_mapUserConfig[ strModule ]->m_mapString2Int[strKey] = YL_EncFileReg::GetInt( strModule.c_str(),strKey.c_str(),0);
}
void AfxUserConfig::GetIntFromFile_HasSep(const string& strModule,const string& strKey)
{
	if(m_bIsSepMode)
	{
		string strSepKey="sep"+strKey;
		m_mapUserConfig[ strModule ]->m_mapString2Int_sep[strKey] = YL_EncFileReg::GetInt( strModule.c_str(),strSepKey.c_str(),0);
	}
	else
	{
		GetIntFromFile(strModule,strKey);
	}
}
void AfxUserConfig::WriteInt2File(const string& strModule,const string& strKey)
{
	YL_EncFileReg::WriteInt( strModule.c_str(),strKey.c_str(),m_mapUserConfig[ strModule ]->m_mapString2Int[strKey] );
}
void AfxUserConfig::WriteInt2File_HasSep(const string& strModule,const string& strKey)
{
	if(m_bIsSepMode)
	{
		string strSepKey="sep"+strKey;
		YL_EncFileReg::WriteInt( strModule.c_str(),strSepKey.c_str(),m_mapUserConfig[ strModule ]->m_mapString2Int_sep[strKey] );
	}
	else
	{
		WriteInt2File(strModule,strKey);
	}
}

void AfxUserConfig::GetBoolFromFile(const string& strModule,const string& strKey)
{
	m_mapUserConfig[ strModule ]->m_mapString2Bool[strKey] = YL_EncFileReg::GetInt( strModule.c_str(),strKey.c_str(),0) == 1?true:false;
}
void AfxUserConfig::GetBoolFromFile_HasSep(const string& strModule,const string& strKey)
{
	if(m_bIsSepMode)
	{
		string strSepKey="sep"+strKey;
		m_mapUserConfig[ strModule ]->m_mapString2Bool_sep[strKey] = YL_EncFileReg::GetInt( strModule.c_str(),strSepKey.c_str(),0) == 1?true:false;
	}
	else
	{
		GetBoolFromFile(strModule,strKey);
	}
}
void AfxUserConfig::WriteBool2File(const string& strModule,const string& strKey)
{
	YL_EncFileReg::WriteBool(strModule.c_str(),strKey.c_str(),m_mapUserConfig[ strModule ]->m_mapString2Bool[strKey] )	;
}
void AfxUserConfig::WriteBool2File_HasSep(const string& strModule,const string& strKey)
{
	if(m_bIsSepMode)
	{
		string strSepKey="sep"+strKey;
		YL_EncFileReg::WriteBool(strModule.c_str(),strSepKey.c_str(),m_mapUserConfig[ strModule ]->m_mapString2Bool_sep[strKey] )	;
	}
	else
	{
		WriteBool2File(strModule,strKey);
	}
}

bool AfxUserConfig::CheckIsSepMode()
{
	return false;
}
