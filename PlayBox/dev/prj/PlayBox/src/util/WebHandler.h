#ifndef _WEB_HANDLER_H
#define _WEB_HANDLER_H

class CWebHandler
{
public:
	CWebHandler();
	~CWebHandler();
	static CWebHandler*		m_pWebHandler;
	static CWebHandler*		GetInstance();
	static void				DelInstance();

	void	OnPlay(const char* );
	void	OnPlayWebGame(const char* );
};

#endif