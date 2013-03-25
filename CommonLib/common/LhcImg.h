//const.h
#pragma once
#include "globalparas.h"
#include <string>
using namespace std;

class CLhcImg
{
public:
	static bool GetHomePath(char szPath[], int pathlen);
	static bool GetUpdateServer(char szSvr[], int svrlen);
	static bool GetLogServer(char szUrl[], int urllen);
	static bool GetConfigServer(char szUrl[], int urllen);
};