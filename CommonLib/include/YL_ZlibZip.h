#ifndef _YL_ZLIB_H
#define _YL_ZLIB_H

#ifdef	YL_ZLIB_EXPORT
#define YL_ZLIB_DLL __declspec(dllexport)
#else
#define YL_ZLIB_DLL __declspec(dllimport)
#endif


#define ZLIB_MAX_BUFFER(sourceLen) (sourceLen) + ((sourceLen) >> 12) + ((sourceLen) >> 14) + 11;

class YL_ZLIB_DLL YL_CZlibZip
{
public:
	enum ZIP_RESULT
	{
		ERROR_MEM = 0,		//�ڴ治��
		ERROR_DATA,			//���ݴ���
		ERROR_BUFFER,		//Buffer����
		ERROR_OTHER,		//��������
		RESULT_SUC			//�����ɹ�
	};

	//**************************************************************************//
	//	ѹ��һ���ڴ�															//
	//																			//
	// ������																	//
	//		ucDes		: ѹ��֮����ڴ�										//
	//		lDesLen		: ѹ��֮�����ݵĳ���									//
	//		ucSrc		: ѹ��ǰ���ڴ�											//
	//		lSrcLen		: ѹ��ǰ�ڴ����ݵĳ���									//
	//																			//
	// ����ֵ��																	//
	//																			//
	//**************************************************************************//
	static ZIP_RESULT Compress (unsigned char* ucDes, long* lDesLen, const unsigned char* ucSrc, long lSrcLen);
		

	//**************************************************************************//
	//	��ѹ��һ���ڴ�															//
	//																			//
	// ������																	//
	//		ucDes		: ��ѹ��֮����ڴ�										//
	//		lDesLen		: ��ѹ��֮�����ݵĳ���									//
	//		ucSrc		: ��ѹ��ǰ���ڴ�										//
	//		lSrcLen		: ��ѹ��ǰ�ڴ����ݵĳ���								//
	//																			//
	//																			//
	//**************************************************************************//
	static ZIP_RESULT UnCompress( unsigned char* ucDes, long* lDesLen, const unsigned char* ucSrc, long lSrcLen );
};
#endif
