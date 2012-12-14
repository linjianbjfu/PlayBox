#pragma once

#include <string>
#include <vector>
using namespace std;

class CPlayerConfig
{
public:
	//��ȡ���õĲ������б�
	vector<string>& GetExistPlayerList();

public:
	//��winamp��·��
	void GetWinamp(string & str);

	//��mediaplayer��·��
	void GetMediaPlayer(string & str);

	//��realplayer��·��
	void GetRealPlayer(string & str);

	//��foobar��·��
	void GetFoobar(string & str);

	//��ǧǧ������·��
	void GetQian(string & str);

	//��KMPlayer��·��
	void GetKMPlayer(string & str);

	//��Storm Codec��·��
	void GetStorm(string & str);

protected:
	//�������б�
	vector<string> m_playerList;

public:
	CPlayerConfig(void);
	~CPlayerConfig(void);
};
