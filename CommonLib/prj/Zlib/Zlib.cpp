// Zlib.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "../../include/YL_ZlibZip.h"
#include "zlib.h"



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

YL_CZlibZip::ZIP_RESULT YL_CZlibZip::Compress(unsigned char* ucDes, long* lDesLen, const unsigned char* ucSrc, long lSrcLen)
{
	int iRet =  compress( ucDes,(unsigned long*) lDesLen,ucSrc,lSrcLen );
	switch( iRet )
	{
	case Z_OK:
		return YL_CZlibZip::RESULT_SUC;
	case Z_MEM_ERROR:
		return YL_CZlibZip::ERROR_MEM;
	case Z_BUF_ERROR:
		return YL_CZlibZip::ERROR_BUFFER;
	case Z_DATA_ERROR:
		return YL_CZlibZip::ERROR_DATA;
	default:
		return YL_CZlibZip::ERROR_OTHER;
	}
}

YL_CZlibZip::ZIP_RESULT YL_CZlibZip::UnCompress( unsigned char* ucDes, long* lDesLen, const unsigned char* ucSrc, long lSrcLen )
{
	try
	{
		int iRet = uncompress( ucDes,(unsigned long*) lDesLen,ucSrc,lSrcLen);
		switch( iRet )
		{
		case Z_OK:
			return YL_CZlibZip::RESULT_SUC;
		case Z_MEM_ERROR:
			return YL_CZlibZip::ERROR_MEM;
		case Z_BUF_ERROR:
			return YL_CZlibZip::ERROR_BUFFER;
		case Z_DATA_ERROR:
			return YL_CZlibZip::ERROR_DATA;
		default:
			return YL_CZlibZip::ERROR_OTHER;
		}
	}
	catch(...)
	{
		return YL_CZlibZip::ERROR_OTHER;
	}
}
