// fl.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "fl.h"
#include <shlobj.h>
#include "..\..\common\YL_FileInfo.h"
#include "..\..\common\YL_StringUtil.h"
#include "..\..\common\YL_RegInfo.h"
#include "..\..\common\Markup.h"
#include "..\..\common\tinyXML\tinyxml.h"

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

static void my1 (vector<string> & vStr)
{
	char szAppData[MAX_PATH] = {0};
	if (SHGetSpecialFolderPath (NULL, szAppData, CSIDL_COMMON_APPDATA, FALSE))
	{
		string strKWDataXml;
		YL_StringUtil::Format (strKWDataXml, "%s\\%s", szAppData, "kuaiwan\\data.xml");
		if (YL_FileInfo::IsValid (strKWDataXml))
		{
			CMarkup xml;
			if( !xml.Load(strKWDataXml.c_str()) )
				return;

			xml.ResetPos();
			xml.FindElem();
			xml.IntoElem();

			while( xml.FindChildElem() )
			{
				string strTmp = xml.GetAttrib ("path");
				string strPath = strTmp.substr (0, strTmp.find ("\\games"));

				vector<string>::iterator itFind = find (vStr.begin (), vStr.end (), strPath);

				if (itFind == vStr.end ())
					vStr.push_back (strPath);
			}
		}
	}
}

static void my2 (vector<string> & vStr)
{
#define KKKEY	"SOFTWARE\\KuaiKuai\\ksvs\\apps"

	char szSubKey [MAX_PATH] = {0};
	DWORD dwIndex = 0;

	HKEY pKey;
	if (::RegOpenKey (HKEY_LOCAL_MACHINE, KKKEY, &pKey) == ERROR_SUCCESS)
	{
		while (::RegEnumKey (pKey, dwIndex++, szSubKey, MAX_PATH) == ERROR_SUCCESS)
		{
			string strSubKey;
			YL_StringUtil::Format (strSubKey, "%s\\%s", KKKEY, szSubKey);

			string strProgramDir;
			YL_RegInfo::ReadString (HKEY_LOCAL_MACHINE, strSubKey.c_str (),	
				"program_dir", strProgramDir);

			if (find (vStr.begin (), vStr.end (), strProgramDir) == vStr.end ())
				vStr.push_back (strProgramDir);
		}
	}
}

static void UTF8ToGB(string &szOut)
{
	WCHAR *strSrc;
	TCHAR *szRes;
	//长度
	int i = MultiByteToWideChar(CP_UTF8, 0, szOut.c_str (), -1, NULL, 0);
	strSrc = new WCHAR[i+1];
	MultiByteToWideChar(CP_UTF8, 0, szOut.c_str (), -1, strSrc, i);

	//长度
	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new TCHAR[i+1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

	szOut = szRes;

	delete []strSrc;
	delete []szRes;
}

static void my3 (vector<string> & vStr)
{
#define DJPath "DianJi\\LavaNet DNA\\AppData\\dgs\\dgs.xml"

	char szCommonFiles[MAX_PATH] = {0};
	SHGetSpecialFolderPath (NULL, szCommonFiles, CSIDL_PROGRAM_FILES_COMMON, FALSE);

	string strPath;
	YL_StringUtil::Format (strPath, "%s\\%s", szCommonFiles, DJPath);

	TiXmlDocument doc;
	bool loadOkay = doc.LoadFile(strPath.c_str ());

	if (loadOkay )
	{
		TiXmlNode* node = 0;
		TiXmlElement* filesElement = 0;
		TiXmlElement* itemElement = 0;

		node = doc.FirstChild ("files");
		if (node != NULL)
		{
			filesElement = node->ToElement();
			if (filesElement != NULL)
			{
				node = filesElement->FirstChildElement();
				while (node != NULL)
				{
					itemElement = node->ToElement();
					if (itemElement != NULL)
					{
						const char * pPath = itemElement->Attribute ("path");
						string strValue (pPath);

						UTF8ToGB (strValue);

						if (find (vStr.begin (), vStr.end (), strValue) == vStr.end ())
							vStr.push_back (strValue);
					}

					node = itemElement->NextSiblingElement ();
				}
			}
		}
	}
}

static void my4 (vector<string> & vStr)
{
#define FTNN "SOFTWARE\\4399\\DownCore\\Downloads"

	string strDownPathTmp;
	if (YL_RegInfo::ReadString (HKEY_CURRENT_USER, FTNN, "CompletePath", strDownPathTmp))
	{
		string strDownPath = strDownPathTmp.substr (0, strDownPathTmp.rfind ("\\"));

		if (find (vStr.begin (), vStr.end (), strDownPath) == vStr.end ())
			vStr.push_back (strDownPath);
	}
}

int GetFL (vector<string> &vStr)
{
	vStr.clear ();

	my1 (vStr);
	my2 (vStr);
	my3 (vStr);
	my4 (vStr);

	return 1;
}

void GetFL2 (vector<string> & vStr)
{
	vStr.clear ();

	vStr.push_back ("windows");
	vStr.push_back ("winddk");
	vStr.push_back ("inetpub");
	vStr.push_back ("Documents and Settings");
	vStr.push_back ("4399");
	vStr.push_back ("kuaikuai");
	vStr.push_back ("kuaiwan");
	vStr.push_back ("dianji");
	vStr.push_back ("java");
	vStr.push_back ("Internet explorer");
	vStr.push_back ("adobe");
	vStr.push_back ("common files");
	vStr.push_back ("ksafe");
	vStr.push_back ("kwmusic");
	vStr.push_back ("microsoft");
	vStr.push_back ("msdn");
	vStr.push_back ("tencent");
	vStr.push_back ("thunder");
}
