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

//֧��Tag��ʽ��	ID3v1, ID3v2.3, ID3v2.4, iTunes MP4, WMA, Vorbis Comments and APE Tags.	

//֧�ֵ���Ƶ���ͣ�wma,mp3,ape,flac,tta,ogg,mpc,mp+,speex,wavpack,wav,m4a,mp4	

**************************************************************************/
class YL_TAGDEALER_DLL YL_CTagDealer
{

public:

	struct MusicTag
	{
		string strTitle;			//����
		string strArtist;			//����
		string strAlbum;			//ר��
		string strGenerator;		//���̣�ͨ�����ã�
		string strYear;				//¼��ʱ�䣨ͨ�����ã�
		string strTrack;			//��ţ�ͨ�����ã�
		string strComment;			//ע��
		int nKbps;					//����
		unsigned int unLength;		//�������ȣ�����Ϊ��λ��
	};
	struct MusicProperties
	{
		int nSampleRate;	   //������ in Hz
		int nLength;		   //����	in seconds
		int nChannels;         //����
		int nBitrate;          //����	in kb/s
	};
	struct MusicTagEx
	{
		string strTitle;			//����
		string strArtist;			//����
		string strAlbum;			//ר��
		string strGenerator;		//���̣�ͨ�����ã�
		string strYear;				//¼��ʱ�䣨ͨ�����ã�
		string strTrack;			//��ţ�ͨ�����ã�
		string strComment;			//ע��
		MusicProperties musicProperties; //��Ƶ����
	};

public:

	//**************************************************************************//
	//																			//
	//	��������ļ�tag��Ϣ														//
	//																			//
	//	����:																	//
	//		pszFilePath	:	�ļ�·��											//
	//		pMusicTag	:	������MusicTag��Ϣ									//
	//																			//
	//	����ֵ:																	//
	//			true	:	��ȡ�ɹ�											//
	//			false	:	��ȡʧ��											//
	//																			//
	//**************************************************************************//
	static bool GetTag( const char* pszFilePath,MusicTag* pMusicTag );
	static bool GetTagEx( const char* pszFilePath,MusicTagEx* pMusicTagEx, bool bGetAudioProperties = false);
	
	static bool GetAudioProperty(const char * pszFilePath, MusicProperties * pMusicProperty);
	//**************************************************************************//
	//																			//
	//	���������ļ�tag��Ϣ														//
	//	�ļ����ʼ��������Ȳ��ܱ�����											//
	//																			//
	//	����:																	//
	//		pszFilePath	:	�ļ�·��											//
	//		pMusicTag	:	�����MusicTag��Ϣ									//
	//																			//
	//	����ֵ:																	//
	//			true	:	����ɹ�											//
	//			false	:	����ʧ��											//
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