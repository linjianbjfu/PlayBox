//¥¶¿Ìlylk.dat, mylk.dat

#include "windows.h"

class YL_UserId
{
public:
	static bool GetUserFileName(char szFileName[], int namelen);
	static bool GetUserFilePath(char szFilePath[], int pathlen);
	static bool GetUserID(char szID[], int idlen);
	static bool SetUserID(const char *szID);
	static bool GetInstallSRC(char szSRC[], int srclen);
	static bool SetInstallSRC(const char* szSRC);	
};
