/*
 * =====================================================================================
 * 
 *        Filename:  URLEncoder.h
 * 
 *     Description:  为URL进行RFC规定格式的编码.
 * 
 *         Version:  1.0
 *         Created:  2005年10月08日 16时58分59秒 CST
 *        Revision:  none
 *        Compiler:  g++
 * 
 *          Author:  张明辉 ()
 * 
 * =====================================================================================
 */


#ifndef URLENCODER_H_ZMH_2005_10_08
#define URLENCODER_H_ZMH_2005_10_08

#include <string>

using std::string;

class YL_URLEncoder
{
public :
	/**
	 * @brief    依照RFC2396对url字符串进行编码.
	 *
	 * @param    url   string to be translated.
	 * return    the translated string.
	 */
	static string encode(const string &url);

	/**
	 * @brief    同上.
	 *
	 * @param    to    编码后的结果buffer.
	 * @param    from  源url.
	 * @return   返回编码后的字符串.
	 */
	static char *encode(char *tourl, const char *fromurl);

	/**
	 * @brief    对query_string中的value进行编码.
	 *
	 * @param    query    源query值.
	 * @return   返回编码后的结果.
	 */
	static string encodeQuery(const string &query);

	static char* decode(char *s);
};
#endif //URLENCODER_H_ZMH_2005_10_08
