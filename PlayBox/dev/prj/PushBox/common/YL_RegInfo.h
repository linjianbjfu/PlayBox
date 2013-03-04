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
	//	����һ������������Ѵ��ڣ������κβ���									//
	//																				//
	//	����: ����HKEY_LOCAL_MACHINE\SOFTWARE\KOOWO									//
	//	���� HKEY_LOCAL_MACHINE Ϊ��������hRegKey,������HKEY ����					//
	//		HKEY_CLASSES_ROOT														//
	//		HKEY_CURRENT_CONFIG														//
	//		HKEY_CURRENT_USER														//
	//		HKEY_PERFORMANCE_DATA													//
	//		HKEY_USERS																//
	//																				//
	//  SOFTWARE\KOOWO Ϊ��·��,��ע���չ������߲��ֵ�·��						//	
	//																				//
	//  ����:																		//
	//		hRegKey		:	����													//
	//		strRegEntry	:	��·��													//
	//		pszRegEntry	:	��·��													//	
	//																				//
	//	����ֵ��																	//
	//																				//
	//		true	:	�����ɹ���������Ѵ��ڣ�ͬ������true						//
	//		false	:	����ʧ��													//
	//																				//
	//******************************************************************************//
	static bool CreateKey( HKEY hRegKey, const string &strRegEntry );
	static bool CreateKey( HKEY hRegKey, const char* pszRegEntry );

	//******************************************************************************//
	//																				//
	//	ɾ��һ�������߼�·��������ü����·�������ڣ������κβ���				//
	//	���strKeyNameΪ�գ�����pszKeyNameΪ�գ���ɾ����·��������ɾ����			//
	//	���磺																		//	
	//	hRegKey ΪHKEY_LOCAL_MACHINE,												//
	//	strRegEntry ΪSOFTWARE\KOOWO,												//
	//	strKeyName	ΪInstallData													//
	//  ���ɾ��InstallData,���strKeyNameΪ�գ���ɾ��SOFTWARE\KOOWO				//
	//																				//
	//  ����:																		//
	//		hRegKey		:	����													//
	//		strRegEntry	:	��·��													//
	//		pszRegEntry	:	��·��													//
	//		strKeyName	:	��														//
	//		pszKeyName	:	��														//
	//																				//
	//	����ֵ��																	//
	//																				//
	//		true	:	ɾ���ɹ�������������ڣ�ͬ������true						//
	//		false	:	ɾ��ʧ��													//
	//																				//
	//******************************************************************************//
	static bool DeleteKey( HKEY hRegKey, const string &strRegEntry, const string& strKeyName = "");
	static bool DeleteKey( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName = 0);
	static LONG DeleteValue(HKEY regClass, const string &regEntry, const string &valueName);
	static LONG DeleteSubKey(HKEY regClass, const string &regEntry, const string &keyName);

	//******************************************************************************//
	//																				//
	//	��ע����д��string���ͼ�ֵ������ü������ڣ����ȴ����ü�,��д��ֵ			//
	//																				//
	//  ����:																		//
	//		hRegKey		:	����													//
	//		strRegEntry	:	��·��													//
	//		pszRegEntry	:	��·��													//
	//		strKeyName	:	��														//
	//		pszKeyName	:	��														//
	//		strKeyValue	:	����ļ�ֵ												//
	//		pszKeyValue :	����ļ�ֵ												//
	//																				//
	//	����ֵ��																	//
	//																				//
	//		true	:	д��ɹ�													//
	//		false	:	д��ʧ��													//
	//																				//
	//******************************************************************************//
	static bool WriteString(HKEY hRegKey, const string &strRegEntry, const string& strKeyName , const string& strKeyValue);
	static bool WriteString( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,const char* pszKeyValue);

	//******************************************************************************//
	//																				//
	//	��ȡע����string���ͼ�ֵ������ü������ڣ��򷵻�ʧ��						//
	//																				//
	//  ����:																		//
	//		hRegKey		:	����													//
	//		strRegEntry	:	��·��													//
	//		pszRegEntry	:	��·��													//
	//		strKeyName	:	��														//
	//		pszKeyName	:	��														//
	//		strKeyValue	:	���صļ�ֵ												//
	//		szBuffer	:	���صļ�ֵ												//
	//		unBufferLen	:	Buffer����												//
	//																				//
	//	����ֵ��																	//
	//																				//
	//		true	:	����ɹ�													//
	//		false	:	����ʧ��													//
	//																				//
	//******************************************************************************//
	static bool ReadString(HKEY hRegKey, const string &strRegEntry, const string& strKeyName , string& strKeyValue);
	static bool ReadString( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,char szBuffer[],unsigned int unBuffLen);

	//******************************************************************************//
	//																				//
	//	��ע����д���޷����������ͼ�ֵ������ü������ڣ����ȴ����ü�,��д��ֵ		//
	//																				//
	//  ����:																		//
	//		hRegKey		:	����													//
	//		strRegEntry	:	��·��													//
	//		pszRegEntry	:	��·��													//
	//		strKeyName	:	��														//
	//		pszKeyName	:	��														//
	//		unValue		:	����ļ�ֵ												//	
	//																				//
	//	����ֵ��																	//
	//																				//
	//		true	:	д��ɹ�													//
	//		false	:	д��ʧ��													//
	//																				//
	//******************************************************************************//
	static bool WriteDWORD(HKEY hRegKey, const string &strRegEntry, const string& strKeyName ,const unsigned int& unValue);
	static bool WriteDWORD( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,const unsigned int& unValue);

	//******************************************************************************//
	//																				//
	//	��ȡע�����޷����������ͼ�ֵ������ü������ڣ��򷵻�ʧ��					//
	//																				//
	//  ����:																		//
	//		hRegKey		:	����													//
	//		strRegEntry	:	��·��													//
	//		pszRegEntry	:	��·��													//
	//		strKeyName	:	��														//
	//		pszKeyName	:	��														//
	//		unValue		:	���صļ�ֵ												//
	//																				//
	//	����ֵ��																	//
	//																				//
	//		true	:	����ɹ�													//
	//		false	:	����ʧ��													//
	//																				//
	//******************************************************************************//
	static bool ReadDWORD(HKEY hRegKey, const string &strRegEntry, const string& strKeyName , unsigned int& unValue);
	static bool ReadDWORD( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,unsigned int& unValue);

};


#endif