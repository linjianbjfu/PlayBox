#ifndef BASE64_H_2006_04_12
#define BASE64_H_2006_04_12

/* Encode the string STR of length LENGTH to base64 format and place it
   to B64STORE.  The output will be \0-terminated, and must point to a
   writable buffer of at least 1+BASE64_LENGTH(length) bytes.  It
   returns the length of the resulting base64 data, not counting the
   terminating zero.

   This implementation will not emit newlines after 76 characters of
   base64 data.  */

/**
 * @param    b64store   加密后的结果buffer.
 * @param    str        待加密的字符串.
 * @param    length     str中要加密的字节长度.
 *
 * @return   返回b64store的长度.
 */
int YL_Base64Encode(char *b64store, const char *str, int length);
int YL_Base64Encode(char *b64store, const char *str, int length, const char *xor_string);

/* Decode data from BASE64 (assumed to be encoded as base64) into
   memory pointed to by TO.  TO should be large enough to accomodate
   the decoded data, which is guaranteed to be less than
   strlen(base64).

   Since TO is assumed to contain binary data, it is not
   NUL-terminated.  The function returns the length of the data
   written to TO.  -1 is returned in case of error caused by malformed
   base64 input.  */
/**
 * @param    to        解密后的串.
 * @param    base64    加密后的串.
 *
 * @return   解密后的数据的长度.
 */
int YL_Base64Decode(char *to, const char *base64);
int YL_Base64Decode(char *to, const char *base64, const char *xor_string);

/* How many bytes it will take to store LEN bytes in base64.  */
#define BASE64_LENGTH(len) (4 * (((len) + 2) / 3))

#endif //BASE64_H_ZMH_2006_04_12
