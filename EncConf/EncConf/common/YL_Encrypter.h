#ifndef ENCRYPTER_H_060826
#define ENCRYPTER_H_060826

class YL_Encrypter
{
public:
	/* Encrypt 加密函数
		@param	memptr			[in/out]要加密的内存空间数据
				memlen			[in]数据长度
				algorithmType	[in]加密算法
	*/
	static void Encrypt(char* memptr, int memlen, int algorithmType=0);

	/* Decrypt 解密函数
		@param	memptr			[in/out]要解密的内存空间数据
				memlen			[in]数据长度
				algorithmType	[in]解密算法
	*/
	static void Decrypt(char* memptr, int memlen, int algorithmType=0);
};
#endif