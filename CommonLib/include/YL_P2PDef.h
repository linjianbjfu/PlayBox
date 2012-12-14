#ifndef _P2P_DEF_H
#define _P2P_DEF_H

#pragma once 

enum EDownMode
{
	M_PLAY,			
	M_PREFETCH,		
	M_DOWNLOAD,		
	M_UNKNOWN		
};

enum EFileType
{
	T_UNKNOWN,
	T_RAW,
	T_MP3,
	T_OGG,
	T_OGM,
	T_MOV,
	T_MPG,
	T_NSV,
	T_WMA, 	//8
	T_WMV, 	//9
	T_FLV,
	T_SWF,
	T_PLS,
	T_ASX,
	T_RM,
	T_RMVB,
	T_EXE,
	T_ASF,
	T_RAR,
	T_ZIP, 	//19
	T_APE,
	T_PACK,
	T_MKV	
};


enum P2P_DOWN_FAILED_REASON
{
	P2P_FAILED_NOPEER = 0,
	P2P_FAILED_NOTCONNECT,
	P2P_FAILED_NOSIG,
	P2P_FAILED_TIMEOUT,
	P2P_FAILED_TOOMANY,
	P2P_FAILED_P2P_NOTUSE
};


#ifndef SIGN_DEF
#define SIGN_DEF


struct Sign
{
	unsigned long sign1;
	unsigned long  sign2;

	bool operator < (const Sign& sg)
	{
		if(sign1 < sg.sign1)
		{
			return true;
		}
		else if(sign1 == sg.sign1 && sign2 < sg.sign2)
		{
			return true;
		}
		else 
		{
			return false;
		}

		return false;
	}
	bool operator == (const Sign &sg)
	{
		return sign1 == sg.sign1 && sign2 == sg.sign2;
	}
	Sign & operator = (const Sign & sg)
	{
		sign1 = sg.sign1;
		sign2 = sg.sign2;
		return *this;
	}
};


#endif

#endif