#include "YL_StringUtil.h"
#include "stdio.h"
#include "stdarg.h"

void YL_StringUtil::Trim( string& str )
{
	size_t begin = 0;
	for (; begin < str.length(); begin++)
	{
		if (strncmp(str.c_str()+begin, "¡¡", 2)==0)
		{
			begin++;
			continue;
		}
		if( str[begin] != ' ' )
			break;
	}

	if (begin == str.length())
	{
		str.clear();
		return ;
	}

	size_t end = str.length()-1;
	for (; end > begin; end--)
	{
		if( str[end] != ' ')		
			break;
	}

	str = str.substr(begin, end-begin+1);

	return ;
}

bool isChineseChar(char c1, char c2)
{
	return (unsigned char)c1>=0x80&&(unsigned char)c1<=0xFE&&(unsigned char)
		c2>=0x40&&(unsigned char)c2<=0xFE;
}

void YL_StringUtil::Str2Lower( string& str )
{
	const char* pChar = str.c_str();
	string _lower;
	while (*pChar != '\0')
	{
		if (isChineseChar(*pChar, *(pChar+1)))
		{
			_lower.append(pChar, 2);
			pChar += 2;
		}
		else
		{
			_lower.push_back(tolower(*pChar));
			pChar++;
		}
	}
	str = _lower;

	return ;
}

bool YL_StringUtil::Format(string& str, const char* szFmt, ...)
{
	va_list argList;
	int len;
	char* buf;

	va_start( argList, szFmt );
	len = _vscprintf(szFmt, argList) + 1;

	buf = new char[len];

	if(vsprintf(buf, szFmt, argList) < 0)
	{
		return false;
	}

	str = string(buf, len - 1);
	delete[] buf;
	va_end( argList );
	return true;
}

void YL_StringUtil::Tokenize(const string& str, vector<string>& tokens, const string& delims)
{
	//ensure the tokens vector is empty.
	tokens.clear();
	string::size_type last_pos,pos;	
	pos = last_pos = 0;	

	do{
		last_pos = pos;
		last_pos = str.find_first_not_of(delims,last_pos);
		if(last_pos == string::npos){
			////WriteLog(Warnning, "YLString", "No token delims");
			break;
		}
		pos = str.find_first_of(delims, last_pos);

		string token = str.substr(last_pos, pos-last_pos);
		tokens.push_back(token);
	}while(pos != string::npos);
}

bool YL_StringUtil::EndsWith(const string& str, const string& endStr)
{
	string::size_type pos = str.find(endStr,str.length() - endStr.length());
	return pos != string::npos;
}

bool YL_StringUtil::StartsWith(const string& str, const string& startStr)
{
	string::size_type pos = str.find(startStr);
	return pos == 0;
}

void YL_StringUtil::ReplaceAll(string& str, const string& strToReplaced, const string& strReplaced)
{
	string strTemp;
	string::size_type pos, tpos;
	tpos = 0;
	pos = str.find(strToReplaced);
	while(pos != string::npos)
	{
		strTemp += str.substr(tpos, pos - tpos) + strReplaced;		
		tpos = pos + strToReplaced.length();
		pos = str.find(strToReplaced, tpos);
	}
	strTemp += str.substr(tpos);
	str = strTemp;
}

string YL_StringUtil::ComposeDirPath( const string& strFront, const string& strBehind )
{
	string strSrc1 = strBehind;
	while( strSrc1.find("\\") == 0 )
	{
		strSrc1 = strSrc1.substr( 1 );
	}

	string strSrc2 = strFront;
	while( strSrc2.find("\\",strSrc2.length() - 1) != string::npos )
	{
		strSrc2 = strSrc2.substr( 0, strSrc2.length()-1 );
	}

	string strDes = strSrc2 + "\\" + strSrc1;
	return strDes;
}