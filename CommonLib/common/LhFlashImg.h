//const.h
#pragma once
#include <string>
using namespace std;

class CLhFlashImg
{
public:
	static bool GetHomePath(char szPath[], int pathlen);
	static bool GetSoftwareVersion(string& strVer);
	static bool GetAdiniPath(string &strFile);
	static bool CheckValidIndex(unsigned int nGroup, unsigned int nIndex);

	// flash µ¯¿ò¹ã¸æ
	static unsigned int GetStartTimer();
	static unsigned int GetFlashMessageNum();
	static unsigned int GetFlashCandidateNum(unsigned int nGroup);
	static unsigned int GetFlashMessageTimer(unsigned int nGroup, unsigned int nIndex);
	static bool GetFlashMessageFile(unsigned int nGroup, unsigned int nIndex, string& strFile);
	static bool GetFlashMessageSize(unsigned int nGroup, unsigned int nIndex, int& nWidth, int& nHeight);
	static unsigned int GetFlashMessageWeight(unsigned int nGroup, unsigned int nIndex);
	static bool GetFlashMessageId(unsigned int nGroup, unsigned int nIndex, string &strId);

	static bool SelectCandidate(unsigned int nUserId, unsigned int nGroup, unsigned int &nIndex);
	static bool GetFlashMsgInfo(unsigned int nUserId, unsigned int nGroup, int &top, int &left, int &nWidth, int &nHeight, unsigned int &nTimer, string &strId, string & strFile);

	static int GetFlashPopDate();
	static bool SetFlashPopDate(unsigned int nDate);
	static int GetFlashPopGroupIndex();
	static bool SetFlashPopGroupIndex(unsigned int nIndex);
};

