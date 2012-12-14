#pragma once

#include <string>
#include <vector>
using namespace std;

class CPlayerConfig
{
public:
	//获取可用的播放器列表
	vector<string>& GetExistPlayerList();

public:
	//读winamp的路径
	void GetWinamp(string & str);

	//读mediaplayer的路径
	void GetMediaPlayer(string & str);

	//读realplayer的路径
	void GetRealPlayer(string & str);

	//读foobar的路径
	void GetFoobar(string & str);

	//读千千静听的路径
	void GetQian(string & str);

	//读KMPlayer的路径
	void GetKMPlayer(string & str);

	//读Storm Codec的路径
	void GetStorm(string & str);

protected:
	//播放器列表
	vector<string> m_playerList;

public:
	CPlayerConfig(void);
	~CPlayerConfig(void);
};
