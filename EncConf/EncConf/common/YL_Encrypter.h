#ifndef ENCRYPTER_H_060826
#define ENCRYPTER_H_060826

class YL_Encrypter
{
public:
	/* Encrypt ���ܺ���
		@param	memptr			[in/out]Ҫ���ܵ��ڴ�ռ�����
				memlen			[in]���ݳ���
				algorithmType	[in]�����㷨
	*/
	static void Encrypt(char* memptr, int memlen, int algorithmType=0);

	/* Decrypt ���ܺ���
		@param	memptr			[in/out]Ҫ���ܵ��ڴ�ռ�����
				memlen			[in]���ݳ���
				algorithmType	[in]�����㷨
	*/
	static void Decrypt(char* memptr, int memlen, int algorithmType=0);
};
#endif