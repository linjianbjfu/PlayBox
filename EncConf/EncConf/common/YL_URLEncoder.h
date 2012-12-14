/*
 * =====================================================================================
 * 
 *        Filename:  URLEncoder.h
 * 
 *     Description:  ΪURL����RFC�涨��ʽ�ı���.
 * 
 *         Version:  1.0
 *         Created:  2005��10��08�� 16ʱ58��59�� CST
 *        Revision:  none
 *        Compiler:  g++
 * 
 *          Author:  ������ ()
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
	 * @brief    ����RFC2396��url�ַ������б���.
	 *
	 * @param    url   string to be translated.
	 * return    the translated string.
	 */
	static string encode(const string &url);

	/**
	 * @brief    ͬ��.
	 *
	 * @param    to    �����Ľ��buffer.
	 * @param    from  Դurl.
	 * @return   ���ر������ַ���.
	 */
	static char *encode(char *tourl, const char *fromurl);

	/**
	 * @brief    ��query_string�е�value���б���.
	 *
	 * @param    query    Դqueryֵ.
	 * @return   ���ر����Ľ��.
	 */
	static string encodeQuery(const string &query);

	static char* decode(char *s);
};
#endif //URLENCODER_H_ZMH_2005_10_08
