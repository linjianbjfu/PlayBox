#ifndef _JUDGE_FIREWALL_H
#define _JUDGE_FIREWALL_H


enum HTTP_REQUEST_TYPE
{
	HTTP_WEB_COMMAND,
	HTTP_SEARCH,	
};


class CJudgeFirewall
{
public:
	static void	SetHTTPRes( HTTP_REQUEST_TYPE hrt,int iRes );
};



#endif