#ifndef _YL_TAG_DEALER_H
#define _YL_TAG_DEALER_H

#ifdef	YL_TAGDEALER_EXPORT
#define YL_TAGDEALER_DLL __declspec(dllexport)
#else
#define YL_TAGDEALER_DLL __declspec(dllimport)
#endif

#include <string>
using std::string;

/**************************************************************************

//支持Tag格式：	ID3v1, ID3v2.3, ID3v2.4, iTunes MP4, WMA, Vorbis Comments and APE Tags.	

//支持的音频类型：wma,mp3,ape,flac,tta,ogg,mpc,mp+,speex,wavpack,wav,m4a,mp4	

**************************************************************************/
class YL_TAGDEALER_DLL YL_CTagDealer
{

public:

	struct MusicTag
	{
		string strTitle;			//歌名
		string strArtist;			//歌手
		string strAlbum;			//专辑
		string strGenerator;		//厂商（通常不用）
		string strYear;				//录制时间（通常不用）
		string strTrack;			//编号（通常不用）
		string strComment;			//注释
		int nKbps;					//码率
		unsigned int unLength;		//歌曲长度（以秒为单位）
	};
	struct MusicProperties
	{
		int nSampleRate;	   //采样率 in Hz
		int nLength;		   //长度	in seconds
		int nChannels;         //声道
		int nBitrate;          //码率	in kb/s
	};
	struct MusicTagEx
	{
		string strTitle;			//歌名
		string strArtist;			//歌手
		string strAlbum;			//专辑
		string strGenerator;		//厂商（通常不用）
		string strYear;				//录制时间（通常不用）
		string strTrack;			//编号（通常不用）
		string strComment;			//注释
		MusicProperties musicProperties; //音频属性
	};

public:

	//**************************************************************************//
	//																			//
	//	获得音乐文件tag信息														//
	//																			//
	//	参数:																	//
	//		pszFilePath	:	文件路径											//
	//		pMusicTag	:	传出的MusicTag信息									//
	//																			//
	//	返回值:																	//
	//			true	:	获取成功											//
	//			false	:	获取失败											//
	//																			//
	//**************************************************************************//
	static bool GetTag( const char* pszFilePath,MusicTag* pMusicTag );
	static bool GetTagEx( const char* pszFilePath,MusicTagEx* pMusicTagEx, bool bGetAudioProperties = false);
	
	static bool GetAudioProperty(const char * pszFilePath, MusicProperties * pMusicProperty);
	//**************************************************************************//
	//																			//
	//	保存音乐文件tag信息														//
	//	文件码率及歌曲长度不能被保存											//
	//																			//
	//	参数:																	//
	//		pszFilePath	:	文件路径											//
	//		pMusicTag	:	保存的MusicTag信息									//
	//																			//
	//	返回值:																	//
	//			true	:	保存成功											//
	//			false	:	保存失败											//
	//																			//
	//**************************************************************************//
	static bool SaveTag( const char* pszFiePath,const MusicTag* pMusicTag );

	static bool YL_CTagDealer::GetTag_Title		(const char* pszFilePath, string & strTitle);
	static bool YL_CTagDealer::GetTag_Artist	(const char* pszFilePath, string & strArtist);
	static bool YL_CTagDealer::GetTag_Album		(const char* pszFilePath, string & strAlbum);
	static bool YL_CTagDealer::GetTag_Year		(const char* pszFilePath, int & nYear);
	static bool YL_CTagDealer::GetTag_Year		(const char* pszFilePath, string & strYear);
	static bool YL_CTagDealer::GetTag_Comment	(const char* pszFilePath, string & strComment);
	static bool YL_CTagDealer::GetTag_Track		(const char* pszFilePath, int & nTrack);
	static bool YL_CTagDealer::GetTag_Track		(const char* pszFilePath, string & strTrack);
	static bool YL_CTagDealer::GetTag_Genre		(const char* pszFilePath, string & strGenre);

	static bool YL_CTagDealer::SaveTag_Title	( const char* pszFiePath, const char * pszTitle );
	static bool YL_CTagDealer::SaveTag_Artist	( const char* pszFiePath, const char * pszArtist );
	static bool YL_CTagDealer::SaveTag_Album	( const char* pszFiePath, const char * pszAlbum );
	static bool YL_CTagDealer::SaveTag_Year		( const char* pszFiePath, int nYear );
	static bool YL_CTagDealer::SaveTag_Year		( const char* pszFiePath, const char * pszYear );
	static bool YL_CTagDealer::SaveTag_Comment  ( const char* pszFiePath, const char * pszComment );
	static bool YL_CTagDealer::SaveTag_Track	( const char* pszFiePath, int nTarck );
	static bool YL_CTagDealer::SaveTag_Track	( const char* pszFiePath, const char * pszTrack );
	static bool YL_CTagDealer::SaveTag_Genre	( const char* pszFiePath, const char * pszGenre );

};

#endif