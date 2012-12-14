/*
 * =====================================================================================
 * 
 *        Filename:  URLEncoder.cpp
 * 
 *     Description:  
 * 
 *         Version:  1.0
 *         Created:  2005年10月10日 12时52分04秒 CST
 *        Revision:  none
 *        Compiler:  g++
 * 
 *          Author:  张明辉 ()
 * 
 * =====================================================================================
 */

#include "YL_URLEncoder.h"

enum {
  /* rfc1738 reserved chars + "$" and ",".    RFC2396 add '+' '$' ','*/
  urlchr_reserved = 1,

  /* rfc1738 unsafe chars, plus non-printables.  */
  urlchr_unsafe   = 2
};

#define urlchr_test(c, mask) (urlchr_table[(unsigned char)(c)] & (mask))
#define URL_RESERVED_CHAR(c) urlchr_test(c, urlchr_reserved)
#define URL_UNSAFE_CHAR(c) urlchr_test(c, urlchr_unsafe)

#define XNUM_TO_DIGIT(x) ("0123456789ABCDEF"[x] + 0)
#define XNUM_TO_digit(x) ("0123456789abcdef"[x] + 0)

#define XDIGIT_TO_NUM(h) ((h) < 'A' ? (h) - '0' : toupper (h) - 'A' + 10)
#define X2DIGITS_TO_NUM(h1, h2) ((XDIGIT_TO_NUM (h1) << 4) + XDIGIT_TO_NUM (h2))

/* Shorthands for the table: */
#define R  urlchr_reserved
#define U  urlchr_unsafe
#define RU R|U

static const unsigned char urlchr_table[256] =
{
  U,  U,  U,  U,   U,  U,  U,  U,   /* NUL SOH STX ETX  EOT ENQ ACK BEL */
  U,  U,  U,  U,   U,  U,  U,  U,   /* BS  HT  LF  VT   FF  CR  SO  SI  */
  U,  U,  U,  U,   U,  U,  U,  U,   /* DLE DC1 DC2 DC3  DC4 NAK SYN ETB */
  U,  U,  U,  U,   U,  U,  U,  U,   /* CAN EM  SUB ESC  FS  GS  RS  US  */
  U,  0,  U, RU,   R,  U,  R,  0,   /* SP  !   "   #    $   %   &   '   */
  0,  0,  0,  R,   R,  0,  0,  R,   /* (   )   *   +    ,   -   .   /   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* 0   1   2   3    4   5   6   7   */
  0,  0, RU,  R,   U,  R,  U,  R,   /* 8   9   :   ;    <   =   >   ?   */
 RU,  0,  0,  0,   0,  0,  0,  0,   /* @   A   B   C    D   E   F   G   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* H   I   J   K    L   M   N   O   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* P   Q   R   S    T   U   V   W   */
  0,  0,  0, RU,   U, RU,  U,  0,   /* X   Y   Z   [    \   ]   ^   _   */
  U,  0,  0,  0,   0,  0,  0,  0,   /* `   a   b   c    d   e   f   g   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* h   i   j   k    l   m   n   o   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* p   q   r   s    t   u   v   w   */
  0,  0,  0,  U,   U,  U,  0,  U,   /* x   y   z   {    |   }   ~   DEL */

  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,

  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
};
#undef R
#undef U
#undef RU

/* 在query_string的value中只要不是安全的字符就需要编码, %XX除外.*/
static inline bool query_char_needs_escaping(const char *p)
{
	if (*p == '%')
	{
		if (isxdigit(*(p + 1)) && isxdigit(*(p + 2)))
			return false;
		else
			/* Garbled %.. sequence: encode `%'. */
			return true;
	}
	else if (URL_UNSAFE_CHAR (*p) || URL_RESERVED_CHAR (*p))
		return true;
	else
		return false;

}

/* 在URL中 不安全的字符如果不是保留字就要编码 %XX中的%除外 */
static inline bool url_char_needs_escaping(const char *p)
{
	if (*p == '%')
	{
		if (isxdigit(*(p + 1)) && isxdigit(*(p + 2)))
			return false;
		else
			/* Garbled %.. sequence: encode `%'. */
			return true;
	}
	else if (URL_UNSAFE_CHAR (*p) && !URL_RESERVED_CHAR (*p))
		return true;
	else
		return false;
}


string YL_URLEncoder::encode(const string &url)
{
	string __encoded;
	const char *__p = url.c_str();

	while (*__p) {
		if (url_char_needs_escaping(__p)) {
			unsigned char c = *__p++;
			__encoded.push_back('%');
			__encoded.push_back(XNUM_TO_DIGIT (c >> 4));
			__encoded.push_back(XNUM_TO_DIGIT (c & 0xf));
		} else {
			__encoded.push_back(*__p++);
		}
	}

	return __encoded;
}

char *YL_URLEncoder::encode(char *tourl, const char *fromurl)
{
	const char *__p = fromurl;
	char *__p1 = tourl;

	while (*__p) {
		if (url_char_needs_escaping(__p)) {
			unsigned char c = *__p++;
			*__p1++ = ('%');
			*__p1++ = XNUM_TO_DIGIT (c >> 4);
			*__p1++ = XNUM_TO_DIGIT (c & 0xf);
		} else {
			*__p1++ = *__p++;
		}
	}

	*__p1 = '\0';

	return tourl;
}

string YL_URLEncoder::encodeQuery(const string &query)
{
	string __encoded;
	const char *__p = query.c_str();

	while (*__p) {
		if (query_char_needs_escaping(__p)) {
			unsigned char c = *__p++;
			__encoded.push_back('%');
			__encoded.push_back(XNUM_TO_DIGIT (c >> 4));
			__encoded.push_back(XNUM_TO_DIGIT (c & 0xf));
		} else {
			__encoded.push_back(*__p++);
		}
	}

	return __encoded;
}

char* YL_URLEncoder::decode(char *s)
{
	char *t = s;                  /* t - tortoise */
	char *h = s;                  /* h - hare     */

	for (; *h; h++, t++)
	{
		if (*h != '%')
		{
copychar:
			*t = (*h=='+')?' ':*h;
		}
		else
		{
			char c;
			/* Do nothing if '%' is not followed by two hex digits. */
			if (!h[1] || !h[2] || !(isxdigit (h[1]) && isxdigit (h[2])))
				goto copychar;
			c = X2DIGITS_TO_NUM (h[1], h[2]);
			/* Don't unescape %00 because there is no way to insert it
			*              into a C string without effectively truncating it. */
			if (c == '\0')
				goto copychar;
			*t = c;
			h += 2;
		}
	}
	*t = '\0';

	return s;
}
