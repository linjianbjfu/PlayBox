//¥¶¿Ìlylk.dat, mylk.dat

#include "windows.h"
#include <string>

class YL_UserId
{
public:
	static LPCSTR USER_ID;
	static LPCSTR INSTALL_SRC;
	static LPCSTR VERSION;
	static bool Set(LPCSTR key, LPCSTR value);
	static bool Get(LPCSTR key, std::string& strValue);
private:
	static bool GetUserFilePath(std::string& strPath);
};
