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
		ERROR_MEM = 0,		//内存不够
		ERROR_DATA,			//数据错误
		ERROR_BUFFER,		//Buffer不够
		ERROR_OTHER,		//其他错误
		RESULT_SUC			//操作成功
	};

	//**************************************************************************//
	//	压缩一段内存															//
	//																			//
	// 参数：																	//
	//		ucDes		: 压缩之后的内存										//
	//		lDesLen		: 压缩之后内容的长度									//
	//		ucSrc		: 压缩前的内存											//
	//		lSrcLen		: 压缩前内存内容的长度									//
	//																			//
	// 返回值：																	//
	//																			//
	//**************************************************************************//
	static ZIP_RESULT Compress (unsigned char* ucDes, long* lDesLen, const unsigned char* ucSrc, long lSrcLen);
		

	//**************************************************************************//
	//	解压缩一段内存															//
	//																			//
	// 参数：																	//
	//		ucDes		: 解压缩之后的内存										//
	//		lDesLen		: 解压缩之后内容的长度									//
	//		ucSrc		: 解压缩前的内存										//
	//		lSrcLen		: 解压缩前内存内容的长度								//
	//																			//
	//																			//
	//**************************************************************************//
	static ZIP_RESULT UnCompress( unsigned char* ucDes, long* lDesLen, const unsigned char* ucSrc, long lSrcLen );
};
#endif
