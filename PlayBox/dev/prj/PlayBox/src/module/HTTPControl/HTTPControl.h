#ifndef _HTTP_CONTROL_H_JRK
#define _HTTP_CONTROL_H_JRK

class CHTTPControl
{
public:
	static  void	Init();
	static  void	Close();

	static	void	HTTPWait_TodayRecommend();
	static	void	HTTPWait_BangDan();
	static	void	HTTPWait_AD();
public:
	static  void	HTTPFinish_TodayRecommend();
	static  void	HTTPFinish_RegID();
	static	void	HTTPFinish_ConfigFile();
	static  void	HTTPFinish_BangDan();
	static  void	HTTPFinish_UpClientLog();
	static  void	HTTPFinish_DownAD();
};

#endif