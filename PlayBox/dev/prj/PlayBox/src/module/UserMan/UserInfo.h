#ifndef  USER_INFO_H
#define USER_INFO_H
#pragma once
#include <string>

struct UserInfo
{
	int iID;
	std::string strName;
	std::string strPassMD5;
	UserInfo() : iID(-1) {}
	void clear()
	{
		iID = -1;
		strName.clear();
		strPassMD5.clear();
	}
};

#endif //USER_INFO_H