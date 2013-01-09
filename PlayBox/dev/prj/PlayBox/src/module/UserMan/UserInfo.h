#ifndef  USER_INFO_H
#define USER_INFO_H
#pragma once
#include <string>

struct UserInfo
{
	int iID;
	std::string strName;
	std::string strPassMD5;
};

#endif //USER_INFO_H