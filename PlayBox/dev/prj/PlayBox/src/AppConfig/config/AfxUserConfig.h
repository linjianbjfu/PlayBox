#ifndef _AFX_USER_CONFIG_H
#define _AFX_USER_CONFIG_H

#include "YL_Ini.h"

#include <map>
#include <string>
#include <list>

using std::map;
using std::string;
using std::list;


class CConfigObject
{
public:
	CConfigObject()
	{
		m_mapString2String.clear();
		m_mapString2Bool.clear();
		m_mapString2Int.clear();
		m_mapString2String_sep.clear();
		m_mapString2Bool_sep.clear();
		m_mapString2Int_sep.clear();
	}
public:
	map<string,int>		m_mapString2Int;
	map<string,string>	m_mapString2String;
	map<string,bool>	m_mapString2Bool;
	map<string,int>		m_mapString2Int_sep;
	map<string,string>	m_mapString2String_sep;
	map<string,bool>	m_mapString2Bool_sep;

};

class AfxUserConfig_Observer
{
public:
	virtual void	OnConfigChange( const string& strKey ) = 0;
};

class AfxUserConfig
{
private:
	
	map<string,CConfigObject*>					m_mapUserConfig;

	map<string,list<AfxUserConfig_Observer*> >	m_mapObservers;

	CRITICAL_SECTION	m_cs;
	CRITICAL_SECTION	m_Configcs;

	YL_Ini				m_ylIni;
	BOOL				m_bIsSepMode;
public:

	AfxUserConfig();
	~AfxUserConfig();
	
public:

	void	Config_AppStart();
	void	Config_AppExit();

	void	AttachConfigMessage( const string& strModule,AfxUserConfig_Observer* pObserver );
	void	DetachConfigMessage( const string& strMolude,AfxUserConfig_Observer* pObserver );

	void	GetConfigStringValue( const string& strModule,const string& strKey,string& strValue);
	void	SetConfigStringValue(const string& strModule,const string& strKey,const string& strValue,bool bWrite = false);

	void	GetConfigIntValue( const string& strModule,const string& strKey,int& iValue);
	void	SetConfigIntValue(const string& strModule,const string& strKey,int iValue,bool bWrite = false);

	void	GetConfigDoubleValue( const string& strModule,const string& strKey,double& dValue);
	void	SetConfigDoubleValue(const string& strModule,const string& strKey,double dValue,bool bWrite = false);

	void	GetConfigBoolValue( const string& strModule,const string& strKey,bool& bValue);
	void	SetConfigBoolValue(const string& strModule,const string& strKey,bool bValue,bool bWrite = false);

private:

	void	NotifyObserver(const string& strModule,const string& strKey );
	void	AddConfigObject( const char* pszModuleName,const char** ppConfig,int iConfigItemNum);

	void	Load();
	void	Save();

	void	GetStringFromFile(const string& strModule,const string& strKey );
	void	GetStringFromFile_HasSep(const string& strModule,const string& strKey );
	void	WriteString2File( const string& strModule,const string& strKey );
	void	WriteString2File_HasSep( const string& strModule,const string& strKey );

	void	GetIntFromFile( const string& strModule,const string& strKey );
	void	GetIntFromFile_HasSep( const string& strModule,const string& strKey );
	void	WriteInt2File( const string& strModule,const string& strKey );
	void	WriteInt2File_HasSep( const string& strModule,const string& strKey );

	void	GetDoubleFromFile( const string& strModule,const string& strKey );
	void	GetDoubleFromFile_HasSep( const string& strModule,const string& strKey );
	void	WriteDouble2File( const string& strModule,const string& strKey );
	void	WriteDouble2File_HasSep( const string& strModule,const string& strKey );

	void	GetBoolFromFile( const string& strModule,const string& strKey );
	void	GetBoolFromFile_HasSep( const string& strModule,const string& strKey );
	void	WriteBool2File( const string& strModule,const string& strKey );
	void	WriteBool2File_HasSep( const string& strModule,const string& strKey );

	bool	CheckIsSepMode();
};

#endif