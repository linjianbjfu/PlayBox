//#include "StdAfx.h"
#include "PlayerConfig.h"

//#include "ConfigReader.h"
#include "../../common/version.h"
#include "../../common/tools.h"
#include "../../common/LhcImg.h"
#include "../../common/YL_FileInfo.h"
#include "../../common/YL_DirInfo.h"
#include "../../common/Functions.h"

CPlayerConfig::CPlayerConfig(void)
{
}

CPlayerConfig::~CPlayerConfig(void)
{
}

//��ȡ���õĲ������б�
vector<string>& CPlayerConfig::GetExistPlayerList()
{
	//����б�
	m_playerList.clear();

	//�����жϸ��������Ƿ����
	string str;
	str = "";
	GetWinamp(str);
	if (str != "")
	{
		m_playerList.insert(m_playerList.end(), str);
	}

	str = "";
	GetMediaPlayer(str);
	if (str != "")
	{
		m_playerList.insert(m_playerList.end(), str);
	}

	str = "";
	GetRealPlayer(str);
	if (str != "")
	{
		m_playerList.insert(m_playerList.end(), str);
	}

	str = "";
	GetFoobar(str);
	if (str != "")
	{
		m_playerList.insert(m_playerList.end(), str);
	}

	str = "";
	GetQian(str);
	if (str != "")
	{
		m_playerList.insert(m_playerList.end(), str);
	}

	str = "";
	GetKMPlayer(str);
	if (str != "")
	{
		m_playerList.insert(m_playerList.end(), str);
	}

	str = "";
	GetStorm(str);
	if (str != "")
	{
		m_playerList.insert(m_playerList.end(), str);
	}
	//�����б�
	return m_playerList;
}
//��KMPlayer��·��
void CPlayerConfig::GetKMPlayer(string & str)
{
	str = "";
	//char tmp[MAX_PATH];
	//memset(tmp, 0, MAX_PATH);
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\KMPlayer",0, KEY_READ, &hKey) == ERROR_SUCCESS) 
	{
		str = "K2";
		RegCloseKey(hKey);
	}else
	{
		str = "";
	}
}
//��Storm Codec��·��
void CPlayerConfig::GetStorm(string & str)
{
	str = "";
	//char tmp[MAX_PATH];
	//memset(tmp, 0, MAX_PATH);
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_LANGHUA_ROOT, "Software\\Storm Codec",0, KEY_READ, &hKey) == ERROR_SUCCESS) 
	{
		str = string("S");
		RegCloseKey(hKey);
	}else
	{
		str = "";
	}
}
//��winamp��·��
void CPlayerConfig::GetWinamp(string & str)
{
	str = "";
	char tmp[MAX_PATH];
	//memset(tmp, 0, MAX_PATH);
	REG_GET_STRING(HKEY_CURRENT_USER, "Software\\Winamp", "", tmp, MAX_PATH);
	if (tmp[0] != 0)
	{
		//str = str.substr( 1 , str.find("\"", 1) - 1 );
		str = "W5";
	}
	else
	{
		REG_GET_STRING(HKEY_LANGHUA_ROOT, "Software\\Winamp", "", tmp, MAX_PATH);
		if (tmp[0] != 0)
		{
			str = "W2";
		}
		else
		{
			REG_GET_STRING(HKEY_LANGHUA_ROOT, "Winamp.File\\shell\\open\\command", "", tmp, MAX_PATH);
			if(tmp[0] != 0)
			{
				str = "W";
			}else
			{
				str = "";
			}			
		}
	}
}

