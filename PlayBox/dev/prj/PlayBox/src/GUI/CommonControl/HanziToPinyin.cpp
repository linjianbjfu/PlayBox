
#include  "HanziToPinyin.h"
#include <string.h> 
#include   <Afx.h>
#ifndef MAX_PATH
#define MAX_PATH          260
#endif
/*=============================================================================   
zhoupeng 02-9-2009
忽略大小写字符串查找， str1为源串， str2为匹配串小写， 返回匹配首地址
==============================================================================*/ 
int FindString(const char* str1, const char* str2, bool& end)
{
	const char* pstr0 = str1 ;
	const char* pstr1 = str1 ;
	const char* pstr2 = str2;
	while( 	*str1 != '\0' )
	{
		pstr1 = str1;
		while( *str2 != '\0')
		{
			if( (*str1>0X40&&*str1<0X5B) ? (*str1+0X20) != *str2 :*str1 != *str2 )
			{
				str2 = pstr2;
				break;
			}
			else
			{
				str2++;
				str1++;
				if( *str2 == '\0' ) 
				{
					if( *str1 == '\0' ) end = true;
					return (int)( str1-pstr0-(int)strlen(pstr2));
				}

			} 
		}
		str1 = pstr1+1;
	}
	return -1;
};
//
///*=============================================================================   
//zhoupeng 02-17-2009
//中文拼音匹配， 返回匹配首地址
//==============================================================================*/ 
//
int FindPinyin(const char* str1, const char* str2, bool& end)
{
	const char* pstr0 = str1 ;
	const char* pstr1 = str1 ;
	const char* pstr2 = str2;
	while( 	*str1 != '\0' )
	{
		pstr1 = str1;
		while( *str2 != '\0')
		{
			if( (*str1>0X40&&*str1<0X5B) ? (*str1+0X20) != *str2 :*str1 != *str2 )
			{
				str2 = pstr2;
				break;
			}
			else
			{
				str2++;
				str1++;
				if( *str2 == '\0' ) 
				{
					int count = (int)( str1-pstr0-(int)strlen(pstr2)) -1;
					if( (int)( str1-pstr0-(int)strlen(pstr2)) -1 )
					{
						*(pstr0+count) == ' ';
						if( *str1 == '\0' ) end = true;
						return (count+1);
					}
					else
					{
						return -1;
					}
				}

			} 
		}
		str1 = pstr1+1;
	}
	return -1;
};
//
#include "KwcImg.h"
using   namespace   std;
#include <hash_map>
///*=============================================================================   
//zhoupeng 02-18-2009
//中文拼音匹配
//==============================================================================*/ 
//#ifndef _ChToPinyin
//#define _ChToPinyin
int ChToPinyin(__int16& nChChar,  char* cOutPinyin  )
{
	static bool bIsInit = false;
	static hash_map<__int16,string,hash_compare<__int16,std::less<__int16> >,std::allocator<__int16> > pinyin_map;
	hash_map<__int16,string>::iterator itor;

	if( !bIsInit )
	{

		char homepath[MAX_PATH];
		::ZeroMemory(homepath,MAX_PATH); 
		CKwcImg::GetHomePath(homepath, MAX_PATH);
		strcat(homepath,"\\");
		strcat(homepath,"hanzi_pinyin.dict");
	
		CFile file;
		if( !file.Open( homepath,CFile::modeRead ))  
		{
			//::AfxMessageBox("无法查找 hanzi_pinyin.dict !");
			return -1;
		}
		//if( !file.Open( homepath,CFile::modeRead ))  ::AfxMessageBox("无法查找 hanzi_pinyin.dict !");
		__int64 nLenght = file.GetLength();

		char* pcFile = new char[nLenght+1];
		pcFile[nLenght] = '\0';
		file.Read(pcFile,nLenght);
		file.Close();

		char cCh[3]; 
		__int16 nCh;
		char ctempinyin[8];

		int test = ::GetTickCount();
		cCh[2] = '\0';
		ctempinyin[7] = '\0';
		while( *pcFile !='\0')
		{
			if( *pcFile & 0x80 )
			{
				
				::memcpy(cCh,pcFile,2);
				nCh = (__int16&)(*cCh);
				pcFile+=3;

				char* nFirst = pcFile;
				// 读入拼音
				for(;;)
				{
				   if( (*pcFile>0X60&&*pcFile<0X7B) )
				   {
					   pcFile++;
				   }
				   else
				   {
					   ::memcpy(ctempinyin,nFirst, pcFile-nFirst);
					   ctempinyin[pcFile-nFirst] = '\0';
					   break;
				   }
				}
				//建立索引
				pinyin_map[nCh] = ctempinyin;
				// 到下一行
				while( *pcFile !='\n') pcFile++;
			}
			pcFile++;
		}
		
		test = ::GetTickCount()-test;

		bIsInit = true;
		
	}

	// 匹配单汉字
	itor = pinyin_map.find(nChChar);
	if( itor != pinyin_map.end() )
	{
		
		strcpy( cOutPinyin, (*itor).second.c_str() );
		return (*itor).second.length();
	}
	return -1;
};
//
///*=============================================================================   
//zhoupeng 02-18-2009
//中文串匹配成拼音串
//==============================================================================*/ 
//
const char*  ChStringToPinyin(char   *pinyin,   const char *ch, int nMaxLenght)   
{
	char caCh[3];
	char ctempinyin[8];
	__int16 nCh;

	char* tempPinyin = pinyin;
	caCh[2] = '\0';
	ctempinyin[7]= '\0';
	while( *ch != '\0')
	{
		if( *ch & 0x80 )
		{
			::memcpy(caCh,ch,2);
			nCh = (__int16&)(*caCh);
			ch+=2;
			
			int nLenght = ChToPinyin( nCh, ctempinyin );
			if( nLenght>0 )
			{
				if( pinyin+nLenght - tempPinyin>nMaxLenght-1 ) return tempPinyin;
				::strcpy( pinyin, ctempinyin);
				pinyin += nLenght; 
			}
		}
		else
		{
			ch++;
		}
		
	}
	 return tempPinyin;
	//ChToPinyin();
};
//
//
const char* GetFirstLetter(char   *pinyin,   const char *ch, int *nPos,int nMaxLenght)
{
	char caCh[3];
	char ctempinyin[8];
	__int16 nCh;

	static int nLastLenght;
	nLastLenght = 0;
	int* nTempPos = nPos;

	char* tempPinyin = pinyin;
	caCh[2] = '\0';
	ctempinyin[7]= '\0';
	while( *ch != '\0')
	{
		if( *ch & 0x80 )
		{
			::memcpy(caCh,ch,2);
			nCh = (__int16&)(*caCh);
			ch+=2;
			
			
			int nLenght = ChToPinyin( nCh, ctempinyin );
			if( nLenght>0 )
			{
				if( ::strlen(tempPinyin)+1> nMaxLenght-1 ) 
				{
					*nTempPos =0;
					return tempPinyin;
				}
				ctempinyin[1]='\0';
				::strcpy( pinyin, ctempinyin);
				pinyin ++;
				
				if( nLastLenght != 0 )
				{
					*nTempPos = *(nTempPos-1) + nLastLenght;
				}
				else
				{
					*nTempPos= nLastLenght;
				}
				nLastLenght = nLenght;
				nTempPos++;
			}
		}
		else
		{
			ch++;
		}
		
	}
	*nTempPos =0;
	 return tempPinyin;
};
//
///*----------------------------------------------------------------------
//将字符串格式化为拼音串（ 根据拼音规则 ）
//zhoupeng 2009.2.18
//
//判断字符是否声母（initial consonant）
//------------------------------------------------------------------------*/
//
bool IsICchar(const char* str,int nPos)  // 是否双声母
{
	static short charTable[123];
	static bool isInit = false;

	char ch = *(str+nPos);
	if( !isInit )
	{
		::ZeroMemory( charTable, 123);
		charTable['b'] = 1;
		charTable['p'] = 1;
		charTable['m'] = 1;
		charTable['f'] = 1;
		charTable['b'] = 1;
		charTable['t'] = 1;
		charTable['n'] = 1;
		charTable['l'] = 1;
		charTable['g'] = 1;
		charTable['k'] = 1;
		charTable['h'] = 1;
		charTable['j'] = 1;
		charTable['q'] = 1;
		charTable['x'] = 1;
		charTable['r'] = 1;
		charTable['z'] = 2;
		charTable['c'] = 3;
		charTable['s'] = 4;
		charTable['y'] = 1;
		charTable['w'] = 1;

		charTable['B'] = 1;
		charTable['P'] = 1;
		charTable['M'] = 1;
		charTable['F'] = 1;
		charTable['B'] = 1;
		charTable['T'] = 1;
		charTable['N'] = 1;
		charTable['L'] = 1;
		charTable['G'] = 1;
		charTable['K'] = 1;
		charTable['H'] = 1;
		charTable['J'] = 1;
		charTable['Q'] = 1;
		charTable['X'] = 1;
		charTable['R'] = 1;
		charTable['Z'] = 2;
		charTable['C'] = 3;
		charTable['S'] = 4;
		charTable['Y'] = 1;
		charTable['W'] = 1;

		isInit = true;
	}
	if( ch > 122 )      return false;
	if(charTable[ch]>0) return true;
	return false;
};
