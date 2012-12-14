#pragma once
#include <string>
using std::string;

class ConfigParaDefault
{
public:
	static string GetDefaultAPP_SWF_PATH();
	//
	// added by wang
	//
	static DWORD GetDefaultAPP_Dwon_Number();
	static string GetDefaultAPP_PicSave_Path();
	static BOOL GetDefaultAPP_IS_Pop_Up_Dialog_Box();
	static bool IsPathValid( string& strPath );
	
};
