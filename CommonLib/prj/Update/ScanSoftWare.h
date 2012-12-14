
#define STR_ANTI_KAPA6_PROC       "avp.exe"
#define STR_ANTI_KAPA5_PROC       "kav.exe"
#define STR_ANTI_KAPA_NAME        "KASP"

#define STR_ANTI_NOR_PROC         "rtvscan.exe"
#define STR_ANTI_NOR_NAME         "NOR"
#define STR_ANTI_NORFW_PROC       "nscsrvce.exe"
#define STR_ANTI_NORFW_NAME       "NORFW"

#define STR_ANTI_SKY_PROC         "pfwmain.exe"
#define STR_ANTI_SKY_NAME         "SKY"

#define STR_ANTI_KING_PROC        "kwatch.exe"
#define STR_ANTI_KING_NAME        "KING"
#define STR_ANTI_KINGFW_PROC      "kavpfw.exe"
#define STR_ANTI_KINGFW_NAME      "KINGFW"

#define STR_ANTI_KV_PROC          "kvsrvxp.exe"
#define STR_ANTI_KV_NAME          "KV"
#define STR_ANTI_KVFW_PROC        "kvfw.exe"
#define STR_ANTI_KVFW_NAME        "KVFW"

#define STR_ANTI_RIS_PROC         "ravmon.exe"
#define STR_ANTI_RIS_NAME         "RIS"
#define STR_ANTI_RISFW_PROC       "rfwmain.exe"
#define STR_ANTI_RISFW_NAME       "RISFW"

#define STR_ANTI_IPARM_PROC       "iparmor.exe"
#define STR_ANTI_IPARM_NAME       "IPARM"

/**
* 获得用户安装的杀毒软件列表.
* 
*/
string GetAntiVursSofts()
{
	string strRet, strAntis = "A:";
	HANDLE hSnapshot;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	BOOL bProcess = Process32First(hSnapshot, &pe32);
	while(bProcess)
	{
		CString AgentPath = pe32.szExeFile;
		AgentPath.MakeLower();
		if(AgentPath.Find(STR_ANTI_KAPA5_PROC) != -1 || AgentPath.Find(STR_ANTI_KAPA6_PROC) != -1)
		{
			strAntis = strAntis + STR_ANTI_KAPA_NAME + ",";
		}
		else if(AgentPath.Find(STR_ANTI_KING_PROC) != -1)
		{
			strAntis = strAntis + STR_ANTI_KING_NAME + ",";
		}
		else if(AgentPath.Find(STR_ANTI_KINGFW_PROC) != -1)
		{
			strAntis = strAntis + STR_ANTI_KINGFW_NAME + ",";
		}
		else if(AgentPath.Find(STR_ANTI_KV_PROC) != -1)
		{
			strAntis = strAntis + STR_ANTI_KV_NAME + ",";
		}
		else if(AgentPath.Find(STR_ANTI_KVFW_PROC) != -1)
		{
			strAntis = strAntis + STR_ANTI_KVFW_NAME + ",";
		}
		else if(AgentPath.Find(STR_ANTI_NOR_PROC) != -1)
		{
			strAntis = strAntis + STR_ANTI_NOR_NAME + ",";
		}
		else if(AgentPath.Find(STR_ANTI_NORFW_PROC) != -1)
		{
			strAntis = strAntis + STR_ANTI_NORFW_NAME + ",";
		}
		else if(AgentPath.Find(STR_ANTI_RIS_PROC) != -1)
		{
			strAntis = strAntis + STR_ANTI_RIS_NAME + ",";
		}
		else if(AgentPath.Find(STR_ANTI_RISFW_PROC) != -1)
		{
			strAntis = strAntis + STR_ANTI_RISFW_NAME + ",";
		}
		else if(AgentPath.Find(STR_ANTI_SKY_PROC) != -1)
		{
			strAntis = strAntis + STR_ANTI_SKY_NAME + ",";
		}
		else if(AgentPath.Find(STR_ANTI_IPARM_PROC) != -1)
		{
			strAntis = strAntis + STR_ANTI_IPARM_NAME + ",";
		}
		bProcess = Process32Next(hSnapshot, &pe32);
	}
	if(strAntis[strAntis.length()-1] == ',')
		strRet = strAntis.substr(0, strAntis.length()-1);
	else
		strRet = strAntis;
	return strRet;
}