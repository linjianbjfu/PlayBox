#include "YL_Base64.h"
#include <stdlib.h>
#include <string.h>

int
YL_Base64Encode (char *b64store, const char *str, int length)
{
  /* Conversion table.  */
  static char tbl[64] = {
    'A','B','C','D','E','F','G','H',
    'I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X',
    'Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n',
    'o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3',
    '4','5','6','7','8','9','+','/'
  };
  int i;
  const unsigned char *s = (const unsigned char *) str;
  char *p = b64store;

  ///* Transform the 3x8 bits to 4x6 bits, as required by base64.  */
  //for (i = 0; i < length; i += 3)
  //  {
  //    *p++ = tbl[s[0] >> 2];
  //    *p++ = tbl[((s[0] & 3) << 4) + (s[1] >> 4)];
  //    *p++ = tbl[((s[1] & 0xf) << 2) + (s[2] >> 6)];
  //    *p++ = tbl[s[2] & 0x3f];
  //    s += 3;
  //  }

  ///* Pad the result if necessary...  */
  //if (i == length + 1)
  //  *(p - 1) = '=';
  //else if (i == length + 2)
  //  *(p - 1) = *(p - 2) = '=';

  int to = length - length % 3;
  /* Transform the 3x8 bits to 4x6 bits, as required by base64.  */
  for (i = 0; i < to; i += 3)
  {
	  *p++ = tbl[s[0] >> 2];
	  *p++ = tbl[((s[0] & 3) << 4) + (s[1] >> 4)];
	  *p++ = tbl[((s[1] & 0xf) << 2) + (s[2] >> 6)];
	  *p++ = tbl[s[2] & 0x3f];
	  s += 3;
  }

  if (length - to == 1)
  { //! one char.
	  *p++ = tbl[s[0] >> 2];
	  *p++ = tbl[((s[0] & 3) << 4)];
	  *p++ = '=';
	  *p++ = '=';
  }
  else if (length - to == 2)
  {
	  *p++ = tbl[s[0] >> 2];
	  *p++ = tbl[((s[0] & 3) << 4) + (s[1] >> 4)];
	  *p++ = tbl[((s[1] & 0xf) << 2)];
	  *p++ = '=';
  }

  /* ...and zero-terminate it.  */
  *p = '\0';

  return (int)(p - b64store);
}


#define IS_ASCII(c) (((c) & 0x80) == 0)
#define IS_BASE64(c) ((IS_ASCII (c) && base64_char_to_value[c] >= 0) || c == '=')

/* Get next character from the string, except that non-base64
   characters are ignored, as mandated by rfc2045.  */
#define NEXT_BASE64_CHAR(c, p) do {			\
  c = *p++;						\
} while (c != '\0' && !IS_BASE64 (c))

int
YL_Base64Decode(char *to, const char *base64)
{
  /* Table of base64 values for first 128 characters.  Note that this
     assumes ASCII (but so does Wget in other places).  */
  static short base64_char_to_value[128] =
    {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,	/*   0-  9 */
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,	/*  10- 19 */
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,	/*  20- 29 */
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,	/*  30- 39 */
      -1,  -1,  -1,  62,  -1,  -1,  -1,  63,  52,  53,	/*  40- 49 */
      54,  55,  56,  57,  58,  59,  60,  61,  -1,  -1,	/*  50- 59 */
      -1,  -1,  -1,  -1,  -1,  0,   1,   2,   3,   4,	/*  60- 69 */
      5,   6,   7,   8,   9,   10,  11,  12,  13,  14,	/*  70- 79 */
      15,  16,  17,  18,  19,  20,  21,  22,  23,  24,	/*  80- 89 */
      25,  -1,  -1,  -1,  -1,  -1,  -1,  26,  27,  28,	/*  90- 99 */
      29,  30,  31,  32,  33,  34,  35,  36,  37,  38,	/* 100-109 */
      39,  40,  41,  42,  43,  44,  45,  46,  47,  48,	/* 110-119 */
      49,  50,  51,  -1,  -1,  -1,  -1,  -1		/* 120-127 */
    };

  const char *p = base64;
  char *q = to;

  while (1)
    {
      unsigned char c;
      unsigned long value;

      /* Process first byte of a quadruplet.  */
      NEXT_BASE64_CHAR (c, p);
      if (!c)
	break;
      if (c == '=')
	return -1;		/* illegal '=' while decoding base64 */
      value = base64_char_to_value[c] << 18;

      /* Process scond byte of a quadruplet.  */
      NEXT_BASE64_CHAR (c, p);
      if (!c)
	return -1;		/* premature EOF while decoding base64 */
      if (c == '=')
	return -1;		/* illegal `=' while decoding base64 */
      value |= base64_char_to_value[c] << 12;
      *q++ = (char)(value >> 16);

      /* Process third byte of a quadruplet.  */
      NEXT_BASE64_CHAR (c, p);
      if (!c)
	return -1;		/* premature EOF while decoding base64 */

      if (c == '=')
	{
	  NEXT_BASE64_CHAR (c, p);
	  if (!c)
	    return -1;		/* premature EOF while decoding base64 */
	  if (c != '=')
	    return -1;		/* padding `=' expected but not found */
	  continue;
	}

      value |= base64_char_to_value[c] << 6;
      *q++ = (char)(0xff & value >> 8);

      /* Process fourth byte of a quadruplet.  */
      NEXT_BASE64_CHAR (c, p);
      if (!c)
	return -1;		/* premature EOF while decoding base64 */
      if (c == '=')
	continue;

      value |= base64_char_to_value[c];
      *q++ = (char)(0xff & value);
    }

  return (int)(q - to);
}

int YL_Base64Encode(char *b64store, const char *str, int length, const char *xor_string)
{
	char *__tmp = new char[length];
	memcpy(__tmp, str, length);

	int __len = (int)strlen(xor_string);

	for (int i = 0; i < length; )
	{
		for (int j = 0; j < __len && i < length; j++)
		{
			__tmp[i++] ^= xor_string[j];
		}
	}

	int ret = YL_Base64Encode(b64store, __tmp, length);
	delete []__tmp;

	return ret;
}

int YL_Base64Decode(char *to, const char *base64, const char *xor_string)
{
	int ret = YL_Base64Decode(to, base64);
	if (ret == -1)
		return -1;

	int __len = (int)strlen(xor_string);

	for (int i = 0; i < ret; )
	{
		for (int j = 0; j < __len && i < ret; j++)
		{
			to[i++] ^= xor_string[j];
		}
	}

	return ret;
}