//��mediaplayer��·��
void CPlayerConfig::GetMediaPlayer(string & str)
{
	str = "";
	char tmp[MAX_PATH];
	//memset(tmp, 0, MAX_PATH);
	REG_GET_STRING(HKEY_CLASSES_ROOT, "Applications\\wmplayer.exe\\shell\\open\\command", "", tmp, MAX_PATH);
	if (tmp[0] != 0)
	{
		//str = str.substr( 0 , str.find(" /Open") );
		str = "M";
		REG_GET_STRING(HKEY_LANGHUA_ROOT, "SOFTWARE\\Microsoft\\MediaPlayer\\11.0\\Registration", "UDBVersion", tmp, MAX_PATH);
		if (tmp[0] != 0){
			str += "11";
		}
		else{
			REG_GET_STRING(HKEY_LANGHUA_ROOT, "SOFTWARE\\Microsoft\\MediaPlayer\\10.0\\Registration", "UDBVersion", tmp, MAX_PATH);
			if (tmp[0] != 0){
				str += "10";
			}
			else{
				REG_GET_STRING(HKEY_LANGHUA_ROOT, "SOFTWARE\\Microsoft\\MediaPlayer\\9.0\\Registration", "UDBVersion", tmp, MAX_PATH);
				if (tmp[0] != 0){
					str += "9";
				}
				else{
					REG_GET_STRING(HKEY_LANGHUA_ROOT, "SOFTWARE\\Microsoft\\MediaPlayer\\7.0\\Registration", "UDBVersion", tmp, MAX_PATH);
					if (tmp[0] != 0){
						str += "7";
					}
					else{
						str = "";
					}
				}
			}
		}
	}
}

//��realplayer��·��
void CPlayerConfig::GetRealPlayer(string & str)
{
	//��ȡreal�汾������汾����realone��realplayer10�ͽ��Ѿ���װ��real���ж��
	char baseDir[MAX_PATH];
	memset(baseDir, 0, MAX_PATH);
	REG_GET_STRING(HKEY_CLASSES_ROOT, ".rm", "", baseDir, MAX_PATH);
	if(strlen(baseDir) != 0)//.rm�ж�Ӧ��Ӧ�ó���
	{
		//RealPlayer.RM.6
		char realPath[MAX_PATH];
		memset(realPath, 0, MAX_PATH);
		string key = string(baseDir) + "\\shell\\open\\command";
		REG_GET_STRING(HKEY_CLASSES_ROOT, key.c_str(), "", realPath, MAX_PATH);
		//�ҵ�realplay.exe��·��
		//"C:\Program Files\Real\RealPlayer\RealPlay.exe" "%1"
		char* endPos = strrchr(realPath, '\\');
		char* startPos = strstr(realPath, "\"");
		if(endPos != NULL && startPos != NULL)
		{
			char nBaseDir[MAX_PATH];
			memset(nBaseDir, 0, MAX_PATH);
			*endPos = 0;
			_snprintf(nBaseDir, MAX_PATH-1, "%s", startPos+1);
			string fileName = string(nBaseDir) + "\\RealPlay.exe";
			try
			{
				version ver(fileName);
				string verStr = ver.get_product_version();
				//realplayer10--6.0.12.
				//realone--6.0.11
				//realplayer8--6.0.9
				if(verStr.find("6.0.12.") != string::npos)
				{
					str = "R10";
				}
				else if(verStr.find("6.0.11.") != string::npos)
				{
					str = "RONE";
				}
				else if(verStr.find("6.0.9.") != string::npos)
				{
					str = "R8";
					//KWLyric_real.rpv��֧��real8�����Ҫɾ����
					string progDir;
					YL_DirInfo::GetProgramFilesDir(progDir);
					ATL::CString fileName;
					fileName.Format("%s\\Common Files\\Real\\Visualizations\\KWLyric_real.rpv", 
						progDir.c_str());
					MoveFileDelayAbPath(fileName);
				}
			}
			catch(std::exception e)
			{
			}
		}
	}
}


//��foobar��·��
void CPlayerConfig::GetFoobar(string & str)
{
	str = "";
	char tmp[MAX_PATH];
	//memset(tmp, 0, MAX_PATH);
	REG_GET_STRING(HKEY_CLASSES_ROOT, "Applications\\foobar2000.exe\\shell\\open\\command", "", tmp, MAX_PATH);
	str = tmp;
	if (str != "")
	{
		//str = str.substr( 1 , str.find("\"", 1) - 1 );
		str = "F";
	}
}

//��ǧǧ������·��
void CPlayerConfig::GetQian(string & str)
{
	str = "";
	char tmp[MAX_PATH];
	//memset(tmp, 0, MAX_PATH);
	REG_GET_STRING(HKEY_LANGHUA_ROOT, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\TTPlayer.exe", "", tmp, MAX_PATH);
	str = tmp;
	if (str != "")
	{
		str = "T";
	}
}