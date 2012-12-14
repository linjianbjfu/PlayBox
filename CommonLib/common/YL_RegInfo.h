#ifndef _YL_REG_INFO_H
#define _YL_REG_INFO_H

#include "windows.h"
#include <string>
using std::string;

class YL_RegInfo
{

public:

	//******************************************************************************//
	//																				//
	//	创建一个键，如果键已存在，则不做任何操作									//
	//																				//
	//	例如: 创建HKEY_LOCAL_MACHINE\SOFTWARE\KOOWO									//
	//	其中 HKEY_LOCAL_MACHINE 为主键，即hRegKey,其他的HKEY 包括					//
	//		HKEY_CLASSES_ROOT														//
	//		HKEY_CURRENT_CONFIG														//
	//		HKEY_CURRENT_USER														//
	//		HKEY_PERFORMANCE_DATA													//
	//		HKEY_USERS																//
	//																				//
	//  SOFTWARE\KOOWO 为键路径,即注册表展开的左边部分的路径						//	
	//																				//
	//  参数:																		//
	//		hRegKey		:	主键													//
	//		strRegEntry	:	键路径													//
	//		pszRegEntry	:	键路径													//	
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	创建成功，如果键已存在，同样返回true						//
	//		false	:	创建失败													//
	//																				//
	//******************************************************************************//
	static bool CreateKey( HKEY hRegKey, const string &strRegEntry );
	static bool CreateKey( HKEY hRegKey, const char* pszRegEntry );

	//******************************************************************************//
	//																				//
	//	删除一个键或者键路径，如果该键或键路径不存在，则不做任何操作				//
	//	如果strKeyName为空，或者pszKeyName为空，则删除键路径，否则删除键			//
	//	例如：																		//	
	//	hRegKey 为HKEY_LOCAL_MACHINE,												//
	//	strRegEntry 为SOFTWARE\KOOWO,												//
	//	strKeyName	为InstallData													//
	//  则会删除InstallData,如果strKeyName为空，则删除SOFTWARE\KOOWO				//
	//																				//
	//  参数:																		//
	//		hRegKey		:	主键													//
	//		strRegEntry	:	键路径													//
	//		pszRegEntry	:	键路径													//
	//		strKeyName	:	键														//
	//		pszKeyName	:	键														//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	删除成功，如果键不存在，同样返回true						//
	//		false	:	删除失败													//
	//																				//
	//******************************************************************************//
	static bool DeleteKey( HKEY hRegKey, const string &strRegEntry, const string& strKeyName = "");
	static bool DeleteKey( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName = 0);
	static LONG DeleteValue(HKEY regClass, const string &regEntry, const string &valueName);
	static LONG DeleteSubKey(HKEY regClass, const string &regEntry, const string &keyName);

	//******************************************************************************//
	//																				//
	//	对注册表键写入string类型键值，如果该键不存在，则先创建该键,再写入值			//
	//																				//
	//  参数:																		//
	//		hRegKey		:	主键													//
	//		strRegEntry	:	键路径													//
	//		pszRegEntry	:	键路径													//
	//		strKeyName	:	键														//
	//		pszKeyName	:	键														//
	//		strKeyValue	:	具体的键值												//
	//		pszKeyValue :	具体的键值												//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	写入成功													//
	//		false	:	写入失败													//
	//																				//
	//******************************************************************************//
	static bool WriteString(HKEY hRegKey, const string &strRegEntry, const string& strKeyName , const string& strKeyValue);
	static bool WriteString( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,const char* pszKeyValue);

	//******************************************************************************//
	//																				//
	//	读取注册表键string类型键值，如果该键不存在，则返回失败						//
	//																				//
	//  参数:																		//
	//		hRegKey		:	主键													//
	//		strRegEntry	:	键路径													//
	//		pszRegEntry	:	键路径													//
	//		strKeyName	:	键														//
	//		pszKeyName	:	键														//
	//		strKeyValue	:	返回的键值												//
	//		szBuffer	:	返回的键值												//
	//		unBufferLen	:	Buffer长度												//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	读入成功													//
	//		false	:	读入失败													//
	//																				//
	//******************************************************************************//
	static bool ReadString(HKEY hRegKey, const string &strRegEntry, const string& strKeyName , string& strKeyValue);
	static bool ReadString( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,char szBuffer[],unsigned int unBuffLen);

	//******************************************************************************//
	//																				//
	//	对注册表键写入无符号整数类型键值，如果该键不存在，则先创建该键,再写入值		//
	//																				//
	//  参数:																		//
	//		hRegKey		:	主键													//
	//		strRegEntry	:	键路径													//
	//		pszRegEntry	:	键路径													//
	//		strKeyName	:	键														//
	//		pszKeyName	:	键														//
	//		unValue		:	具体的键值												//	
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	写入成功													//
	//		false	:	写入失败													//
	//																				//
	//******************************************************************************//
	static bool WriteDWORD(HKEY hRegKey, const string &strRegEntry, const string& strKeyName ,const unsigned int& unValue);
	static bool WriteDWORD( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,const unsigned int& unValue);

	//******************************************************************************//
	//																				//
	//	读取注册表键无符号整数类型键值，如果该键不存在，则返回失败					//
	//																				//
	//  参数:																		//
	//		hRegKey		:	主键													//
	//		strRegEntry	:	键路径													//
	//		pszRegEntry	:	键路径													//
	//		strKeyName	:	键														//
	//		pszKeyName	:	键														//
	//		unValue		:	返回的键值												//
	//																				//
	//	返回值：																	//
	//																				//
	//		true	:	读入成功													//
	//		false	:	读入失败													//
	//																				//
	//******************************************************************************//
	static bool ReadDWORD(HKEY hRegKey, const string &strRegEntry, const string& strKeyName , unsigned int& unValue);
	static bool ReadDWORD( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,unsigned int& unValue);

};


#endif