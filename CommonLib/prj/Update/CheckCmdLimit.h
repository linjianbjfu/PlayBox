
#define CMD_CHECK_FAIL            2
#define CMD_CHECK_FALSE           1
#define CMD_CHECK_OK              0

bool CheckTimeStamp(const char* szRegValName, unsigned int nInterval);
int CheckDailyLimit(const char* szLastDateKey, const char* szTimesKey, unsigned int nTimesLimit);
int CheckTimesLimit(unsigned int nIndex);
int CheckCmdLimit(unsigned int nIndex);
