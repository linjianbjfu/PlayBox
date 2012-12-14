#ifndef _YL_RECO_SONG_H
#define _YL_RECO_SONG_H


#ifdef	YL_RECOSONG_EXPORT
#define YL_RECOSONG_DLL __declspec(dllexport)
#else
#define YL_RECOSONG_DLL __declspec(dllimport)
#endif

#include <string>
#include <vector>
using std::string;
using std::vector;

struct RECO_SONG_INFO
{
	string	strSongTitle;
	string	strSongArtist;
	string	strSongAlbum;
	bool	bSucceed;
};

struct RECO_RESULT
{
	int           iRecoBySig;//0:fail,1:sig,2:fp
	unsigned int  unSig1;
	unsigned int  unSig2;
	unsigned int  fp[256];
	int			  iSongID;

	RECO_RESULT()
	{
		iRecoBySig = 0;
		unSig1     = 0;
		unSig2	   = 0;
		memset(fp,0,256*sizeof(unsigned int));
		iSongID    = 0;
	}
};
class YL_RECOSONG_DLL YL_CRecoSong
{
public:

	static void	SetInfo( const char* pszRecoServer,
						 const char* pszID = NULL,
						 const char* pszSrc = NULL,
						 const char* pszSoftware = NULL );

	static bool RecoSongs2(	const vector<string>& vecInputSong,vector<RECO_SONG_INFO>& vecRecoSongInfo,vector<RECO_RESULT>* vecResult);
	static bool RecoSongs(	const vector<string>& vecInputSong,vector<RECO_SONG_INFO>& vecRecoSongInfo);

	static bool RecoSongByFp( const string& strInpuSong,RECO_SONG_INFO& recoInfo,unsigned int* fp,bool bNeedFp);

	static void GetFPTimeAndInfo(int* pnCalFP,int* pnSvrConnect,int* pnSvrRead,int* pnFPSvrData,bool* pFPSvrSuc );
};

#endif